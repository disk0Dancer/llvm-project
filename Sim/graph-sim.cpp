// graph-sim: ISA simulator for the custom "Graph" LLVM target.
//
// Loads an ELF32-LE relocatable object produced by `llc -filetype=obj` for
// triple `graph`, applies R_GRAPH_16 / R_GRAPH_PC16 relocations against an
// internal address space (text at 0, bss after text), locates symbol `main`,
// and interprets the resulting instruction stream. Graphics intrinsics drive
// an in-memory framebuffer dumped as a PPM (P6) image on GFX_FLUSH.
//
// Instruction word layout (little-endian 32-bit):
//   [31:24] opcode | [23:20] r1 | [19:16] r2 | [15:0] r3_imm

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#ifdef GRAPH_SIM_SDL
#include <SDL.h>
#endif

namespace {

// --- Minimal ELF32 little-endian structs ----------------------------------
#pragma pack(push, 1)
struct Elf32_Ehdr {
  uint8_t  e_ident[16];
  uint16_t e_type, e_machine;
  uint32_t e_version, e_entry, e_phoff, e_shoff, e_flags;
  uint16_t e_ehsize, e_phentsize, e_phnum, e_shentsize, e_shnum, e_shstrndx;
};
struct Elf32_Shdr {
  uint32_t sh_name, sh_type, sh_flags, sh_addr, sh_offset, sh_size,
           sh_link, sh_info, sh_addralign, sh_entsize;
};
struct Elf32_Sym {
  uint32_t st_name, st_value, st_size;
  uint8_t  st_info, st_other;
  uint16_t st_shndx;
};
struct Elf32_Rela {
  uint32_t r_offset;
  uint32_t r_info;
  int32_t  r_addend;
};
#pragma pack(pop)

constexpr uint16_t EM_GRAPH = 259;
constexpr uint32_t SHT_SYMTAB = 2;
constexpr uint32_t SHT_RELA   = 4;
constexpr uint32_t SHT_NOBITS = 8;

constexpr uint32_t R_GRAPH_16   = 1;
constexpr uint32_t R_GRAPH_PC16 = 2;

constexpr uint32_t MEM_SIZE   = 0x400000;
constexpr uint32_t BSS_BASE   = 0x0000;
constexpr uint32_t TEXT_BASE  = 0x10000;
constexpr uint32_t STACK_TOP  = 0x3F0000;
constexpr uint32_t HALT_PC    = 0xDEAD0000u;

inline uint32_t elf32SymIndex(uint32_t info) { return info >> 8; }
inline uint8_t  elf32RelType (uint32_t info) { return info & 0xFF; }

std::vector<uint8_t> readBinary(const std::string &path) {
  std::ifstream f(path, std::ios::binary);
  if (!f) { std::cerr << "error: cannot open " << path << "\n"; std::exit(1); }
  return {std::istreambuf_iterator<char>(f), {}};
}

// --- Simulator state ------------------------------------------------------
struct Sim {
  uint32_t regs[16] = {0};
  std::vector<uint8_t> mem;         // flat 64 KB address space
  uint32_t entryPc = 0;
  uint32_t pc = 0;
  uint32_t color = 0x00FFFFFFu;
  int W = 0, H = 0;
  std::vector<uint32_t> fb;
  std::string outPath = "out.ppm";
  uint64_t insnCount = 0;
  bool trace = false;
  uint32_t textEnd = 0;             // first address past loaded text

  // Windowed mode.
  bool windowed = false;
  int  scale    = 16;               // logical pixel → window pixel
  int  frameDelayMs = 80;           // throttle GFX_FLUSH presents
  bool windowClosed = false;

#ifdef GRAPH_SIM_SDL
  SDL_Window   *win   = nullptr;
  SDL_Renderer *ren   = nullptr;
  SDL_Texture  *tex   = nullptr;
#endif

  void loadElf(const std::vector<uint8_t> &blob);
  uint32_t loadW(uint32_t addr) const;
  void     storeW(uint32_t addr, uint32_t val);
  void putPixel(int x, int y);
  void writePPM() const;
  void initWindow();
  void presentFrame();
  void pumpEvents();
  void waitClose();
  void teardownWindow();
  void step();
  void run(uint64_t maxInsns);
};

void Sim::loadElf(const std::vector<uint8_t> &blob) {
  if (blob.size() < sizeof(Elf32_Ehdr) ||
      blob[0] != 0x7F || blob[1] != 'E' || blob[2] != 'L' || blob[3] != 'F') {
    std::cerr << "error: not an ELF file\n"; std::exit(1);
  }
  auto eh = reinterpret_cast<const Elf32_Ehdr *>(blob.data());
  if (eh->e_ident[4] != 1 || eh->e_ident[5] != 1) {
    std::cerr << "error: expected ELF32 little-endian\n"; std::exit(1);
  }
  if (eh->e_machine != EM_GRAPH)
    std::cerr << "warning: e_machine=" << eh->e_machine
              << " (expected " << EM_GRAPH << ")\n";

  auto shtab = reinterpret_cast<const Elf32_Shdr *>(blob.data() + eh->e_shoff);
  auto shstr = reinterpret_cast<const char *>(blob.data() +
                                              shtab[eh->e_shstrndx].sh_offset);

  // First pass: locate .text, .bss, .symtab/.strtab, list of .rela* sections.
  uint16_t textIdx = 0, bssIdx = 0;
  const Elf32_Shdr *textSh = nullptr, *bssSh = nullptr;
  const Elf32_Shdr *symSh  = nullptr, *strSh = nullptr;
  std::vector<const Elf32_Shdr *> relaShs;
  for (uint16_t i = 0; i < eh->e_shnum; ++i) {
    const char *name = shstr + shtab[i].sh_name;
    if (!std::strcmp(name, ".text")) { textSh = &shtab[i]; textIdx = i; }
    if (!std::strcmp(name, ".bss"))  { bssSh  = &shtab[i]; bssIdx  = i; }
    if (shtab[i].sh_type == SHT_SYMTAB)            symSh = &shtab[i];
    if (shtab[i].sh_type == SHT_RELA)              relaShs.push_back(&shtab[i]);
  }
  if (!textSh) { std::cerr << "error: .text not found\n"; std::exit(1); }
  if (symSh)   strSh = &shtab[symSh->sh_link];

  mem.assign(MEM_SIZE, 0);

  // Load .text at TEXT_BASE.
  if (textSh->sh_size + TEXT_BASE > MEM_SIZE) {
    std::cerr << "error: .text too large for layout\n"; std::exit(1);
  }
  std::memcpy(mem.data() + TEXT_BASE, blob.data() + textSh->sh_offset,
              textSh->sh_size);
  textEnd = TEXT_BASE + textSh->sh_size;

  // Reserve .bss room (already zero-filled).
  uint32_t bssSize = bssSh ? bssSh->sh_size : 0;
  if (BSS_BASE + bssSize > MEM_SIZE) {
    std::cerr << "error: .bss does not fit in simulator memory\n"; std::exit(1);
  }

  // Resolve symbol addresses in the simulator's address space.
  std::vector<uint32_t> symAddr;
  uint32_t mainOff = 0; bool foundMain = false;
  if (symSh && strSh) {
    uint32_t n = symSh->sh_size / sizeof(Elf32_Sym);
    auto syms = reinterpret_cast<const Elf32_Sym *>(blob.data() + symSh->sh_offset);
    auto strs = reinterpret_cast<const char *>(blob.data() + strSh->sh_offset);
    symAddr.resize(n);
    for (uint32_t i = 0; i < n; ++i) {
      const auto &s = syms[i];
      if (s.st_shndx == textIdx)      symAddr[i] = TEXT_BASE + s.st_value;
      else if (s.st_shndx == bssIdx)  symAddr[i] = BSS_BASE  + s.st_value;
      else                             symAddr[i] = s.st_value;
      const char *nm = strs + s.st_name;
      if (s.st_shndx == textIdx && !std::strcmp(nm, "main")) {
        mainOff = s.st_value; foundMain = true;
      }
    }

    // Apply relocations targeted at .text.
    for (const Elf32_Shdr *rs : relaShs) {
      if (rs->sh_info != textIdx) continue; // only relocations into .text
      uint32_t nrel = rs->sh_size / sizeof(Elf32_Rela);
      auto rels = reinterpret_cast<const Elf32_Rela *>(blob.data() + rs->sh_offset);
      for (uint32_t i = 0; i < nrel; ++i) {
        const auto &r = rels[i];
        uint32_t symIdx = elf32SymIndex(r.r_info);
        uint8_t  type   = elf32RelType (r.r_info);
        uint32_t S = symIdx < symAddr.size() ? symAddr[symIdx] : 0;
        uint32_t loc = TEXT_BASE + r.r_offset;
        if (loc + 2 > MEM_SIZE) continue;
        uint32_t val;
        switch (type) {
        case R_GRAPH_16:   val = S + static_cast<uint32_t>(r.r_addend); break;
        case R_GRAPH_PC16: val = (S + static_cast<uint32_t>(r.r_addend) - loc) / 4; break;
        default:
          std::cerr << "warning: unknown reloc type " << (int)type << "\n";
          continue;
        }
        // Patch low 16 bits at `loc` using OR (matches assembler behaviour).
        mem[loc + 0] |= static_cast<uint8_t>( val        & 0xFF);
        mem[loc + 1] |= static_cast<uint8_t>((val >> 8 ) & 0xFF);
      }
    }
  }

  if (!foundMain)
    std::cerr << "warning: symbol `main` not found, starting at .text+0\n";
  entryPc = TEXT_BASE + mainOff;
  pc = entryPc;

  // Codegen treats R0 as a zero base for symbol address synthesis
  // ("ORi r4 r0 sym"), so we must enter main with R0 = 0. BR Rn is then
  // detected as a function-return; for main that means halt.
  regs[0] = 0;
  // R1 is the stack pointer; initialise it so prologue spills don't clobber
  // .text/.bss. Frame layout grows downward (lower addresses).
  regs[1] = STACK_TOP;
}

uint32_t Sim::loadW(uint32_t addr) const {
  if (addr + 4 > mem.size() || (addr & 3)) {
    std::cerr << "error: LD bad addr 0x" << std::hex << addr << "\n"; std::exit(1);
  }
  return static_cast<uint32_t>(mem[addr]) |
        (static_cast<uint32_t>(mem[addr+1]) << 8 ) |
        (static_cast<uint32_t>(mem[addr+2]) << 16) |
        (static_cast<uint32_t>(mem[addr+3]) << 24);
}

void Sim::storeW(uint32_t addr, uint32_t val) {
  if (addr + 4 > mem.size() || (addr & 3)) {
    std::cerr << "error: ST bad addr 0x" << std::hex << addr << "\n"; std::exit(1);
  }
  mem[addr+0] =  val        & 0xFF;
  mem[addr+1] = (val >> 8 ) & 0xFF;
  mem[addr+2] = (val >> 16) & 0xFF;
  mem[addr+3] = (val >> 24) & 0xFF;
}

void Sim::putPixel(int x, int y) {
  if (W <= 0 || H <= 0) return;
  if (x < 0 || y < 0 || x >= W || y >= H) return;
  fb[y * W + x] = color & 0x00FFFFFFu;
}

void Sim::initWindow() {
#ifdef GRAPH_SIM_SDL
  if (!windowed || win) return;
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
    windowed = false; return;
  }
  win = SDL_CreateWindow("graph-sim",
                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         W * scale, H * scale, SDL_WINDOW_SHOWN);
  if (!win) { std::cerr << "SDL_CreateWindow: " << SDL_GetError() << "\n";
              windowed = false; return; }
  ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  if (!ren) ren = SDL_CreateRenderer(win, -1, 0);
  tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888,
                          SDL_TEXTUREACCESS_STREAMING, W, H);
#endif
}

void Sim::presentFrame() {
#ifdef GRAPH_SIM_SDL
  if (!windowed || !tex) return;
  // fb is 0x00RRGGBB; ARGB8888 expects A in MSB.
  std::vector<uint32_t> argb(fb.size());
  for (size_t i = 0; i < fb.size(); ++i)
    argb[i] = 0xFF000000u | (fb[i] & 0x00FFFFFFu);
  SDL_UpdateTexture(tex, nullptr, argb.data(), W * 4);
  SDL_RenderClear(ren);
  SDL_RenderCopy(ren, tex, nullptr, nullptr);
  SDL_RenderPresent(ren);
  pumpEvents();
  if (frameDelayMs > 0) SDL_Delay(frameDelayMs);
#endif
}

void Sim::pumpEvents() {
#ifdef GRAPH_SIM_SDL
  SDL_Event ev;
  while (SDL_PollEvent(&ev)) {
    if (ev.type == SDL_QUIT ||
        (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE))
      windowClosed = true;
  }
#endif
}

void Sim::waitClose() {
#ifdef GRAPH_SIM_SDL
  if (!windowed || !win || windowClosed) return;
  SDL_Event ev;
  while (!windowClosed && SDL_WaitEvent(&ev)) {
    if (ev.type == SDL_QUIT ||
        (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE))
      windowClosed = true;
  }
#endif
}

void Sim::teardownWindow() {
#ifdef GRAPH_SIM_SDL
  if (tex) SDL_DestroyTexture(tex);
  if (ren) SDL_DestroyRenderer(ren);
  if (win) SDL_DestroyWindow(win);
  if (windowed) SDL_Quit();
  tex = nullptr; ren = nullptr; win = nullptr;
#endif
}

void Sim::writePPM() const {
  std::ofstream f(outPath, std::ios::binary);
  if (!f) { std::cerr << "error: cannot write " << outPath << "\n"; return; }
  f << "P6\n" << W << " " << H << "\n255\n";
  std::vector<uint8_t> row(static_cast<size_t>(W) * 3);
  for (int y = 0; y < H; ++y) {
    for (int x = 0; x < W; ++x) {
      uint32_t c = fb[y * W + x];
      row[3 * x + 0] = (c >> 16) & 0xFF;
      row[3 * x + 1] = (c >>  8) & 0xFF;
      row[3 * x + 2] =  c        & 0xFF;
    }
    f.write(reinterpret_cast<const char *>(row.data()), row.size());
  }
  std::cerr << "graph-sim: wrote " << outPath << " (" << W << "x" << H << ")\n";
}

static inline int32_t sext16(uint16_t v) {
  return static_cast<int32_t>(static_cast<int16_t>(v));
}

void Sim::step() {
  uint32_t insn = loadW(pc);
  uint8_t  op   = (insn >> 24) & 0xFF;
  uint8_t  r1   = (insn >> 20) & 0x0F;
  uint8_t  r2   = (insn >> 16) & 0x0F;
  uint16_t imm  =  insn        & 0xFFFF;
  int32_t  simm = sext16(imm);
  uint32_t nextPc = pc + 4;

  if (trace) {
    std::fprintf(stderr, "[%06x] %08x op=%02x r1=%u r2=%u imm=%u  "
                         "R[r1]=%u R[r2]=%u\n",
                 pc, insn, op, r1, r2, imm, regs[r1], regs[r2]);
  }

  auto &R = regs;
  auto branchTarget = [&]() { return pc + simm * 4; };
  auto setIncCC = [&](bool cond) { R[r1] = cond ? 1u : 0u; R[r2] = R[r2] + 1; };

  switch (op) {
  // R-form arithmetic: r1 = r2 op R[imm & 0xF].
  case 0x30: R[r1] = R[r2] + R[imm & 0xF]; break;
  case 0x31: R[r1] = R[r2] - R[imm & 0xF]; break;
  case 0x32: R[r1] = R[r2] * R[imm & 0xF]; break;
  case 0x33: R[r1] = static_cast<uint32_t>(
               static_cast<int32_t>(R[r2]) /
               static_cast<int32_t>(R[imm & 0xF])); break;
  case 0x34: R[r1] = R[r2] | R[imm & 0xF]; break;
  case 0x35: R[r1] = R[r2] & R[imm & 0xF]; break;
  case 0x36: R[r1] = R[r2] ^ R[imm & 0xF]; break;
  case 0x37: R[r1] = static_cast<uint32_t>(
               static_cast<int32_t>(R[r2]) >> (R[imm & 0xF] & 31)); break;
  case 0x38: R[r1] = R[r2] >> (R[imm & 0xF] & 31); break;
  case 0x39: R[r1] = R[r2] << (R[imm & 0xF] & 31); break;
  case 0x3A: R[r1] = R[r2] % R[imm & 0xF]; break;

  // I-form arithmetic: r1 = r2 op sext16(imm).
  case 0x40: R[r1] = R[r2] + static_cast<uint32_t>(simm); break;
  case 0x41: R[r1] = R[r2] - static_cast<uint32_t>(simm); break;
  case 0x42: R[r1] = R[r2] * static_cast<uint32_t>(simm); break;
  case 0x43: R[r1] = static_cast<uint32_t>(static_cast<int32_t>(R[r2]) / simm); break;
  case 0x44: R[r1] = R[r2] | static_cast<uint32_t>(simm); break;
  case 0x45: R[r1] = R[r2] & static_cast<uint32_t>(simm); break;
  case 0x46: R[r1] = R[r2] ^ static_cast<uint32_t>(simm); break;
  case 0x47: R[r1] = static_cast<uint32_t>(
               static_cast<int32_t>(R[r2]) >> (simm & 31)); break;
  case 0x48: R[r1] = R[r2] >> (simm & 31); break;
  case 0x49: R[r1] = R[r2] << (simm & 31); break;
  case 0x4A: R[r1] = static_cast<uint32_t>(simm) ?
               R[r2] % static_cast<uint32_t>(simm) : 0; break;

  // Memory ops.
  case 0x50: R[r1] = loadW(R[r2] + static_cast<uint32_t>(simm)); break;
  case 0x51: storeW(R[r2] + static_cast<uint32_t>(simm), R[r1]);  break;

  // INC + compare (r1 := cmp result; r2 := r2 + 1).
  case 0x90: setIncCC(static_cast<int32_t>(R[r2]) == simm); break;
  case 0x91: setIncCC(static_cast<int32_t>(R[r2]) != simm); break;
  case 0x92: setIncCC(static_cast<int32_t>(R[r2]) <= simm); break;
  case 0x93: setIncCC(static_cast<int32_t>(R[r2]) <  simm); break;
  case 0x94: setIncCC(static_cast<int32_t>(R[r2]) >= simm); break;
  case 0x95: setIncCC(static_cast<int32_t>(R[r2]) >  simm); break;

  // Moves / misc.
  case 0xAA: R[r1] = static_cast<uint32_t>(imm); break;
  case 0xAF: R[r1] = static_cast<uint32_t>(imm) << 16; break;
  case 0xAB: { int32_t v = static_cast<int32_t>(R[r2]);
               R[r1] = static_cast<uint32_t>(v < 0 ? -v : v); break; }

  // Branches.
  case 0xB0: nextPc = branchTarget(); break;
  case 0xBB: // BR Rn — used only as PseudoRET; treat as exit.
    nextPc = HALT_PC; break;
  case 0xBC: if (R[r1] != 0) nextPc = branchTarget(); break;

  // Graphics intrinsics.
  case 0xC0: {
    W = static_cast<int>(R[r1]);
    H = static_cast<int>(R[r2]);
    if (W <= 0 || H <= 0 || W > 4096 || H > 4096) {
      std::cerr << "error: GFX_INIT bad size " << W << "x" << H << "\n";
      std::exit(1);
    }
    fb.assign(static_cast<size_t>(W) * H, 0);
    initWindow();
    break;
  }
  case 0xC1: color = R[r1] & 0x00FFFFFFu; break;
  case 0xC2: putPixel(static_cast<int>(R[r1]),
                      static_cast<int>(R[r2])); break;
  case 0xC3:
    if (windowed) presentFrame();
    else          writePPM();
    break;

  case 0x01: break; // nop padding

  default:
    std::cerr << "error: unknown opcode 0x" << std::hex << (int)op
              << " at PC=0x" << pc << std::dec << "\n";
    std::exit(1);
  }

  pc = nextPc;
  ++insnCount;
}

void Sim::run(uint64_t maxInsns) {
  while (pc != HALT_PC) {
    if (windowed && windowClosed) {
      std::cerr << "graph-sim: window closed, stopping at "
                << insnCount << " insns\n";
      return;
    }
    if (insnCount >= maxInsns) {
      std::cerr << "warning: insn limit (" << maxInsns
                << ") exceeded; dumping current framebuffer\n";
      if (W > 0 && H > 0) {
        if (windowed) presentFrame();
        else          writePPM();
      }
      return;
    }
    step();
  }
  std::cerr << "graph-sim: halted after " << insnCount << " insns\n";
}

} // namespace

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr <<
      "usage: graph-sim <obj.o> [-o out.ppm] [--trace] [--max N]\n";
    return 1;
  }
  Sim sim;
  std::string input = argv[1];
  uint64_t maxInsns = 10'000'000ull;
  for (int i = 2; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "-o" && i + 1 < argc)       sim.outPath = argv[++i];
    else if (a == "--trace")             sim.trace   = true;
    else if (a == "--max" && i + 1 < argc) maxInsns  = std::strtoull(argv[++i], nullptr, 0);
    else if (a == "--window")            sim.windowed = true;
    else if (a == "--scale" && i + 1 < argc) sim.scale = std::atoi(argv[++i]);
    else if (a == "--fps" && i + 1 < argc) {
      int fps = std::atoi(argv[++i]);
      sim.frameDelayMs = fps > 0 ? 1000 / fps : 0;
    }
    else { std::cerr << "unknown arg: " << a << "\n"; return 1; }
  }
#ifndef GRAPH_SIM_SDL
  if (sim.windowed) {
    std::cerr << "warning: built without SDL2; --window ignored\n";
    sim.windowed = false;
  }
#endif
  sim.loadElf(readBinary(input));
  sim.run(maxInsns);
  if (sim.windowed) {
    sim.waitClose();
    sim.teardownWindow();
  }
  return 0;
}

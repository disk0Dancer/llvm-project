#ifndef LLVM_LIB_TARGET_GRAPH_GRAPH_H
#define LLVM_LIB_TARGET_GRAPH_GRAPH_H

#include "llvm/Support/raw_ostream.h"

#define GRAPH_DUMP(Color)                                                      \
  {                                                                            \
    llvm::errs().changeColor(Color)                                            \
        << __func__ << "\n\t\t" << __FILE__ << ":" << __LINE__ << "\n";        \
    llvm::errs().changeColor(llvm::raw_ostream::WHITE);                        \
  }
// #define GRAPH_DUMP(Color) {}

#define GRAPH_DUMP_RED     GRAPH_DUMP(llvm::raw_ostream::RED)
#define GRAPH_DUMP_GREEN   GRAPH_DUMP(llvm::raw_ostream::GREEN)
#define GRAPH_DUMP_YELLOW  GRAPH_DUMP(llvm::raw_ostream::YELLOW)
#define GRAPH_DUMP_CYAN    GRAPH_DUMP(llvm::raw_ostream::CYAN)
#define GRAPH_DUMP_MAGENTA GRAPH_DUMP(llvm::raw_ostream::MAGENTA)

#endif // LLVM_LIB_TARGET_GRAPH_GRAPH_H

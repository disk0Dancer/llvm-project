#!/usr/bin/env bash
# Full Graph compilation+simulation pipeline:
#   FrontEnd (clang, optional) -> MiddleEnd (opt) -> BackEnd (llc) -> Simulator
#
# Usage:
#   ./compile_sim.sh <input.c|input.ll> [-o image.ppm]
#
# Environment overrides:
#   LLVM_BIN   path to LLVM build/bin (default: <repo>/build/bin)
#   CLANG      path to a clang that knows the `graph` triple
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

LLVM_BIN="${LLVM_BIN:-$REPO_DIR/build/bin}"
OPT="$LLVM_BIN/opt"
LLC="$LLVM_BIN/llc"
LLVM_AS="$LLVM_BIN/llvm-as"
SIM="$SCRIPT_DIR/graph-sim"
CLANG="${CLANG:-${LLVM_BIN}/clang}"

if [ ! -x "$SIM" ]; then
  echo "==> building graph-sim"
  make -C "$SCRIPT_DIR" >&2
fi
for tool in "$OPT" "$LLC"; do
  [ -x "$tool" ] || { echo "missing $tool" >&2; exit 1; }
done

INPUT="${1:-}"
OUT="out.ppm"
shift || true
while [ $# -gt 0 ]; do
  case "$1" in
    -o) OUT="$2"; shift 2;;
    *)  echo "unknown arg: $1" >&2; exit 1;;
  esac
done

[ -n "$INPUT" ] || { echo "usage: $0 <input.c|input.ll> [-o image.ppm]" >&2; exit 1; }

BASE="$(basename "$INPUT")"
STEM="${BASE%.*}"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

case "$INPUT" in
  *.c)
    [ -x "$CLANG" ] || { echo "no clang with graph target at $CLANG" >&2; exit 1; }
    echo "==> [FrontEnd] clang -target graph -emit-llvm -O0"
    "$CLANG" -target graph -O0 -S -emit-llvm "$INPUT" -o "$TMP/$STEM.ll"
    IR="$TMP/$STEM.ll"
    ;;
  *.ll)
    IR="$INPUT"
    ;;
  *)
    echo "unsupported input extension" >&2; exit 1;;
esac

echo "==> [MiddleEnd] opt ${OPT_LEVEL:--O1}"
# -O2 currently exposes a backend gap (no loadRegFromStackSlot), so default
# to -O1; override with `OPT_LEVEL=-O2` if your backend has caught up.
"$OPT" "${OPT_LEVEL:--O1}" -S "$IR" -o "$TMP/$STEM.opt.ll"

echo "==> [BackEnd] llc -filetype=obj"
"$LLC" -filetype=obj "$TMP/$STEM.opt.ll" -o "$TMP/$STEM.o"

echo "==> [Simulator] graph-sim -> $OUT"
"$SIM" "$TMP/$STEM.o" -o "$OUT"
echo "done: $OUT"

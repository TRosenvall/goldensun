# Batch 03 — 11 functions elevated to matching C

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01 and 02 are
already in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a **clean** build. Every address below was read back from the linked ELF
and the overlay ELFs, and every path confirmed to contain the function named.

**Five of these were parked in `src/non_matching/` upstream** and now match;
those files are deleted in this branch.

## The functions

| Function | Address | New source | Replaces |
|---|---|---|---|
| `InitSpriteLayer` | `0x0800b868` | `src/rom_9000/rom_b798_c_a_a_b.c` | **split** |
| `Camera_SetTarget` | `0x0800c4bc` | `src/rom_9000/rom_c004_c_a_a_c_a_c_c_c_c_c.c` | whole file |
| `Func_80925e0` | `0x080925e0` | `src/rom_8a000/rom_925e0_a_a_a_b.c` | **split** |
| `Func_80a23c0` | `0x080a23c0` | `src/rom_a1000/rom_a1814_c_a_a_c_a_c_b.c` | **split** |
| `OvlFunc_974_2008130` | `0x02008130` | `src/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_a_b.c` | **split** |
| `OvlFunc_974_2008148` | `0x02008148` | `src/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_a_c.c` | **split** |
| `OvlFunc_974_2008160` | `0x02008160` | `src/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_b.c` | **split** |
| `OvlFunc_974_2008180` | `0x02008180` | `src/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_c.c` | whole part |
| `OvlFunc_974_2008198` | `0x02008198` | (same file) | |
| `OvlFunc_974_20081b8` | `0x020081b8` | (same file) | |
| `OvlFunc_974_20081d8` | `0x020081d8` | (same file) | |

Sibling `.s` files produced by the splits must travel with them or the batch
will not link:

| Split | Siblings |
|---|---|
| `InitSpriteLayer` | `rom_b798_c_a_a_a.s`, `rom_b798_c_a_a_c.s` |
| `Func_80925e0` | `rom_925e0_a_a_a_a.s`, `rom_925e0_a_a_a_c.s` |
| `Func_80a23c0` | `rom_a1814_c_a_a_c_a_c_a.s`, `..._c.s` |
| `OvlFunc_974_*` | `ovl_30_a_c_a_c_c_a_a_a.s` |

## The technique that made most of this batch possible

**Stopping a constant fold with symbol addresses.**

The seven `OvlFunc_974` stubs each pass a message id and the *span* of a
message range. The ROM computes that span at runtime — two pool loads and a
subtraction — for a value that is constant:

    ldr r3, =0xc9b / ldr r1, =0xcc6 / sub r1, r3

Written as `0xcc6 - 0xc9b` in C, gcc folds it to `mov r1, #0x2b` and a pool
word disappears. Your parked note on these said exactly what was needed:

> to stop the fold, X and Y must be SYMBOLS (FP#9 message/file IDs via the
> file_table infra), not literals; identify the two symbols per call site.

The diagnosis was right; the symbols just did not exist yet. `message.sym`
gains four absolute definitions (`_MSG_c9b`, `_MSG_cc6`, `_MSG_d21`,
`_MSG_d4c`) and the C takes their addresses. A linker symbol definition emits
no bytes, and gcc cannot fold the difference between two link-time addresses.

**Which ids are symbols and which stay literals is not arbitrary.** The span
operands must be symbols or the fold returns. The first argument stays a plain
literal wherever the ROM loads it independently, and becomes a symbol only in
`2008130`, `2008148` and `2008180`, where the ROM reuses one register for both
the argument and a side of the subtraction. Getting that wrong costs an
instruction in either direction.

The same trick should apply anywhere the ROM computes a constant at runtime.

## Two build issues worth fixing upstream

**`make clean` deletes tracked files.** `data/strings/strings.s`,
`data/strings/strings.txt` and the four `src/lib/*.i` intermediates are all
committed, and `clean` removes them. Worse, the stale `.d` files left behind
carry **bare filenames with no directory**, so a later build fails with
`No rule to make target 'rom_2e00_b.c'` — pointing at a file unrelated to the
actual problem. Recovering needs `git restore` plus clearing every `.d`.

**`message.sym` is not a tracked dependency of `stage1.o`.** Editing it does
not trigger the relink that carries new symbols out to the overlays, so an
overlay referencing a newly added symbol fails with an undefined reference
until `stage1.o` is deleted by hand. Both of these are the same latent-
staleness family.

## Also in this branch

- **Six false-negative classes fixed in `tools/tryc.py`**, all of the same
  character — a spelling difference reported as a code difference. In order
  found: per-file label numbering; pool labels consuming a number; register
  aliases (`sl` vs `r10`); `.set` constants from `gba.inc` (`=REG_DMA3SAD` vs
  `=0x40000d4`); absolute symbols from `message.sym` (`=_MSG_c9b` vs
  `=0xc9b`); and the last of those needing to be resolved inside pool folding
  rather than in `canon()`.

  Each one reported a byte-exact function as a failure. `ActorCmd_GotoIfZ`,
  `Func_8091ff0`, `LoadMoveIcon` and `LoadOldMoveIcon` were all hidden by them
  at some point, and `Func_80a22f4` appeared to differ at instruction zero when
  that line was fine.

- **`tools/split_s.py` hardened twice**: it now keeps trailing data out of the
  part destined to become a `.c` (a `.rodata` section following the last
  function used to travel with it and vanish), and refuses when the preamble
  holds anything but includes — which is what a `.thumb_func_**S**tart` with a
  capital S did to it, silently duplicating a whole function across every part.

- **`Func_80a22f4` advanced from a structural mismatch to three instructions**
  using `DMA3_SET` from `include/dma.h`, which is the idiom its parked note
  asked for and which already existed in the tree.

## The blocker worth attacking next

Three independent functions now show the same thing: **gcc-2.96 as invoked
here is more eager to reuse a value than the original build was.** It derives a
constant by arithmetic from a previous one where the ROM loads it fresh; it
common-subexpressions an id into a callee-saved register where the ROM reloads
it; it builds one `-1` and copies it where the ROM materialises three.

Eleven flags affecting CSE and register reuse have been ruled out by
experiment (`-fno-cse-follow-jumps`, `-fno-rerun-cse-after-loop`,
`-fno-expensive-optimizations`, `-fno-caller-saves`, `-fno-force-mem`,
`-fno-defer-pop`, `-fno-strength-reduce`, `-fno-thread-jumps`, `-fno-peephole`,
`-fno-function-cse`, and both `-fno-schedule-insns` variants). So it is not the
invocation. Details in `src/non_matching/overlays/constant_reuse.c`.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'

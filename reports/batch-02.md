# Batch 02 — 7 functions elevated to matching C

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batch 01 is already
in — it carries the tooling and the corrected `struct Actor`.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a **clean** build, not an incremental one. Every function was screened
per-function with `tools/tryc.py` first, and every address below was read back
from the linked ELF rather than from memory.

## The functions

| Function | Address | New source | Replaces | Our reading |
|---|---|---|---|---|
| `LoadOldMoveIcon` | `0x08019f98` | `src/rom_15000/rom_19ebc_a_c_c_b.c` | **split** | load a move's item icon |
| `LoadMoveIcon` | `0x0801a3d0` | `src/rom_15000/rom_19ebc_a_c_c_c_b.c` | **split** | the same, second table |
| `Func_801c21c` | `0x0801c21c` | `src/rom_15000/rom_1aeec_c_a_a_a_a_a_c_a_a_b.c` | **split** | release a UI panel's OBJ tiles |
| `Func_8091ff0` | `0x08091ff0` | `src/rom_8a000/rom_91584_c_c_a_c_a_c_c_a_a_c.c` | whole file | start a looping sound |
| `Func_8097a54` | `0x08097a54` | `src/rom_8a000/rom_97384_c_c_b.c` | **split** | restart idle script once stopped |
| `Func_809ad70` | `0x0809ad70` | `src/rom_8a000/rom_9ad70_a_a_a_b.c` | **split** | idle palette flicker |
| `Func_80a735c` | `0x080a735c` | `src/rom_a1000/rom_a5534_c_c_c_b.c` | **split** | is this item selectable? |

Six needed `tools/split_s.py`. The sibling `.s` files it produced must come
across with them or the batch will not link:

| Split target | Siblings that must travel with it |
|---|---|
| `LoadOldMoveIcon` | `rom_19ebc_a_c_c_a.s`, `rom_19ebc_a_c_c_c_a.s`, `rom_19ebc_a_c_c_c_c.s` |
| `LoadMoveIcon` | (shares the chain above) |
| `Func_801c21c` | `rom_1aeec_c_a_a_a_a_a_c_a_a_a.s`, `..._c.s` |
| `Func_8097a54` | `rom_97384_c_c_a.s`, `rom_97384_c_c_c.s` |
| `Func_809ad70` | `rom_9ad70_a_a_a_c.s` |
| `Func_80a735c` | `rom_a5534_c_c_c_a.s`, `rom_a5534_c_c_c_c.s` |

`stage1.ld` changes to match, in order.

## One asm change, byte-neutral

`asm/rom_8a000/rom_97384_c_c.s` gains a single line:

    .global .La0128

`Func_8097a54` references that table, and a `.L` symbol is file-local — once
the function moves to C the reference crosses a file boundary and cannot link.
`.global` emits no bytes; `compare` was confirmed green across the change
before anything else moved. Its sibling `.La0108` was already exported the
same way.

**This technique matters beyond this one function.** A data table reachable
only as a local label can be named from C with an asm label:

    extern signed char Data_809f160[] __asm__(".L9f160");

That unblocks a whole category of functions whose only obstacle was
unreachable data.

## A type trap worth knowing

`include/gba/types.h` defines `s8` as plain `char`, which is **unsigned** in
this build (Camelot compiled with `__CHAR_UNSIGNED__`). A table of signed
offsets declared `s8` silently compiles to `ldrb` where the ROM has `ldrsb` —
no warning, just a wrong byte. `Func_809ad70` needs `signed char` spelled out.

## Another wrong annotation

`Func_801c21c` was documented as taking the panel in `r0`. It takes no
arguments; `r0` is loaded from the panel block before its only use.

That is the fourth annotation found wrong this way, and they all fail
identically: a parameter list inferred from which registers are live at the
top of a function. `docs/attribution.md` already warns the corpus gets
mechanism right and purpose wrong; this is the same defect in the signatures.

## Near-misses added

Parked in `src/non_matching/` under this tree's `rom_<addr>.c` convention,
each naming its blocker class from `docs/elevation.md` and listing what was
already tried:

- `rom_1671c` (`Func_801671c`) — scheduling; gcc hoists the destination
  address above the function-pointer load, through six formulations.
- `rom_1a2ec` (`LoadStatusIcon`) — scheduling; 26 of 27 instructions right,
  the three argument registers filled in a different order.
- `rom_bf524` (`Func_80bf524`, `Func_80bf54c`) — the same function at two
  offsets. The ROM stores the decremented byte untruncated and lets `strb`
  narrow it; every formulation here truncates first and pays an instruction.
  Worth retrying together, and there is a third sibling family with the shape.

## Three false negatives fixed in the screen

Worth stating because it changes how much a "no match" from `tools/tryc.py` is
worth. All three reported byte-exact functions as failures:

1. **Label numbering was per-file**, so a single-function candidate compared
   against a multi-function reference got different numbers for identical
   control flow. Hid `ActorCmd_GotoIfZ` for three rounds.
2. **Pool labels consumed a number** the ROM side never had, shifting every
   branch target after the first pool load. Hid `Func_8091ff0` entirely.
3. **Register aliases** — gcc emits `sl`, the disassembly writes `r10`. Hid
   `LoadMoveIcon` and `LoadOldMoveIcon`, which differed by nothing else.

Each surfaced only because a diff looked nonsensical and got read rather than
trusted. Every parked function has been re-checked against the corrected
screen after each fix; the ones still listed are genuine.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'

Per-function, without touching the build:

    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        python3 tools/tryc.py src/rom_8a000/rom_97384_c_c_b.c

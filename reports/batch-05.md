# Batch 05 — 7 functions elevated to matching C

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–04 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a **clean** build. Every address read back from the overlay ELFs, every
path confirmed.

## The functions

| Function | Address | New source |
|---|---|---|
| `OvlFunc_901_20084b4` | `0x020084b4` | `src/overlays/rom_797990/ovl_314_c_c_a_a_a_b.c` |
| `OvlFunc_910_20088e8` | `0x020088e8` | `src/overlays/rom_79dd90/ovl_30_c_c_c_c_a_c_b.c` |
| `OvlFunc_920_2008280` | `0x02008280` | `src/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a_b.c` |
| `OvlFunc_920_20082ac` | `0x020082ac` | `src/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a_c_b.c` |
| `OvlFunc_920_20082d8` | `0x020082d8` | `src/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a_c_c_b.c` |
| `OvlFunc_888_200a660` | `0x0200a660` | `src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_b.c` |
| `OvlFunc_968_2008594` | `0x02008594` | `src/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_b.c` |

All seven came out of splits, so the sibling `.s` files must travel with them.
The `rom_7a6ae4` chain is three sequential splits.

## The tooling change that produced most of this batch

`tools/elevation_candidates.py` now reads the assembly for **known-unmatchable
shapes** and reports or drops them (`--clean`):

| blocker | what it means |
|---|---|
| `narrow-mask` | the `mov`/`neg` pair that gates 34 functions — width solved, ordering open |
| `pool-tell` | a pooled constant that would fit in an 8-bit `mov`, so the operand was a **symbol reference**; blocked on naming |
| `arg-interleave` | a shifted constant's `mov`/`lsl` split by another register's move; nine formulations failed |

Of **1,940** overlay candidates, **1,116** show no known blocker. The three
`OvlFunc_920` stubs were the first three off that list and all matched on the
first attempt.

This mattered more than it sounds. The same knowledge had been in
`docs/elevation.md` as prose since batch 01, and I was still reading assembly
by hand to apply rules already written down. Prose does not filter 1,940
candidates.

## A pattern worth exploiting deliberately

These overlays are full of **families of near-identical stubs, duplicated per
map rather than shared**. Three families so far: `OvlFunc_974` (7 functions),
`OvlFunc_970` (4), `OvlFunc_920` (3).

Finding a family is worth far more than finding a function, and the ranking
surfaces them naturally — low branch count and high call count score well, and
duplicated stubs cluster at consecutive addresses in one file.

## Still blocked on a question only you can answer

Unchanged from batch 04 and worth repeating, because it is now **three**
functions and the same tell appears in candidates I am still skipping:

The ROM pools a constant that would fit in an 8-bit `mov` — `ldr r0, =1`,
`ldr r2, =0xf`, `ldr r1, =0` — which means the operand was a **symbol
reference** in the original source. gcc never pools what it can `mov`, and
always pools a symbol address. Verified by assembling both forms.

`message.sym` covers message ids and `file_table.sym` covers file ids. Neither
covers a map id (`__SetDestMap`'s first argument) or whatever `0xf` is in
`SetTextColor`.

**What are those id namespaces?** Naming them unblocks this class outright.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'

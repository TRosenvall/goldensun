# Batch 19 — 19 functions, and the arity cap that hid them

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–18 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean build, unassisted. Every address read back from the linked ELFs.

## The functions

All `GetEntrances` — the per-area table selector — at arities from two-way up to
twelve-way.

| `OvlFunc_920_20080f4` | `0x020080f4` | `src/overlays/rom_7a6ae4/ovl_30_c_a_c_a_c_b.c` |
| `OvlFunc_922_2008050` | `0x02008050` | `src/overlays/rom_7a8c8c/ovl_30_a_c_c.c` |
| `OvlFunc_922_20080f8` | `0x020080f8` | `src/overlays/rom_7a8c8c/ovl_30_c_a_c_a_a_b.c` |
| `OvlFunc_924_2008e80` | `0x02008e80` | `src/overlays/rom_7ac2d8/ovl_e20_c_c_a.c` |
| `OvlFunc_932_20080e4` | `0x020080e4` | `src/overlays/rom_7b9cb4/ovl_30_a_c_a_b.c` |
| `OvlFunc_932_200820c` | `0x0200820c` | `src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_a_b.c` |
| `OvlFunc_933_20083ac` | `0x020083ac` | `src/overlays/rom_7bc690/ovl_314_a_c_b.c` |
| `OvlFunc_936_20080ec` | `0x020080ec` | `src/overlays/rom_7c097c/ovl_30_a_c_b.c` |
| `OvlFunc_936_2008180` | `0x02008180` | `src/overlays/rom_7c097c/ovl_30_c_c_a_a_a_a.c` |
| `OvlFunc_936_2008240` | `0x02008240` | `src/overlays/rom_7c097c/ovl_30_c_c_a_a_c.c` |
| `OvlFunc_946_2008cc4` | `0x02008cc4` | `src/overlays/rom_7ced6c/ovl_30_a_c.c` |
| `OvlFunc_946_2008ec4` | `0x02008ec4` | `src/overlays/rom_7ced6c/ovl_30_c_c_a_a_c_a.c` |
| `OvlFunc_947_2009440` | `0x02009440` | `src/overlays/rom_7d0e88/ovl_1440_a.c` |
| `OvlFunc_947_200a580` | `0x0200a580` | `src/overlays/rom_7d0e88/ovl_2580_a_b.c` |
| `OvlFunc_951_20081a8` | `0x020081a8` | `src/overlays/rom_7d6418/ovl_30_c_c_c_a_b.c` |
| `OvlFunc_957_200b598` | `0x0200b598` | `src/overlays/rom_7e3e08/ovl_30_c_c_c_b.c` |
| `OvlFunc_959_2008a80` | `0x02008a80` | `src/overlays/rom_7e7574/ovl_9dc_a_c_a_a_b.c` |
| `OvlFunc_960_20083ac` | `0x020083ac` | `src/overlays/rom_7eaf28/ovl_314_c_a_c_a_b.c` |
| `OvlFunc_968_2008e04` | `0x02008e04` | `src/overlays/rom_7f2f14/ovl_30_a_c_c_c_b.c` |

`area.sym` gains 43 entries.

## A second self-imposed limit in the same sweep

Batches 08–15 elevated the two-, three- and four-way forms and reported the
families complete. **The sweep had `len(cs) <= 3` in it** — an arity cap I wrote
and never questioned. Removing it found 25 more, at arities up to twelve.

The first limit, corrected in batch 15, was matching only `.L` returns.

Same failure both times: a criterion narrowed to whatever the first member
happened to look like, then trusted because it kept returning matches. Finding
the second one immediately after correcting the first suggests the lesson I drew
from batch 15 — "seed from a member you have not solved" — was too narrow. The
real one is that **every constant in a sweep is a claim**, and `<= 3` was one I
made without noticing.

The twelve-way is 65 instructions and is the same C as the two-way with ten more
comparisons. Nothing new was needed to match it, which is the argument for
removing the cap rather than treating high-arity forms as a different problem.

## Two guards added, both from mistakes in this batch

**`tools/asmfacts.py`** now exists so that facts about a `.s` are importable
rather than reachable only through `split_s.py`:

- `carries_data()` — "one function in the file" and "nothing else in the file"
  are different facts, and only the second decides whether the `.s` can be
  deleted. I read a sweep's "whole-file" as the second, deleted a `.s` holding
  twelve `.incbin` blobs, and broke the link. `split_s.py` had refused that exact
  case since batch 10; generating from a sweep's output simply never called it.
- `return_targets_are_symbols()` — the sweep counts "N compares, N+1 pool loads
  into `r0`" and cannot tell a table address from a constant, so a function
  loading a flag id satisfies it. Generated C for those does not compile. That
  confusion cost **five** generate-and-screen cycles across batches 15 and 19.
  I parked one such function last round, which was treating the symptom; three
  more appeared immediately.

## A commit that only built because of a stale object

Worth stating in full, because it is a property of the build system rather than
a slip in the discipline.

Commit `06c3147` left three overlay linker scripts referencing `.o` files with
no source, and **passed `make -j8 && make compare` anyway**. Make treats an
existing `.o` with no rule as up to date, so objects from the previous build
satisfied the link. `make clean` removed them and the next build failed with
`No rule to make target`.

Two came from splits whose generated `.c` was an over-match and got deleted. The
third is worse: a cleanup loop removed `ovl_30_a_c_a_c.s`, the source of a
function that had **never been elevated** — it was in the loop's input because
`split_s.py` had reported it as needing no split, and the loop deleted every path
in that list without checking whether the corresponding `.c` had survived. Five
`.s` files deleted for three `.c` files.

Repaired in `57e3eb8`. Two rules now in `docs/elevation.md`:

- **`make -j8 && make compare` does not gate a commit that removes source.**
  Run `python3 tools/asmfacts.py --orphans` first — it answers "is the tree
  consistent" in a second instead of a five-minute rebuild.
- **Derive a cleanup list from what was written, not what was attempted.**

Only `make clean && make compare` answers "does this build from source", which
is the check every batch report has always used. That is why this surfaced
within one round rather than at porting time.

## Still open, and still only answerable by you

- **Five ambiguous offsets in `actor.h`** (batch 03), documented rather than
  guessed.
- **`narrow_constant`**, 34 functions, down to one peephole: gcc folds the mask
  to `sub r3, #0x10` because a `3` is live and `3 - 0x10 == ~0xc`.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'

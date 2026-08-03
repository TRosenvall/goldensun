# Batch 10 — 12 functions, a second GetEntrances family, and two guard fixes

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–09 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean build, unassisted. Every address read back from the linked overlay
ELFs.

## The functions

Two singles:

| Function | Address | New source |
|---|---|---|
| `OvlFunc_901_2008bf8` | `0x02008bf8` | `src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_c_b.c` |
| `OvlFunc_956_20081b4` | `0x020081b4` | `src/overlays/rom_7e0928/ovl_30_a_c_c_a_c_b.c` |

Ten from a **second `GetEntrances` family** — same gState halfword, but a
four-way selector instead of two-way. All whole-file conversions:

| Function | Address | New source |
|---|---|---|
| `OvlFunc_907_2008198` | `0x02008198` | `src/overlays/rom_79b154/ovl_30_c_a_a_c_a_a_c.c` |
| `OvlFunc_920_2008040` | `0x02008040` | `src/overlays/rom_7a6ae4/ovl_30_a_c.c` |
| `OvlFunc_924_2008e20` | `0x02008e20` | `src/overlays/rom_7ac2d8/ovl_e20_a.c` |
| `OvlFunc_927_2008ee0` | `0x02008ee0` | `src/overlays/rom_7b4558/ovl_30_a_c_c_c.c` |
| `OvlFunc_934_2008d20` | `0x02008d20` | `src/overlays/rom_7bdeb0/ovl_d20_a.c` |
| `OvlFunc_935_2008030` | `0x02008030` | `src/overlays/rom_7bf5a8/ovl_30_a.c` |
| `OvlFunc_935_20082e0` | `0x020082e0` | `src/overlays/rom_7bf5a8/ovl_2e0_a_a.c` |
| `OvlFunc_942_2008040` | `0x02008040` | `src/overlays/rom_7c6bac/ovl_30_a_c.c` |
| `OvlFunc_958_2008cc0` | `0x02008cc0` | `src/overlays/rom_7e636c/ovl_cc0_a.c` |
| `OvlFunc_959_20089dc` | `0x020089dc` | `src/overlays/rom_7e7574/ovl_9dc_a_a.c` |

Two of these sit at the same address (`0x02008040`) in different overlays,
which is expected — overlays share a load base.

`unknown_id.sym` gains 28 entries. Every compared constant in this family is
pooled where a `cmp #imm` would fit, so all of them are pool-tell symbols.

## Two guards that were wrong, and are now right

Both cost a real failure. Both are the kind that only show up at link or build
time, well after the thing that caused them.

### 1. `split_s.py` advised deleting a file's data

An eleventh family member, `OvlFunc_924_2008f30`, screened clean and broke the
link:

    undefined reference to `.L6c10'

Its `.s` holds one function **and fourteen `.incbin` tables**. Deleting the
`.s` after writing the `.c` deleted the data with it.

Neither guard saw it. `tryc.py` compiles and does not link. And `split_s.py`
had *actively advised* the deletion — its "holds only this function, convert it
directly" check counted **functions** and ignored everything else in the file.

It now refuses that case:

    ovl_e20_c_c_c_c_c_c_c.s holds only OvlFunc_924_2008f30, but ALSO 14
    .incbin blob(s) and 17 label(s)...
    Converting the whole file would delete that data and the link would fail.

The data-free case still advises direct conversion, so the common path is
unchanged. All six functions in the second half of this batch were checked with
it first, and all six came back "and no data".

`OvlFunc_924_2008f30` is left as assembly. Separating a function from its own
data is a hand job the splitter cannot do — it cuts on function boundaries.

### 2. The screen used the wrong optimisation level for parked candidates

Re-screening the park (see below) reported one new match. It was a false
positive: parked files live in `src/non_matching/`, which is not where the
build puts them, so the Makefile had no per-file rule for their path and
`tryc.py` fell back to `-O2`. That TU builds at `-O1`, where it does not match
at all.

`tryc.py` now takes flags from the `--ref` assembly as well as the `.c` path,
so a scratch or parked candidate is screened with the flags of the translation
unit it would really become.

Worth knowing if you screen candidates from scratch paths — the failure mode
is a clean match that then fails the build, which is the most expensive kind.

## The park is now maintained, not just appended to

`tools/rescreen_park.py` re-runs every parked function against the current
screen. The reason to have it: `OvlFunc_906_2008314` (batch 08) sat parked with
a note that correctly diagnosed both its diffs and proposed the permuter for
one of them. It needed neither — only a precedent written three batches later.
Re-screening turned that one parked function into ten elevated ones.

Running it exposed that **33 of 83 parked notes named a `Source asm:` path that
no longer existed**, because the `.s` had since been split or elevated. Those
could not be re-screened at all.

- 27 repointed at the `.s` that currently defines the function.
- 6 deleted: they were for functions that have **since been elevated**
  (`OvlFunc_936_20095e0`, `OvlFunc_968_2008594`, and the four `OvlFunc_970`
  stubs). A parked note claiming a matching function does not match is worse
  than no note.

77 parked, all 77 now genuinely screenable.

## A technique, and a class it does not solve

**A local keeps a shifted constant's `mov`/`lsl` pair together.** Where gcc
splits the pair around the next argument:

    rom    mov r1, #0xc8 / lsl r1, #4 / ldr r0, =OvlFunc_956_200804c
    ours   mov r1, #0xc8 / ldr r0, =OvlFunc_956_200804c / lsl r1, #4

assigning the shifted value to a local before the call makes gcc finish
building it first. Spelling the same value as one constant (`0xc80`) does not
work; hoisting the *other* argument does not work. Solved
`OvlFunc_956_20081b4`.

**This does not solve `arg-interleave`**, which points the other way: there the
ROM wedges a plain `mov` *into* the pair and gcc emits the pair contiguously.
The park note for `OvlFunc_891_20095d4` now records that its diff was
misdiagnosed — it was never the interleave, which gcc reproduces unaided. It is
one instruction: where `mov r0, #2` sits.

gcc *does* emit the wedged form, in three honest sites, one of which calls the
same function with the same two constants. The mechanism is control flow
separating each assignment from its use, which cannot be transplanted into a
function that has no branches.

### A methodology warning

The sweep that found those sites first reported **335** of them, and was wrong.
It counted `// fakematch` translation units, which force shapes with inline-asm
barriers and register variables — so their listings are evidence about the
barriers, not the compiler. The tell is `.code 16` directives leaking into the
output. Excluding them, the honest figure is 212.

Filter on `fakematch.txt` and the `// fakematch` first-line marker before
concluding anything from generated assembly.

## Still open, and still only answerable by you

- **Semantic names for the id namespaces.** `unknown_id.sym` now holds 30-odd
  entries and is carrying real weight, so a real name is worth more than it was
  two batches ago. Note again that there are demonstrably at least two id
  spaces that both look like map ids.
- **Five ambiguous offsets in `actor.h`** (batch 03), documented rather than
  guessed.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'

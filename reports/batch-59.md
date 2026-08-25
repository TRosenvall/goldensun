# Batch 59 — six functions, and two checks that close silent failures

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_882_20092f0` | `020092f0` | ovl_77dd1c | [ovl_30_c_c_c_a_c_c_c_c_b.c](../src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_b.c) |
| `OvlFunc_943_200b150` | `0200b150` | ovl_7c7b9c | [ovl_30_c_a_a_c_a_c_a_c_b.c](../src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_b.c) |
| `OvlFunc_945_200cfa8` | `0200cfa8` | ovl_7cb2c0 | [ovl_30_c_c_c_c_c_c_a_c_c_a_c_b.c](../src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_c_c_a_c_b.c) |
| `OvlFunc_948_2009da0` | `02009da0` | ovl_7d30e0 | [ovl_30_c_c_c_a_b.c](../src/overlays/rom_7d30e0/ovl_30_c_c_c_a_b.c) |
| `OvlFunc_959_200a410` | `0200a410` | ovl_7e7574 | [ovl_9dc_…_c_b.c](../src/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_c_c_c_b.c) |
| `OvlFunc_959_200a468` | `0200a468` | ovl_7e7574 | [ovl_9dc_…_c_c.c](../src/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_c_c_c_c.c) |

## A green build does not prove a new `.c` is being used

`split_s.py` reports *"holds only X and no data, convert it directly"* for a
single-function `.s` — meaning the `.c` replaces **that** file and keeps its name.
This batch wrote one under the `_b` name a **split** would have produced.

It compiled. `make compare` stayed green. `--orphans` reported clean. The stray
object was not *missing* from anything — it was simply unreferenced, while the
original `.s` went on supplying the function. It was caught by reading a
directory listing.

**`tools/asmfacts.py --unlinked` closes that**: for every `src/*.c` outside
`non_matching` and `src/lib`, check that some linker script names its `.o`.

### Its first run found seven, all inherited

```
src/rom_b0000/dummy.c
src/rom_c0/rom_447c_a_b.c
src/rom_f9000/rom_f9ef8_b.c, _c_a_b.c, _c_a_c_b.c, _c_b.c, _c_c_b.c
```

All seven arrived with the base commit, so they predate this work and are
**reported in `HANDOFF.md` rather than changed**. The `rom_f9ef8_*` ones look
like an abandoned pass at the m4a engine — the linker script takes
`src/lib/m4a/m4a.o(.text)` for that region and the matching `.s` files are gone.

Worth knowing rather than tidy-up trivia: **anyone who elevates a function into
one of those files gets a passing build and no effect on the ROM.**

## Sixty parks are within six instructions

Measured with `tools/near_parks.py` (added this batch), which counts instructions
in *disagreeing regions* rather than positions. That separates two populations
"parked" was hiding — a compiler difference nobody has cracked, versus a function
whose C is probably wrong — and says the remaining difficulty is concentrated in
a handful of compiler behaviours rather than spread across the corpus.

**Its first advice was wrong and following it caught that.** It flagged one park
as *"screens clean — re-check first"*; `tryc` normalises literal-pool loads, so a
function whose only defect is **pool placement** screens OK and still fails
`make compare`. `src/non_matching/ovl_7ec19c/200816c.c` is exactly that, already
through two split-and-revert cycles. The tool now says so and points at it.

## Enumerating a shape beats reading the ranked list

Two rounds off `pick_candidates.py` yielded **one function each** — both landing
on register-allocation residuals. Enumerating a *shape* instead — 24–42
instructions, all calls to simple helpers, no struct fields — found twelve, of
which **eight are now elevated**, nearly all first attempt.

Rank by tractability and you get a mixed bag; describe what you can already do
and you get a worklist.

## Two selectors unsigned in one function

`OvlFunc_943_200b150` and its twin are the first functions here with a real
`switch`. It lowers to gcc's **balanced tree** — `cmp #1 / beq`, then
`cmp #1 / bcc`, then 2 and 3 — which a `switch` reproduces directly and an
if/else chain does not.

The `bcc` rather than `blt` says the **selector is unsigned**. And the loop
counter is unsigned too: `while (i <= 8)` on a signed `int` gives `ble` where the
ROM has `bls` — **one instruction of 44**, after everything else matched. The same
tell twice in one function, once for a parameter and once for a local.

## A value that spans nothing must not reuse a held local

`OvlFunc_882_20092f0` holds two values across four calls, and has a fourth stack
value that must **not** reuse either — the ROM builds it with a fresh
`mov r3, #0x2b` rather than overwriting the held `r5`. Writing it into the same
local is 10 of 39: gcc reuses the held register and the whole `r5`/`r6`
assignment swaps.

That is batch 57's "one local per independent operation" with the tell now
concrete: **a fresh `mov` into a scratch register, rather than a re-store of a
held one, is the reference saying this value spans nothing.**

## Also parked

`OvlFunc_959_2008dcc` (6 of 38) on a **new sub-shape of constant-CSE** — the same
constant as *two arguments of one call*, which `-fno-rerun-cse-after-loop` makes
worse and separate locals do not touch; `OvlFunc_968_2009644` (4 of 39), where
the basic-block lever was tried *after* the plain form per batch 57 and did
nothing; `Anim_Attack` (5 of 39), pure register exchange; `OvlFunc_941_2008094`
(9 of 30), gcc chaining two field addresses — the cross-jumping form batch 56
established is unfixable; and `OvlFunc_965_200a46c` (2 of 30), **prologue order**,
now a named shape after a second instance.

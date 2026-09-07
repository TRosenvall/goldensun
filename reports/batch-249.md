# Batch 249 — six twins for the price of three, and the search has an order

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_964_2009744` | `0x02009744` | [ovl_30_a_c_c_a_c_a_c_a_c.c](src/overlays/rom_7ed0a0/ovl_30_a_c_c_a_c_a_c_a_c.c) |
| 2 | `OvlFunc_964_2009550` | `0x02009550` | [ovl_30_a_c_c_a_c_a_a_c_c.c](src/overlays/rom_7ed0a0/ovl_30_a_c_c_a_c_a_a_c_c.c) |
| 3 | `OvlFunc_958_20091f8` | `0x020091f8` | [ovl_cc0_c_c_a_c.c](src/overlays/rom_7e636c/ovl_cc0_c_c_a_c.c) |
| 4 | `OvlFunc_903_2008dd8` | `0x02008dd8` | [ovl_314_c_c_c_a.c](src/overlays/rom_798dc4/ovl_314_c_c_c_a.c) |
| 5 | `OvlFunc_906_20084f4` | `0x020084f4` | [ovl_314_c_c_c_b.c](src/overlays/rom_79aad8/ovl_314_c_c_c_b.c) |
| 6 | `OvlFunc_934_2008f78` | `0x02008f78` | [ovl_d20_c_c_a_c_c.c](src/overlays/rom_7bdeb0/ovl_d20_c_c_a_c_c.c) |
| 7 | `OvlFunc_934_2008e04` | `0x02008e04` | [ovl_d20_c_c_a_c_c.c](src/overlays/rom_7bdeb0/ovl_d20_c_c_a_c_c.c) |
| 8 | `OvlFunc_932_2008d2c` | `0x02008d2c` | [ovl_30_…_a_c_a_b.c](src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_a_c_a_b.c) |

## Parked

| function | address | blocker | floor |
|---|---|---|---|
| `OvlFunc_968_200a47c` | `0x0200a47c` | halfword store poisoning a later narrow constant across a call | **11 of 273**, exact size, count and relocations |
| `OvlFunc_968_2009d48` | `0x02009d48` | interference-graph colouring: which of `p`/`r` takes the high register | **2** once the spill is forced |
| `OvlFunc_968_200c2bc` | `0x0200c2bc` | loop-form / induction-variable placement | **110 of 271** at exact encoding count |

## THREE TWIN PAIRS, AND THE SECOND OF EACH WAS NEARLY FREE

| pair | second member's first screen |
|---|---|
| `2009744` → `2009550` | **exact**, no lever re-derived |
| `2008dd8` → `20084f4` | **exact** immediately after `sed` |
| `2008f78` → `2008e04` | **3 differing**, all in the one construct the first lacks |

Six of the eight elevated came from three solved functions. The twin play has
now paid six times this week and is the highest-yield move on the board.

### But the minimised pin set does NOT transfer — and you can predict which sites move

The genuinely new result, and it is a *prediction available before compiling*.
Of five pinned sites in the `2008dd8`/`20084f4` pair, the three whose constants
are unchanged cost identically in both files; **the two whose constants changed
are exactly the two that move.**

| site constants | shape | pin worth |
|---|---|---|
| `0xa0`, `0xc0`, `0x8b`, `0xc4` (first twin) | bare `mov #imm8` — all-cheap | **inert** |
| `0xbc<<1`, `0x90<<1`, `0xad<<1`, `0x92<<1` (second) | `mov`+`lsl` pairs | **2** |

The recorded all-cheap prune says a site with nothing to common needs no
ordering pin. Applied *across* twins it becomes predictive: read the constants,
know which pins move.

It bites on **width** too, non-additively — every site reduces to PIN2 alone on
both, but all five together collapse on the second twin (145 differing, four
bytes short).

> **Minimise on the expensive-constant twin, or ship the uniform fill.** A set
> minimised on the cheap twin under-covers silently.

## THE SEARCH HAS AN ORDER, AND IT IS MECHANISM SIZE

`2008d2c` is the clean demonstration:

| step | differing |
|---|---|
| first draft | 169 |
| `__sin` hoisted out of the `if` (the ROM calls it unconditionally) | **14** |
| a union on the `int` field, forcing alias set 0 | **6** |
| permuting the three opening reads | **EXACT** |

Run the permutation sweep **first** and its six orderings measure 6, 6, 6, 7, 10
with nothing under 14 to find — the function reads *blocked*. This is a second
specimen for the pin-strip ordering rule, which now generalises:

> **Control flow, then alias, then allocation, then order.** Order the search by
> mechanism size, not by cost of trying.

## A ONE-MEMBER UNION IS STILL ALIAS SET 0

This corrects the "and only those" clause of the batch-244 entry, which says a
union conflicts against accesses whose type is one of its **members**.

gcc-2.96's C front end walks out through every `COMPONENT_REF`/`ARRAY_REF` and
returns a flat **0** the moment any step's object is a `UNION_TYPE`. The subset
machinery is never consulted, so the member list cannot matter. Controls on the
finished base, against an `unsigned short` load:

| wrapper on the `int` store | result |
|---|---|
| `union { int w; unsigned short h; }` | EXACT |
| `union { int w; long l; }` — no related member | EXACT |
| `union { int w; }` — **one** member | **EXACT** |
| `struct { int w; }` | 8 differing |
| no wrapper | 8 differing |

The struct control is the discriminator: same single member, same layout, 8
differing. **It is the union-ness, not the membership.**

The same entry's *"the only source-level escape is `volatile` on both sides"*
also fails here — `volatile` measures 131 differing and **eight bytes long**, so
it is not a worse escape, it is not an escape.

## A PIN CAN BE WORSE THAN NO PIN, AND WIDTH DECIDES

On `2008e04`, at one call site: no pin **3**, PIN1 **4**, PIN3 **exact**. "N pins
is a size, not a set" now has a companion — the **width** of a single pin is not
free to guess either.

## A REUSED LOCAL SURFACES 25 INSTRUCTIONS AWAY

On `2008f78`, one local per call site rather than a reused one is 16 → exact.
The defect the reused local causes appears **twenty-five instructions
downstream**, moving a loop constant from r2 to r3. Anyone chasing that constant
would never think to look at the declaration list.

## `hiv` READ TRUE THIS TIME

Recorded deliberately, because the last two rounds recorded it reading false in
both directions. On `2008f78` (`hiv=2`) gcc reached both of the ROM's high
registers unaided and **no eviction pin appears anywhere in the file.**

Against that, `2009744` was selected as `hiv=1` on the theory that low
high-register traffic means no eviction work, and the opposite held: the ROM
holds one value, unaided gcc used **four**, and the eviction pins were the
largest single lever (190 → 144). `hiv` counts the **ROM's** registers, so a low
value means gcc has that many *more* candidates to shed.

## TWO LANDINGS NEEDED A SPLIT, AND ONE COULD NOT BE DELETED AT ALL

`20084f4`'s `.s` holds one function **and** a trailing `.section .data` with
seven `.incbin` blobs — none used by that function, all seven imported by four
already-elevated siblings. Deleting it breaks the link on all seven.
`tools/asmfacts.py` says so directly (*"WHOLE+DATA needs a hand split — do NOT
delete the .s"*), and `split_s.py` cannot cut it because with a single function
the data sits inside its block. Hand split; `_b.s` the function, `_c.s` the
data; **both `.ld` lines kept their `asm/` paths.**

`2008d2c` was a normal two-function split, and `split_s.py` rewrote the linker
script itself.

**Both landings gated the layout-only build green FIRST** — split in place,
function still in assembly, no `.c` anywhere. That is what separates a layout
mistake from a bad decompilation, which otherwise look identical at the end.

## TWO PROCESS FAILURES, BOTH CAUGHT

**A green build does not tell you which source it used.** A `git rm` on a
freshly-split `.s` failed because the file was never tracked, and `set -e` did
not stop the commands after it — so the `.c` landed while the `.s` still existed
and the compare came back green with two rules able to produce the same `.o`.
Reading `make -n` settled it: the `asm/%.o: src/%.c` rule wins and *regenerates*
the `.s` from the `.c`. The green was correct, but it was not **evidence** until
checked.

**A single `objcmp` run is not evidence, in either direction.** One function
reported 116 differing once and OK on five consecutive re-runs, with an
independent objdump diff empty. That is *not* the recorded ASLR
non-determinism, which produces two wrong bytes, not 116. A surprising failure
and a surprising success are equally suspect.

## Gate

`make clean && make -j8 && make compare` green, `goldensun.gba: OK`. Every
address above checked against the linked ELF with `tools/checkaddr.py`. Pairing
4,602 sources; `--orphans` 0; `--unlinked` unchanged at 10, so no landing here
added debt.

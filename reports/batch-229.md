# Batch 229 — three functions were already solved, and the tool that says so already existed

Eight functions. Three of them cost almost nothing, because they are the same
code as functions this project had already matched. Finding that out was worth
more than any single match in the batch.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_941_2008828` | `0x02008828` | [ovl_30_…_a_c.c](src/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_c_c_a_c.c) |
| 2 | `OvlFunc_955_20090dc` | `0x020090dc` | [ovl_30_…_c_b.c](src/overlays/rom_7ddb88/ovl_30_c_c_c_c_a_c_b.c) |
| 3 | `OvlFunc_956_2009a0c` | `0x02009a0c` | [ovl_30_…_c_b.c](src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c_b.c) |
| 4 | `OvlFunc_954_2008974` | `0x02008974` | [ovl_30_c_c_c_a.c](src/overlays/rom_7db0c8/ovl_30_c_c_c_a.c) |
| 5 | `OvlFunc_955_2009538` | `0x02009538` | [ovl_30_…_c_a.c](src/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_a.c) |
| 6 | `OvlFunc_885_2008170` | `0x02008170` | [ovl_30_…_c_a.c](src/overlays/rom_78603c/ovl_30_c_c_a_c_a_a_c_a.c) |
| 7 | `OvlFunc_955_20099bc` | `0x020099bc` | [ovl_30_…_c_b.c](src/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c_b.c) |
| 8 | `OvlFunc_956_2008c5c` | `0x02008c5c` | [ovl_30_c_c_c_a_a_b.c](src/overlays/rom_7e0928/ovl_30_c_c_c_a_a_b.c) |

## Twins

`OvlFunc_956_2009a0c` is the same 207-instruction cutscene as the already-solved
`OvlFunc_955_20090dc`, differing in **one immediate** — a message id, `0x20e9`
against `0x20ed`. Copying the solved candidate and changing that constant was
byte-exact on the **first screen**: no pin re-derived, no fill re-ordered, no
statement moved.

**I then wrote a tool to generalise that, and the tool already existed.**
`tools/solved_twins.py` has been in this repository since 29 August and does
precisely the same thing — searches remaining functions against solved ones,
matching on the mnemonic stream only, no registers or immediates — and
`docs/elevation.md` documents it under its own heading, alongside
`find_twins.py`, `twin_families.py`, `twin_finder.py` and `find_families.py`.
The original claim here, that nothing was looking for these, was false.

CLAUDE.md says to grep elevation.md before writing anything up as a new finding.
I did grep it — for the phrasings of my own conclusions, which of course were not
in it. I did not grep it for the CONCEPT before building. Searching for what you
have already decided to say is not searching.

The duplicate has been deleted. It was also strictly worse: it used a
40-instruction floor where `solved_twins.py` uses 12, and that floor hid a real
hit — `OvlFunc_924_2008ffc` at 36 instructions, which the existing tool finds
and mine did not.

What survives is the work, not the tooling claim. Four twin pairs were found and
all four are byte-exact: `OvlFunc_956_2009a0c` (one immediate),
`OvlFunc_954_2008974` (eight operands), `OvlFunc_956_2008c5c` (eight operands and
one symbol), and `OvlFunc_911_200a7ac` (no operand differences at all — only
names). Each cost a handful of substitutions rather than a screening run. That
part is real, and it is what `solved_twins.py`'s own entry predicted: "a hit is
the cheapest elevation there is".

## What a correct symbol looks like, twice

Two functions needed a link-time symbol, and both showed the **same signature**
under objcmp: one encoding differing where a pool word reads its value in the
reference and **zero** in ours, plus one extra `R_ARM_ABS32` relocation the
reference "lacks".

- `OvlFunc_885_2008170` needed `_MSG_fbf`. The ROM holds `0xfbf` across a call
  and reaches its neighbours with `add r0, r5, #1` / `#2`. As a literal, gcc
  constant-propagates and folds `m+1` and `m+2` into two more pool words —
  228 differing. gcc cannot fold arithmetic on a link-time address, so the
  symbol is the only spelling that survives.
- `OvlFunc_956_2008c5c` needed `_AREA_91`. The ROM **pools** `0x91` where an
  eight-bit `mov` would do, which is the recorded tell; its twin carries the
  same shape one area earlier with `_AREA_8f`, already in `area.sym`.

That zero-pool-word-plus-phantom pattern is what a *correct* symbol looks like
against a disassembly, and it is indistinguishable by eye from a refutation.
Only `make compare` settles it. Both are green.

**And the reference disassembly is symbol-substituted from `message.sym`.**
`OvlFunc_885_2008170` reaches ten message ids; exactly one is in `message.sym`
and exactly one prints as a symbol in the `.s`. So a bare literal in a reference
is **not evidence against a symbol** — it only means the id was not in the file
when that `.s` was written. `tryc` normalises `=_MSG_*` to its value and cannot
see the difference at all. This turns the recorded "a relocation check cannot
refute a symbol" from a caution into a mechanism.

## An index into a shape is itself a measurement

Two functions were salvaged from screening runs killed mid-minimisation, so the
candidates existed and their claims did not. Re-measuring rather than trusting
them is what turned this up.

The `2008828` run left a `drop_fwd.json` naming eighteen pin-site **indices**,
beside the `cand2.c` it had built from them. `cand2.c` verifies byte-exact. But
re-running that drop set against the current source does **not** reproduce it,
and is not exact: the list names index 93 where the file now holds only 76 pin
blocks. The source had been revised between the sweep and the save, so every
index past the edit re-bound to a different site.

**Nothing errors.** An out-of-range index is never matched; an in-range one
unpins whatever now sits there. The rebuilt candidate looks like a finished
minimisation and is not. Unlike a differing count, an index carries no evidence
of having gone stale.

Minimise from a candidate you have **confirmed exact**, using it as its own
base. Re-run that way, `2008828` ships all 76 pins with none removable.

## A partial pin can be worse than no pin at all

On `OvlFunc_955_20099bc`, pinning **only q0** at `__Func_80933f8` measured
**20 differing — against 8 for no pins anywhere**.

It is not a partial win but a regression of a different kind. With r0's value
forced into the hard register, the block's pressure drops far enough that CSE's
`-1` pseudo survives allocation into a *callee-saved* register, so the ROM's
per-site `mov r1,#1 / neg r1,r1` becomes a spill-and-copy — one extra
instruction, and every following relocation shifts.

The recorded rules ("pin the first use", "one pin at the first use covers the
later ones") say *which site* to pin. This says **how far along the argument
list**: a pin set must reach through any constant the site shares with a later
site, or it hands that constant to CSE.

## Two smaller corrections

**The withheld-prototype set is per-site and does not transfer between
siblings.** Both the template neighbour's header and elevation.md name
`_1078`, `_15b8`, `_5e4` as the three callees wanting r0 last. In
`OvlFunc_955_2009538` only two respond — `OvlFunc_common1_1078` is indifferent,
exact as void, int, or absent. The list must be re-derived, not copied.

**A discarded result still selects the order.** `OvlFunc_955_2008310`'s return
value is never read, yet it must be declared `int`; `void` costs 2 differing.
You cannot type a callee `void` merely because nobody uses what it returns.

## Housekeeping

Four `.s` files were split, one of them carrying `.data`/`.data1`. Every split
was gated on a `make compare` **green on the layout change alone** before any
`.c` was written.

One screening note advised deleting `stage1.o` by hand after editing
`message.sym`, because it is not a tracked dependency. **That advice is out of
date and checking beat following it**: `Makefile:65` already defines
`ld_sym_deps`, which makes each ELF depend on the `.sym` files its linker script
INCLUDEs, and the comment directly above records the exact `_MSG_256c` failure
that motivated the fix.

A `.data` line in a linker script is also not evidence that the `.s` has data in
it — `rom_7ddb88/ovl_30_c_c_c_c_a_c` is named for all three sections and carries
none. The script names sections an object *may* contribute.

## Carried forward, and one convention I could not pin down

`OvlFunc_960_2008c00` verified byte-exact (228 bytes, 93 encodings, 18
relocations) but arrived ninth, so it is held for batch 230 rather than
stretching this one past the gate. It needs `_CONST_2` and `_AREA_a5` and a
split, and it brought a finding worth its own write-up: **a pin can be a symptom
of a different defect and measure load-bearing right up until the real defect is
fixed.** A `register ... __asm__("r5")` there took the screen from 18 differing
to 11 and looked like the allocation fix; splitting a second local out of a
variable doing double duty fixed the allocation for the right reason, and with
that done the pin measures exactly zero and does not ship. Greedy sweeping only
caught it because the drop was re-tested *after* an unrelated later edit.

The fourth twin, `OvlFunc_911_200a7ac`, is also carried forward: its diff is
almost entirely a different jump table and a different `gScript_` symbol.

**`fakematch.txt`: I still do not know the criterion, and I have not guessed
one.** The file indexes 277 unique paths and tracks the in-file `// fakematch`
marker to within three files of drift. What it does *not* track is register
pins: 452 `src/*.c` files use `register ... __asm__` and are correctly absent,
so "ships pins" is not the rule. This batch therefore added rows only where the
candidate carried the marker, which left `OvlFunc_955_20099bc` unlisted despite
a screening note asking for it. That is a deliberate choice to follow the
observable convention rather than invent one, and it is flagged here so the next
person can settle it — along with the pre-existing exact duplicate row for
`src/rom_c0/rom_3650_c_b.c`.


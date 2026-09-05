# Batch 230 — a tool I duplicated, and an instruction the compiler cannot write

Six functions elevated and one parked. The batch's most useful result is a
correction to the previous one; its most useful *findings* are a blocker proved
by counting rather than argued, and a blocker class partly refuted.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_936_200a008` | `0x0200a008` | [ovl_30_…_c_b.c](src/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_c_c_c_b.c) |
| 2 | `OvlFunc_960_2008c00` | `0x02008c00` | [ovl_314_c_c_a_a_b.c](src/overlays/rom_7eaf28/ovl_314_c_c_a_a_b.c) |
| 3 | `OvlFunc_911_200a7ac` | `0x0200a7ac` | [ovl_30_c_c_a_c.c](src/overlays/rom_79e5c0/ovl_30_c_c_a_c.c) |
| 4 | `OvlFunc_897_2008e30` | `0x02008e30` | [ovl_30_a_c_c_b.c](src/overlays/rom_791794/ovl_30_a_c_c_b.c) |
| 5 | `OvlFunc_924_2008ffc` | `0x02008ffc` | [ovl_f84_a_a_c.c](src/overlays/rom_7ac2d8/ovl_f84_a_a_c.c) |
| 6 | `OvlFunc_956_200a0f0` | `0x0200a0f0` | [ovl_30_…_c_b.c](src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c_b.c) |

## Parked

| function | address | file | blocker |
|---|---|---|---|
| `OvlFunc_882_2009154` | `0x02009154` | [2009154.c](src/non_matching/ovl_77dd1c/2009154.c) | an instruction gcc-2.96 cannot emit |

## The correction: I rebuilt a tool that already existed

Batch 229 landed three functions that were near-copies of already-solved ones,
generalised that into `tools/twins.py`, and reported that **nothing had been
looking for them**. That was false.

`tools/solved_twins.py` has been in this repository since 29 August and does
precisely the same thing — remaining functions searched against solved ones,
matched on the mnemonic stream only, no registers or immediates. It has its own
heading in `docs/elevation.md`, alongside `find_twins.py`, `twin_families.py`,
`twin_finder.py` and `find_families.py`. **There were five such tools. I made a
sixth.**

CLAUDE.md says to grep `elevation.md` before writing anything up as a new
finding, and I did grep it — for the phrasings of my own conclusions, which of
course were not in it. I never grepped it for the **concept**. Searching for
what you have already decided to say is not searching: it returns nothing, and
the nothing reads as confirmation. `ls tools/ | grep -i twin` would have ended
it in one command.

This is the fourth instance of a failure this project already documents. The
entry "Ad-hoc candidate scans MUST reuse `pickable.parked()`" was **already
logged as the third repeat** when it was written; that one is about re-deriving
a park, this one about rebuilding a whole tool. Mine is now filed against that
lesson rather than as a discovery.

The duplicate was also strictly worse, which is the usual shape of a
reimplementation: it used a 40-instruction floor where `solved_twins.py` uses
12, and that floor hid a real hit. `OvlFunc_924_2008ffc` is 36 instructions —
the existing tool finds it, mine did not. It is elevated here, which is the
cleanest possible close to the episode.

**What survived was the work, not the tooling claim.** All four twin pairs are
byte-exact and each cost a handful of substitutions rather than a screening run
— exactly what `solved_twins.py`'s own entry already promised: *"a hit is the
cheapest elevation there is."* The batch 229 report and its HANDOFF row are
corrected and `twins.py` is deleted.

## A bucket the docs call out of reach, reopened

`elevation.md`'s section on **the zero interleaved into a shifted build** records
one cure — named locals in a *dominating block*, which requires a branch to
dominate from — sizes the affected population at ~248 functions, and states that
roughly **98 are straight-line at every site and therefore out of reach**.

Site 3 of `OvlFunc_956_200a0f0` is straight-line: deep inside a single `r == 0`
block, no branch to dominate from. It matched anyway, through a **second and
independent cure** — a pinned *whole-value* fill with the zero written first,
pinning r0 and r1 only and leaving the third argument bare.

The mechanism explains why no branch is needed. Seed `mov`s sort by
first-consumer position, so the split build puts `lsl r2` first and therefore
`mov r2` first, which is backwards. The whole-value spelling makes both seeds
depth-2, so they tie and break by **argument order** instead — and the zero is
the depth-1 statement you place by hand between the two shifts. Nothing is being
hoisted, so nothing needs dominating; the ordering is settled inside the call's
own argument list.

**This is site-specific, not a blanket rule, and the same function proves it.** A
second `__MapActor_SetSpeed(0, 0x80<<8, 0x80<<7)` with *identical arguments*
wants the opposite spelling — the split build, r0 unpinned — and taking site 3's
form there costs 108. Eighteen spellings were measured across that one block.
Barriers are the wrong tool at an interleaved site too: 7 differing at any
placement, because they reschedule the neighbouring `ldr`s as well.

So the 98 are worth re-screening rather than assumed reachable. That re-screen
is running; whichever way it lands — a general cure or a bounded one — the
"out of reach" line as written is no longer accurate.

## A blocker proved by counting

`OvlFunc_882_2009154` reached **2 differing encodings of 160**, with size, pool
order, and all 33 relocations identical in symbol *and* offset. The whole
residue:

```
rom    movs r2,#35 / ldr r3,[r0,#0x50] / mov r8,r0     / add r8,r2
ours   movs r2,#35 / ldr r3,[r0,#0x50] / adds r2,r2,r0 / mov r8,r2
```

The ROM adds **in place into a high register**. This compiler, as configured
here, never emits that form — and that is measurable rather than arguable:

| corpus | files containing `add rHIGH, rN` |
|---|---|
| ROM disassembly `.s` | **244** |
| gcc-generated `.s` (3729 total) | **0** |

Zero of 3729 is the same class of evidence as the recorded "two consecutive
`neg`" test, and belongs in the same place: **check it before spending screens
on high-register address arithmetic.** From the `.00.rtl` dump, the cause is
that gcc-2.96's expander never produces an in-place add — it routes through a
temp pseudo, combine folds the temp and the preceding copy, and the surviving
`(set r8 (plus p 35))` needs two reloads under either alternative, with reload
taking the earlier low-register one.

The control matters: under `-ffixed-r7`, with the local left as a naturally
allocated pseudo and **no pin at all**, the add still comes out
`adds r2,r2,r0`. So the residue is not an artifact of the `__asm__` pin.

The park records the whole 127→2 path so a later attempt starts there rather
than re-deriving it — including that the base pointer must be *live* at the add
(statement order alone, worth 122 positions) and that naming the stored byte in
a low register takes 5→2.

## Two levers that fire backwards

Both sharpen entries that already exist — established by grepping the doc by
concept first, which is the correction above being applied rather than merely
recorded.

**The `goto`-loop lever's signature is necessary, not sufficient.**
`OvlFunc_897_2008e30` shows the documented selection signature twice over — one
loop rebuilds a shifted constant every pass, the other reloads `0x1999` every
pass — and the lever is *actively wrong*, costing **102 of 105** against 0 for a
plain `do/while`. gcc-2.96 never hoists either invariant to begin with, because
each body contains a call and the invariant feeds only a memory
read-modify-write. The signature says the ROM did not hoist; it does not say gcc
will.

**Naming a value gcc already carries destroys the carry.** The same function
adds `-0x28f` at four sites. As four bare `+= -0x28f;`, gcc hoists it into
callee-saved r7 — exactly where the ROM keeps it. As `int d = -0x28f;`, gcc
gives it a *low* register and rematerialises it from the pool inside the body:
**102 of 105**. A named local is not a neutral way to ask.

## Twins, closed out

`OvlFunc_911_200a7ac` was the purest of the four pairs: **not one operand value
differs** across 147 instructions. Every difference was a name — three `.L`
cells and a script blob.

The trap there is worth keeping: **a `.L####` name is an address, not an
identity.** The disassembler numbers local labels by address, so the same name
recurs in unrelated overlays; a bare grep for `.L3694` finds a definition in
`rom_7ef4f4` with nothing to do with a function in `rom_79e5c0`, and taken at
face value it makes a correct mapping look wrong. What actually fixes a mapping
between parallel overlays is the address arithmetic — both declare a trio of
4-byte `.global`/`.lcomm` cells at matching relative offsets.

## A second mechanism for the cancelling length tell

`OvlFunc_936_200a008` sat at 1720/1720 bytes and 685/685 encodings while still
376 differing. Batch 228 recorded one cause for that cancellation (a widened
prologue paying for removed rematerialisations); this is a different one
entirely. Two extra `movs` were paid for exactly by two `.short 0x0000` slugs
the ROM emits to align its literal pools.

**Pool-alignment padding absorbs a small odd length change.** With two
independent causes now on record, an equal length should stop being treated as
information at all.

That function also used batch 229's stale-index warning as intended: its harness
re-verified the saved drop set against the exact source after every generator
change, and caught one edit that silently renumbered the sites and sent the
candidate to 633 differing.

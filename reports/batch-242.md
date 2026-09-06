# Batch 242 — eight for eight, and three of my own rules were wrong

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_903_200867c` | `0x0200867c` | [ovl_314_c_a_c_a_c.c](src/overlays/rom_798dc4/ovl_314_c_a_c_a_c.c) |
| 2 | `OvlFunc_896_200978c` | `0x0200978c` | [ovl_314_…_c_c_b.c](src/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c_b.c) |
| 3 | `OvlFunc_951_20084bc` | `0x020084bc` | [ovl_30_…_a_a_a_b.c](src/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_a_a_b.c) |
| 4 | `OvlFunc_953_2009688` | `0x02009688` | [ovl_30_c_c_c_a_a_c_a_c.c](src/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_a_c.c) |
| 5 | `OvlFunc_945_200c254` | `0x0200c254` | [ovl_30_…_c_a_a.c](src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_c_a_a.c) |
| 6 | `OvlFunc_968_200ca2c` | `0x0200ca2c` | [ovl_30_c_c_c_c_b.c](src/overlays/rom_7f2f14/ovl_30_c_c_c_c_b.c) |
| 7 | `Func_80b7aac` | `0x080b7aac` | [rom_b7410_a_a_c_c_a_b.c](src/rom_b5000/rom_b7410_a_a_c_c_a_b.c) |
| 8 | `Debug_FlagEditor` | `0x080291e4` | [rom_23178_a_c_c_b.c](src/rom_15000/rom_23178_a_c_c_b.c) |

Eight targets, eight matches, no parks. Entry 8 is the only NAMED symbol, so
its address was read from the linked ELF rather than from its name.

Only entry 2 depends on a flag group.

## A BLOCKER CLASS IS REFUTED, VERIFIED FROM THE GENERATED OUTPUT

`## Identical constants in ONE basic block: a controlled blocker` records
that **zero of 3235** generated `.s` files build the same constant twice
inside a block, and concludes that such a ROM is *"not reachable by any
spelling of that constant."*

Entry 5 produces exactly that shape. Its ROM has `ldr r5,=0xa01b` /
`ldr r0,=0xa01b` adjacent; the `.s` gcc generates from the landed `.c` has
`ldr r5, .L14+16` / `ldr r0, .L14+16` on adjacent lines, no label and no
branch between, both reading the single pool word `40987`.

This was checked against the file gcc actually wrote, not taken from a
report — and that mattered. The agent quoted the `=0xa01b` form, which is
disassembly syntax that never appears in generated output; a first grep for
it found nothing. Tracing the pool word and the `.L14+16` references is what
confirmed the claim.

**The measurement was true of the corpus when taken. The conclusion drawn
from it was not, and the conclusion is the part that cost parks.**
`src/non_matching/ovl_7ac2d8/20090c0.c` is parked on this class and should be
re-screened.

Why no earlier sweep found the spelling: it needs **both halves together** —
a call-clobbered pin on the literal site AND the named copy assigned *after*
that call. Either alone leaves 4 of 350, because the pin cannot beat CSE
while a callee-saved copy is still live where the pin is set.

## THE HIGH-REGISTER SCREEN COUNTS REFERENCES, NOT VALUES

Entry 3 was this round's deliberate bad bet: 31 hi-register uses, against a
recorded table where 27 meant abandoned, 17 meant parked and zero meant
elevated first try. It was briefed to park cleanly rather than grind.

It matched in twelve screens. **Thirty of the 31 uses are the same
reference** — one pseudo's reload copy, repeated once per call site. A count
of distinct high-register *values* would have said 1.

The predictor is real; its metric is wrong. High-register traffic predicts
trouble when it means **pressure**. Target selection has leaned on this screen
all week, so the correction matters beyond one function: count distinct
values, not references.

## THE `-ffixed-r7` TELL HAS A FALSE-POSITIVE CLASS

Its recorded signature — the ROM saves a high register and you do not, gap
about four — fires on entry 5 and is wrong: reserving r7 would give three
call-crossing registers where the ROM has four.

**Discriminator: count the ROM's callee-saved registers against yours.** Same
count rotated means the flag; one *more* means a live-range conflict, cured
in source by making the shorter-lived value born earlier. That is the
high-register form of the recorded "two constants in different registers
means they are simultaneously live".

Entry 3 is the second counter-example the same day, from the other side:
`-ffixed-r7` is entirely inert there **because we already save r8**, and that
inertness is what justified shipping its pin.

## AN ALL-CHEAP CALL SITE NEEDS NO ORDERING PIN

Entry 1: 133 of its 186 sites carry only bare `mov rN, #imm8` arguments, and
**not one** appears in any fixpoint from any direction. Re-running
minimisation from the 53 expensive-argument sites alone converges on the
identical 42 pins. The test prunes the sweep 186 → 53 at zero cost.

This appeared to contradict batch 240, which recorded an all-cheap site that
*did* need a pin. Checked rather than picking a side: that site was
`__Func_8092c40`, one of the two callees recorded as wanting the **descending
fill**. Entry 1 calls neither of those callees — zero sites — while the
batch-240 file calls that one seven times.

Both hold. The boundary: an all-cheap site needs no *ordering* pin, but the
descending-fill callees are reached **by name**, regardless of argument cost —
which is exactly why they are recorded by name and not by argument shape.

Entry 2 then confirmed it from the third side: CSE plus family nomination
missed four required sites there, one of them a `__Func_8092c40(1, 0)` with
both arguments cheap, wanting the descending fill.

## UNIFY A STRUCT TAG TO *ADD* A DEPENDENCE

The recorded lever splits into two struct tags to **remove** a dependence.
Entry 4 is its mirror.

Residue was one transposition worth 2 encodings. `-fsched-verbose=8` reads it
out: the pair **ties on priority** at 96, both are class 3, so
`rank_for_schedule` falls to **dependent count** — the narrow tag gives the
halfword store five dependents against the shift's seven, and it loses.
Declaring the two neighbouring word fields **in the same tag** makes them
conflict, the store gains exactly those two insns, seven against seven ties,
and insn order picks the ROM's.

**The lever is the struct definition, not the access spelling**: wide tag with
a cast is 0, narrow tag with the same cast is 2.

And the standing alias hazard inverts between two functions in one batch —
`-fno-strict-aliasing` is 281 differing on entry 4 and byte-identical on
entry 6.

## THE CALLER'S OWN RETURN TYPE REACHES sched2

Recorded: `pop {r1}` means non-void, and `return x;` is not the spelling.
Both hold. The addition is a second effect — declaring `s32` with **no**
return statement also fixes an argument-setup order forty bytes earlier, at a
call unrelated to the return value.

From `-fsched-verbose=8`: the `ldr` and the `add` tie at priority 65, because
`arm_adjust_cost` returns 1 for any data dependence into a `CALL_INSN`, so
load latency buys nothing. `rank_for_schedule` decides on dependent count,
and the **epilogue** is the second dependent — the return `unspec_volatile`
takes a dep on the last writer of whichever register `thumb_exit` pops into.
VOIDmode offers r0; a size-≤4 return offers r1.

So the epilogue's scratch register decides argument order at an unrelated
call. The existing entry justifies the spelling by a spurious `mov r0,#0`,
which entry 7 does not have at all.

## A FLAG RULE WITH THE PASS NAMED

Entry 2 is the batch's only flag-dependent match, and `GCSE_CFLAGS`'s own
Makefile comment demands the diff be read first.

`cse.c:6572` ends a cse block only at a `CODE_LABEL`, so a message id and a
later `m + 1` share ONE cse block across the conditional jump, and
`related_value` produces the ROM's `add`. It is **gcse's cprop** that
substitutes across the CFG edge and kills the register. So cse *wants* the
ROM's form here and gcse takes it away — a gcse rule, not a cse one. Reduced
to two eight-line probes; six other cse-family flags are byte-identical to
the default.

Two points of discipline. **The split is what keeps the flag honest** — the
`.s` held four functions and the rule now covers a TU with one, leaving three
siblings on the default. And **`volatile` is unavailable, not declined**: it
costs a stack frame and 526 encodings, so the recorded preference for a
spelling over a rule still stands; this function simply cannot pay it.

This is also the converse of the batch-239 cprop rule. That says a name is
lost to cprop *within* one basic block and a branch is required to keep it.
Here the **branch is what loses it**. The two are halves of one boundary.

## THE SPLITTER REFUSED A CUT, AND WAS RIGHT

Entry 6's `.s` carries five functions and a real `.section .data` of 46
blobs. `split_s.py` refused: a local label `.L52cc` is a data table defined
in the `.data` section and referenced from a function landing on the far side
of the cut, and a `.L` symbol does not reach the object symbol table.

Fixed with `.global .L52cc` and **gated alone first** — a `.global` emits no
bytes, so `make compare` had to stay green before the split, keeping the two
changes separable. It did.

The `.data` group then came out as three lines, one per piece, with the
section landing in the piece that holds all 46 blobs and the label. The other
two carry `.data` lines for sections they do not have — the documented
harmless case, and safer than hand-repointing a single line, because **an
unmatched `.ld` entry is not an error**: a wrong aim there drops every script
in the overlay silently.

## Two levers whose direction is the opposite of the usual

**The loop addend must be the LITERAL, not the named local** (entry 6).
Naming it is 175 differing and *two instructions short*: the named pseudo
loses global-alloc priority to the hot counter, gets no register, and reload
rematerialises the constant inside the loop. The "shorter output means
something live is missing" signal points at it, and the cure is to **stop**
naming the value.

**The high-register pins are harmful and deleting them is the lever** (entry
6). The template's r8+r10 recipe measures 154; r8 alone 66; r10 alone 154;
neither **zero**. gcc reaches those registers unaided in call-saved order, and
forcing them reserves them function-wide. Hard-pinning two pointers also
produced **wrong code**, gcc reusing a pinned register for the counter.

## Method notes

**Declaration order is completely inert on entry 6** — all **5040**
permutations of seven locals score identically. Worth recording because it is
the cheapest thing to reach for when two pointers land in swapped registers,
and here it was a symptom of the addend lever rather than a cause.

**`4 + (x == 1)` and `5 - (x != 1)` canonicalise to identical RTL, and only
one matches** (entry 7) — because only the first lets the `1` be the *same
RTL constant* as a live initialiser, which is what the ROM's `eor r3, r5`
actually is. Forcing the sharing by hand is 38 differing.

**A naming discrepancy left as found**: the annotation above entry 8 reads
`@ RunDjinnListScreen`, contradicting the exported symbol. The `.export_func`
entry is authoritative and the comment is a shape guess; recorded rather than
"corrected".

**Neither of entries 7 and 8 got its match from its template**, and entry 7's
template lever is actively wrong there because that ROM re-calls the accessor
its template caches.

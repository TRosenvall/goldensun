# Batch 32 — 9 functions, an eighth blocker class, and two templates that pay

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–31 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted — 96 overlays
compared byte-for-byte and `goldensun.gba: OK`. Every address below was read
back from the linked overlay ELF with `nm`, and the five new `message.sym`
symbols read back from `stage1.o`.

## Read this first: pool loads come first, and no lever moves them

**An eighth blocker class, four members.** Within one argument block gcc-2.96
emits every literal-pool load ahead of every `mov`, whatever order the source
writes them in. The ROM emits them in source order.

It shows up in two disguises, and the second is the expensive one:

    rom    mov r1, #0x66 / ldr r2, =0x4b6 / mov r0, #0
    ours   ldr r2, =0x4b6 / mov r1, #0x66 / mov r0, #0

    rom    mov r0, r5 / ldr r1, =0xcccc / ldr r2, =0x6666
    ours   ldr r1, =0xcccc / ldr r2, =0x6666 / mov r0, r5

The second reads as a misplaced `r0`, so it looks exactly like the class the
**declaration lever** retires — and batch 31 had just widened that lever from
"where `r0` lands" to "the order of the whole argument block", which makes the
misreading more likely, not less. It is a different thing. In both cases above,
every *pooled* operand has moved ahead of every non-pooled one and their
relative orders are otherwise untouched.

**Before spending screens on the declaration lever, check whether the displaced
operands are exactly the pooled ones.** If they are, stop:

* the mismatching callee declared, undeclared, and with a parameter widened —
  identical output
* the *preceding* callee undeclared, and its return type swept through `char`,
  `short`, `unsigned`, `void *`, `unsigned char *`, `long long`, `float`,
  `double` — every non-`void` return goes from 2 differing positions to 5,
  which is the return value being kept live, not a reordering
* `-fno-schedule-insns`, `-fno-schedule-insns2`, `-fno-peephole`,
  `-fno-force-mem`, `-fno-caller-saves`, `-fno-expensive-optimizations`,
  `-fno-cse-follow-jumps` — all byte-identical
* naming the values as locals immediately before the call in the ROM's order,
  the stack-arg-pair lever's trick, for one value and for all three — no change

`src/non_matching/overlays/pool_load_first.c` has the full negative result.

**A decision for you.** An inherited fakematch, `OvlFunc_883_2008fbc`
(`src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_b.c`), has the *first* of
those two shapes exactly — same callee, same `0x66`, same `0x4b6` — and reaches
it by pinning `r0` and `r1` with `register … __asm__("r0")` plus an empty
`__asm__ volatile` barrier. So the shape has been solved once, and only that
way.

We left these four as assembly rather than adding four more fakematches. The
reasoning: a fakematch records "we could not find the source construct" in a
form that compiles, and four more in one family makes the family look solved
when it is not. **That is a judgement call and it is yours to reverse** — the
park note lists every member and the change is mechanical.

## Two templates paid for themselves

Five of the nine came out of *sweeping `asm/` for more members of a shape that
had just matched*, not off the candidate ranker, and four of those five matched
on the first screen. Both sweeps are worth repeating:

| shape | members left in `asm/` | first written up in |
|---|---|---|
| interaction halfword at `[iwram_3001ebc]+n` | 151, of which 15 under 40 instructions | batch 31 |
| three-message prompt (`base`, `base+1`, `base+2`) | 6 | this batch |

The ranker is built to find *tractable* functions. A shape that has already
matched once is a stronger signal than any of its heuristics, and nothing in the
tooling looks for one. That is the cheapest improvement available to whoever
picks this up next.

## `narrow_constant`, inverted: now four members and a rule

Batch 31 reported this running the opposite way to the whole class — gcc
**pools** a small constant the ROM builds with a `mov`, because the destination
is narrow and gcc narrows the *operation*, not the operand. Two more members
here (`OvlFunc_948_2009120` and its twin) make it four, and it is now a rule
rather than an observation:

    *(short *)p = 0;      ->  ldrh r3, .L1       (a halfword pool entry)
    int z = 0; *p = z;    ->  mov r3, #0

**The symptom looks exactly like the pool tell** — ROM `mov`, ours pool load —
which would send a reader hunting for a symbol that does not exist. The
discriminator is the direction: the pool tell is the ROM pooling what gcc could
`mov`; this is gcc pooling what the ROM did `mov`.

## Live at the right moment, in both directions

`OvlFunc_959_2009718` reads the iwram base in the **prologue** and keeps it
across a call. Written at the point of use, gcc reloads it afterwards and the
streams part at instruction three.

That is the stack-arg-pair question asked backwards. There the fix is to stop a
value being live too early (batch 30: *"being live earlier is not the same as
being live at the right moment"*); here the fix is to make it live earlier. Both
are the same rule — **the C has to put the value where the ROM's compiler could
see it** — and a reader who has only met one half will apply it the wrong way.

## A register swap between two near-identical functions proves nothing

`OvlFunc_952_20080c8` puts the message base in `r5` and the actor slot in `r6`.
`OvlFunc_952_200bfc4` is the same function with a different id and does it the
other way round. **The C is the same shape for both.** The allocation follows
which value the compiler happened to see first; nothing in the source controls
it.

Worth stating because the natural reading is the opposite one — the registers
differ, so the sources must differ, so go looking for the difference. There
isn't one.

## Removing a prototype is a lever

The tree has been writing the declaration lever as *"add a declaration"*. The
subtractive form matters just as much. All four three-message prompts here need
`__Func_8092c40` left **undeclared**; declared, gcc builds its arguments
`mov r0 / mov r1` and the ROM has them the other way round.

An existing sibling, `src/overlays/rom_7ebdfc/ovl_30_c_c_a_c_b.c`, was already
relying on this for the same callee without saying so — which is exactly how a
lever gets lost.

## Functions

| function | address | overlay | note |
|---|---|---|---|
| `OvlFunc_895_2008200` | `0x02008200` | rom_78dee8 | interaction halfword, first template hit |
| `OvlFunc_952_20080c8` | `0x020080c8` | rom_7d768c | three-message prompt |
| `OvlFunc_952_2008524` | `0x02008524` | rom_7d768c | twin |
| `OvlFunc_952_2008564` | `0x02008564` | rom_7d768c | twin |
| `OvlFunc_952_200bfc4` | `0x0200bfc4` | rom_7d768c | prompt, actor line in both arms |
| `OvlFunc_950_2008760` | `0x02008760` | rom_7d5838 | prompt inside a cutscene |
| `OvlFunc_948_2009120` | `0x02009120` | rom_7d30e0 | narrow_constant inverted |
| `OvlFunc_948_200915c` | `0x0200915c` | rom_7d30e0 | twin |
| `OvlFunc_959_2009718` | `0x02009718` | rom_7e7574 | self-unregistering task |

Five symbols added to `message.sym`, all by value with no semantic claim, the
same way `_MSG_25b8` was: `_MSG_1ff1`, `_MSG_22a8`, `_MSG_22ab`, `_MSG_22a3`,
`_MSG_1fbb`.

## Parked

**`OvlFunc_882_2008398`, `OvlFunc_882_20083cc`, `OvlFunc_882_2008400`**
(rom_77dd1c) and **`OvlFunc_969_2009280`** (rom_7f6e64) — the pool-load-first
class above.

**`OvlFunc_939_20087f4`** (rom_7c460c) — 27 against 28, on something else.
gcc loads the fall-through constant of a two-way pick **before** the compare and
then conditionally overwrites it, which inverts the branch and costs a `mov` to
get the merge into `r0`:

    rom    cmp r0,#0 / bne .L1 / ldr r0,=0x1be2 / b .L2 / .L1: ldr r0,=0x1ba5 / .L2: bl
    ours   ldr r3,=0x1be2 / cmp r0,#0 / beq .L1 / ldr r3,=0x1ba5 / .L1: mov r0,r3 / bl

Four source forms and five flags tried. **The one to start from next is the
fourth**: calling `__MessageID` in both arms gets the ROM's branch structure
exactly right — `cmp`/`bne`, fall-through constant first, join in the right
place — and then fails to cross-jump the identical one-instruction tail, coming
out one instruction long. The outer arms are not the problem; the ROM duplicates
`mov r0,#0x12 / mov r1,#0 / bl __ActorMessage` in both of them and gcc
reproduces that happily.

## Counts

296 functions elevated in total. 2,999 hand-written functions remain in `asm/`
of 5,714 — the first time the remainder has been under three thousand. 95 parked
functions, plus 6 files that document blocker classes rather than individual
functions.

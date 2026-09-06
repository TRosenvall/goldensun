# Batch 246 — the hiv=4 reject was wrong, four times out of four

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_945_200b8ac` | `0x0200b8ac` | [ovl_30_…_a_c_a_b.c](src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a_c_a_b.c) |
| 2 | `OvlFunc_928_2008f30` | `0x02008f30` | [ovl_314_c_c_a_c_c_c_c_c_a.c](src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_c_c_c_a.c) |
| 3 | `OvlFunc_945_2009b34` | `0x02009b34` | [ovl_30_…_c_c_a_a_b.c](src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a_a_b.c) |
| 4 | `OvlFunc_943_2008ca0` | `0x02008ca0` | [ovl_30_c_a_a_c_a_c_a_c_a_a_a_b.c](src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_a_a_b.c) |

## Parked

None. Four targets, four matches.

Three of the four are first-of-N in their `.s` and needed a split; each split
was proved byte-neutral as a layout-only build before the `.c` landed. None
needs a flag group.

## THE EXPERIMENT

`templated.py` reports `hiv`, the count of DISTINCT high registers a function's
body mentions, and the recorded reading was that `hi` high with `hiv` at 3 or 4
"is the real reject" — a function needing more values live than the low
registers hold is where the allocation-order parks come from. That reading had
never been tested against a deliberate sample; it had only ever been used to
*skip* functions, which cannot falsify it.

This round took four hiv=4 functions on purpose, including the worst case on
the board. **Four for four.** Two more in the follow-up round (batch 247) make
it six for six.

## WHAT hiv=4 ACTUALLY MEANS

The same mechanism appeared in all four, and it is not the one the reject
assumed.

**gcc reaches the ROM's high-register assignment UNAIDED.** On `200b8ac` the
first compile from plain C emits `mov fp, r2 / mov r9, r3 / mov sl, r2 /
mov r8, r3` with nothing in the source asking for it. On `2008f30` — the
control, picked small so it could be characterised completely — the plain first
transcription already emits the eight-instruction wide prologue, all four
`mov rN, r3` definitions **in the ROM's order**, all thirteen reload copies
including per-site fill order, and the seven-instruction wide epilogue.
**All 25 high-register references were correct before anything was tried.**

What gcc gets wrong is not the count but the **tenants**. It commons values the
ROM rematerialises, and then has nothing left for the values the ROM holds:

> **YOU-HAVE-MORE-REGISTERS IS A COMMONING TELL.**

On `2009b34`, unpinned gcc spends **seven** callee-saved registers where the ROM
spends six, and the first differing encoding is the push mask itself. Seven
candidates, seven registers, three of them the wrong three. The cause is
candidate supply, not allocation.

This is the mirror of a recorded rule. A sibling records *"SAME COUNT, rotated →
flag; ONE MORE → live range"* — that is the ROM-has-more direction. This is the
we-have-more direction, and it wants the opposite remedy.

## THE CURE IS EVICTION, NOT PLACEMENT

> **THE PIN IS AN EVICTION DEVICE, NOT AN ORDERING ONE.** Pin the ROM's
> *rebuilt* constants to r0–r2 so they leave the commoning pool. Leave the ROM's
> *held* values as plain locals. The allocator then picks the ROM's set itself.

On `2008ca0` that is **273 differing → 16 in one step**. Once the wrong three
values go, the seventh register vanishes by itself, because the seventh tenant
does.

**Not one of the four ships a hard-register pin on r8–r11.** `2009b34`'s final
matching file contains three register declarations and they are r0, r1, r2.
The lever that solves a high-register problem never mentions a high register.

## THE METRIC COUNTS BOILERPLATE

Two functions reported this independently, and both reported the same number:
**8 of the high-register references are prologue/epilogue save/restore pairs**
that any four-high-register frame carries. `2009b34`'s 26 references are really
18; batch 247's `20089dc` is 31 minus 8 = 23.

So the counter should **subtract save/restore pairs**, not have its threshold
nudged. `hi` counts references, not values, and eight of them are noise in every
function of this shape.

## `2008f30`: FOUR DISTINCT HIGH-REGISTER VALUES ARE FREE

The control's answer is the starkest. Its four values are ordinary — a task
function address born first and living across eight calls, plus an x/y/z triple
shared by three consecutive calls. Ordinary values, ordinary lifetimes, and the
assignment is **non-monotone**: the first-born value takes r11, the *last* slot
of `REG_ALLOC_ORDER`. The recorded unaided cases are all *single* values; this
is four.

Its three levers were elsewhere entirely: a first-use pin at a twice-used flag
id, an `int` local for a halfword store constant, and the loaded pointer named
and assigned **before** that constant. That last is a transcription trap worth
keeping — **the ROM emits the constant first and the pointer load second, and
the source that matches does the opposite**, with sched2 hoisting afterwards.
Transcribing the emitted order costs 10.

One recorded lever measured **actively wrong** rather than inert: a narrow
`unsigned short` for the halfword constant is 59 differing where `int` is exact.

## AN HONEST UNEXPLAINED RESULT

In `2008f30`, **r7 is never allocated** — in the ROM and in our matching output
alike — though six values are live across calls. Two explanations were tested
and **both fail**: gcc's own `.17.lreg` dump shows all 21 pseudos preferring
`LO_REGS` at cost zero, so it is not an operand-class story; and r7 is not
reserved as a frame pointer, since the template compiles to
`push {r5, r6, r7, lr}` under identical flags.

Real, reproducible, unexplained, and recorded as such. It also means the tidy
story that a fourth long-lived value reaches r11 only after r8, r10 and r9 are
taken does not describe every function.

## TWO BOUNDS ON THE BARRIER

`2008ca0` bounds an entry stated twice since batch 207 — *"zero means the
barrier is available, non-zero means it is not, whatever the residue looks
like"*. This function has 23 high-register references across four values and a
full high-register prologue, and **eleven barriers land it byte-exact**. The
batch-207 mechanism holds; its boundary is one step early. A barrier hurts when
the split **shortens a range the ROM's allocation depends on**, and here all four
values are live across forty-odd calls, so every barrier falls strictly inside
all four ranges.

> **Refinement: PIN FIRST so the allocation is the ROM's, THEN barrier for
> order.** Both batch-207 specimens were barriered while the allocation was
> still gcc's own.

**And the barrier count is a hard budget set by minipool reach.** An empty
`__asm__ volatile("")` emits nothing, but `shorten_branches` still charges it a
full instruction, which pushes the pool's first entry out of reach of the
end-of-function barrier, so `arm_reorg` dumps it early with a branch over it.
Measured by adding no-op barriers to the *finished* file:

| barriers | result |
|---|---|
| 11, 12, 13 | exact |
| 14, 15, 16, 17 | **the same 12 differing**, code stream unchanged |

So a **correct** lever can read as "worse" purely because it tipped the pool.
Diagnostic: first difference jumps to the last few instructions, length +2, a
bare branch-to-next-label at the tail. The budget is fungible — removing a
barrier *anywhere* buys the room back. This cost three intermediate candidates
before it was understood.

## LEVER INVERSION, A FOURTH TIME

`2009b34` measured its template's fill lever at the same 22 pins:

| spelling | result |
|---|---|
| ascending | **exact** |
| ROM-window replay | **exact** |
| descending | 52 |
| whole-value `q1 = 0x81 << 1` folded into the initialiser | 224, and **four bytes short** |

That last row is the shorter-output tell at its cleanest: folding the shift hands
gcse a plain `CONST_INT`, the seventh register comes back, four bytes vanish.
**The split shift is what keeps the constant rebuilt.**

The family verdict is now precise: the commoning-set mismatch and the pin remedy
**are** shared across this overlay's cutscene family — three landed siblings show
it — but the **fill direction is not transferable**.

## "N PINS IS A SIZE, NOT A SET", CLEANLY

`2009b34`: greedy removal reaches 22 forward and 22 reversed with **different
sets**, the two kept sites being the two ends of one CSE class, either alone
exact.

`2008ca0`: 57 blocks screened individually, 40 inert, 17 load-bearing — and
unlike a sibling, the **union** of all 40 inert removals also passes.
Front-and-back greedy stripping converge on the same 17.

## Gate

`make clean && make -j8 && make compare` green. Every address above checked
against the linked ELF with `tools/checkaddr.py`.

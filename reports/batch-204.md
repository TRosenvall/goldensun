# Batch 204

Five elevated. The batch's most useful output is not an elevation — it is a
**correction to batch 203**, the process change that followed it, and the
housekeeping that change immediately turned up.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_949_200828c` | `0x0200828c` | [ovl_30_c_c_a_a_c_c.c](src/overlays/rom_7d4af4/ovl_30_c_c_a_a_c_c.c) |
| 2 | `OvlFunc_930_20088a8` | `0x020088a8` | [ovl_30_…_a_c_a_c.c](src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_c_a_c.c) |
| 3 | `OvlFunc_953_2008648` | `0x02008648` | [ovl_30_…_c_c_a.c](src/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_c_a.c) |
| 4 | `OvlFunc_922_2009b1c` | `0x02009b1c` | [ovl_30_…_a_c.c](src/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_c_c_a_c.c) |
| 5 | `OvlFunc_966_200810c` | `0x0200810c` | [ovl_30_c_c_a_c_c_a.c](src/overlays/rom_7f148c/ovl_30_c_c_a_c_c_a.c) |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## A PUBLISHED FINDING THAT WAS ALREADY IN THE TREE

Batch 203 reported "an unsigned counter blocks gcc's loop reversal" as a
finding. It had been recorded since August, in
`src/non_matching/ovl_7ac2d8/200adcc.c` — the park for the **twin of the very
function it was measured on** — and stated better there: the ROM's `bls` *is*
the evidence, since an unsigned branch on a loop counter means the counter is
unsigned.

That park was also better work. It screens at **5 of 24** where the batch-203
candidate screened at 9 and 10, because it additionally knew that assigning the
counter *before* the source pointer is worth four instructions. Its diagnosis of
the remainder — **constant derivation**, gcc deriving the source pointer from
the save target rather than taking a third pool entry — is different from and
better than the register-role rotation batch 203 claimed. And the batch-203 park
asserted it covered both twins while that older park already existed.

The cause was procedural: the pair was triaged out of `tools/shape_groups.py`
and `src/non_matching` was never grepped for their names first. **The tree's own
rule — locate a function by NAME — applies to checking whether a park exists,
not only to finding `.s` files.**

Corrections are in place: the batch-203 park now carries the better body and
defers to the older one, and both `reports/batch-203.md` and its HANDOFF row
carry the correction inline.

### The re-attack that park proposed also fails

It named one untried idea — give the three addresses a common symbolic base.
Measured: **28 lines against the ROM's 24**, and a source-derived-from-
destination variant gives 26. Both far worse than the 5 already recorded.

The reason is now written down: the ROM wants three *independent* pool entries,
so every construct that relates the addresses pushes gcc further toward deriving
them, which is the defect itself. **The idea was pointing the wrong way.**

## THE PROCESS CHANGE PAID IMMEDIATELY

Checking for an existing park *before* working a candidate is now the first
step. Applying it, **every** candidate the shape groups offered had a park — so
they were read first, which is what produced three of this batch's elevations
and turned up two housekeeping problems:

**Five stale parks removed.** `ovl_780898/2008dc0.c` opened "THIS FILE STANDS FOR
A GROUP OF SEVEN" and described the same blocker as `arg_interleave_flat.c`
under a different name; all seven are elevated. Four more named only functions
elevated in batches 198–203, three of which I elevated myself and left the parks
behind.

These are not harmless. Searching the park directory for a blocker shape returns
them beside live parks, so **a candidate list built by grep is polluted with work
already done** — which is exactly how they were found. `tools/stale_parks.py`
exists for this and should run at the end of a batch, not only when something
looks wrong.

**27 functions appear in more than one park file**, per that same tool. A park
found by grep is not necessarily the only park for that function — the hazard
behind the batch-203 error, now recorded where the next reader meets it.

## THE BARRIER LEVER, BOUNDED

`do { } while (0)` took `src/non_matching/overlays/200db90.c` from **11 differing
to 4**, and naming one loaded value took it to **2** — a park whose blocker was
described as scheduling and which had never tried a scheduling barrier.

On `src/non_matching/ovl_7b6668/2008d0c.c`, whose park describes its blocker in
nearly the same words, the barrier is **inert in three forms**.

The difference is what the barrier has to separate. In `200db90` the hoist
crossed **statement boundaries** — loads belonging to the last statement had
migrated up past three stores. In `2008d0c` the two instructions are operands of
**one expression**, with no boundary to place a barrier on. **The barrier orders
statements, not the operands within one**, and the cheap test is whether the two
instructions sit on opposite sides of a statement boundary.

## Smaller results

**A pin orders two independent movs but not two independent loads.** Batch 197
ordered three argument registers by pinning because each was a `mov` of an
immediate, and a pin decides where that materialisation happens. `200db90`'s
last two instructions are a pool load and a memory load; naming their destination
registers says nothing about when the loads issue, and five forms are
byte-identical against them.

**A `-1` PAIR wants grouping, not interleaving.** Batch 192 established that a
`-1` *triple* needs its assignments and negations interleaved, because three
registers receiving the same value have nothing to order them. Two of this
batch's elevations carry a *pair*, and it wants the opposite — written grouped in
the ROM's own order with the registers pinned. That rule is what to do when
ordering fails, not a shape to apply everywhere.

**`200a69c` moved 29 → 18 and is still a line long.** Pinning both
`__MapActor_SetSpeed` sites forces the rebuild gcc was CSEing across the
intervening call, but the prologue is still `push {r5, r6, lr}` against the ROM's
`lr` alone — so something *else* is live across a call, and the CSE was not the
whole cause. The park now says to find that before spending screens on the fills.

## Two process slips, both mine

**A commit landed with only deletions.** `git add` was given an
already-`git rm`'d park path alongside the new files, and it aborts the entire
invocation on a bad pathspec rather than adding the rest. The build stayed green
because the `.c` was on disk; only the commit was wrong. Fixed in a follow-up.
Second occurrence this session — the rule is to never hand `git add` a path
already removed.

**A verification was not observed before its commit.** In batch 203 the
`make compare` and address check for one function were issued in the same command
as a backgrounded clean rebuild, so they landed in a log rather than in front of
me. Both passed, but the gate is meant to be read before committing.

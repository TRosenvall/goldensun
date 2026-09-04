# Batch 210

Five elevated, three exact on the first screen. The batch's result is a **hazard**
rather than a lever: the pin idiom this tree leans on hardest has a failure mode
that produces silently wrong code, and it took a function one instruction short
to notice.

## Function breakdown

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_924_2009340` | `0x02009340` | [ovl_f84_…_a_c_b.c](src/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a_c_b.c) |
| 2 | `OvlFunc_928_2009060` | `0x02009060` | [ovl_314_…_c_c_b.c](src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_c_c_c_b.c) |
| 3 | `OvlFunc_882_2009828` | `0x02009828` | [ovl_30_…_c_a_a.c](src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_a_a.c) |
| 4 | `OvlFunc_907_2008404` | `0x02008404` | [ovl_30_…_c_c_b.c](src/overlays/rom_79b154/ovl_30_c_a_a_c_c_c_c_c_c_b.c) |
| 5 | `OvlFunc_883_20092bc` | `0x020092bc` | [ovl_30_…_c_c_c.c](src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_c_c.c) |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## A PIN ASSIGNED BEFORE A CALL AND USED AFTER IT IS SILENTLY DROPPED

The rematerialisation lever works *because* r0–r3 are call-clobbered: a constant
pinned there cannot survive a `bl`, so gcc must rebuild it. When the ROM does
**not** rebuild it, that same fact deletes the assignment outright.

On `20092bc`'s tail, written with r1 pinned and assigned before a statement whose
expression contains a call:

    q1 = (int)gScript;                                  <-- assigned here
    *(void **)(__MapActor_GetActor(0x16) + 0x6c) = fn;   <-- the call clobbers r1
    q0 = 0x16;
    __MapActor_SetBehavior(q0, q1);                      <-- used here

**The `ldr r1` is not emitted at all.** The callee is entered with r1 never
written. gcc sees a dead store to a hard register and deletes it; unlike a
pseudo, a pin is not something it can reload, so nothing replaces it.

**The tell is a function one instruction SHORT with an argument register never
written.** A length undershoot normally reads as gcc commoning something — that
is how I first read it — and here it meant an argument had vanished. Read *which*
instruction is missing, not the count.

The rule, now in `docs/elevation.md`: **a pinned register's live range must not
cross a call.** Assign it after the last call before its use, or don't pin that
site. On this function plain literals with no pin are exact.

This is the sharpest reminder yet that the pin is a *declared fakematch* with
teeth. Everything else it does is visible in the diff; this failure removes an
instruction the source clearly asks for, and only `make compare` or a careful
read of the missing line catches it.

## FIX THE ALLOCATION BEFORE ADDING A BARRIER

`2008404` showed two residues at once: a store whose address and value sat in
each other's registers, and a pool load issued one instruction ahead of the
previous statement's `strb`.

    do { } while (0) wall            4 of 138  (load fixed, store still swapped)
    pin the store's two operands     EXACT     (both fixed, no wall)

The load was not being hoisted for its own reasons — it was filling a slot the
wrongly-allocated store had left open. **A barrier that papers over a slot is how
a function ends up carrying scaffolding it does not need.** Fix what the
allocator got wrong first, then see what is left.

## AN EXISTING RULE BOUNDED

`docs/elevation.md` records that for two constants competing for callee-saved
registers, "the second-assigned wins the lower-numbered register." On `2009060`
that **predicts the ROM and gcc does the opposite**: the ROM holds `0` in r6 and
`0x80 << 24` in r5, and gcc gives the first-assigned r5.

Declaration order is inert — swapping the two declarations and merging them into
one `int z, v;` both give the identical 10 of 83. Pinning both settles it, which
is cheap here because r5 and r6 are **low** registers; the batch-207 pressure
boundary is about r8–r11 and does not apply.

## THE SYMBOL-VERSUS-INTEGER CONTRAST, CONFIRMED

Batch 209 found a message base that needed no pin because it was a linker symbol.
`2009828` is the control: its base is the plain integer `0xe74` with `m += 5`, and
without `register int m __asm__("r5")` constant propagation folds both uses into
their own pool words. Same shape, opposite answer, and the spelling of the id is
what decides it.

Its pinned load then hoisted one statement early and took **one** wall — the
batch-207 rule holding, since it had crossed one statement rather than two.
Bracketing with two also matches, so the minimal form is kept.

## A CROSS-JUMPED TAIL NEEDS NO HELP

`2009340`'s ROM sets r0/r1/r2 three different ways in three arms of an if-chain,
then jumps to one shared `lsl r2, #2 / bl __MapActor_TravelTo`. Written as three
separate calls, each with its own pinned fill and each spelling the shift at the
call site, gcc cross-jumps the tail itself.

The tempting simplification — hoist one call after the chain and pass variables —
would not match, because **the three arms differ in which register is filled
first and a single call site cannot express three orders.**

## SELECTION

All five came from `templated.py` ranked on template quality and filtered to
`hi == 0`. `crossed.py` flagged three of them during selection, and in every case
the barrier went into the first draft from the listing rather than being
discovered by iterating — six crossed sites across the batch, none of which cost
an attempt. That is the third batch running where the pre-filter's verdict is
used as a route.

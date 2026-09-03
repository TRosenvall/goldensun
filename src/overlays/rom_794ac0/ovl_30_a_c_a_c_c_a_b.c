/* OvlFunc_899_20085bc  --  0x020085bc
 *
 * Cut from the head of goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_a_c_c_a.s;
 * the second function stays in _c.s. No file-scope data -- the only pool is
 * this function's own, mid-body behind a branch -- so the split is a pure text
 * cut, verified byte-neutral before this landed.
 *
 * A cutscene beat that advances a six-state step counter and picks the next
 * behaviour script from one of two tables, walking the counter up or down
 * depending on which way the actor is facing.
 *
 * A SHARED THIRD BLOCK IN A TWO-ARM ROM IS EVIDENCE OF CROSS-JUMPING, NOT OF A
 * SHARED SOURCE BLOCK. The ROM is a plain two-level if/else with BOTH inner
 * arms written out in full:
 *
 *      if (RANGE) { if (step <= 2) A; else B; }
 *      else       { if (step >  2) A; else B; }
 *
 * gcc cross-jumps the two B copies -- identical blocks with the same successor
 * -- which produces exactly the ROM's layout, and it does NOT merge the two A
 * copies even though they are byte-identical too. One merge happened and one
 * did not. Hand-performing the merge with a `goto`, which is what I tried
 * first, is measurably worse. Write the duplicate and let gcc merge it. The
 * same mechanism merges both arms' stores into the join, so the increment
 * belongs INSIDE each arm and a named join variable is wrong.
 *
 * ARM ORDER WAS WORTH THREE CONDITION CODES HERE, NOT THE WHOLE DIFF, and the
 * negative result is the useful part. My working hypothesis was that a large
 * deficit means the arms are in the wrong order; it was not. Reordering the
 * arms on the wrong-tailed candidate bought ONE instruction. A LARGE
 * ALIGNED-REGION DEFICIT STARTING EARLY IS NOT DIAGNOSED BY BRANCH POLARITY --
 * polarity errors show up as isolated single-instruction replacements.
 *
 * BLOCKER 1b, AND A CORRECTION TO THE RECORDED TABLE. The notebook says small
 * values in roughly 1..0x7fff need no local because they emit `mov`. Measured
 * here, that is wrong for anything stored through a halfword lvalue: storing a
 * literal 5 through a `short *` pools it and costs four bytes of pool, and only
 * an `int` local gives the ROM's `mov`. Both the 5 site and the 0 site need it.
 * The honest rule for this compiler is that EVERY literal stored through a
 * halfword lvalue pools and the escape is always an `int` local. The recorded
 * counter-examples were struct members of a wider object, which may reach a
 * different pattern; that wants a corpus re-check before the section is
 * rewritten.
 *
 * Both `int` locals must sit inside their own if-body: both at the top of the
 * function costs 5 regions, one at the top costs 6.
 *
 * THE POOLED HImode ZERO NEEDS NO CONSTRUCT. It falls straight out of the
 * natural re-read -- store zero, then test the field again -- because cse
 * store-forwards the second read to an HImode constant, which pools. The
 * unsigned-short struct or array trick recorded elsewhere is unnecessary here;
 * the ROM's own re-read is the natural spelling.
 *
 * THE FIELD AT +0x64 IS READ SIGNED. actor.h declares it `u16`, but the ROM
 * uses a sign-extending load and the function tests it for negative, so this
 * overlay treats it as a signed step index -- it cycles 0..5 and indexes two
 * int tables, which does not read like a facing angle. Reaching it through a
 * `short *` matches; using the header's field directly does not. Recorded, not
 * resolved.
 *
 * The [cse] marker was a false positive again, and the flag is actively harmful
 * here -- 6 aligned regions worse on the matching source.
 */
#include "gba/types.h"
#include "actor.h"

/* The two step tables the ROM indexes are file-local `.L` data in
 * asm/overlays/rom_794ac0/ovl_30_c_c_c_c_c_c_c.s, already `.global` there. */
extern int L64a8[] __asm__(".L64a8");
extern int L64c0[] __asm__(".L64c0");

extern Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80925cc(int slot, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int b);
extern void __MapActor_SetSpeed(int slot, int x, int z);
extern void __MapActor_SetBehavior(int slot, int b);
extern void __MapActor_WaitScript(int slot);

void OvlFunc_899_20085bc(void)
{
    Actor *a;
    short *step;

    a = __MapActor_GetActor(0x18);
    __CutsceneStart();
    __Func_80925cc(0x18, 2);
    __MessageID(0x12ac);
    __ActorMessage(0x18, 0);
    __MapActor_SetSpeed(0x18, 0x80 << 11, 0x80 << 10);
    if ((u32)((a->facing & (0xf0 << 8)) - 0x5000) <= (u32)(0xc0 << 7)) {
        step = (short *)&a->goalFacing;
        if (*step <= 2) {
            __MapActor_SetBehavior(0x18, L64a8[*step]);
            *step = *step + 1;
        } else {
            __MapActor_SetBehavior(0x18, L64c0[*step]);
            *step = *step - 1;
        }
    } else {
        step = (short *)&a->goalFacing;
        if (*step > 2) {
            __MapActor_SetBehavior(0x18, L64a8[*step]);
            *step = *step + 1;
        } else {
            __MapActor_SetBehavior(0x18, L64c0[*step]);
            *step = *step - 1;
        }
    }
    if (*step > 5) {
        int zero = 0;
        *step = zero;
    }
    if (*step < 0) {
        int five = 5;
        *step = five;
    }
    __MapActor_WaitScript(0x18);
    __CutsceneEnd();
}

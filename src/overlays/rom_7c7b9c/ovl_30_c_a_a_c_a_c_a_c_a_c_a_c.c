/* OvlFunc_943_2009c14  --  0x02009c14    EXACT, ON THE FIRST SCREEN
 *
 * From asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_c.s.  91
 * instructions, 97 encodings, 248 bytes.  A two-actor cutscene: arm the next
 * map through the iwram pointer, fade in, pose and walk actor `a` along four
 * path calls, then jump actor `b`, speak, re-arm the map and fade out.
 *
 * VERDICT
 *   OK OvlFunc_943_2009c14 -- 248 bytes, 97 encodings and 22 relocations identical
 * (against the ORIGINAL asm/ path.)
 *
 * THIS IS THE CLEANEST REFUTATION OF THE "98 ARE OUT OF REACH" SIZING.  The
 * function has NO CONDITIONAL BRANCH AT ALL -- one straight basic block from
 * push to pop -- and FIVE interleave sites, all of them therefore unguarded:
 *
 *     mov r2,#0x96 / mov r0,r5 / mov r1,#0xd8 / lsl r2,#2   -> __Func_80921c4
 *     mov r2,#0x97 / mov r0,r5 / mov r1,#0xda / lsl r2,#2   -> __Func_80921c4
 *     mov r2,#0x97 / mov r0,r5 / mov r1,#0xea / lsl r2,#2   -> __Func_80921c4
 *     mov r1,#0xa0 / mov r2,#0x14 / mov r0,r5 / lsl r1,#7   -> __Func_8092adc
 *     mov r1,#0xd8 / mov r2,#0x93 / mov r0,r5 / lsl r1,#16 / lsl r2,#18
 *
 * The dominating-block cure is unavailable at every one of them.  The whole
 * function came out byte-identical on the FIRST screen from a uniform pinned
 * whole-value fill -- one statement per argument, ascending q0/q1/q2, the
 * shifted constants written as `0x96 << 2` and not as a mov/lsl pair.
 *
 * THE INTERLEAVED ARGUMENT IS A REGISTER MOVE HERE, NOT A ZERO.  Four of the
 * five sites interleave `mov r0, r5` -- the slot variable -- rather than an
 * immediate.  docs/elevation.md already widened the detector from "r0 must be
 * zero" to "any single-instruction argument"; this widens it again to a
 * REGISTER COPY.  Nothing about the cure changes: it is still "assign the
 * single-instruction arguments to pinned registers in ascending order and
 * leave the split build bare".
 *
 * MINIMISATION, TWO ROUNDS TO A FIXPOINT.  The first exact candidate pinned
 * nine blocks / 26 registers.  What ships is five blocks / six registers:
 *   - __MapActor_SetPos, __MapActor_SetSpeed, OvlFunc_943_200ba00 and
 *     __MapActor_Jump measured EXACTLY INERT as whole blocks and are bare
 *     calls now.  __MapActor_SetPos is itself a two-split-build interleave
 *     (`mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2`) and needs NO pin.
 *   - at four of the five survivors the ONLY load-bearing pin is r1; r0 and r2
 *     are inert.  At the `0xda / 0x97 << 2` site r1 AND r2 are both needed.
 *   The asymmetry is not visible in the listing -- the `0xda` and `0xea` sites
 *   are the same five instructions with one immediate changed, and they want
 *   different pin sizes.  MEASURE EVERY SITE.
 *
 * THE r1 PIN IS DOING TWO JOBS AT ONCE.  `0x97 << 2` appears at two sites and
 * `0xa0 << 7` at two more, so dropping the wrong pin does not just lose the
 * interleave, it lets constant-CSE hoist the value into a callee-saved
 * register: the two failures that score 90 and 91 (rather than 2) are exactly
 * those, and they are 3 lines LONGER.  This is "A PARTIAL PIN CAN BE WORSE
 * THAN NO PIN AT ALL", measured: a pin set has to reach through every constant
 * the site shares with a later site.
 *
 * THE IWRAM STORE IS A SOLVED CONSTRUCT, REUSED, NOT REDERIVED.
 * src/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_a_b.c records that a named local
 * for the base costs the preferred register and that the inline dereference
 *     *(int *)(*(char **)iwram_3001ebc + (0xe0 << 1)) = 0x100;
 * is what matches.  Written that way at both sites it is exact here too, and
 * the ROM's `sub r3, #0xc0` -- deriving the stored 0x100 from the offset 0x1c0
 * it already has in a register -- falls out with both constants written as
 * plain literals, exactly as that file records for its own 0x201.
 *
 * MEASURED-WORSE TABLE (out of 97 encodings, against the final source):
 *   spelling                                                        differing
 *   ---------------------------------------------------------------  -------
 *   0x96<<2 site: no pins                                                   2
 *   0x96<<2 site: drop r1 (keep r0, r2)                                     2
 *   0x96<<2 site: drop r0 or r2                                             0 (inert)
 *   0xda/0x97<<2 site: drop r1                                              2
 *   0xda/0x97<<2 site: drop r2       91, and 3 LINES LONGER (constant-CSE)
 *   0xea/0x97<<2 site: drop r1                                              2
 *   0xec/0x26a site: drop r1                                                2
 *   __Func_8092adc site: drop r1     90, and 3 LINES LONGER (constant-CSE)
 *   __MapActor_SetPos / SetSpeed / 200ba00 / Jump: pinned                   0 (inert)
 *
 * LANDING NEEDS NO SPLIT.  The .s holds exactly ONE function
 * (.thumb_func_start line 7, .func_end line 99) and no data -- no .section,
 * .data, .word, .byte or .incbin.  One linker-script line names the object:
 *     overlays/rom_7c7b9c/overlay.ld:43
 *         asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_c.o(.text)
 * Matched on the FULL PATH; no other .ld in the tree refers to it.  Landing is
 * this .c under the same basename in src/, delete the .s, and swap that line's
 * `asm/` for `src/`.  Default -O2 rule; no flag group.
 */
extern unsigned char iwram_3001ebc[];

extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int x, int z);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_8092adc(int slot, int a, int b);
extern void __Func_809259c(int slot, int a);
extern void __Func_8093040(int slot, int a, int b);
extern void __Func_8091e9c(int n);
extern void OvlFunc_943_2008bb8(void);
extern void OvlFunc_943_200ba00(int slot, int n);

#define PIN1  register int q1 __asm__("r1")
#define PIN12 PIN1; register int q2 __asm__("r2")

void OvlFunc_943_2009c14(int a, int b)
{
    *(int *)(*(char **)iwram_3001ebc + (0xe0 << 1)) = 0x100;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    OvlFunc_943_2008bb8();
    __MapActor_SetPos(a, 0xd8 << 16, 0x93 << 18);
    __MapActor_SetSpeed(a, 0xcccc, 0x6666);
    { PIN1; q1 = 0xd8; __Func_80921c4(a, q1, 0x96 << 2); }
    { PIN12; q1 = 0xda; q2 = 0x97 << 2; __Func_80921c4(a, q1, q2); }
    { PIN1; q1 = 0xea; __Func_80921c4(a, q1, 0x97 << 2); }
    { PIN1; q1 = 0xec; __Func_80921c4(a, q1, 0x26a); }
    { PIN1; q1 = 0xa0 << 7; __Func_8092adc(a, q1, 0x14); }
    __MapActor_DoAnim(a, 3);
    __CutsceneWait(0x14);
    OvlFunc_943_200ba00(b, 0xa0 << 7);
    __MapActor_Jump(b, 4, 0x28);
    __Func_809259c(b, 2);
    __MessageID(0x1e39);
    __Func_8093040(b, 0, 0x14);
    *(int *)(*(char **)iwram_3001ebc + (0xe0 << 1)) = 0x202;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0xa);
}

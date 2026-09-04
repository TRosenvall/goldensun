// fakematch
/* OvlFunc_891_2009b44  --  0x02009b44
 *
 * Was goldensun/asm/overlays/rom_78c76c/ovl_30_c_c_c_c_a.s, which held it
 * alone.
 *
 * A cutscene beat: give the named actor and the player the same walk speed,
 * clear bit 0 of the actor's flag byte at +0x5a, then walk the player to one
 * mark and the actor to another, both in tile coordinates scaled by 16 and
 * centred by 8. Waits for the actor, drops the player back to animation 1.
 *
 * Found by tools/templated.py at a perfect 1.00 with eight shared symbols. The
 * neighbour, ovl_30_c_c_a_c_c_c_c_a_c_c_b.c, supplied the whole extern block
 * AND a struct A that already declared f5a at 0x5a -- the exact field this
 * function touches. Nothing about the data model had to be derived.
 *
 * FAKEMATCH, and it corrects a reading I made of this notebook's own rule.
 *
 * 0x3333 and 0x1999 are each used at TWO __MapActor_SetSpeed calls in a
 * function with NO BRANCH anywhere. The recorded false-positive list says a
 * CALL BETWEEN THE TWO SITES excuses a rebuild, because a call clobbers the
 * argument registers -- and there is a call between them here, so I first read
 * this as reachable. IT IS NOT. gcc parked both constants in CALLEE-SAVED
 * registers, r5 and r6, precisely so they survive the call, and fed the second
 * site with `mov r1, r5 / mov r2, r6`:
 *
 *     rom    ldr r1, =0x3333   ldr r2, =0x1999      (at BOTH sites)
 *     plain  ldr r5, =0x3333   ldr r6, =0x1999      (once, then mov/mov)
 *
 * THE REFINEMENT: a call between two uses does not guarantee a rebuild. It only
 * does so when the constant lives in a call-CLOBBERED argument register. If gcc
 * has spare callee-saved registers it will spend two of them to keep the
 * constant, and the marker is a WIDER PROLOGUE rather than a `mov` -- here
 * `push {r5, r6, r7, lr}` plus r11 against the ROM's `push {r5, r6, lr}`. When
 * a straight-line candidate comes out seven instructions LONG with extra
 * callee-saved registers, look for a hoisted constant before looking anywhere
 * else.
 *
 * SCAFFOLDING, measured by REMOVING each piece from the finished file:
 *   - the pin block on the FIRST SetSpeed call        without it: 73 differing
 *   - the two-step on the second coordinate pair      without it: 24 differing
 * Both load-bearing. A third pin on that call's r0 argument was tried and
 * bought nothing (24 either way), so it is not here -- the anchor-every-
 * argument rule is about arguments that PARTICIPATE in the perturbed
 * interleave, not about the whole list unconditionally.
 *
 * THE TWO-STEP IS ORDINARY C, not part of the fakematch. The ROM emits both
 * shifts, then `mov r3, #8`, then both adds:
 *
 *     mov r3, r8 / lsl r3, #4 / mov r8, r3
 *     mov r3, r10 / lsl r3, #4 / mov r10, r3
 *     mov r3, #8 / add r8, r3 / add r10, r3
 *
 * Written `(ax << 4) + 8` inline, gcc completes the first coordinate before
 * starting the second. Written as four compound assignments -- both shifts,
 * then both adds -- it lands exactly. That the transforms are two-address
 * (`lsl r3, #4` with the destination as the source) is what says compound
 * assignment rather than a fresh local; naming the results instead costs a
 * register and measured 75.
 *
 * The first coordinate pair needs no such treatment: gcc already interleaves it
 * into the flag-clearing block, which is where the ROM has it.
 */

struct A {
    unsigned char pad00[0xa];
    short fa;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x46];
    unsigned char f5a;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);

void OvlFunc_891_2009b44(int slot, int ax, int ay, int bx, int by)
{
    struct A *a;

    __CutsceneStart();
    __PlaySound(0xb9);
    {
        register unsigned int v0 __asm__("r1") = 0x3333;
        register unsigned int v1 __asm__("r2") = 0x1999;
        __MapActor_SetSpeed(slot, v0, v1);
    }
    __MapActor_SetSpeed(0, 0x3333, 0x1999);
    a = __MapActor_GetActor(slot);
    a->f5a &= ~1;
    __MapActor_SetAnim(0, 8);
    __MapActor_TravelTo(0, (bx << 4) + 8, (by << 4) + 8);
    ax <<= 4;
    ay <<= 4;
    ax += 8;
    ay += 8;
    __MapActor_TravelTo(slot, ax, ay);
    __MapActor_WaitMovement(slot);
    __MapActor_SetAnim(0, 1);
    __CutsceneEnd();
}

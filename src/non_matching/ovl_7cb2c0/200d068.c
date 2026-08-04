/* OvlFunc_945_200d068  [ovl_7cb2c0]  --  0x0200d068
 *
 * Source asm: goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_a.s
 *
 * A staging cutscene, near-twin of OvlFunc_945_200c198 which IS elevated
 * (src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_c_b.c). Forty-three
 * instructions against forty-three, forty-two identical.
 *
 * ONE THING SOLVED AND WORTH KEEPING: the two scripts must be SEPARATE
 * variables. Written as one pointer reassigned -- which is what the ROM's
 * reuse of r5 looks like -- the register allocation comes out swapped from
 * instruction 4 onward and forty of the forty-three lines shift. Two named
 * pointers move the first divergence from instruction 4 to instruction 35.
 *
 * Blocker: ONE ARGUMENT TRANSPOSITION, on the last __MapActor_SetBehavior:
 *
 *     rom    mov r0, #0x25 / mov r1, r5
 *     ours   mov r1, r5 / mov r0, #0x25
 *
 * The ROM fills r0 first for BOTH of its 0x24 and 0x25 calls. gcc gets 0x24
 * right and 0x25 backwards.
 *
 * THIS IS NOT THE CONSTANT-CSE CLASS, despite both involving a value used more
 * than once. There, the ROM loads a constant twice and gcc caches it in a
 * callee-saved register -- gcc does MORE caching than the ROM. Here both cache
 * the pointer in r5 and agree that they should; what differs is the order two
 * arguments are filled in for one call. Different mechanism, different fix
 * needed. (The last time two blockers were filed together on a surface
 * resemblance -- `free` and FindEntityAtPosition, batch 14 -- solving one did
 * nothing for the other.)
 *
 * TRIED, all still 43-vs-43 at instruction 35:
 *   1. the second script passed directly rather than through a variable
 *   2. the second use perturbed with `s2 + 0` to break any CSE
 *   3. declaration order swapped between the pointer and the int
 *
 * Its elevated twin has the SAME callee filled r0-first once and r0-last three
 * times, reproduced by plain C, so whatever decides this is per-call and not
 * per-callee. That is recorded in the twin's header as a bound on the
 * declaration lever.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __DeleteFieldActor(int slot);
extern int OvlFunc_945_200cfa8(int a, int b);
extern void OvlFunc_945_200c8e8(int slot, int a, int b);
extern void OvlFunc_945_200b7b4(void);
extern void OvlFunc_945_200d0e4(void);
extern unsigned char gScript_945__0200e840[];
extern unsigned char gScript_945__0200e8e4[];

void OvlFunc_945_200d068(void)
{
    unsigned char *s;
    unsigned char *s2;
    int who;

    who = OvlFunc_945_200cfa8(0, 0);
    __CutsceneStart();
    OvlFunc_945_200c8e8(0x18, 1, 0);
    OvlFunc_945_200c8e8(0x19, 2, 0);
    OvlFunc_945_200b7b4();
    OvlFunc_945_200c8e8(0x13, who, 0xc);
    __MapActor_SetAnim(0xa, 6);
    s = gScript_945__0200e840;
    __MapActor_SetBehavior(who, s);
    __DeleteFieldActor(0xb);
    __MapActor_SetBehavior(0xc, s);
    s2 = gScript_945__0200e8e4;
    __MapActor_SetBehavior(0x24, s2);
    __MapActor_SetBehavior(0x25, s2);
    OvlFunc_945_200d0e4();
    __CutsceneEnd();
}

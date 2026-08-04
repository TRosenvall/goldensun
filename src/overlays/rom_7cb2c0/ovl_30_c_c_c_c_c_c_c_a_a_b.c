/* Cluster OvlFunc_945_200d068..OvlFunc_945_200d068 extracted from goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_a_a.o and asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_a_c.o in
 * goldensun/overlays/rom_7cb2c0/overlay.ld.
 *
 * A staging cutscene, near-twin of OvlFunc_945_200c198
 * (src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_c_b.c).
 *
 * PARKED FOR A WHOLE ROUND ON SOMETHING NEVER TRIED. It sat at 43 against 43
 * with one transposition on the last __MapActor_SetBehavior:
 *
 *     rom    mov r0, #0x25 / mov r1, r5
 *     ours   mov r1, r5    / mov r0, #0x25
 *
 * The park note listed three attempts -- the script passed directly, the use
 * perturbed with `s2 + 0`, the declaration order swapped -- all of them about
 * the SCRIPT VARIABLES, because the diff sat next to one. None of them was the
 * declaration lever, which is the first thing docs/elevation.md says to try
 * when argument fill order is the only mismatch. Adding
 *
 *     extern void __MapActor_SetBehavior(int who, unsigned char *s);
 *
 * matches on the first screen. The lesson is about search, not about gcc: the
 * note reasoned from where the diff was rather than from what the diff WAS.
 *
 * WHY THE TWIN DOES NOT NEED THIS. The twin calls the same
 * __MapActor_SetBehavior four times -- r0 filled first once and last three
 * times -- and matches with NO prototype. Its header records that as a bound on
 * the lever, and THAT BOUND STANDS: a callee is either declared in a TU or it
 * is not, so a lever that were a law would give one order throughout, and the
 * twin plainly shows both.
 *
 * The two facts fit together as: the declaration sets a DEFAULT and individual
 * calls can still deviate from it. The twin's deviations happen to fall where
 * plain C already puts them. This function has exactly one call that does not,
 * and moving the default is what brings it into line -- at the cost of nothing,
 * since the other three calls here were already r0-first.
 *
 * So the lever is worth trying even when a sibling matched without it. It was
 * skipped here on the reasoning that the twin proved it inapplicable, and that
 * reasoning cost the round.
 *
 * KEEP -- still load-bearing, and unrelated to the above: the two scripts must
 * be SEPARATE variables. Written as one pointer reassigned, which is what the
 * ROM's reuse of r5 looks like, the allocation comes out swapped from
 * instruction 4 and forty of the forty-three lines shift.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int who, unsigned char *s);
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

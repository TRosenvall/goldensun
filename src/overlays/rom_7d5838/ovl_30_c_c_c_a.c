/* OvlFunc_950_2008898  --  0x02008898, was asm/overlays/rom_7d5838/ovl_30_c_c_c_a.s
 * (this function alone), so it converts whole.
 *
 * FAKEMATCH -- the corpus's PIN3 idiom on the __MapActor_Emote call (the same
 * spelling as src/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a.c),
 * booked in fakematch.txt. The generated corpus has 20+ sites with this exact
 * `mov r2 / mov r0 / ldr r1,=` -> `bl __MapActor_Emote` shape; the park's call
 * for an arg-interleave discriminator was not needed.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __ActorMessage(int actor, int b);

void OvlFunc_950_2008898(int slot)
{
    __CutsceneStart();
    __MessageID(0x23a8);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0x28; q0 = 0x1f; q1 = 0x103;
        __MapActor_Emote(q0, q1, q2);
    }
    __ActorMessage(slot, 0);
    __CutsceneEnd();
}

/* OvlFunc_942_2008260
 *
 * Cut out of goldensun/asm//overlays/rom_7c6bac/ovl_30_c_c_a_c_a_a.s.
 *
 * A villager with a one-shot line. Needs CSE_CFLAGS for the doubled flag-id
 * pool load, and `__Func_8092c40` declared `int` -- as `void` it is 2 of 45.
 *
 * BUILT WITH CSE_CFLAGS.
 *
 * Drafted by a parallel screening agent; re-screened here before wiring.
 */
extern char *iwram_3001ebc;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int slot, int arg);

void OvlFunc_942_2008260(void)
{
    unsigned short *p;

    __CutsceneStart();
    if (__GetFlag(0x8a6) == 0) {
        __MessageID(0x1cfd);
        __Func_8092c40(0xb, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __ActorMessage(0xb, 0);
            __SetFlag(0x8a6);
        } else {
            p = (unsigned short *)(iwram_3001ebc + (0xec << 1));
            *p = *p + 1;
            __ActorMessage(0xb, 0);
        }
    } else {
        __MessageID(0x1cfe);
        __ActorMessage(0xb, 0);
    }
    __CutsceneEnd();
}

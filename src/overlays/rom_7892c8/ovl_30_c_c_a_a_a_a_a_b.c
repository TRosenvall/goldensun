/* Cluster OvlFunc_888_200827c..OvlFunc_888_200827c extracted from goldensun/asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_a_a.s.
 *
 * Slotted between ..._a_a.o and the rest of the overlay.
 *
 * Near-twin of ovl_30_c_c_a_a_a_a_b.c, differing in four message ids. Same
 * treatment: __MapActor_SetAnim and __ActorMessage declared (r0 first),
 * __Func_8092848 not (r0 last).
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int b);

void OvlFunc_888_200827c(void)
{
    unsigned char *g;

    __CutsceneStart();
    if (__GetFlag(0x855))
        __MessageID(0x1377);
    else
        __MessageID(0x1289);
    g = (unsigned char *)&gState;
    g += 0xe1 << 1;
    if (*(short *)g == 0xb)
        __MessageID(0x1ce9);
    __MapActor_SetAnim(9, 1);
    __Func_8092848(9, 0, 0);
    __CutsceneWait(2);
    __ActorMessage(9, 0);
    __CutsceneEnd();
}

/* Cluster OvlFunc_888_20082ec..OvlFunc_888_20082ec extracted from goldensun/asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_a.s.
 *
 * Slotted between ovl_30_c_c_a_a_a_a_a.o and the rest of the overlay.
 *
 * __ActorMessage is DECLARED (r0 first in the ROM) while __MapActor_SetAnim is
 * not (r1 first). Two callees, opposite treatment, read off the ROM.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern void __ActorMessage(int slot, int b);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __MapActor_SetIdle(int slot);
extern void __CutsceneWait(int n);
extern void __MapActor_SetBehavior(int slot, int b);

void OvlFunc_888_20082ec(void)
{
    unsigned char *g;

    __CutsceneStart();
    if (!__GetFlag(0x855))
        __MessageID(0x128b);
    else
        __MessageID(0x1379);
    g = (unsigned char *)&gState;
    g += 0xe1 << 1;
    if (*(short *)g == 0xb)
        __MessageID(0x1ceb);
    __MapActor_SetIdle(9);
    __MapActor_SetAnim(9, 1);
    __CutsceneWait(2);
    __ActorMessage(9, 0);
    __MapActor_SetBehavior(9, 2);
    __CutsceneEnd();
}

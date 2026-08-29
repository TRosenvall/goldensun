/* Cluster OvlFunc_908_2008124..OvlFunc_908_2008124 extracted from goldensun/asm/overlays/rom_79c0c4/ovl_30_c_c_c_a_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_79c0c4/ovl_30_c_c_c_a_a_a_a.o and the rest of the overlay in
 * goldensun/overlays/rom_79c0c4/overlay.ld.
 *
 * A three-way talk gated on the player's facing and a save flag.
 *
 * FOUND BY tools/find_bb_lever.py, which lists functions the basic-block lever
 * reaches. `v = 0xc0 << 6;` is assigned at the top and used inside the second
 * arm, so the assignment and the call are in different basic blocks; that is
 * what produces the ROM's `mov r1,#0xc0 / mov r0,#0x11 / lsl r1,#6`. Written as
 * a literal at the call site it comes out contiguous and the function is two
 * positions out.
 *
 * The facing test is UNSIGNED and written as an offset compare, which is what
 * the ROM does: add 0xffff5fff and test against 0x3ffe with `bhi`. Written as
 * a range test on the original value gcc emits two comparisons.
 */
extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80b0278(int a, int b);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_908_2008124(void)
{
    unsigned int h;
    void *a;
    int v;

    v = 0xc0 << 6;
    a = __MapActor_GetActor(0);
    h = *(unsigned short *)((unsigned char *)a + 6);
    __CutsceneStart();
    h += 0xffff5fff;
    if (h <= 0x3ffe) {
        __Func_80b0278(8, 0x11);
    } else if (!__GetFlag(0x845)) {
        __MessageID(0x13e5);
        __Func_809280c(0x11, 0, 0);
        __CutsceneWait(0xa);
        __Func_8093054(0x11, 0);
        __Func_8092adc(0x11, v, 0xa);
    } else {
        __MessageID(0x16f7);
        __ActorMessage(0x11, 0);
    }
    __CutsceneEnd();
}

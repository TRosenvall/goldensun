/* Cluster OvlFunc_921_2008a3c..OvlFunc_921_2008a3c extracted from goldensun/asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_a_a_a.o and the rest of the
 * overlay in goldensun/overlays/rom_7a7298/overlay.ld.
 *
 * A talk gated on the player's facing, then on a save flag.
 *
 * `v = 0xc0 << 6;` at the top is the basic-block lever: the call that uses it
 * is two branches deep, so the assignment and the use are in different blocks
 * and gcc rematerialises at the call, splitting the pair around `mov r0`. See
 * reports/arg-interleave.md.
 *
 * THE FACING TEST SHIFTS BOTH OPERANDS. The ROM adds 0x5fff, shifts left 16 and
 * compares against 0x3ffe0000 with `bhi`. Written as a compare on the unshifted
 * value -- which is what the near-identical OvlFunc_908_2008124 does -- gcc
 * emits a different constant and a different branch. The two functions ask the
 * same question and the ROM answers it two ways; take the form from the ROM in
 * front of you rather than from the sibling.
 */
extern void *__MapActor_GetActor(int slot);
extern void __Func_80b3284(int a, int b);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_809280c(int a, int b, int c);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __Func_8093054(int a, int b);
extern void __ActorMessage(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_921_2008a3c(void)
{
    unsigned int h;
    void *a;
    int v;

    v = 0xc0 << 6;
    a = __MapActor_GetActor(0);
    h = *(unsigned short *)((unsigned char *)a + 6);
    h += 0x5fff;
    if ((h << 16) <= 0x3ffe0000) {
        __Func_80b3284(4, 0x10);
    } else {
        __CutsceneStart();
        __Func_809280c(0x10, 0, 0xa);
        if (__GetFlag(0x881)) {
            __MessageID(0x1653);
            __Func_8093054(0x10, 0);
        } else {
            __MessageID(0x154b);
            __ActorMessage(0x10, 0);
        }
        __Func_8092adc(0x10, v, 0xa);
        __CutsceneEnd();
    }
}

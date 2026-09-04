// fakematch
/* OvlFunc_890_2008c00  --  0x02008c00
 *
 * Cut out of goldensun/asm/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a.s, which held
 * twelve functions.
 *
 * 175 instructions, six guarded blocks, four short-circuit `||` conditions --
 * and an EXACT FIRST SCREEN with no lever that is not already written down.
 * That is the entry: at this size the work is transcription, and everything it
 * needed came from files elevated in the last three batches.
 *
 * WHAT IT REUSED, all documented elsewhere:
 *
 *   - `0x81 << 4` is tested at FOUR sites and `0x80a` at three; every one is
 *     rebuilt by the ROM, so each takes an r0 pin assigned immediately before
 *     its call. Without them gcc caches both in callee-saved registers and the
 *     prologue widens.
 *   - Eleven call sites want an interleaved argument fill and are pinned with
 *     each site's assignments in that site's own ROM order.
 *   - The message id is a linker symbol, spelled `extern int _MSG_1000;` with
 *     `(int)&_MSG_1000` -- the idiom already recorded in this tree, and the
 *     reason the ROM pools it where a plain integer of that size would not be.
 *   - Both actor fetches are named locals tested against zero before their
 *     fields are read; the `ldrsh` register-offset form falls out unaided.
 *
 * ONE SHAPE IS WORTH A LINE because it looks like a decompilation error and is
 * not. At `.Lcd2`/`.Lce0` the ROM calls __Func_80921c4 with identical arguments
 * in the true arm AND again at the join, while the false arm calls __GetFlag
 * and DISCARDS THE RESULT:
 *
 *     if (cond) { __Func_80921c4(0x10, 0x90 << 1, 0xe8); }
 *     else      { __GetFlag(0x80a); }
 *     __Func_80921c4(0x10, 0x90 << 1, 0xe8);
 *
 * So the call happens twice down one path and once down the other. Written
 * literally that way it matches; the temptation to "simplify" it into one call
 * after the if would change the output. A discarded call result and a
 * duplicated call are both things the original source can contain.
 */
extern void OvlFunc_890_2008108(void);
extern int _MSG_1000;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_890_2008c00(void)
{
    unsigned char *a;
    unsigned char *p;
    register int p0 __asm__("r0");

    a = __MapActor_GetActor(0x10);
    if (__GetFlag(0x809) != 0) {
        if (__GetFlag(0x814) != 0) {
            OvlFunc_890_2008108();
        } else if (__GetFlag(0x819) == 0) {
            __CutsceneStart();
            __MapActor_SetAnim(0, 0);
            __MessageID((int)&_MSG_1000);
            p0 = 0x81; p0 <<= 4;
            if (__GetFlag(p0) != 0 || __GetFlag(0x80a) == 0) {
                p = __MapActor_GetActor(0);
                if (p != 0)
                    __MapActor_SetPos(0x10, *(int *)(p + 8), *(int *)(p + 0x10));
                __CutsceneWait(4);
                { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x10; q1 <<= 9; q2 <<= 8;
                  __MapActor_SetSpeed(q0, q1, q2); }
            } else {
                p0 = 0x81; p0 <<= 4;
                if (__GetFlag(p0) != 0 || *(int *)(a + 8) > (0xaa << 17)) {
                    { PIN3; q1 = 0xc4; q2 = 0xa8; q1 <<= 17; q2 <<= 16; q0 = 0x10;
                      __MapActor_SetPos(q0, q1, q2); }
                    __CutsceneWait(4);
                    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x10; q1 <<= 10; q2 <<= 9;
                      __MapActor_SetSpeed(q0, q1, q2); }
                }
            }
            p0 = 0x81; p0 <<= 4;
            if (__GetFlag(p0) != 0 || *(int *)(a + 8) > (0xaa << 17)) {
                { PIN3; q1 = 0x90; q0 = 0x10; q1 <<= 1; q2 = 0xe8;
                  __Func_80921c4(q0, q1, q2); }
            } else {
                __GetFlag(0x80a);
            }
            { PIN3; q1 = 0x90; q0 = 0x10; q1 <<= 1; q2 = 0xe8;
              __Func_80921c4(q0, q1, q2); }
            { PIN3; q1 = 0xc0; q0 = 0; q1 <<= 8; q2 = 0;
              __Func_8092adc(q0, q1, q2); }
            { PIN3; q1 = 0x80; q0 = 0x10; q1 <<= 7; q2 = 0xa;
              __Func_8092adc(q0, q1, q2); }
            __Func_8093040(0x10, 0, 0xa);
            __MapActor_DoAnim(0, 3);
            p0 = 0x81; p0 <<= 4;
            if (__GetFlag(p0) != 0 || __GetFlag(0x80a) == 0) {
                __MapActor_SetAnim(0x10, 2);
                p = __MapActor_GetActor(0);
                if (p != 0)
                    __MapActor_TravelTo(0x10, *(short *)(p + 0xa),
                                        *(short *)(p + 0x12));
                __MapActor_WaitMovement(0x10);
                __MapActor_SetPos(0x10, 0, 0);
                { PIN3; q1 = 0x90; q0 = 0; q1 <<= 1; q2 = 0xe8;
                  __Func_80921c4(q0, q1, q2); }
            } else {
                { PIN3; q1 = 0x90; q0 = 0; q1 <<= 1; q2 = 0xf8;
                  __Func_80921c4(q0, q1, q2); }
            }
            __CutsceneEnd();
        }
    }
}

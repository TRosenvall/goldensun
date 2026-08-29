/* OvlFunc_945_20087f8  --  0x020087f8
 *
 * Cut out of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_a_c.s.
 *
 * The Psynergy tutor. Once his flag is set he offers the prompt; taking it or
 * declining it are two different endings, and declining bumps a counter in the
 * scene block.
 *
 * THE SAME CONSTANT WANTS TWO DIFFERENT SPELLINGS IN ONE FUNCTION, which is the
 * clearest example of the two argument shapes being genuinely different that
 * has come up so far. `0xc0 << 6` is passed twice, in the two arms of the inner
 * `if`:
 *
 *     OvlFunc_945_200c880   mov r1, #0xc0 / lsl r1, #6 / mov r0, #8   pair together
 *     __Func_8092adc        mov r1, #0xc0 / mov r0, #8 / lsl r1, #6   pair SPLIT
 *
 * The first wants a bare literal at the call site; the second wants the
 * BASIC-BLOCK LEVER, a named local assigned in the block above the inner `if`.
 * One of each, in the same function, on the same value.
 *
 * Four callees are declared `int` and two `void`, read straight off whether the
 * ROM emits `mov r0` first or last at each call -- the return-type lever's two
 * rows (batch 99, table in reports/batch-106.md). Nothing else distinguishes
 * them; `__Func_809280c(8, 0, 0xa)` is `void` and `__Func_8092c40(8, 0)` next to
 * it is `int`.
 *
 * -fno-rerun-cse-after-loop was screened and is byte-identical here, so this
 * one stays on the default flags. 0x925 is read once.
 */
extern char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __Func_809259c(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_945_200c86c(int slot);
extern int OvlFunc_945_200c880(int slot, int v);

void OvlFunc_945_20087f8(void)
{
    unsigned short *q;
    int v;

    __CutsceneStart();
    if (__GetFlag(0x925)) {
        v = 0xc0 << 6;
        __Func_809259c(8, 2);
        __MessageID(0x1e13);
        OvlFunc_945_200c86c(8);
        __Func_809280c(8, 0, 0xa);
        __Func_8092c40(8, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0x28);
            OvlFunc_945_200c86c(8);
            OvlFunc_945_200c880(8, 0xc0 << 6);
            __ActorMessage(8, 0);
        } else {
            q = (unsigned short *)(iwram_3001ebc + (0xec << 1));
            *q += 2;
            __ActorMessage(8, 0);
            __Func_8092adc(8, v, 0);
        }
    } else {
        __MessageID(0x1d4e);
        __ActorMessage(8, 0);
    }
    __CutsceneEnd();
}

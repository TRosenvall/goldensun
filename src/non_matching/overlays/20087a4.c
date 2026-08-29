/* OvlFunc_921_20087a4  --  0x020087a4, asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_a_a_a.s
 *
 * Source asm: goldensun/asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_a_a_a.s
 *
 * BLOCKER CLASS: constant rematerialisation. gcc BUILDS a constant once and
 * keeps it in a callee-saved register, paying a push and a pop; the ROM builds
 * it twice. Same decision as the repeated-pool-load blocker, with `mov + lsl`
 * in place of `ldr =`.
 *
 * Status: 85 lines against 85, 37 differing, from TWO instances at once --
 * 0xd5 << 17 and the `-1`, each built twice by the ROM for its two
 * __Func_80933f8 calls and hoisted by gcc into r5 and r6.
 *
 * A SECOND, SEPARATE DIFFERENCE is in the arc test and is worth recording on
 * its own. The ROM shifts first and subtracts a pre-shifted constant:
 *
 *     rom    lsl r3, r5, #16 / add r3, =0x5fff0000 / cmp r3, =0x3ffe0000
 *     ours   add r3, r5, =0x5fff / lsl r3, #16     / cmp r3, =0x3ffe0000
 *
 * Both are `(unsigned short)(v - 0xa001) <= 0x3ffe` lowered differently; gcc
 * narrows the subtraction to sixteen bits and shifts afterwards. The test
 * appears TWICE in this function, once per arm, so this costs four lines on its
 * own and would still be there with the CSE fixed.

 * MEASURED, all unchanged at the figure above: -fno-gcse, -fno-cse-follow-jumps.
 * -fno-rerun-cse-after-loop, -fno-cse-skip-blocks and
 * -fno-expensive-optimizations are all worse. No spelling separates two
 * occurrences of one value: gcc folds every arrangement to the same constant
 * and then shares it.
 *
 * `tools/pick_candidates.py` now SEES this shape -- batch 85 added `mov #imm`
 * followed by a shift or a negate to its repeated-constant filter, having met
 * three functions on it in one round. These two would not have been offered.
 *
 * The reading is otherwise believed correct and the arm structure matches
 * instruction for instruction; both streams are the same length.
 */
struct E { unsigned char pad00[6]; short f6; };

extern struct E *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_RunScript(int slot, void *s);
extern void __Func_80b0278(int shop, int slot);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern unsigned char gScript_921__0200a5ec[];

void OvlFunc_921_20087a4(void)
{
    struct E *e;
    short v;

    e = __MapActor_GetActor(0);
    v = e->f6;
    if (__GetFlag(0x881)) {
        if ((unsigned short)(v - 0xa001) <= 0x3ffe) {
            __Func_80b0278(0xa, 0xc);
            return;
        }
        __CutsceneStart();
        __Func_809280c(0xc, 0, 0);
        __CutsceneWait(0xa);
        __MessageID(0x164b);
        __ActorMessage(0xc, 0);
        __Func_8092adc(0xc, 0x80 << 7, 0xa);
        __CutsceneEnd();
    } else if ((unsigned short)(v - 0xa001) <= 0x3ffe) {
        __CutsceneStart();
        __Func_80933d4(0xc0 << 11, 0xc0 << 8);
        __Func_80933f8(0xd5 << 17, -1, 0xf6 << 17, 1);
        __Func_8093530();
        __CutsceneWait(0x14);
        __MapActor_RunScript(0xc, gScript_921__0200a5ec);
        __MessageID(0x153e);
        __ActorMessage(0xc, 0);
        __Func_80933f8(0xd5 << 17, -1, 0x9a << 18, 1);
        __Func_8093530();
        __CutsceneEnd();
    }
}

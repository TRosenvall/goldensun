/* OvlFunc_887_2008e34  --  0x02008e34, asm/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_a_a.s
 *
 * Source asm: goldensun/asm/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_a_a.s
 *
 * BLOCKER CLASS: constant rematerialisation. gcc BUILDS a constant once and
 * keeps it in a callee-saved register, paying a push and a pop; the ROM builds
 * it twice. Same decision as the repeated-pool-load blocker, with `mov + lsl`
 * in place of `ldr =`.
 *
 * Status: 72 lines against 72, 25 differing, and the 25 are one cascade. The
 * flag id 0x300 is used twice on the SAME path -- `__GetFlag(0xc0 << 2)` and
 * then `__SetFlag(0xc0 << 2)` inside one arm -- and gcc hoists it:
 *
 *     rom    mov r0, #0xc0 / lsl r0, #2   ... mov r0, #0xc0 / lsl r0, #2
 *     ours   mov r5, #0xc0 / lsl r5, #2   ... mov r0, r5    ... mov r0, r5
 *
 * so we push {{r5, r6}} where the ROM pushes {{r5}}, and every register in the
 * body shifts by one. Instruction COUNT is identical; it is purely what lives
 * where.
 *
 * One good reading did survive: the arc bound 0x9000 is the same value the ROM
 * later passes to __Func_8092adc in r1, so writing `0x90 << 8` in both places
 * is right and gcc's CSE of THOSE two is what the ROM does too.

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
struct E { unsigned char pad00[6]; unsigned short f6; };

extern struct E *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern void __Func_80b3284(int inn, int slot);
extern void __Func_80925cc(int slot, int n);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8093054(int slot, int n);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_887_2008e34(void)
{
    struct E *e;

    e = __MapActor_GetActor(0);
    if ((unsigned int)(e->f6 - 0x2000) > (0x90 << 8)) {
        __Func_80b3284(0, 0xd);
        return;
    }
    __CutsceneStart();
    if (__GetFlag(0x87a)) {
        __Func_80925cc(0xd, 2);
        __Func_809280c(0xd, 0, 0xa);
        if (__GetFlag(0xc0 << 2) == 0) {
            __MessageID(0x1c14);
            __ActorMessage(0xd, 0);
            __SetFlag(0xc0 << 2);
        }
        __MessageID(0x1c15);
        __Func_8093054(0xd, 0);
        __Func_8092adc(0xd, 0x90 << 8, 0xa);
    } else {
        if (__GetFlag(0x815))
            __MessageID(0x11a9);
        else
            __MessageID(0xf58);
        __ActorMessage(0xd, 0);
    }
    __CutsceneEnd();
}

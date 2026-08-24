/* Cluster OvlFunc_896_200a674..OvlFunc_896_200a674 extracted from goldensun/asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a.o and the rest of the overlay in
 * goldensun/overlays/rom_78ef88/overlay.ld.
 *
 * A three-way talk on two save flags. One of a twin pair differing only in the
 * actor slot and the four message ids -- see the sibling file.
 *
 * THE CALLEE MUST BE DECLARED, and it is the one thing that decides this
 * function. The ROM fills __Func_8092848's arguments r1, r0, r2 -- r0 in the
 * MIDDLE of an all-`mov` block:
 *
 *     mov r1, #0 / mov r0, #9 / mov r2, #0 / bl __Func_8092848
 *
 * Undeclared, gcc emits r0 first and the function is two positions out.
 * Declared, it matches. That is batch 26's finding confirmed on a fresh pair:
 * a middle-position r0 is reachable by the declaration lever when the operands
 * either side of it are plain `mov`s, and not when one is a shift or a pool
 * load. tools/pick_candidates.py reports that distinction as the `r0-mid` tag,
 * and it said `all-mov` here.
 *
 * Note this cuts the opposite way to the SUBTRACTIVE form used for
 * __Func_8092c40 elsewhere in the tree. The lever is "declare the ones whose r0
 * comes first, withhold from the ones whose r0 comes last" -- and for an r0 in
 * the middle of plain movs, declare.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_8092848(int a, int b, int c);

void OvlFunc_896_200a674(void)
{
    __CutsceneStart();
    if (__GetFlag(0x83e)) {
        __MessageID(0x10cb);
        __ActorMessage(9, 0);
    } else {
        if (!__GetFlag(0x83c))
            __MessageID(0x1079);
        else
            __MessageID(0x107b);
        __Func_8092848(9, 0, 0);
        __CutsceneWait(0xa);
        __ActorMessage(9, 0);
    }
    __CutsceneEnd();
}

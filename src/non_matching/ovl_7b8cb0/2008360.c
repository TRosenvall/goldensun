/* OvlFunc_931_2008360  [ovl_7b8cb0]  --  0x02008360
 *
 * Source asm: goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_a.s
 *
 * A three-way talk: before flag 0x242 is set, one line; after it, either a
 * hand-off to __Func_80b0278 or a second line that gains a third if flag 0x909
 * is also set. Thirteen calls in thirty-five instructions.
 *
 * THE SCREEN SAID OK AND THE BUILD DID NOT. tools/tryc.py reports a clean
 * match: same thirty-five instructions, same order, and the literal pool has
 * the same five words in the same order (0x242, 0x18e7, 0x18ea, 0x909,
 * 0x1941). The overlay then differs at byte 0x3A8, which is just past the end
 * of the function -- in the pool region.
 *
 * The generated .o has 0x74 bytes of .text where the function plus its pool is
 * 0x5A. Something else is landing in that object. The split was verified
 * byte-neutral BEFORE the .c was written, so the extra bytes arrive with the
 * conversion rather than with the cut.
 *
 * Reverted rather than left broken; the pre-split .s is restored and the tree
 * is green.
 *
 * WHAT THIS MEANS FOR THE SCREEN. tryc.py compares the instruction stream and
 * resolves pool loads to their values, which is what lets it see through the
 * `ldr r0, =0x242` versus `ldr r0, .L8` spelling difference. It does not
 * compare SECTION SIZE. A conversion that emits the right instructions and the
 * right pool can still emit a differently sized object, and nothing between
 * the screen and `make compare` would notice.
 *
 * That is a real gap and this is the first function to fall into it in
 * seventeen batches. Worth a size check in the screen -- compare the .text
 * size of the assembled candidate against the byte span of the reference
 * function -- before attempting this one again.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int b);
extern void __Func_80b0278(int a, int b);
extern int OvlFunc_931_2008338(void);

void OvlFunc_931_2008360(void)
{
    if (!__GetFlag(0x242)) {
        __CutsceneStart();
        __MessageID(0x18e7);
        __Func_8093054(0xf, 0);
        __CutsceneEnd();
    } else if (OvlFunc_931_2008338()) {
        __Func_80b0278(0x13, 0xf);
    } else {
        __CutsceneStart();
        __MessageID(0x18ea);
        if (__GetFlag(0x909)) {
            __MessageID(0x1941);
            __ActorMessage(0xf, 0);
        }
        __CutsceneEnd();
    }
}

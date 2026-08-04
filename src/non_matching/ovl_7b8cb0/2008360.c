/* OvlFunc_931_2008360  --  SOLVED; kept for the account of HOW the screen
 * lied. The matching version is src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_a_b.c.
 *
 * [ovl_7b8cb0]  --  0x02008360
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
 * WHAT IT ACTUALLY WAS. Exactly one byte differed: a `beq` whose offset was
 * 0x02 in the ROM and 0x06 here. Same mnemonic, same normalised target, four
 * bytes of difference in the encoded distance.
 *
 * tools/tryc.py normalised every label to L<n> in first-appearance order and
 * then DROPPED the definitions, on the reasoning -- written in its own
 * docstring -- that "their position is implied by branch order". It is not.
 * Both streams had `beq L3` at the same index and neither had anything left
 * to disagree about, so the screen reported a clean match.
 *
 * The C was genuinely wrong: flag 0x909 guards only the extra __MessageID,
 * and the __ActorMessage after it runs either way. I had put both inside the
 * guard. That is a semantic error, not a codegen one, and the screen was the
 * only thing that could have caught it before the build.
 *
 * FIXED IN THE SCREEN. renumber() now keeps label definitions that something
 * actually branches to, so a target's POSITION is part of the comparison --
 * which is what a branch encodes. Unreferenced definitions are still dropped,
 * because gcc leaves those behind after pool resolution and the disassembly
 * does not.
 *
 * A .text size check was added first, on a wrong diagnosis, and is kept: it
 * catches a different class and costs nothing. It is skipped when the
 * reference holds more than one function, where there is no honest
 * per-function size to compare.
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

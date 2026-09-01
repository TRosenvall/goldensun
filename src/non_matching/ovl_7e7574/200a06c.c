/* OvlFunc_959_200a06c (0x0200a06c) -- NON-MATCHING.
 * Blocker class: DUPLICATE-CONSTANT CSE, straight-line variant. THIRD instance
 * this batch, after ovl_7d30e0/200938c.c (a pair) and ovl_7e7574/200a1c4.c.
 *
 * 38 lines against the ROM's 37, and the extra line is the prologue:
 * `push {r5, r14}` where the ROM has `push {r14}`.
 *
 * A NEW PRESENTATION OF THE CLASS, and the reason this one is worth its own
 * file. The three __MapActor_SetPos calls pass 0xac<<18, 0xb0<<15; 0xb0<<18,
 * 0xb0<<15; 0xb4<<18, 0xc0<<15. gcc does not common the shifted VALUE -- it
 * commons the PRE-SHIFT BASE, hoisting `mov r5, #0xb0` and shifting copies of
 * it at each site, where the ROM writes `mov r1, #0xb0` and `mov r2, #0xb0`
 * separately.
 *
 * So the shared operand does not have to be the argument. `0xb0 << 15` and
 * `0xb0 << 18` are different values and neither is repeated in a way the
 * earlier two parks would describe; what is repeated is the eight-bit
 * immediate underneath both of them.
 *
 * This is straight-line from __CutsceneStart to __MapTransitionIn, so the
 * dominating-boundary remedy has nothing to attach to, exactly as in the other
 * two parks. Not re-measured here: the flag table in ovl_7d30e0/200938c.c
 * (-fno-rerun-cse-after-loop, -fno-gcse, -fno-cse-follow-jumps, and per-site
 * named locals) was inert on both earlier instances and this is the same
 * mechanism.
 *
 * NOT COPIED FROM ITS EXEMPLAR, deliberately. tools/fuzzy_solved.py offered
 * OvlFunc_925_200aeb8 at ratio 0.865, and that file is marked `// fakematch`:
 * it forces its register allocation with `__asm__ volatile ("" : "+r" (x))`
 * on thirteen locals. Copying that would have produced a match and propagated
 * a hack into a function that does not need one. The C below is written
 * plainly from the disassembly.
 *
 * NEXT: nothing source-level. Three specimens of this class now sit in the
 * tree; the useful next step is a tool that flags a repeated 8-bit immediate
 * across argument sites BEFORE the C is written, since all three were only
 * discovered from the prologue after screening.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int a);
extern void __Func_809280c(int a, int b, int c);
extern void __MapTransitionIn(void);

void OvlFunc_959_200a06c(void)
{
    __CutsceneStart();
    __MapActor_SetPos(0xc, 0xac << 18, 0xb0 << 15);
    __MapActor_SetPos(0xd, 0xb0 << 18, 0xb0 << 15);
    __MapActor_SetPos(0xe, 0xb4 << 18, 0xc0 << 15);
    __MapActor_SetAnim(0xc, 5);
    __MapActor_SetAnim(0xd, 5);
    __MapActor_SetAnim(0xe, 5);
    __Func_809280c(0, 0xd, 0);
    __CutsceneEnd();
    __MapTransitionIn();
}

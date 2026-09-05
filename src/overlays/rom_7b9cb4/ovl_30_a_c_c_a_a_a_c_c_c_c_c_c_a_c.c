/* OvlFunc_932_20088d4 -- 0x020088d4, overlay rom_7b9cb4
 *
 * A cutscene beat: repaint three map rects, play a sting, pan the camera in,
 * drift one object down over 24 frames, run a scanline handler for 101 frames,
 * pan back out, repaint the rect and set story flag 0x907.
 *
 * TWIN OF A SOLVED FUNCTION. src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_
 * c_c_c_c_a_b.c (OvlFunc_932_20087e8) is the same beat with __Func_80105d4 in
 * place of __CopyMapTiles and flag 0x8fe in place of 0x907; the tail from
 * __PlaySound(0xe6) to __SetFlag is instruction-for-instruction the same shape.
 * Found by grepping the overlay for `.L5238`, which only four functions touch.
 * "Twins are worth checking for before writing anything" -- and the twin's own
 * recorded levers carried over unchanged. First screen (no pins at all): 14
 * instructions in disagreeing regions of 110. Second screen: exact.
 *
 * WHY THIS WAS PARKED AS A ZERO-INTERLEAVE CASE, AND WHY THAT IS NOT WHAT IT IS
 * ---------------------------------------------------------------------------
 * The two __Func_8012330 sites carry the "single-instruction argument wedged
 * into a split build" silhouette:
 *
 *     mov r0,#0x80 / mov r1,#0x80 / mov r2,#0x80 / lsl r0,#10 / lsl r1,#10 / lsl r2,#9
 *     mov r0,#1 / mov r1,#1 / b .L9ac / ... / neg r1,r1 / ldr r2,=0xe666 / neg r0,r0
 *
 * and both are STRAIGHT-LINE -- no guard to dominate from, the shape the
 * "straight-line is out of reach" boundary was written about and that the
 * "wall hid 230 functions" re-derivation refutes. But the residue here is not
 * an ORDER fault at all. Both calls pass THE SAME VALUE TWICE (0x80<<10 twice;
 * -1 twice), and gcc commons it: `mov r0, r1`. That is the recorded
 * "Two uses -- PIN THE ARGUMENT REGISTER" case: a value assigned to a hard
 * call-clobbered register is dead across the next `bl`, so gcc has nothing to
 * carry it in and must rematerialise. Two pins per site, r0 and r1, and both
 * sites come out in the ROM's order for free.
 *
 * So at BOTH sites the answer to "which argument to pin" is the third reading
 * the brief did not list: pin the DUPLICATED value, both copies of it. Nothing
 * was bought by pinning the interleave, because the emitted order was never the
 * problem -- de-CSEing the pair fixed the order as a side effect.
 *
 * LEVERS, WITH MECHANISMS
 * -----------------------
 *  1. x (r0) / y (r1), one shared pair across both __Func_8012330 calls.
 *     De-CSEs the duplicated argument (above). Dropping EITHER pin regresses
 *     (77 and 4); dropping both and writing the calls inline costs 76-78 and a
 *     SHORTER stream -- the `mov r0, r1` copy is one instruction where the
 *     rematerialised build is two, so "our stream is shorter" is the signature.
 *  2. t (r2), the .L5238 pointer, with a SECOND unpinned local q taking the
 *     copy. The ROM materialises the address into r2, stores the zero through
 *     r2, and only then copies to r5 for the loop. One variable makes the copy
 *     disappear; two keep it. Here -- unlike the twin, where BOTH pins were
 *     load-bearing -- pinning q to r5 measured EXACTLY INERT once a/b were
 *     inlined, because r5 is free at that point and REG_ALLOC_ORDER picks it
 *     anyway. The pin on t is load-bearing (52 differing without it), and so is
 *     the existence of q as a separate variable (52 without it).
 *  3. e and f, plain `int` locals for the last __CopyMapTiles stack pair. The
 *     ROM builds BOTH before storing either (mov r3,#4 / mov r2,#3 / str / str);
 *     written inline gcc interleaves build-and-store and reuses r3 for both
 *     (3 differing). "Each stack-argument SITE needs its own pair of locals" --
 *     but here the REGISTER PINS the twin needed (r3/r2) are inert: only the
 *     locals matter, not where they land.
 *  4. p and r as two pointers. iwram_3001e70 is read at the top and lives in
 *     r8 across five calls, because r5/r6 are unavailable; the loop base is
 *     then `mov r5,#0xb2 / lsl r5,#1 / add r5, r8`. Writing `p += 0xb2 << 1`
 *     asks for `add rHIGH, rN`, which gcc-2.96 never emits, so the source has
 *     to birth a second pointer (111 differing if merged, and identical damage
 *     if p is inlined into the expression -- the read must be its own statement
 *     at the top or the whole prologue changes).
 *
 * MEASURED WORSE / MEASURED INERT (objcmp, encodings differing; 0 == exact)
 * ------------------------------------------------------------------------
 *     x pin dropped                                       77
 *     y pin dropped                                        4
 *     first __Func_8012330 written inline                 78
 *     x2/y2 pin dropped (second site)                     27 / 9
 *     second __Func_8012330 written inline                28
 *     t pin dropped                                       52
 *     t and q collapsed into one variable                 52
 *     e/f written inline as literals                       3
 *     p += 0xb2<<1 instead of a second pointer           111
 *     iwram read folded into the loop-base expression    111
 *     fill written DESCENDING (y = ...; x = ...;)          4
 *   -- inert, and therefore NOT shipped:
 *     q pinned to r5                                       0
 *     e pinned to r3, f pinned to r2 (jointly and singly)  0
 *     a=1 / b=2 as named locals for the stack args         0
 *     the third argument pinned too, at either site        0
 *     x/y as four per-site variables instead of two        0
 *     x = 0x80; x <<= 10;  (split spelling under the pin)  0
 *     __SetIntrHandler's handler argument declared `int`   0
 *
 * The sweep was run front to back and re-run in full after each successful
 * drop; the second pass (over the leanest candidate) reproduced every verdict
 * above, so nothing here is inert scaffolding held up by a later pin.
 *
 * NEW, small, and offered as a refinement rather than a rule: at site 1 the
 * whole-value spelling `x = 0x80 << 10;` and the statement form
 * `x = 0x80; x <<= 10;` are BYTE-IDENTICAL once x is pinned to r0. The recorded
 * statement-form lever ("a local keeps a shifted constant's mov/lsl pair
 * together") is about which register the pair lands in relative to its
 * neighbours; a hard register pin already settles that, so the two spellings
 * stop being distinguishable. Where a pin is present, do not spend a screen
 * choosing between them.
 *
 * Landing: the .s holds ONE function and the overlay .ld names its .o on
 * exactly one line --
 *     overlays/rom_7b9cb4/overlay.ld:38
 *         asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_a_c.o(.text)
 * -- so this is a whole-file replacement at
 * src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_a_c.c with NO linker
 * edit and NO split: the default `asm/%.o: src/%.c` rule puts the object where
 * the script already looks. Flag group: NONE. The path matches no explicit and
 * no pattern rule but the default, so plain GCC296_CFLAGS at -O2; do not add it
 * to CSE_CFLAGS, ALIAS_CFLAGS, GCSE_CFLAGS, SCHED2_CFLAGS or the -O1 group.
 */
extern unsigned char L5238[] __asm__(".L5238");
extern unsigned char *iwram_3001e70;

extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int x, int y, int z);
extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __SetIntrHandler(int a, int b, void (*f)(void));
extern void __SetFlag(int id);
extern void OvlFunc_932_20086a0(void);

void OvlFunc_932_20088d4(void)
{
    unsigned char *p;
    unsigned char *r;
    unsigned short *q;
    register unsigned short *t __asm__("r2");
    register int x __asm__("r0");
    register int y __asm__("r1");
    int e, f;
    int i, n;

    p = iwram_3001e70;
    __CopyMapTiles(0x71, 0x1f, 0x67, 0x11, 1, 1);
    __CopyMapTiles(0x6f, 0x20, 0x68, 0x12, 3, 2);
    __CopyMapTiles(0x40, 0x20, 0x67, 0x12, 1, 2);
    __PlaySound(0xe6);
    x = 0x80 << 10;
    y = 0x80 << 10;
    __Func_8012330(x, y, 0x80 << 9);
    __CutsceneWait(0xa);
    r = p + (0xb2 << 1);
    i = 0x17;
    do {
        *(int *)(r + 0xc) -= 0x10000;
        __WaitFrames(4);
        i--;
    } while (i >= 0);
    __SetIntrHandler(1, 0, OvlFunc_932_20086a0);
    t = (unsigned short *)L5238;
    *t = 0;
    q = t;
    do {
        __WaitFrames(1);
        n = *q + 1;
        *q = n;
    } while ((unsigned short)n <= 0x64);
    __WaitFrames(1);
    __SetIntrHandler(1, 0, 0);
    __PlaySound(0x121);
    x = -1;
    y = -1;
    __Func_8012330(x, y, 0xe666);
    __CutsceneWait(0x1e);
    e = 4;
    f = 3;
    __CopyMapTiles(0x67, 0xe, 0x67, 0x11, e, f);
    __SetFlag(0x907);
}

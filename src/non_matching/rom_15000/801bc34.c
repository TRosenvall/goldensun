/* UploadIcon -- NOT MATCHING, a SIZE mismatch: ref 160 bytes against ours 156.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/Nicon_best.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801bc34.c \
 *     asm/rom_15000/rom_1aeec_a_a_c_c.s --func UploadIcon
 *
 * The original prose park is preserved below.
 */
extern int AllocSpriteSlot(void);
extern void LoadOldUIIcon(int a, int b, int *c, int *d, int e);
extern void LoadInventoryIcon(int a, int b);
extern void LoadUIBanner(int a, int b);
extern void LoadMoveIcon(int a, int b, int *c, int *d, int e);

int UploadIcon(int id, int a)
{
    int v4;
    int v8;

    v8 = -1;
    switch (id) {
    case 1:
    case 6:
        LoadOldUIIcon(a, 0, &v8, &v4, 0);
        break;
    case 2:
        v8 = AllocSpriteSlot();
        if (v8 == 0x60)
            return -1;
        LoadInventoryIcon(a, 0x1a);
        break;
    case 9:
        v8 = AllocSpriteSlot();
        if (v8 == 0x60)
            return -1;
        LoadUIBanner(a, 0);
        break;
    case 4:
        LoadMoveIcon(a, 1, &v8, &v4, 0);
        break;
    }
    return v8;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* UploadIcon -- 0x0801bc34, asm/rom_15000/rom_1aeec_a_a_c_c.s
 *
 * 69 lines against the ROM's 71.  Candidate at scratch/Nicon_best.c.
 * The 38 differing is a two-line shift cascading; the real residue is TWO
 * INSTRUCTIONS, one at each of two sites.
 *
 * SOLVED, and this is the part worth keeping: THE JUMP TABLE REPRODUCES FROM A
 * PLAIN SWITCH.  The ROM dispatches with `sub r0, #1 / cmp r0, #8 / bhi / lsl
 * r3, r0, #2 / ldr r3, [r3, r2] / mov pc, r3` over a nine-entry table, five of
 * whose slots point at the default.  Written as `switch (id)` with cases 1, 2,
 * 4, 6 and 9 -- gcc does the biasing itself -- the table, its entry order and
 * the whole dispatch come out exact, and so do the first 32 lines.  Four
 * elevated functions in the tree already contain a jump table, so the shape was
 * known reachable; this confirms the ordinary spelling is what reaches it.
 *
 * BLOCKER: the ROM copies the AllocSpriteSlot result into a register before
 * using it, and we use it in place.
 *      rom   bl AllocSpriteSlot / mov r2, r0 / str r2, [sp, #8] / cmp r2, #0x60
 *      ours  bl AllocSpriteSlot /              str r0, [sp, #8] / cmp r0, #0x60
 * Ours is one instruction SHORTER at each of the two sites.  `v8` has its
 * address taken -- the other two arms pass `&v8` -- so it lives in the frame,
 * and gcc stores the return value and compares it straight out of r0.
 *
 * TRIED: a separate `t = AllocSpriteSlot(); v8 = t; if (t == 0x60)` at both
 * sites, which is the obvious way to ask for the register copy.  Byte-identical
 * to the original -- gcc knows t and v8 hold the same value and folds the copy
 * away.  Nothing in the source distinguishes "store it" from "keep a copy and
 * store that" when the copy has no other use.
 *
 * Also settled: the two `return -1` paths cross-jump into one shared exit that
 * skips the trailing `ldr r0, [sp, #8]`, which is what writing them as two
 * ordinary `return -1;` statements produces.
 *)

*/
#endif

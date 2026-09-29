/* OvlFunc_968_200c2bc -- 0x0200c2bc,
 * asm/overlays/rom_7f2f14/ovl_30_c_c_a_c_c_c_a.s
 *
 * TWO differing encodings of 271, at the ROM's EXACT encoding count, with
 * EVERY REGISTER ROLE ALREADY THE ROM'S.
 *      first at index 163: ref 46ca  ours 4693
 * Best candidate: scratch_elev/b250/trio/final/e2bc_BEST.c.
 *
 * FLOOR HISTORY: 205 -> 110 -> 2. Start from e2bc_BEST.c. Starting over has
 * now cost two rounds and is the single most expensive mistake available here.
 *
 * ITS TWO FILE-MATES WERE LANDED SEPARATELY IN BATCH 300 rather than waiting any
 * longer for this one.  They had been matched since batch 250 and held here on the
 * plan of converting the file whole, but fifty batches passed, and meanwhile census
 * counted them as AVAILABLE -- one was handed to a batch-300 brief as an unattempted
 * target, which is how this was noticed.  A finished candidate parked in a header is
 * invisible to every tool in the tree.
 *
 * The file is now split three ways: OvlFunc_968_200c048 and OvlFunc_968_200c520 are
 * C, and this function keeps asm/overlays/rom_7f2f14/ovl_30_c_c_a_c_c_c_a.s to
 * itself.  Closing it now converts the last third rather than the whole file.
 *
 * Originally (batch 250): its two file-mates were matched and waiting on this one:
 *      OvlFunc_968_200c048  628 bytes, 290 encodings, 20 relocations
 *      OvlFunc_968_200c520  208 bytes,  93 encodings,  9 relocations
 * They sit in scratch_elev/b250/trio/final/ as e048_MATCH.c and e520_MATCH.c,
 * and final/merged.c holds all three in ROM order with both still
 * instruction-identical inside the merged TU. c2bc is in the MIDDLE of the .s,
 * so landing the other two alone would need an awkward three-way split; the
 * file lands WHOLE the moment these two encodings close.
 *
 * THE RESIDUE, in loop 2's preheader:
 *      ROM    mov r2,#0 / mov r8,r2 / mov r10,r9 / mov r11,r2
 *      ours   mov r2,#0 / mov r8,r2 / mov r11,r2 / mov r10,r9
 * One adjacent pair, transposed.
 *
 * BLOCKER CLASS: GCSE INSERTION POINT, not allocation. The count is exact and
 * the register assignment is the ROM's throughout.
 *
 * THE PREVIOUSLY RECORDED HYPOTHESIS IS REFUTED. `base`/`z` are NOT a GIV and a
 * hoisted invariant. What actually held the function was `q = tp`: gcc
 * COALESCES two names for one address -- the recorded "if the two pointers
 * genuinely hold the same address, this lever has nothing to work with" -- which
 * freed r9 and let gcse hoist 0x17ffc there. Writing plain `t.f8 = ...` lets
 * GCSE MANUFACTURE THE SECOND REGISTER ITSELF, and that alone was 114 -> 12.
 *
 * ALSO LOAD-BEARING in the current candidate: `tp = &t` assigned THIRD (birth
 * order); `m = i + 0x1a` named before `n`; and a FRESH n2/s4/s5 for the second
 * __CopyMapTiles site.
 *
 * WHY STATEMENT ORDER CANNOT REACH IT. THIS FILE PREVIOUSLY GAVE A WRONG
 * MECHANISM HERE -- it claimed gcse's `insert_insn_end_bb` appends always-last
 * unless the block ends in a jump or call. THAT IS NOT THE PATH TAKEN, and the
 * claim is struck. `pre_edge_insert` (gcse.c:4440) calls insert_insn_end_bb
 * only for EDGE_ABNORMAL; everything else goes through `insert_insn_on_edge`,
 * and `commit_one_edge_insertion` (flow.c:1656) then chooses among three
 * placements. Nor is the copy gcse's: gcse only rewrites the in-loop address in
 * place (the dump logs `COPY-PROP: Replacing reg 155 in insn 366 with reg 46`),
 * leaving a copy INSIDE loop 2.
 *
 * THE REAL FLOOR IS loop.c's. `move_movables` inserts every hoisted movable
 * with `emit_insn_before (pat, loop_start)` -- every branch of it (loop.c:1825,
 * 1890, 1982, 1991, 2024, 2028, 2047, 2055). `loop_start` is the
 * NOTE_INSN_LOOP_BEG, and `expand_start_loop` emits that note and the loop's
 * top label back to back, so NO C STATEMENT CAN LAND BETWEEN THEM. A hoisted
 * invariant is therefore always after every source-level preheader statement,
 * and `acc = 0` is one of those (`Biv 36 initialized at insn 355`).
 *
 * BOTH ESCAPES WERE MEASURED AND BOTH ARE SHUT. The init would have to be
 * created by a pass running after move_movables:
 *   - strength_reduce's giv initialiser -- refused, because the giv is cheap:
 *     the loop dump prints `not worth while, 0 vs 65` for the shift, so
 *     `acc = i << 20` stays in the body;
 *   - check_dbra_loop's re-emitted biv init -- re-emits only the loop's
 *     COMPARISON biv, and the ROM compares `i`, not `acc`.
 * A source-level copy (`q = &t` / `q = tp`) in the preheader is coalesced away
 * in every placement, freeing r9 for a pool constant -- that is the recorded
 * 114 class, reproduced.
 *
 * 30 further variants measured across for/while/goto forms, `acc` in the
 * for-init, wrapper loops (`do{}while(0)`, `while(1){...break;}`, `for(;;)` --
 * all deleted before loop.c, byte-identical to base), eight `t.` vs `tp->`
 * store combinations, store reordering, operand order, and
 * register/unsigned/declaration-position on `acc`. NOT ONE put the hoisted copy
 * anywhere but last. Notables: b1/t1/t2 swap r9<->r10 globally (10 differing);
 * b2/b3/t3/t4/p1-p5 hoist 0x17ffc into r9; g1 comes out TWO INSTRUCTIONS SHORT
 * (269); g2/g3 land at 108.
 *
 * SO THIS IS A FLOOR, NOT A SPELLING. Full sweep and the RTL dumps are in
 * scratch_elev/b251/c2bc/ (final/NOTES.md, dumps_base/, o.sh, gen.sh, batch.sh).
 *
 * MEASURED WORSE: the split-condition `do` + `break` form is 270 lines, 224
 * differing, frame grown to 0x3c by a spill.
 *
 * LANDING SHAPE WHEN CLOSED: src/overlays/rom_7f2f14/ovl_30_c_c_a_c_c.c holding
 * all three in ROM order. No split, no linker edit -- overlay.ld:91 already
 * names asm/overlays/rom_7f2f14/ovl_30_c_c_a_c_c.o(.text) and MUST KEEP its
 * asm/ path. The .s carries no data, and the only pooled symbols are gState,
 * iwram_3001ebc and iwram_3001e40, all already extern in the tree.
 * makefile_flags() is empty, so plain -O2.
  *
 * NON-MATCHING, 2 of 271 encodings differ.
 * Verify with (recipe added in batch 300; this park never had one, which is why
 * parkcheck could not report its figure):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7f2f14/200c2bc.c \
 *       asm/overlays/rom_7f2f14/ovl_30_c_c_a_c_c_c_a.s --func OvlFunc_968_200c2bc
*/

/* BODY REPLACED IN BATCH 300.  parkcheck caught this park's header lying about its
 * own body: the header claimed 2 of 271 while the body measured 271.  The 2-of-271
 * candidate was sitting in scratch_elev/b250/trio/final/e2bc_BEST.c, named in the
 * header but never installed -- so for fifty batches this park carried a worse body
 * than the one it described, and no tool could see the discrepancy because the park
 * had no Verify recipe either. */
/* Declarations inlined from the batch-250 scratch header hdr.h in batch 300. */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x18 - 0x10];
    unsigned short f18;
    unsigned char pad1a[0x1c - 0x1a];
    void *f1c;
    unsigned char pad20[0x22 - 0x20];
    unsigned short f22;
    unsigned char pad24[0x28 - 0x24];
};

extern char *iwram_3001ebc;
extern unsigned int iwram_3001e40;

extern unsigned int __Random(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __Func_8092950(int a, int b);
extern void __Func_8091ff0(int a);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8091e9c(int n);
extern void OvlFunc_968_2008118(int a, int b, int c, int d,
                                int e, int f, int g, struct P *p);


void OvlFunc_968_200c2bc(void)
{
    struct P t;
    char *p;
    unsigned int i;
    unsigned int j;
    unsigned int k;
    int acc;
    int n;
    int m;
    int n2;
    int s4;
    int s5;
    int s0;
    int s1;
    int s2;
    int s3;
    struct P *tp;
    struct P *q;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x202;
    __CutsceneStart();
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __Func_8092950(0, 0xf);
    __Func_8091ff0(0xaa);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __PlaySound(0xa2);
    j = 0;
    i = 0;
    tp = &t;
    acc = 0;
    do {
        t.f8 = (__Random() * 2 >> 16) * 0x4ccc + 0x17ffc;
        t.fc = (__Random() * 2 >> 16) * 0x4ccc + 0x17ffc;
        t.f22 = (__Random() * 0x1000 >> 16) + (0xf8 << 8);
mid:
        k = 0;
        while (k <= 3 && i <= 7) {
            OvlFunc_968_2008118(((__Random() * 7 >> 16) << 19) + (0xd8 << 18),
                                0, (0xc0 << 14) + acc + (k << 18), 0, 0, 0,
                                0x88 << 16, tp);
            k++;
        }
        __WaitFrames(3);
        if (i == 3) {
            if (j <= 2) {
                j++;
                goto mid;
            }
        }
        n = i + 3;
        s0 = 3;
        s1 = 1;
        __CopyMapTiles(0x30, n, 0x36, n, s0, s1);
        acc += 0x80 << 13;
        i++;
    } while (i <= 9);
    s2 = 5;
    s3 = 2;
    __CopyMapTiles(0x6f, 5, 0x75, 5, s2, s3);
    __CopyMapTiles(0x6f, 0xa, 0x75, 0xa, s2, s3);
    __CopyMapTiles(0x6f, 7, 0x6f, 5, s2, s3);
    __CopyMapTiles(0x6f, 7, 0x6f, 0xa, s2, s3);
    i = 0;
    acc = 0;
    do {
        t.f8 = (__Random() * 2 >> 16) * 0x4ccc + 0x17ffc;
        t.fc = (__Random() * 2 >> 16) * 0x4ccc + 0x17ffc;
        t.f22 = (__Random() * 0x1000 >> 16) + (0xf8 << 8);
        k = 0;
        while (k <= 3 && i <= 7) {
            OvlFunc_968_2008118(((__Random() * 7 >> 16) << 19) + (0xc0 << 18),
                                0, (0xc0 << 14) + acc + (k << 18), 0, 0, 0,
                                0x88 << 16, tp);
            k++;
        }
        __WaitFrames(3);
        m = i + 0x1a;
        n2 = i + 3;
        s4 = 3;
        s5 = 1;
        __CopyMapTiles(0x37, m, 0x30, n2, s4, s5);
        acc += 0x80 << 13;
        i++;
    } while (i <= 9);
    __PlaySound(0x121);
    __CutsceneWait(0x3c);
    __Func_8091e9c(0x15);
    __CutsceneEnd();
}

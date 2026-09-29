/* Anim_Jupiter -- asm/rom_c9000/rom_dfa18_c_c_c_c_c.s, 0x080e01e4.
 *
 * NON-MATCHING, 39 of 367 encodings differ.
 *
 * SIZE EXACT (832 bytes = 832) and INSTRUCTION COUNT EXACT (367 = 367), and
 * objcmp prints NO RELOCATIONS line -- all 30 symbols in the ROM's order.  So
 * 39 IS A TRUE DISTANCE, not a saturated count.  tools/aligncmp.py: 337 of 367
 * aligned-equal (91.8%), 38 differing in 21 hunks, and EVERY HUNK IS EITHER A
 * ONE-POSITION INSTRUCTION SWAP OR A REGISTER RENAME -- the two streams contain
 * the same multiset of instructions from the prologue to the epilogue.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Jupiter.c \
 *     asm/rom_c9000/rom_dfa18_c_c_c_c_c.s --func Anim_Jupiter
 *
 * SPLIT SHAPE, from tools/datacheck.py: rom_dfa18_c_c_c_c_c.s holds TWO
 * functions (Anim_Mercury, Anim_Jupiter) AND a .rodata section, but the blob
 * .Leec5a is read by Anim_Mercury ONLY -- datacheck says "Anim_Jupiter reads no
 * data label -> split needs NO new export".  So the split is a plain text split
 * with the .rodata staying beside Anim_Mercury in the remaining .s.  No new
 * symbols: ewram_2010018 is already spelled `extern Part ewram_2010018[]` by
 * src/non_matching/rom_c9000/e6638_HelmSplitter.c, Data_ede48 by
 * src/rom_c9000/rom_d82b0_b.c, and _FILE_73/_FILE_89/_FILE_90 are in
 * include/file_table.h.  NO SHIMS, NO PINS (tools/shimcount.py is clean).  No
 * per-file Makefile flag override applies to this stem.
 *
 * ================================================================
 * PASS: sched2 (haifa-sched.c rank_for_schedule), TIE-BROKEN DIFFERENTLY IN SIX
 * PLACES, WITH THE REGISTER RENAMES AS ITS CONSEQUENCE
 * ================================================================
 *
 * The residue is six windows.  In every one the instruction that is out of place
 * has NO dependency on its neighbour, so both orders are legal and sched2's
 * ready-list tie decides.  Where the ROM consumes a call-clobbered register
 * earlier than we do, reload then picks a different staging register, and that
 * is where the renames come from -- they are not an independent defect.
 *
 *  (a) THE BIGGEST, ~14 of the 39: THE PARTICLE LOOP'S SECOND MASK.  The ROM
 *      does `bl Random / ldr r5,=0x1ff / and r5,r0` IMMEDIATELY, which kills
 *      Random's r0, so the two `qq->` loads that follow stage qq through r0
 *      (`mov r0,r8 / ldr r3,[r0] / ldr r3,[r0,#4]`).  We schedule the same
 *      `ldr/and` pair AFTER both stores, so r0 is still live and reload stages
 *      qq through r2 instead, and the 0x100 constant lands in r1 not r2.  Four
 *      source forms were measured and ALL are inert or worse:
 *        `(Random() & 0x1ff) + 0x100` as one statement      39 (this file)
 *        `0x1ff & Random()` (operand order flipped)         39, byte-identical
 *        `r = Random() & 0x1ff;` then `r += 0x100;` after
 *           the two stores                                  369 instructions (+2)
 *        the same split with `r += 0x100` before the stores  369 instructions (+2)
 *      The two `+0x100` splits BOTH cost two instructions, so the single
 *      statement is right and the ordering is not reachable from the source.
 *
 *  (b) THE k-LOOP INCREMENT BLOCK, ~7.  Same instructions, ROM order
 *      `ldr boff / movs r2,#0xe0 / lsls r2,#2 / movs r0,#0x1c / adds r1,r1,r2 /
 *       mov r7,sl / add r8,r0 / str boff / cmp r7,#8`.  ALL SIX permutations of
 *      `i++ / boff += 0xe0<<2 / qq++` were measured: every one reads exactly
 *      39 and the generated code is byte-identical between them.  Statement
 *      order does not reach this block at all.
 *
 *  (c) `movs r6,#0x3f` one position late in the setup-loop preheader, 2.  The
 *      mask is a loop invariant; giving it its own source local (the Haunt
 *      park's `mask = 0xff` device) is INERT here -- 44 before and 44 after, on
 *      the candidate where it was first tried.
 *
 *  (d) `adds r6,#0x56` one position early in the first blit's argument 3, 2.
 *  (e) `ldr r1,=gBuffer` one position late plus the r1/r3 rename, 3.
 *  (f) `mov r3,r4 / movs r2,#5` swapped in the Func_80d6888 argument block, 2,
 *      and `mov r1,r9` vs `mov r7,r9` in the target-loop guard, 2.
 *
 * NEXT IDEA, NOT TRIED: this bank has a SCHED2_CFLAGS hook in the Makefile
 * (-fno-schedule-insns2) used by other stems.  Since the residue is entirely
 * sched2 ties on a stream whose instruction multiset already matches, measuring
 * this TU under -fno-schedule-insns2 is the one cheap experiment left, and it is
 * a FLAG-CONDITIONAL figure that must be labelled as such if it helps.  It was
 * NOT measured here (production flags only).
 *
 * ================================================================
 * WHAT IS ALREADY RIGHT AND MUST NOT BE DISTURBED -- 303 of 367 -> 39
 * ================================================================
 *
 * THREE SEPARATE `Part *` WALKERS, worth 303 -> 52.  The ROM uses r5 for the
 * base+0x7080 setup loop, a plain low register for the ewram_2010018 clear, r8
 * for the eight-slot frame loop and r5 again for the 0x200-particle draw loop.
 * A single reused pointer gets ONE hard register for all of them -- it landed in
 * r8, the high register, and every access grew a `mov rX,r8` staging copy.  This
 * is the one-variable-per-region rule and it is worth more here than everything
 * else combined.  The COUNTERS go the other way: ONE `i` serves all four r10
 * loops and ONE `j` serves both r4/sp+8 loops, matching the ROM exactly.
 *
 * THE MULTIPLICAND MUST BE THE CALL RESULT, worth 52 -> 44.  The ROM's thumb
 * `mulsi3` reads `adds r6,r5,#0 / muls r6,r0`, i.e. the DESTINATION IS SEEDED
 * FROM THE NON-CALL OPERAND.  `(0x40 - frame*2) * sin(ang)` gives the opposite
 * seeding; `sin(ang) * (0x40 - frame*2)` gives the ROM's, at all four sites.
 * This is the batch-295 finding (the commutative boundary is the pattern's
 * arity, not the operator) showing up as a plain source-order lever.
 *
 * `frame = 0;` BEFORE `ang = 0x80 << 8;`, worth 44 -> 39.  Two constants
 * materialised into reload registers; the source order decides which gets r0
 * and which r1.
 *
 * THE SCALE FACTOR MUST BE INLINE IN BOTH ARGUMENTS, not a named local.  The ROM
 * computes `0x40 - frame*2` AFTER `bl sin` returns, which only happens if it is
 * part of the argument expression -- a preceding `int s = 0x40 - frame*2;`
 * statement is emitted before the call and has to survive it in a callee-saved
 * register.
 *
 * ALSO LOAD-BEARING: `pp = g; base = *pp++; ctx = *pp;` for `ldmia r3!, {r1}`;
 * `vfx = g[2]` (NOT gPtrs -- the pool entry decides the base symbol, see the
 * Haunt park); `DrawFn fns[2]` written through the array (`fns[0](...)`) and
 * read through the pointer (`fp[1](...)`) with `fp = fns` assigned BEFORE the
 * BuildDraw2DFuncs call, which is what makes one `add r2,sp,#0x20` serve both
 * the argument and the sp+0x14 store; `((short *)p)[1]` / `((short *)p)[3]` for
 * the ROM's register-offset `ldrsh`; `while (j != (*(State **)(base+0x7828))
 * ->f14)` for the knockback loop; and the declaration order fns / ctx / vfx /
 * fp / ang / boff / j, which IS the frame map (sp+0x20 down to sp+0x8, frame
 * `sub sp,#0x28`, every offset already the ROM's).
 *
 * ONE COSMETIC UNKNOWN worth recording: the ROM materialises the -1 stored by
 * the ewram_2010018 clear as `sub r1,#1` on the register that already holds the
 * 0 from `i = 0`, where gcc emits `movs r2,#1 / negs r2,r2`.  That disappeared
 * once the walkers were split, so it was a pressure artefact, not a spelling.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];
extern Part gBuffer[];
extern Part ewram_2010018[];
extern unsigned short Data_ede48[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void BuildDraw2DFuncs(int a, void **fns);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern int  Random(void);
extern int  sin(int a);
extern int  cos(int a);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80e3908(Part *p, int a, int b);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Jupiter(void *context)
{
    DrawFn fns[2];
    void *ctx;
    unsigned char *vfx;
    DrawFn *fp;
    int ang;
    int boff;
    int j;
    void **g;
    void **pp;
    unsigned char *base;
    Part *q;
    Part *q2;
    Part *qq;
    Part *p;
    int i;
    int frame;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    vfx = (unsigned char *)g[2];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    REG_BLDALPHA = 0x1010;
    fp = fns;
    BuildDraw2DFuncs(0, (void **)fp);
    LoadVFXFile(FILE_73, vfx, 0, 0);
    LoadVFXFile(FILE_90, base, 1, 1);
    LoadVFXFile(FILE_89, base + (0xc8 << 2), 1, 0);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, 0x90 << 3);
    i = 0;
    q = (Part *)(base + (0xe1 << 7));
    do {
        q->x = (Random() & 0x3f) + 0x40;
        q->y = (Random() & 0x3f) - 0x50;
        i++;
        q++;
    } while (i != 0x20);
    i = 0;
    q2 = ewram_2010018;
    do {
        q2->x = -1;
        i++;
        q2++;
    } while (i != (0x80 << 2));
    _PlaySound(0xab);
    frame = 0;
    ang = 0x80 << 8;
    do {
        if (frame == 0x38) {
            _Func_80bd7dc(0x85);
        }
        if (frame <= 0x5f) {
            fns[0](ctx, base, ((sin(ang) * (0x40 - frame * 2)) >> 17) + 0x56,
                   ((cos(ang) * (0x40 - frame * 2)) >> 16) + 0x1c, 0x14, 0x28);
        }
        boff = 0;
        i = 0;
        qq = (Part *)(base + (0xe1 << 7));
        do {
            if (frame >= i * 4 + 8 && qq->y <= 0x5f) {
                fns[0](ctx, base + (0xc8 << 2), qq->x - 0x14, qq->y - 0x20,
                       0x28, 0x40);
                qq->x = qq->x - 6;
                qq->y = qq->y + 0xc;
                if (qq->y > 0x5f) {
                    Part *t = (Part *)((char *)gBuffer + boff);
                    j = 0;
                    do {
                        int a = Random() & 0xffff;
                        int r = (Random() & 0x1ff) + 0x100;
                        t->x = qq->x << 16;
                        t->y = qq->y << 16;
                        t->vx = (sin(a) * r) >> 7;
                        t->vy = (cos(a) * r) >> 6;
                        t->t = (Random() & 0xf) + 0x20;
                        j++;
                        t++;
                    } while (j != 0x20);
                    _PlaySound(0x85);
                    *(int *)(base + 0x77a8) = 4;
                    j = 0;
                    while (j != (*(State **)(base + 0x7828))->f14) {
                        Func_80d6888((*(State **)(base + 0x7828))->ids[j],
                                     7, 5, j, 6);
                        _SetBattleActorKnockback(
                            (*(State **)(base + 0x7828))->ids[j], 6);
                        j++;
                    }
                }
            }
            i++;
            boff += 0xe0 << 2;
            qq++;
        } while (i != 8);
        i = 0;
        p = gBuffer;
        do {
            if (p->t != -1) {
                int n = p->t / 0x10 + 1;
                fp[1](ctx, vfx + Data_ede48[n - 1],
                      ((short *)p)[1] - n / 2, ((short *)p)[3] - n,
                      n, n * 2);
                Func_80e3908(p, 0x3e, 0x80 << 6);
                p->t = p->t - 1;
            }
            i++;
            p++;
        } while (i != (0x80 << 2));
        UpdateScreenShake(4, 4);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        ang -= 0x800;
        frame++;
    } while (frame != 0x60);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}

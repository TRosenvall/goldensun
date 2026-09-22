/* BaseAnim_Revive (asm/rom_c9000/rom_cf2a0_c_c.s, 638 instructions) --
 * NON-MATCHING, 587 encodings of 668, size 1476 against the ROM's 1492 (-16).
 *
 * Blocker class: global_alloc -- a whole-function register-role permutation whose
 * root is `base` losing its high register.  See the shared analysis below; it
 * applies equally to src/non_matching/rom_c9000/cd508_Haunt.c from the same round.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/cf2a0_Revive.c \
 *     asm/rom_c9000/rom_cf2a0_c_c.s --func BaseAnim_Revive
 * The reference holds ONE function, so NO SPLIT IS NEEDED to land it.
 *
 * Normalised (tryc --align, pool-blind): rom 673 lines, ours 669, 112 windows, 32
 * of which are pure register renames.  First divergence at instruction 78 -- the
 * whole prologue and all three asset loads are EXACT.  Those normalised figures
 * are NOT objcmp numbers and must not be quoted as such.
 *
 * THE PARAMETER LIST IS NOT A GUESS -- IT IS PUBLISHED IN THE TREE.
 * src/rom_c9000/rom_cf2a0_b.c and rom_cf2a0_c_b.c are landed three-line wrappers
 * declaring `extern void BaseAnim_Revive(void *context, int subanim);` and calling
 * it with 0 and 1.  This is the general rule for this bank: BaseAnim_* takes
 * (context, variant) and the variant selects file ids, timings and which arm runs;
 * Anim_* takes (context) only and hard-codes what the variant would have chosen.
 *
 * ITS .rodata IS SMALL AND READABLE, so no split is needed for the data either:
 * four .incrom blobs -- .Lee0a2 8 bytes {2,1,0,1,2,3,4,3}, .Lee0aa 6 bytes as u16
 * {0x0a8e,0x0dcf,0x11e4}, .Lee0b0 {17,19,20}, .Lee0b3 {49,55,64}.
 * docs/elevation.md 21174 applies: emit them from C with
 * `const ... __asm__(".Lee0aa")` and both stage1.ld lines stay verbatim.
 *
 * No per-file Makefile flag override exists for this stem (generic
 * `asm/%.o: src/%.c`, stock GCC296_CFLAGS).

 * ================================================================
 * THE DOMINANT BLOCKER IS THAT `base` LOSES ITS HIGH REGISTER
 * ================================================================
 *
 * `base` is live across the whole function (~650 instructions) with only ~15
 * references, so global_alloc's floor_log2(refs)*refs/live_length priority ranks
 * it near the BOTTOM; gcc spills it to a stack slot and gives r11 to a
 * loop-local. The ROM keeps it in a high register throughout (r11 in
 * BaseAnim_Revive, r10 in Anim_Haunt).
 *
 * THE SPILL IS NOT COSMETIC. It adds a frame word, which shifts EVERY stack
 * offset in the function, and it turns each `ldr rX,=K / add rX, r11` into a
 * three-instruction `ldr / ldr [sp,#N] / add rX,rY,rZ`. That is most of the
 * reported difference count in both files.
 *
 * A HIGH-REGISTER PIN IS THE ONLY THING THAT REACHES IT, and the precedent is
 * already in the tree -- eight files use __asm__("r8"/"r9"/"r10"/"r11"),
 * including the LANDED
 * src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a_c_a_b.c, so this
 * is established form and not an experiment.  docs/elevation.md 17987 names the
 * class.  ANYONE WRITING THE OTHER FUNCTIONS IN THIS BANK SHOULD PIN `base` FROM
 * THE FIRST CANDIDATE RATHER THAN DISCOVER IT.
 *
 * ================================================================
 * SPLITTING REUSED LOCALS PER REGION IS WORTH MORE THAN ANY SINGLE LEVER
 * ================================================================
 *
 * The first candidate reused f, t, a, d, q, m, actor and id across distant
 * regions. EACH REUSE IS ONE PSEUDO WITH A FUNCTION-LONG LIVE RANGE competing
 * for exactly the registers `base` needs. Giving every region its own local took
 * BaseAnim_Revive 651 -> 595 differing and 697 -> 661 lines IN ONE EDIT -- the
 * largest single step of the session.  At this size, write the C with
 * block-scoped locals from the start; it is not a late optimisation.
 *
 * ================================================================
 * HOW TO NAVIGATE AT THIS SIZE -- THE FRAME IS THE ONLY HARD SIGNAL
 * ================================================================
 *
 * Neither available count is a distance to exact.  tryc's raw "differ" count is a
 * SUFFIX count (once the streams shift by one instruction, everything after it
 * counts); objcmp's is inflated by every `ldr [pc,#N]` whose pool moved.  The
 * agent navigated on (a) window count, (b) total lines inside windows, and
 * (c) FRAME SIZE, WHICH IS A HARD BINARY SIGNAL -- `add sp, #0x74` or you have an
 * extra spill and every offset in the function is wrong.
 *
 * THREE SEPARATE TIMES a candidate's difference count improved WHILE THE FRAME
 * SILENTLY GREW.  The frame caught all three.  Check it every candidate.
 *
 * A classifier that normalises register numbers and labels inside each window,
 * splitting them into STRUCTURAL and PURE REGISTER PERMUTATION, is the useful
 * instrument here (scratch_elev/b281/I/cls.py built one).  It said 32 of
 * BaseAnim_Revive's 112 windows and 15 of Anim_Haunt's 59 have nothing wrong but
 * the register numbers -- so chasing those individually is wasted effort.
 *
 * ================================================================
 * WHAT PAID, with mechanisms -- use these on the rest of the bank
 * ================================================================
 *
 * 1. The high-register pin on `base`, and on the globals-array pointer where the
 *    ROM holds one (Anim_Haunt's r8: 72 -> 59 windows on top of the base pin).
 * 2. Per-region locals (above).
 * 3. `register int qN __asm__("rN")` pins at the Func_8001af8 DMA site, ONE BLOCK
 *    PER CALL SITE, in the ROM's own order -- the src/rom_c9000/rom_cc5d8_a_a_b.c
 *    idiom.  Without them gcc CSEs `0xa0 << 19` across all three sites into a
 *    callee-saved register, WHICH IS WHAT EVICTS `base` IN THE FIRST PLACE.  THE
 *    PIN IS NOT ABOUT THE DMA; IT IS ABOUT NOT SPENDING A REGISTER ON A CONSTANT.
 * 4. `data += 0x80;` as a walking pointer, NOT `DecompressLZ(data + 0x80, ...)`.
 *    The ROM's `add r6, #0x80` DESTROYS `data`, which only happens when the
 *    source advances it.
 * 5. A named `State **slot` where the ROM keeps base+0x7828 in a callee-saved
 *    register across calls and re-loads only the value.  THE ADDRESS IS CSE'd,
 *    THE DEREFERENCE IS NOT.  This one edit moved BaseAnim_Revive's first
 *    divergence from instruction 12 to 78 and fixed the entire prologue.
 * 6. `int` return type on DecompressLZ and BuildDraw2DFuncEx to make gcc fill r0
 *    LAST -- the src/rom_c9000/rom_cd508_a_b.c lever.  In-tree control, not a
 *    guess: src/rom_c9000/rom_ceb30_c_c_c_b.c already declares
 *    BuildDraw2DFuncEx as int.
 * 7. Reaching `gPtrs + 0xb8` THROUGH A POINTER LOCAL, else gcc folds it to a pool
 *    entry `=gPtrs+184` and THE RELOCATION IS WRONG WHILE EVERY INSTRUCTION LOOKS
 *    RIGHT.  See the base-symbol note below.
 * 8. Naming `i * 0x10` so gcc cannot refactor `j*8 + i*0x10` into `(i*2 + j)*8`,
 *    which it does and the ROM does not.
 * 9. STACK-LAYOUT CALIBRATION BEFORE WRITING THE BODY.  vec3_t and array locals
 *    come out in REVERSE declaration order (last declared gets the lowest offset)
 *    while leading scalars ASCEND.  Declaring `apos, pos, vb, va` put va at 0x44
 *    and pos at 0x5c, exactly the ROM.  Three instructions to check, and it
 *    silently poisons every `add rX, sp, #N` otherwise.
 *
 * ================================================================
 * MEASURED NEGATIVE RESULTS -- do not re-run these
 * ================================================================
 *
 * LETTING STRENGTH REDUCTION BUILD THE SECOND LOOP'S INDUCTION VARIABLES COSTS A
 * SPILL SLOT.  Writing `s->ids[k]` and `m + 0x48` instead of explicit io/th
 * locals took the frame 0x74 -> 0x78 and 124 -> 138 windows.  THE ROM'S TWO
 * STACK-RESIDENT IVs ARE EXPLICIT LOCALS IN THE SOURCE.  The agent records this
 * as its most confident-looking fix, and it was wrong.
 *
 * Also measured and inert or harmful: naming the constants the ROM holds in
 * registers across calls (two=2, w=0x28, one=1, bp=base+5) drove the function
 * SHORTER, 669 -> 666 against a 673 target, for no window gain; moving `e = 6`
 * below `d >>= 2` to match the ROM's emission order, inert twice; declaring the
 * particle counter at function scope, inert; folding +0x1c/+0x20 into the load
 * instead of the address, byte-identical; naming `0xc0 << 11` inside the loop
 * body to defeat the invariant hoist, byte-identical.
 *
 * FLAG PROBES, against a 112-window / 602-line baseline -- NO FLAG REACHES THIS
 * RESIDUE, consistent with batch 280: -fno-cse-follow-jumps INERT, -fno-gcse 133
 * windows, -fno-rerun-cse-after-loop 132, -fno-expensive-optimizations 125,
 * -fno-schedule-insns2 143.
 *
 * ================================================================
 * THE RESIDUE, and it has a DIRECTION
 * ================================================================
 *
 * It is a global register-role permutation and it is SYSTEMATICALLY ONE WAY: THE
 * ROM PREFERS HIGH REGISTERS WHERE gcc PREFERS LOW.  BaseAnim_Revive's loop
 * counters sit in r8/r9/r10 and pay `mov r2, r9` before every use; ours sit in
 * r5/r6/r7 and use them directly -- WHICH IS WHY EVERY CANDIDATE COMES OUT 4-16
 * BYTES SHORT AND NEVER LONG.  Same live values, same conflict graph, different
 * allocation order.
 *
 * THE SHARPEST SINGLE INSTANCE, and it is a good specimen of the whole class: in
 * BaseAnim_Revive's second draw loop the ROM has `th2` in r4 (call-clobbered
 * under -fcall-used-r4) and SPILLS IT around the two Func_80d6888 calls, leaving
 * r4 free for the blit function pointer -> `bl _call_via_r4`.  Ours keeps th2 in a
 * callee-saved register, so r4 is occupied at the call and gcc falls back to
 * `mov r12, r1 / bl _call_via_ip` -- A DIFFERENT VENEER SYMBOL.  THE ROM'S WORSE
 * ALLOCATION IS WHAT MAKES THE CALL SITE RIGHT.  Not steerable from C by any
 * lever currently in docs/elevation.md; the honest reading is that the ROM's
 * source has one more simultaneously-live value in that block and it was not
 * found.  Compare the REDUNDANT-COPY PRESSURE class in
 * src/non_matching/rom_b5000/80ba2c0.c -- same shape, different bank.
 *
 * A BASE-SYMBOL TELL THAT tryc CANNOT SEE.  Anim_Haunt reaches the two blit entry
 * points as iwram_3001eec[7]/[8]; BaseAnim_Revive reaches THE SAME TWO WORDS as
 * gPtrs+0xb8/+0xbc (gPtrs = 0x3001e50, so both are 0x3001f08/0x3001f0c).  THE
 * ROM'S POOL ENTRY DECIDES WHICH BASE SYMBOL THE C MUST SPELL, and both spellings
 * live in this one bank.  Getting it wrong is a RELOCATION difference, not an
 * instruction difference -- objcmp sees it, tryc cannot.
 *
 * NO SYMBOL TELLS TO REPORT.  Every constant needed already exists (_FILE_7b/_91/
 * _93/_a9/_b1/_bb in include/file_table.h; iwram_3001eec, iwram_3001ef0,
 * iwram_3001e80, gPtrs, gBuffer in wram.sym), and the base-relative offsets
 * (0x7828, 0x7824, 0x7784, 0x2710, 0x65c0, 0xef<<7) have an IN-BANK CONTROL:
 * src/rom_c9000/rom_d6504_a_c_c_b.c writes iwram_3001eec + 0x7828 as a plain
 * literal and matches.  Literals are correct here; nothing was added.
 *
 * NEXT: finish Anim_Haunt first (see its park -- it is proportionally twice as
 * close on the same levers), because whatever explains its allocation almost
 * certainly explains this one's and the rest of the bank's.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

typedef struct {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    int f20;
    short ids[4];
} State;

typedef struct {
    int x;
    int y;
    int z;
    int p3;
    int p4;
    int p5;
    int t;
} Part;

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

extern int *iwram_3001eec[];
extern void *iwram_3001e80;
extern Part gBuffer[];
extern unsigned char gPtrs[];

extern unsigned char Lee0a2[] __asm__(".Lee0a2");
extern unsigned short Lee0aa[] __asm__(".Lee0aa");
extern unsigned char Lee0b0[] __asm__(".Lee0b0");
extern unsigned char Lee0b3[] __asm__(".Lee0b3");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void Anim_Djinni(int a, int b, int c, int d, int *e, int *f);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int DecompressLZ(void *src, void *dst);
extern int Random(void);
extern void GetBattleActorPos2(int id, int *out);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern int sin(int a);
extern int cos(int a);
extern void BuildDraw2DFuncs(int a, void **fns);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);
extern void *_GetBattleActor(int id);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void MatrixTranslatev(vec3_t *v);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80d6888(int id, int a, int b, int c, int d);

void BaseAnim_Revive(void *context, int variant)
{
    register unsigned char *base __asm__("r11");
    void *ctx;
    int **pp;
    State *s;
    State **slot0;
    unsigned char *data;
    CopyFn copy;
    CopyFn copy2;
    int fid;
    int frame;
    int off;
    int arg;
    int id0;
    void *view;
    unsigned char *pt;
    void *fns[2];
    int d2;
    int d1;
    int apos[3];
    vec3_t pos;
    vec3_t vb;
    vec3_t va;

    pp = (int **)iwram_3001eec;
    base = (unsigned char *)*pp++;
    ctx = (void *)*pp;
    slot0 = (State **)(base + 0x7828);
    *slot0 = (State *)context;
    AnimStart(0);
    s = *slot0;
    if (s->f1c == 1) {
        Anim_Djinni((int)context, variant, s->f4 ^ 1, 0, &d1, &d2);
    }

    data = GetFile(FILE_7b);
    {
        PIN3;
        q0 = 0xa0;
        q2 = 0x80;
        q1 = (int)data;
        copy = Func_8001af8;
        data += 0x80;
        q0 <<= 19;
        copy((volatile u16 *)q0, (void *)q1, q2);
    }
    DecompressLZ(data, base);
    data = GetFile(FILE_b1);
    {
        PIN3;
        q0 = 0xa0;
        q1 = (int)data;
        q0 <<= 19;
        q2 = 0x80;
        copy((volatile u16 *)q0, (void *)q1, q2);
    }
    data += 0x80;
    DecompressLZ(data, base + 0x2710);
    if (variant == 0) {
        fid = FILE_93;
    } else {
        fid = FILE_91;
    }
    data = GetFile(fid);
    {
        PIN3;
        q0 = 0xa0;
        q1 = (int)data;
        copy2 = Func_8001af8;
        q2 = 0x80;
        q0 <<= 19;
        copy2((volatile u16 *)q0, (void *)q1, q2);
    }
    data += 0x80;
    DecompressLZ(data, base + 0x65c0);

    {
        Part *g = gBuffer;
        int n = 0;
        do {
            g->x = 0;
            g->y = 0xa0 << 15;
            g->z = (Random() | -0x20) << 14;
            g->t = Random() & 0xff;
            n++;
            g++;
        } while (n != (0x80 << 2));
    }

    REG_BG2PA = 0x100;
    s = *(State **)(base + 0x7828);
    if (s->f14 == 1) {
        id0 = s->ids[0];
        GetBattleActorPos2(id0, apos);
        off = 0x40 - apos[0];
    } else {
        off = -0x70;
        if (s->f4 != 1) {
            off = 0;
        }
    }
    REG_BG2X = off << 8;
    *(int *)(base + (0xef << 7)) = 2;
    arg = 0x90;
    *(int *)(base + 0x7784) = 0x32;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);

    frame = 0;
    while (frame != (*(State **)(base + 0x7828))->f14 * 0x10 + 0x74) {
        view = iwram_3001e80;
        if (frame == 0x40) {
            _PlaySound(0xd4);
        }
        if (frame == 0x50) {
            _Func_80bd7dc(0);
        }
        {
            State **slot = (State **)(base + 0x7828);
            int kind = (*slot)->f1c;
            if (kind == 1) {
                int ang = frame << 11;
                int sv = -sin(ang);
                int px = ((sv * 20) >> 16) + d1 + off - 0x14;
                int py = ((cos(ang) * 4) >> 16) + d2;
                DrawFn f;
                unsigned char *src;
                BuildDraw2DFuncs((*slot)->f4 ^ kind, fns);
                py -= 0x18;
                if (frame > 0x20) {
                    py = py - frame * 2 + 0x40;
                }
                src = base + 0x65c0;
                f = (DrawFn)fns[1];
                f(ctx, src, px, py, 0x28, 0x28);
                if (frame <= 3) {
                    f = (DrawFn)fns[1];
                    f(ctx, src, px, py, 0x28, 0x28);
                }
                gfree(0x2f);
                gfree(0x2e);
            }
        }
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
        pt = gPtrs;
        fns[0] = *(void **)(pt + 0xb8);
        BuildDraw2DFuncEx(0x2f, 7, 7, 7, 2);
        fns[1] = *(void **)(pt + 0xbc);
        if ((*(State **)(base + 0x7828))->f14 != 0) {
            int i = 0;
            int boff = 0;
            do {
                int ioff = i * 0x10;
                int *actor = (int *)_GetBattleActor((*(State **)(base + 0x7828))->ids[i]);
                int ab = *actor;
                InitMatrixStack();
                MatrixSetLook(view, (char *)view + 0xc);
                va.x = *(int *)(ab + 8);
                va.y = 0;
                va.z = *(int *)(ab + 0x10);
                MatrixTranslatev(&va);
                if (frame > ioff) {
                    Part *g = (Part *)((char *)gBuffer + boff);
                    int j = 0;
                    do {
                        if (frame > j * 8 + ioff && g->y > (0x80 << 12)) {
                            DrawFn *fp;
                            int sv;
                            int u;
                            Func_80e3944((vec3_t *)g, &pos);
                            pos.x += off;
                            sv = (sin(g->t << 10) << 4) >> 16;
                            if (j & 1) {
                                pos.x -= sv;
                            } else {
                                pos.x += sv;
                            }
                            fp = (DrawFn *)&fns[j & 1];
                            u = g->t;
                            if (u < 0) {
                                u += 7;
                            }
                            (*fp)(ctx, base + 0x2710 + (Lee0a2[(u >> 3) & 7] * 9 << 6),
                                  pos.x - 0xc, pos.y - 0xc, 0x18, 0x18);
                            g->y += 0xffff0000;
                            g->t += 1;
                        }
                        j++;
                        g++;
                    } while (j != 8);
                }
                boff += 0xe0 << 3;
                i++;
            } while (i != (*(State **)(base + 0x7828))->f14);
        }

        if ((*(State **)(base + 0x7828))->f14 != 0) {
            int k = 0;
            int io = 0x24;
            int th = 0x48;
            do {
                int m = k << 4;
                if (frame >= th) {
                    State **slot = (State **)(base + 0x7828);
                    int *actor = (int *)_GetBattleActor(*(short *)((char *)*slot + io));
                    int ab = *actor;
                    int th2;
                    InitMatrixStack();
                    MatrixSetLook(view, (char *)view + 0xc);
                    if (frame == th) {
                        Func_80d6888(*(short *)((char *)*slot + io), 1, -1, -1, 0);
                    }
                    th2 = th + 0x10;
                    if (frame == th2) {
                        Func_80d6888(*(short *)((char *)*slot + io), 0, -1, -1, 0);
                    }
                    vb.x = *(int *)(ab + 8);
                    vb.y = 0;
                    vb.z = *(int *)(ab + 0x10);
                    Func_80e3944(&vb, &pos);
                    pos.x += off;
                    if (frame < m + 0x68) {
                        int d = frame;
                        int e = 6;
                        if (d < 0) {
                            d += 3;
                        }
                        d >>= 2;
                        if (frame > th2) {
                            e = 6 - (frame - m - 0x58) / 3;
                        }
                        if (d > 2) {
                            d = (d & 1) + 1;
                        }
                        if (frame < m + 0x64) {
                            int w = Lee0b0[d];
                            int hh = Lee0b3[d];
                            DrawFn f;
                            f = (DrawFn)fns[0];
                            f(ctx, base + Lee0aa[d], pos.x - w, pos.y - hh + 8, w, hh);
                            f = (DrawFn)fns[1];
                            f(ctx, base + Lee0aa[d], pos.x, pos.y - hh + 8, w, hh);
                        }
                        if (pos.y != 0) {
                            int n2 = 0;
                            do {
                                DrawFn f;
                                f = (DrawFn)fns[0];
                                f(ctx, base + 5, pos.x - e, n2, e, 1);
                                f = (DrawFn)fns[1];
                                f(ctx, base + 5, pos.x, n2, e, 1);
                                n2++;
                            } while (n2 != pos.y);
                        }
                    }
                }
                io += 2;
                th += 0x10;
                k++;
            } while (k != (*(State **)(base + 0x7828))->f14);
        }
        gfree(0x2f);
        gfree(0x2e);
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_BlitAnim);
    AnimEnd();
}

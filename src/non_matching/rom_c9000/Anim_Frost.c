/* Anim_Frost -- PARKED.  0x080dab74, 782 instructions in the ROM listing.
 * NON-MATCHING, 788 of 815 encodings differ.
 * SIZE  ref 1776 bytes, ours 1692 (-84) -- NOT exact.
 * COUNT ref 815, ours 774 (-41)        -- NOT exact.
 * Both axes are off, so the objcmp figure SATURATES and does not rank.
 * tools/aligncmp.py: aligned-equal 466 of 815 (57.2%), 422 differing in 161 hunks.
 * SHIMS: none.  `python3 tools/shimcount.py` is silent -- no register pins, no
 * `.equ`, no `asm volatile`, no per-file Makefile flag override.  Pin-free.
 *
 * Verify with (the delivered park body, runnable as written; installed path
 * is src/non_matching/rom_c9000/Anim_Frost.c):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py scratch_elev/b310c/PARK_Anim_Frost.c \
 *     asm/rom_c9000/rom_d9ab8_c_c_c_c_c_c.s --func Anim_Frost
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py scratch_elev/b310c/PARK_Anim_Frost.c \
 *     asm/rom_c9000/rom_d9ab8_c_c_c_c_c_c.s Anim_Frost -v
 *
 * ================================================================
 * SPLIT SHAPE -- TEXT/DATA SPLIT, TEN NEW EXPORTS
 * ================================================================
 *
 * asm/rom_c9000/rom_d9ab8_c_c_c_c_c_c.s is what batch 301 left behind when it
 * landed Anim_Fireball: it now holds TWO functions, Anim_Frost (FIRST, line 12)
 * and Anim_Ray (SECOND, line 860), followed by the long .rodata run that ends
 * at 0xeeae2.  `tools/split_s.py --dry-run <file> Anim_Frost` (run, dry, and it
 * refuses until the exports exist) gives:
 *
 *   src/rom_c9000/rom_d9ab8_c_c_c_c_c_c_b.c   THIS FUNCTION
 *   asm/rom_c9000/rom_d9ab8_c_c_c_c_c_c_c.s   Anim_Ray + ALL the .rodata
 *
 * No `_a` piece is written -- nothing precedes Anim_Frost but the two .include
 * lines.  tools/datacheck.py agrees a TEXT/DATA SPLIT is needed and names the
 * exports; the file's existing globals (.Leea08 .Leea20 .Leea2c .Leea38 .Leea41
 * .Leea44 .Leea4a .Leea50 .Leea56, the last five added by batch 301 for
 * Anim_Fireball) are a DIFFERENT SET and none of them is Anim_Frost's.  Add, each
 * IMMEDIATELY BEFORE ITS LABEL in the data remainder:
 *
 *     .global .Leea62      .global .Leeab2
 *     .global .Leea88      .global .Leeab8
 *     .global .Leea91      .global .Leeabb
 *     .global .Leea99      .global .Leeac3
 *     .global .Leeaa2      .global .Leeacc
 *
 * A `.global` emits no bytes: export them and prove `make compare` green BEFORE
 * the split, then split and prove it green again, then write the .c.  Every
 * other symbol this file names already exists as a global: Data_ede84 and
 * Data_ede96 are `.incdata` at asm/rom_c9000/rom_eda78.s:33-34 and `.incdata`
 * expands to `.global \sym` (include/macros.inc:46), gBuffer and iwram_3001eec
 * are in wram.sym, and Task_BlitAnim / BuildDraw2DFuncs / MatrixTranslatev /
 * Func_80e3944 / Func_80d6888 are all spelled by LANDED .c files in this bank.
 *
 * ================================================================
 * THE PROGRAM, and how it was read
 * ================================================================
 *
 * Read off the LANDED SIBLING FROM ITS OWN ORIGINAL FILE,
 * src/rom_c9000/rom_d9ab8_c_c_c_c_c_b.c (Anim_Fireball, byte-exact in batch
 * 301), which supplied without a single probe: the `State`/`Part` layouts, the
 * `ldmia r3!, {r1}` iwram_3001eec opening, "DECLARATION ORDER IS THE FRAME MAP",
 * `sin(a) * amp` (right operand seeds the Thumb 2-address multiply), the
 * `DrawFn fns[2]` + `fp = fns` + `fp[1](...)` blit idiom, and the embedded
 * `(w = Table[u]) >> 1` argument.  The first candidate came out at 49.9%
 * aligned with the relocation SEQUENCE already almost right, which is the
 * cheapest confirmation the shape is correct -- a landed sibling in the same
 * original file really is the strongest oracle in this project.
 *
 * Six loops, all of them over 28-byte `Part` records:
 *   (1) 0x40 records at base+0x7080, seeded with Random();
 *   (2) a `while (i != f14)` lane loop calling Func_80d6888;
 *   (3) the big particle loop, `cnt` records, projecting through
 *       Func_80e3944 and blitting, then integrating;
 *   (4) a `Leea62` sprite loop bounded by `i < d / 3` with __divsi3 INSIDE
 *       the loop (loop.c cannot hoist a call);
 *   (5) two gBuffer loops, 0x20 seed and 0x18 draw/integrate;
 *   (6) two more base+0x7080 loops, 0x20 reseed and 0x20 draw.
 *
 * `Leea88` is a THREE-BYTE-PER-ENTRY table: every index in the ROM is
 * `lsl r3,r2,#1 / add r3,r2 / add r3,#K`, i.e. `f18 * 3 + K`, so it is declared
 * `unsigned char Leea88[][3]` and written `Leea88[s->f18][2]`.  Where two of the
 * three columns are read in one dominance region the ROM shares the `* 3`
 * (`ldrb r3,[r1,r3] / ldrb r2,[r1,r2]`, reg+reg both times) and cse does that on
 * its own -- no named offset local is needed, and adding one is not what this
 * function wants.
 *
 * ================================================================
 * LEVERS THAT PAID, IN ORDER, WITH FIGURES
 * ================================================================
 *
 * (1) THE OUTER FRAME LOOP IS A `while`, AND THE ROM SAYS SO IN ONE
 *     INSTRUCTION.  The entry test at .Ldac54 is
 *
 *         movs r6, #0x4b / negs r6, r6 / cmp r3, r6
 *
 *     against a loop-bottom test of `cmp r2, r3` with r3 = `Leea88[..][2]+0x4b`.
 *     `frame != L + 0x4b` with frame folded to 0 is `L != -0x4b` -- a SIGNED -75
 *     compared against an `ldrb`, a test that can never be true, which is the
 *     fingerprint of `duplicate_loop_exit_test` copying the test above the loop
 *     and cse folding the copy.  jump.c:1137 gates that on the RTL signature of
 *     a while/for, so a `do`-`while` could not have produced it.
 *
 * (2) THE PARTICLE LOOP'S WALKER IS ITS OWN VARIABLE, worth
 *     aligned-equal 407 -> 466 (49.9% -> 57.2%) with no change of count.
 *     base+0x7080 is walked in FOUR places; the ROM keeps three of them in r5
 *     and the particle loop's in r8.  One `Part *` for all four gives the
 *     particle loop r6 and rotates every register in it.  This is Anim_Ray's
 *     "THE THREE gBuffer WALKERS ARE THREE VARIABLES" transplanted, and it is
 *     the family's "pointers split, counters unify" pair resolving the same way.
 *     Splitting it FURTHER (a separate variable for each of the six walkers,
 *     frost3.c) is BYTE-IDENTICAL to splitting only this one -- gcc coalesces
 *     the rest -- so the one split is the whole lever.
 *
 * (3) THE COUNTER IS ONE VARIABLE ACROSS ALL SIX LOOPS.  Measured, not assumed:
 *     eight per-loop counters cost 466 -> 388 aligned and another 19
 *     instructions of size.  Third confirmation of lever 3 in this family.
 *
 * (4) `-0x300000`, NOT `0xffd00000`.  The threshold test is `ldr r0,=0xffd00000
 *     / cmp r3,r0 / ble`, a SIGNED branch; spelled as the hex literal C makes
 *     the comparison unsigned and the branch becomes `bls`.
 *
 * (5) `(short)(q->x >> 16)` for the three halfword reads.  combine turns
 *     `(subreg:HI (lshiftrt:SI x 16))` into `(subreg:HI x 2)` and Thumb-1 has
 *     only the register-offset LDRSH, so the ROM's `mov r0,#6 / ldrsh r3,[r5,r0]`
 *     falls out with no addressing-mode work at all.  NOT a lever site -- the
 *     `mov #K` is forced by the ISA, so do not read it as a named offset.
 *
 *
 * ================================================================
 * BATCH 310C -- RE-MEASURED AS INSTALLED, AND WHY THE NEW LEVER OF THAT BATCH
 * DOES NOT REACH THIS BLOCKER
 * ================================================================
 *
 * RE-MEASURE, as installed, no edits: 788 of 815 encodings, SIZE 1692/1776, COUNT 774/815 -- both axes still
 * inexact, so the 788 still SATURATES.  aligncmp 466 of 815 (57.2%).
 * Every figure in the header above reproduces exactly.  The header does not lie.
 * `python3 tools/shimcount.py` emits three rows in the whole tree and none of
 * them name this file: PIN-FREE confirmed.
 *
 * THE NEW LEVER, AND ITS DIRECTION.  Batch 310c closed the shared blocker of
 * Anim_Djinni and Anim_CriticalHit -- a constant that the ROM rematerialises at
 * each use while we held it in a callee-saved register -- with three tokens:
 *
 *     int clen = 0x80 << 7;     <-- INITIALISED AT ITS DECLARATION
 *
 * > AN INT CARRIER INITIALISED AT ITS DECLARATION MAKES ITS PSEUDO LIVE FROM
 * > FUNCTION ENTRY, SO allocno_compare's log2(n_refs) * freq / LIVE_LENGTH PUTS
 * > IT LAST.  It is allocated last, loses its hard register, and because its
 * > REG_EQUIV is a constant reload REMATERIALISES it instead of spilling it.
 * > A BODY ASSIGNMENT DOES THE OPPOSITE -- it keeps the range short and the
 * > priority high.  Measured both ways on Anim_Djinni: declaration initialiser
 * > 26 -> 16 with the relocations becoming exact, body assignment inert at 26.
 * > Declaration RANK is free; the constant's SPELLING is free.
 *
 * WHY IT DOES NOT APPLY HERE, stated so it is not retried.  The lever LOWERS a
 * quantity's priority.  The quantity that must lose here -- `base` -- is
 * ALREADY whole-function-lived, which is already the longest range and the
 * lowest priority available, and it still wins a register because there is one
 * free when its turn comes.  There is nothing left to lower.
 *
 * WHAT THIS FUNCTION STILL NEEDS IS THE COMPLEMENT: RAISE A COMPETITOR SO THE
 * LOOP WALKER IS PUSHED OFF ITS LOW CALLEE-SAVED REGISTER.  The recorded
 * causality is that the ROM's walker sits HIGH, every `ldr rX,[walker,#imm]`
 * then needs a LOW base, reload manufactures one copy per iteration, no low
 * callee-saved register is free to be that scratch, and so reload spills the
 * lowest-priority allocno -- the victim -- which is where its reloads and the
 * missing frame word come from.  find_reg walks REG_ALLOC_ORDER low-first, so
 * our walker takes the low register and the cycle never starts.  To reach it
 * from source, a SHORT-RANGE, MANY-REFERENCE quantity must claim that low
 * register before the walker's turn -- which is region-splitting a competitor
 * (one variable per region), NOT reuse, and NOT the carrier.  The recorded
 * caution is live: counter-splitting on Anim_Frost measured 466 -> 388, the
 * complement applied where it does not belong, and partition splits are NOT
 * ADDITIVE, so a whole partition must be applied before anything is concluded.
 *
 * THE OTHER HALF OF THE PAIR IS UNCHANGED AND STILL THE REASON TO BELIEVE THIS
 * IS A CLASS: Anim_DragonCloud, which spills `frame` instead has the same one-allocno rotation with a DIFFERENT
 * VICTIM, which is what shows the spilled variable is whoever is left over
 * rather than any particular named local -- so no per-variable spelling reaches
 * it, and the open item remains REG_ALLOC_ORDER.
 *
 * ================================================================
 * THE BLOCKER: `base` MUST BE SPILLED AND IS NOT -- ONE ALLOCNO, AND THE
 * WHOLE HIGH-REGISTER FILE ROTATES BEHIND IT
 * ================================================================
 *
 * Every remaining figure is downstream of one fact.  The ROM SPILLS `base` to
 * sp+0x30 and reloads it SIXTEEN times; we keep it in r11 for the whole
 * function.  That single difference accounts for:
 *
 *   - the SIZE and COUNT deficit.  16 `ldr rX,[sp,#0x30]` reloads plus the
 *     `adds r3,r1,r2` they feed, against our one `add r3,r3,fp`, is the -41.
 *     There is no missing work anywhere: both objects make the same 5 sin/cos,
 *     13 Random, 2 __umodsi3, 2 __modsi3, 1 __divsi3 and 5 _call_via_r4 calls,
 *     and the relocation SEQUENCE matches symbol for symbol except for pool
 *     placement.
 *   - the frame.  `sub sp,#0x5c` against the ROM's `#0x60`: the missing word IS
 *     base's spill slot, and it is the TOP scalar slot, so base is also the
 *     first-declared scalar -- the frame map and the spill agree.
 *   - 161 hunks of pure rotation.  With base holding r11 the ROM's
 *     (i r10, walker r8, &pv r11, &sv r9) becomes ours
 *     (i r8, walker r6, &pv r9, &sv r10), and every instruction that names one
 *     of those registers differs while its ROLE is already right.  This is
 *     batch 301's "A WHOLE-FUNCTION HIGH-REGISTER ROTATION IS ONE EXTRA
 *     ALLOCNO, NOT A REGISTER-ORDER PROBLEM", and the extra allocno is base.
 *
 * WHY IT IS A FIXED POINT, not an ordering accident.  Both allocations use all
 * seven call-saved registers (r5-r11; r4 is caller-saved under -fcall-used-r4)
 * and both are internally consistent:
 *
 *   ROM   walker r8 (high) -> every `ldr rX,[walker,#imm]` needs a LOW base, so
 *         reload emits `mov r6,r8` once per iteration and r6 is spent; r5 holds
 *         the blit's `w`; r7 holds `j` across sin/cos/Func_80e3944.  Seven
 *         registers, base is the eighth quantity, base spills.
 *   ours  walker r6 (low) -> no copy is needed, r5 stays free as the scratch,
 *         and r11 is therefore still unclaimed when base's turn comes.
 *
 * So the ROM's allocation is strictly WORSE by one instruction per particle-loop
 * iteration, which is the tell that the allocator was forced into it and we are
 * not.  find_reg walks REG_ALLOC_ORDER low-first, so our walker takes r6 and the
 * cycle closes.
 *
 * WHAT IS RULED OUT
 *   - EVERY SINGLE-FLAG ROUTE.  Fifteen flags measured on the candidate for
 *     `mov fp, r1` (base in r11) and the frame size: -fno-gcse (frame 88),
 *     -fno-rerun-cse-after-loop, -fno-strength-reduce, -fno-schedule-insns2,
 *     -fno-cse-follow-jumps, -fno-expensive-optimizations (frame 100),
 *     -fno-force-mem, -fno-caller-saves, -fno-thread-jumps,
 *     -fno-delete-null-pointer-checks, -fno-optimize-sibling-calls,
 *     -fno-peephole, -fno-function-cse, -fno-cse-skip-blocks.  base stays in
 *     r11 in ALL FIFTEEN.  There is no CSE_CFLAGS-style row that lands this.
 *   - A BLOCK-LOCAL COPY OF THE WALKER.  `Part *p = b;` at the top of the
 *     particle-loop body, placed exactly where the ROM's `mov r6,r8` sits, is
 *     BYTE-IDENTICAL to not writing it (frost4.c): gcc coalesces the copy, so
 *     the reload scratch cannot be manufactured from the source.
 *   - AN INT CARRIER FOR base.  `int base` with casts at every use (frost7.c)
 *     is inert on the allocation -- base still takes r11.  Fireball's
 *     `(int)fp + k` lever is about fold's canonicalisation of a pointer PLUS,
 *     not about where the pointer lives.
 *   - DECLARATION ORDER.  base cannot be moved down the declaration list: the
 *     ROM's own frame map puts its spill slot at the TOP of the scalars, which
 *     is only reachable if base is declared first.
 *   - sched1, which does not run in this build, and sched2, which cannot move a
 *     register assignment.  This is global_alloc/reload, and the registers'
 *     ROLES are already correct everywhere -- only their NAMES are rotated.
 *
 * THE TESTABLE NEXT STEP is the one HANDOFF.md already names as the corpus's
 * top open item: REG_ALLOC_ORDER.  This function is a clean instance -- a
 * documented, reproduced, one-allocno rotation where the ROM's choice is
 * measurably worse than gcc's and no source spelling reaches it.  If the order
 * were changed so find_reg did not prefer the lowest free low register, the
 * walker would go high, the reload scratch would appear, and base would spill.
 * Until then the honest reading is: the program is right, the allocation is not,
 * and a source-level route has not been found.
 *
 * NOT RESIDUES, recorded so they are not chased:
 *   - the two extra pool words.  The ref materialises `.Leea88` and `.Leeab8`
 *     from TWO pools each and we reach them from one; that is gas dumping the
 *     literal pool at a different point because our text is 84 bytes shorter,
 *     not a missing constant.
 *   - `ldr r3, .Ldabdc @ 0x100` for `REG_BG2PA = 0x100`.  0x100 is shiftable, so
 *     a pooled WORD looks wrong -- but REG_BG2PA is `vu16`, gcc emits
 *     `ldrh rX,<pool>` for a HImode volatile store, and gas assembles Thumb
 *     `ldrh <pool-label>` to the same halfword as `ldr`.  Fourth time recorded.
 *   - `Data_ede84` before `Data_ede96` in the pool: fixed by naming the source
 *     pointer first in the last loop's blit and reading the width through the
 *     embedded `(w = Data_ede96[u])`, which is what this file does.
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
extern unsigned short Data_ede84[];
extern unsigned char  Data_ede96[];
extern unsigned char  Leea62[]    __asm__(".Leea62");
extern unsigned char  Leea88[][3] __asm__(".Leea88");
extern unsigned char  Leea91[]    __asm__(".Leea91");
extern unsigned char  Leea99[]    __asm__(".Leea99");
extern unsigned short Leeaa2[]    __asm__(".Leeaa2");
extern unsigned short Leeab2[]    __asm__(".Leeab2");
extern unsigned char  Leeab8[]    __asm__(".Leeab8");
extern unsigned char  Leeabb[]    __asm__(".Leeabb");
extern unsigned char  Leeac3[]    __asm__(".Leeac3");
extern unsigned short Leeacc[]    __asm__(".Leeacc");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int file, void *dst, int a, int b);
extern void BuildDraw2DFuncs(int a, void **fns);
extern int  Random(void);
extern int  sin(int a);
extern int  cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void InitMatrixStack(void);
extern void MatrixTranslatev(vec3_t *v);
extern int  Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Frost(void *context)
{
    vec3_t pv;
    vec3_t sv;
    vec3_t tv;
    DrawFn fns[2];
    unsigned char *base;
    void *ctx;
    int frame;
    void *base2;
    DrawFn *fp;
    int cnt;
    int ymin;
    int ymax;
    int d;
    void **gp;
    void **pp;
    Part *a;
    Part *b;
    Part *q;
    int i;

    gp = iwram_3001eec;
    pp = gp;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    base2 = gp[2];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(1);
    REG_BG2PA = 0x100;
    LoadVFXFile(FILE_VFX_FROST, base, 1, 1);
    LoadVFXFile(FILE_VFX_BOREAS_SPARKLE, base2, 0, 0);
    fp = fns;
    BuildDraw2DFuncs(0, (void **)fp);
    a = (Part *)(base + 0x7080);
    i = 0;
    do {
        a->x = Random() & 0xffff;
        a->z = (Random() & 0x3f) + 0x38;
        a->y = ((Random() & 0x1f) - 0x40) << 16;
        i++;
        a++;
    } while (i != 0x40);
    *(int *)(base + 0x7780) = 2;
    *(int *)(base + 0x7784) = 0x32;
    StartTask(Task_BlitAnim, 0x480);
    if ((*(State **)(base + 0x7828))->f4 == 1) {
        REG_BG2X = 0xffff9000;
    }
    frame = 0;
    while (frame != Leea88[(*(State **)(base + 0x7828))->f18][2] + 0x4b) {
        ymin = 0x780000;
        ymax = 0;
        if (frame == Leea88[(*(State **)(base + 0x7828))->f18][2] + 0xb) {
            _Func_80bd7dc(0x84);
        }
        tv.x = 0;
        tv.y = 0;
        tv.z = 0x2000000;
        InitMatrixStack();
        MatrixTranslatev(&tv);
        d = frame - 0x24;
        if ((unsigned)d <= 0x1b && (frame & 3) == 0) {
            _PlaySound(0x73);
        }
        if (frame == 0x55) {
            _PlaySound(0x88);
        }
        i = 0;
        while (i != (*(State **)(base + 0x7828))->f14) {
            if (frame == i * 4 + 0x28) {
                Func_80d6888((*(State **)(base + 0x7828))->ids[i], 9, 5, -1, 0);
            }
            i++;
        }
        cnt = 0x10;
        if (frame < Leea88[(*(State **)(base + 0x7828))->f18][2]) {
            cnt = Leea88[(*(State **)(base + 0x7828))->f18][0];
        }
        if (frame < Leea88[(*(State **)(base + 0x7828))->f18][2] + 0x23) {
            i = 0;
            if (cnt != 0) {
                b = (Part *)(base + 0x7080);
                do {
                    if (frame > i) {
                        int j = i % 8;
                        if (b->y < ((0x30 - i / 2) << 16) && b->y > -0x300000) {
                            unsigned char w;
                            unsigned char h;
                            pv.x = sin(b->x) * b->z;
                            pv.y = b->y;
                            pv.z = cos(b->x) * b->z;
                            Func_80e3944(&pv, &sv);
                            sv.x = (sv.x >> 17) + 0x40;
                            sv.y = (short)(sv.y >> 16) + 0x3c;
                            fp[1]((void *)ctx, base + Leeaa2[j],
                                  sv.x - ((w = Leea91[j]) >> 1),
                                  sv.y - ((h = Leea99[j]) >> 1), w, h);
                        }
                        if (frame < Leea88[(*(State **)(base + 0x7828))->f18][2]) {
                            if (frame > i + 0x10) {
                                if (b->z > 4) {
                                    b->z -= 2;
                                }
                                if (b->y <= 0x2fffff) {
                                    b->y += 0x50000;
                                }
                                b->x += 0x200;
                            }
                        } else {
                            b->z += 8;
                            b->y -= (i % 5 + 2) << 16;
                            if (ymin > b->y) {
                                ymin = b->y;
                            }
                            if (ymax < b->y) {
                                ymax = b->y;
                            }
                        }
                    }
                    i++;
                    b++;
                } while (i != cnt);
            }
        }
        ymin += 0x400000;
        ymax += 0x400000;
        if (frame < Leea88[(*(State **)(base + 0x7828))->f18][2]) {
            i = 0;
            if (Leea88[(*(State **)(base + 0x7828))->f18][1] != 0) {
                do {
                    if (i < d / 3) {
                        int m = i % 3;
                        unsigned char e = Leeab8[m];
                        if (frame >= Leea88[(*(State **)(base + 0x7828))->f18][2] - 7) {
                            fp[1]((void *)ctx, base + Leeab2[m], Leea62[i * 2],
                                  Leea62[i * 2 + 1] - e, 0x20, e);
                        } else {
                            fns[0]((void *)ctx, base + Leeab2[m], Leea62[i * 2],
                                   Leea62[i * 2 + 1] - e, 0x20, e);
                        }
                    }
                    i++;
                } while (i != Leea88[(*(State **)(base + 0x7828))->f18][1]);
            }
        }
        if (frame == Leea88[(*(State **)(base + 0x7828))->f18][2]) {
            i = 0;
            q = gBuffer;
            do {
                q->x = (Random() & 0x7f) << 16;
                q->y = ((Random() & 0xf) + 0x50) << 16;
                q->z = ((Random() & 0x3f) - 0x20) << 12;
                q->vy = ((-Random() & 0xf) - 0x10) << 13;
                q->t = (Random() & 0xf) + 0x10;
                i++;
                q++;
            } while (i != 0x20);
        }
        if (frame >= Leea88[(*(State **)(base + 0x7828))->f18][2]) {
            i = 0;
            q = gBuffer;
            do {
                if (q->t >= 0) {
                    int j = i % 8;
                    fp[1]((void *)ctx, base + Leeacc[j], (short)(q->x >> 16),
                          (short)(q->y >> 16), Leeabb[j], Leeac3[j]);
                    q->x += q->vx;
                    q->y += q->vy;
                    q->t = q->t - 1;
                }
                if (ymin > q->y) {
                    ymin = q->y;
                }
                if (ymax < q->y) {
                    ymax = q->y;
                }
                i++;
                q++;
            } while (i != 0x18);
        }
        ymin >>= 16;
        ymax >>= 16;
        if (ymax <= ymin) {
            ymax = ymin + 1;
        }
        if (frame == Leea88[(*(State **)(base + 0x7828))->f18][2]) {
            i = 0;
            a = (Part *)(base + 0x7080);
            do {
                a->vx = (Random() & 0x7f) << 16;
                if (ymax == ymin) {
                    a->vy = ymin << 16;
                } else {
                    a->vy = ((unsigned)Random() % (unsigned)(ymax - ymin) + ymin) << 16;
                }
                a->t = (Random() & 0xf) + 0x14;
                i++;
                a++;
            } while (i != 0x20);
        }
        if (frame >= Leea88[(*(State **)(base + 0x7828))->f18][2]) {
            int k = (frame - Leea88[(*(State **)(base + 0x7828))->f18][2]) / 2;
            i = 0;
            a = (Part *)(base + 0x7080);
            do {
                int t = a->t;
                if ((unsigned)t <= 0x11) {
                    int u = (0x11 - t) / 2;
                    unsigned char w = Data_ede96[u];
                    fp[1]((void *)ctx, (char *)base2 + Data_ede84[u],
                          (short)(a->vx >> 16) - (w >> 1),
                          (short)(a->vy >> 16) - (w >> 1) - k, w, w);
                    t = a->t;
                }
                t = t - 1;
                a->t = t;
                if (t == -1 || t == 0x11) {
                    if (frame < Leea88[(*(State **)(base + 0x7828))->f18][2] + 0x23) {
                        a->t = 0x11;
                        a->vx = (Random() & 0x7f) << 16;
                        a->vy = ((unsigned)Random() % (unsigned)(ymax - ymin) + ymin) << 16;
                    }
                }
                i++;
                a++;
            } while (i != 0x20);
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    gfree(0x2f);
    gfree(0x2e);
    StopTask(Task_BlitAnim);
    AnimEnd();
}

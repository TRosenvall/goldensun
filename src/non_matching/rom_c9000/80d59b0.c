/* Anim_Sleep  [rom_c9000]  --  asm/rom_c9000/rom_d5258_c_c_c_c_c_c_c.s @ 0x080d59b0
 *
 * NON-MATCHING: 147 encodings of 291 differ (objcmp).
 * SIZE IS EXACT (664 bytes) AND THE INSTRUCTION COUNT IS EXACT (291 against 291).
 * All 28 relocations are present, in the ROM's exact order, with the ROM's exact
 * symbols -- including BOTH `_call_via_r4` indirect calls, which the ROM also
 * makes through r4.
 * 147 IS THEREFORE A TRUE DISTANCE IN AGGREGATE, but it is NOT a clean one:
 * 14 of the 28 relocation OFFSETS are shifted, in three contiguous groups --
 * +4 on the first pool's two words (iwram_3001eec, _FILE_a8), +2 on everything
 * from the first Random to _Func_80bd7dc, and -2 from _GetBattleActor through
 * Func_80d6888, after which they realign at _call_via_r4 (0x1be) and stay
 * aligned to the end.  So there are exactly TWO local length shifts and they
 * cancel: one instruction gained before the init loop and one lost in the matrix
 * loop.  Both are identified below.  Progression: 252 -> 152 -> 149 -> 147.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/80d59b0.c \
 *     asm/rom_c9000/rom_d5258_c_c_c_c_c_c_c.s --func Anim_Sleep
 *
 * ================================================================
 * LOAD-BEARING, each measured by a single drop from the file below
 * ================================================================
 *
 * WHICH LOOPS SHARE A COUNTER, AND IT IS WORTH 100.  The ROM keeps the INIT
 * loop's counter and the DRAW loop's counter in the SAME register (r7) and gives
 * the matrix loop a separate one (r10).  Writing it that way -- `i` for the init
 * and draw loops, `k` for the matrix loop -- is 252 -> 152 together with the two
 * items below.  With `i` shared by the init and MATRIX loops instead (the obvious
 * reading, since those two are adjacent in the source), the shared counter is
 * forced into r8 and the init loop's increment costs three extra instructions
 * (`mov r4,#1 / add r8,r4 / mov r0,r8` against the ROM's `add r7,#1`).
 * This is Anim_Break's "A COUNTER REUSED ACROSS SEVERAL LOOPS IS ONE VARIABLE"
 * rule, with the addition that WHICH loops share it is readable off the ROM's
 * register assignment and is not always the adjacent pair.
 *
 * THE BLIT COORDINATES MUST BE WRITTEN INLINE IN EACH ARM'S ARGUMENT LIST, NOT
 * PRECOMPUTED INTO LOCALS.  The ROM recomputes `ldrsh r2,[r6,#2]` and
 * `asr r3,r0,#16` inside BOTH arms of the idx<=3 / idx>3 branch.  Precomputed
 * into `sx` / `sy` locals ahead of the branch, gcc hoists them above the compare
 * once (two instructions fewer, and the whole argument window rotates).
 *
 * `g->y = g->y + ovy`, NOT `g->y = y + ovy`.  The ROM RELOADS `ldr r2,[r6,#4]`
 * in the update block even though the same value is already live in r0 from the
 * `<= 0x7fffff` guard.  Reusing the guard's local loses that load.
 *
 * THE FRAME'S SPILL-SLOT MAP IS DECLARATION ORDER, DESCENDING, AND IT IS WORTH 3.
 * The ROM's frame is `sub sp, #0x28` and reads
 *     0x00-0x07 outgoing args, 0x08 lookp, 0x0c blit1, 0x10 blit2,
 *     0x14 frame, 0x18 ctx, 0x1c-0x27 vin
 * Slots are handed out from the TOP down in declaration order, so the five
 * spilled locals must be declared ctx, frame, blit2, blit1, lookp -- which is
 * NOT their order of use.  Declared in use order the map comes out
 * 0x08 frame / 0x0c lookp / 0x10 blit2 / 0x14 blit1.  149 -> 147 once corrected
 * (the remaining 2 of that group are the `w = 0x20` statement position, next).
 *
 * `g = gBuffer; i = 0; w = 0x20;` IN THAT ORDER in the draw-loop preheader (149
 * -> 147).  `w = 0x20` first is 149, `w` then `i` then `g` is 149.
 *
 * ALSO KEPT: `arg = 0x90; arg <<= 3;` as its own statements before StartTask (the
 * recorded StartTask lever, inherited from Anim_Curse); `REG_BLDCNT`, not
 * REG_BLDALPHA, for the ROM's `add r2,#0x30` off REG_BG2PA (Anim_Curse's
 * one-register-off note -- REG_BG2PA is 0x4000020 and BLDCNT 0x4000050);
 * `tbl = (int **)iwram_3001eec; pp = tbl; base = *pp++; ctx = *pp;` for the
 * ROM's `mov r3,r6 / ldmia r3!,{r1} / ldr r3,[r3]` (Anim_Break's walker idiom);
 * a bare `Random();` statement whose result is discarded, which the ROM has
 * between the `g->y` and `g->vy` stores.
 *
 * ================================================================
 * THE BLOCKER: cse2 KEEPS `*slot` LIVE FROM THE GUARD INTO THE MATRIX LOOP
 * ================================================================
 *
 * The ROM's matrix-loop guard reads the State pointer with base and the 0x7828
 * offset in SEPARATE REGISTERS (`ldr r2,=0x7828 / mov r1,r9 / ldr r3,[r1,r2]`),
 * computes the `slot` pointer in the PREHEADER (`add r7,r1,r2`) and then RELOADS
 * `ldr r3,[r7]` at each of the loop's three uses.  gcc computes the pointer
 * BEFORE the guard (`ldr r7,=0x7828 / add r7,r9 / ldr r2,[r7]`) and then keeps
 * the loaded State pointer in r2 across the guard, using it for the body's first
 * `ldrsh` and re-deriving it (`mov r2,r3`) at the loop bottom.  That is cse2
 * forwarding a load across a basic-block boundary, and it costs the two
 * instructions that the init-loop region gains back -- the two cancelling length
 * shifts in the relocation offsets.
 *
 * It also holds the SECOND unresolved component, a two-register swap in the init
 * loop: the ROM gives the gBuffer walker r5 and the shared zero r6, gcc gives the
 * walker r6 and the zero r5.  REG_ALLOC_ORDER is {3,2,1,0,12,14,4,5,6,7,...}, so
 * the quantity that wins r5 is the higher-priority one; the ROM behaves as though
 * the walker outranks the zero and gcc as though the zero does.  Because the
 * zero is also stored to `base + 0x7784` AFTER the loop, and gcc loads gBuffer
 * into the register the zero wants, the blit2 store and the gBuffer pool load
 * swap places around the first literal pool -- which is the +4 on the first
 * pool's relocation offsets.
 *
 * MEASURED INERT (all 147 of 291, 291 instructions, i.e. literally unchanged):
 *   - the matrix-loop guard spelled `((State **)base)[0x1e0a]->f14` to keep base
 *     and the offset in separate registers (the documented member-array idiom);
 *     the same with `slot = &((State **)base)[0x1e0a]`;
 *   - `slot` assigned BEFORE the guard and the guard reading through it (149, the
 *     same as the raw-expression form -- gcc produces identical code either way);
 *   - an explicit `int z = 0;` carrier for the shared zero, the lever that closed
 *     the last 8 encodings of the landed Func_80cc960 in this same bank
 *     (rom_cc5d8_a_a_a_b.c item (e)) -- inert here, in two declaration positions;
 *   - Anim_Break's pool-hoist lever, `gp = (char *)gBuffer;` before the loop and
 *     `g = (Part *)gp;` after -- inert;
 *   - FOUR declaration positions for `g` (first, last, between the counters,
 *     before ctx) -- all 147.  This is the recorded rule that declaration-order
 *     permutation is INERT when the locals are register-resident, confirmed again.
 *   - `do { } while (0);` before `blit2 = (DrawFn)tbl[8];` -- 153, WORSE;
 *   - swapping `blit2 = tbl[8]` and `g = gBuffer` -- 192 and a length break, WORSE;
 *   - `io = 0x24` before `lookp = look + 0xc` -- 149, no change;
 *   - the final `if` block's three stores in the ROM's compute order, and with the
 *     new vy hoisted into a local -- 181 and a length break, WORSE;
 *   - writing the matrix loop entirely through the raw `*(State **)(base+0x7828)`
 *     expression with no `slot` local at all -- 237, much WORSE.
 *   - THE CALLEE-RETURN-TYPE LEVER IS EXHAUSTED HERE.  All twelve callees were
 *     flipped one at a time (AnimStart, LoadVFXFile, BuildDraw2DFuncEx,
 *     InitMatrixStack, MatrixSetLook, MatrixTranslatev, StartTask, _PlaySound,
 *     _Func_80bd7dc, Func_80d6888, WaitFrames, gfree): nine are inert at exactly
 *     147, and the two that move, move the WRONG WAY -- LoadVFXFile as `int` is
 *     150 and BuildDraw2DFuncEx as `void` is 151.  Worth recording because the
 *     same lever is worth 5 encodings on this batch's AnimStart/AnimStart2 in the
 *     same bank, so its reach is per function, not per bank.
 *
 * AND THE TWO SHIM ROUTES TO THE r5/r6 SWAP ARE BOTH NEGATIVE, MEASURED:
 *   - `__asm__ volatile ("" : "+r" (g))` as a QTY_CMP_PRI ref-count adjuster
 *     (docs/elevation.md ~19430) inside the init loop: 183 of 291 and a length
 *     break (289 instructions).  In the preheader instead: 151, no length break
 *     but still worse than 147.
 *   - pinning the walker `register Part *g __asm__("r5")`: 182 of 291, 289
 *     instructions.  Adding `register int i __asm__("r7")` on the shared counter:
 *     176 of 291, 289 instructions.  BOTH PINS LOSE AN INSTRUCTION, so the swap
 *     is not a preference gcc is nudgeable on here -- pinning the walker frees the
 *     register the zero then takes and gcc drops the separate `mov` it needed.
 * So no shim reaches this; the remaining route is the cse2 load-forwarding above.
 * NOT YET TRIED: pinning `slot` to r7 with a BLOCK-SCOPED register variable (the
 * ROM shares r7 between `slot` and the init/draw counter, so a whole-function pin
 * would conflict).
 *
 * No new symbol is warranted.  No per-file Makefile flag override was found
 * (none tried on this function; the file-mate Anim_Curse measured all three of
 * -fno-gcse / -fno-schedule-insns2 / -fno-rerun-cse-after-loop as negative or
 * inert on the same stem).
 *
 * ================================================================
 * SPLIT SHAPE
 * ================================================================
 * asm/rom_c9000/rom_d5258_c_c_c_c_c_c_c.s holds FOUR functions
 * (`grep -ci func_start` = 4): BaseAnim_ParticleCloud @ 0x080d52c8,
 * Anim_Sleep @ 0x080d59b0, Anim_Curse @ 0x080d5c48, Anim_Unused_Fizz @ 0x080d5e54,
 * plus a `.rodata` section at the end of the file.
 * ANIM_SLEEP READS NO DATA LABEL AT ALL, so its split needs ZERO `.global`
 * exports and nothing crosses the boundary in either direction.  The file's
 * `.rodata` is read only by BaseAnim_ParticleCloud (which reads `.Lee2ae`, NOT
 * currently `.global`), and that function stays in assembly -- so the `.rodata`
 * and its reader stay together on the assembly side of the cut.
 * Anim_Sleep is the SECOND of the four, so the split is a middle cut: it leaves
 * BaseAnim_ParticleCloud in the `_a`-side file and Anim_Curse +
 * Anim_Unused_Fizz + `.rodata` in the other, and `tools/repoint_parks.py` must be
 * run afterwards because src/non_matching/rom_c9000/d5c48_Curse.c names this `.s`
 * in its Verify recipe.
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
    int x;
    int y;
    int z;
    int vx;
    int vy;
    int vz;
    int t;
} Part;

extern int *iwram_3001eec[];
extern u8 *iwram_3001e80;
extern Part gBuffer[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void MatrixTranslatev(vec3_t *v);
extern void *_GetBattleActor(int id);
extern int Random(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Sleep(void *context)
{
    vec3_t vin;
    int **tbl;
    int **pp;
    u8 *base;
    void *ctx;
    int frame;
    DrawFn blit2;
    DrawFn blit1;
    void *lookp;
    void *look;
    Part *g;
    int i;
    int k;
    int io;
    int w;
    int arg;

    tbl = (int **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    ctx = (void *)*pp;
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    REG_BG2PA = 0x100;
    REG_BLDCNT = 0;
    LoadVFXFile(FILE_a8, base, 1, 1);
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 1);
    blit1 = (DrawFn)tbl[7];
    BuildDraw2DFuncEx(0x2f, 7, 7, 0xf, 1);
    blit2 = (DrawFn)tbl[8];
    g = gBuffer;
    i = 0;
    do {
        g->x = ((Random() & 0x3f) + 0x20) << 16;
        g->y = 0xffe00000;
        Random();
        g->vy = 0;
        g->z = Random() & 3;
        g->t = Random() & 0xff;
        i++;
        g++;
    } while (i != 0x20);
    if ((*(State **)(base + 0x7828))->f4 == 1) {
        REG_BG2X = 0xffff9000;
    }
    *(int *)(base + (0xef << 7)) = 1;
    *(int *)(base + 0x7784) = 0;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    _PlaySound(0x8e);
    frame = 0;
    do {
        look = iwram_3001e80;
        if (frame == 0x50) {
            _Func_80bd7dc(0);
        }
        k = 0;
        if ((*(State **)(base + 0x7828))->f14 != 0) {
            State **slot = (State **)(base + 0x7828);
            lookp = (char *)look + 0xc;
            io = 0x24;
            do {
                int *a = (int *)*(int **)_GetBattleActor(*(short *)((char *)*slot + io));
                InitMatrixStack();
                MatrixSetLook(look, lookp);
                vin.x = a[2];
                vin.y = 0xa0 << 14;
                vin.z = a[4];
                MatrixTranslatev(&vin);
                if (frame == k * 0x10 + 0x40) {
                    Func_80d6888(*(short *)((char *)*slot + io), 0, 5, -1, 0);
                }
                io += 2;
                k++;
            } while (k != (*slot)->f14);
        }
        g = gBuffer;
        i = 0;
        w = 0x20;
        do {
            if (frame > i * 4) {
                int y = g->y;
                if (y <= 0x7fffff) {
                    int idx = (g->t / 16) & 7;
                    int ovy;

                    if (idx <= 3) {
                        blit1(ctx, base + idx * 0x400,
                              *(short *)((char *)g + 2) - 0x10, (y >> 16) - 0x10, w, w);
                    } else {
                        blit2(ctx, base + idx * 0x400 - 0x1000,
                              *(short *)((char *)g + 2) - 0x10, (y >> 16) - 0x10, w, w);
                    }
                    ovy = g->vy;
                    g->y = g->y + ovy;
                    g->vy = ovy + (0x80 << 6);
                    g->t = g->t + g->z;
                    if (g->y > (0xb8 << 15) && g->vy == 0) {
                        g->z = g->z + 4;
                        g->y = 0xb8 << 15;
                        g->vy = -(ovy + 0x2001) / 2;
                    }
                }
            }
            i++;
            g++;
        } while (i != 0xc);
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x94);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}

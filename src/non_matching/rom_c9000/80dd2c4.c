/* BaseAnim_Growth -- 0x080dd2c4, 503 instructions.  PARKED.
 *
 * NON-MATCHING: 380 encodings of 541 differ (objcmp).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/80dd2c4.c asm/rom_c9000/rom_dd2ac_c_c_a.s --whole
 *
 * ================================================================
 * STATE: SIZE, INSTRUCTION COUNT AND FRAME ARE ALL EXACT
 * ================================================================
 *
 *   ref 1208 bytes / 541 encodings / `sub sp, #0x38`
 *   ours 1208 bytes / 541 encodings / `sub sp, #0x38`   (no SIZE line from objcmp)
 *
 * So 380 IS a real distance, not a misalignment artefact -- but essentially ALL of it
 * is register NAMING, not shape.  An independent aligned diff (difflib over normalised
 * instruction text, pool loads folded to one token) puts 241 of 503 instructions in a
 * differing group, and reading them one by one the residue decomposes into exactly two
 * register-allocation swaps that then cascade through every instruction that mentions
 * the affected registers:
 *
 *   ROM   r5=gPtrs/&state  r6=const1/k  r7=slot/th2/mask  r8=(i&1),g  r10=p,off  r9=i,n  r11=base
 *   ours  r5=gPtrs/&state  r6=.Leeb4b   r7=p             r8=slot      r10=i       r9=n    r11=base
 *
 *   (1) `slot` vs `p`: the ROM ranks `slot` (= base + 0x7828, live across the two
 *       BuildDraw2DFuncEx calls) ABOVE the inner-loop particle pointer `p`, so slot
 *       takes r7 and p falls to r10.  We rank p above slot, so p takes r7 and slot is
 *       pushed into r8 -- which costs `mov r8, r3` / `mov r1, r8` where the ROM just
 *       re-reads `ldr r3, [r7]`.
 *   (2) the `i` vs `p` order in HI_REGS then follows from (1): REG_ALLOC_ORDER is
 *       { 3, 2, 1, 0, 12, 14, 4, 5, 6, 7, 8, 10, 9, 11, ... } (arm.h:989), so among the
 *       high registers the order is r8, r10, r9, r11 and a single priority swap between
 *       two allocnos relabels every use of both.
 *
 * THE BLOCKER IS global_alloc's allocno_compare PRIORITY ORDER (global.c), not a
 * source shape.  Both competing values are register-resident in both builds, the
 * instruction stream is the same length, the stack frame is the same size, and the
 * stack SLOT ASSIGNMENT is the ROM's for eight of nine slots.  Nothing in the source
 * distinguishes them except their (n_refs, freq, live_length) triple.  MEASURED INERT:
 * every declaration-order permutation tried (p/g last, slot first, fp last-but-one)
 * gives BYTE-IDENTICAL output -- 380 of 541 each time -- which is the recorded
 * "declaration-order permutation is INERT when locals are all register-resident" rule
 * holding exactly.
 *
 * ================================================================
 * THE ONE SHIM, AND WHY IT IS STILL IN THE FILE
 * ================================================================
 *
 * `register unsigned char *base __asm__("r11")` is a PIN and would need a fakematch
 * row.  It is here because it is the measurement that made the rest legible:
 *
 *   without the pin  539 encodings, 1204 bytes, frame 0x38 -- base spilled to a stack
 *                    slot, `th` promoted into r9, so the frame size comes out right for
 *                    the WRONG reason and the count is 2 short
 *   with the pin     541 encodings, 1208 bytes, frame 0x38 -- exact on all three
 *
 * A pin-free route needs base to win r11 on its own.  base's global-alloc priority is
 * structurally the lowest of any value here (n_refs ~15, but live_length = the whole
 * function), which is why it is last in the ROM too -- it gets r11, the LAST high
 * register in REG_ALLOC_ORDER.  So the pin is not fighting gcc, it is only settling a
 * tie; the pin-free version loses r11 to whichever loop counter is ranked 4th.
 *
 * ================================================================
 * FOUR LEVERS THAT PAID, EACH WITH ITS SINGLE-DROP MEASUREMENT
 * ================================================================
 *
 * (a) THE DRAW LOOP'S TABLE READS MUST BE WRITTEN INLINE IN BOTH CALLS, NOT HOISTED
 *     INTO LOCALS.  Worth 549 -> 541 encodings (pinned) and it is the lever that made
 *     the count exact.  The ROM re-loads `.Leeb79[u]`, `.Leeb80[u]` and `.Leeb88[u]`
 *     after the first `bl _call_via_r4`, and keeps only `u` and `u * 2` across it (in
 *     r7 and r6, both callee-saved LOW registers).  MECHANISM: a call invalidates all
 *     of cse's memory table, so two textually identical array reads either side of a
 *     call CANNOT be unified -- but `a = Leeb79[u];` before the calls makes them one
 *     pseudo, which gcc then has to keep live across the call in a callee-saved HIGH
 *     register.  That single decision is what stole r8/r9/r10 from the loop counters
 *     and forced base out of r11 in the first place.  So: for a value read from a
 *     const table and passed to two calls, WRITE THE SUBSCRIPT TWICE.
 *
 * (b) `th = i * 4 + 8` AND `p = base + (0xe1 << 7) + i * 0x1c` AS STRENGTH-REDUCED
 *     givs, not as explicit locals advanced at the loop bottom.  This is Anim_Break's
 *     recorded giv rule and it holds here: as givs their initialising stores land in
 *     the loop PREHEADER (after the `cmp r3, #0` guard branch), which is where the ROM
 *     has `mov r2, #0xe1 / lsl r2, #7 / add r2, r11 / str r1, [sp, #0xc] / mov r10, r2`,
 *     and `th` takes stack slot 0xc -- the LOWEST slot, the SR-pseudo signature.
 *     MEASURED: writing p as an explicit local advanced with `p = (Part *)((char *)p +
 *     0x1c)` at the bottom gives 537 encodings / 1200 bytes -- three instructions SHORT
 *     and the giv init moves above the guard.
 *
 * (c) THE INNER i-LOOP IS THE `while` FORM, THE FRAME LOOP IS THE `if (n) do..while`
 *     FORM.  Both are forced by the ROM and they disagree, which is the per-level rule:
 *       - frame loop: the three invariants `n - 0x40`, `n - 0x14`, `n - 4` are computed
 *         ONCE in a preheader at .Ldd420, so it hoists -> real do-while behind an `if`.
 *       - inner i-loop: `Leeb5e[(*slot)->f18]` is recomputed IN FULL both at the guard
 *         (.Ldd46e) and at the loop bottom (.Ldd622) -- two independent copies, which is
 *         duplicate_loop_exit_test on a `while` whose bound gcse could not hoist.
 *     MEASURED the other way: giving the k-loop (Func_80d6888) an explicit `if` guard
 *     plus a held `State **s2` local costs 8 instructions (549 encodings) -- the ROM's
 *     `mov r5, r2` there is gcse's own doing off the `while` form, not a source local.
 *
 * (d) `fp` DECLARED AFTER `frame` AND `nframes`.  This is the stack-slot ORDER lever:
 *     slots go to spilled pseudos in descending regno (low slot = high regno, which is
 *     why the SR giv `th` is at 0xc), so the ROM's 0x1c=fp / 0x20=nframes / 0x24=frame
 *     requires regno(fp) > regno(nframes) > regno(frame).  Worth 382 -> 380 and it puts
 *     eight of the nine named slots at the ROM's offsets.  The one still wrong is the
 *     three frame-loop invariants: ROM has 0x10=n-4, 0x14=n-0x14, 0x18=n-0x40 (created
 *     in source order, descending slots); we get 0x10=n-0x40, 0x18=n-0x14, 0x14=n-4,
 *     so one of the three is created by a different pass than in the ROM.
 *
 * ================================================================
 * MEASURED INERT
 * ================================================================
 *
 *  - every declaration-order permutation (see above), byte-identical each time;
 *  - merging the gBuffer draw-loop counter into `i` (Anim_Break's "a counter reused
 *    across several loops is ONE variable"): 543 encodings, frame 0x34.  It is the
 *    WRONG rule here -- the ROM does reuse r9 for both counters, but that is register
 *    sharing between two non-conflicting allocnos, not one variable.  The search loop
 *    inside the inner loop and the draw loop after it DO need separate pointers
 *    (`sg` vs `g`), which is the "one particle pointer per loop region" rule;
 *  - `fp = fns;` before vs after `fns[1] = ...`: no change with the pin.
 *
 * ================================================================
 * NOT A DIFFERENCE, AND IT COSTS AN HOUR IF YOU BELIEVE IT
 * ================================================================
 *
 * The ROM writes `ldr r3, .Ldd30c  @ 0x100` / `ldr r3, .Ldd310  @ 0` for the two
 * `REG_BG2PA = 0x100` / `REG_BLDCNT = 0` stores; gcc writes `ldrh r3, .L55` / `ldrh r3,
 * .L55+4` over the same `.word` pool.  THE ENCODINGS ARE IDENTICAL -- objcmp's first
 * difference is at index 45, past both -- because gas has no PC-relative `ldrh` and
 * assembles it as the word load.  Any text-level diff (tryc, or a hand-rolled aligned
 * diff) reports these as differences and they are not.  Same situation as Anim_Vine,
 * whose LANDED generated .s carries `ldrh r3, .L27`.
 *
 * ================================================================
 * WHOLE-FILE, AND NO ASM EDIT IS NEEDED
 * ================================================================
 *
 * rom_dd2ac_c_c_a.s holds this function alone (`grep -ci func_start` = 1) and no data
 * section, so the landing is a plain whole-file conversion -- no split_s.py, no
 * `.global` additions.  All eleven .rodata labels it reads (.Leeb48 .Leeb4b .Leeb4e
 * .Leeb54 .Leeb58 .Leeb5e .Leeb61 .Leeb71 .Leeb79 .Leeb80 .Leeb88) are ALREADY
 * `.global` in asm/rom_c9000/rom_dd2ac_c_c_c.s (lines 1888-1898), exported when
 * Anim_Vine landed.  datacheck.py cannot tell you this; the greps can.
 *
 * Signed-char access forms, for the record, because the two disagree inside one loop:
 * `.Leeb71[i & 7]` and `.Leeb79[u]` come out as the register-offset `ldrsb rd,[rn,rm]`
 * while `.Leeb80[u]` comes out as `ldrb / lsl #24 / asr #24`.  That is NOT a source
 * difference -- gcc needs the un-narrowed `(byte << 24)` in a register so that combine
 * can reuse its bit 31 as the sign for `Leeb80[u] / 2` (`lsr r0, #31 / add r0, r4, r0 /
 * asr r0, #1`).  Plain `extern signed char` on all three reproduces both forms.
 *
 * No per-file Makefile flag override applies to this stem.
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

extern int *iwram_3001eec[];
extern void *gPtrs[];
extern int ewram_2010018;
extern Part gBuffer[];

extern unsigned char Leeb48[] __asm__(".Leeb48");
extern unsigned char Leeb4b[] __asm__(".Leeb4b");
extern unsigned short Leeb4e[] __asm__(".Leeb4e");
extern unsigned char Leeb54[] __asm__(".Leeb54");
extern unsigned short Leeb58[] __asm__(".Leeb58");
extern unsigned char Leeb5e[] __asm__(".Leeb5e");
extern unsigned char Leeb61[] __asm__(".Leeb61");
extern signed char Leeb71[] __asm__(".Leeb71");
extern signed char Leeb79[] __asm__(".Leeb79");
extern signed char Leeb80[] __asm__(".Leeb80");
extern unsigned short Leeb88[] __asm__(".Leeb88");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int Random(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void BaseAnim_Growth(void *context, int variant)
{
    int **tbl;
    int **pp;
    register unsigned char *base __asm__("r11");
    void *ctx;
    State **slot;
    DrawFn fns[2];
    Part *p;
    Part *g;
    int i, j, k, n;
    int frame, nframes, th, arg;
    DrawFn *fp;

    tbl = (int **)iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = (void *)*pp;
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(1);
    REG_BG2PA = 0x100;
    REG_BLDCNT = 0;
    if (variant == 1) {
        LoadVFXFile(FILE_83, base, 1, 1);
    } else {
        LoadVFXFile(FILE_84, base, 1, 1);
    }
    slot = (State **)(base + 0x7828);
    if ((*slot)->f4 == 1) {
        REG_BG2X = 0xffff9000;
    }
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 1);
    fns[0] = (DrawFn)gPtrs[0xb8 / 4];
    BuildDraw2DFuncEx(0x2f, 7, 7, 7, 1);
    fp = fns;
    fns[1] = (DrawFn)gPtrs[0xbc / 4];
    nframes = Leeb5e[(*slot)->f18] * 4 + 0x38;
    {
        int *q = &ewram_2010018;
        j = 0;
        do {
            j++;
            *q = -1;
            q = (int *)((char *)q + 0x1c);
        } while (j != (0x80 << 3));
    }
    p = (Part *)(base + (0xe1 << 7));
    i = 0;
    do {
        int v;
        p->x = Leeb61[i] + (Random() & 7) - 4;
        p->y = i / 2 + 0x6c;
        v = (Random() & 0x3f) + 0x37;
        p->vy = v;
        if (Leeb4b[i % 3] < v) {
            p->vy = Leeb4b[i % 3];
        }
        p->t = i * 4 + 8;
        i++;
        p = (Part *)((char *)p + 0x1c);
    } while (i != 0x10);
    *(int *)(base + (0xef << 7)) = 1;
    *(int *)(base + 0x7784) = 0;
    arg = 0x90;
    {
        register void *tf __asm__("r0");
        tf = (void *)Task_BlitAnim;
        arg <<= 3;
        StartTask(tf, arg);
    }
    frame = 0;
    if (nframes != 0) {
      do {
        if (frame == nframes - 0x40) {
            _Func_80bd7dc(0x84);
        }
        if (frame >= nframes - 0x14 && frame < nframes - 4) {
            REG_BLDCNT = 0x3f44;
            REG_BLDALPHA = (nframes - frame - 5) | 0x1000;
        }
        if (frame < nframes - 4) {
            i = 0;
            while (i != Leeb5e[(*(State **)(base + 0x7828))->f18]) {
                th = i * 4 + 8;
                p = (Part *)(base + (0xe1 << 7) + i * 0x1c);
                if (frame == i * 4 + 9) {
                    *(int *)(base + 0x77a8) = 2;
                }
                if (frame > th) {
                    int m = i % 3;
                    int h = (frame - th) * 8;
                    if (h > p->vy) {
                        h = p->vy;
                    }
                    if (variant == 0) {
                        int w = Leeb48[m];
                        fp[i & 1](ctx, base + Leeb4e[m], p->x - w / 2,
                                  p->y - h, w, h);
                    } else {
                        int w;
                        if (h > Leeb71[i & 7]) {
                            h = Leeb71[i & 7];
                        }
                        w = Leeb54[m];
                        fp[i & 1](ctx, base + Leeb58[m], p->x - w / 2,
                                  p->y - h, w, h);
                    }
                }
                k = 0;
                while (k != (*(State **)(base + 0x7828))->f14) {
                    if (frame == th + 4) {
                        if ((i & 1) == 0) {
                            _PlaySound(0x85);
                        }
                        Func_80d6888((*(State **)(base + 0x7828))->ids[k],
                                     7, 5, k, 3);
                    }
                    k++;
                }
                if (frame == th + 4 || frame == th + 8) {
                    g = gBuffer;
                    n = 0;
                    while (n != (0x80 << 2)) {
                        if (g->t == -1) {
                            g->x = (Random() & 0xf) + p->x - 8;
                            g->y = (Random() & 0xf) + 0x50;
                            g->t = 0;
                            break;
                        }
                        g++;
                        n++;
                    }
                }
                i++;
            }
        }
        g = gBuffer;
        n = 0;
        do {
            if (g->t >= 0) {
                int u = g->t / 2;
                int off = 0x1e59;
                if (variant != 0) {
                    off = 0xaff;
                }
                fns[0](ctx, base + off + Leeb88[u], g->x - Leeb79[u],
                       g->y - Leeb80[u] / 2, Leeb79[u], Leeb80[u]);
                fp[1](ctx, base + off + Leeb88[u], g->x,
                      g->y - Leeb80[u] / 2, Leeb79[u], Leeb80[u]);
                g->t += 1;
                if (g->t == 0xe) {
                    g->t = -1;
                }
            }
            n++;
            g++;
        } while (n != (0x80 << 2));
        UpdateScreenShake(4, 4);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
      } while (frame != nframes);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}

/* BaseAnim_SpecialAttack (0x080e47b8) -- NON-MATCHING, 3233 encodings of 3380
 * differ.  THE LONGEST FUNCTION IN THE TREE at 3,070 ROM instructions.  Zero
 * shims: no register pin, no __asm__, production flags only.  Second pass.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_c9000/e47b8_SpecialAttack.c asm/rom_c9000/rom_e47b8.s \
 *       --func BaseAnim_SpecialAttack
 *
 * STATE: size 7796 against 7808 (12 bytes short), count 3377 against 3380 (3
 * short).  THE FRAME IS NOW THE ROM'S -- `sub sp, sp, #184` = 0xb8 -- and all 26
 * distinct [sp,#N] offsets match the reference exactly.  The first divergence has
 * moved from index 7 to index 41, and the leading ~40 instructions are exact.
 *
 * ================== THE SLOT MAP IS THE OBSERVABLE, NOT THE FRAME ==================
 * SPILL-SLOT ORDER IS DECLARATION ORDER, ONE FOR ONE.  Declared locals take the
 * HIGH offsets in declaration order (first declared = highest); compiler temps and
 * the outgoing-argument words take the low offsets.  Proved here with a swap probe
 * on two adjacent declarations.
 *
 * The consequence is large and reusable: A REFERENCE .s PUBLISHES THE ORIGINAL
 * DECLARATION ORDER OF ITS SPILLED LOCALS.  Read the slot map top-down and you
 * have the source's declaration block.  For this function that is, from the top:
 *   context, subanim, base, ctx, frame, view, gfx, bgx, bgvx, nframes, pA, pB,
 *   fnp, sav24..sav48, tgt, pS
 * then the compiler temps -- `subanim-2` at 0x14, `subanim-4` at 0x10, two more at
 * 0x0c and 0x08 -- then the two outgoing-argument words at 0x04 and 0x00.
 *
 * THE FIRST REVISION'S SLOT ANNOTATIONS WERE WRONG AND THAT IS WHAT HID THIS.  It
 * marked `pv` as sp+0x08 and `ax` as sp+0x0c; 0x14 and 0x10 demonstrably hold the
 * two `subanim` bias temps, so the ROM's DECLARED block ends at `pS` (0x18).
 *
 * The two surplus slots were two DECLARED LOCALS, not an arm and not a statement:
 *   - `pv` (= &vin) had a declared slot here; the ROM has none for it and
 *     materialises the address inline, `add r1, sp, #0x94` (asm:593).
 *   - `ax` (= pA->x / 2) had a declared slot here; the ROM DOES spill that value,
 *     but to [sp,#0xc] (asm:1949) -- BELOW the CSE temps.  A declared local can
 *     never land below a CSE temp, so `ax` was not a variable in the original:
 *     `pA->x / 2` was written at its four use sites and cse unified them.
 * So the fix was to reorder the declaration block to the ROM's published order and
 * delete both locals.  Both changes are needed: the reorder is worth NOTHING until
 * the frame closes, and then it is worth 123 instructions.
 *
 * ================== THE BISECTION I WAS TOLD TO RUN CANNOT WORK ==================
 * The plan was to binary-search the twelve dispatch arms by stubbing subsets and
 * reading `sub sp, #N`.  FRAME SIZE IS NOT MONOTONE IN CODE REMOVED, so no such
 * search is valid.  Measured, blanking the 14 non-empty arms:
 *   none 0xc0 | arm1 0xdc | arm3 0xe0 | arm4 0xbc | arm6 0xc4 | arm7 0xe0
 *   arm5 or arm9 0xc0 | 1,3 0xdc | 1,4 0xb4 | 3,4 0xbc | 1,3,4 0xb4
 *   1,3,4,5 0xb8 | 1,3,4,5,6,7,9 0xb8 | all 14 0xac
 * Removing ONE arm RAISES the frame by up to seven slots, because deleting code
 * changes which pseudos cross calls and the allocator re-decides globally.  Use
 * the SLOT MAP -- a set, comparable element by element -- not the frame total.
 *
 * ================== objcmp's DIFFERENCE COUNT SATURATES ==================
 * objcmp compares index by index with no alignment, so once one instruction is
 * missing everything after it differs.  Verified: this park's first revision, the
 * reorder-only variant and the swap probe ALL read EXACTLY 3250 despite hundreds
 * of changed slot offsets between them.  objcmp remains the authority on
 * byte-exactness -- OK is OK -- but ITS COUNT IS NOT A PROGRESS METRIC while the
 * instruction count differs.
 *
 * For a sensitive figure, `scratch_elev/b296c/align.py` (gitignored) aligns the two
 * raw encoding streams with an LCS.  MASKING RULES: NONE -- raw 16/32-bit
 * encodings compared verbatim, only insert/delete alignment tolerated, so a pool
 * load whose offset moved still counts as a difference; the single looseness is
 * that relocated words read as 0 in both streams and so compare equal.
 *   first revision      0xc0   1681/3380 equal (49.7%)   652 hunks
 *   reorder only        0xc0   1677/3380 (49.6%)         648
 *   pv+ax removal only  0xb8   1724/3380 (51.0%)         646
 *   both (this file)    0xb8   1847/3380 (54.6%)         603
 *
 * ================== THE NEXT DIVERGENCE, AND IT CASCADES ==================
 * At the LoadVFXFile(..., gBuffer, ...) chain, gcse hoists `gBuffer` into r5 and
 * then feeds every call with `adds r1, r5, #0`, where the ROM reloads
 * `ldr r1, =gBuffer` from a single pool word at each site.  Diagnostic only and
 * FLAG-CONDITIONAL, NOT PRODUCTION: `-fno-gcse` gives eight separate pool words
 * and frame 0xa8.
 *
 * The knock-on is global.  With r5 tied up we use r4 wherever the ROM uses r5
 * (`ldr r4,[sp,#92]` against the reference's `ldr r5,[sp,#92]`), which is very
 * likely the same cause as the eight `Func_80008d4` pointers landing in r7 where
 * the ROM uses r5, recorded in the first revision.  It also explains the known
 * surplus gBuffer pool word -- 9 against the ROM's 8 -- now localised to this
 * chain.
 *
 * SO THE NEXT MOVE IS THAT CHAIN, not another whole-function re-spelling and not
 * individual arms: find the spelling that stops gcse keeping gBuffer in a
 * call-saved register.  If it flips the r4/r5 pairing globally it should move a
 * large block at once.
 *
 * ================== CARRIED FORWARD FROM THE FIRST REVISION ==================
 * Still valid, and not re-derived here:
 *   - The RELOCATION evidence that the reconstruction is semantically close: the
 *     R_ARM_THM_CALL sequence matched in order 207 of 217, with nothing missing
 *     from the R_ARM_ABS32 symbol multiset.
 *   - Two call counts come out LOWER than the ROM's because CROSS-JUMPING MERGES
 *     fire here and not in the ROM -- our arms are MORE identical than the ROM's.
 *     Do not make those arms look alike.
 *   - Three levers that paid: a two-sided range test on a spilled counter wants
 *     the unsigned biased form (fold_range_test cannot build it from a >=/<= pair
 *     once a reload separates the comparisons); default-then-override is not an
 *     if/else; a conditional ARGUMENT is a duplicated call, not a ternary.
 *   - A register-masking normalizer read 90.7% on this reconstruction when objcmp
 *     said 96% of encodings were wrong.  It masked branch targets and pool
 *     constants too.  Any helper metric must state its masking rules; align.py
 *     above states that it has none.
 */
#include "decls.h"

void BaseAnim_SpecialAttack(void *context, int subanim)
{
    vec3_t posB;                 /* sp+0xac */
    vec3_t posA;                 /* sp+0xa0 */
    vec3_t vin;                  /* sp+0x94 */
    vec3_t vstep;                /* sp+0x88 */
    vec3_t vout;                 /* sp+0x7c */
    vec3_t vp;                   /* sp+0x70 */
    DrawFn fns[2];               /* sp+0x68 */
    unsigned char *base;         /* sp+0x5c */
    void *ctx;                   /* sp+0x58 */
    int frame;                   /* sp+0x54 */
    void *view;                  /* sp+0x50 */
    unsigned char *gfx;          /* sp+0x4c */
    int bgx;                     /* sp+0x48 */
    int bgvx;                    /* sp+0x44 */
    int nframes;                 /* sp+0x40 */
    vec3_t *pA;                  /* sp+0x3c = &posA */
    vec3_t *pB;                  /* sp+0x38 = &posB */
    DrawFn *fnp;                 /* sp+0x34 = &fns[0] */
    int sav24, sav28, sav2c, sav34, sav48;   /* sp+0x30..0x20 */
    int *tgt;                    /* sp+0x1c */
    vec3_t *pS;                  /* sp+0x18 = &vstep */
    int **tbl;
    int **pp;
    int *actor;
    int *ab;
    Part *q;
    Part *p;
    ClearFn clr;
    FillFn fill;
    int i, j, k, n, cnt;
    int h, yb, sx, sy, a, b;

    tbl = (int **)iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = (void *)*pp;
    view = *(void **)((char *)tbl - 0x6c);
    gfx = (unsigned char *)tbl[2];
    *(void **)(base + 0x7828) = context;
    if (subanim == 0xb || subanim == 8 || subanim == 0x20) {
        Func_80cdb24(0);
    } else {
        AnimStart(0);
    }
    REG_BLDALPHA = 0x1010;
    LoadVFXFile(FILE_73, gfx, 0, 0);
    LoadVFXFile(FILE_96, base, 1, 0);
    LoadVFXFile(FILE_99, gBuffer, 1, 0);
    Func_80df9d0(gBuffer, base + (0xa2 << 7), 0x28, 0x90 << 1);
    if (subanim == 5 || subanim == 0x17) {
        LoadVFXFile(FILE_7d, gBuffer, 1, 0);
    } else if (subanim == 0xc) {
        LoadVFXFile(FILE_a9, gBuffer, 1, 0);
    } else if (subanim == 6 || subanim == 0x1b) {
        LoadVFXFile(FILE_ce, gBuffer, 1, 0);
        LoadVFXFile(FILE_c4, ewram_2010c56, 1, 0);
    } else if (subanim == 0x1f) {
        LoadVFXFile(FILE_79, gBuffer, 1, 1);
    } else if (subanim == 8) {
        LoadVFXFile(FILE_c3, gBuffer, 1, 1);
    } else if (subanim == 0xe) {
        LoadVFXFile(FILE_6f, gBuffer, 1, 0);
    } else if (subanim == 0x1e) {
        LoadVFXFile(FILE_ce, gBuffer, 1, 0);
    } else if (subanim == 0x10) {
        LoadVFXFile(FILE_b8, gBuffer, 1, 0);
    } else if (subanim == 0x14) {
        LoadVFXFile(FILE_b4, gBuffer, 1, 0);
    } else if (subanim == 0x21 || subanim == 0x22) {
        LoadVFXFile(FILE_53, gBuffer, 1, 0);
    } else if (subanim != 0xb && subanim != 0x20) {
        LoadVFXFile(FILE_9e, gBuffer, 1, 0);
    }
    switch (subanim) {
    case 0: case 4: case 7: case 8: case 9: case 10: case 11: case 12:
    case 13: case 33:
        LoadVFXFile(FILE_94, ewram_2013c56, 1, 1);
        break;
    case 2: case 14: case 15: case 16: case 17: case 18: case 19:
        LoadVFXFile(FILE_92, ewram_2013c56, 1, 1);
        break;
    case 3: case 5: case 20: case 21: case 22: case 23: case 24: case 25:
    case 34: case 35:
        LoadVFXFile(FILE_8e, ewram_2013c56, 1, 1);
        break;
    case 1: case 6: case 26: case 27: case 28: case 29: case 30: case 31:
    case 32:
        LoadVFXFile(FILE_90, ewram_2013c56, 1, 1);
        break;
    case 100:
        LoadVFXFile(FILE_92, ewram_2013c56, 1, 1);
        break;
    }
    *(int *)(base + (0xef << 7)) = 2;
    if (subanim == 0xc) {
        *(int *)(base + 0x7784) = 0x4b;
    } else {
        *(int *)(base + 0x7784) = 0x32;
    }
    StartTask(Task_BlitAnim, 0x90 << 3);
    pA = &posA;
    GetBattleActorPos2((*(State **)(base + 0x7828))->ids[0], pA);
    pB = &posB;
    GetBattleActorPos2((*(State **)(base + 0x7828))->f8, pB);
    fnp = fns;
    BuildDraw2DFuncs((*(State **)(base + 0x7828))->f4, fnp);
    *(int *)(base + 0x77b4) = 0x18;
    *(int *)(base + 0x77b8) = 0;
    actor = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->f8);
    q = (Part *)(base + (0xe1 << 7));
    i = 0;
    do {
        q->x = (Random() & 0x3f) + 0x20;
        q->y = 0;
        q->z = 0;
        q->vx = Random() & 0xffff;
        q->vy = Random() & 0xffff;
        q->vz = Random() & 0xffff;
        i++;
        q++;
    } while (i != 0x40);
    _Actor_SetAnimSpeed(actor, 0);
    vin.x = actor[2];
    vin.y = actor[3] + (0xa0 << 15);
    vin.z = actor[4];
    sav24 = actor[9];
    sav28 = actor[10];
    sav2c = actor[11];
    sav34 = actor[13];
    sav48 = actor[18];
    actor[9] = 0;
    actor[10] = 0;
    actor[11] = 0;
    actor[13] = 0;
    actor[18] = 0;
    GetBattleActorPos2((*(State **)(base + 0x7828))->f8, pB);
    pB->x = pB->x / 2;
    _PlaySound(0xd4);
    frame = 0;
    do {
        q = (Part *)(base + (0xe1 << 7));
        n = 0;
        i = 0;
        do {
            if (q->x >= 0) {
                if (frame >= i / 4) {
                    k = 5;
                    InitMatrixStack();
                    MatrixRoll(q->vz);
                    MatrixPitch(q->vx);
                    MatrixYaw(q->vy);
                    Func_80e3944((vec3_t *)q, &vout);
                    vout.x = vout.x / 2 + pB->x;
                    if (subanim <= 7) {
                        vout.y = vout.y + pB->y - 8;
                    } else if (subanim == 0x23) {
                        vout.y = vout.y + pB->y + 0x2c;
                    } else {
                        vout.y = vout.y + pB->y + 0xc;
                    }
                    if (vout.z < -0x3c) {
                        vout.z = -0x3c;
                    }
                    if (vout.z > 0x3c) {
                        vout.z = 0x3c;
                    }
                    vout.z += 0x3c;
                    fnp[1](ctx, gfx + Data_ede48[k - 1], vout.x - 2,
                           vout.y - k, k, k * 2);
                    q->x -= 4;
                }
                n++;
            }
            i++;
            q++;
        } while (i != 0x40);
        if (subanim <= 7 && n <= 0x3f) {
            InitMatrixStack();
            MatrixSetLook(view, (char *)view + 0xc);
            Func_80e3944(&vin, &vout);
            vout.x = vout.x / 2;
            fns[0](ctx, ewram_2013c56, vout.x - 0xa, vout.y - 4, 0x14, 0x28);
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x20);
    /* asm:630 -- T1: per-subanim BG2 affine setup */
    if (subanim == 0xb) {
        REG_BG2PA = 0x100;
        if ((*(State **)(base + 0x7828))->f4 == 0) {
            REG_BG2X = (frame - pA->x) << 8;
        } else {
            REG_BG2X = (0x60 - pA->x) << 8;
        }
    } else if (subanim == 0x20) {
        REG_BG2PA = 0x100;
        if ((*(State **)(base + 0x7828))->f4 == 0) {
            bgx  = -(0x80 << 16);
            bgvx = 0xc0 << 12;
        } else {
            bgx  = 0x80 << 12;
            bgvx = -(0xc0 << 12);
        }
        REG_BG2X = (bgx >> 16) << 8;
    }
    if (subanim == 8) {
        REG_BG2PA = 0x100;
        REG_BG2X = (0x40 - pA->x) << 8;
        *(int *)(base + (0xef << 7)) = 1;
        *(int *)(base + 0x7784) = 0;
        clr = Func_80008d4;
        clr((void *)0x6004000, 0x80 << 7);
        clr(ctx, 0x80 << 7);
        REG_BLDCNT = 0;
    }
    if (subanim == 0x1f) {
        REG_BG2PA = 0x100;
        if ((*(State **)(base + 0x7828))->f4 == 0) {
            REG_BG2X = (0x20 - pA->x) << 8;
        } else {
            REG_BG2X = (0x60 - pA->x) << 8;
        }
    }
    /* asm:779 -- T2: four early-exit subanims */
    if (subanim == 0xf || subanim == 0x11 || subanim == 0x18 || subanim == 0x1a) {
        clr = Func_80008d4;
        clr((void *)0x6004000, 0x80 << 7);
        clr(ctx, 0x80 << 7);
        (*(State **)(base + 0x7828))->f1c = 0;
        StopTask(Func_80cd4b4);
        StopTask(Task_BlitAnim);
        gfree(0x2f);
        gfree(0x2e);
        _Actor_SetAnim(actor, 3);
        if (subanim == 0xf) {
            BaseAnim_ParticleSpray(context, 9);
        }
        if (subanim == 0x18) {
            Anim_MysticFlame(context);
        }
        if (subanim != 0x1a) {
            return;
        }
        BaseAnim_ParticleSpray(context, 8);
        return;
    }
    /* asm:838 -- T3: restore the caster */
    _Actor_SetAnimSpeed(actor, 0x10);
    actor[9] = sav24;
    actor[10] = sav28;
    actor[11] = sav2c;
    actor[13] = sav34;
    actor[18] = sav48;
    /* asm:852 -- T4: 0x23 -> Nova */
    if (subanim == 0x23) {
        clr = Func_80008d4;
        clr((void *)0x6004000, 0x80 << 7);
        clr(ctx, 0x80 << 7);
        (*(State **)(base + 0x7828))->f1c = 0;
        StopTask(Func_80cd4b4);
        StopTask(Task_BlitAnim);
        gfree(0x2f);
        gfree(0x2e);
        ((State *)context)->f18 = 3;
        BaseAnim_Nova(context, 2);
        return;
    }
    /* asm:885 -- T5: target record and the 6-frame approach step */
    tgt = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->ids[0]);
    pS = &vstep;
    pS->x = (tgt[2] - vin.x) / 6;
    pS->y = (tgt[3] - vin.y + (0xf0 << 13)) / 6;
    pS->z = (tgt[4] - vin.z) / 6;
    /* asm:927 -- T6a */
    i = 0;
    q = (Part *)(base + (0xe1 << 7));
    do {
        i++;
        q->t = 0;
        q++;
    } while (i != 0x40);
    /* asm:940 -- T6b */
    if (subanim != 0xe) {
        h = _Func_80b8530((*(State **)(base + 0x7828))->ids[0]) / 2;
        i = 0;
        q = (Part *)(base + (0xe1 << 7));
        do {
            q->x = tgt[2];
            q->y = h;
            q->z = tgt[4];
            if (subanim == 0x1f) {
                q->vx = ((Random() & 0xff) - 0x7f) << 12;
                q->vy = ((Random() & 0xff) - 0x40) << 10;
            } else {
                q->vx = ((Random() & 0xff) - 0x7f) << 12;
                q->vy = ((Random() & 0xff) - 0x40) << 12;
            }
            q->vz = ((Random() & 0xff) - 0x7f) << 12;
            q->t = i / 2 + 0x20;
            i++;
            q++;
        } while (i != 0x20);
    }
    /* asm:1012 -- T6c */
    if (subanim == 0xb) {
        LoadVFXFile(FILE_ab, base, 1, 1);
        LoadVFXFile(FILE_ac, gBuffer, 1, 0);
        REG_BLDALPHA = 0xe10;
    }
    if (subanim == 0x20) {
        LoadVFXFile(FILE_ad, base, 1, 1);
        LoadVFXFile(FILE_ae, gBuffer, 1, 0);
        REG_BLDALPHA = 0xe10;
    }
    /* asm:1053 -- T6d */
    if (subanim != 7 && subanim != 0xd && subanim != 0x12 &&
        subanim != 0xb && subanim != 0x20 && subanim != 0x13) {
        yb = 0;
        if (subanim != 0xc) {
            yb = 0xa0 << 13;
        }
        i = 0;
        p = (Part *)ewram_2014000;
        do {
            p->x = tgt[2];
            p->y = yb;
            p->z = tgt[4];
            if (subanim == 5 || subanim == 0x17) {
                p->vx = ((Random() & 0xff) - 0x7f) << 11;
                p->vy =  (Random() & 0xff) << 11;
                p->vz = ((Random() & 0xff) - 0x7f) << 11;
            } else if (subanim == 0x19) {
                p->vx = ((Random() & 0xff) - 0x7f) << 11;
                p->vy =  (Random() & 0x7f) << 10;
                p->vz = ((Random() & 0xff) - 0x7f) << 11;
            } else {
                p->vx = ((Random() & 0xff) - 0x7f) << 10;
                p->vy =  (Random() & 0x7f) << 10;
                p->vz = ((Random() & 0xff) - 0x7f) << 10;
            }
            p->t = 0;
            i++;
            p++;
        } while (i != 0x40);
    }
    /* asm:1148 -- T7a */
    if (subanim == 2 || subanim == 3 || subanim == 0xc ||
        subanim == 0x16 || subanim == 0x1d || subanim == 0x1c) {
        StartTask(Func_80dbb9c, 0x90 << 3);
    }
    /* asm:1169 -- T7b: nframes */
    if (subanim == 4 || subanim == 5 || subanim == 6 || subanim == 0x17
        || subanim == 0x1e || subanim == 0x1b || subanim == 0x21
        || subanim == 0x22 || subanim == 0x64) {
        nframes = 0x20;
    } else if (subanim == 0 || subanim == 1 || subanim == 2 || subanim == 3
               || subanim == 8 || subanim == 9 || subanim == 0xa
               || subanim == 0x16 || subanim == 0x19 || subanim == 0x1d
               || subanim == 0x1f || subanim == 0xe) {
        nframes = 0x30;
    } else {
        nframes = 0x14;
        if (subanim != 0x15) {
            if (subanim == 0xb || subanim == 0x20 || subanim == 0x14) {
                nframes = 0x28;
            } else if (subanim == 0x1c) {
                nframes = 0x40;
            } else {
                nframes = 0x50;
                if (subanim == 0xc) {
                    nframes = 0x40;
                }
            }
        }
    }
    /* asm:1243 -- the main frame loop */
    frame = 0;
    while (frame != nframes) {
        /* asm:1250 -- per-scanline wave table */
        if (subanim != 0xb && subanim != 0x20) {
            int *wq = (int *)(base + (0xd3 << 7));
            a = frame << 12;
            i = 0;
            do {
                *wq++ = ((0x80 << 11) - (sin(a) << 2)) >> 10;
                i++;
                a += 0x80 << 4;
            } while (i != 0xa0);
        }
        /* asm:1281 */
        if (frame <= 2) {
            GetBattleActorPos2((*(State **)(base + 0x7828))->f8, pB);
            pB->x = pB->x / 2;
            pB->y += 0x10;
        }
        /* asm:1301 */
        if (subanim != 0xb && subanim != 8 && subanim != 0x20
            && subanim != 0x21 && subanim != 0x22 && frame <= 0xb) {
            if ((*(State **)(base + 0x7828))->f4 == 0) {
                fns[0](ctx, base + (frame / 2) * 0xd80,
                       pB->x - 0x20, pB->y - 0x28, 0x30, 0x48);
            } else {
                fns[0](ctx, base + (frame / 2) * 0xd80,
                       pB->x, pB->y - 0x28, 0x30, 0x48);
            }
        }
        /* asm:1368 */
        switch (subanim) {
        case 0x21: Func_80e46f0(FILE_53); break;
        case 0xe:  Func_80e46f0(FILE_6f); break;
        case 0x1f: Func_80e46f0(FILE_79); break;
        case 8:    Func_80e46f0(FILE_c3); break;
        case 0: case 0xa: Func_80e46f0(FILE_8d); break;
        case 0xc: case 0xd: case 0x19: Func_80e46f0(FILE_bb); break;
        case 0x12: Func_80e46f0(FILE_b9); break;
        case 0x13: Func_80e46f0(FILE_c0); break;
        case 2: case 0x1d: Func_80e46f0(FILE_a4); break;
        case 1: case 0x1c: Func_80e46f0(FILE_a3); break;
        case 3: case 0x14: case 0x16: Func_80e46f0(FILE_b4); break;
        case 9:    Func_80e46f0(FILE_a0); break;
        case 5: case 0x17: Func_80e46f0(FILE_7d); break;
        }
        /* asm:1469 */
        if (subanim != 0xb && subanim != 8 && subanim != 0x20
            && (unsigned)(frame - 4) <= 0xb) {
            fnp[1](ctx, base + ((frame - 4) / 2) * 0x3c0 + (0xa2 << 7),
                   pA->x / 2 - 8, pB->y - 0x18, 0x14, 0x30);
        }
        /* asm:1510 */
        InitMatrixStack();
        MatrixSetLook(view, (char *)view + 0xc);
        /* asm:1518 */
        if (frame > 3) {
            i = 0;
            do {
                p = (Part *)(base + (i / 2) * 0x1c + (0xe1 << 7));
                if (p->t > 0) {
                    n = (p->t >> 4) + 1;
                    Func_80e3944((vec3_t *)p, &vp);
                    vp.x = vp.x / 2;
                    fnp[(i / 2) & 1](ctx, gfx + Data_ede48[n - 1],
                                     vp.x - n / 2, vp.y - n, n, n * 2);
                    Func_80e38b8(p, 0x3c, -0x1000);
                    p->t -= 1;
                }
                i++;
            } while (i != 0x80);
        }
        /* asm:1586 */
        if (subanim == 7 || subanim == 0xd || subanim == 0x12
            || subanim == 0x13) {
            if (frame == 0x32) {
                Func_80d6888((*(State **)(base + 0x7828))->f8, 7, -1, -1, 0);
            }
            if (frame == 0x4f) {
                Func_80d6888((*(State **)(base + 0x7828))->f8, 0, -1, -1, 0);
            }
            if (frame == 0xc) {
                p = (Part *)ewram_2014000;
                i = 0;
                do {
                    p->x = tgt[2];
                    p->y = 0xa0 << 13;
                    p->z = tgt[4];
                    p->vx = ((Random() & 0xff) - 0x80) << 10;
                    p->vy = ((Random() & 0xff) - 0x80) << 10;
                    p->vz = ((Random() & 0xff) - 0x80) << 10;
                    p->t = 0;
                    i++;
                    p++;
                } while (i != 0x40);
            }
            if (frame > 0xb) {
                ab = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->f8);
                h = _Func_80b8530((*(State **)(base + 0x7828))->f8) / 2;
                p = (Part *)ewram_2014000;
                i = 0;
                do {
                    if (p->t >= 0) {
                        int dx, dy, dz;
                        n = (i & 1) + 6;
                        Func_80e3944((vec3_t *)p, &vp);
                        vp.x >>= 1;
                        fns[0](ctx, gfx + Data_ede48[n - 1],
                               vp.x - n / 2, vp.y - n, n, n * 2);
                        Func_80e38b8(p, 0x3e, 0);
                        if (frame > i + 0x16) {
                            dx = (ab[2] - p->x) >> 8;
                            dy = (ab[3] + h - p->y) >> 8;
                            dz = (ab[4] - p->z) >> 8;
                            p->vx += dx;
                            p->vy += dy;
                            p->vz += dz;
                            if (dx > -0x1000 && dx < 0x1000 &&
                                dz > -0x1000 && dz < 0x1000) {
                                p->t = -1;
                            }
                        }
                    }
                    i++;
                    p++;
                } while (i != 0x20);
            }
        }
        /* asm:1781 */
        else if (subanim == 0x15) {
        }
        /* asm:1786 */
        else if (subanim == 6 || subanim == 0x1b) {
            if (frame >= 6 && frame <= 0x13) {
                i = 0;
                j = frame;
                do {
                    k = (j / 2) & 3;
                    fns[0](ctx, ewram_2010c56 + k * 0xb40,
                           pA->x / 2 - 8, 0, 0x18, 0x68);
                    i++;
                    j += 3;
                } while (i != 2);
            }
            if (frame >= 8 && frame <= 0x17) {
                i = 0;
                do {
                    k = i & 3;
                    n = Random() & 0xffff;
                    sx = ((sin(n) << 3) >> 16) + pA->x / 2 - Data_edeca[k] / 2;
                    sy = ((cos(n) << 5) >> 16) - Data_eded0[k] / 2;
                    gfree(0x2f);
                    gfree(0x2e);
                    BuildDraw2DFuncEx(0x2f, 7, 7, 3 | Leedd0[Random() & 3], 2);
                    ((DrawFn)iwram_3001f0c)(ctx, (char *)gBuffer + Data_edebe[k],
                                            sx, sy + 0x38,
                                            Data_edeca[k], Data_eded0[k]);
                    gfree(0x2f);
                    BuildDraw2DFuncs((*(State **)(base + 0x7828))->f4, fnp);
                    i++;
                } while (i != 3);
            }
        }
        /* asm:1933 */
        else if (subanim == 0xe) {
            gfree(0x2f);
            gfree(0x2e);
            if ((unsigned)frame <= 0x17) {
                a = frame * 0x20 - 0xe8;
                b = frame * 0x10 - 0x30;
                if (a > 0) {
                    a = 0;
                }
                while (b > 0x68) {
                    b -= 0x68;
                }
                BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
                ((DrawFn)gPtrs[0xbc / 4])(ctx, gBuffer, pA->x / 2 - 8,
                                          a + b - 0x68, 0x11, 0x68);
                ((DrawFn)gPtrs[0xbc / 4])(ctx, gBuffer, pA->x / 2 - 8,
                                          a + b, 0x11, 0x68 - b);
                ((DrawFn)gPtrs[0xbc / 4])(ctx, ewram_20106e8, pA->x / 2 - 0x11,
                                          a + 0x2f, 0x22, 0x41);
                gfree(0x2f);
                if (frame == 8) {
                    *(int *)(base + 0x77a8) = 8;
                }
                if (frame > 1) {
                    p = (Part *)(base + (0xe1 << 7));
                    i = 0;
                    cnt = 0;
                    do {
                        if (p->t == 0) {
                            p->x = tgt[2];
                            p->y = 0xa0 << 13;
                            p->z = tgt[4];
                            p->vx = ((Random() & 0xff) - 0x7f) << 12;
                            p->vy = ((Random() & 0xff) - 0x40) << 10;
                            p->vz = ((Random() & 0xff) - 0x7f) << 12;
                            p->t = i / 2 + 0x20;
                            cnt++;
                            if (cnt == 4) {
                                break;
                            }
                        }
                        i++;
                        p++;
                    } while (i != 0x40);
                }
            }
            BuildDraw2DFuncs((*(State **)(base + 0x7828))->f4, fnp);
        }
        /* asm:2090 */
        else if (subanim == 0x1f) {
            gfree(0x2f);
            gfree(0x2e);
            if (frame >= 4 && frame <= 0x17) {
                sx = pA->x / 2;
                BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
                ((DrawFn)gPtrs[0xbc / 4])(ctx, gBuffer, sx - 0x18, 0x30,
                                          0x18, 0x30);
                gfree(0x2f);
                BuildDraw2DFuncEx(0x2f, 7, 7, 7, 2);
                ((DrawFn)gPtrs[0xbc / 4])(ctx, gBuffer, sx, 0x30, 0x18, 0x30);
                gfree(0x2f);
            }
            BuildDraw2DFuncs((*(State **)(base + 0x7828))->f4, fnp);
        }
        /* asm:2163 */
        else if (subanim == 0x1e) {
            if (frame > 0xf) {
                REG_BLDALPHA = (0x20 - frame) | 0x1000;
            }
            if (frame > 5) {
                n = frame / 2 % 3;
                sx = pA->x / 2 - 0x14;
                fns[0](ctx, ewram_2010c56 + n * 0xa00, sx, 0x10, 0x28, 0x20);
                fns[0](ctx, ewram_2012a56 + n * 0x500, sx, 0x30, 0x28, 0x20);
                fns[0](ctx, ewram_2011156 + n * 0xa00, sx, 0x50, 0x28, 0x20);
            }
        }
        /* asm:2243 */
        else if (subanim == 5 || subanim == 0x17) {
            i = 0;
            p = (Part *)ewram_2014000;
            do {
                if (frame >= i / 2 + 4 && p->t <= 0xb) {
                    Func_80e3944((vec3_t *)p, &vp);
                    vp.x = vp.x / 2;
                    fns[0](ctx, (char *)gBuffer + ((p->t / 2) << 11),
                           vp.x - 0x10, vp.y - 0x20, 0x20, 0x40);
                    Func_80e38b8(p, 0x3c, 0x80 << 5);
                    p->t = p->t + 1;
                }
                i++;
                p++;
            } while (i != 0x10);
        }
        /* asm:2310 */
        else if (subanim == 4) {
        }
        /* asm:2319 */
        else if (subanim == 0xb) {
            a = frame << 9;
            sin(a);
            yb = ((cos(a) << 2) >> 16) + *(short *)((char *)pA + 6) + 0x10;
            if (frame <= 3) {
                fns[0](ctx, base, Leedd4[(*(State **)(base + 0x7828))->f4 * 7],
                       yb + Leede2[0], 0x39, 0x62);
            } else {
                if (frame <= 7) {
                    fns[0](ctx, base,
                           Leedd4[(*(State **)(base + 0x7828))->f4 * 7],
                           yb + Leede2[0], 0x39, 0x62);
                }
                fns[0](ctx, base + 0x15d2,
                       Leedd4[(*(State **)(base + 0x7828))->f4 * 7 + 1],
                       yb + Leede2[1], 0x63, 0x45);
                if ((unsigned)(frame - 4) <= 1) {
                    fill = Func_80008d8;
                    fill(ctx, 0x80 << 7, 0x3f3f3f3f);
                }
                if ((unsigned)(frame - 6) <= 1) {
                    fns[0](ctx, base + 0x3081,
                           Leedd4[(*(State **)(base + 0x7828))->f4 * 7 + 2],
                           yb + Leede2[2], 0x80, 0x5b);
                }
                if ((unsigned)(frame - 8) <= 1) {
                    fns[0](ctx, gBuffer,
                           Leedd4[(*(State **)(base + 0x7828))->f4 * 7 + 3],
                           yb + Leede2[3], 0x80, 0x5b);
                }
                if ((unsigned)(frame - 0xa) <= 1) {
                    fns[0](ctx, ewram_2012d80,
                           Leedd4[(*(State **)(base + 0x7828))->f4 * 7 + 4],
                           yb + Leede2[4], 0x80, 0x3b);
                }
                if ((unsigned)(frame - 0xc) <= 1) {
                    fns[0](ctx, ewram_2014b00,
                           Leedd4[(*(State **)(base + 0x7828))->f4 * 7 + 5],
                           yb + Leede2[5], 0x7a, 0x1d);
                }
                if ((unsigned)(frame - 0xe) <= 1) {
                    fns[0](ctx, ewram_20158d2,
                           Leedd4[(*(State **)(base + 0x7828))->f4 * 7 + 6],
                           yb + Leede2[6], 0x4c, 0x19);
                }
            }
        }
        /* asm:2535 */
        else if (subanim == 0x20) {
            bgx = bgx + bgvx;
            if (frame > 6) {
                bgvx = bgvx * 0x30 / 0x40;
            }
            REG_BG2X = (bgx >> 16) << 8;
            if ((unsigned)(frame - 0x10) <= 0xf) {
                REG_BLDALPHA = (0x10 - (frame - 0x10)) | 0x1000;
            }
            if ((unsigned)(frame - 4) <= 1) {
                fill = Func_80008d8;
                fill(ctx, 0x80 << 7, 0x3f3f3f3f);
            }
            if (frame <= 3) {
                if ((*(State **)(base + 0x7828))->f4 == 1) {
                    fns[0](ctx, base, 0, 0x18, 0x50, 0x68);
                } else {
                    fns[0](ctx, base, 0x30, 0x18, 0x50, 0x68);
                }
            } else {
                if (frame <= 7) {
                    if ((*(State **)(base + 0x7828))->f4 == 1) {
                        fns[0](ctx, base, 0, 0x18, 0x50, 0x68);
                    } else {
                        fns[0](ctx, base, 0x30, 0x18, 0x50, 0x68);
                    }
                }
                if ((*(State **)(base + 0x7828))->f4 == 1) {
                    fns[0](ctx, base + (0xf0 << 5), 0x10, 0x10, 0x50, 0x68);
                } else {
                    fns[0](ctx, base + (0xf0 << 5), 0x20, 0x10, 0x50, 0x68);
                }
                if ((unsigned)(frame - 6) <= 1) {
                    fns[0](ctx, base + (0xfa << 6), 0, 0x10, 0x80, 0x5b);
                }
                if ((unsigned)(frame - 8) <= 1) {
                    fns[0](ctx, gBuffer, 0, 0x10, 0x80, 0x5b);
                }
                if ((unsigned)(frame - 0xa) <= 1) {
                    fns[0](ctx, ewram_2012d80, 0, 0x10, 0x80, 0x3b);
                }
                if ((unsigned)(frame - 0xc) <= 1) {
                    fns[0](ctx, ewram_2014b00, 0, 0x10, 0x80, 0x1d);
                }
                if ((unsigned)(frame - 0xe) <= 1) {
                    fns[0](ctx, ewram_2015980, 0, 0x10, 0x80, 0x1a);
                }
            }
        }
        /* asm:2777 */
        else if (subanim == 0x14) {
            i = 0;
            do {
                if (frame >= i + 6 && frame < i + 0x12) {
                    k = (frame - i - 6) / 2;
                    sx = pA->x / 2 - (Data_ede9f[k] >> 1);
                    if (i & 1) {
                        sx += (i + 1) / 2 * 3;
                    } else {
                        sx -= (i + 1) / 2 * 3;
                    }
                    fnp[(i == 0 || ((i - 1) & 3) >= 2)](
                            ctx, (char *)gBuffer + Data_edeb2[k],
                            sx, Data_edeab[k] + 0x30,
                            Data_ede9f[k], Data_edea5[k]);
                }
                i++;
            } while (i != 0xc);
        }
        /* asm:2871 */
        else if (subanim == 0x10) {
            if (frame == 0) {
                q = (Part *)ewram_2014000;
                i = 0;
                do {
                    q->x  = (Random() & 0x7f) + 0x20;
                    q->y  = 0;
                    q->z  = 0;
                    q->vx = Random() & 0xffff;
                    q->vy = Random() & 0xffff;
                    q->vz = Random() & 0xffff;
                    i++;
                    q++;
                } while (i != 0x40);
                ((Part *)ewram_20146e4)->y = 0x9f;
            }
            q = (Part *)ewram_2014000;
            i = 0;
            do {
                if (q->x >= 0 && frame >= i / 2) {
                    n = i & 3;
                    InitMatrixStack();
                    MatrixPitch(q->vx);
                    MatrixYaw(q->vy);
                    Func_80e3944((vec3_t *)q, &vp);
                    vp.x = vp.x / 2 + pA->x / 2;
                    vp.y = vp.y + pA->y + 0x20;
                    fns[1](ctx, (char *)gBuffer + Leedea[n],
                           vp.x - 4, vp.y - 4, 8, 8);
                    q->x -= 6;
                    if (q->x < 0) {
                        if ((i & 7) == 0 || i == 0x3f) {
                            _PlaySound(0x85);
                            Func_80d6888((*(State **)(base + 0x7828))->ids[0],
                                         7, 5, 0, 4);
                        }
                    }
                }
                i++;
                q++;
            } while (i != 0x40);
        }
        /* asm:3008 */
        else if (subanim == 8) {
            if (frame >= 5 && frame <= 0x31) {
                if (frame > 0x19) {
                    h = 0xc4 - frame * 4;
                } else {
                    h = (frame << 4) - 0x40;
                }
                if (h > 0x60) {
                    h = 0x60;
                }
                fns[0](ctx, gBuffer, 0x30, 0x68 - h, 0x20, h);
            }
        }
        /* asm:3046 */
        else if (subanim == 0x21 || subanim == 0x22) {
            if (frame <= 5) {
                k = 6 - frame;
                if ((*(State **)(base + 0x7828))->f4 == 0) {
                    sx = pA->x / 2 + k * 6;
                } else {
                    sx = pA->x / 2 - k * 6;
                }
                sy = pA->y - k * 0xc + 0x18;
                fns[1](ctx, gBuffer, sx - 0x10, sy - 0x20, 0x20, 0x40);
            }
        }
        /* asm:3115 */
        else if (subanim == 0xc) {
            if (frame > 0x2f) {
                REG_BLDALPHA = (0x40 - frame) | 0x1000;
            }
            q = (Part *)ewram_2014000;
            i = 0;
            do {
                n = i % 3;
                Func_80e3944((vec3_t *)q, &vp);
                vp.x = vp.x / 2;
                fns[i & 1](ctx, (char *)gBuffer + n * (9 << 6),
                           vp.x - 0xc, vp.y - 0xc, 0x18, 0x18);
                Func_80e38b8(q, 0x3c, 1 << ((i & 3) + 0xb));
                q->t++;
                i++;
                q++;
            } while (i != 0x10);
        }
        /* asm:3193 */
        else if (subanim != 0x64) {
            i = 0;
            p = (Part *)ewram_2014000;
            do {
                if (frame >= i + 4) {
                    if (p->t <= 0x17) {
                        n = p->t / 4;
                        Func_80e3944((vec3_t *)p, &vp);
                        vp.x = vp.x / 2;
                        fns[i & 1](ctx, (char *)gBuffer + n * (9 << 7),
                                   vp.x - 0xc, vp.y - 0x18, 0x18, 0x30);
                        if (subanim == 0x19) {
                            Func_80e38b8(p, 0x3c, 0x80 << 3);
                        } else {
                            Func_80e38b8(p, 0x3c, 0x80 << 5);
                        }
                        p->t += 1;
                    }
                }
                i++;
                p++;
            } while (i != 0x10);
        }
        /* asm:3272 -- the common per-frame tail */
        if (subanim <= 7 && frame <= 5) {
            Func_80e3944(&vin, &vp);
            vp.x = vp.x / 2;
            fns[1](ctx, ewram_2013c56, vp.x - 0xa, vp.y - 4, 0x14, 0x28);
            vin.x += pS->x;
            vin.y += pS->y;
            vin.z += pS->z;
        }
        if (frame == 3) {
            _Func_80bd7dc(-1);
        }
        if (frame == 4) {
            _PlaySound(0x86);
        }
        if (frame == 6) {
            if (subanim == 4 || subanim == 5 || subanim == 7 || subanim == 0xd
                || subanim == 0x12 || subanim == 0x13 || subanim == 0x17
                || subanim == 0x22 || subanim == 0x64) {
                _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[0], 4);
                *(int *)(base + 0x77a8) = 8;
            } else if (subanim == 0x14 || subanim == 0xe || subanim == 0x21) {
                _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[0], 1);
                *(int *)(base + 0x77a8) = 2;
            } else if (subanim == 0x1e || subanim == 8) {
                _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[0], 3);
                *(int *)(base + 0x77a8) = 8;
            }
        }
        if (frame == 6) {
            Func_80d6888((*(State **)(base + 0x7828))->ids[0], 7, 5, 0, 4);
        }
        if (frame == 0xe) {
            Func_80d6888((*(State **)(base + 0x7828))->ids[0], 7, 5, 0, 4);
        }
        UpdateScreenShake(8, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    /* asm:3456 */
    if (subanim == 0x15) {
        clr = Func_80008d4;
        clr((void *)0x6004000, 0x80 << 7);
        clr(ctx, 0x80 << 7);
        (*(State **)(base + 0x7828))->f1c = 0;
        StopTask(Func_80cd4b4);
        StopTask(Task_BlitAnim);
        gfree(0x2f);
        gfree(0x2e);
        Anim_Impair(context);
        return;
    }
    if (subanim == 2 || subanim == 3 || subanim == 0xc || subanim == 0x16
        || subanim == 0x1c || subanim == 0x1d) {
        StopTask(Func_80dbb9c);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}

/* BaseAnim_SpecialAttack (0x080e47b8) -- NON-MATCHING, 3250 encodings of 3380
 * differ.  THE LONGEST FUNCTION IN THE TREE at 3,070 ROM instructions, and the
 * first attempt on it.  Zero shims: no register pin, no __asm__, production
 * flags only.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_c9000/e47b8_SpecialAttack.c asm/rom_c9000/rom_e47b8.s \
 *       --func BaseAnim_SpecialAttack
 *
 * NOT a true distance: size is 7792 against 7808 and the count is 3072 against
 * 3070, so this is 16 bytes short and two instructions long.  The frame is
 * `sub sp, #0xc0` against the ROM's `#0xb8` -- two surplus spill slots -- and
 * that is the very first difference, at index 7.
 *
 * ================== WHAT IS ACTUALLY ESTABLISHED ==================
 * The credible evidence that the reconstruction is semantically close is the
 * RELOCATIONS, which were measured independently of any shape metric:
 *   - the R_ARM_THM_CALL sequence matches IN ORDER 207 of 217 times (95.4%)
 *   - the R_ARM_ABS32 symbol multiset has nothing MISSING, one surplus gBuffer
 *     pool word
 * So the right calls happen in the right order, and no code is absent.  Of the
 * ten disagreeing call relocations, eight are a `Func_80008d4` function pointer
 * landing in r7 where the ROM uses r5.
 *
 * TWO relocation counts are LOWER than the ROM's (_SetBattleActorKnockback 2
 * against 3, _call_via_r4 42 against 43) and the cause is worth keeping:
 * CROSS-JUMPING MERGES THAT FIRE HERE AND NOT IN THE ROM, because our arms are
 * MORE identical than the ROM's.  Worked out at asm:3349 against asm:3390 -- the
 * ROM's first knockback arm reaches base+0x77a8 as `sub r5,#0x80` off the 0x7828
 * it already holds, while its third arm does `ldr r3,=0x77a8`, so jump.c can only
 * merge from `mov r3,#8`.  DO NOT "fix" those arms to look alike; the difference
 * between them is load-bearing.
 *
 * ================== A METRIC THAT FLATTERS -- READ THIS FIRST ==================
 * The attempt reported "structural match 2785/3070 = 90.7% with registers masked,
 * 60.9% without" and concluded "the residue is register allocation, not
 * semantics".  THE ARITHMETIC IS RIGHT AND THE CONCLUSION DOES NOT FOLLOW, so it
 * is recorded here as a caution rather than as a finding.
 *
 * The normalizer behind those numbers (`scratch_elev/b296/pfx2.py`, gitignored)
 * masks, under `--noreg`: every register (so `ldr r3,[r0,r4]` is equal to
 * `ldr r7,[r1,r2]`), every branch TARGET (any `beq` equals any `beq`), every
 * POOL CONSTANT (`ldr r3,=0x77a8` equals `ldr r3,=gBuffer` -- different values
 * AND different symbols count as matching), and every `[sp,#off]`.  What survives
 * is the opcode and inline immediates.  So 90.7% says the instruction MIX AND
 * ORDER are broadly right; it cannot separate allocation from semantics, because
 * masking registers also masks whether the right values reach the right places.
 * The honest headline is the objcmp figure: 3250 of 3380.
 *
 * This is the FOURTH flattering metric found in two batches -- after `--align`
 * on a byte-identical function reading 10, a line-level `.s` diff reporting 33 of
 * 33 when 13 were byte-identical, and `grep -c thumb_func_start` counting `@`
 * prose.  Any normalizer built to make a big residue legible must be reported
 * with its masking rules beside its number.
 *
 * ================== THREE LEVERS THAT PAID ==================
 * 3266 -> 3250 differ, and all three are reusable elsewhere:
 *   1. A two-sided range test on a SPILLED counter wants the unsigned biased
 *      form.  `frame >= 4 && frame <= 0xf` emits four instructions; the ROM has
 *      `sub r3,#4 / cmp #11 / bhi`.  fold_range_test builds that from
 *      `a == K || a == K+1` but not from a >=/<= pair on a variable it has
 *      spilled, because a reload separates the comparisons before fold sees them.
 *   2. Default-then-override is not an if/else.
 *      `yb = 0; if (subanim != 0xc) yb = 0xa0 << 13;` -- the if/else costs an
 *      extra `b` over the join.  Same shape at asm:1217 and asm:1236.
 *   3. A conditional ARGUMENT is a duplicated call, not a ternary -- 17
 *      instructions in the Asura arm.
 * Counterpart negative: inlining `(6 - frame) * 6` into duplicated arms raises
 * the shape figure but costs nine instructions.  Shape is not the target.
 *
 * ================== THE FRAME IS BRACKETED, AND BOTH SIDES ARE WORSE ==================
 * `#0xc0` against `#0xb8`, two surplus slots.  Merging scalars goes to 0xd0 and
 * 3298 differ; the documented per-region split across all twelve dispatch arms
 * goes to 0xc4 and 3271 differ at 3065 instructions.  So the ROM has neither
 * fewer nor more distinct scalars than the flat form -- the two slots come from
 * somewhere else, and the one-variable-per-region rule does not reach them.
 *
 * ================== SPLIT SHAPE, AND ONE CLAIM TO RE-CHECK ==================
 * Text/DATA split.  The `.s` holds exactly ONE function, and its .rodata is four
 * labels over a contiguous, isolated run at 0xeedd0..0xeedf4 -- 4, 14, 8 and 10
 * bytes -- wholly this object's.  stage1.ld names the object twice, :1904 for
 * .text and :1968 for .rodata.  datacheck requires all four exported:
 * .Leedd0, .Leedd4, .Leede2, .Leedea.
 *
 * The attempt concluded that because landing deletes all the text, "there is no
 * .s left to hang .global off, so all four labels must be defined from C" as
 * const arrays (with the note that a 4-byte element type pads .Leede2 and shifts
 * the last two blobs).  I DOUBT THAT and have not spent a build on it: a
 * rodata-only .s can carry `.global .Leedd0` perfectly well, which would keep the
 * ordinary route available.  Settle it at landing time, not from this note.
 *
 * ================== HONEST ASSESSMENT AND THE NEXT MOVE ==================
 * Not landable and not one round away.  What remains is roughly 285 instructions
 * of genuine shape residue spread over 344 one- and two-instruction hunks, plus a
 * global allocation difference, on a function with 238 basic blocks, 21 loops and
 * r8-r11 in use.
 *
 * The next move is NOT another whole-function re-spelling -- three were tried and
 * the two bracketing ones were worse.  The twelve dispatch arms are independent,
 * so the move is a PER-ARM BISECTION: compile each arm against its own window of
 * the reference and find which one spills.  That converts one 3,070-instruction
 * problem into twelve tractable ones, and it is the only route here that gets
 * cheaper rather than more expensive as it proceeds.
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
    vec3_t *pv;                  /* sp+0x08 = &vin  */
    vec3_t *pS;                  /* sp+0x18 = &vstep */
    DrawFn *fnp;                 /* sp+0x34 = &fns[0] */
    vec3_t *pA;                  /* sp+0x3c = &posA */
    vec3_t *pB;                  /* sp+0x38 = &posB */
    unsigned char *base;         /* sp+0x5c */
    void *ctx;                   /* sp+0x58 */
    unsigned char *gfx;          /* sp+0x4c */
    void *view;                  /* sp+0x50 */
    int *tgt;                    /* sp+0x1c */
    int **tbl;
    int **pp;
    int *actor;
    int *ab;
    Part *q;
    Part *p;
    ClearFn clr;
    FillFn fill;
    int frame;                   /* sp+0x54 */
    int nframes;                 /* sp+0x40 */
    int bgx;                     /* sp+0x48 */
    int bgvx;                    /* sp+0x44 */
    int ax;                      /* sp+0x0c */
    int i, j, k, n, cnt;
    int h, yb, sx, sy, a, b;
    int sav24, sav28, sav2c, sav34, sav48;

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
    pv = &vin;
    pv->x = actor[2];
    pv->y = actor[3] + (0xa0 << 15);
    pv->z = actor[4];
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
    pS->x = (tgt[2] - pv->x) / 6;
    pS->y = (tgt[3] - pv->y + (0xf0 << 13)) / 6;
    pS->z = (tgt[4] - pv->z) / 6;
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
                ax = pA->x / 2;
                a = frame * 0x20 - 0xe8;
                b = frame * 0x10 - 0x30;
                if (a > 0) {
                    a = 0;
                }
                while (b > 0x68) {
                    b -= 0x68;
                }
                BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
                ((DrawFn)gPtrs[0xbc / 4])(ctx, gBuffer, ax - 8,
                                          a + b - 0x68, 0x11, 0x68);
                ((DrawFn)gPtrs[0xbc / 4])(ctx, gBuffer, ax - 8,
                                          a + b, 0x11, 0x68 - b);
                ((DrawFn)gPtrs[0xbc / 4])(ctx, ewram_20106e8, ax - 0x11,
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
            Func_80e3944(pv, &vp);
            vp.x = vp.x / 2;
            fns[1](ctx, ewram_2013c56, vp.x - 0xa, vp.y - 4, 0x14, 0x28);
            pv->x += pS->x;
            pv->y += pS->y;
            pv->z += pS->z;
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

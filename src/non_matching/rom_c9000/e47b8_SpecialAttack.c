/* BaseAnim_SpecialAttack (0x080e47b8) -- NON-MATCHING, 3080 encodings of 3380
 * differ.  THE LONGEST FUNCTION IN THE TREE at 3,070 ROM instructions.  Zero
 * shims: no register pin, no __asm__, production flags only.  Third pass.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_c9000/e47b8_SpecialAttack.c asm/rom_c9000/rom_e47b8.s \
 *       --func BaseAnim_SpecialAttack
 *
 * PROGRESS ON THIS FUNCTION MUST BE READ WITH tools/aligncmp.py, NOT objcmp's
 * count -- the count saturates while the instruction count differs (three earlier
 * candidates all read exactly 3250 despite hundreds of changed slot offsets).
 *   pass 1   0xc0   1681/3380 = 49.7%   652 hunks   objcmp 3250
 *   pass 2   0xb8   1847/3380 = 54.6%   603 hunks   objcmp 3233
 *   pass 3   0xb8   1959/3380 = 58.0%   588 hunks   objcmp 3080   <- this file
 * Size 7788 against 7808, count 3374 against 3380, first divergence index 41.
 * Frame `sub sp, sp, #184` (0xb8) and all 26 [sp,#N] offsets match the reference;
 * ANY change that moves either is wrong -- it is one grep of the generated asm.
 *
 * ================== CORRECTION: IT IS cse1, NOT gcse ==================
 * Pass 2 attributed the gBuffer hold to gcse and cited `-fno-gcse` as confirming
 * it.  BOTH WERE WRONG, and the correction is the main result of pass 3.
 *
 * The production `-da` dumps settle it.  At .00.rtl the two loads are SEPARATE
 * pseudos with one use each -- the ROM's shape:
 *     (insn 102 (set (reg:SI 81) (mem/u/f:SI (symbol_ref/u:SI ("*.LC4")) 4)))
 *     (insn 114 (set (reg:SI 82) (mem/u/f:SI (symbol_ref/u:SI ("*.LC4")) 4)))
 * By .03.cse insn 114 is ALREADY GONE and insn 122 reads (reg:SI 81).
 * .07.gcse changes nothing there.
 *
 * `-fno-gcse` does NOT break the hold: under it the asm is still
 * `ldr r5, .L382+16 / mov r1, r5 ... mov r0, r5`, byte-for-byte the production
 * shape.  Its 8-against-9 gBuffer pool words -- the signal pass 2 read as
 * confirmation -- came from POOL RE-REGIONING after that flag moved the frame to
 * 0xa8, nothing to do with the hold.  A flag that changes the symptom you are
 * counting for an unrelated reason is worse than no evidence.
 *
 * WHY NO SOURCE SPELLING REACHES IT.  The Thumb backend materialises gBuffer at
 * EXPAND time as a constant-pool MEM (`*.LC4`, alias set 4) carrying
 * REG_EQUAL (symbol_ref "gBuffer").  cse commons it through its pool-constant
 * equivalence, and a `bl` does not invalidate that, because the value lives in a
 * pseudo and a (symbol_ref) is a constant.  All 22 ROM sites share ONE
 * force_const_mem entry -- the same symbol always hashes to the same *.LC4 -- so
 * no spelling of the argument can produce a second pool entry.
 *
 * At .17.lreg insn 102 does carry a genuine REG_EQUIV (symbol_ref "gBuffer"), so
 * reload COULD rematerialise it at every use, which is exactly the ROM's shape.
 * Global-alloc hands it r5 first instead, and that is an allocno_compare priority
 * decision reachable only through register pressure, not through source text.
 * FORCING IT WOULD REQUIRE A PIN, and none is used here.
 *
 * Measured and inert, all production flags, all leaving the hold intact:
 * `&gBuffer[0]`; a distinct block-scope local per site (cse merges them, as
 * documented); two locals split across the chain; and the assignment placed at the
 * top of the function, above the guard `if`, and immediately before first use.
 * A `((Part*)0x02010000)` macro reads 57.3%/596 but DELETES ALL EIGHT gBuffer
 * relocations -- rejected, it is not the same object.
 * Diagnostic only, non-production: -fno-gcse 48.1%/650; -fno-cse-follow-jumps
 * 57.1%/601 at frame 0xb8; -fno-rerun-cse-after-loop 49.4%/664;
 * -fno-expensive-optimizations 39.0%/643; -ffixed-r4 48.7%/636; -fcall-saved-r4
 * 49.3%/652.  None removes the hold.
 *
 * ================== WHAT DID MOVE, AND IT IS NOT THE HOLD ==================
 * Naming the chain's gBuffer as ONE shared pointer local (`Part *gb`, declared
 * LAST so it cannot perturb the slot map) across the 13 chain sites is worth
 * 54.6% -> 58.0% and 603 -> 588 hunks.  The hold survives -- it is merely hoisted
 * one call earlier -- so the gain is from the naming, not from breaking it.  It
 * also removes the surplus gBuffer pool word: 9 -> 8, the ROM's count.
 *
 * Three controls, because a 3.4-point move on a helper metric deserves them:
 *   (a) naming the UNRELATED constant 1 at the same 13 sites is byte-for-byte
 *       identical to the baseline (1847/603) -- so this is specific to gBuffer,
 *       not naming noise;
 *   (b) it is dose-dependent -- all 13 sites 58.0%, omitting the Func_80df9d0
 *       site 54.8%, omitting the FILE_99 site 55.7%, and extending to all 30 uses
 *       in the function 52.2% WITH THE FRAME BROKEN to 0xbc (rejected);
 *   (c) the hunk classes that improve are structural (same-mnemonics-different-
 *       operands 126 -> 115; "other" 372 -> 358 hunks) while pure register
 *       substitution is UNCHANGED at 176 hunks.
 *
 * ================== THE r4/r5 HYPOTHESIS IS DEAD ==================
 * Pass 2 predicted that breaking the hold would flip an r4/r5 pairing globally.
 * It does not, and the pairing was never the shape of the problem.  Register
 * histograms over objdump text on BOTH sides (the .s text is useless here -- gcc
 * prints `lsl r0, r0, #7` where the reference prints `lsl r0, #7`):
 *     ref   r0 687  r1 547  r2 594  r3 1114  r4  66  r5 400  r6 121  r7 116
 *     ours  r0 668  r1 439  r2 541  r3 1127  r4 277  r5 324  r6 111  r7 166
 * r4 is +211 where the hold can account for ~22.  The substitution hunks show a
 * DIFFUSE permutation -- r5->r7 29, r5->r4 28, r0->r5 22, r6->r5 19, r2->r0 17,
 * r6->r7 15 -- not one pair.  High-register traffic is EQUAL (ref 177, ours 180),
 * so we are not spilling more; the same values sit in different registers.  This
 * is the corpus-wide REG_ALLOC_ORDER class, on the largest available instrument.
 *
 * The hold itself is worth AT MOST ~40 of the differing encodings -- 21
 * `adds rN, r5, #0`, one insert at index 48, and the index-41/45 pool
 * displacement (our block is 2 bytes longer, so gcc dumps the second minipool one
 * instruction early: ref `bl / b / .short 0 / pool`, ours `b / pool / bl`).
 * About 3% of the residue.  Pass 2's expectation that it would "move a large
 * block at once" was wrong.
 *
 * ================== THE MIX IS NOW ALMOST EXACT ==================
 * Mnemonic deltas against the reference: bl -3 (the three cross-jump merges),
 * add +1, asr +2, b +1, bgt +1, bhi -1, ble +1, cmp +1, lsl -3, lsr +2, str +2,
 * sub -1, ldr -1.  IGNORE the ldrh +13 / ldr -13 pair: those are `ldrh rD, .Lxx`
 * pool loads, which assemble to the SAME Thumb-1 encoding as `ldr rD, .Lxx`
 * (docs/elevation.md:2309).
 * Of the 588 hunks, 232 are net-zero insert/delete pairs and 176 are pure register
 * substitution.  SO THE RESIDUE IS REGISTER ASSIGNMENT PLUS LOCAL INSTRUCTION
 * ORDERING, NOT CODE SHAPE.  The relocation multiset now differs from the
 * reference only by _call_via_r5 x8 -> _call_via_r7 x8, one _call_via_r4, and the
 * known _SetBattleActorKnockback cross-jump merge.
 *
 * ================== SECOND FINDING: TWO BYTE TABLES ==================
 * Data_edeca and Data_eded0 are BYTE tables, not unsigned short.  Reference
 * asm/rom_c9000/rom_e47b8.s:1905,1908 reads them with `ldrb r3, [r0, r2]` on the
 * UNSCALED counter, immediately after the halfword read of Data_edebe
 * (`lsl r3, r1, #1 / ldrh r1, [r2, r3]`), which independently confirms the
 * neighbour is genuinely halfword.  Declaring both `unsigned char` takes ldrb to
 * exactly the ROM's 25 and removes 4 spurious ldrh.  It is NEUTRAL on aligncmp,
 * so a metric-driven search would never have found it.
 * Pass 3 flagged this as needing re-screening because decls.h might be shared
 * across this directory's parks.  IT IS NOT: decls.h is used by this park alone
 * (checked), having been added with the park in 6925b736, so there is nothing to
 * re-screen and it is applied here.
 *
 * ================== NEXT ==================
 * Not this chain -- it is closed, and closed for a mechanical reason rather than
 * for want of spellings.  Two candidates, in order:
 *   1. The three cross-jump merges (bl -3).  Our arms are MORE identical than the
 *      ROM's, so jump.c merges where the ROM could not; the first revision worked
 *      out one case at asm:3349 against asm:3390.  Do NOT make those arms look
 *      alike -- the differences between them are load-bearing.
 *   2. The REG_ALLOC_ORDER question.  This function is now the best instrument in
 *      the corpus for it: the instruction mix is within +/-3 per mnemonic while
 *      176 hunks are pure register substitution, so a change there is visible
 *      without any confounding shape difference.
 *
 * ================== CARRIED FORWARD ==================
 *   - The spill-slot rule this function produced: declared locals take the HIGH
 *     offsets in declaration order, temps and outgoing-arg words the low ones, so
 *     a reference .s PUBLISHES its declaration order and a value spilled below the
 *     temps was never a declared variable.  See docs/elevation.md.
 *   - Do not bisect on the frame TOTAL: it is not monotone in code removed.
 *   - Three levers that paid in pass 1: the unsigned biased two-sided range test
 *     on a spilled counter; default-then-override rather than if/else; a
 *     conditional ARGUMENT as a duplicated call rather than a ternary.
 *   - RELOCATION evidence of semantic completeness: 207 of 217 THM_CALL in order,
 *     nothing missing from the ABS32 multiset.
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
    Part *gb;

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
    gb = gBuffer;
    REG_BLDALPHA = 0x1010;
    LoadVFXFile(FILE_73, gfx, 0, 0);
    LoadVFXFile(FILE_96, base, 1, 0);
    LoadVFXFile(FILE_99, gb, 1, 0);
    Func_80df9d0(gb, base + (0xa2 << 7), 0x28, 0x90 << 1);
    if (subanim == 5 || subanim == 0x17) {
        LoadVFXFile(FILE_7d, gb, 1, 0);
    } else if (subanim == 0xc) {
        LoadVFXFile(FILE_a9, gb, 1, 0);
    } else if (subanim == 6 || subanim == 0x1b) {
        LoadVFXFile(FILE_ce, gb, 1, 0);
        LoadVFXFile(FILE_c4, ewram_2010c56, 1, 0);
    } else if (subanim == 0x1f) {
        LoadVFXFile(FILE_79, gb, 1, 1);
    } else if (subanim == 8) {
        LoadVFXFile(FILE_c3, gb, 1, 1);
    } else if (subanim == 0xe) {
        LoadVFXFile(FILE_6f, gb, 1, 0);
    } else if (subanim == 0x1e) {
        LoadVFXFile(FILE_ce, gb, 1, 0);
    } else if (subanim == 0x10) {
        LoadVFXFile(FILE_b8, gb, 1, 0);
    } else if (subanim == 0x14) {
        LoadVFXFile(FILE_b4, gb, 1, 0);
    } else if (subanim == 0x21 || subanim == 0x22) {
        LoadVFXFile(FILE_53, gb, 1, 0);
    } else if (subanim != 0xb && subanim != 0x20) {
        LoadVFXFile(FILE_9e, gb, 1, 0);
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

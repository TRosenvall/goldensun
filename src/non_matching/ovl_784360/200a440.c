/* OvlFunc_884_200a440 (0x0200a440) -- NON-MATCHING.
 *
 * NON-MATCHING, 12 of 136 encodings  (RE-MEASURED, batch 338 brief B; the
 *   park's figure stands).  INSTRUCTION COUNTS AGREE -- ref 124, ours 124,
 *   stream 136/136, 292 bytes each side, all relocations identical, dsize 0.
 *   A DISTANCE, not phase.  NOT POOL-INFLATED -- all 12 are real instructions.
 *   First difference at stream index 29.
 *
 * asm/overlays/rom_784360/ovl_30_c_c_c_a_a_c_c_c.s (1 function, confirmed by
 * tools/piece.py).  tools/datacheck.py is silent: no data sections, whole-file
 * conversion, no exports beyond the function.  Pin-free.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_784360/200a440.c \
 *     asm/overlays/rom_784360/ovl_30_c_c_c_a_a_c_c_c.s --func OvlFunc_884_200a440
 *
 * ***********************************************************************
 * *** READ src/non_matching/rom_8a000/8096ddc.c FIRST.  IT CARRIES THE
 * *** WHOLE MECHANISM, THE BROKEN CLOSURE AND THE NEXT STEP.
 * ***********************************************************************
 *
 * ========== BATCH 338: THE 12 EXACTLY, AND A NEW CONFOUND NAMED ==========
 *
 * Ten of the twelve are the setup block of the loop body, and they are a
 * two-register exchange plus the ORDER that follows from it:
 *
 *   rom   str r3,[r0,#0x14] / mov r3,r0 / ldr r5,[r0,#0x50] / add r3,#0x55 /
 *         mov r2,#0x0 / ldr r1,=0x0 / strb r2,[r3,#0x0] / add r3,#0xf /
 *         strh r2,[r3,#0x0] / mov r8,r1
 *   ours  mov r2,r0 / add r2,#0x55 / str r3,[r0,#0x14] / ldr r1,=0x0 /
 *         mov r3,#0x0 / ldr r5,[r0,#0x50] / strb r3,[r2,#0x0] / add r2,#0xf /
 *         mov r8,r1 / strh r3,[r2,#0x0]
 *
 * The ROM carries the f55/f64 ADDRESS in r3 and the stored ZERO in r2; we do
 * the reverse.  The other two (idx 75, 76) are the `neg r1,r1` / `strh r3,
 * [r5,#0x8]` pair swapped, and batch 316 established that they are DOWNSTREAM
 * OF THE SAME r2/r3 BLOCKER -- one fact with two symptoms, not two parts.
 *
 * *** ANY EXTRA LOCAL MOVES THE FRAME, SO HALF THIS PARK'S NEGATIVE LIST IS
 * *** UNINTERPRETABLE. ***  The body holds `struct Actor *arr[2]` and the
 * function reaches it with `lsl r3,r7,#0x2 / mov r2,r10 / str r0,[r3,r2]`.
 * Adding one `int` local to carry the zero re-measures as **131 of 136, dsize
 * 8, RELOCATION DIFFERENCE, first difference at index 7** -- a whole-function
 * perturbation that says nothing about the zero.  The same trap was found
 * independently on OvlFunc_905_2008a68 this batch.  So a probe here must
 * either add no local or reuse a dead one, and the park's figures for
 * spellings that introduced a carrier should be treated as unmeasured.
 *
 * MEASURED AT 12 (inert), batch 338, ten spellings of the two zero stores in
 * `scratch_elev/b338/B/v5`:
 *   * BLK-u8 + PLAIN-u16 (`((struct ZB *)p)->b = 0;` then
 *     `*(unsigned short *)p = 0;`)                             12
 *   * `p = (unsigned char *)a + 0x55;` instead of `&a->f55`     12
 * WORSE:
 *   * both stores through one pointer expression,
 *     `((struct ZH *)(p + 0xf))->h = 0;` with no `p += 0xf`     16
 *   * `a->f55 = 0; a->f64 = 0;` with no pointer at all          17
 *   * PLAIN-u8 + BLK-u16                                        21
 *   * PLAIN-u8 + PLAIN-u16                                      79, RELOCDIFF
 *   * the two stores in the other order (`p = &a->f55 + 0xf`
 *     first, then `p -= 0xf`)                                   79, RELOCDIFF
 *   * any shared `int` zero carrier (three spellings)           131, RELOCDIFF
 * autosweep: both cheap axes CLOSED -- 8 flag groups (best 12) and 15 anchor
 * placements (best 12).  TWO FLAG RESULTS WORTH KEEPING: `-fno-strict-aliasing`
 * is **48 of 136**, i.e. this body genuinely depends on strict aliasing for 36
 * encodings it currently gets RIGHT; and `-fno-schedule-insns2` is 98, so the
 * body's agreement past index 29 is substantially sched2's doing.
 *
 * ========== WHAT STILL STANDS, BATCHES 295-316 ==========
 *
 * 1. *** "FIX ONE AND THE OTHER FOLLOWS" IS FALSE. ***  Under
 *    -fno-expensive-optimizations the twin Func_8096ddc reads 95 of 146 with
 *    its residue entirely closed (first diff at index 120) while this function
 *    reads 125 of 136 with a global perturbation (first diff at index 7).  Same
 *    flag, opposite behaviour.  *** THEY SHARE A MECHANISM, NOT A FIX. ***  A
 *    landing on 8096ddc must be RE-MEASURED here, not assumed.
 * 2. The tail tie is an INSN_LUID decision, not a CLASS-rung one, because
 *    rank_for_schedule's class test short-circuits on `insn_cost (...) == 1`
 *    and puts a data-dependent insn in class 3 anyway.  The alias-set lever
 *    cannot reach it for a better reason than batch 315 gave: the dependence
 *    that would have to be removed is a REGISTER ANTI-DEPENDENCE ON r3, not a
 *    memory one, and both MEMs are already in the same alias set at disjoint
 *    offsets.  Full derivation in 8096ddc.c.
 * 3. From .17.lreg / .18.greg, exactly parallel to Func_8096ddc and re-checked
 *    in batch 338:
 *      reg 37  p (walk ptr)  8 refs / 7 insns in block 2, set 2 times,
 *                            DIES IN 2 PLACES, pref STACK_REG -> r2
 *      reg 50  dead QI 0     2 refs / 2 insns in block 2, 1 bytes -> r3
 *    `";; 11 regs to allocate: 34 60 68 37 35 32 38 55 172 119 36"`.
 *    THE STACK_REG GATE and its four compiler-source citations (arm.h:989,
 *    arm.md:496, arm.h:1095, regclass.c:1459-1462) are all right.  The gate is
 *    a reason the walk pointer is GLOBAL; it is NOT a reason the residue is
 *    unreachable -- that was the conflation, and 8096ddc.c sets out why.
 *    BATCH 338 ADDS ONE NUMBER TO IT: both reg 37 and reg 50 are BLOCK-LOCAL
 *    to block 2, so this is a local-alloc ordering (QTY_CMP_PRI,
 *    local-alloc.c:1496-1498), and by that formula the address quantity
 *    (8 refs over 7 insns, 3*8/7*10000 = 34285) outranks the zero (2 refs over
 *    2, 1*2/2*10000 = 10000) and SHOULD take r3, the head of REG_ALLOC_ORDER.
 *    It does not, because `pref STACK_REG` sends it down find_free_reg's
 *    alternate-class path.  *** THAT IS THE SHARPEST AVAILABLE STATEMENT OF
 *    THE BLOCKER: the address quantity has the priority and loses the register
 *    to its own preferred class. ***
 * 4. The park's original diagnosis, "the dead QImode zero steals an address
 *    register", is the right target.  The flag's actual route on the twin is
 *    that the dead QImode zero CEASES TO EXIST AS A PSEUDO, while the walk
 *    pointer's `pref STACK_REG` does not move at all.
 *
 * ===== NEXT STEP (shared with the whole family) =====
 * Find a source spelling of the f55 byte store whose zero does NOT create a
 * second, DEAD QImode pseudo, WITHOUT ADDING A LOCAL, while the SImode zero
 * stays SHARED between the strb and the strh.  Every spelling swept so far
 * varies the CARRIER MEMBER'S TYPE; none has removed the dead pseudo.
 * LEVER 5 (callee return type) is EXHAUSTED: all 5 `extern void` callees
 * flipped to `extern int` -- __Sprite_SetAnim, __Func_8003f3c, __PlaySound,
 * OvlFunc_884_200a3ec, OvlFunc_884_200a39c -- ALL INERT at 12, dsize 0, no
 * relocation change.
 *
 * ===== FAMILY =====
 * A near-twin of OvlFunc_887_200968c (identical but for the leading
 * __PlaySound), of OvlFunc_897_200aeb0, and of the parked OvlFunc_883_200dd68 /
 * OvlFunc_882_200c41c.  tools/dupfuncs.py adds two facts the family notes did
 * not have: OvlFunc_883_200dd68 is a BYTE DUPLICATE of OvlFunc_881_200c058
 * (asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_c.s), and 200aeb0's overlay-mate
 * OvlFunc_897_200b01c is a byte duplicate of OvlFunc_896_200c49c -- neither
 * partner is parked with the family.
 *
 * ===== MEASURED NEGATIVES, ALL AT THE BASELINE OF 12 =====
 * The two-address spellings (`p2 = a + 0x64` independently, `p2 = p + 0xf`,
 * `((struct ZH *)(p + 0xf))`) all move the ZERO to r2 and give the f64 address
 * r3 correctly, leaving only the f55 address in r1 -- the cleanest statement of
 * the blocker.  Inert: every bitfield spelling of the ZB/ZH carrier members and
 * all four combinations; BLK-u8 + PLAIN-u16.  The r3 pin on the walk pointer
 * costs 4 instructions here (140 encodings, 300 bytes, 131 differ) at all three
 * declaration scopes, so it is not even usable as a diagnostic.
 * *** CAUTION: all figures at 12, and none of them bears on the
 * dead-QImode-pseudo question, which is the one that matters. ***
 */
struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

struct Spr {
    unsigned char pad0[5];
    unsigned char f5_a : 5;
    unsigned char f5_b : 1;
    unsigned char f5_c : 2;
    unsigned char pad6;
    unsigned char f7_a : 6;
    unsigned char f7_b : 2;
    unsigned short f8_a : 10;
    unsigned short f8_b : 2;
    unsigned short f8_c : 4;
    unsigned char pad_a[0x12];
    unsigned char f1c;
    unsigned char f1d;
    unsigned char pad1e[8];
    unsigned char f26;
    unsigned char pad27;
    unsigned char *f28;
};

struct Spr9 {
    unsigned char pad[9];
    unsigned char a : 2;
    unsigned char b : 2;
};

struct Actor {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0xb];
    unsigned char f23;
    unsigned char pad24[0x2c];
    struct Spr *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0xe];
    unsigned short f64;
    unsigned char pad66[2];
    struct Actor *f68;
    void *f6c;
};

/* BLKmode (3 and 6 bytes) on purpose -- see the header. */
struct ZB { unsigned char b; unsigned char c[2]; };
struct ZH { unsigned short h; unsigned char c[4]; };

extern struct SpriteSlot gSpriteSlots[];
extern unsigned char *iwram_3001f30;

extern struct Actor *__CreateActor(int kind, int x, int y, int z);
extern void __Sprite_SetAnim(struct Spr *s, int n);
extern void __Func_8003f3c(int n);
extern void __PlaySound(int id);
extern void OvlFunc_884_200a3ec(void);
extern void OvlFunc_884_200a39c(void);

void OvlFunc_884_200a440(struct Actor *c)
{
    struct Actor *arr[2];
    struct Actor *a;
    struct Spr *s;
    unsigned char *w;
    unsigned char *p;
    int i;

    w = iwram_3001f30;
    __PlaySound(0x83);
    for (i = 0; i <= 1; i++) {
        a = __CreateActor(0x1a, c->f8, c->fc, c->f10);
        arr[i] = a;
        if (a != 0) {
            a->f14 = c->f14;
            s = a->f50;
            p = &a->f55;
            ((struct ZB *)p)->b = 0;
            p += 0xf;
            ((struct ZH *)p)->h = 0;
            a->f68 = c;
            if (s != 0) {
                __Sprite_SetAnim(s, 0);
                s->f26 = 0;
                __Func_8003f3c(s->f1c);
                s->f1c = *(unsigned short *)(w + 0x46);
                s->f1d |= 1;
                s->f8_a = gSpriteSlots[s->f1c].vramOffset >> 5;
                s->f5_b = 0;
                s->f5_c = 1;
                s->f7_b = 2;
                *(s->f28 + 0x16) = 0;
            }
        }
    }
    arr[0]->f6c = OvlFunc_884_200a3ec;
    ((struct Spr9 *)arr[0]->f50)->b = 2;
    ((struct Spr9 *)arr[1]->f50)->b = 2;
    arr[1]->f6c = OvlFunc_884_200a39c;
    arr[1]->f23 = 2;
}

/* OvlFunc_884_200a440 (0x0200a440) -- NON-MATCHING, 12 encodings of 136 differ,
 * SAME LENGTH (ref 136 encodings / 292 bytes; ours identical, all relocations
 * identical).  PRODUCTION-FLAG FIGURE, re-measured in batch 316 with
 * tools/objcmp.py itself.  NOT POOL-INFLATED -- all 12 are real instructions.
 *
 * asm/overlays/rom_784360/ovl_30_c_c_c_a_a_c_c_c.s (1 function).
 * tools/datacheck.py is silent: no data sections, whole-file conversion, no
 * exports beyond the function.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_784360/200a440.c \
 *     asm/overlays/rom_784360/ovl_30_c_c_c_a_a_c_c_c.s --func OvlFunc_884_200a440
 *
 * ***********************************************************************
 * *** BATCH 316: READ src/non_matching/rom_8a000/8096ddc.c FIRST.  IT NOW
 * *** CARRIES THE WHOLE MECHANISM, THE BROKEN CLOSURE AND THE NEXT STEP.
 * ***********************************************************************
 * Two things changed for this park and both are corrections:
 *
 * 1. *** "FIX ONE AND THE OTHER FOLLOWS" IS FALSE.  STRIKE IT. ***  This header
 *    and 8096ddc's both assert the two are the same function with the same gate
 *    and that fixing either fixes both.  Under -fno-expensive-optimizations:
 *        8096ddc   95 of 146, FIRST DIFF AT INDEX 120 -- residue entirely closed
 *        200a440  125 of 136, FIRST DIFF AT INDEX 7   -- global perturbation
 *    Same flag, opposite behaviour.  The extra `__PlaySound` and the `w` base
 *    pointer are enough to separate them -- as this header already half-knew,
 *    recording that the r3 pin which isolates the residue on the twin costs four
 *    instructions here.  *** THEY SHARE A MECHANISM, NOT A FIX. ***  So a landing
 *    on 8096ddc must be RE-MEASURED here, not assumed.
 *
 * 2. The residue is confirmed index-for-index identical IN SHAPE to the twin's:
 *        200a440  idx 29-38 the chain block;  idx 75,76  ref 4249 negs / 812b
 *                 strh SWAPPED
 *        8096ddc  idx 31-39 the chain block;  idx 81,82  the SAME two encodings
 *    and the tail tie is the SAME TIE.  Batch 315 called that tie a CLASS-rung
 *    decision; batch 316 shows it is an INSN_LUID decision, because
 *    rank_for_schedule's class test short-circuits on `insn_cost (...) == 1` and
 *    puts a data-dependent insn in class 3 anyway.  The conclusion that the
 *    alias-set lever cannot reach it survives for a BETTER reason: the
 *    dependence that would have to be removed is a REGISTER ANTI-DEPENDENCE ON
 *    r3, not a memory one, and both MEMs are already in the same alias set at
 *    disjoint offsets.  *** That makes the tail 2 encodings DOWNSTREAM OF THE
 *    SAME r2/r3 BLOCKER as the other 10 -- ONE FACT WITH TWO SYMPTOMS, not two
 *    independent parts. ***  Full derivation in 8096ddc.c.
 *
 * ===== LEVER 5 (callee return type): SWEPT HERE FOR THE FIRST TIME, EXHAUSTED
 * All 5 `extern void` callees flipped to `extern int` -- __Sprite_SetAnim,
 * __Func_8003f3c, __PlaySound, OvlFunc_884_200a3ec, OvlFunc_884_200a39c -- ALL
 * INERT at 12, dsize 0, no relocation change.
 *
 * ===== WHAT STILL STANDS FROM BATCH 295 =====
 * From .17.lreg / .18.greg, exactly parallel to Func_8096ddc:
 *   reg 37  p (walk ptr)  8 refs / 7 insns in block 2, set 2 times, DIES IN 2
 *                         PLACES, pref STACK_REG   -> GLOBAL allocno, gets r2
 *   reg 50  dead QI 0     2 refs / 2 insns, REG_UNUSED, pref LO_REGS -> LOCAL, r3
 *   reg 52  SImode 0      6 refs / 10 insns, pref LO_REGS            -> LOCAL, r3
 * THE STACK_REG GATE and its four compiler-source citations (arm.h:989,
 * arm.md:496, arm.h:1095, regclass.c:1459-1462) were all re-checked in batch 316
 * and all four are right.  The gate is a reason the walk pointer is GLOBAL.  It
 * is NOT a reason the residue is unreachable -- that was the conflation, and
 * 8096ddc.c sets out why.
 * The park's original diagnosis, "the dead QImode zero steals an address
 * register", is the right target: the flag's actual route on the twin is that
 * the dead QImode zero CEASES TO EXIST AS A PSEUDO, while the walk pointer's
 * `pref STACK_REG` does not move at all.
 *
 * ===== NEXT STEP (shared with the whole family) =====
 * Find a source spelling of the f55 byte store whose zero does NOT create a
 * second, DEAD QImode pseudo, while the SImode zero stays SHARED between the
 * strb and the strh.  The recorded bitfield sweep varied the CARRIER MEMBER'S
 * TYPE, never whether a separate dead zero pseudo is created.
 *
 * ===== FAMILY =====
 * A near-twin of OvlFunc_887_200968c (identical but for the leading
 * __PlaySound), of OvlFunc_897_200aeb0, and of the parked OvlFunc_883_200dd68 /
 * OvlFunc_882_200c41c.  tools/dupfuncs.py (re-run batch 316) adds two facts the
 * family notes did not have: OvlFunc_883_200dd68 is a BYTE DUPLICATE of
 * OvlFunc_881_200c058 (asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_c.s), and
 * 200aeb0's overlay-mate OvlFunc_897_200b01c is a byte duplicate of
 * OvlFunc_896_200c49c -- neither partner is parked with the family.
 *
 * ===== MEASURED NEGATIVES, ALL AT THE BASELINE OF 12 =====
 * The two-address spellings (`p2 = a + 0x64` independently, `p2 = p + 0xf`,
 * `((struct ZH *)(p + 0xf))`) all move the ZERO to r2 and give the f64 address r3
 * correctly, leaving only the f55 address in r1 -- the cleanest statement of the
 * blocker.  Inert: every bitfield spelling of the ZB/ZH carrier members and all
 * four combinations; BLK-u8 + PLAIN-u16 (the plain `*(unsigned short *)p = 0`
 * still shares the SImode zero).  The r3 pin on the walk pointer costs 4
 * instructions here (140 encodings, 300 bytes, 131 differ) at all three
 * declaration scopes, so it is not even usable as a diagnostic on this function.
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

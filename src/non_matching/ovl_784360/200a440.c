/* OvlFunc_884_200a440 (0x0200a440) -- NON-MATCHING, 12 encodings of 136 differ, SAME LENGTH
 * (ref 136 encodings / 292 bytes; ours identical length, all relocations identical).
 *
 * asm/overlays/rom_784360/ovl_30_c_c_c_a_a_c_c_c.s (1 function).  tools/datacheck.py is
 * silent on it: no data sections, whole-file conversion, no exports beyond the function.
 * (The batch-295 brief called this OvlFunc_876_200a440; the .s and this header are right,
 * it is 884.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py src/non_matching/ovl_784360/200a440.c asm/overlays/rom_784360/ovl_30_c_c_c_a_a_c_c_c.s --func OvlFunc_884_200a440
 *
 * FAMILY: a near-twin of OvlFunc_887_200968c (identical but for the leading __PlaySound)
 * and of the parked OvlFunc_883_200dd68 / OvlFunc_882_200c41c
 * (src/non_matching/ovl_780898/200dd68.c, ovl_77dd1c/200c41c.c).  Batch 295 established
 * that it is ALSO the exact twin of src/non_matching/rom_8a000/8096ddc.c
 * (Func_8096ddc, 11 of 146): same two residues, same allocnos, same gate.  Fix one and
 * the other follows.
 *
 * ===== BATCH 295: THE BLOCKER IS FULLY LOCATED AND IT IS CLOSED =====
 *
 * From .17.lreg / .18.greg, exactly parallel to Func_8096ddc:
 *   reg 37  p (walk ptr)  8 refs / 7 insns in block 2, set 2 times, DIES IN 2 PLACES,
 *                         pref STACK_REG   -> GLOBAL allocno, gets r2
 *   reg 50  dead QI 0     2 refs / 2 insns, REG_UNUSED, pref LO_REGS -> LOCAL, r3
 *   reg 52  SImode 0      6 refs / 10 insns, pref LO_REGS            -> LOCAL, r3
 *
 * The park's own diagnosis ("the dead QImode zero steals an address register", "the add
 * needs its tied alternative ... CLASS_LIKELY_SPILLED, local-alloc.c:362") was RIGHT.
 * What batch 295 adds is WHY the walk pointer prefers STACK_REG, and that it cannot be
 * avoided at this offset:
 *
 * THE STACK_REG GATE.  An address pseudo set by `(set (reg) (plus (reg) (const_int N)))`
 * and used as a memory base gets `pref STACK_REG` when N is NOT a valid `add rd, sp, #imm`
 * operand and `pref BASE_REGS` when it is.  Measured on a four-line isolate: +0x55
 * STACK_REG, +0x54 BASE_REGS, +0x64 BASE_REGS, +0x65 STACK_REG -- the CONSTANT, not the
 * store's mode.  reg_class_size[STACK_REG] == 1 so the default CLASS_LIKELY_SPILLED_P is
 * TRUE, and local-alloc.c:362-368 refuses the pseudo even when it dies exactly once.
 * The ROM's a->f55 sits at 0x55, so the walk pointer is a global allocno by construction.
 *
 * WHY THE r2/r3 SWAP IS UNREACHABLE FROM SOURCE.  For a global p to get r3, no LOCAL
 * quantity may hold r3 anywhere in p's live range.  The shared SImode zero is born INSIDE
 * that range and is local, so it takes r3 (first in REG_ALLOC_ORDER 3,2,1,0,...) unless
 * another local holds r3 there -- and any local that does also conflicts with p.  So the
 * two escapes are (a) make p local, closed by the gate above, or (b) make the SImode zero
 * non-local.  (b) requires the zero to span two blocks or die twice, and every source
 * route to that also stops it being SHARED between the strb and the strh, which is what
 * produces the ROM's single `mov r2, #0`.  Measured on the twin: a plain `int z = 0`
 * gives the strh its own pooled HImode zero (22 differ; loop.c hoists it if declared in
 * the loop, 148 encodings), and a BLKmode zero store after the `s == 0` branch is NOT
 * cse'd together with the walk's zero.
 *
 * So the park's "NEXT" is answered, both halves negative: there is no spelling whose f55
 * store lacks the dead QI while still sharing the SI zero, and there is no spelling that
 * makes the walk pointer LOCAL at offset 0x55.
 *
 * WHAT IS LEFT, ISOLATED.  On the twin, pinning the walk pointer with
 * `register unsigned char *q __asm__("r3")` reads 11 -> 6 and shows the whole remainder
 * is two sched2 ties: the `ldr r5,[r0,#0x50]` slot inside the store chain (4 encodings)
 * and `mov r1,#0x21 / neg r1,r1` belonging before `strh r3,[r5,#8]` (2 encodings).  HERE
 * the same pin is NOT free -- it costs 4 instructions (140 encodings, 300 bytes, 131
 * differ) at all three declaration scopes tried, because this function's extra
 * __PlaySound call and the `w` base pointer contend for r3.  So on this function the pin
 * is not even usable as a diagnostic; use the twin for that.
 *
 * Of this function's 12, ten are the chain block and two are the `neg r1,r1` tie -- the
 * same 5 + 6 split as the twin plus one extra encoding from the different tail.
 *
 * DELTA to the inert list: the two-address spellings (`p2 = a + 0x64` independently, or
 * `p2 = p + 0xf`, or `((struct ZH *)(p + 0xf))`) all move the ZERO to r2 and give the f64
 * address r3 correctly, and leave only the f55 address in r1 -- that is the cleanest
 * statement of the blocker and is worth keeping over the bare "16" the park records.
 * Also inert: every bitfield spelling of the ZB/ZH carrier members (`unsigned int v : 8`,
 * `unsigned char v : 8`, `unsigned int v : 16`, `unsigned short v : 16`, and all four
 * combinations), and BLK-u8 + PLAIN-u16 -- confirming the park's "12, same" and adding
 * that the plain `*(unsigned short *)p = 0` still shares the SImode zero.
 * NO FLAG ROUTE: -fno-expensive-optimizations is the only production-flag candidate that
 * touches regclass's altclass computation (regclass.c:1161) and on the twin it is much
 * worse (--align 53 -> 94); -fno-schedule-insns2 53 -> 97.
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

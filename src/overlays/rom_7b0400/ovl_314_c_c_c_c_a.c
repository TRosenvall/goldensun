/* OvlFunc_925_200b460 and OvlFunc_925_200b4bc -- 0x0200b460 and 0x0200b4bc, both
 * functions of what was asm/overlays/rom_7b0400/ovl_314_c_c_c_c.s.
 *
 * TEXT/DATA SPLIT, no new exports.  The file's `.data` was six `.incbin` runs of
 * overlays/rom_7b0400/orig.bin at the tail; it now lives alone in
 * asm/overlays/rom_7b0400/ovl_314_c_c_c_c_b.s and overlay.ld's `.data` line points
 * there while its `.text` line points at this file's object.  Checked in both
 * directions before cutting: the only text->data reference is
 * gScript_925__0200bc54, already .global; the data is pure .incbin with no reference
 * back into the text; and the other five exported labels are read from five other
 * .s files in the overlay, all already .global.  The split was gated on its own with
 * no .c written.
 *
 * 456 bytes, 202 encodings and 21 relocations identical.  objcmp notes
 * _divsi3_RAM / __divsi3 as one symbol at the same linked address.
 *
 * A COMMUTATIVE `mul`'s SOURCE OPERAND ORDER PICKS WHICH OPERAND BECOMES THE TIED
 * DESTINATION, and that closed OvlFunc_925_200b460 by itself.  `*thumb_mulsi3` ties
 * operand 0 to operand 1 with `%0`, and canonicalisation does NOT erase the order:
 * `(a->f30 + 0x1c) * __cos(ang)` is 2 differing, `__cos(ang) * (a->f30 + 0x1c)` is
 * exact.  This is the counter-case to "all four pointer-arithmetic operand orders are
 * inert" -- for `mult` the order is a real lever, and the ROM's `mul rD,rS`
 * destination is the readout.
 *
 * ARGUMENT PRECOMPUTE'S ORDER IS THE LEVER, NOT ITS PRESENCE.  Three block-scoped
 * locals immediately before __CreateActor, and only one of the four orders works:
 * x,y,z exact; y,x,z 7; y,z,x 45; z,y,x 45; left as call-site expressions 46.  The
 * tell is the ROM's SECOND low-register copy of a high-register pointer
 * (`mov r0,r10` beside the surviving `mov r1,r10`) -- the load that comes first
 * forces reload to allocate a fresh register because the base is still live.
 *
 * A 2-BIT `unsigned char` BITFIELD COPY IS THE ONLY ROUTE TO AN SImode `~0xc`.  The
 * ROM's `mov r3,#13 / neg r3,r3 / and r3,r1 / orr r3,r2 / strb r3,[r5,#9]` is not
 * reachable from any masked-merge spelling: combine's simplify_and_const_int masks
 * -13 by the zero-extended byte's nonzero_bits and yields `mov r3,#0xf3`.  Seven
 * spellings measured (~0xc, 0xfffffff3, -13, an int carrier, an unsigned carrier, a
 * <<16>>16 round-trip, an outer & 0xff, an outer (unsigned char) cast) -- all 48.
 * The bitfield reproduces it register for register, because store_fixed_bit_field
 * builds its mask in the container mode and never reaches force_to_mode.
 *
 * Other load-bearing choices: `unsigned short ang` widened to `int` (7 of 43
 * otherwise); the named `t = a->f68` local (41 of 43, and 84 bytes vs 92, without);
 * `a->f64 -= 0x200` rather than `+= 0xfe00` (11 of 43 -- the pooled
 * `ldr r1,=0xfffffe00 / add` survives only for the subtract spelling); an `int t`
 * carrier for `na->f64 = 0xffff000 & __Random()` (55); and
 * `v = v / K; v <<= 16;` as two statements (66).
 *
 * SHIM, BOOKED for OvlFunc_925_200b4bc: the PIN1/PIN2/PIN3 macros, used in ONE block
 * for __MapActor_SetPos(8, 0x80<<12, 0x80<<12) -- 83 differing without it.
 *
 * Inert, and therefore not written: the `m` local for `iwram_3001e40 & 0xf`; the `sb`
 * local; `__Random() & 0xffff000` vs `0xffff000 & __Random()`.
 */
#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

struct Bits {
    unsigned char pad00[9];
    unsigned char lo : 2;
    unsigned char mid : 2;
    unsigned char hi : 4;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[0x30 - 0x20];
    int f30;
    unsigned char pad34[4];
    int f38;
    unsigned char pad3c[4];
    int f40;
    unsigned char pad44[0x50 - 0x44];
    unsigned char *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    unsigned short f64;
    unsigned short f66;
    struct Actor *f68;
    void (*f6c)(struct Actor *);
};

extern char *iwram_3001e70;
extern unsigned int iwram_3001e40;
extern unsigned char gScript_925__0200bc54[];

extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern unsigned int __Random(void);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern void __Func_80929d8(struct Actor *a, int n);
extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_925_200b460(struct Actor *a)
{
    unsigned short ang;
    struct Actor *t;

    ang = a->f64;
    t = a->f68;
    a->f8 = t->f8 + __cos(ang) * (a->f30 + 0x1c);
    a->f10 = (__sin(ang) << 4) + (0x90 << 16);
    a->f38 = a->f8;
    a->f40 = a->f10;
    a->f64 -= 0x200;
}

void OvlFunc_925_200b4bc(void)
{
    struct Actor *act;
    struct Actor *na;
    unsigned char *s;
    short *q;
    int v;
    int t;

    act = __MapActor_GetActor(8);
    q = (short *)(iwram_3001e70 + 0xe8);
    v = ((__Random() * 48) >> 16) << 16;
    if (q[1] <= 0x81) {
        if ((iwram_3001e40 & 1) != 0) {
            __MapActor_SetPos(8, 0x98 << 17, 0x90 << 16);
            __MapActor_GetActor(8)->f18 = 0x80 << 9;
            __MapActor_GetActor(8)->f1c = 0x80 << 9;
        } else {
            __MapActor_SetPos(8, 0x98 << 17, 0x97 << 16);
            __MapActor_GetActor(8)->f18 = 0x14ccc;
            __MapActor_GetActor(8)->f1c = 0x14ccc;
        }
    } else {
        { PIN3; q1 = 0x80; q2 = 0x80; q0 = 8; q1 <<= 12; q2 <<= 12;
          __MapActor_SetPos(q0, q1, q2); }
    }
    if (act != 0) {
        if ((iwram_3001e40 & 0xf) == 0) {
            { int x = act->f8 + (0x80 << 12);
              int y = act->fc + v + (0x80 << 12);
              int z = act->f10;
              na = __CreateActor(0x8e << 1, x, y, z); }
            v = v / (0xc0 << 11);
            v <<= 16;
            if (na != 0) {
                s = na->f50;
                __Actor_SetScript(na, gScript_925__0200bc54);
                __Func_80929d8(na, 3);
                na->f55 = 0;
                t = 0xffff000 & __Random();
                na->f64 = t;
                na->f66 = 0;
                na->f68 = act;
                na->f6c = OvlFunc_925_200b460;
                na->f30 = (__sin((v & 0xfffff) >> 4) * 24) >> 16;
                s[0x26] = 0;
                ((struct Bits *)s)->mid = ((struct Bits *)act->f50)->mid;
            }
        }
    }
}

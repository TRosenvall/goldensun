/* Func_8096ddc -- NON-MATCHING, 11 encodings of 146 (objcmp: ENCODINGS differ in 11
 * place(s), ref 146 / ours 146).  LENGTH EXACT.  The .s (asm/rom_8a000/rom_96cdc_a_a_c_c.s)
 * holds only this function -- whole-file, no split.  Two mid-function pools (a HImode 0 and
 * a HImode 0xfffffc00) both reproduce.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_8a000/8096ddc.c asm/rom_8a000/rom_96cdc_a_a_c_c.s --func Func_8096ddc
 *
 * Levers that got it here (fresh, batch 287):
 *  - struct Sprite bitfields from rom_8d9a4_c_a_c_c_c_c_a_c.c (c5/c6 at +5, f08:10 / b10:2 at
 *    +8) plus a d6:2 at +7: the `sub #0x21`/`and 0x3f`/`orr 0x40` byte-5 sequence, the pooled
 *    0xfffffc00 tile insert and both +9 priority RMWs come out exactly.
 *  - `unsigned short zero` stored into two byte fields gives the pooled `ldr r1,=0 / mov r8`.
 *  - THE ROM's `mov r3,r0 / add r3,#0x55 / strb r2,[r3] / add r3,#0xf / strh r2,[r3]` CHAIN
 *    needs BOTH (a) a pointer local stepped `q += 0xf` and (b) the stores done as members of a
 *    BLKmode struct (struct B1/H1, 12 bytes): a plain member store to a BLKmode struct goes
 *    through store_bit_field (direct_store[QI/HI] is false on thumb), whose `x & 0` folds to
 *    ONE SImode zero shared by the strb and strh -- which is the ROM's r2.  Raw
 *    `*(u8 *)q = 0; *(u16 *)q = 0` gives a QI and a HI pseudo instead, and the HI one pools.
 *    Written as `o->f55 = 0; o->f64 = 0` (no q) the two address pseudos land in r1/r3 and
 *    reload's move2add never sees the chain (20 of 146).
 *  - Statement order inside the `o != 0` block: 1260 permutations screened, best 11.
 *
 * RESIDUE (11 encodings, three pieces):
 *  1. q/zero swap: ROM q=r3 zero=r2, ours q=r2 zero=r3.  q is set twice so it dies twice and
 *     local-alloc refuses it (REG_N_DEATHS != 1); the SImode zero is local and takes r3
 *     first; global then gives q r2.  Splitting q into two locals makes both local but loses
 *     the chain (17).  Re-deriving q from o in two steps, declaring q elsewhere, typing it
 *     struct B1 *: all 11.
 *  2. sched2 tie: ROM emits `mov r1,#0x21 / neg r1,r1` BEFORE `strh r3,[r5,#8]` (tile store),
 *     ours after.
 *  3. tryc also shows the second pool as `b L3 / L3: / L1:` against the ROM's `b L1`; that is
 *     only a label-naming artifact (same bytes), objcmp does not count it.
 */
struct Sprite {
    unsigned char pad00[5];
    unsigned char c0 : 5;
    unsigned char c5 : 1;
    unsigned char c6 : 2;
    unsigned char pad06[1];
    unsigned char d0 : 6;
    unsigned char d6 : 2;
    unsigned short f08 : 10;
    unsigned short b10 : 2;
    unsigned short b12 : 4;
    unsigned char pad0a[0x1c - 0x0a];
    unsigned char f1c;
    unsigned char f1d;
    unsigned char pad1e[0x26 - 0x1e];
    unsigned char f26;
    unsigned char pad27;
    unsigned char *f28;
};

struct Actor {
    void *f00;
    unsigned char pad04[2];
    unsigned short f06;
    int f08;
    int f0c;
    int f10;
    int f14;
    int f18;
    int f1c;
    unsigned char pad20[3];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Sprite *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    unsigned short f64;
    unsigned char pad66[2];
    struct Actor *f68;
    void *f6c;
};

struct B1 { unsigned char v; unsigned char pad[11]; };
struct H1 { unsigned short v; unsigned char pad[10]; };
struct SpriteSlot {
    unsigned short f0;
    unsigned short f2;
};

extern unsigned char *iwram_3001f30;
extern struct SpriteSlot gSpriteSlots[];
extern struct Actor *_CreateActor(int id, int x, int y, int z);
extern void _Sprite_SetAnim(struct Sprite *s, int n);
extern void Func_8003f3c(int n);
extern void Func_8096d84(void);
extern void Func_8096d2c(void);

void Func_8096ddc(struct Actor *e)
{
    struct Actor *arr[2];
    struct Actor *c;
    struct Actor *o;
    struct Sprite *s;
    unsigned char *m;
    unsigned short zero;
    int i;
    unsigned char *q;

    m = iwram_3001f30;
    c = *(struct Actor **)(m + 0x10);
    for (i = 0; i <= 1; i++) {
        o = _CreateActor(0x1a, e->f08, e->f0c, e->f10);
        arr[i] = o;
        if (o == 0)
            continue;
        o->f14 = e->f14;
        s = o->f50;
        q = (unsigned char *)o + 0x55;
        ((struct B1 *)q)->v = 0;
        q += 0xf;
        ((struct H1 *)q)->v = 0;
        o->f68 = e;
        o->f1c = 0x1999;
        zero = 0;
        o->f18 = 0x1999;
        if (s == 0)
            continue;
        _Sprite_SetAnim(s, 0);
        s->f26 = zero;
        Func_8003f3c(s->f1c);
        s->f1c = *(unsigned short *)(m + 0x46);
        s->f1d |= 1;
        s->f08 = gSpriteSlots[s->f1c].f2 >> 5;
        s->c5 = 0;
        s->c6 = 1;
        s->d6 = 2;
        s->f28[0x16] = zero;
    }
    arr[0]->f6c = Func_8096d84;
    arr[0]->f50->b10 = 0;
    arr[1]->f6c = Func_8096d2c;
    arr[1]->f50->b10 = c->f50->b10;
    arr[1]->f23 = 2;
}

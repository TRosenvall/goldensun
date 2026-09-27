/* OvlFunc_897_200aeb0 (0x0200aeb0) -- NON-MATCHING, 12 encodings of 148 differ, SAME LENGTH
 * (ref 148 encodings; all relocations identical).
 *
 * asm/overlays/rom_791794/ovl_30_c_c_c_a_a_c_c_c.s (1 function, no data -- no split needed).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_791794/200aeb0.c asm/overlays/rom_791794/ovl_30_c_c_c_a_a_c_c_c.s --func OvlFunc_897_200aeb0
 *
 * FAMILY: the parked OvlFunc_887_200968c (src/non_matching/ovl_787e04/200968c.c) with a
 * leading __PlaySound(0x124) and a different tail -- f50->f9 bits 2-3 are COPIED from
 * __MapActor_GetActor(0xf)->f50 onto both new actors (fetched twice), not set to 2.
 * The loop is instruction-for-instruction the ROM's 887 loop, so it carries the
 * same 12-differing residue and nothing else: the "CHAIN"
 *     mov r3,r0 / add r3,#0x55 / strb r2,[r3] / add r3,#0xf / strh r2,[r3]
 * comes out with the walk pointer and the SI zero swapped (ours p=r2, zero=r3), plus
 * the one-slot schedule shifts that follow. See 200968c.c's header for the mechanism
 * (a dead REG_UNUSED QI zero left by store_fixed_bit_field takes r3 in local-alloc;
 * the global walk pointer then gets r2) -- everything there applies verbatim.
 *
 * The whole prologue, the PlaySound placement before `sub sp`, the whole tail with its
 * two GetActor(0xf) fetches, and the r8 pooled zero / r9 0x3f / r10 sp / r11 w roles
 * all match first time from the 887 park's body.
 *
 * INERT / WORSE here: splitting the walk into two pointers so the second is local
 * (p = &a->f55; q = p + 0xf) -- 324 bytes with the BLK stores, the plain u8/u16 stores,
 * or BLK u8 + plain u16; the same with an int zero local shared by both stores --
 * 332 bytes (loop.c hoists it, as the 887 park found).
 *
 * NEXT: whatever closes 200968c closes this (and 200a440, 200dd68, 200c41c).
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
extern void OvlFunc_897_200ae5c(void);
extern void OvlFunc_897_200ae0c(void);
extern struct Actor *__MapActor_GetActor(int id);

void OvlFunc_897_200aeb0(struct Actor *c)
{
    struct Actor *arr[2];
    struct Actor *a;
    struct Spr *s;
    unsigned char *w;
    unsigned char *p;
    int i;

    w = iwram_3001f30;
    __PlaySound(0x124);
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
    arr[0]->f6c = OvlFunc_897_200ae5c;
    ((struct Spr9 *)arr[0]->f50)->b = ((struct Spr9 *)__MapActor_GetActor(0xf)->f50)->b;
    ((struct Spr9 *)arr[1]->f50)->b = ((struct Spr9 *)__MapActor_GetActor(0xf)->f50)->b;
    arr[1]->f6c = OvlFunc_897_200ae0c;
    arr[1]->f23 = 2;
}

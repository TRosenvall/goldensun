/* OvlFunc_882_200c41c (0x0200c41c) -- NON-MATCHING (ref 144 encodings / 308 bytes; ours 142 /
 * 304). Blocker class: the 200dd68 address chain, PLUS a cached pointer field.
 *
 * asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_c_c_c_c_c.s (1 function, so landing needs NO split).
 *
 * THIS IS A NEAR-TWIN OF OvlFunc_883_200dd68 (src/non_matching/ovl_780898/200dd68.c) and
 * inherited everything from it -- they differ only by a leading `__PlaySound(0x98)` and a
 * `c->f50->f8_b` source operand in the tail. Read that park first; its priced two-route
 * exclusion is residue 1 here too (+1).
 *
 * RESIDUE 2 IS A MISSING RELOAD (-1): the ROM reads `c->f50` TWICE and gcc caches it.
 *
 * ===== -fno-strict-aliasing IS THE "ROM RELOADS A POINTER FIELD" LEVER =====
 *
 * Probed directly against gcc-2.96 rather than inferred: with strict aliasing on, gcc ALWAYS
 * caches `x->ptrfield`, including across a store to a `char` field, an `int` field, a pointer
 * field of the pointee, and a pointer store to the same parent struct. With
 * -fno-strict-aliasing it RELOADS, exactly as the ROM does.
 *
 * NO SOURCE-LEVEL TYPE CHANGE REACHES THIS -- `unsigned char`, `unsigned short` and
 * `unsigned int` bitfield containers were all tried. So when the ROM re-reads a pointer field
 * with nothing obviously invalidating it, the question is the FLAG, not the typing. That is a
 * real distinction from the recorded "do not cache a repeated read" rule, which is about
 * SCALAR reads a source can simply re-write.
 *
 * ALIAS_CFLAGS already exists in the Makefile (line 235) for exactly this flag, so this would be
 * an existing group rather than a new one. Not added here because the function is not otherwise
 * exact -- residue 1 stands.
 *
 * NEXT: residue 1, jointly with 200dd68. Then the flag.
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

extern struct SpriteSlot gSpriteSlots[];
extern unsigned char *iwram_3001f30;

extern struct Actor *__CreateActor(int kind, int x, int y, int z);
extern void __Sprite_SetAnim(struct Spr *s, int n);
extern void __Func_8003f3c(int n);
extern void OvlFunc_882_200c3c8(void);
extern void OvlFunc_882_200c378(void);
extern void __PlaySound(int id);

void OvlFunc_882_200c41c(struct Actor *c)
{
    struct Actor *arr[2];
    struct Actor *a;
    struct Spr *s;
    unsigned char *w;
    int i;

    w = iwram_3001f30;
    __PlaySound(0x98);
    for (i = 0; i <= 1; i++) {
        a = __CreateActor(0x1a, c->f8, c->fc, c->f10);
        arr[i] = a;
        if (a != 0) {
            a->f14 = c->f14;
            s = a->f50;
            a->f55 = 0;
            a->f64 = 0;
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
    arr[0]->f6c = OvlFunc_882_200c3c8;
    arr[0]->f50->f8_b = c->f50->f8_b;
    arr[1]->f50->f8_b = c->f50->f8_b;
    arr[1]->f6c = OvlFunc_882_200c378;
    arr[1]->f23 = 2;
}

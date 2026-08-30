/* OvlFunc_881_200c058  [overlays/rom_77a7c8]
 *
 * Source asm: goldensun/asm/overlays/rom_77a7c8/ovl_30_c_c_c_c_c_c.s
 *
 * BLOCKER: an address-walk chain and tail register roles. 27 of 129, on the
 * FIRST screen; six later variants were all worse, which is itself the useful
 * result -- the obvious spellings have been spent.
 *
 * WHAT CAME OUT FREE, so nobody re-derives it: the two-iteration loop, the
 * stack `struct Actor *arr[2]` addressed as `arr[i]`, all seven bitfield
 * writes including the ten-bit field at +8 fed by a table lookup shifted right
 * by five, and THE ROM'S POOLED ZERO -- gcc emits `ldr r1, =0x0` into an inline
 * pool unaided, so this needs no const.sym entry. It also emits `mov r4, #4`
 * without pushing r4, matching the ROM.
 *
 * WHAT IS LEFT:
 *   1. The `add r3, #0x55 / ... / add r3, #0xf` ADDRESS WALK (~8). gcc builds
 *      two independent address registers and is one instruction longer. Per the
 *      batch-56 test that means the chain WAS in the source -- but every
 *      explicit walking-pointer spelling costs a local, and the extra local
 *      re-ranks the whole allocation: as `unsigned char *` from a cast 46, from
 *      `&a->f55` 46, with the tail loads reordered 52 aligned. The chain is
 *      right and the local that expresses it is what breaks it; this probably
 *      wants a spelling that REUSES an existing pointer rather than adding one.
 *   2. Tail register roles (~10). Reordering the source to match is WORSE, not
 *      better: arr[1] only 29, arr[0] only 38, both 33.
 *   3. One `strh r3, [r5, #8]` a position early.
 *
 * CAVEAT: tryc printed its mid-function-pool warning -- this reference keeps
 * two literal pools inside the body -- so even at OK this one would need
 * `make compare` as the authority, not the screen.
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

struct Cfg {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    int f14;
};

struct Actor {
    unsigned char pad0[0x14];
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
    struct Cfg *f68;
    void *f6c;
};

extern struct SpriteSlot gSpriteSlots[];
extern unsigned char *iwram_3001f30;

extern struct Actor *__CreateActor(int kind, int x, int y, int z);
extern void __Sprite_SetAnim(struct Spr *s, int n);
extern void __Func_8003f3c(int n);
extern void OvlFunc_881_200c004(void);
extern void OvlFunc_881_200bfb4(void);

void OvlFunc_881_200c058(struct Cfg *c)
{
    struct Actor *arr[2];
    struct Actor *a;
    struct Spr *s;
    unsigned char *w;
    int i;

    w = iwram_3001f30;
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
    a = arr[0];
    a->f6c = OvlFunc_881_200c004;
    a->f50->f8_b = 1;
    a = arr[1];
    a->f6c = OvlFunc_881_200bfb4;
    a->f50->f8_b = 1;
    a->f23 = 2;
}

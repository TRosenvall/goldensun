/* OvlFunc_883_200db48  --  0x0200db48, cut from
 * goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_c_c_a_c_c_c.s.
 *
 * Dresses an actor as a portrait: clear two sprite-attribute fields, set the
 * animation mode, hide it, load the item icon into a scratch IWRAM block and
 * upload it, then hand the actor its per-frame callback. One of SEVEN members
 * found by tools/find_shape.py, and they differ in NOTHING but the callback
 * name -- every id, mask and offset is identical across all seven.
 *
 * THE THREE MASKS ARE ALL BITFIELDS, AND THE ORDER OF THE ASSIGNMENTS IS THE
 * ROM'S. The ROM interleaves them across two bytes with one load and one store
 * each:
 *
 *     mov r2, #0xd / neg r2, r2 / ldrb r3, [r6, #9] / and r2, r3    ~0xc
 *     mov r3, #4 / ldrb r1, [r6, #5] / orr r2, r3
 *     mov r3, #0x21 / neg r3, r3 / and r3, r1 / strb r3, [r6, #5]   ~0x20
 *     mov r3, #0xf / and r2, r3 / strb r2, [r6, #9]
 *
 * Every mask is built with `mov / neg`, which is batch 71's 32-bit width rule
 * saying bitfield rather than hand-written masking -- including the `& 0xf`,
 * which is a four-bit field cleared to zero. Written as ordinary masking with
 * `int` locals to keep the constants wide, it is 77 of 98; as three bitfield
 * assignments in the order f9_mid, f5_b5, f9_hi it is exact, and gcc merges the
 * two f9 writes into the single load and store the ROM has.
 *
 * The ORDER of those three matters on its own: f9_mid, f9_hi, f5_b5 -- the same
 * three assignments regrouped -- is 20 of 98.
 *
 * TWO CONSTANTS, TWO ANSWERS. The zero stored to five fields is a named `int z`
 * assigned at the top, so it lives in a pushed register as the ROM has it. The
 * ONE stored to two fields is written as a plain literal in both places: gcc
 * shares it into r9 by itself, and giving it a local puts the `mov #1` on the
 * wrong side of a neighbouring store. Same function, opposite answers.
 *
 * `__UploadSpriteGFX` is deliberately undeclared -- the second declaration
 * lever, putting r0 last in its own argument block.
 */
struct S {
    unsigned char pad00[5];
    unsigned char f5_lo : 5;
    unsigned char f5_b5 : 1;
    unsigned char f5_hi : 2;
    unsigned char pad06[3];
    unsigned char f9_lo : 2;
    unsigned char f9_mid : 2;
    unsigned char f9_hi : 4;
    unsigned char pad0a[0x12];
    unsigned char f1c;
    unsigned char pad1d[10];
    unsigned char f27;
};

struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    unsigned char pad10[0x13];
    unsigned char f23;
    unsigned char pad24[0xc];
    int f30;
    unsigned char pad34[4];
    int f38;
    int f3c;
    unsigned char pad40[0x10];
    struct S *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char f56;
    unsigned char pad57[5];
    unsigned char f5c;
    unsigned char pad5d[4];
    unsigned char f61;
    unsigned char pad62[10];
    void *f6c;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct A *a, int f);
extern int __GetFlag(int id);
extern void *__galloc_iwram(int tag, int n);
extern void __gfree(int tag);
extern void __LoadItemIcon(int id);
extern void OvlFunc_883_200dae0(void);

void OvlFunc_883_200db48(int slot)
{
    struct A *a;
    struct S *s;
    void *buf;
    int z;

    a = __MapActor_GetActor(slot);
    s = a->f50;
    s->f9_mid = 1;
    s->f5_b5 = 0;
    s->f9_hi = 0;
    z = 0;
    s->f27 = z;
    __Actor_SetSpriteFlags(a, 0);
    a->f5c = z;
    a->f55 = z;
    if (__GetFlag(0x109) == 0)
        a->fc += 0x80 << 14;
    a->f23 &= 0xfe;
    a->f61 = 1;
    buf = __galloc_iwram(0x11, 0xc1 << 3);
    __LoadItemIcon(0xb5);
    __UploadSpriteGFX(s->f1c, 0x80, (char *)buf + (0x80 << 3));
    __gfree(0x11);
    a->f38 = a->f8;
    a->f30 = z;
    a->f3c = a->fc;
    a->f5c = 1;
    a->f6c = OvlFunc_883_200dae0;
    a->f56 = z;
}

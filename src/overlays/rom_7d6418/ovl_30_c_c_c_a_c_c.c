/* Cluster OvlFunc_951_2008dd0..OvlFunc_951_2008dd0 extracted from goldensun/asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c.s.
 *
 * Total .text for this TU = 116 bytes (= 0x74).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_b.o and asm/overlays/rom_7d6418/ovl_30_c_c_c_b.o in
 * goldensun/overlays/rom_7d6418/overlay.ld.
 *
 * Was parked at 20 of 57, ONE INSTRUCTION SHORT, and resting on an _AREA_00
 * symbol its own park called "almost certainly the WRONG NAME". No pins, no
 * flags, and NO SYMBOL -- the dependency is retired.
 *
 * THE MISSING INSTRUCTION WAS THE `b` OVER A MID-FUNCTION POOL, and the cause is
 * the MODE of the pooled zero, not its contents.
 *
 *   1. GAS assembles a Thumb `ldrh rX, <label>` into the SAME BYTES as
 *      `ldr rX, [pc, #n]` -- `ldrh r1, .L5` is `4900`. So the ROM's `ldr r2, =0`
 *      is indistinguishable from gcc emitting an HImode pool load, and the
 *      disassembly cannot tell you which it was. (docs/elevation.md already
 *      records the folding; what is new is the consequence below.)
 *   2. An HImode pool fixup has the NARROW 32..60 byte range, so arm_reorg
 *      cannot carry it to the end-of-function barrier and dump_table
 *      manufactures a pool in the middle -- with a `b` over it. An SImode fixup
 *      reaches the end and the pool lands after the epilogue with no branch,
 *      which is what the _AREA_00 spelling produced and why it was a line short.
 *
 * So the fix is one plain-C line: the pooled zero is an `unsigned short` local,
 * byte-stored twice. gcc then emits `ldrh r2, .L10` / `b .L11` / `.word 0` at the
 * ROM's position. Verified in the generated .s, not inferred.
 *
 * WORTH KNOWING WHEN A FUNCTION IS EXACTLY ONE INSTRUCTION SHORT and the ROM has
 * a mid-function pool: the missing instruction is probably that `b`, and the
 * question is the MODE of something in the pool.
 *
 * NOTE FOR RE-SCREENING: tools/tryc.py reports this 58 lines against 57 with 19
 * differing. That is the documented label false negative -- gcc emits `.L11:` and
 * `.L9:` where the ROM has one label, and a label emits no bytes. objcmp says 116
 * bytes and 55 encodings identical, and the build is green.
 */
struct Ent {
    unsigned char pad00[5];
    unsigned char f05;
    unsigned char pad06[0x16 - 6];
    unsigned char f16;
};

struct Obj {
    unsigned char pad00[0x26];
    unsigned char f26;
    unsigned char f27;
    struct Ent *f28[1];
};

struct Actor {
    unsigned char pad00[6];
    short f06;
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[0x50 - 0x14];
    struct Obj *f50;
    unsigned char pad54;
    unsigned char f55;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetAnimSpeed(struct Actor *a, int n);

void OvlFunc_951_2008dd0(int slot, int *src, int h, int v, int sp)
{
    struct Actor *e;
    struct Obj *q;
    struct Ent *r;
    struct Ent **list;
    unsigned short z;
    unsigned int i;
    unsigned int n;
    int m;
    int t;

    e = __MapActor_GetActor(slot);
    if (e != 0) {
        e->f08 = *src++;
        e->f0c = *src++;
        e->f10 = *src;
        e->f06 = h;
        z = 0;
        e->f55 = z;
        e->f50->f26 = z;
        __Actor_SetAnimSpeed(e, sp);
    }
    q = e->f50;
    i = 0;
    if (i < q->f27) {
        m = 0xff;
        list = q->f28;
        n = q->f27;
        do {
            r = *list++;
            if (r->f05 != v) {
                t = r->f16 | m;
                r->f05 = v;
                r->f16 = t;
            }
            n--;
        } while (n != 0);
    }
}

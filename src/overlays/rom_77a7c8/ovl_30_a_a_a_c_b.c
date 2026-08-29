/* OvlFunc_881_200813c  --  0x0200813c
 *
 * Cut out of goldensun/asm/overlays/rom_77a7c8/ovl_30_a_a_a_c.s.
 *
 * Per-frame hook on one effect actor: set its two speed fields from a global
 * mode bit, and on another mode bit spawn a companion sparkle.
 *
 * THREE LEVERS, EACH WORTH A SENTENCE.
 *
 * 1. THE TWO STORES GO INSIDE BOTH ARMS. Written as `v = ...; a->f18 = v;
 *    a->f1c = v;` after an if/else, gcc goes BRANCHLESS -- it preloads the pool
 *    constant, then conditionally overwrites it -- and the ROM's diamond
 *    disappears. Duplicating the two stores into each arm gives the diamond and
 *    gcc cross-jumps the tail itself.
 *
 * 2. THE ACTOR ID NEEDS THE BASIC-BLOCK LEVER, and the symptom is a whole
 *    register. gcc issues `ldr r0, =0x11d` before the three `ldr rN, [r0, #k]`
 *    that fill r1-r3, so the incoming parameter has to be copied out of r0
 *    first -- a `mov r4, r0` the ROM does not have. `int id = 0x11d;` in a
 *    dominating block puts the pool load last and the parameter stays in r0 to
 *    the end. Same lever, same shape, as src/overlays/common/common1_c_a_c_c_a_a.c.
 *
 * 3. `neg` OF 0xd IS `~0xc`, NOT `~0xd`. Reading `mov r3, #0xd / neg r3, r3` as
 *    a mask that clears bits 0, 2 and 3 costs three extra instructions, because
 *    it turns one bitfield write into two. -13 is 0xfffffff3: bits 2 and 3
 *    only. It is a SINGLE two-bit field at offset 2 set to 1, and the `mov r2,
 *    #4 / orr` is its value. Worth stating because the batch-71 rule says a
 *    32-bit `mov`/`neg` pair means a bitfield, and getting the WIDTH of that
 *    bitfield wrong looks exactly like a codegen difference.
 */
struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 1,
                  b1 : 1,
                  b2 : 2,
                  b4 : 4;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x18 - 0x14];
    int f18;
    int f1c;
    unsigned char pad20[0x50 - 0x20];
    struct Sprite *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    short f64;
    unsigned char pad66[0x6c - 0x66];
    void (*f6c)(void);
};

extern unsigned int iwram_3001e40;
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void OvlFunc_881_200811c(void);

void OvlFunc_881_200813c(struct Actor *a)
{
    struct Actor *q;
    struct Sprite *s;
    int id;

    id = 0x11d;
    if (iwram_3001e40 & 4) {
        a->f18 = 0x14ccc;
        a->f1c = 0x14ccc;
    } else {
        a->f18 = 0x80 << 9;
        a->f1c = 0x80 << 9;
    }
    if (iwram_3001e40 & 2) {
        q = __CreateActor(id, a->f8, a->fc, a->f10);
        __PlaySound(0xf6);
        if (q != 0) {
            q->f55 = 0;
            s = q->f50;
            s->b2 = 1;
            __Actor_SetSpriteFlags(q, 0);
            __Actor_SetAnim(q, 1);
            q->f64 = 0;
            q->f6c = OvlFunc_881_200811c;
        }
    }
}

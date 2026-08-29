/* OvlFunc_935_2008b8c  --  asm/overlays/rom_7bf5a8/ovl_b8c_a.s
 *
 * Spawns up to four actors at the caller's position, each with jittered
 * velocity, and breaks out early if the spawn fails.
 *
 * An un-rotated loop: the ROM jumps straight to the test (`b .Lbda`), so the
 * `for` with the spawn AT THE TOP of the body and a `break` on failure is the
 * natural spelling and needs no goto.  Field writes are in the ROM's order
 * (+0x1c before +0x18); the two __Random results are consumed in call order.
 * No --cflags.
 */
struct Actor {
    unsigned char pad00[0x18];
    int f18;
    int f1c;
    unsigned char pad20[0x28 - 0x20];
    int f28;
    unsigned char pad2c[0x30 - 0x2c];
    int f30;
    unsigned char pad34[0x55 - 0x34];
    unsigned char f55;
    unsigned char pad56[3];
    unsigned char f59;
    unsigned char pad5a[0x5e - 0x5a];
    short f5e;
};

struct Src {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
};

extern unsigned char gScript_935__02009884[];
extern unsigned int __Random(void);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern void OvlFunc_935_2008b54(struct Actor *a, unsigned int b, unsigned int c);

void OvlFunc_935_2008b8c(struct Src *s)
{
    struct Actor *a;
    int i;

    for (i = 0; i <= 3; i++) {
        a = __CreateActor(0xf0, s->f8, s->fc, s->f10);
        if (a == 0)
            break;
        a->f1c = 0x8ccc;
        a->f18 = 0x8ccc;
        a->f55 = 2;
        a->f28 = 0xffff0000;
        a->f30 = __Random() + 0xcccc;
        a->f59 = 1;
        OvlFunc_935_2008b54(a, 0x200000, __Random());
        a->f5e = 8;
        __Actor_SetScript(a, gScript_935__02009884);
    }
}

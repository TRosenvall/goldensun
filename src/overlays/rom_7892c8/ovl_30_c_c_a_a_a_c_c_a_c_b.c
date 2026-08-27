/* OvlFunc_888_200b144  --  asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a.s
 *
 * Per-frame step for a spinning actor: advance y by the short at +0x64, pick a
 * spin speed from bits 2-3 of the counter at +0x66, then tick the counter down
 * and hand the actor back to a script when it expires.
 *
 * THE CASE ORDER IS THE SOURCE ORDER, AND CASES 1 AND 3 MUST SHARE A LABEL.
 * Written as four separate cases (case 3 repeating case 1's body) gcc still
 * cross-jumps them but lays the 0x9999 block before the 0xcccc one -- 4
 * differing of 54, and the only difference is which of the two pool loads
 * comes first.  Written `case 1: case 3:` the shared body takes case 1's
 * position and it is exact.
 *
 * The `ldr r3, =0x3` pool load of a three-bit constant comes out of the plain
 * `(a->f66 >> 2) & 3` on a short; nothing had to be done to provoke it.
 * No --cflags.
 */
struct Actor {
    unsigned char pad00[0xc];
    int fc;
    unsigned char pad10[0x18 - 0x10];
    int f18;
    int f1c;
    unsigned char pad20[0x3c - 0x20];
    int f3c;
    unsigned char pad40[0x64 - 0x40];
    short f64;
    short f66;
};

extern unsigned char gScript_888__0200c18c[];
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);

void OvlFunc_888_200b144(struct Actor *a)
{
    int v;

    a->fc = a->fc + (a->f64 << 12);
    a->f3c = a->fc;
    v = 0;
    switch ((a->f66 >> 2) & 3) {
    case 0:
        v = 0x10000;
        break;
    case 1:
    case 3:
        v = 0xcccc;
        break;
    case 2:
        v = 0x9999;
        break;
    }
    a->f18 = v;
    a->f1c = v;
    a->f66--;
    if (a->f66 <= 0)
        __Actor_SetScript(a, gScript_888__0200c18c);
}

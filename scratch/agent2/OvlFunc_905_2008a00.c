/* OvlFunc_905_2008a00  --  asm/overlays/rom_799abc/ovl_30_a_a_a_c_c_c_c.s
 *
 * A three-case switch on the short at +0x66 that adds the two velocity words
 * at +0x30/+0x34 into two of the three position words, mirroring each result
 * into +0x38/+0x3c/+0x40.
 *
 * Three case labels means a DECISION TREE, and the case bodies come out in
 * SOURCE order -- 0, 1, 2 here, which is also numeric order.  Cases 1 and 2
 * share their tail (`f10 += f34`) and gcc cross-jumps it into the ROM's .La4a.
 * No --cflags.
 */
struct S {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    int f34;
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0x66 - 0x44];
    short f66;
};

void OvlFunc_905_2008a00(struct S *s)
{
    switch (s->f66) {
    case 0:
        s->f8 = s->f8 + s->f30;
        s->f38 = s->f8;
        s->fc = s->fc + s->f34;
        s->f3c = s->fc;
        break;
    case 1:
        s->f8 = s->f8 + s->f30;
        s->f38 = s->f8;
        s->f10 = s->f10 + s->f34;
        s->f40 = s->f10;
        break;
    case 2:
        s->fc = s->fc + s->f30;
        s->f3c = s->fc;
        s->f10 = s->f10 + s->f34;
        s->f40 = s->f10;
        break;
    }
}

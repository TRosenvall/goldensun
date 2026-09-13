/* Cluster OvlFunc_907_2008db4..OvlFunc_907_2008ed8, the WHOLE of
 * goldensun/asm/overlays/rom_79b154/ovl_30_c_c_a.s.  The .s holds only these
 * two functions and no .data, so the file converts whole -- no split.
 *
 * WHAT UNPARKED 2008ed8: `ldr r3, =0` FEEDING A `strh` IS NOT A SYMBOL AND NOT
 * AN SImode CONSTANT.  The park read the ROM's two `ldr r3, .Lf30 / strh` as a
 * word load of a pooled zero -- the const.sym tell -- and every spelling that
 * chased that got `mov r3, #0`.  THE ASSEMBLER ENCODES A THUMB PC-RELATIVE
 * `ldrh rN, <label>` AS `ldr rN, [pc, #N]`: Thumb-1 has no pc-relative `ldrh`,
 * so gcc's HImode pool load and an SImode pool load are the SAME two bytes and
 * a disassembly cannot tell them apart.  The ROM's zero is gcc's ordinary
 * HImode constant pool entry.
 *
 * And which of the two forms gcc picks is a source question, measured here:
 *
 *     L1d88[0] = 0;                  ->  mov r3, #0        (what the park had)
 *     short *p = L1d88; p[0] = 0;    ->  ldrh r3, <pool>   (what the ROM has)
 *
 * Naming the halfword pointer is the whole lever; it is the exact INVERSE of
 * the recorded "int intermediate" rule, which exists to turn a pool into a mov.
 *
 * WHAT CLOSED 2008db4: the table word has to be read INSIDE the store
 * expression.  `w = tab[i]; p[0] = *(int *)(t + 8) + (w & 0xffff0000);` lets
 * the table base die before `&p` is born, so gcc shares one low callee-saved
 * register between them and comes out one register short of the ROM (33 of
 * 121).  Written inline -- `p[0] = *(int *)(t + 8) + (tab[i] & 0xffff0000);`
 * -- expand materialises the LHS address FIRST, the two live ranges overlap,
 * the base is pushed up to r10 and the zero gets r9: 33 -> 8 -> exact.
 *
 * The gState word in 2008ed8 must be reached through the `base`/`off` idiom of
 * ovl_30_a_b.c.  Folded (`*(int *)((char *)&gState + 0x1f4)`) gcc pools
 * `gState+500` and the five-instruction address build collapses to two.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_1e;
extern int _AREA_20;
extern int _AREA_23;
extern int gOvl_02009d3c[];
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char *OvlFunc_907_2008d80(int x, int y);
extern int __TestCollision(unsigned char *a, int *v);
extern void __Actor_SetAnim(unsigned char *a, int anim);
extern void __WaitFrames(int frames);
extern void __PlaySound(int id);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_WaitMovement(unsigned char *a);
extern void OvlFunc_907_20089cc(void);
extern void OvlFunc_907_2008cb4(void);
extern void OvlFunc_907_2008fa0(void);
extern void OvlFunc_907_2008f3c(unsigned char *src);
extern short L1d88[] __asm__(".L1d88");

void OvlFunc_907_2008db4(void)
{
    unsigned char *a;
    unsigned char *t;
    unsigned int base;
    unsigned int off;
    int i;
    int x;
    int y;
    int p[3];
    short area;

    a = __MapActor_GetActor(0);
    i = *(unsigned short *)(a + 6) >> 12;
    x = *(short *)(a + 0xa) + (gOvl_02009d3c[i] >> 16);
    y = *(short *)(a + 0x12) + (short)gOvl_02009d3c[i];
    t = OvlFunc_907_2008d80(x >> 4, y >> 4);
    if (t == 0)
        return;
    {
        int zero = 0;

        t[0x22] = 2;
        p[0] = *(int *)(t + 8) + (gOvl_02009d3c[i] & 0xffff0000);
        p[1] = *(int *)(t + 0xc);
        p[2] = *(int *)(t + 0x10) + (gOvl_02009d3c[i] << 16);
        if (__TestCollision(t, p) > 0)
            return;
        __Actor_SetAnim(a, 8);
        __WaitFrames(0xf);
        __PlaySound(0xb9);
        *(int *)(t + 0x30) = 0x3333;
        *(int *)(t + 0x34) = 0x3333;
        __Actor_TravelTo(t, p[0], p[1], p[2]);
        *(int *)(a + 0x30) = 0x3333;
        *(int *)(a + 0x34) = 0x3333;
        __Actor_TravelTo(a, p[0], p[1], p[2]);
        __Actor_WaitMovement(t);
        *(int *)(t + 8) = p[0];
        *(int *)(t + 0x10) = p[2];
        *(int *)(t + 0x24) = zero;
        *(int *)(t + 0x2c) = zero;
    }
    __Actor_SetAnim(a, 1);
    base = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    base += off;
    off = 0;
    area = *(short *)((char *)base + off);
    if (area == (int)&_AREA_23)
        OvlFunc_907_2008cb4();
    else if (area == (int)&_AREA_1e)
        OvlFunc_907_20089cc();
    else if (area == (int)&_AREA_20)
        OvlFunc_907_2008fa0();
}

void OvlFunc_907_2008ed8(void)
{
    unsigned char *a;
    unsigned int base;
    unsigned int off;

    base = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    base += off;
    a = __MapActor_GetActor(*(int *)base);
    if (*(int *)(a + 8) >= 0x8e0000)
        return;
    if (*(int *)(a + 0xc) < 0x80000) {
        short *p = L1d88;

        if (p[0] == 0)
            OvlFunc_907_2008f3c(a);
        p[0] = p[0] + 1;
        if (p[0] == 0x1e)
            p[0] = 0;
    } else {
        short *q = L1d88;

        q[0] = 0;
    }
}

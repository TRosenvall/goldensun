/*
 * OvlFunc_895_20087d0 (TryPushBlock) -- asm/overlays/rom_78dee8/ovl_30_c_c_c_a_a.s
 *
 * BLOCKER: the allocator uses one high register where the ROM uses three.
 * 117 lines against 123 -- SIX SHORT, and the six are the extra
 * `mov r6, r9 / mov r5, r8` pair before the push and their restores after the
 * pop.
 *
 *      rom   r8 = player actor, r9 = the constant 0, r10 = the direction table
 *      ours  r8 = player actor, table in r6 (a LOW callee-saved), 0 rebuilt
 *
 * The structure is believed right: the whole body screens in order, the two
 * table reads reload through the same base and offset the ROM uses, and the
 * three-way entrance dispatch at the end lines up.
 *
 * The gap is the ROM keeping a ZERO live in r9 across eleven calls, to store it
 * twice at the end into +0x24 and +0x2c. gcc will not spend a callee-saved
 * register on a value it rebuilds in one instruction -- the same rule recorded
 * for OvlFunc_948_2009ac8, Func_80b0070 and OvlFunc_926_200a5b8, where it
 * blocked two functions and delivered a third.
 *
 * The table pointer is a named local and is used twice with a call between, and
 * gcc does keep it -- just in r6 rather than r10, because with only one value
 * forced high there is no pressure to reach further up.
 *
 * NOT TRIED, and probably the only route: something that raises the live-value
 * count past what r4-r7 can hold, so the allocator has to use r9 and r10.
 * Every attempt at that in this session has come from ADDING named locals,
 * which changes the source rather than the codegen and has not worked.
 */
extern unsigned char gState[];
extern unsigned char L265c[] __asm__(".L265c");
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char *OvlFunc_895_200879c(int x, int y);
extern int __TestCollision(unsigned char *a, int *b);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_WaitMovement(unsigned char *a);
extern void OvlFunc_895_200856c(void);
extern void OvlFunc_895_20085ac(void);
extern void OvlFunc_895_20085ec(void);
extern void OvlFunc_895_2008634(void);
extern void OvlFunc_895_200867c(void);
extern void OvlFunc_895_20086c4(void);
extern void OvlFunc_895_200870c(void);
extern void OvlFunc_895_2008754(void);

void OvlFunc_895_20087d0(void)
{
    int buf[3];
    int *bp;
    unsigned char *e;
    unsigned char *b;
    unsigned char *tbl;
    unsigned char *gp;
    int off;
    int d;
    int z;
    int sp;
    int a;

    e = __MapActor_GetActor(0);
    off = (*(unsigned short *)(e + 6) >> 12) << 2;
    tbl = L265c;
    d = *(int *)(tbl + off);
    b = OvlFunc_895_200879c((*(short *)(e + 0xa) + (d >> 16)) >> 4,
                            (*(short *)(e + 0x12) + ((d << 16) >> 16)) >> 4);
    if (b == 0)
        return;
    z = 0;
    b[0x22] = 2;
    d = *(int *)(tbl + off);
    bp = buf;
    bp[0] = *(int *)(b + 8) + (d & 0xffff0000);
    bp[1] = *(int *)(b + 0xc);
    bp[2] = *(int *)(b + 0x10) + (d << 16);
    if (__TestCollision(b, bp) > 0)
        return;
    __Actor_SetAnim(e, 8);
    sp = 0x3333;
    __WaitFrames(0xf);
    __PlaySound(0xb9);
    *(int *)(b + 0x30) = sp;
    *(int *)(b + 0x34) = sp;
    __Actor_TravelTo(b, bp[0], bp[1], bp[2]);
    *(int *)(e + 0x30) = sp;
    *(int *)(e + 0x34) = sp;
    __Actor_TravelTo(e, bp[0], bp[1], bp[2]);
    __Actor_WaitMovement(b);
    *(int *)(b + 8) = bp[0];
    *(int *)(b + 0x10) = bp[2];
    *(int *)(b + 0x24) = z;
    *(int *)(b + 0x2c) = z;
    __Actor_SetAnim(e, 1);
    gp = gState;
    a = *(short *)(gp + (0xe1 << 1));
    if (a < 0xb)
        return;
    if (a <= 0xd) {
        OvlFunc_895_200856c();
        OvlFunc_895_20085ac();
        return;
    }
    if (a > 0x10)
        return;
    OvlFunc_895_20085ec();
    OvlFunc_895_2008634();
    OvlFunc_895_200867c();
    OvlFunc_895_20086c4();
    OvlFunc_895_200870c();
    OvlFunc_895_2008754();
}

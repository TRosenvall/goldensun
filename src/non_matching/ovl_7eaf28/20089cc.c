/* OvlFunc_960_20089cc  --  0x020089cc  [asm/overlays/rom_7eaf28/ovl_314_c_a_c_c_c_c_c_c_c_c_c_c_c_a_c.s]
 *
 * NOT MATCHING. Best 115 of 128, ours 124 lines -- FOUR INSTRUCTIONS SHORT, and
 * those four are now identified exactly, which is the value of this park. The
 * .s holds this function alone with no data tail.
 *
 * An actor update: copy some fields, measure the Manhattan distance to another
 * actor, and if it is close enough do extra work and spawn an effect through an
 * eight-argument call with a 0x28-byte local struct.
 *
 * THE FOUR MISSING INSTRUCTIONS ARE A TAIL DUPLICATION. The distance test is
 *
 *     dy = a[0x10] - s[0x10];  if (dy < 0) dy = s[0x10] - a[0x10];
 *     if (dx + dy < (0x80 << 12)) { ... }
 *
 * and gcc compiles that with ONE copy of the add-and-compare, joined after the
 * sign fix-up. THE ROM HAS TWO COPIES, one in each arm:
 *
 *     sub r3,r0,r1 / cmp r3,#0 / blt .L3
 *       add r3,r2,r3 / mov r2,#0x80 / lsl r2,#0xc / cmp r3,r2 / blt .L4 / b .L5
 *     .L3:
 *       sub r3,r1,r0 / add r3,r2,r3 / mov r2,#0x80 / lsl r2,#0xc / cmp r3,r2 / bge .L5
 *     .L4:
 *
 * -- `add / mov / lsl / cmp` appears twice, which is the four-instruction
 * shortfall, and the branch asymmetry (`blt .L4 / b .L5` against a bare
 * `bge .L5`) is the signature of a block that was duplicated rather than
 * shared. The absolute-value fix-up itself is right; it is the CONSUMER of dy
 * that the ROM copies.
 *
 * WHAT LANDED FIRST, and both were needed before the shortfall was visible:
 * `gState` must be the array idiom with a named local, or the base and offset
 * fold into one pool word (`ldr r3, =gState+500` against the ROM's
 * `ldr r3, =0x2000240 / mov r2, #0xfa / lsl r2, #1 / add`); and the parameter
 * has to be copied into a pinned callee-saved local, because the ROM keeps it
 * in r6 and the fetched actor in r5 while gcc gives the parameter r5. Those two
 * took 127 of 121 to 115 of 124 and moved the first divergence from instruction
 * 1 to 12.
 *
 * NEXT: this is the "how much of a duplicated tail has to be duplicated in the
 * source" question that docs/elevation.md already records for the mirror case.
 * The obvious try is to write the comparison inside BOTH arms of the sign test
 * -- which also duplicates the body, so the body would have to be a helper or
 * a goto target. Neither was tried; the function was set aside to finish the
 * batch. The remaining 115 is dominated by the four-instruction shift, so the
 * count will drop sharply once the duplication is right and should be re-read
 * from scratch afterwards rather than trusted now.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern int iwram_3001e40;

extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern int __GetFlag(int id);
extern int __Random(void);
extern void OvlFunc_common0_10c(int a, int b, int c, int d, int e, int f,
                                int g, void *h);

int OvlFunc_960_20089cc(unsigned char *arg)
{
    register unsigned char *a __asm__("r6");
    unsigned char *g;
    register unsigned char *s __asm__("r5");
    unsigned char *t;
    unsigned char *u;
    unsigned char *w;
    unsigned char f[0x28];
    int dx, dy;
    int z;
    int r;

    a = arg;
    g = gState;
    s = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    *(int *)(a + 0x34) = 0x80 << 7;
    *(int *)(a + 0x30) = 0xc0 << 9;
    t = a + 0x55;
    *t = 0;
    __Actor_SetSpriteFlags(a, 0);
    u = a + 0x54;
    *u ^= 1;
    if (__GetFlag(0x82 << 1) != 0) {
        *(int *)(a + 0x38) = 0x80 << 24;
        *(int *)(a + 0x3c) = 0x80 << 24;
        *(int *)(a + 0x40) = 0x80 << 24;
    } else {
        *(int *)(a + 0x38) = *(int *)(s + 8);
        *(int *)(a + 0x3c) = *(int *)(s + 0x14);
        *(int *)(a + 0x40) = *(int *)(s + 0x10);
        dx = *(int *)(a + 8) - *(int *)(s + 8);
        if (dx < 0)
            dx = *(int *)(s + 8) - *(int *)(a + 8);
        dy = *(int *)(a + 0x10) - *(int *)(s + 0x10);
        if (dy < 0)
            dy = *(int *)(s + 0x10) - *(int *)(a + 0x10);
        if (dx + dy < (0x80 << 12)) {
            w = iwram_3001ebc;
            if (s[0x55] != 0)
                *(short *)(w + (0xc1 << 1)) = 0x37;
            *t = 3;
            *(int *)(a + 0x38) = *(int *)(s + 8);
            *(int *)(a + 0x3c) = *(int *)(s + 0xc);
            *(int *)(a + 0x40) = *(int *)(s + 0x10);
        }
    }
    z = iwram_3001e40 & 7;
    if (z == 0) {
        *(int *)(f + 8) = 0xcccc;
        *(int *)(f + 0xc) = 0xcccc;
        r = __Random();
        *(short *)(f + 0x22) = ((unsigned)(r << 12) >> 16) + (0xf8 << 8);
        OvlFunc_common0_10c(*(int *)(a + 8), *(int *)(a + 0xc),
                            *(int *)(a + 0x10), 0, z, z, 0x880001, f);
    }
    return 1;
}

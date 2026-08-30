/*
 * OvlFunc_948_2009ac8 -- asm/overlays/rom_7d30e0/ovl_30_c_c_a_a_c_c_a_c_c_a_a.s
 *
 * BLOCKER: a materialised zero that gcc will not keep in a callee-saved
 * register. 68 lines against 71 -- THREE SHORT, and the three are
 * `push {r6}` / `mov r6, #0` / `pop {r6}`.
 *
 * The ROM sets `mov r6, #0` before the four-way branch and uses it in the last
 * arm, twenty instructions and three calls later, holding it in a callee-saved
 * register the whole way. gcc rematerialises the zero at the point of use --
 * which is strictly cheaper, and is why it will not be talked out of it.
 *
 * TRIED AND REJECTED, measured:
 *
 *   * Naming the zero in a local assigned just before the branch (as written
 *     below). This is what makes the ROM's shape reachable in principle and it
 *     is not enough on its own.
 *   * Moving that assignment to the very top of the function, to lengthen the
 *     live range across three more calls. NO CHANGE -- still 47 differing.
 *
 * This is the same blocker as Func_80b0070, where the ROM builds its zero in
 * r1 and we build it in r2. Both are the register allocator declining to spend
 * a callee-saved register on a value it can rebuild in one instruction, and
 * neither has yielded to a source spelling yet. They should be re-attacked
 * together.
 *
 * SETTLED, and it was worth 8 differing:
 *
 *   Both fields the ROM reads from the actor -- +8 and +0xc -- are loaded
 *   BEFORE the sign test that begins the signed division. Writing
 *   `d = *(int *)(p + 8) / 0x100000;` and testing `*(int *)(p + 0xc)`
 *   separately makes gcc interleave the second load into the division; naming
 *   both loads in locals first reproduces the ROM's pair of `ldr`s. 55
 *   differing to 47.
 *
 *   `*(int *)(p + 8) / 0x100000` is a signed division by a power of two, not a
 *   shift -- the `if (x < 0) x += 0xfffff` fixup is gcc's own expansion and the
 *   constant never appears in the source.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_948_20099e8(void);
extern void OvlFunc_948_2009a9c(void);
extern void OvlFunc_948_2009a48(void);
extern void OvlFunc_948_2009a70(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_948_2009ac8(void)
{
    unsigned char *p;
    int d;
    int z;
    int n;
    int a;
    int b;

    p = __MapActor_GetActor(8);
    a = *(int *)(p + 8);
    b = *(int *)(p + 0xc);
    d = a / 0x100000;
    if (b == 0)
        __MapActor_GetActor(8)[0x23] = 2;
    OvlFunc_948_20099e8();
    z = 0;
    __MapActor_GetActor(8)[0x55] = 3;
    if (d == 0x28) {
        OvlFunc_948_2009a9c();
    } else if (d == 0x2a) {
        OvlFunc_948_2009a48();
    } else if (d == 0x29) {
        OvlFunc_948_2009a70();
    } else if (d == 0x27 || d == 0x26 || d == 0x25) {
        n = 0x2a;
        __Func_8010704(0x3d, 0x24, 1, 1, d, n);
        __MapActor_GetActor(8)[0x55] = z;
        *(int *)(__MapActor_GetActor(8) + 0xc) = 0x80 << 14;
    }
}

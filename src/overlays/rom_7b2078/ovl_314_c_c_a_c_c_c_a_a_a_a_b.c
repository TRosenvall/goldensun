/* OvlFunc_926_2008db4  --  0x02008db4
 *
 * Cut out of goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_a.s.
 *
 * Drops actor 0x13 four steps -- each step waiting two frames fewer than the
 * last, 8 down to 2 -- then clears a halfword on its sub-object, plays a thud,
 * and fires four particle bursts around the landing point.
 *
 * Found by tools/templated.py at a perfect 1.00. The neighbour supplied the
 * eight-argument signature of OvlFunc_common0_10c and the struct its last
 * argument points at, which is most of what this function needed.
 *
 * MATCHED ON THE FIRST CANDIDATE BUT FOR ONE PAIR OF REGISTERS. The residue was
 * six instructions, all of it an r5/r6 swap: the ROM keeps the loop counter in
 * r6 and the frame count in r5, and we had them the other way round.
 *
 * ASSIGNMENT ORDER IS THE LEVER; DECLARATION ORDER IS INERT. Measured, and the
 * contrast is the useful part:
 *
 *     n = 8; for (i = 0; ...)          6 differing  -- n materialised first
 *     i = 0; n = 8; for (; ...)        0            -- counter first
 *     for (i = 0, n = 8; ...)          0
 *     swapping the two DECLARATIONS    6            -- no effect at all
 *
 * The ROM opens the block with `mov r6, #0 / mov r5, #8`, so the counter is
 * materialised first and takes the earlier register. Writing `n = 8` before the
 * loop reverses that. Swapping which variable is DECLARED first changes
 * nothing, because local-alloc orders by priority rather than by pseudo number.
 *
 * This is worth separating from the r8/r10 rotations parked in
 * src/non_matching/overlays/200b668.c, which look like the same class and are
 * not. There the two values are initialised far apart -- one before a call, one
 * after -- and no source order reaches the allocator, so six spellings tie.
 * Here the two initialisations are ADJACENT and in the same basic block, so
 * their order is still visible to local-alloc and the source has a vote.
 *
 * The rule to carry: for a register swap between two values initialised
 * ADJACENTLY, sweep the ASSIGNMENT order before concluding allocation order.
 * For values initialised far apart, do not bother -- that is the allocator.
 *
 * `bls` on the trip count makes the counter unsigned; signed gives `ble`.
 */

struct St {
    int f0;
    int f4;
    unsigned char pad8[0x10];
    unsigned short f18;
    unsigned char pad1a[0xe];
};

struct B {
    unsigned char pad00[0x1e];
    unsigned short f1e;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x40 - 0x14];
    int f40;
    unsigned char pad44[0x50 - 0x44];
    struct B *f50;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void OvlFunc_common0_10c(int a, int b, int c, int d, int e, int f, int g, struct St *h);

void OvlFunc_926_2008db4(void)
{
    struct Actor *a;
    unsigned int i;
    int n;

    a = __MapActor_GetActor(0x13);
    for (i = 0, n = 8; i <= 3; i++) {
        __WaitFrames(n);
        a->f10 -= 0x10000;
        a->f40 = 0x80000000;
        n -= 2;
    }
    a->f50->f1e = 0;
    __PlaySound(0xe3);
    OvlFunc_common0_10c(a->f8, a->fc, a->f10 - 0x80000, 0xffff3334, 0, 0xffffcccd, 0, 0);
    OvlFunc_common0_10c(a->f8, a->fc, a->f10 - 0x80000, 0xcccc, 0, 0xffffcccd, 0, 0);
    OvlFunc_common0_10c(a->f8 - 0x60000, a->fc, a->f10 + 0xa0000, 0x3333, 0, 0xffff0000, 0, 0);
    OvlFunc_common0_10c(a->f8 + 0x60000, a->fc, a->f10 + 0xa0000, 0x3333, 0, 0xffff0000, 0, 0);
}

/* OvlFunc_common0_70  --  asm/overlays/common/common0_a_b.s
 *
 * Create an actor and initialise it; returns 0 if the spawn failed.
 *
 * TWO THINGS DECIDED IT.
 *
 * 1. THE NULL RETURN MUST BE INSIDE AN `else`-SHAPED TAIL.  Written as the
 *    guard `if (a == 0) return 0;` followed by the body, gcc hoists
 *    `mov r0, #0` above the `cmp` and merges the two exits -- 38 differing of
 *    48, every line shifted by one.  Written as `if (a != 0) { body; return
 *    a; } return 0;` the ROM's `mov r0, r5 / b .Lfc / .Lfa: mov r0, #0`
 *    reappears.  A `goto` to a trailing `return 0;` also matches; assigning a
 *    result variable in both arms does not.
 *
 * 2. THE TWO MASKS ARE DIFFERENT KINDS.  +9 of the sub-object is a two-bit
 *    BITFIELD (`mov r3, #0xd / neg` = 32-bit ~0xc); +0x23 of the actor is
 *    HAND-WRITTEN masking (`mov r3, #0xfe`, a byte-width immediate).  Spelling
 *    +0x23 as two 1-bit bitfields costs the extra `neg`.  This is batch 71's
 *    width rule doing exactly what it says.
 * No --cflags.
 */
struct Sub {
    unsigned char pad0[9];
    unsigned char f9_b0 : 2;
    unsigned char f9_b2 : 2;
    unsigned char f9_b4 : 4;
};

struct Actor {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Sub *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[3];
    unsigned char f59;
};

extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Func_80929d8(struct Actor *a, int n);

struct Actor *OvlFunc_common0_70(int x, int y, int z, int id)
{
    struct Actor *a;

    a = __CreateActor(id, x, y, z);
    if (a != 0) {
        a->f50->f9_b2 = 1;
        a->f55 = 0;
        a->f59 = 8;
        __Actor_SetSpriteFlags(a, 0);
        __Func_80929d8(a, 0xf);
        a->f23 = (a->f23 & 0xfe) | 2;
        return a;
    }
    return 0;
}

/* Cluster OvlFunc_904_2008054..OvlFunc_904_2008054 split out of goldensun/asm/overlays/rom_799998/ovl_30_c_c.s.
 *
 * Code to this file, the trailing .section .data to its _c sibling.
 *
 * THE BASIC-BLOCK LEVER, IN ITS WORKING DIRECTION. First body was 2 differing
 * on the __MapActor_SetPos argument block: the ROM fills `mov r0, #8` before
 * `lsl r1, #0x10` and gcc did the reverse. The declaration lever does NOT
 * reach it -- SetPos implicit, GetFlag implicit, both implicit, SetPos
 * returning `int`, and named argument locals inside the `if` are all still 2.
 *
 * Assigning both coordinates in the block ABOVE the `if (__GetFlag(...))`
 * closes it: gcc then rematerialises them split at the call with `mov r0`
 * scheduled into the gap. BOTH coordinates must move -- hoisting only x is 2,
 * only z is 4. This is the guarded case of the same shape that is unreachable
 * in straight-line functions.
 */
struct Blk {
    unsigned char pad000[0x1c0];
    unsigned int stepDelay;
    unsigned char pad1c4[4];
    unsigned int msgDelay;
};

extern struct Blk *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

int OvlFunc_904_2008054(void)
{
    struct Blk *b;
    int x;
    int z;
    int s0;
    int s1;
    int zero;

    b = iwram_3001ebc;
    b->stepDelay = 0x204;
    b->msgDelay = 0x18;
    x = 0xd8 << 16;
    z = 0x88 << 16;
    if (__GetFlag(0x300)) {

        __MapActor_SetPos(8, x, z);
        __MapActor_SetAnim(8, 2);
        __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
        *(__MapActor_GetActor(8) + 0x23) = 2;
        zero = 0;
        *(__MapActor_GetActor(8) + 0x59) = zero;
        s0 = 0xb;
        s1 = 6;
        __Func_8010704(0xb, 0x24, 5, 5, s0, s1);
    }
    return 0;
}

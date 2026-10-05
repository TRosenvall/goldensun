/* OvlFunc_955_200862c -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7ddb88/ovl_30_c_c_c_a_c_a.s
 * Best screen: 8 differing of 49, streams the same length.
 *
 * BLOCKER CLASS: a shifted field read placed in a SECOND register.
 *
 *     rom    ldr r3, [r0, #8] / asr r2, r3, #0x14 / str r2, [sp]
 *     ours   ldr r3, [r0, #8] / asr r3, #0x14     / str r3, [sp]
 *
 * Thumb's immediate ASR is always three-operand; ours simply has rd == rs. The
 * ROM keeps the loaded word and the shifted result in different registers even
 * though the loaded word is dead immediately. It does this at all three call
 * sites, which is what makes eight positions differ.
 *
 * TRIED AND IDENTICAL AT 8: naming the loaded field into its own `int` local so
 * the load and the shift are separate statements -- the spelling that usually
 * separates two values into two registers.
 *
 * This is the same residue as src/non_matching/ovl_7db0c8/200842c.c, which has
 * it once rather than three times and has absorbed six spellings. Anything that
 * moves one will move the other.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_955_200862c(void)
{
    int f;

    f = 0xb;
    __Func_8010704(0x64, 0xb, 0xc, 4, 0xe, f);
    __Func_8010704(0xd, 0x1c, 1, 4, __MapActor_GetActor(0xf)->f8 >> 20, f);
    __Func_8010704(0xd, 0x1c, 1, 4, __MapActor_GetActor(0x10)->f8 >> 20, f);
    __Func_8010704(0xd, 0x1c, 4, 1, 0x12,
                   __MapActor_GetActor(0x11)->f10 >> 20);
}

/* OvlFunc_944_2008a84  --  0x02008a84
 *
 * Cut out of goldensun/asm/overlays/rom_7ca63c/ovl_30_c_c_a_c_c.s; the other
 * seven functions stay as assembly on either side of it.
 *
 * Scatters one actor: four calls to __Random place its two sprite offsets, its
 * depth and its horizontal position, then it gets a fixed pair of speeds and a
 * behaviour script.
 *
 * __Random RETURNS UNSIGNED, and that was the entire difference -- two
 * instructions of forty-seven:
 *
 *      rom    lsr r0, #0xf        ours   asr r0, #0xf
 *
 * A right shift of a signed value is arithmetic and of an unsigned value is
 * logical, so the `lsr` says the return type. Nothing else in the function
 * distinguishes them, because the other three uses feed shift pairs that
 * discard the sign bit anyway.
 *
 * THE TWO SHIFT PAIRS ARE 16-BIT EXTRACTS AT AN OFFSET, which is worth
 * recording as a shape:
 *
 *      lsl r0, #2  / lsr r0, #16      ==  (unsigned short)(x >> 14)
 *      lsl r3, #13 / lsr r3, #16      ==  (unsigned short)(x >> 3)
 *
 * gcc reaches a narrowing cast of a shifted value in two instructions this way
 * rather than the three a literal `>> n` then mask would take. Read the pair as
 * "take sixteen bits starting at bit (32 - lsl - 16)"; here bits 14 and 3.
 *
 * The 0x4c write then adds 0xffffd000, which is -0x3000 -- the ROM pools it
 * because it cannot be built by `mov`, so it is written as the unsigned
 * constant the pool actually holds rather than as a subtraction.
 */
struct A {
    unsigned char pad00[0xc];
    int fc;
    unsigned char pad10[8];
    int f18;
    int f1c;
    unsigned char pad20[0x4c - 0x20];
    int f4c;
    unsigned char pad50[0x55 - 0x50];
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    unsigned short f64;
    unsigned short f66;
};

extern struct A *__MapActor_GetActor(int slot);
extern unsigned int __Random(void);
extern void __Func_8092b08(int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *script);
extern unsigned char gScript_944__020093a4[];

void OvlFunc_944_2008a84(int slot)
{
    struct A *a;

    a = __MapActor_GetActor(slot);
    __Func_8092b08(slot, 1);
    a->f55 = 0;
    a->f64 = __Random() >> 15;
    a->f66 = __Random() >> 15;
    a->fc = ((unsigned short)(__Random() >> 14) << 16) + (0xc0 << 11);
    a->f4c = (unsigned short)(__Random() * 3 >> 3) + 0xffffd000;
    a->f18 = 0xa0 << 9;
    a->f1c = 0xa0 << 9;
    __MapActor_SetBehavior(slot, gScript_944__020093a4);
}

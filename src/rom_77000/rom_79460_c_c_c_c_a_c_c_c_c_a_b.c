/* Cluster Func_807a498..Func_807a498 extracted from goldensun/asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_c_a.s.
 *
 * Total .text for this TU = 184 bytes (= 0xb8). Never attempted before batch 277.
 * No pins, no flags. 84 instructions, the strongest same-stem twin left in the 61-100 band.
 *
 * TWO LEVERS.
 *
 * 1. A BYTE INDEX PAST THE 5-BIT OFFSET WANTS THE INDEX NAMED, NOT THE ADDRESS. Written
 *    `p[arg1 + 0x118]`, gcc folds the base into the address and spends three instructions:
 *    `add r2, r7, r5 / add r2, r3 / ldrb r3, [r2, #0]`. Written `idx = arg1 + 0x118;` then
 *    `p[idx]`, it emits the ROM's register-offset form in two: `add r2, r5, r3 /
 *    ldrb r3, [r7, r2]`. That fixed the LENGTH in one edit, 38 differing to 3.
 *
 *    The sibling Func_807a350 already spells it this way -- a file-mate idiom that
 *    transferred verbatim, and a reminder that the cheapest lever is usually in the next
 *    function along. Read it with the recorded rule about byte fields past Thumb's 5-bit
 *    `strb` offset: that rule says "use a typed struct field", and this is its other form,
 *    for when the offset is not a constant.
 *
 * 2. A CALLEE'S RETURN TYPE IS AN ARGUMENT-FILL-ORDER LEVER -- AND THE CALLEE'S OWN LISTING
 *    SETTLES IT. The last 3 differences were `Func_807a3a8(arg0, arg1, arg2)` filling
 *    r0,r1,r2 where the ROM fills r1,r2,r0. Declaring it `int` rather than `void` is exact.
 *
 *    This is not a guess dressed up as a match: Func_807a3a8 ends `mov r0, r9 / pop {r1} /
 *    bx r1`, so it really does return a value -- the `pop {r1}` tell recorded in batch 276.
 *    And the same change on Func_807a458, Func_807a350 and SetDjinni was INERT at 3
 *    differing each. So SCREEN THE CALLEE THE DIFF POINTS AT, and confirm against that
 *    callee's own epilogue rather than sweeping every declaration in the file.
 *
 *    Read alongside batch 276's counter-example, where an argument-fill difference was a
 *    CONSEQUENCE of a register allocation and all four void/int combinations were
 *    byte-identical. The discriminator is the callee's epilogue, which is checkable.
 */
extern unsigned int GetUnit(unsigned int);
extern unsigned int Func_807a2bc(unsigned int, unsigned int, unsigned int);
extern unsigned int GiveDjinni(unsigned int, unsigned int, unsigned int);
extern void Func_807a350(unsigned int, unsigned int, unsigned int);
extern void SetDjinni(unsigned int, unsigned int, unsigned int);
extern int Func_807a3a8(unsigned int, unsigned int, unsigned int);
extern void Func_807a458(unsigned int, unsigned int, unsigned int);

int Func_807a498(unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3)
{
    unsigned char *p;
    unsigned int off;
    unsigned int mask;
    unsigned int t;
    unsigned int idx;

    p = (unsigned char *)GetUnit(arg0);
    off = (arg1 << 2) + 0xf8;
    mask = 1 << arg2;
    if ((*(unsigned int *)(p + off) & mask) != 0) {
        t = Func_807a2bc(arg0, arg1, arg2);
        if (GiveDjinni(arg3, arg1, arg2) == 0) {
            Func_807a350(arg0, arg1, arg2);
            *(unsigned int *)(p + off) &= ~mask;
            idx = arg1 + 0x118;
            p[idx] += 0xff;
            if (t != 0) {
                SetDjinni(arg3, arg1, arg2);
            } else {
                Func_807a3a8(arg0, arg1, arg2);
                Func_807a458(arg3, arg1, arg2);
            }
            return 0;
        }
    }
    return -1;
}

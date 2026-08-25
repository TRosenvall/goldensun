/* Func_8096ab0  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_8a000/rom_944ec_a_c_c_a_a_a.s
 * Best screen: 8 instructions in disagreeing regions, of 25 (rom 25, ours 24).
 *
 * BLOCKER CLASS: constant-CSE across a branch.
 *
 * The ROM materialises the constant 0 THREE times: twice as the register
 * operand of a Thumb `ldrsh` (which has no immediate-offset form, so the zero
 * is forced by the ISA, not by the source) and once more as the value stored
 * by the trailing `strb`.  gcc materialises it twice and reuses the second
 * `mov r1, #0` as the stored value, so our stream is one instruction shorter:
 *
 *      rom   mov r2, #0x0        <-- absent from ours
 *      rom   strb r2, [r3, #0x0]
 *      ours  strb r1, [r3, #0x0]
 *
 * WHAT WAS TRIED
 *
 *  1. Giving the stored zero its own local (`z = 0; *q = z;`), assigned after
 *     the `if (a == b) return;` so that it sits in a DIFFERENT BASIC BLOCK
 *     from the ldrsh offset.  No effect at all -- byte-identical output, same
 *     8 of 25.  The basic-block lever works against local-alloc's
 *     update_equiv_regs; this sharing is global CSE, which runs earlier and
 *     does not care about block boundaries.
 *  2. Separate named offset locals for 0x1e / 0x24a / 0x1a, which is what
 *     produces the correct register-offset ldrsh shape in the first place.
 *     Kept below; it is what gets this to 8.
 *
 * The remaining 8 are this one missing `mov` plus the register renaming it
 * drags along (ROM walks r1 where we use r0).
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001f30;
extern void Func_8097608(void);

void Func_8096ab0(void)
{
    unsigned char *p;
    unsigned char *g;
    unsigned char *q;
    unsigned int o1;
    unsigned int o2;
    unsigned int o3;
    short a;
    short b;

    p = iwram_3001f30;
    o1 = 0x1e;
    if (*(short *)(p + o1) != 2)
        return;
    Func_8097608();
    o2 = 0x24a;
    g = (unsigned char *)&gState + o2;
    o2 = 0;
    a = *(short *)(g + o2);
    o3 = 0x1a;
    b = *(short *)(p + o3);
    if (a == b)
        return;
    q = *(unsigned char **)(p + 0x14);
    q += 0x5b;
    *q = 0;
}

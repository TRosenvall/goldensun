/* Func_8006384  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_c0/rom_5cf8_a_a_c_a.s
 * Best screen: 9 instructions in disagreeing regions, of 25 (rom 25, ours 22).
 *
 * BLOCKER CLASS: operand canonicalisation on AND, register-to-register form.
 *
 * A spin-wait: poll a halfword global until all the bits of the mask argument
 * are set, then return bits 4-5 of REG_SIOCNT.  The ROM tests it like this,
 * three instructions, twice:
 *
 *      ldrh r2, [r1]
 *      mov  r3, r5        <- COPY the mask
 *      and  r3, r2        <- and the copy is the destination
 *
 * gcc does it in two, and the loaded value is the destination:
 *
 *      ldrh r3, [r6]
 *      and  r3, r5
 *
 * Ours is strictly shorter, so this is not a missed optimisation on our side;
 * the ROM's version keeps the mask live in r5 by copying it, which gcc has no
 * reason to do because `and r3, r5` already leaves r5 intact.
 *
 * WHAT WAS TRIED
 *
 *   Spelling the copy explicitly -- `t = m; t &= v;` in both the entry test and
 *   the loop -- which is the "constant is the destination" lever.  NO CHANGE AT
 *   ALL, still 9 of 25.  gcc canonicalises the register-register AND regardless
 *   of which side the source names first.
 *
 * That lever DOES work when one operand is a literal: see
 * src/non_matching/ovl_7e3e08/200b610.c, where `m = 0xc; m &= u;` took a
 * function from 8 of 25 to 3.  The distinction is literal-versus-register, and
 * this function is the register case.
 *
 * The other consequence is the pointer: because our two tests are shorter, gcc
 * never needs the ROM's `mov r6, r1` and allocates r6 from the start.  Naming
 * a second pointer local for the loop (`q = p`) does not reintroduce it.
 */
extern unsigned short iwram_3001f64;
extern volatile unsigned int REG_SIOCNT;
extern void WaitFrames(int n);

int Func_8006384(int mask)
{
    unsigned short *p;
    unsigned short *q;
    int m;
    int v;
    int t;
    unsigned int x;

    p = &iwram_3001f64;
    m = mask;
    v = *p;
    t = m;
    t &= v;
    if (t != m) {
        q = p;
        do {
            WaitFrames(1);
            v = *q;
            t = m;
            t &= v;
        } while (t != m);
    }
    x = REG_SIOCNT;
    return (x << 26) >> 30;
}

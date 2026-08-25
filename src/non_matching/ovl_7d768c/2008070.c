/* OvlFunc_952_2008070  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7d768c/ovl_30_c_a_a_a_a.s
 * Best screen: 7 instructions in disagreeing regions, of 32 (rom 32, ours 31).
 *
 * BLOCKER CLASS: pool tell -- the function is blocked on NAMING A CONSTANT, not
 * on any code shape.
 *
 *      rom   ldr r3, =0x8b / cmp r2, r3
 *      ours  cmp r3, #0x8b
 *
 * 0x8b fits in `cmp #imm8` and gcc uses it. The ROM spends a literal-pool word
 * and a register instead, which it would only do if the operand were a SYMBOL
 * whose value happens to be small. Until that symbol has a name and an extern
 * declaration, no spelling of this comparison can produce the ROM's two
 * instructions.
 *
 * Everything else in the function matches: the four-way selector, the three
 * `.L` table returns, `0x95 << 4` for the first flag id and the pooled 0x962
 * for the second. The remaining count is this one comparison plus the register
 * renaming it drags behind it (r1/r2 swap through the ldrsh).
 *
 * SIBLING, same blocker, same shape: OvlFunc_963_200808c in
 * asm/overlays/rom_7ec968/ovl_30_c_c_a_a_a.s compares against a pooled 0xaa and
 * 0xa9. Naming those two constants would very likely land both functions.
 *
 * This is not a rare situation -- see the HANDOFF.md entry measuring it across
 * the corpus. It is a maintainer's call, since inventing a name is exactly what
 * this effort has deliberately deferred.
 */
typedef struct { unsigned char _b[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char L4b3c[] __asm__(".L4b3c");
extern unsigned char L4e6c[] __asm__(".L4e6c");
extern unsigned char L4d64[] __asm__(".L4d64");
extern unsigned char L4b84[] __asm__(".L4b84");
extern int __GetFlag(int id);

void *OvlFunc_952_2008070(void)
{
    unsigned char *g;
    unsigned int k;
    unsigned int o;
    int v;

    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    o = 0;
    v = *(short *)(g + o);
    if (v == 0x8b)
        return L4b3c;
    if (__GetFlag(0x95 << 4) != 0)
        return L4e6c;
    if (__GetFlag(0x962) != 0)
        return L4d64;
    return L4b84;
}

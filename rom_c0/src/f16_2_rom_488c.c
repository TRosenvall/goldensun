/* Func_488c -- GetIwramFree
 *
 * Returns how many bytes are left in the IWRAM arena: the top of the arena,
 * iwram_7800, minus the bump pointer the allocator keeps at [iwram_1e50 + 4].
 * See docs/runtime.md for the two-arena allocator.
 *
 * STATUS: MATCHING.  Verify with
 *     tools/asmdiff.py Func_488c rom_c0/src/f16_2_rom_488c.c \
 *         --rom-offset 0x488c --rom-size 0x14
 *
 * Both operands are link-time symbols, so the whole body is two literal loads,
 * a load through one of them and a subtract.  The `register ... asm("r3")` pin
 * is a matching aid.  The original reuses r3 for BOTH the table address and the
 * word loaded out of it, so `v` is assigned twice through it; written as one
 * expression agbcc keeps the address in r3 but lands the loaded value in r1.
 *
 * NOTE that asmdiff reports this function as matching either way -- the
 * differing instructions are relocation sites, which it masks, so the register
 * choice inside them is invisible to it.  A full `make compare` is the only
 * check that catches it.
 */

typedef unsigned int u32;

extern u32 iwram_1e50[];
extern u32 iwram_7800;

int Func_488c(void)
{
    register int v asm("r3");
    register int top asm("r0");

    v = (int)iwram_1e50;
    top = (int)&iwram_7800;
    v = (int)((u32 *)v)[1];

    return top - v;
}

/* Cluster OvlFunc_908_20084c8..OvlFunc_908_20084c8 extracted from goldensun/asm/overlays/rom_79c0c4/ovl_30_c_c_c_c.s.
 *
 * Total .text for this TU = 52 bytes (= 0x34).
 * The .data section stays behind in the original .s, which keeps its name and
 * its .data line in goldensun/overlays/rom_79c0c4/overlay.ld; only the .text
 * line is repointed here.
 *
 * Slot 0: map-load entry. Sets the scene step delay at [iwram_3001ebc]+0x1c0
 * to 0x209, then fixes up slot 0x1b's presentation.
 *
 * TWO THINGS MAKE THIS MATCH, and the second is the whole reason it took a
 * second attempt:
 *
 *   - The offset chain is ONE variable, not two.  0x1c0 (as `0xe0 << 1`, which
 *     is how the ROM builds it) becomes 0x209 by `k = k + 0x49`, so the same
 *     register carries both, and likewise `z` is 0 for the strb and then
 *     `z - 0xd` for the mask -- the ROM's `sub r3, #0xd` on a register still
 *     holding zero.
 *   - `k` IS ASSIGNED BEFORE `p`.  Written the other way round, gcc loads the
 *     global first and the two values land in swapped registers, which costs
 *     six instructions for no other reason.  Declaration order does not do
 *     this; statement order does.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char *__MapActor_GetActor(int slot);

int OvlFunc_908_20084c8(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *a;
    unsigned char *b;
    unsigned char *c;
    unsigned int k;
    int z;
    int t;
    int m;

    k = 0xe0 << 1;
    p = iwram_3001ebc;
    q = p + k;
    k = k + 0x49;
    *(int *)q = k;
    a = __MapActor_GetActor(0x1b);
    b = a + 0x23;
    z = 0;
    *b = z;
    c = *(unsigned char **)(a + 0x50);
    t = c[9];
    z = z - 0xd;
    z = z & t;
    m = 8;
    z = z | m;
    c[9] = z;
    return 0;
}

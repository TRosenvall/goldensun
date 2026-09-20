/* Cluster Func_809c314..Func_809c314 extracted from goldensun/asm/rom_8a000/rom_9bb64_c.s.
 *
 * Total .text for this TU = 144 bytes (= 0x90).
 * Preserves the original ROM layout when slotted between
 * asm/rom_8a000/rom_9bb64_c_a_a.o and asm/rom_8a000/rom_9bb64_c_b.o in
 * goldensun/stage1.ld.
 *
 * Never attempted before batch 273. No pins, no flags.
 *
 * Marks every field actor in slots 8..0x41 as inside or outside a box around the
 * player, writing 1 or 0 to its +0x54 byte.
 *
 * THE 36-INSTRUCTION LOOP MATCHED ON THE FIRST CANDIDATE, straight off the twin
 * Func_808b8e8 in src/rom_8a000/rom_8b674_c_a_a.c -- `g = gState` to block the pool
 * fold, `iwram_3001ebc` as `unsigned int`, the four-way `||` bound chain, and the
 * indexed `actor[0x54]`. Eighth or ninth round the file-mate heuristic has paid.
 *
 * THE REMAINING 18 WERE THE PREHEADER AND THEY WERE ONE REGISTER. The ROM keeps the
 * player pointer in r0 so that r2 stays free as reload's scratch for the three
 * high-register constants; ours put it in r2 and pushed those scratches to r1.
 * REUSING THE LOOP'S OWN `actor` VARIABLE for the player pointer -- which already
 * owned r0 -- fixed all eighteen at once.
 *
 * MEASURED AND INERT, all exactly 18: naming the id, declaring MapActor_GetActor
 * `void`, assigning its result to a separate `player` local, and moving that
 * declaration. THE VARIABLE IDENTITY IS THE LEVER, not the declaration -- which is
 * the same shape as "two results of the same call need two pointer variables" run in
 * reverse: here two uses of one register need ONE variable.
 */
extern unsigned char gState[];
extern unsigned int iwram_3001ebc;
extern unsigned char *MapActor_GetActor(int slot);
extern unsigned char *GetFieldActor(int id);

void Func_809c314(void)
{
    unsigned char *g;
    unsigned int base;
    unsigned char *actor;
    int px, pz;
    int xlo, xhi, zlo, zhi;
    unsigned int i;
    int x, z;

    g = gState;
    MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    base = iwram_3001ebc;
    actor = *(unsigned char **)(base + (0xf0 << 1));
    px = *(int *)(actor + 8);
    xlo = px - 0x1400000;
    xhi = px + 0x1400000;
    pz = *(int *)(actor + 0x10);
    zlo = pz - 0x2580000;
    zhi = pz + 0x1900000;
    i = 8;
    do {
        actor = GetFieldActor(i);
        if (actor != 0) {
            x = *(int *)(actor + 8);
            z = *(int *)(actor + 0x10);
            if (x < xlo || x > xhi || z < zlo || z > zhi)
                actor[0x54] = 0;
            else
                actor[0x54] = 1;
        }
        i++;
    } while (i <= 0x41);
}

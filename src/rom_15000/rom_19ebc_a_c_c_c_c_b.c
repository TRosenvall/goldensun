/* Cluster DecompressStatusIcon..DecompressStatusIcon extracted from
 * goldensun/asm/rom_15000/rom_19ebc_a_c_c_c_c.s.
 *
 * Total .text for this TU = 46 bytes (= 0x2e).
 * Placed in the run in goldensun/stage1.ld.
 *
 * Points the icon block at one of the compressed status icons and asks
 * LoadIcon to unpack it, with both dimension halfwords set to 2 tiles.
 *
 * THREE OFFSETS, THREE DIFFERENT TREATMENTS, all of them gcc's own choice once
 * the fields are struct members: 0x604 comes from the literal pool, 0x600 is
 * synthesised as `mov r0, #0xc0 / lsl r0, #3`, and 0x602 is DERIVED from it
 * with `add r0, #2`. Writing any of them as arithmetic forces a single chain
 * and loses two of the three. Same lever as Func_80c90e4 and Func_801c46c.
 */

typedef struct {
    unsigned char pad[0x600];
    short f600;
    short f602;
    int f604;
} Blk;

extern Blk *iwram_3001e94;
extern int L308a0[] __asm__(".L308a0");
extern void LoadIcon(Blk *b, int n);

void DecompressStatusIcon(int i)
{
    Blk *b;

    b = iwram_3001e94;
    b->f604 = L308a0[i];
    b->f600 = 2;
    b->f602 = 2;
    LoadIcon(b, 0);
}

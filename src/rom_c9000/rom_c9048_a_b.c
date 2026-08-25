/* Cluster Func_80c90e4..Func_80c90e4 extracted from goldensun/asm/rom_c9000/rom_c9048_a.s.
 *
 * Total .text for this TU = 60 bytes (= 0x3c).
 * Placed in the run in goldensun/stage1.ld.
 *
 * A frame counter in the effects block: bumps the count at +0x7790 and, when it
 * reaches the limit at +0x7794, adds the two deltas at +0x7798 and +0x779c into
 * a pair of halfwords in iwram_3001ad0 and resets the count.
 *
 * FOUR OFFSETS, AND THE ROM DERIVES EXACTLY ONE OF THEM. `add r1, #0x4` gets
 * 0x7794 from the 0x7790 it already has, while 0x7798 and 0x779c are loaded
 * fresh from the pool. Written as struct members that pattern falls out
 * unaided: gcc keeps the base+0x7790 address live because the field is read
 * AND written, derives the neighbouring compare from it, and materialises the
 * other two independently because nothing is holding them.
 *
 * That is worth knowing next to Func_80173ac, where struct members STOPPED a
 * derivation chain. The lever is not "members prevent derivation" -- it is that
 * members let gcc decide per access, and gcc's decision matched the ROM's in
 * both cases. Writing the offsets as arithmetic is what forces a single chain.
 *
 * NOTE FOR RESCREENING: tryc.py warns that the reference keeps its pool inside
 * the function. Cleared by `make compare`, not by the screen.
 */

typedef struct {
    unsigned char pad[0x7790];
    int f7790;
    int f7794;
    int f7798;
    int f779c;
} Blk;

typedef struct {
    unsigned char pad[4];
    unsigned short h4;
    unsigned short h6;
} Q;

extern Blk *iwram_3001eec;
extern Q iwram_3001ad0;

void Func_80c90e4(void)
{
    Blk *b;
    int c;

    b = iwram_3001eec;
    c = b->f7790 + 1;
    b->f7790 = c;
    if (c == b->f7794) {
        iwram_3001ad0.h4 += b->f7798;
        iwram_3001ad0.h6 += b->f779c;
        b->f7790 = 0;
    }
}

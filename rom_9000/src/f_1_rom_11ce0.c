/* Func_11ce0 -- TileHeight_Flat (shape 0)
 *
 * One of the terrain height handlers reached from the shape dispatch table at
 * the end of rom_11ce0.s.  A flat tile ignores the position entirely: it takes
 * the signed byte at the head of the tile record and scales it into the 16.16
 * world height by shifting left 19, which is one height unit per 8 subtiles.
 *
 * Func_11f3c and Func_11f48 (shapes 14 and 15) are byte-for-byte identical to
 * this one and match the same C; they are still in rom_11ce0.s because they sit
 * mid-file, above the rodata jump table that names them.
 *
 * STATUS: MATCHING.  Verify with
 *     tools/asmdiff.py Func_11ce0 rom_9000/src/f_1_rom_11ce0.c \
 *         --rom-offset 0x11ce0 --rom-size 0xc
 *
 * No register pins needed -- the sign-extending load, the shift and the
 * interworking return all fall out of the obvious C.
 */

typedef signed char s8;

int Func_11ce0(s8 *tile)
{
    return *tile << 19;
}

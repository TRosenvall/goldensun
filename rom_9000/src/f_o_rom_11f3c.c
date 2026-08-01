/* Func_11f3c -- TileHeight_Flat2 (shape 14)
 *
 * Constant height from corner byte 0. Byte-for-byte the same behaviour as
 * Func_11ce0, kept as a separate table entry.
 *
 * STATUS: MATCHING.
 */
typedef signed char s8;
int Func_11f3c(s8 *p) { return *p << 19; }

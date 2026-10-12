/* Func_80a5fe0 -- 0x080a5fe0, asm/rom_a1000/rom_a5534_c_a_a.s
 *
 * BYTE-IDENTICAL.  objcmp: 76 bytes, 34 encodings and 3 relocations identical;
 * 30 instructions against the ROM's 30.  Pin-free (shimcount: 0).
 *
 * Verify with:
 *     python3 tools/objcmp.py src/rom_a1000/rom_a5534_c_a_a_b.c \
 *         asm/rom_a1000/rom_a5534_c_a_a_b.s --func Func_80a5fe0
 *   (before the split, against the parent:  --func Func_80a5fe0 on
 *    asm/rom_a1000/rom_a5534_c_a_a.s)
 *
 * WHAT CLOSED IT, from the park's own 7-of-34 body (batch 338, brief E):
 *
 *     parked   v = rec[0] ^ 2;  return 1 - (v != 0);        7 of 34, 30/30
 *     lands    v = rec[0];      return 1 - ((v ^ 2) != 0);  0 of 34
 *
 * Move the xor OUT of the named local and INTO the return expression, so the
 * local holds the LOAD.  Thumb has no three-operand `eor`, so the destination
 * is tied to operand 1; whichever pseudo the SOURCE names as the destination is
 * the register the result lands in, and the whole neg/orr/lsr tail is allocated
 * around that choice.  Naming the load puts the loaded byte in the eor's
 * destination (the ROM's `eor r3, r2` with the VALUE in r3) instead of the
 * constant, which frees r0 for the branchless chain.
 *
 * The park had measured `v = rec[0]; v ^= 2;` WORSE (8 differing) and concluded
 * the load could not be the thing in the local.  `v ^= 2` still makes `v` the
 * xor's destination -- it moves the statement boundary, not the destination.
 *
 * RECORDED INERT, all at 7 of 34 (one container, 14 variants): `return v == 0`,
 * `return !v`, `return 1 - !!v`, `return (v != 0) ^ 1`, `return 1 - (int)(v != 0)`,
 * `return 1 - (((unsigned)(-v | v)) >> 31)`, `v = *(unsigned char *)rec ^ 2`.
 * gcc normalises every spelling of "is it zero" to one RTL, so the
 * EXPRESSION-SHAPE axis is closed here; only the named-destination axis moved it.
 * `return rec[0] == 2;` and `return 1 - (rec[0] != 2);` both branch (16, size -4).
 */
extern unsigned char *iwram_3001f2c;
extern unsigned char *_GetMoveInfo(int id);
extern int _Func_808e96c(int a);

int Func_80a5fe0(void)
{
    unsigned char *rec;
    unsigned int p;
    int id;
    int v;

    p = (unsigned int)iwram_3001f2c;
    p += 0xbc << 1;
    id = 0x3fff & *(unsigned short *)p;
    rec = _GetMoveInfo(id);
    if (_Func_808e96c(rec[0xc]) != 0)
        return 0;
    if (rec[8] == 0xff)
        return 2;
    v = rec[0];
    return 1 - ((v ^ 2) != 0);
}

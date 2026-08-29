/* Func_80a7440  --  0x080a7440, asm/rom_a1000/rom_a7380.s
 *
 * BLOCKER CLASS: gcc is two instructions AHEAD.
 * Status: 21 lines against the ROM's 23.
 *
 * WHAT IT DOES
 * Clears a halfword at +0x174, asks Func_80a77a4 for a slot, and returns either
 * that answer or, when it is not -1, the byte at +0x21a.
 *
 * WHERE THE TWO INSTRUCTIONS GO. The ROM keeps the result in r2 and moves it to
 * r0 at the return (`mov r2, r0` after the call, `mov r0, r2` at the end); gcc
 * leaves it in r0 throughout and needs neither move. Writing the result and the
 * return value as two separate locals -- `v = r; if (r != -1) v = ...;` -- is
 * exactly what the ROM's register use says, and gcc coalesces them anyway
 * because r0 is free.
 *
 * TWO THINGS THAT DID WORK and are kept below:
 *   the zero stored to +0x174 is a named local, which puts `mov r2, #0` before
 *   the offset is built rather than after;
 *   `-1` needs no help -- gcc emits `mov r3, #1 / neg r3, r3` for a comparison
 *   against it, which is the ROM's form.
 */

typedef struct {
    unsigned char pad[0x174];
    short f174;
    unsigned char pad176[0xa4];
    unsigned char f21a;
} Blk;

extern Blk *iwram_3001f2c;
extern int Func_80a77a4(int n);

int Func_80a7440(void)
{
    Blk *p;
    int r;
    int v;
    int z;

    p = iwram_3001f2c;
    z = 0;
    p->f174 = z;
    r = Func_80a77a4(0);
    v = r;
    if (r != -1)
        v = p->f21a;
    return v;
}

/* OvlFunc_922_2009fac  --  0x02009fac
 *
 * The whole of goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_c_c_c_c_c_c_a.s,
 * which held this function and nothing else, so the linker script's existing
 * line for that object now picks up this file's.
 *
 * A one-shot cutscene trigger, skipped once the story counter at gState+0x1c2
 * has passed 6: set three script flags, start a sound and a fade, wait.
 *
 * THE SECOND BASE POINTER IS DERIVED FROM THE FIRST, and that is the reading
 * this function turns on. The ROM has ONE pool entry for two symbols:
 *
 *      ldr r3, =iwram_3001f30 / ldr r2, [r3] / sub r3, #0x64 / ldr r1, [r3]
 *
 * 0x3001f30 - 0x64 is 0x3001ecc, and wram.sym defines both names. Naming both
 * externs gives two pool loads, because gcc has no way to know two unrelated
 * externs are 0x64 apart. Writing the second as an offset from the FIRST --
 *
 *      q = *(char **)((char *)&iwram_3001f30 - 0x64);
 *
 * -- gives gcc the relationship and it emits the `sub`. That is ugly C, and it
 * is almost certainly not what Camelot wrote; the honest reading is that the
 * original had one object spanning both addresses. But it is the spelling that
 * reproduces the bytes, and the two words at 0x3001ecc and 0x3001f30 do look
 * like neighbouring members of one block.
 *
 * THE 1 AND THE 0 ARE NAMED LOCALS. Four byte stores use them (one 1 at
 * [p+0x34], then 0, 1, 1 into q), the ROM keeps the 1 in r0 and the 0 in r4
 * across all four, and with literals gcc schedules `mov r0, #1` after the
 * pointer arithmetic instead of before it. Ten differing positions to zero.
 *
 * The three q offsets are 0x53e, 0x53c, 0x53d in that order, and the ROM
 * derives the second from the first (`sub r2, #2`) while loading the third
 * fresh from the pool. That is gcc's own arithmetic on nearby constants, not
 * anything the source says.
 */
extern unsigned char gState[];
extern char *iwram_3001f30;
extern void __Func_8091220(int a, int b);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __WaitFrames(int n);

void OvlFunc_922_2009fac(void)
{
    unsigned char *g;
    char *p;
    char *q;
    int one;
    int zero;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) <= 6) {
        one = 1;
        zero = 0;
        p = iwram_3001f30;
        q = *(char **)((char *)&iwram_3001f30 - 0x64);
        p[0x34] = one;
        q[0x53e] = zero;
        q[0x53c] = one;
        q[0x53d] = one;
        __Func_8091220(0, 1);
        __Func_8091200(0x203108, 1);
        __Func_8091254(0x10);
        __WaitFrames(0x10);
    }
}

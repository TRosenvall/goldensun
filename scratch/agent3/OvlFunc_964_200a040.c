/* OvlFunc_964_200a040 -- NOT MATCHING. 31 differing of 43; ours is 40 lines
 * against the ROM's 43 -- SHORTER, which is the signature of gcc rewriting.
 * ref: asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a.s
 *
 * SCREEN WITH --cflags "-O2" (see the note in OvlFunc_964_2009fdc.c: tryc.py
 * matches an -O1 wildcard rule that belongs to a neighbouring TU).
 *
 * BLOCKER: gcc DERIVES -1 from the live 0x31 with `sub r5, #0x32` and then
 * reuses r5 for all four -1 arguments, where the ROM materialises `mov #1 /
 * neg` freshly at each of the four sites:
 *     rom   mov r1,#1 / mov r2,#1 / mov r0,#0x64 / neg r1,r1 / neg r2,r2
 *     ours  mov r1, r5 / mov r2, r5 / mov r0, #0x64
 * That is three instructions saved, hence the short stream.  The first
 * __Func_8010704 call and its stack pair are exact.
 *
 * MEASURED, all 31 of 43 / 40 lines: 0x31 as a local and as bare literals;
 * `int m = 1, n = 1; f(0x64, -m, -n)` per call; `int m, n; m = -1; n = -1;`
 * per call; -fno-gcse, -fno-rerun-cse-after-loop, -fno-expensive-optimizations,
 * -fno-cse-follow-jumps, -fno-force-mem.  -fno-schedule-insns2 and -O1 are 37.
 * Straight-line: the basic-block lever (one local per site in a dominating
 * block), which is what this shape wants, has nowhere to put the assignment.
 */
extern void __Func_8010704(int, int, int, int, int, int);
extern void __Func_808edac(int, int, int);
extern void __MapActor_SetPos(int, int, int);

void OvlFunc_964_200a040(void)
{
    int s;

    s = 0x31;
    __Func_8010704(8, 0x71, 1, 1, 8, s);
    __Func_8010704(0x31, 0x6b, 1, 1, s, 0x2b);
    __Func_808edac(0x64, -1, -1);
    __Func_808edac(0x65, -1, -1);
    __MapActor_SetPos(0xf, 0, 0);
    __MapActor_SetPos(0x10, 0, 0);
}

/* OvlFunc_964_2009fdc -- NOT MATCHING. 14 differing of 43.
 * ref: asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a.s
 *
 * SCREEN WITH --cflags "-O2".  tryc.py picks up an -O1 WILDCARD rule
 * (src/overlays/rom_7ed0a0/ovl_30_c_c_c_a%.c) that belongs to a neighbouring
 * TU; under -O1 this reads 22 of 43 and the first half is wrong for no reason.
 *
 * Under -O2 the whole first half is EXACT, including both six-argument
 * __Func_8010704 calls and the shared 0x31 in the stack slot (one local, per
 * the batch-49 shared-callee-saved rule).
 *
 * BLOCKER: the two __MapActor_SetPos calls.  0xc6 << 18 appears in both, so gcc
 * hoists `mov r5, #0xc6` across the intervening call; and even with the two
 * made distinct (control: 7 of 43) the ROM's `mov r1 / mov r2 / mov r0 /
 * lsl r1 / lsl r2` interleave does not appear.  Straight-line: no boundary.
 *
 * MEASURED on the control: int and void return types on __MapActor_SetPos and
 * on the preceding __Func_808edac all leave it at 7 (int on __Func_808edac is
 * 13, worse).
 */
extern void __Func_8010704(int, int, int, int, int, int);
extern void __Func_808edac(int, int, int);
extern void __MapActor_SetPos(int, int, int);

void OvlFunc_964_2009fdc(void)
{
    int s;

    s = 0x31;
    __Func_8010704(0x48, 0x31, 1, 1, 8, s);
    __Func_8010704(0x71, 0x2b, 1, 1, s, 0x2b);
    __Func_808edac(0x64, 0, 0);
    __Func_808edac(0x65, 0, 0);
    __MapActor_SetPos(0xf, 0x88 << 16, 0xc6 << 18);
    __MapActor_SetPos(0x10, 0xc6 << 18, 0xae << 18);
}

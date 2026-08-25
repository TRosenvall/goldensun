/* OvlFunc_893_2008054  --  0x02008054, asm/overlays/rom_78dd40/ovl_30_c_c.s
 * (a second byte-identical copy exists; solving one solves both)
 *
 * BLOCKER CLASS: constant-CSE across a call's arguments.
 * Status: 28 lines against the ROM's 30. The first seventeen are exact.
 *
 * WHAT IT DOES
 * Seeds two words in the block at iwram_3001ebc and, if flag 0x814 is set,
 * runs a cutscene shake: one setup call, a three-axis magnitude call, and
 * __StartEarthquake. Returns 0.
 *
 * THE WHOLE DIFFERENCE IS THREE IDENTICAL ARGUMENTS
 *
 *      rom   mov r0,#0x80 / mov r1,#0x80 / mov r2,#0x80
 *            / lsl r0,#9 / lsl r1,#9 / lsl r2,#9
 *      ours  mov r2,#0x80 / lsl r2,#9 / mov r0,r2 / mov r1,r2
 *
 * __Func_8012330 is called with the same value on all three axes. gcc builds it
 * once and copies -- four instructions where the ROM spends six. gcc is
 * strictly ahead, which is the signature of this class.
 *
 * -fno-rerun-cse-after-loop DOES NOT HELP; it is byte-identical. The unification
 * happens before that pass. Nothing in C distinguishes three equal constants
 * from one: writing them as separate literals, as separate named locals, or as
 * `0x10000` rather than `0x80 << 9` all fold to the same RTL.
 *
 * WHAT IS ALREADY RIGHT AND SHOULD NOT BE RE-DERIVED. The ROM builds a chain of
 * derived constants for the two stores --
 *
 *      mov r3, #0xe0 / lsl r3, #1   (0x1c0, the first offset)
 *      add r3, #0x44                (0x204, the first VALUE)
 *      sub r3, #0x3c                (0x1c8, the second offset)
 *
 * -- and plain literals reproduce it exactly. The same constant-derivation
 * peephole that blocks the bitfield cases is doing the work here. Do not try to
 * force the chain with explicit arithmetic; it is already there.
 */

extern char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __Func_8091ff0(int n);
extern void __Func_8012330(int x, int y, int z);
extern void __StartEarthquake(void);

int OvlFunc_893_2008054(void)
{
    char *p;

    p = iwram_3001ebc;
    *(int *)(p + 0x1c0) = 0x204;
    *(int *)(p + 0x1c8) = 0x10;
    if (__GetFlag(0x814) != 0) {
        __Func_8091ff0(0x8d);
        __Func_8012330(0x80 << 9, 0x80 << 9, 0x80 << 9);
        __StartEarthquake();
    }
    return 0;
}

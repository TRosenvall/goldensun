/* OvlFunc_884_2008780  --  NOT MATCHING, 2 differing of 44
 * ref: asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_c_c_a.s
 *
 * Residue (the FIRST __Func_80921c4 only; the second is exact):
 *     rom    mov r1, #0xb0 / mov r2, #0x99 / lsl r2, #1 / mov r0, #0 / lsl r1, #1
 *     ours   mov r1, #0xb0 / mov r2, #0x99 / lsl r2, #1 / lsl r1, #1 / mov r0, #0
 * i.e. the straight-line arg-interleave class: the ROM wedges `mov r0, #0`
 * INTO the mov/lsl pair, and the function has no branch for the basic-block
 * lever to use.  Measured and unchanged at 2: the return type of
 * __Func_80921c4 both ways, no declaration for it, no declaration for the
 * preceding __WaitFrames, both, naming the third argument as its own local,
 * naming the zero as its own local, an explicit (int) cast on the zero, and
 * -fno-rerun-cse-after-loop / -fno-schedule-insns / -fno-peephole /
 * -fno-force-mem / -fno-caller-saves / -fno-strict-aliasing.
 * (-fno-schedule-insns2, -O1 and -fcall-saved-r4 are all much worse.)
 *
 * WHAT GOT IT FROM 14 TO 2 -- a lever this tree does not have yet.
 * The two calls share the argument 0xb0 << 1.  With the value written as one
 * expression, gcc builds it ONCE into r5 and copies (`mov r1, r5`) at both
 * sites; the ROM rebuilds it at each.  That is the constant-CSE shape the docs
 * say needs the basic-block lever, and this function is straight-line.
 * Splitting the constant into TWO STATEMENTS -- `p = 0xb0; p <<= 1;` -- at the
 * LATER site defeats the CSE with no branch involved.  The four-way control:
 *
 *     site1 / site2      differing of 44
 *     folded  folded          14
 *     stmt    folded          14
 *     folded  stmt             2
 *     stmt    stmt             2
 *
 * Only the second site's spelling matters, and TWO SEPARATE LOCALS each
 * assigned `= 0xb0 << 1` is 14 -- the same as literals.  So it is the split
 * into two statements that does it, not the naming.
 */
extern void __PlaySound(int id);
extern int __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void OvlFunc_884_2008714(int n);

void OvlFunc_884_2008780(void)
{
    int p;

    __PlaySound(0xbc);
    __CopyMapTiles(0, 0x3f, 0x33, 8, 2, 2);
    __WaitFrames(0xa);
    __CopyMapTiles(2, 0x3f, 0x33, 8, 2, 2);
    __WaitFrames(0xa);
    p = 0xb0;
    p <<= 1;
    __Func_80921c4(0, p, 0x99 << 1);
    __Func_8092b08(0, 3);
    p = 0xb0;
    p <<= 1;
    __Func_80921c4(0, p, 0x94 << 1);
    OvlFunc_884_2008714(2);
}

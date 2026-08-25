/* OvlFunc_959_2008dcc -- NOT MATCHING. 6 of 38, same length.
 *
 * Source asm: goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c.s
 *
 * Blocker: THE SAME CONSTANT AS TWO ARGUMENTS OF ONE CALL. The ROM builds
 * 0xc0 << 10 twice --
 *
 *     mov r0,#0xc0 / mov r1,#0xc0 / mov r2,#0x80 / lsl r0,#10 / lsl r1,#10 / lsl r2,#9
 *
 * -- three movs then three shifts. gcc materialises it once and copies:
 * `mov r1,#0xc0 / lsl r1,#0xa / mov r0, r1`.
 *
 * This is a NEW SUB-SHAPE of constant-CSE and worth distinguishing. The
 * documented one is a constant used on BOTH SIDES OF A CALL, which
 * -fno-rerun-cse-after-loop fixes and which has a recognition rule (a flag id
 * read in a guard and written in the body). Here the two uses are two ARGUMENTS
 * OF THE SAME CALL, and:
 *
 *   two separate named locals    no change -- copy propagation, exactly as
 *                                docs/elevation.md's "separate variables do not
 *                                defeat a copy" says
 *   -fno-rerun-cse-after-loop    WORSE, 39 lines against 38
 *
 * WHAT GOT IT FROM 10 TO 6, and should not be undone: the -1 is built AFTER the
 * __CheckPartyItem call, not before it. The ROM has
 * `bl __CheckPartyItem / mov r6,#1 / neg r6,r6 / cmp r0,r6`, so the source is
 *
 *     t = __CheckPartyItem(0xea);
 *     n = -1;
 *     if (t == n) return;
 *
 * and `n` is then reused as both arguments of the later __Func_8012330 -- the
 * ROM holds it in r6, a pushed callee-saved register, which is the batch-49
 * tell that one value spans both uses.
 *
 * NEXT: nothing known. The two-identical-arguments shape has no lever.
 */
extern unsigned char *iwram_3001ebc;
extern int __CheckPartyItem(int item);
extern void OvlFunc_959_2008d54(int a);
extern void __PlaySound(int id);
extern void __Func_8012330(int a, int b, int c);
extern void __SetFlag(int id);

void OvlFunc_959_2008dcc(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned int off;
    int n;
    int v;
    int t;

    p = iwram_3001ebc;
    t = __CheckPartyItem(0xea);
    n = -1;
    if (t == n)
        return;
    off = 0xb6;
    off <<= 1;
    q = p + off;
    off = 0;
    v = *(short *)(q + off);
    OvlFunc_959_2008d54(v - 0x28);
    __PlaySound(0x9d);
    __Func_8012330(0xc0 << 10, 0xc0 << 10, 0x80 << 9);
    __Func_8012330(n, n, 0xe666);
    __SetFlag(v + 0x32d);
}

/* OvlFunc_common1_148 -- asm/overlays/common/common1_a_a_a_a_a_c_a.s
 *
 * BLOCKER: ONE pooled constant. 1 of 30, LENGTH EXACT.
 *
 * Reads a word out of gState, compares it against a sign-extended field
 * shifted down 10, and on a flag check writes 0x63 into the module block.
 *
 * THREE LEVERS LANDED, 28 differing at 28 lines down to 1 at 30:
 *
 *   1. A LOCAL POINTER FOR gState. `gState + 0xfa * 2` folds to
 *      `ldr r3, =gState+500`; the ROM loads the base plain and builds the
 *      offset at runtime. `unsigned char *g = gState` restores that.
 *
 *   2. THE SECOND OFFSET DERIVED FROM THE FIRST. The ROM computes 0x1f4 and
 *      then `sub r1, #0x76` to reach 0x17e, reusing the register. Writing
 *      `off = 0xfa * 2; ... off -= 0x76;` reproduces it; building 0x17e
 *      independently costs two lines.
 *
 *   3. NAME THE ADDRESS, NOT THE OFFSET -- and this is the batch-158 lever in
 *      its OTHER direction. Here the ROM folds the offset INTO the base
 *      (`add r3, r1 / ldr r2, [r3, #0]`) where naming the offset gives the
 *      register-offset form (`ldr r1, [r3, r2]`). Assigning the address to a
 *      pointer local produced the ROM's form and fixed the length: 26 -> 1.
 *
 * WHAT REMAINS: `ldr r3, =0x63` where the ROM has `mov r3, #0x63` -- a bare
 * literal into a HALFWORD store, HImode, pooled. The documented fix is to
 * route it through an `int` local, which closed Func_8011b00, Func_80173f4
 * and Func_80c01bc.
 *
 * IT BACKFIRES HERE, AND THAT IS THE FINDING. Four routes, all WORSE than
 * leaving the literal alone:
 *
 *   a new `int w` local                       1 -> 4 differ
 *   reusing the dead `t`                      1 -> 4 differ
 *   reusing the dead `off`                    1 -> 4 differ
 *   reusing the dead `v`                      1 -> 5 differ
 *
 * Reusing a local that is already dead does not help, so it is not about the
 * number of variables -- it is that the value must be LIVE ACROSS the store,
 * and this function has no register to spare at that point. The operand-mode
 * fix costs one live value; where there is headroom it is free, and here it is
 * not.
 *
 * So the rule needs a precondition: route a stored constant through an int
 * local when the function has a spare register at the store, and check the
 * differing count afterwards rather than assuming the fix is free.
 *
 * NOT INSTALLED: a pooled word is four bytes of .text, so this would fail
 * make compare despite being one instruction away.
 * Also tried (this round): short / unsigned short / char local for the 0x63
 * store, on the theory that a HImode local would avoid the narrowing that
 * pushes the constant to the pool.  All three are WORSE than the literal:
 * 4 differing instead of 1, first diff moves earlier (21 vs 24).  The narrow
 * local forces its own truncation sequence.  Constant-mode locals are
 * exhausted here; the remaining 1 instruction is pool-vs-immediate placement.
 */
extern int iwram_3001ebc;
extern unsigned char gState[];
extern int __GetFlag(int id);

void OvlFunc_common1_148(void)
{
    char *p;
    unsigned char *g;
    unsigned char *q;
    int v;
    int t;
    int off;

    p = (char *)iwram_3001ebc;
    g = gState;
    off = 0xfa * 2;
    q = g + off;
    v = *(int *)q;
    if (v == 0)
        return;
    off -= 0x76;
    q = (unsigned char *)(p + off);
    t = (short)*(unsigned short *)q >> 10;
    if (t != v)
        return;
    if (__GetFlag(0x141) == 0)
        return;
    *(unsigned short *)(p + 0xc1 * 2) = 0x63;
}

/* Cluster OvlFunc_941_20091b8..OvlFunc_941_20091b8 extracted from
 * goldensun/asm/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_c_c.s.
 *
 * A dialogue state machine: nine tests over seven predicate calls, a loop that
 * re-enters itself two different ways, and three exits.  Written with explicit
 * gotos because that is the only form that reproduces the ROM's block order --
 * the same conclusion as OvlFunc_927_20099b8.
 *
 * TWO LEVERS, AND THE SECOND IS A RULE WORTH KEEPING.
 *
 * 1. __Func_8092c40 IS CALLED TWICE AND THE TWO SITES WANT OPPOSITE ARGUMENT
 *    ORDERS.  The ROM emits `mov r1, #0 / mov r0, #1` at the first and
 *    `mov r0, #1 / mov r1, #0` at the second.  One declaration cannot do both,
 *    so the first call goes through an `__asm__` alias with a different return
 *    type -- the per-call-site lever from batch 147.  6 differing -> 4.
 *
 * 2. `if (c) goto X; goto Y;` COMPILES TO `b!c Y; b X`, NOT `bc X; b Y`.
 *    gcc expands the conditional goto as `jump-if-false to the next statement`,
 *    and jump threading then folds the following unconditional jump into the
 *    branch -- which INVERTS the sense of what you wrote.  To get the ROM's
 *    `bne loop / b out` from a pair of gotos, write the test the other way
 *    round: `if (!c) goto Y; goto X;`.  Two sites here, four differing between
 *    them, and inverting both closed the function exactly.
 *
 *    This applies only to a conditional goto FOLLOWED BY an unconditional one.
 *    The six single-branch tests in this function -- `if (p()) goto L;` with a
 *    fallthrough after it -- all came out right written the natural way, and
 *    inverting those would break them.
 *
 * The seven predicates return `char`: the ROM tests each with `lsl r0, #24 /
 * cmp r0, #0`, which is what gcc emits for a byte-wide result and not for an
 * int.  The two message bases are plain ints held in r5 and incremented in
 * place (`add r5, #1`), so they do NOT need the message.sym symbol treatment --
 * that is for a base whose neighbours are read off it with `add rD, rS, #K`
 * while the base stays live.
 */
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern int C40I(int a, int b) __asm__("__Func_8092c40");
extern int __Func_8091c7c(int a, int b);
extern char OvlFunc_941_20092ac(void);
extern char OvlFunc_941_20092c4(void);
extern char OvlFunc_941_20092f0(void);
extern char OvlFunc_941_2009320(void);
extern char OvlFunc_941_2009368(void);
extern char OvlFunc_941_2009394(void);
extern char OvlFunc_941_200941c(void);
extern void OvlFunc_941_200931c(void);
extern void OvlFunc_941_200934c(void);
extern void OvlFunc_941_2009448(void);
extern void OvlFunc_941_2009760(void);

void OvlFunc_941_20091b8(void)
{
    int m;
    int f;

    m = 0x2547;
    __MessageID(m);
    __ActorMessage(0xc, 0);
    m += 1;
    __Func_809280c(1, 0, 0);
    __MessageID(m);
    C40I(1, 0);
    __Func_809280c(2, 0, 0);
    __Func_809280c(3, 0, 0);
    __Func_809280c(0xd, 0, 0);
    __Func_809280c(0xc, 0, 0);
top:
    if (OvlFunc_941_20092ac() == 0)
        goto alt;
retry:
    if (OvlFunc_941_2009320() == 0)
        goto out1;
    f = 0;
    if (OvlFunc_941_200941c() != 0)
        goto check;
set1:
    f = 1;
loop:
    OvlFunc_941_200934c();
    if (__Func_8091c7c(0, 0) == 0)
        goto out1;
check:
    if (OvlFunc_941_2009394() != 0)
        goto out2;
    if (f == 0)
        goto out2;
    goto loop;
alt:
    if (OvlFunc_941_20092c4() == 0)
        goto more;
    if (OvlFunc_941_20092f0() != 0)
        goto out2;
    goto set1;
more:
    if (OvlFunc_941_2009368() != 0)
        goto retry;
    m = 0x254b;
    __MessageID(m);
    __ActorMessage(2, 0);
    m += 1;
    __MessageID(m);
    __Func_8092c40(1, 0);
    goto top;
out1:
    OvlFunc_941_2009760();
    return;
out2:
    OvlFunc_941_200931c();
    OvlFunc_941_2009448();
}

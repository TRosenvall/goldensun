/* OvlFunc_959_2008e80  --  0x02008e80
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * One of three identical shrine-offering handlers, found together by
 * tools/prologue_families.py. They differ only in the callee they hand the
 * scene number to and in the flag they set at the end.
 *
 * FOUR CONSTANTS NEED THE BASIC-BLOCK LEVER and the `if` supplies the boundary.
 * Two of them are the SAME value passed as two arguments of one call:
 *
 *     rom    mov r0, #0xc0 / mov r1, #0xc0 / mov r2, #0x80 / lsl r0, #10 / lsl r1, #10
 *     ours   mov r1, #0xc0 / lsl r1, #0xa / mov r2, #0x80 / mov r0, r1
 *
 * gcc builds 0xc0 << 10 once and copies it; the ROM builds it twice. Two
 * separate locals assigned above the `if` -- REBUILT, one per site -- give the
 * ROM's form. The 0xe666 in the next call is the pool-loads-first shape and
 * takes the same treatment.
 *
 * THE -1 IS A PLAIN LITERAL, NOT A NAMED LOCAL, even though the ROM carries it
 * in r6 across three calls. Named and assigned before the `if`, gcc builds it
 * BEFORE __CheckPartyItem where the ROM builds it after -- 22 differing of 38.
 * As a literal in all three places gcc carries it into r6 by itself and
 * materialises it at the comparison, which is where the ROM does. A value the
 * ROM carries does not always want naming; it wants naming only when gcc would
 * otherwise rebuild it.
 */
extern char *iwram_3001ebc;
extern int __CheckPartyItem(int id);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __Func_8012330(int a, int b, int c);
extern void OvlFunc_959_2008e30(int n);

void OvlFunc_959_2008e80(void)
{
    char *p;
    int n;
    int a1;
    int a2;
    int a3;
    int e;

    a1 = 0xc0 << 10;
    a2 = 0xc0 << 10;
    a3 = 0x80 << 9;
    e = 0xe666;
    p = iwram_3001ebc;
    if (__CheckPartyItem(0xea) != -1) {
        n = *(short *)(p + (0xb6 << 1));
        OvlFunc_959_2008e30(n - 0x28);
        __PlaySound(0x9d);
        __Func_8012330(a1, a2, a3);
        __Func_8012330(-1, -1, e);
        __SetFlag(n + (0xcc << 2));
    }
}

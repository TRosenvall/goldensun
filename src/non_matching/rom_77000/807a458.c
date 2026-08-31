/* Func_807a458 -- 0x0807a458 -- asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_c.s
 *
 * BLOCKER: two address forms collapsed into one. 19 of 30, two lines short.
 *
 * Appends a four-byte record to a table: pick a bank by whether the first
 * argument exceeds 7, read the table's count, write four bytes at
 * base + 8 + count*4, and store count + 1 back.
 *
 * The first eleven instructions are exact -- both calls, the parameter saves,
 * and the `mov r0,#0 / cmp / bls / mov r0,#1` bank select, which a plain
 * `(unsigned)a <= 7 ? 0 : 1` produces unaided.
 *
 * WHAT REMAINS is that the ROM uses TWO addressing forms for the four stores
 * and we use one:
 *
 *     rom    mov r2, r3 / add r2, #8 / lsl r3, r1, #2
 *            strb r6, [r2, r3]      <- first store REGISTER-OFFSET
 *            add r2, r3             <- then advance
 *            strb r7, [r2, #1] / strb r5, [r2, #2] / strb r3, [r2, #3]
 *
 *     ours   add r0, #8 / lsl r3, r1, #2 / add r3, r0
 *            strb r6, [r3, #0] / strb r7, [r3, #1] / ... all immediate
 *
 * Ours folds the index in once and uses immediate offsets throughout, which is
 * two instructions cheaper. The source says what the ROM says -- `e[idx] = b;`
 * then `e += idx;` -- and gcc reorders the advance ahead of the first store.
 *
 * MEASURED, both byte-identical to the baseline at 19:
 *   the index named in its own local, assigned after `e`
 *   the index named and assigned BEFORE `e`
 *
 * So the split between register-offset and immediate addressing is not
 * reachable by naming or ordering here. That is the same wall as
 * src/non_matching/rom_f6000/80f7f30.c, parked the same round: gcc picks one
 * addressing form per address expression and reuses it, where the ROM mixes
 * the two. Both functions have the length right and differ only in that
 * choice.
 *
 * The name-the-address lever works when the ROM builds an address ONCE and
 * loads with an immediate (Func_808bc44, OvlFunc_916_2008b3c). It does not
 * reach a function where the ROM uses BOTH forms off the same base, because
 * there is only one C expression and gcc gives it one form.
 */
extern void Func_807a3a8(void);
extern char *Func_8077330(int n);

void Func_807a458(int a, int b, int c)
{
    char *p;
    int *cnt;
    int n;
    char *e;

    Func_807a3a8();
    p = Func_8077330((unsigned int)a <= 7 ? 0 : 1);
    cnt = (int *)(p + 0x84 * 2);
    n = *cnt;
    e = p + 8;
    e[n * 4] = b;
    e += n * 4;
    e[1] = c;
    e[2] = a;
    e[3] = 0xff;
    *cnt = n + 1;
}

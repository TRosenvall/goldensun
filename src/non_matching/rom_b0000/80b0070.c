/*
 * Func_80b0070  (ClassifyShopItem) -- asm/rom_b0000/rom_b0070_a_a_a.s
 *
 * BLOCKER: register choice for a materialised zero. 55 lines against 55, 14
 * differing, first difference at line 8:
 *
 *      rom   mov r1, #0x0 / mov r10, r1
 *      ours  mov r2, #0x0 / mov r10, r2
 *
 * The counter starts in r10, a high register, so Thumb has to build the zero
 * in a low register first and move it. The ROM picks r1 and we pick r2, and
 * everything after that renumbers.
 *
 * PROGRESS RECORD, each step measured:
 *
 *   17 differing  -- first draft (out, then n = 0, then i = 0)
 *   16 differing  -- n = 0 moved ahead of the output-pointer computation
 *   14 differing  -- i = 0 moved ahead of it as well
 *   42 differing  -- i = 0 placed before n = 0    <-- DO NOT
 *
 * That last one is the useful negative. Reordering the two counters relative
 * to EACH OTHER lets gcc merge the two zeros into one register and drops a
 * whole instruction (54 lines against 55). Moving them both ahead of the
 * address computation helps; reordering them against each other does not.
 *
 * SETTLED, and it is the half worth reusing:
 *
 *   The trailing `*(short *)(base + 0x26c + n * 2) = 0` keeps its LITERAL zero.
 *   The ROM emits `ldr r2, .Lb00e0` where `.Lb00e0` is a `.word 0` -- it pools
 *   the zero itself, which is the halfword exception recorded in const.sym
 *   running in the direction where the literal is correct. Naming it in an int,
 *   which is the fix in Func_801eea0 and Func_80175c0, would be the error here.
 *   Check which way the ROM runs before applying that lever.
 *
 *   The signed-char compare is reloaded every iteration -- `ldr r3, =0x3a9 /
 *   add r3, r8 / ldrsb` inside the loop, not hoisted -- because the two calls
 *   may write through the pointer. Assigning it to a local at the top of the
 *   body reproduces that and also fixes the evaluation order of the `&&`,
 *   whose operands are otherwise unsequenced.
 */
extern unsigned char *iwram_3001f2c;
extern int _Func_8078480(int id);
extern int _Func_8078ad0(int id, int b);

int Func_80b0070(void)
{
    unsigned char *base;
    short *out;
    int i;
    int n;
    int c;

    base = iwram_3001f2c;
    n = 0;
    i = 0;
    out = (short *)(base + (0x9b << 2));
    do {
        c = *(signed char *)(base + 0x3a9);
        if (c == _Func_8078480(i) && _Func_8078ad0(i, 0) != 0) {
            *out = i;
            n++;
            out++;
        }
        i++;
    } while (i <= 0x1ff);
    *(short *)(base + (0x9b << 2) + n * 2) = 0;
    base[0x3a6] = n;
    return n;
}

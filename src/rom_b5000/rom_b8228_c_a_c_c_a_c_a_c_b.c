/* Cluster Func_80b90f8..Func_80b90f8 extracted from goldensun/asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c.s.
 *
 * Total .text for this TU = 276 bytes (= 0x114). Never attempted before batch 277.
 * No pins, no flags. First screen was 8 differing and the fix was ONE declaration swap.
 *
 * THE ROM REALLY DOES INITIALISE A LOOP COUNTER FROM A LIVE VARIABLE THAT ALREADY HOLDS
 * ZERO, and this file is one of two instances in its own `.s`. `for (i = ret; i < cnt; i++)`
 * emits the ROM's `ldr r2, [sp, #4] / cmp r2, r8 / bge` -- and it was REQUIRED for the
 * match, not merely tolerated. A `mov rX, #0` there is NOT what gcc emits when the zero is
 * already named and live.
 *
 * That is worth holding beside the recorded `i = 0`-as-a-statement lever: both are about
 * where the counter's initial value comes from, and this is the case where the answer is
 * "from something already in a register". If the ROM's loop entry compares two registers
 * instead of loading a literal zero, look for a live zero to start from.
 *
 * The parked sibling src/non_matching/rom_b5000/80b9554.c supplied the `pop {r1} / bx r1`
 * live-return-value tell, which was load-bearing here and on Func_80b9324 -- and which
 * correctly said `void` for Func_80b9470 (`pop {r0} / bx r0`). Its other claim, that
 * Func_80b9604 and Func_80b9554 must land together with Func_80b9724 as gcc NESTED
 * FUNCTIONS, implied a whole-file job; this function and Func_80b9324 landing as ordinary
 * standalone functions out of the same `.s` mean that nested-function work can now be
 * scoped to just those three.
 */
extern unsigned char *iwram_3001e74[];
extern unsigned char gState[];
extern int Func_80b6b40(int kind, short *buf);
extern unsigned char *_GetUnit(int id);
extern unsigned int Random(void);

int Func_80b90f8(void)
{
    short buf[14];
    unsigned char *base;
    int ret;
    unsigned char *pflag;
    int acc;
    int cnt;
    int sum;
    int i;

    ret = 0;
    base = iwram_3001e74[0];
    if (base[0x45] == 1) {
        ret = 1;
    } else {
        pflag = base + 0x46;
        acc = 0x1388 + *pflag * 2000;
        cnt = Func_80b6b40(1, buf);
        sum = 0;
        for (i = ret; i < cnt; i++)
            sum += _GetUnit(buf[i])[0xf];
        acc += sum * 500 / cnt;
        cnt = Func_80b6b40(2, buf);
        sum = 0;
        for (i = 0; i < cnt; i++)
            sum += _GetUnit(buf[i])[0xf];
        acc -= sum * 500 / cnt;
        if (acc > 0) {
            if (Random() * 0x2710 >> 16 < acc)
                ret = 1;
        }
        (*pflag)++;
    }
    if (gState[0x22b] == 2)
        ret = 0;
    return ret;
}

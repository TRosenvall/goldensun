/* Cluster Func_80bf5a8..Func_80bf5a8 extracted from goldensun/asm/rom_b5000/rom_bbb0c_a_c_c_a_c.s.
 *
 * Total .text for this TU = 180 bytes (= 0xb4). Never attempted before batch 276.
 * No pins, no flags.
 *
 * Ticks every djinni's recovery counter down, then applies the ones that have
 * reached zero. Note that the second loop advances `q` and `i` only in the ELSE
 * arm -- the apply path re-tests the same element, which is what the ROM does and
 * what the callees are presumably expected to make terminate. Written any other
 * way the loop does not match.
 *
 * TWO LEVERS.
 *
 * 1. DISTINCT POINTERS FOR THE TWO LOOPS. One shared `p` is 87 lines against the
 *    ROM's 91 and 86 differing: with a single pointer there is no `sub sp, #4` and
 *    `ret` takes a callee-saved register instead of the ROM's stack slot. Two
 *    pointers is 91/91 and 24 differing. This is the "distinct call results want
 *    distinct variables" rule applied to two loop cursors over the same array.
 *
 * 2. `d = &rec->l;` BEFORE `n = rec->l.count;`, not after. 24 differing to exact,
 *    on statement order alone.
 *
 * AND THE THING WORTH CARRYING IS WHAT THAT SECOND LEVER ALSO FIXED. Of the 24
 * differing, the visible half was `_Func_807a3a8` filling r2 before r1 -- the
 * argument-fill-order signature that normally means a callee's return type is
 * wrong. IT WAS NOT. All four combinations of `void`/`int` on the two callees
 * measured 24 differing with IDENTICAL pairs, and the arg order corrected itself
 * when the r1/r2 ALLOCATION was fixed by the statement reorder above.
 *
 * So: AN ARGUMENT-FILL-ORDER DIFFERENCE CAN BE A CONSEQUENCE RATHER THAN A CAUSE.
 * The return-type lever is real and is now on file for five functions, but diff the
 * PAIRS before spending the round on it -- if the same two registers are also
 * swapped elsewhere in the function, the allocation is the cause and the call is
 * just where you noticed.
 */
typedef unsigned char u8;

struct Djinni {
    u8 f0;
    u8 f1;
    u8 f2;
    signed char f3;
};

struct List {
    struct Djinni e[0x40];
    int count;
};

struct Rec {
    u8 pad0[8];
    struct List l;
};

extern struct Rec *_Func_8077330(int a);
extern u8 *_GetUnit(int id);
extern void _SetDjinni(int a, int b, int c);
extern void _Func_807a3a8(int a, int b, int c);
extern void _CalcStats(int id);

int Func_80bf5a8(void)
{
    struct Rec *rec;
    struct List *d;
    struct Djinni *p;
    struct Djinni *q;
    u8 *u;
    int i;
    int ret;
    int n;
    int id;

    rec = _Func_8077330(0);
    d = &rec->l;
    n = rec->l.count;
    ret = 0;
    i = 0;
    if (i < n) {
        p = d->e;
        do {
            if (p->f3 > 0) {
                u = _GetUnit(p->f2);
                if (*(short *)(u + 0x38) != 0)
                    p->f3 = p->f3 - 1;
            }
            i++;
            p++;
        } while (i < d->count);
    }
    i = 0;
    if (i < d->count) {
        q = d->e;
        do {
            if (q->f3 == 0) {
                id = q->f2;
                _SetDjinni(id, q->f0, q->f1);
                _Func_807a3a8(id, q->f0, q->f1);
                _CalcStats(id);
                ret = 1;
            } else {
                q++;
                i++;
            }
        } while (i < d->count);
    }
    return ret;
}

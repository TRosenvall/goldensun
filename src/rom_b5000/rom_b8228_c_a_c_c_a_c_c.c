/* Func_80b9934  --  0x080b9934
 *
 * The whole of goldensun/asm/rom_b5000/rom_b8228_c_a_c_c_a_c_c.s: one function,
 * no data. From the annotation on the original .s: clears the 20-entry array at
 * [iwram_3001e74] + 0x2ec, refreshes the summaries, seeds the state array and
 * sets save bit 0x16b, then runs the turn collection and clears the bit again.
 *
 * THE [cse] PREDICTION FAILED AGAIN -- five wrong out of six -- AND THIS ONE IS
 * A COUNTEREXAMPLE TO A RECORDED ROW, not just an inert flag. 0x16b is a pooled
 * constant used twice, at the top and at the shared exit, with the first use
 * DOMINATING the second and branches in between. The notebook's pool-constant
 * table says that shape is hoisted at default flags. gcc reloaded it at both
 * sites anyway, and -fno-rerun-cse-after-loop is byte-inert. The likely reason,
 * and it is inferred rather than proved: about ten calls separate the two uses,
 * so hoisting would cost a callee-saved register across all of them.
 *
 * REUSING ONE LOCAL FOR TWO ROLES WAS WORTH 43 TO 16, AND THE SYMPTOM WAS A
 * REGISTER ROTATION. One `int` served as both a call result and, later, a loop
 * counter. Splitting it into two variables -- same instructions, same values --
 * moved three unrelated pointers into the ROM's registers in one step. Read out
 * of the .18.greg dispositions: with the roles joined, the loop counter is
 * allocated BEFORE the result, which is what pushes the result down a register.
 * Third confirmation of the read-count rule's third face, with an addition
 * worth carrying: A TWO-ROLE LOCAL MIS-PRICES THE ALLOCNO PRIORITIES, so the
 * symptom is a rotation rather than a missing instruction.
 *
 * A STORE THROUGH A CALL RESULT NEEDS BOTH NAMES, NOT EITHER ONE. Written as
 * `_GetUnit(x)[0x12b] = expr;`, the expander produces the destination address
 * before the value -- expand_assignment evaluates the target first, and for an
 * out-of-range offset memory_address materialises the add right there -- so the
 * pool load and add land immediately after the call, ahead of the value. Naming
 * only the pointer is inert; naming only the value is WORSE; naming both, as
 * three statements in the ROM's order, is 15 to 4. Survives --no-sched2, so it
 * is the expander and not the scheduler.
 *
 * NAMING THE TWO HALFWORD CONSTANTS DID NOT COST A CALLEE-SAVED REGISTER here,
 * because the loop contains no call -- the recorded warning that a named local
 * costs one is a property of loops that CONTAIN CALLS, not of the spelling.
 * What the naming bought instead was a conflict count: with the two locals and
 * the induction variable live before the offset pseudo dies, r3/r2/r1/r0 are
 * all taken and the offset lands in r4, which is what the ROM has. A CONSTANT
 * IN r4 IN A CALL-FREE PROLOGUE IS A CONFLICT-COUNT READOUT -- count the values
 * that must be live there, and add named locals until there are four.
 *
 * The last two instructions were pure statement order. Six permutations of the
 * four pre-loop assignments measured 4, 6, 7, 8, 14, 17 and exact; only one
 * order works. Post-reload scheduling breaks ties on original RTL order, so
 * when the residue is an adjacent pair of independent constant materialisations,
 * PERMUTE THE SOURCE STATEMENTS EXHAUSTIVELY -- the space is small and it is
 * cheaper than reasoning about it.
 *
 * Two smaller readings. The out-of-line zero arm says the `if` must be written
 * with the other side as its body, worth 72 to 43. And the `!= 3` expression
 * reproduces the eor/neg/orr/lsr/add idiom AS A PLAIN EXPRESSION -- no
 * statement-level branch was needed, which narrows the recorded rule about that
 * idiom to the MASKED case only.
 */
typedef struct Slot {
    short f0;
    short f2;
    short f4;
    short f6;
    short f8;
    short fa;
    short fc;
    short fe;
} Slot;

typedef struct Ent {
    short f0;
    short f2;
    short f4;
    short f6;
    short f8;
    short fa;
    short fc;
    short fe;
} Ent;

extern unsigned char *iwram_3001e74;
extern int *iwram_3001f00;

extern void Func_80b90ac(void);
extern void Func_80b98b4(int a);
extern void _SetFlag(int f);
extern void _ClearFlag(int f);
extern void Func_80b8fd4(int a);
extern void _Func_80174d8(void);
extern int Func_80b920c(Ent *p);
extern int Func_80b90f8(void);
extern int Func_80b9724(Ent *p, int n);
extern int Func_80b60a0(void);
extern int Func_80b9324(Ent *p);
extern void Func_80b9470(Ent *p, int n);
extern unsigned char *_GetUnit(int id);
extern void Func_80b7f9c(void);

int Func_80b9934(Ent *p)
{
    unsigned char *b;
    Slot *s;
    unsigned int i;
    int k;
    int n;
    int c;
    Ent *e;
    int v;
    int w;
    unsigned char *u;
    int c1;
    int c2;

    b = iwram_3001e74;
    c1 = 0xff;
    i = 0;
    c2 = 0x8000;
    s = (Slot *)(b + 0x2ec);
    do {
        i++;
        s->f0 = c1;
        s->f4 = c2;
        s++;
    } while (i <= 0x13);
    Func_80b90ac();
    Func_80b98b4(8);
    _SetFlag(0x16b);
    Func_80b8fd4(0);
    _Func_80174d8();
    if (b[0x45] != 2) {
        k = Func_80b920c(p);
        if (k < 0)
            goto out;
        if (k != 0 && p->f6 == 0x63 && Func_80b90f8() == 0)
            b[0x45] = 2;
    } else {
        k = 0;
    }
    if (iwram_3001e74[0x44] != 0) {
        n = Func_80b9724(p, k);
        if (Func_80b60a0() < 0) {
            k = -1;
            goto out;
        }
        k += n;
        if (n < 0) {
            k = -1;
            goto out;
        }
    } else {
        k += Func_80b9324(p + k);
    }
    Func_80b9470(p, k);
    if (k > 0) {
        e = p;
        c = k;
        do {
            v = e->f6;
            if (v == 3 || v == 7) {
                u = _GetUnit(e->f0);
                w = (e->f6 != 3) + 1;
                u[0x12b] = w;
            }
            c--;
            e++;
        } while (c != 0);
    }
out:
    _ClearFlag(0x16b);
    Func_80b7f9c();
    *iwram_3001f00 = 0x2000;
    return k;
}

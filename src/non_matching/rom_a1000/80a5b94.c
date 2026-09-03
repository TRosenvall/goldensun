/* Func_80a5b94  --  0x080a5b94
 * asm/rom_a1000/rom_a5534_c_a_a.s, line 534 (second of six functions).
 *
 * PARKED at 32 aligned of 118. Every one of the 32 traces to ONE allocator
 * decision, and the mechanism is confirmed against the compiler source rather
 * than inferred.
 *
 * BLOCKER CLASS: pool-constant CSE, in its NO-BOUNDARY form -- and this is a
 * concrete 118-instruction instance of the class the notebook calls its single
 * most valuable open question. It is a far better probe target than the
 * 250-instruction script band where the class was first noticed.
 *
 * THE RULE, READ OUT OF local-alloc.c:868-871 (verified in the build image at
 * /opt/camelot-gcc/agbcc/gcc/local-alloc.c):
 *
 *      if (REG_N_REFS (regno) == 2
 *          && REG_BASIC_BLOCK (regno) < 0
 *          && rtx_equal_p (XEXP (note, 0), SET_SRC (set)))
 *        reg_equiv_replace[regno] = 1;
 *
 * Set once, used once, AND referenced in more than one basic block. Both the
 * replace and the "move the initialisation just before the use" arm are gated on
 * that third condition. So UPDATE_EQUIV_REGS NEEDS A BASIC-BLOCK BOUNDARY: a
 * pooled constant's twin is rematerialised only across a branch.
 *
 * That is the whole residue here. `*(unsigned char *)x = 1;` never pools on its
 * own -- probed, gcc emits `mov r3, #1`. The ROM's `ldr r3, =1 / strb` is cse
 * substituting the HImode truncation temp that `*(short *)y = one;` leaves
 * behind, where `one` is an int local stored through a short pointer. For the
 * `1` pair the two sites straddle the `if`, REG_BASIC_BLOCK is -1, the
 * definition is moved to the use, and we reproduce the ROM exactly. For the `0`
 * pair BOTH sites sit in the same tail block, so the definition is NOT moved:
 * the HImode zero stays live across five calls, takes r5, pushes the 0xea6
 * offset to r6, pushes the walked pointer to r8 and the base out to r10 --
 * eleven extra differing regions plus the whole prologue and epilogue.
 *
 * PROVED BY CONSTRUCTION. Probe pD changes exactly one thing: the strh's `zero`
 * local becomes 5, so no zero exists to common. It drops to 21 of 118, with the
 * prologue, the base in r8, the walked pointer and the entire head matching.
 * Every one of the eleven extra regions is downstream of that single hoist.
 *
 * Nothing source-level defeats it. `short` locals at either site, four separate
 * locals, initialiser against assignment, declaration order, and five spellings
 * of the walked pointer all land on exactly 32 -- which by the notebook's own
 * rule means the residue is not in any of them. `-fno-cse-skip-blocks` reaches
 * 23 and not 0, and this tree has no flag group containing it.
 *
 * THREE LEVERS DID FIRE, and the first changes how any register-swap park should
 * start.
 *
 * ALLOCNO PRIORITY IS COMPUTABLE, AND THE COMPILER WILL PRINT THE ANSWER.
 * Compiling with -da and reading `;; N regs to allocate:` from the .18.greg dump
 * gives the allocation order DIRECTLY -- the ROM's r5/r6/r7/r8 assignment is
 * just that list. The .17.lreg dump's `;; Register N in M.` lines give what
 * local-alloc grabbed before global alloc ran. This turns "which register does
 * the ROM want" from guesswork into reading, and should be the first move on
 * every register-swap park in this tree.
 *
 * THE INITIALISER LEVER, QUANTIFIED. `unsigned char **g = base;` against
 * `g = base;` is 37 against 40 here: the initialiser puts the definition at
 * function entry, lengthens live_length, drops the priority below the return
 * value's, and moves the base from r7 to r8, which is what the ROM wants.
 * Ordering the INITIALISERS so the allocating call is declared first keeps the
 * bl ahead of the pool load while retaining most of the effect.
 *
 * THE THREE IWRAM NAMES ARE ONE OBJECT. The ROM keeps a single pool word for
 * the base and reaches the other two with `add r6, #0x24` and `ldr r5,[r3,#0x54]`.
 * cse cannot relate two distinct symbol_refs, so iwram_3001e68, iwram_3001e8c
 * and iwram_3001ebc are one array or struct in the source, indexed from the
 * first. Written as three externs it costs three pool words and 13 regions.
 * tools/object_proposals.tsv corroborates: it records the second name's "panel
 * field group at +0xea8", and this function's store is base[9][0xea6].
 *
 * ~45 spellings and 8 flags measured. Screened with tools/tryc.py --align; the
 * reference keeps its pool INSIDE the body behind a branch, so only make compare
 * could settle a byte result. Not built.
 */
extern unsigned char *iwram_3001e68[];

unsigned char *galloc_iwram(int, int);
void _Func_80170f8(int, int, int, int);
void WaitFrames(int);
void Func_80a1090(int);
int _Func_80796c4(unsigned char *);
void Func_80a3354(int, int, int, int);
int _CreateUIBox(int, int, int, int, int);
void Func_80a2144(int);
void _Func_80219c8(int);
void Func_80a2474(void);
int Func_80a5cc0(int *, int *, int *);
void Func_80a2490(void);
void _GetMoveInfo(int);
void _Func_80164ac(int);
void Func_80a34c0(void);
void gfree(int);
void _ClearUIRegion(int, int, int, int);
void _Func_8091858(void);

int Func_80a5b94(void) {
    unsigned char *p = galloc_iwram(0x37, 0xa70);
    unsigned char **g = iwram_3001e68;
    unsigned char *q;
    unsigned char **s;
    int a, b, c;
    int r;
    int one = 1;
    int zero = 0;

    *(short *)(g[0] + 4) = one;
    _Func_80170f8(0, 0, 0x1e, 0x14);
    WaitFrames(1);
    Func_80a1090(0);
    *(unsigned char *)(p + 0x219) = _Func_80796c4(p + 0x208);
    Func_80a3354(0, 3, 0, 7);
    *(int *)(p + 0x10c) = _CreateUIBox(0xd, 0, 0x11, 3, 2);
    Func_80a2144(0xe);
    _Func_80219c8(0x6002500);
    Func_80a2474();
    r = Func_80a5cc0(&a, &b, &c);
    Func_80a2490();
    if (r == 1) {
        q = g[0x15];
        _GetMoveInfo(*(unsigned short *)(p + 0x178) & 0x3fff);
        *(short *)(q + 0x17e) = c | (a << 10);
    }
    _Func_80164ac(*(int *)(p + 0x24));
    s = g;
    s += 9;
    *(unsigned char *)(*s + 0xea6) = 1;
    Func_80a34c0();
    _Func_80170f8(0, 0, 0x1e, 0x14);
    gfree(0x37);
    *(short *)(g[0] + 4) = zero;
    WaitFrames(1);
    _ClearUIRegion(0, 0, 0x1e, 0x14);
    *(unsigned char *)(*s + 0xea6) = 0;
    _Func_8091858();
    return r;
}

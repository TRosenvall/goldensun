/* rom_79008_a_b.c  --  Func_8079008, cut from asm/rom_77000/rom_79008_a.s.
 *
 * NEEDS A SPLIT.  That .s holds TWO functions -- this one and Func_807905c, the
 * 299-line stat-recompute routine -- so split at Func_8079008 leaves this
 * function in asm/rom_77000/rom_79008_a_b.s and the rest in
 * asm/rom_77000/rom_79008_a_c.s, with stage1.ld:720 rewritten to list both in
 * order.  Nothing needs exporting: the table this function indexes is already
 * `.global .L7a830` in asm/rom_77000/rom_79008_c.s.
 *
 *   OK Func_8079008 -- 84 bytes, 39 encodings and 2 relocations identical.
 *   Pin-free; nothing goes in fakematch.txt.
 *
 * Looks up a per-class growth-table entry: rejects the unit if a byte at +0x129
 * is clear, rejects a level outside 1..0x63, rejects a class above 7, then
 * indexes a table of 99-entry rows by class and level.
 *
 * TWO LEVERS CLOSED IT, AND THE PARK HAD CONCLUDED THERE WAS NOTHING AT THE
 * EXPRESSION LEVEL.
 *
 * 1. THE MISSING INSTRUCTION IS A SECOND READ OF THE SAME BYTE.  The park's
 *    residue was the ROM's `mov r2, r3` ahead of the multiply chain, which it
 *    read correctly -- the ROM copies the class index because the chain's first
 *    shift overwrites the register holding it -- and then filed as allocation
 *    rather than expression shape.  It is expression shape, and the shape is HOW
 *    MANY TIMES THE BYTE IS READ.  Writing the range test and the use as two
 *    separate reads of the same lvalue takes the body from two instructions short
 *    and twenty-nine differing to THE EXACT LENGTH and three differing: cse
 *    commons the two loads, and the value then arrives in the register the chain
 *    needs to clobber, so the copy survives.  Written as one read, gcc puts the
 *    byte somewhere the chain does not need and there is no copy to make.
 *
 *      GENERAL FORM: A CANDIDATE ONE INSTRUCTION SHORT AT A MULTIPLY OR SHIFT
 *      CHAIN, WHERE THE ROM COPIES ITS OPERAND FIRST, IS ASKING FOR A SECOND
 *      READ -- not for a register pin and not for a reassociation.  Checking it
 *      costs one compile.
 *
 * 2. THE TABLE BASE WANTS ITS OWN LOCAL, ASSIGNED BEFORE THE MULTIPLY.  With the
 *    base left inline, its pool load lands one slot late and in the wrong
 *    register, and the indexed load comes out with the operands exchanged -- the
 *    ROM uses the table as the base and the offset as the index, we had it the
 *    other way round.  Naming it is not enough on its own; WHERE THE ASSIGNMENT
 *    SITS DECIDES IT.  Between the class read and the multiply it is exact;
 *    after the level is added it is two differing (right slot, wrong register);
 *    before the level range tests it is fifteen.  Every declaration-order
 *    permutation is inert, so it is the assignment's position in the statement
 *    stream and not the declaration's.
 *
 * WHAT THE PARK HAD ALREADY GOT RIGHT, and both held up: `t = k * 99` is the
 * correct spelling even though the ROM emits the chain by hand -- gcc produces
 * exactly that chain, and writing the chain out is no better -- and the mixed
 * signedness of the three range tests is real, `ble`/`bgt` on the level against
 * `bhi` on the class.
 *
 * ONE MORE PIECE OF THE PARK'S PROSE IS WORTH KEEPING: the byte offset wants to
 * stay a `sub` instruction rather than being folded into the symbol, which is
 * what splitting `off = t << 2; off -= 4;` into two statements buys.  The park
 * measured that and saw no change, because the two real differences were
 * elsewhere; with both levers above in place it is load-bearing.
 */
extern void *GetUnit(void);
extern unsigned char L7a830[] __asm__(".L7a830");

int Func_8079008(int unused, int idx)
{
    unsigned char *u;
    unsigned int k;
    int t;
    unsigned char *tab;
    unsigned int off;

    u = (unsigned char *)GetUnit();
    if (u[0x129] == 0)
        goto neg;
    if (idx <= 0)
        return 0;
    if (idx > 0x63)
        goto neg;
    if (u[0x94 << 1] > 7)
        goto neg;
    k = u[0x94 << 1];
    tab = L7a830;
    t = k * 99;
    t += idx;
    off = t << 2;
    off -= 4;
    return *(int *)(tab + off);
neg:
    return -1;
}

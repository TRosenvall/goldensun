/* OvlFunc_969_200b7c4  --  0x0200b7c4
 *
 * The tail of goldensun/asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_a.s;
 * the three functions ahead of it stay in _a_a.s, and their pools with them.
 * Nothing follows the target, so the split is a clean tail cut, verified
 * byte-neutral before this landed.
 *
 * A per-frame nudge for a pair of actors: if both have reached their rest
 * values, face them forward, run one of three animation choices, and drift them
 * toward a target while two flags allow it.
 *
 * AN ORPHAN COMPARE OVER A LONE LOAD IS A DEAD BOOLEAN ASSIGNMENT, AND YOU
 * TRANSCRIBE IT. The ROM opens with a compare, a branch, and a single load
 * whose destination is redefined on the very next instruction. That is not a
 * compiler curiosity to be explained away -- it is the residue of a complete
 * three-field test whose result is overwritten by the next statement. gcc does
 * not delete the dead non-volatile load, and it collapses the rest of the chain
 * to exactly one compare and one load. Writing the dead if/else out literally
 * reproduced all four instructions first try. Generalise: an isolated
 * compare-and-branch whose only guarded instruction is a load into a register
 * that is immediately redefined means a WHOLE STATEMENT WAS DEAD in the source.
 *
 * BRANCH POLARITY HAS A THIRD FACE -- the polarity of a boolean
 * MATERIALISATION. For one predicate gcc emits three shapes that are not
 * interchangeable:
 *
 *      ok = (A && B && C);                 false-first: the 0 is hoisted to the
 *                                          TOP of the chain, the 1 out of line
 *      ok = 1; if (!(A && B && C)) ok = 0; a third shape again
 *      if (A && B && C) ok = 1; else ok=0; true-first: the 1 sits INSIDE the
 *                                          last compare's block, the 0 out of
 *                                          line  <- this is the ROM
 *
 * So the recorded "which side of the if carries the exit picks the condition"
 * extends to flag variables: WHICH LITERAL IS OUT OF LINE TELLS YOU WHETHER THE
 * SOURCE IS AN EXPRESSION-ASSIGNMENT OR AN if/else STATEMENT. The expression
 * form puts the default out of line; the statement form puts the else arm out.
 *
 * THREE NEGATIVES WORTH THE SPACE. De Morgan is not a lever on this shape --
 * the negated-or form compiled byte-identically to the and-form, so rewriting
 * the predicate does nothing and only expression-to-statement moved it. The
 * flag's type is free: int and unsigned char both match, so do not spend a
 * round on it once the shape is right. And a `static` helper is NOT the same as
 * duplicated source -- factoring the triple test into a function called twice
 * went to 37 differing, because gcc inlines it but shares the copies'
 * structure differently. DUPLICATED ROM CODE MEANS DUPLICATED SOURCE, NOT A
 * HELPER.
 *
 * Free, needing no lever: the CSE that turns the second and third comparisons
 * into compares against the register already known equal to the constant. Write
 * all three against the same literal and gcc's equivalence-after-compare does
 * it.
 *
 * The corpus grep paid off in orientation rather than text: a solved file in
 * another overlay carries the same three-field rest-value idiom and settled the
 * field triple and their unsigned typing immediately.
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short facing;
    int x;
    int pad0c;
    int y;
    int pad14;
    int f18;
    int f1c;
    int pad20[6];
    unsigned int f38;
    unsigned int f3c;
    unsigned int f40;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __Func_8092950(int slot, int n);
extern unsigned int iwram_3001e40;

void OvlFunc_969_200b7c4(void)
{
    struct Actor *a;
    struct Actor *b;
    int ok;

    a = __MapActor_GetActor(0x14);
    b = __MapActor_GetActor(0x13);
    if (a->f38 == 0x80000000 && a->f3c == 0x80000000 && a->f40 == 0x80000000)
        ok = 1;
    else
        ok = 0;
    if (b->f38 == 0x80000000 && b->f3c == 0x80000000 && b->f40 == 0x80000000)
        ok = 1;
    else
        ok = 0;
    if (ok) {

        a->facing = 0;
        b->facing = 0;
        if (__GetFlag(0x235)) {
            __Func_8092950(0x14, 7);
            __Func_8092950(0x13, 7);
            if (a->f18 < 0x14000) {
                a->f18 += 0x200;
                a->f1c += 0x200;
                b->f18 += 0x200;
                b->f1c += 0x200;
            }
        } else if (iwram_3001e40 & 2) {
            __Func_8092950(0x14, 0xf);
            __Func_8092950(0x13, 0);
        } else {
            __Func_8092950(0x14, 0);
            __Func_8092950(0x13, 0xf);
        }
        if (__GetFlag(0x234)) {
            if (a->x < 0x1380000) {
                a->x += 0x1000;
                b->x += 0x1000;
            }
            if (a->y > 0xb60000) {
                a->y -= 0x1000;
                b->y -= 0x1000;
            }
        }
    }
}

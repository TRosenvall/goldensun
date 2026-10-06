/* OvlFunc_common1_588  --  NOT MATCHING
 *
 * NON-MATCHING, 9 differing encodings of 37  (MEASURED, batch 332).
 *   LENGTHS AGREE: stream 37 against 37, instructions 27 against 27, SIZE
 *   ref 92 bytes, ours 92.  So this figure IS a distance.
 *   SEVEN of the nine are register ROLES in one region.  The other two are
 *   the area pool words, which are PHANTOM -- see CORRECTION ONE below before
 *   treating them as a blocker.
 *   The batch-319 backfill figure -- twenty-five of thirty-seven, at a stream
 *   two encodings short -- is SUPERSEDED and so is its relocation warning.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_common/common1_588.c \
 *     asm/overlays/common/common1_a_a_a_a_c_a.s --func OvlFunc_common1_588
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * Source asm: goldensun/asm/overlays/common/common1_a_a_a_a_c_a.s
 * A screen reading of fifteen instructions in disagreeing regions of
 * thirty-three was recorded here; it is SUPERSEDED.
 *
 * BLOCKER CLASS: gcc if-converts the three-way constant select.
 *
 * The ROM keeps three separate blocks, each loading its message id and jumping
 * to a shared join:
 *
 *      cmp r2, r3 / bne L0 / ldr r0, =0x2076 / b L1
 *      L0: ... bne L2 / ldr r0, =0x2078 / b L1
 *      L2: ldr r0, =0x207a
 *      L1: add r0, #1 / bl __MessageID
 *
 * gcc hoists each pool load ABOVE its compare and branches straight to the
 * join, which is TWO instructions shorter.  (This block said four; that was
 * wrong, and the two are the two branches to the join.)
 *
 * WHAT WAS TRIED
 *   1. An if/else-if chain assigning a separate local.  Two instructions
 *      short, and no longer the body in this file.
 *   2. Explicit blocks with `goto done;` from each arm -- literally the ROM's
 *      block structure.  Identical to (1), not to the reference.
 *
 * (2) is the informative result: this is not block PLACEMENT, which source
 * structure can sometimes reach, but a pool load scheduled before its own
 * compare. Same family as the pool-loads-first parks.
 *
 * THIS FUNCTION IS WHERE _AREA_8f AND _AREA_90 WERE IDENTIFIED. The values are
 * compared against gState+0x1C0 while the message ids passed to __MessageID are
 * 0x2076/0x2078/0x207a -- so a value-based reading would have named them
 * _FILE_8f and _FILE_90, which is what file_table.sym calls them. See the
 * batch-67 block in area.sym.
 *
 * ----------------------------------------------------------------------------
 * BATCH 332.  THREE CORRECTIONS AND A MECHANISM.
 *
 * DIRECTION OF THE GAP: the reference is TWO 16-bit instructions LONGER than
 * ours, so WE ARE MISSING TWO, and they are the two unconditional branches to
 * the shared join.
 *
 * CORRECTION ONE -- THE RELOCATION WARNING IN THE LEADING BLOCK READS THE WRONG
 * WAY, and this tree already says so elsewhere.  The two area symbols are REAL
 * entries in area.sym (they are defined there by value), so they emit no bytes
 * and resolve at link; the reference is a DISASSEMBLY, in which a symbol was
 * baked to its value before the .s was written, so the assembled reference
 * cannot carry a relocation for it either way.  The symbol spelling therefore
 * shows as a PHANTOM relocation.  That is exactly the class const.sym's note on
 * its a1 entry describes, and the authority is the full build, not the
 * relocation list.  This figure IS a distance.
 *
 * CORRECTION TWO -- the leading block's claim that the pool loads are "hoisted
 * ABOVE the compare and the branch goes straight to the join" is right about
 * WHAT happens and wrong about WHO does it.  It is not the scheduler and it is
 * not block placement.  It is IF-CONVERSION: the pass dump taken between
 * combine and regmove shows four jump insns, and the very next dump -- the
 * if-conversion pass, run unconditionally whenever optimising at toplev.c:3232
 * -- shows two.  The transformation is find_if_case_1 at ifcvt.c:1651.
 *
 * ITS GATES, which is what a source lever has to reach:
 *   * the then block must have ONE successor, must not fall through, and must
 *     have ONE predecessor (ifcvt.c:1660-1670);
 *   * the else block must FOLLOW the then block in block order (:1673);
 *   * the then block must be SMALL -- count_bb_insns no greater than
 *     BRANCH_COST (:1683), and config/arm/arm.h:2413 makes BRANCH_COST ONE in
 *     Thumb when optimising above the first level, so a then block of two
 *     insns or more already refuses;
 *   * dead_or_predicable must succeed (:1697), and in the non-predicated case
 *     that refuses on a CALL, on a may_trap_p pattern, on ANY MEMORY REFERENCE
 *     in the then block (:1918-1928), and on the register set in the then block
 *     being live out of the else block or set inside the test range.
 *
 * CORRECTION THREE -- "Explicit blocks with goto done from each arm -- literally
 * the ROM's block structure" being recorded as reproducing nothing new is
 * correct, and now explained: source block STRUCTURE cannot reach any of those
 * gates.  Five further structures were measured and all are inert, each leaving
 * the function two instructions short: an inverted outer test with the chain in
 * the then arm, a nested-brace else, a conditional-expression chain, two
 * independent tests with the default assigned first, and an all-goto form with
 * inverted senses.
 *
 * WHAT DOES REACH A GATE, and it takes the function to the SAME LENGTH as the
 * reference: REUSE THE COMPARED VARIABLE AS THE MESSAGE ID.  Assigning the ids
 * to the halfword that was just read, instead of to a second local, makes that
 * register live out of the else block, so dead_or_predicable refuses and both
 * branches to the join survive.  Measured: same instruction count, same stream
 * length, and the residue is SEVEN instructions plus the two phantom area pool
 * words.  All seven are register ROLES in one region -- the reference builds the
 * 0xe0 shift in r1 and lands the halfword in r2, where this lands the halfword
 * in r0 and is pushed to r2 for the shift.
 *
 * AND THAT LAST BIT IS FORCED, which is why the reuse cannot finish the job:
 * the join's increment is a two-address add into the argument register, so the
 * single reused pseudo MUST be r0.  The reference plainly has two distinct
 * registers there, so the original had two distinct variables -- and with two
 * variables, if-conversion fires again.
 *
 * THE GATE THE ORIGINAL ACTUALLY TRIPPED IS THE MEMORY ONE, and it is worth
 * recording precisely because it is cheap to check and nobody here had looked.
 * In the pre-reload dumps the area ids appear as MEMs from the start -- a
 * symbol_ref in Thumb is sent through force_const_mem at expand -- while a
 * plain const_int message id stays a const_int until reload turns it into a
 * pool load.  So if the three message ids were SYMBOLS, each then block would
 * contain a memory reference, find_memory would fire at ifcvt.c:1926, and the
 * conversion would be refused with the two variables kept apart.
 *
 * MEASURED WITH A LABELLED INSTRUMENT (three fictitious value-named message
 * symbols, NOT shipped and NOT proposed): the ENTIRE INSTRUCTION STREAM becomes
 * identical, and the only differing encodings are the five pool words, every
 * one an unresolved placeholder -- two of which are the real area entries and
 * resolve at link.
 *
 * THAT IS NOT A LANDING AND MUST NOT BE READ AS ONE.  These message ids are
 * neither eight-bit-movable nor shiftable, so gcc pools them as literals anyway
 * and the pool word proves nothing about the source -- which is precisely the
 * class the owner DECLINED in docs/owner-decisions.md, and the force_const_mem
 * argument is named there and rejected as explaining "the scheduling, not the
 * value".  The one thing batch 332 adds is that here the MEM does not explain a
 * schedule at all: it explains a LENGTH, through an if-conversion gate, worth
 * two instructions.  Whether that is an independent reason the original named
 * them is the owner's call and nothing has been added to any .sym file.
 *
 * SO THE PARK STANDS, and the body to beat is the compared-variable reuse at
 * equal length with a seven-instruction register-role residue, not the
 * if/else-if chain below.  The next lever to try is anything that makes a then
 * block two insns or more, since BRANCH_COST of one means that alone refuses
 * the conversion.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_8f;
extern int _AREA_90;
extern void __Func_8019908(int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(void *a, int n);

void OvlFunc_common1_588(void *a, int b)
{
    unsigned char *g;
    void *p;
    unsigned int k;
    int v;

    p = a;
    __Func_8019908(b, 5);
    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)(g + (unsigned int)0);
    if (v == (int)(&_AREA_8f))
        v = 0x2076;
    else if (v == (int)(&_AREA_90))
        v = 0x2078;
    else
        v = 0x207a;
    __MessageID(v + 1);
    __ActorMessage(p, 0);
}

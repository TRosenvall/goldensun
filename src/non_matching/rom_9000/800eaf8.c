/* Func_800eaf8 -- asm/rom_9000/rom_ea54_c_c.s
 *
 * BLOCKER: OPERAND MODE on one stored constant. 3 of 78, LENGTH EXACT.
 *
 * A key-binding dispatcher: five masks read out of gState are tested against
 * gKeyPress; the first three set a halfword flag through a pointer from
 * iwram_3001ebc and return 1, the last two pass a mask to Func_800ea60.
 *
 * TWO LEVERS LANDED, worth 28 lines between them:
 *
 *   1. gSTATE POINTER + volatile KEY READ                50 -> 66 lines
 *      `gState + 0x85 * 4` folded to `ldr r0, =gState+532`, after which gcc
 *      addressed every other field relative to THAT (`sub r3, r0, #4`,
 *      `ldrh r3, [r0, #2]`). A local `unsigned char *g = gState` restores the
 *      ROM's runtime construction (`mov r2, #0x85 / lsl r2, #2 / add r3, r4,
 *      r2`) at each site. Separately, gcc CSE'd gKeyPress into one register
 *      where the ROM keeps the ADDRESS and re-loads `[r0]` at all five tests
 *      -- `volatile` on the extern reproduces that.
 *
 *   2. WRITE THE TAIL INTO EVERY ARM                     66 -> 78 lines, 3 diff
 *      Written as three arms setting an `id` and falling into a shared store,
 *      gcc hoisted each `mov id` ABOVE its test, inverted the branch and
 *      dropped the `b` to the tail -- twelve lines short. Writing the store
 *      out in full in each arm lets gcc CROSS-JUMP them back into one tail,
 *      which is the ROM's shape exactly, id in a register and all.
 *      This is batch 152's "put the call in every arm" applied to a store.
 *
 * WHAT REMAINS, all three lines of it:
 *
 *     rom    mov r3, #0x1 / strh r3, [r2] / mov r5, #0x1
 *     ours   ldr r3, =0x1 / mov r5, #0x1  / strh r3, [r2]
 *
 * VERIFIED IN THE GENERATED ASSEMBLY, not inferred: gcc emits a literal
 * `.word 1` in the pool. It is a real pool entry, so the object is a different
 * size and this would fail `make compare` -- it was NOT installed.
 *
 * This is batch 155's operand-mode rule: a bare literal into a HALFWORD store
 * is HImode and gets pooled, while the same constant through an `int` local is
 * SImode and becomes a `mov`. That fix closed Func_8011b00. HERE IT CANNOT BE
 * APPLIED, and that is the finding:
 *
 *   int local assigned in each arm before the store   76 lines, 42 differ
 *   int local assigned once above the chain           78 lines, 31 differ
 *   the same with an unsigned short local             78 lines, 31 differ
 *   int local with an initialiser at declaration      78 lines, 43 differ
 *   `r = 1;` first, then storing `r`                  75 lines, 50 differ
 *   `r = 1;` first, then storing the literal          75 lines, 50 differ
 *   the store made volatile                           78 lines,  3 differ (tie)
 *
 * EVERY form that introduces a named value into the arms DESTROYS THE
 * CROSS-JUMP, because the arms are then no longer identical tails and gcc has
 * nothing to merge. The two levers are in direct conflict: cross-jumping needs
 * the arms textually identical, and the operand-mode fix needs an extra
 * statement inside them.
 *
 * That conflict is the reusable result. Both levers are documented and both
 * are correct in isolation; this is the first case where applying one forfeits
 * the other. Anything that resolves it has to put the SImode constant in the
 * merged tail without putting a statement in the arms, and no C spelling
 * available here does that.
 */
extern int iwram_3001ebc;
extern unsigned char gState[];
extern volatile int gKeyPress;
extern int Func_800ea60(int mask);

int Func_800eaf8(void)
{
    unsigned short *p;
    unsigned char *g;
    int r;

    p = (unsigned short *)iwram_3001ebc;
    r = 0;
    if (p == 0)
        return 0;
    g = gState;
    if (gKeyPress & *(unsigned short *)(g + 0x85 * 4)) {
        p[0xb9] = 1;
        r = 1;
    } else if (gKeyPress & *(unsigned short *)(g + 0x84 * 4)) {
        p[0xba] = 1;
        r = 1;
    } else if (gKeyPress & *(unsigned short *)(g + 0x216)) {
        p[0xbb] = 1;
        r = 1;
    } else if (gKeyPress & *(unsigned short *)(g + 0x86 * 4)) {
        r = Func_800ea60(*(unsigned short *)(g + 0x88 * 4));
    } else if (gKeyPress & *(unsigned short *)(g + 0x21a)) {
        r = Func_800ea60(*(unsigned short *)(g + 0x222));
    }
    return r;
}

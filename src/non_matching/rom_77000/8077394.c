/* ===================== BATCH 297a DELTA -- GetUnit =====================
 * FIRST MEASUREMENT EVER RECORDED FOR THIS PARK: 16 encodings of 31 differ, and
 * it IS a true distance -- ref 31 enc / 68 bytes / 2 rel, ours 31 / 68 / 2.
 * The park quoted no objcmp figure and carried no `Verify with:` recipe, so
 * tools/parkcheck.py reported it UNCHECKABLE and no ranking pass could see it.
 *
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/8077394.c \
 *     asm/rom_77000/rom_77320_a_a_c_c_a.s --func GetUnit
 *
 * SPLIT SHAPE: the reference holds TWO functions (Func_8077348 @ 0x08077348 and
 * GetUnit @ 0x08077394) and datacheck.py prints nothing -- no data section, so a
 * landing is a TEXT-ONLY split with NO new export.
 *
 * ALL 16 ARE ONE PERMUTATION, and the counts already agree:
 *     ref   1 mov r3, lr   (dead)     ours  1 mov r2, r0   (copy of `id`)
 *     ref   id in r0, base in r2, k in r3
 *     ours  id in r2, base in r3, k in r0
 * Our 31st instruction is spent copying `id` out of r0 because `k` takes r0; the
 * ROM spends its 31st on the dead `mov r3, lr` and leaves `id` in r0.
 *
 * THE PARK'S HYPOTHESIS IS NOT SOURCE-REACHABLE, and that is now measured rather
 * than assumed.  It proposed "the original source read a variable before
 * assigning it".  gcc-2.96 DELETES every such read -- all of these are exactly
 * 16 of 31, i.e. byte-for-byte the same output as the plain file:
 *     int k = k;                                   16
 *     int u; k = u;                                16
 *     int u; k = u; u = k;                         16
 *     int u; register int u2 __asm__("lr"); k = u2; 16
 * So an uninitialised read cannot produce the dead move; whatever does, it is not
 * a variable this source can name.
 *
 * WHAT THE PERMUTATION IS REALLY ABOUT, and it is checkable: thumb's
 * REG_ALLOC_ORDER starts 3, 2, 1, 0, so the multiplier SHOULD get r3 -- and it
 * would, except that `k` is tied to r0 by the return-value copy, which outranks
 * the allocation order.  `register int k __asm__("r3")` proves it: the multiply
 * block then comes out EXACTLY as the ROM has it
 * (`mov r3,#0xa6 / lsl r3,#1 / mul r3,r0`, ref indices 5-7), and the 27 that
 * remain are (a) the one-instruction shift from the missing dead `mov r3, lr`
 * and (b) a new `add r3,r3,r2 / mov r0,r3` pair at the end of the second block
 * where the ROM has a single `add r0,r3,r2` -- the pin is too strong to release
 * r3 for the result.  So a pin alone does not land it either, and it would need
 * a fakematch row.
 *
 * ALSO MEASURED AND INERT (all 16): block-scoped `int k` per arm; two separate
 * locals k and k2; `k` declared last; `k = 0xa6; k <<= 1; k *= id;`;
 * `(int)base + k` operand order; a local `n = id` used throughout.
 * WORSE: a named result variable with a single `return` (26).
 * -- scratch_elev/b297a/t5
 */

/* GetUnit  --  0x08077394, asm/rom_77000/rom_77320_a_a_c_c_a.s
 *
 * BLOCKER CLASS: an uninitialised-pseudo copy in the prologue, plus the
 * register assignment that follows from it.
 *
 * WHAT IT DOES
 * Maps a unit id to a 0x14c-byte record. Ids 0..7 index gPartyStatus
 * directly. Ids 0x80..0x85 index the block at iwram_3001f28, biased by
 * -0xa600 (= 0x80 * 0x14c) so the same multiply serves both ranges. Anything
 * else, or a null block pointer, returns 0. Both range tests are UNSIGNED --
 * the ROM uses bhi, not bgt.
 *
 * THE INSTRUCTION THAT CANNOT BE WRITTEN
 *
 *      push {lr}
 *      mov  r3, r14      <-- copies the return address into r3, which is
 *      ldr  r2, =gPartyStatus    overwritten on both paths before any read
 *
 * That is a dead read of an UNINITIALISED value. gcc-2.96 emits exactly this
 * shape when a pseudo is live-in with no reaching definition: the pseudo gets a
 * hard register from REG_ALLOC_ORDER (arm.h:989 lists r14 sixth, right after
 * r12) and reload materialises the copy. So the original source read a variable
 * before assigning it, on some path that this reconstruction initialises.
 *
 * IT IS ALSO UNIQUE. A sweep of the corpus finds 211 `mov rN, r14`
 * instructions and exactly ONE of them immediately after `push {lr}` -- this
 * one. The other 210 are r14 used as an ordinary scratch register after being
 * saved, which is normal for this compiler. So there is no second example to
 * generalise from.
 *
 * WHY IT MATTERS BEYOND ONE INSTRUCTION. The dead move writes r3, and r3 is
 * where the ROM then keeps the 0x14c multiplier for the rest of the function,
 * leaving the id undisturbed in r0. Without it gcc has no reason to reserve
 * r3, so it puts the multiplier in r0 and copies the id to r2 -- and every
 * subsequent instruction differs by that permutation.
 *
 * WHAT WAS TRIED
 *   - `id * 0x14c` and `0x14c * id`: both give `mul r0, r3`. The ROM's
 *     `mul r3, r0` needs the CONSTANT as the multiply's destination, which is
 *     reached by `k = 0x14c; k *= id;` -- that part does work, and gets the
 *     line count from 30 to the ROM's 31.
 *   - Declaring `k` before or after the other locals: no effect on which hard
 *     register it lands in.
 *   - `(id - 0x80) * 0x14c` versus `id * 0x14c - 0xa600`: the ROM computes the
 *     second, and the pooled constant confirms it (0xffff5a00 = -0xa600, not
 *     -0x2a600). That is settled and is in the source below.
 *   - Result-variable spelling instead of `return` per path: no change.
 *
 * The remaining gap is one dead instruction and the allocation it forces.
 * Producing it would mean deliberately reading an uninitialised variable, which
 * is a guess at which variable and not something the ROM's bytes identify.
 */

extern unsigned char gPartyStatus[];
extern char *iwram_3001f28;

void *GetUnit(unsigned int id)
{
    int k;
    char *base;
    char *p;

    base = gPartyStatus;
    if (id <= 7) {
        k = 0x14c;
        k *= id;
        return (void *)(k + (int)base);
    }
    if (id - 0x80 <= 5) {
        p = iwram_3001f28;
        if (p != 0) {
            k = 0x14c;
            k *= id;
            return (void *)((int)(p + k) - 0xa600);
        }
    }
    return 0;
}

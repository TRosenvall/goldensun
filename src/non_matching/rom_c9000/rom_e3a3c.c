/* Anim_Attack  [rom_c9000]  --  5 of 39, MEASURED batch 326 (brief H).
 *
 * NON-MATCHING, 5 of 39 encodings.  INSTRUCTION COUNT 39 = 39 and SIZE equal,
 * so the 5 IS a true distance (no pad absorbing a length difference).
 * objcmp verbatim:
 *     XX ENCODINGS differ in 5 place(s) (ref 39, ours 39)
 *        first at index 12: ref 682b  ours 682a
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/rom_e3a3c.c \
 *     asm/rom_c9000/rom_e3958_c_c_c_c_a.s --func Anim_Attack
 *
 * THE RESIDUE IS TWO REGISTERS, AND NOTHING ELSE (aligncmp, 34/39 aligned-equal):
 *     ref   ldr r3,[r5] / adds r1,r3,#0 / subs r1,#0x64 / cmp r1,#0x23 ... cmp r3,#0xc7
 *     ours  ldr r2,[r5] / adds r3,r2,#0 / subs r3,#0x64 / cmp r3,#0x23 ... cmp r2,#0xc7
 * ROM: v=r3, t=r1.  Ours: v=r2, t=r3.
 *
 * ============ WHY, SETTLED FROM local-alloc.c AND CONFIRMED BY LADDER ========
 * `.17.lreg`: pseudo 33 = v (3 refs / 4 insns, live in bb0 AND bb2 -> global),
 * pseudo 34 = t (2 refs, LOCAL TO bb0, pref LO_REGS, no suggestion), pseudos
 * 35/36/37 = the three galloc size arguments (copy-suggested r1, and all three
 * DIE BEFORE t is born).  `.17.lreg` ends `;; Register 34 in 3.` and `.18.greg`
 * says `32 in 5  33 in 2  34 in 3`, so LOCAL-alloc took r3 for t and global then
 * had to give v r2.
 *
 * `block_alloc` (local-alloc.c:1337-1348) runs a SUGGESTED pass first, then a
 * life-length pass calling `find_free_reg` (:1441).  `find_free_reg`
 * (:1963-2050) builds `used` from `fixed_reg_set` (t crosses no calls), the
 * union of `regs_live_at[]` over t's lifetime, and the complement of min_class,
 * then walks REG_ALLOC_ORDER = {3,2,1,0,12,14,4,5,...} (arm.h:989) and returns
 * the FIRST free register.  r3 is free, so t is r3.
 *
 * For t to be r1, BOTH r3 AND r2 must be in `used` across t's lifetime -- and
 * t's lifetime is insns 39-41, the LAST TWO INSNS OF bb0.  No *local* qty can
 * overlap it (a qty live at a block end is not block-local and goes to global
 * alloc instead), and the only hard regs live there are r5 (p) and sp.  So the
 * walk is deterministic and the source has nothing to put in r3 or r2.
 *
 * MEASURED LADDER -- this is the confirmation, not the argument:
 *     no instrument                      t -> r3 (1st in order)    5
 *     register int v __asm__("r3")        t -> r2 (2nd in order)   *3*
 *     register unsigned int t __asm__("r1")                        *0*
 *     both pinned                                                  *0*
 * Blocking r3 alone moves t exactly one step down REG_ALLOC_ORDER.  That is the
 * prediction, and it is why no respelling helps.
 *
 * 0 OF 39 WITH ONE PIN: `register unsigned int t __asm__("r1")` on the range
 * temporary.  Device-free; r1 is call-clobbered and t crosses no calls, so the
 * prologue push set is unchanged.  NOT LANDED: pin policy prefers a pin-free
 * body, so this is parked at 5 pin-free with the 0 recorded beside it.
 * See reports/pass3-depin.md.
 *
 * MEASURED FLAT AT EXACTLY 5 (sweep_variants, byte-identical to this file):
 *   compiler temp `(unsigned)(v - 0x64) <= 0x23`; `unsigned v` with
 *   `(int)v > 0xc7`; `t` declared before `v`; `v >= 0xc8`; `t = v = *p; t -= 0x64`;
 *   `t` scoped to an inner block; `void *p` with a cast; `int t` with
 *   `(unsigned)t <= 0x23`.
 * MEASURED WORSE: inverted `if (t > 0x23) {...} else {...}` = 10 and RELOCDIFF;
 *   `unsigned char t` = 29.
 *
 * WHAT IS RIGHT (unchanged from the earlier header, and re-confirmed): the range
 * check is a SUBTRACTION into an unsigned local followed by ONE compare, and the
 * second bound is a separate SIGNED compare, so the two must not be fused.
 */
extern void *galloc_ewram(int tag, int size);
extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void BaseAnim_SpecialAttack(void *p);
extern void Anim_CriticalHit(void *p);
extern void BaseAnim_Attack(void *p);

void Anim_Attack(int *p)
{
    int v;
    unsigned int t;

    galloc_ewram(0x29, 0x60e);
    galloc_iwram(0x27, 0x782c);
    galloc_iwram(0x28, 0x80 << 7);
    v = *p;
    t = v - 0x64;
    if (t <= 0x23)
        BaseAnim_SpecialAttack(p);
    else if (v > 0xc7)
        Anim_CriticalHit(p);
    else
        BaseAnim_Attack(p);
    gfree(0x28);
    gfree(0x27);
    gfree(0x29);
}

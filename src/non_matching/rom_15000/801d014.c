/* Func_801d014 (0x0801d014) -- NON-MATCHING, 3 differing encodings of 91.
 *
 *   SIZE IS EXACT (220 bytes both), ENCODING COUNT IS EXACT (91 = 91), and ALL
 *   FOUR RELOCATIONS ARE EXACT -- objcmp prints no SIZE and no RELOCATIONS
 *   line.  NO PINS, NO FLAGS, NO SYMBOLS.  The literal pool is the ROM's pool
 *   in the ROM's order.  Figure re-derived in batch 325.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801d014.c asm/rom_15000/rom_1ca1c_c_a_c.s --func Func_801d014
 *
 * SPLIT SHAPE: none needed for a park.  tools/datacheck.py on
 * asm/rom_15000/rom_1ca1c_c_a_c.s reports no data section and no required data
 * export; the file holds this one function.  shimcount.py: PIN-FREE.
 *
 * WAS 37 of 91; batch 324 took it to 3 with one edit (`unsigned char t`, where
 * `int t` reads 49).  The 37's decomposition, the `expand_assignment` reading
 * behind it (expr.c:3402 / :3604 / :3643) and the whole measured table from
 * that batch stand and are not repeated here.
 *
 * ======================================================================
 * THE REMAINING 3, AND THE ONE EDGE THAT HOLDS IT
 * ======================================================================
 * Ref indices 36-38 (0x48-0x4c), inside copy 3:
 *   ROM   lsls r0,r0,#2 | movs r3,#15 | strb r3,[r2]
 *   ours  movs r3,#15   | strb r3,[r2] | lsls r0,r0,#2
 * `.23.sched2` at t = 78:  `Ready list (t = 78):  270  102`  ->  schedules 102.
 *   insn 102  (set (reg:QI 3 r3) (const_int 15))      prio 90   `movs r3,#15`
 *   insn 270  (set (reg:SI 0 r0) (ashift r0, 2))      prio 89   `lsls r0,r0,#2`
 * `rank_for_schedule`'s first rung is priority, so the 1-point gap decides.
 *
 * THE PREVIOUS HEADER'S DIAGNOSIS IS REFUTED ON ITS DECIDING POINT.  It said
 * the gap "arrives by TWO independent paths, each worth exactly +1 ... Cut
 * either of 103's two edges and the other still holds it at 89", naming
 *     103 -> 110 (WAR on r3) + 1   and   103 -> 112 (memory) + 2.
 * The WAR edge is worth ZERO, not +1: ARM's `ADJUST_COST` is `arm_adjust_cost`
 * (arm.h:2418) and its FIRST statement is
 *     if (REG_NOTE_KIND (link) == REG_DEP_ANTI
 *         || REG_NOTE_KIND (link) == REG_DEP_OUTPUT)
 *       return 0;                                        (arm.c:2425-2427)
 * so that path gives prio(110) + 0 = 88.  **prio(103) = 89 comes ONLY from the
 * memory edge 103 -> 112**, as prio(112) + insn_cost(103) = 87 + 2.  The RTL
 * confirms the kinds: insn 112's dependence list ends `(insn_list 103 (nil))`
 * -- no REG_DEP_ prefix, so kind 0, a true memory dependence -- while insn
 * 110's list carries `(insn_list:REG_DEP_ANTI 103 ...)`.
 *
 * SO THERE IS ONE EDGE, AND CUTTING IT CLOSES THE 3.  Writing X = prio(124),
 * the whole tail is a rigid ladder:
 *     prio(121) = X+1   prio(238) = X+3   prio(112) = X+2
 *     prio(110) = max(prio(112)+1, prio(238)) = X+3   -> prio(270) = X+4
 *     prio(103) = max(prio(112)+2, prio(110))  = X+4   -> prio(102) = X+5
 * i.e. prio(102) = prio(270) + 1 for EVERY X, which is why the previous
 * header's nine reorderings and five-temporary variants were all worse and why
 * no ladder-length change can help.  Remove the 103 -> 112 edge and prio(103)
 * drops to X+3 = prio(110), prio(102) = X+4 = prio(270), the rungs continue,
 * and **270 wins**: at t = 78 `last_scheduled_insn` is 100, whose INSN_DEPEND
 * contains 102 by a REG_DEP_ANTI on r3 (class 2, haifa-sched.c:4083) and does
 * NOT contain 270 (link == 0 -> class 3, :4078), and line 4094 prefers the
 * higher class.
 *
 * MEASURED, WITH AN INSTRUMENT: 0 of 91, size and relocations exact.
 * `extern const unsigned char gState[];` with DIRECT `gState[0x205]` accesses
 * (the `g` local deleted) is byte-identical -- 220 bytes, 91 encodings, 4
 * relocations.  `.23.sched2` then shows the predicted tie exactly: the movqi
 * constant and the ashift both at prio 88, their store and add both at 87, and
 * the memory edge replaced by a bare register anti-dependence.
 * `extern const GlobalState gState;` as a struct with the five byte members
 * named, member access throughout, is ALSO 0 -- same figure, same mechanism.
 *
 * ======================================================================
 * THE const IS A DEVICE, BY GCC'S OWN ARGUMENT -- SO THE BODY SHIPS WITHOUT IT
 * ======================================================================
 * `RTX_UNCHANGING_P` is only cut-through at `true_dependence` (alias.c:1583):
 *     if (RTX_UNCHANGING_P (x) && ! RTX_UNCHANGING_P (mem)) return 0;
 * and it is set on a DIRECT reference to a `const` object but NOT on a
 * dereference of a pointer-to-const, for a reason gcc states in the source
 * (expr.c:6507-6512, the INDIRECT_REF case):
 *     "It is incorrect to set RTX_UNCHANGING_P from TREE_READONLY here,
 *      because, in C and C++, the fact that a location is accessed through a
 *      pointer to const does not mean that the value there can never change."
 *     RTX_UNCHANGING_P (temp) = TREE_READONLY (exp) & TREE_STATIC (exp);
 * The flag therefore asserts that THE VALUE CAN NEVER CHANGE.  gState is the
 * game's mutable global state block; 565 `extern ... gState` declarations across 566 other
 * files in this tree are non-const (283 `extern unsigned char gState[]`,
 * 232 `extern GlobalState gState`, and twelve further spellings).
 * A `const` here is a false statement about the object whose only effect is to
 * delete one scheduling dependence -- a DEVICE by docs/owner-decisions.md's
 * standard, and not the "applied consistently to a genuine property" case.
 * The 0 is recorded above as a FIGURE ABOUT THE BLOCKER; this body is the
 * device-free one at 3.  An owner ruling would settle it either way.
 *
 * ======================================================================
 * CORRECTION TO A RECORDED BOUND IN docs/elevation.md
 * ======================================================================
 * It carries: "**`const` is not the cure for an alias-set-0 memory edge**, even
 * though it looks like exactly that (`anti_dependence` returns 0 for an
 * unchanging read).  It is **bit-identically inert on the dependence table**."
 * That is wrong as stated, and the counterexample is this function.  It is
 * inert through a POINTER and decisive on the OBJECT, because of
 * `& TREE_STATIC (exp)` above.  Measured here, all against the same body:
 *     `const unsigned char *g` (pointee const)                     3   inert
 *     `extern const unsigned char gState[]` kept behind `g`        3   inert
 *     `extern const unsigned char gState[]` + direct access        0
 *     `extern const GlobalState gState` + member access            0
 *     struct members individually `const`, object not              3   inert
 * The honest reading: the bound should say *"a const POINTER cannot cut an
 * alias-set-0 memory edge, because expr.c:6512 ands in TREE_STATIC; a const
 * OBJECT can, and is a device wherever the object is in fact mutable."*
 *
 * ======================================================================
 * DEVICE-FREE ROUTES, MEASURED AND CLOSED
 * ======================================================================
 * `true_dependence` offers exactly four escapes before its QImode catch-all,
 * and all four are closed here:
 *   1. DIFFERENT_ALIAS_SETS_P (alias.c:1573) -- cannot fire: both references
 *      are char-precision and `lang_get_alias_set` (c-common.c:3347-3352)
 *      returns 0 for ANY reference whose type has char precision.  Confirmed,
 *      and NOT weakened by batch 318's retraction: the packed-aggregate and
 *      struct-member routes recorded there reach
 *      `fixed_scalar_and_varying_struct_p`, which in a TRUE dependence is
 *      unreachable -- `aliases_everything_p (x)` returns 1 for a QImode load
 *      at alias.c:1602 and `mem_mode == QImode` returns 1 at :1607, both
 *      BEFORE it.  Measured: gState as a plain struct 3, as a
 *      `__attribute__((packed))` struct 3, the DESTINATION as a struct 3.
 *   2. RTX_UNCHANGING_P -- the device above.
 *   3. base_alias_check (:1592) -- the store's base is the `galloc_ewram`
 *      return value, which `find_base_term` cannot resolve, so it returns "may
 *      conflict" however the load is spelled.
 *   4. memrefs_conflict_p (:1598) -- same reason.
 * And the ORDER route is provably dead, not merely worse: putting copy 3's load
 * above copy 2's constant store replaces the true dependence 103 -> 112 with an
 * ANTI dependence 112 -> 103, which FORCES the ldrb to be scheduled before the
 * strb -- and the ROM has `strb r3,[r2]` BEFORE `ldrb r2,[r3]`.  So the
 * previous header's "hoisting copy 3's load above pair 2's constant store (11)"
 * could not have worked at any figure, and no crossing rescues it.
 *
 * NEXT: nothing device-free is open at the dependence level.  The one untested
 * class is a body in which the destination pointer's base is resolvable by
 * `find_base_term`, which for a `galloc_ewram` return it is not.
 *
 * MEASURED AND INERT AT 3 in batch 325 (candidate prerequisites, not dead ends):
 *   `gState[...]` direct for `g[...]` on all five reads                   0
 *   `const unsigned char *g`                                              0
 *   gState as a struct, member access                                     0
 *   gState as a packed struct, member access                              0
 *   destination `p` as a struct pointer, member access                    0
 *   struct members individually const                                     0
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *galloc_ewram(int tag, int size);
extern unsigned char gState[];
extern void Func_801cf48(void);
extern int StartTask(void *fn, int pri);

void Func_801d014(void)
{
    unsigned char *p;
    unsigned char *g;
    unsigned char t;

    p = galloc_ewram(0x14, 0xc5 << 3);
    DMA3_CLEAR(p, 0xc5 << 3);
    g = gState;
    t = g[0x205];
    p[0x594] = t;
    p[0x599] = 0x18;
    t = g[0x206];
    p[0x595] = t;
    p[0x59a] = 0xf;
    t = g[0x83 << 2];
    p[0x596] = t;
    p[0x59b] = 3;
    t = g[0x20a];
    p[0x597] = t;
    p[0x59c] = 2;
    t = g[0x22a];
    p[0xb3 << 3] = t;
    p[0x59d] = 2;
    StartTask(Func_801cf48, 0xc8 << 4);
}

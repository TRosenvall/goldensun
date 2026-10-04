/* Func_8091254 @ 0x08091254 -- asm/rom_8a000/rom_8d9a4_c_c_c_a_c_a_a.s
 *
 * *** MATCHING.  0 of 29.  PIN-FREE.  (batch 322, brief E) ***
 *   64 bytes, 29 encodings and 2 relocations identical, --func AND --whole.
 *   The park's inherited figure (8 of 29, ref 29 / ours 30) reproduced exactly
 *   before the edit, so the figure was right and only the diagnosis was wrong.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_8d9a4_c_c_c_a_c_a_a.c \
 *     asm/rom_8a000/rom_8d9a4_c_c_c_a_c_a_a.s --func Func_8091254
 *   (and the same with --whole; tools/datacheck.py is silent on the reference,
 *    which holds this function alone -- no split, no exports.)
 *
 * ===== THE PARK SAID "CONSTANT CSE".  REFUTED AT THE SOURCE. =====
 * The park read the ROM's
 *     ldr r1, =0x2a01 / add r3,r4,r1 / add r1,#1 / ... / add r3,r4,r1
 * as cse deriving the second offset from the first.  CSE CANNOT DO THIS.  Its
 * related-value machinery is gated on `GET_CODE (x) == CONST`
 * (rtlanal.c:get_related_value, line 234-247 -- it returns 0 for anything that
 * is not a CONST), and cse.c:4950 only calls use_related_value when
 * `GET_CODE (src_const) == CONST` or the elt already has a related_value.  A
 * plain CONST_INT 0x2a02 is code CONST_INT, never CONST, so it has no related
 * value and no chain is ever built for it.  The related-value path is for
 * symbol+offset, not for integers.
 *
 * *** THE `add r1, #1` IS reload_cse_move2add -- reload1.c:8840, called from
 * reload_cse_regs at reload1.c:7991, i.e. AFTER reload. ***  Its first
 * transform (reload1.c:8888-8912) rewrites
 *     (set (REGX) (CONST_INT A)) ... (set (REGX) (CONST_INT B))
 * into (set (REGX) (plus (REGX) (CONST_INT B-A))), and its preconditions are
 * what this park turns on:
 *   - SAME HARD REGISTER REGX for both constant loads (`reg_set_luid[regno]`,
 *     `reg_offset[regno]` are indexed by the hard regno);
 *   - no CODE_LABEL between them (`reg_set_luid[regno] > last_label_luid`);
 *   - `reg_base_reg[regno] < 0`, i.e. REGX holds a pure constant;
 *   - `rtx_cost (new_src, PLUS) < rtx_cost (src, SET)` -- trivially true here,
 *     0x2a02 costs a literal-pool load and 1 costs an immediate.
 * So this was never a cse question.  IT IS A REGISTER-ASSIGNMENT QUESTION:
 * the two constants must be reloaded into ONE hard register.
 *
 * ===== WHY THE FIX IS THREE DECLARATIONS AND NOTHING ELSE =====
 * Both constant loads here are RELOAD registers, not allocated pseudos --
 * .18.greg prints `Spilling for insn N / Using reg M for reload 0` for each.
 * reload picks the reload reg by REG_ALLOC_ORDER ({3,2,1,0,...},
 * config/arm/arm.h:989), skipping whatever is already in use at that insn.  So
 * the reload register is decided by what LOCAL-ALLOC put in r3 and r2:
 *
 *   park body (`base[0x2a01] = flags; base[0x2a02] = 0;`)
 *       addr A -> r3, addr B -> r2  (ranges overlap)
 *       => reloads land in r2 then r3.  DIFFERENT REGS, move2add dead,
 *          0x2a02 gets its own pool word: 30 insns against 29.
 *   this body
 *       addr A and addr B are ONE register r3 (disjoint ranges, so local-alloc
 *       reuses it), the zero holds r2 across addr A's death
 *       => both reloads must skip r3 (dest) and r2 (live zero) and land in r1.
 *          move2add fires: `add r1, #1`.
 *
 * THE THREE DECLARATIONS EACH BUY ONE OF THOSE FACTS, and all three are needed:
 *   1. `p` named, so addr A is a user pseudo local-alloc ranks first
 *      (.17.lreg: 2 refs across 2 insns, QTY_CMP_PRI highest in the block).
 *   2. `p2` ASSIGNED AFTER `*p = flags`, so addr B is born after addr A dies
 *      and local-alloc hands it r3 again.  Hoisting the assignment into the
 *      declaration (`u8 *p2 = base + 0x2a02;`) reads 3 of 29: the two adds
 *      then overlap, addr B takes r2, and the zero takes r3 -- the two
 *      registers simply swap against the ROM.
 *   3. `z` MATERIALISED BEFORE `*p = flags`, so the zero's live range overlaps
 *      addr A's and local-alloc may not give it r3.  It takes r2, which is
 *      what pushes both reloads out to r1.
 * Only (1)+(2)+(3) together reach 0.  (1)+(3) without (2) is 3; (1)+(2)
 * without (3) is 8 again, because without the zero holding r2 the reloads go
 * back to r2/r3.
 *
 * ===== MEASURED, pin-free unless marked =====
 *   0   this body; also `u8 z; z = 0;` as a statement, and `u32 z = 0;`
 *   3   `u8 *p2 = base + 0x2a02;` hoisted into the declaration (p2/zero swap)
 *   3   + `u8 *p2 = p + 1;`  (exactly inert -- cse re-bases p+1 to base+0x2a02,
 *       which is the one thing the park said and got right)
 *   3   + a trailing unused `u8 z = 0;` declaration (inert: never materialised)
 *   3   + `p2[0] = 0` for `*p2 = 0` (inert)
 *   7   `base[0x2a01] = flags; base[0x2a02] = z;` with `u8 z = 0;`
 *   8   the park's body (BASE); `*(base + 0x2a01) = ...` exactly inert on it
 *   9   zero hoisted but p2 still early
 *  11   PIN `register u8 *p2 __asm__("r3")` -- A PIN MAKES IT WORSE, which is
 *       why no pin is needed or wanted here
 *  13   `u8 *p2` declared before `u8 *p`
 *  14   `u8 z = 0;` between the two pointer declarations
 *  15   stores swapped (`*p2 = 0;` first)
 *  23   one pointer variable reused for both addresses -- move2add DOES fire
 *       (`add r3,#1`) but the whole allocation rotates: base to r2, flags to
 *       r4, and the prologue drops to `push {lr}`.  A global perturbation, not
 *       a distance.
 *  23   `u32 off = 0x2a01; base[off] = ...; base[off+1] = 0;` and the `off++`
 *       form -- RELOC and 27 insns; cprop turns these into reg+reg addressing.
 *  25   PIN p and p2 both to r3 (RELOC, 27 insns)
 *
 * ===== A BOUND WORTH RECORDING, with its evidence =====
 * local-alloc's "fake lifetime" widening (local-alloc.c:1414-1436, the
 * `fake_birth`/`fake_death` pair that makes a quantity conflict with its
 * immediate neighbours to avoid false scheduler dependences) IS DEAD ON THUMB.
 * Both call sites are gated on `!SMALL_REGISTER_CLASSES`, and
 * config/arm/arm.h:1061 is `#define SMALL_REGISTER_CLASSES TARGET_THUMB`.  So a
 * thumb quantity may reuse a register that died on the immediately preceding
 * insn, which is exactly what lets `p2` take r3 here.  Do not explain a
 * failed register reuse on thumb with that widening.
 *
 * Source asm: goldensun/asm/rom_8a000/rom_8d9a4_c_c_c_a_c_a_a.s
 */
#include "gba/types.h"

extern u8 *iwram_3001ed0;
extern void Func_809088c(u8 *a, u8 *b, u8 *c, u32 flags);

/* Arms the fade pair at the far end of the map-state block and kicks the
 * worker.  Returns immediately when no map state has been allocated.
 */
void Func_8091254(u32 flags)
{
    u8 *base = iwram_3001ed0;

    if (base != NULL) {
        u8 *p = base + 0x2a01;
        u8 *p2;
        u8 z = 0;

        *p = flags;
        p2 = base + 0x2a02;
        *p2 = z;
        Func_809088c(base + 0x380, base + 0xe00, base + 0x1880, flags);
    }
}

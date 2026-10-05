/* Func_808fe38 (AllocOverlayState) -- 0x0808fe38
 *
 * MATCHES: 0 of 50 encodings, 120 bytes against 120, 5 relocations identical.
 * PINS: 0.  DEVICES: 0.  Production flags (objcmp reports no flag adjust).
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_8a000/rom_8d9a4_c_c_a_a_c_b.c asm/rom_8a000/rom_8d9a4_c_c_a_a_c_b.s --func Func_808fe38
 *
 * SPLIT: tools/split_s.py asm/rom_8a000/rom_8d9a4_c_c_a_a_c.s Func_808fe38
 *   -> _a.s (Func_808f498, Task_ScreenWindowTransition; 1157 lines)
 *   -> _b.s (Func_808fe38; 49 lines)   tools/datacheck.py silent.
 * EXPORTS: none needed.  Both callees named by this function are defined in
 * _a.s under .thumb_func_start, which already emits .global (include/macros.inc:14).
 *
 * ====================================================================
 * HOW THE PARK'S LAST 11 CLOSED, and what the park got wrong
 * ====================================================================
 * The park's figure (11 of 50, count/size/relocations exact) was correct.  Its
 * diagnosis -- "ONE cause, indices 16-26, NOT source-reachable ... REG_ALLOC_ORDER
 * is {3,2,1,0,...}; the `hi` carrier is a local-alloc quantity that takes r2 and
 * pushes reload's scratch for 0x528 down to r1" -- is right about WHICH registers
 * and wrong about WHICH RUNG, and the rung is where the lever was.
 *
 * The four `(plus (reg p) (const_int N))` address insns (lreg insns 47, 57, 70, 83)
 * cannot hold those constants as immediates, so RELOAD supplies each one.  The
 * register it uses is NOT chosen by REG_ALLOC_ORDER and is NOT what find_reg
 * prints:
 *
 *   - find_reg (reload1.c:1664) prints `Using reg N for reload R` and builds the
 *     function-wide spill set `used_spill_regs`.  Here it printed r2, r2, r1, r2.
 *   - finish_spills (reload1.c:3609-3627) then ENLARGES each chain's set --
 *     "Mark any unallocated hard regs as available for spills" -- to
 *     (~regs holding pseudos live at that insn) AND used_spill_regs.
 *   - choose_reload_regs_init (reload1.c:5129) makes the complement of that
 *     `reload_reg_unavailable`, and reload_reg_free_p (reload1.c:4280) refuses
 *     anything outside it.
 *   - allocate_reload_reg (reload1.c:4962) then walks `spill_regs` -- built
 *     ASCENDING by finish_spills (reload1.c:3531) -- round-robin from
 *     last_spill_reg (reload1.c:5003), reset once per function (reload1.c:821).
 *
 * So the reload register is a CURSOR over the function-wide spill set, skipped
 * past whatever is live at that insn.  Our old spill set was {r1,r2} and the
 * cursor gave r1, r2, r1, r2; the ROM needs r2, r2, r3, r2, which requires the
 * spill set {r2,r3} -- i.e. find_reg must be able to pick r3 at insn 70.
 *
 * It cannot while pseudo 34 (the 0x3f3f carrier) is live across insn 70, because
 * order_regs_for_reload (reload1.c:1534) puts every hard register holding a
 * pseudo live at the insn into `bad_spill_regs`.  Move 34's def BETWEEN the
 * address insn and the store and two things happen at once: r3 is free at insn
 * 70, and pseudo 55 (p+0x534) -- whose local-alloc span now CONTAINS 34's --
 * ranks below 34 on QTY_CMP_PRI, so 34 is allocated first, takes r3 off
 * reg_alloc_order, and 55 falls to r2.  That is exactly the ROM's map.
 *
 * `expand_assignment` is LHS-first (expr.c:3402), so the only way to put the
 * carrier's def after the address is to make the ADDRESS its own earlier
 * statement: name the pointer, THEN assign the carrier.
 *
 * THE PARK HAD BOTH HALVES AND NEVER CROSSED THEM.  Its inert list records
 * "a named pointer for the 0x534 store only -- 11", and its kept-lever list
 * records the carrier placement rule separately.  The pointer alone is inert at
 * 11; the pointer with the carrier moved after it is 0.  Neither half survives
 * one-at-a-time screening -- the crossing law, with the two halves in different
 * lists.
 *
 * ALSO SUPERSEDED: the park's device figure, `register int hi __asm__("r3")`
 * reading 8 of 50, was reading the same fact from the other end -- pinning the
 * carrier to r3 is what the reordering achieves without a pin.  No pin ships.
 */
#include "dma.h"

extern void *galloc_ewram(int index, unsigned int size);
extern void StartTask(void (*task)(void), unsigned int priority);
extern void Task_ScreenWindowTransition(void);
extern void Func_808f498(void);

void Func_808fe38(unsigned int arg0)
{
    void *p;
    unsigned short *q;
    int hi;
    int pr;
    int one;

    p = galloc_ewram(0x1f, 0xa8 << 3);
    DMA3_CLEAR(p, 0xa8 << 3);
    *(unsigned short *)((unsigned int)p + (0xa5 << 3)) = arg0;
    *(unsigned short *)((unsigned int)p + 0x52a) = 0;
    q = (unsigned short *)((unsigned int)p + 0x534);
    hi = 0x3f3f;
    *q = hi;
    one = 1;
    *(unsigned short *)((unsigned int)p + 0x536) = one;
    pr = 0xc8 << 4;
    StartTask(Task_ScreenWindowTransition, pr);
    pr = 0x90 << 3;
    StartTask(Func_808f498, pr);
}

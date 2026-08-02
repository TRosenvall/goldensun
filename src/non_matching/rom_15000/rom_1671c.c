/* Func_801671c  [rom_15000]
 * Source asm: goldensun/asm/rom_15000/rom_15e8c_a_c_c_c.s
 *
 * Parked: logic faithful, does NOT byte-match (endgame permuter seed).
 * Candidate: tools/runs/run_20260607T010203Z/Func_801671c-iter-4.c
 * TODO(residual): reg-alloc/scheduling divergence (register swap / op-order); logic correct. Permuter seed.
 */
extern void Func_80008d8(void);

/* Takes no arguments. Fills the 0xF00-byte text scratch at 0x6002500 with 0
 * (transparent). Func_16738 below is the same fill with colour 4.
 */
void Func_801671c(void) {
    void (*f)(int, int, int) = (void (*)(int, int, int))Func_80008d8;
    f(0x6002500, 0xf0 << 4, 0);
}

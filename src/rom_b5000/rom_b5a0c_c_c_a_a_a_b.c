/* Cluster Func_80b63b0..Func_80b63b0 extracted from goldensun/asm/rom_b5000/rom_b5a0c_c_c_a_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_b5000/rom_b5a0c_c_c_a_a_a_a.o and asm/rom_b5000/rom_b5a0c_c_c_a_a_a_c.o in
 * goldensun/stage1.ld.
 *
 * Seven instructions. Clears 0x10 bytes at ewram_2002224 through Func_80008d4,
 * called via the `_call_via_r3` veneer.
 *
 * PARKED SINCE JUNE as a permuter seed -- "logic faithful, does NOT byte-match",
 * with both residual diffs correctly identified and neither one solved. Both
 * come from types, and neither needs a permuter.
 *
 *  1. ARGUMENT ORDER. The ROM fills r1 then r0; gcc filled r0 then r1. The
 *     function pointer was typed `void (*)(int, int)`. Typing it `int (*)(int,
 *     int)` fixes it. This is the declaration lever reaching through a POINTER
 *     type: an int-returning callee keeps r0 live, so gcc defers writing it.
 *     Same mechanism as the direct-call form in docs/elevation.md, and worth
 *     knowing it applies to indirect calls too -- the pointer's return type is
 *     the declaration here, there is no other one.
 *
 *  2. THE EPILOGUE:
 *
 *         rom    pop {r1} / bx r1
 *         ours   pop {r0} / bx r0
 *
 *     This one is ALREADY IN docs/elevation.md -- gcc pops its return address
 *     into r1 rather than r0 when the return type is non-void, whether or not
 *     anything is returned. Declaring this `int` matches. Nothing new here; it
 *     simply was not applied when the function was parked, and the sibling park
 *     src/non_matching/rom_15000/rom_1671c.c cites the same rule in its own
 *     note.
 *
 * So the whole seven-instruction function is decided by two type choices, one
 * of them already written down. The park note read both diffs as reg-alloc
 * noise and proposed a permuter; neither was noise, and the permuter would have
 * been the wrong tool for both.
 */
extern int Func_80008d4(int a, int b);
extern unsigned int ewram_2002224;

/* Takes no arguments. Clears 0x10 bytes at ewram_2224 and passes the callee's
 * result straight back -- see the header on why the return type is not void.
 */
int Func_80b63b0(void)
{
    int (*fp)(int, int) = Func_80008d4;

    return fp((int)&ewram_2002224, 0x10);
}

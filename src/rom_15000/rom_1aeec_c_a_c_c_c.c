/* Func_801c954 (CloseMenuScreen) @ 0x0801c954  [asm/rom_15000/rom_1aeec_c_a_c_c_c.s]
 *
 * MATCHING.  objcmp --func: 104 bytes, 43 encodings and 7 relocations identical.
 * objcmp --whole: OK whole file (the reference .s holds this function alone, so
 * the whole TU converts).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_1aeec_c_a_c_c_c.c \
 *     asm/rom_15000/rom_1aeec_c_a_c_c_c.s --whole
 *
 * Install: src/rom_15000/rom_1aeec_c_a_c_c_c.c, and delete the hand .s.
 * No split, no exports, no flag group, NO PINS.
 * Retires src/non_matching/rom_15000/801c954.c (was 39 of 43).
 *
 * ------------------------------------------------------------------- 39 -> 0
 *
 * The park had this at 39 of 43 and called it "the SECOND counterexample to the
 * documented remedy" for constant CSE across a call.  TWO EDITS, EACH ONLY HALF
 * THE ANSWER (the first alone measures 14), close it:
 *
 * 1. THE 0xff4 READ BELONGS INSIDE THE `while` CONDITION, not in a pointer
 *    named before the loop.  The ROM has `ldr r0,[r6]` INSIDE the loop and
 *    `add r6,r5,r3` in the PREHEADER -- that is loop.c hoisting the loop-
 *    invariant ADDRESS while the load itself stays (the two calls in the loop
 *    can write memory, so the load is not invariant).  cse1 cannot common an
 *    in-loop occurrence with the pre-call one, so the 0xff4 constant is
 *    rematerialised in the preheader, which is the ROM's second pool reference.
 *
 *    THE PARK'S DIAGNOSIS WAS WRONG ON ITS KEY POINT.  It said "the boundary is
 *    a CALL, which is as strong a boundary as exists".  A CALL IS NOT A
 *    BASIC-BLOCK BOUNDARY in gcc-2.96 -- an ordinary call does not end a BB --
 *    so cse1 saw both occurrences in one extended basic block and commoned them
 *    exactly as it is supposed to.  The remedy in docs/elevation.md was never
 *    tested here; the park tested something else.  Its sibling claim about
 *    src/non_matching/ovl_7b2078/2008388.c ("the first counterexample", boundary
 *    a `beq`) should be re-measured on the same suspicion.
 *
 * 2. THE 0x352/0x354 PAIR NEEDS A NAMED HALFWORD POINTER as well as the named
 *    offset.  With the offset alone, gcc uses the register-offset form
 *    `ldrh r3,[r6,r3]` and constant-folds `off + 2` into 0x354 -- and because
 *    0x354 = 0xd5 << 2, `*thumb_movsi_insn`'s shifted-constant alternative
 *    matches before the pool path, so it emits `mov r3,#0xd5 / lsl r3,#2`, two
 *    instructions and no pool word.  The ROM instead materialises the address
 *    (`add r3,r5,r2 / ldrh r3,[r3]`) and advances the OFFSET register
 *    (`add r2,#2`).  Naming the pointer is what makes gcc materialise;
 *    the `off` variable is what keeps the `add r2,#2`.
 *
 * Measured: the park's body 39; the in-loop read alone 14; both together 0.
 */
extern char *iwram_3001e9c;
extern void CloseUIBox(int a, int b);
extern void WaitFrames(int n);
extern int Func_8017394(int a);
extern void Func_8003f3c(int n);
extern void gfree(int n);

void Func_801c954(void)
{
    char *s;
    unsigned short *q;
    int off;

    s = iwram_3001e9c;
    CloseUIBox(*(int *)(s + 0xff4), 0);
    while (Func_8017394(*(int *)(s + 0xff4)) == 0)
        WaitFrames(1);
    if (*(unsigned short *)(s + 0x46) != 0)
        Func_8003f3c(*(unsigned short *)(s + 0x48));
    off = 0x352;
    q = (unsigned short *)(s + off);
    if (*q != 0) {
        off += 2;
        q = (unsigned short *)(s + off);
        Func_8003f3c(*q);
    }
    gfree(0x13);
}

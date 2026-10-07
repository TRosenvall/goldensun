/* Func_801e3c8 -- asm/rom_15000/rom_1de5c_a_c_c.s
 *
 * NON-MATCHING, 25 of 38 encodings  (MEASURED, batch 333 brief E).
 *   COUNT DIFFERS (instructions ref 36, ours 35) -- the two STREAMS are both 38
 *   entries only because the reference spends its spare halfword on a pool and
 *   we spend ours on a pad, so this positional figure still counts PHASE.
 *   Aligned-equal 71.1 per cent of the reference stream, eight hunks.
 *   The earlier figure under the same recipe was twenty-nine.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801e3c8.c \
 *     asm/rom_15000/rom_1de5c_a_c_c.s --func Func_801e3c8
 *
 * Sets a flag at +0xea2 and clears 128 bytes at +0xe20. Both arms do the same
 * 128 stores, and THE ROM WRITES THEM WITH DIFFERENT LOOP FORMS:
 *
 *     on != 0 arm   r3 = 0x80;  do { r3++; *q++ = 0; } while (r3 <= 0xff);
 *     on == 0 arm   r3 = 0x7f;  do { r3--; *q++ = 0; } while (r3 >= 0);
 *
 * THE COUNTER-DIRECTION BLOCKER IS SOLVED, AND THE PARK'S DIAGNOSIS OF IT WAS
 * WRONG.  The park said gcc normalises a dead-counter loop to count down, that
 * no `for` spelling prevents it, and that the honest next step was to find a
 * USE for the first arm's counter in the caller -- i.e. that the ROM's source
 * must keep the counter live.  IT DOES NOT NEED TO.  A `goto` loop disables loop
 * optimisation outright, so the induction variable is never reversed and the
 * count-up form survives with a dead counter.  That lever is recorded in
 * src/non_matching/ovl_78c76c/2008098.c, a park in the same tree that had
 * already measured it on the same problem; it cost one `cat` to find and this
 * park spent a batch reasoning around it.  Both arms now reproduce their own
 * direction, bottom-tested, with the ROM's compare and branch.
 *
 * THE SECOND LEVER IS NAMING THE FILL VALUE.  With the goto loops alone the body
 * reads 47.4 per cent aligned; giving the stored zero its own local (`z`) and
 * writing `*q = z` takes it to 71.1.  gcc then materialises the zero once before
 * the loop, which is where the ROM has it, instead of folding it into the store.
 *
 * WHAT IS LEFT -- ONE INSTRUCTION, AND IT IS IN THE SECOND ARM'S FLAG STORE.
 * The ROM builds a FRESH zero for it (`mov r3, #0` then `strb r3, [r2]`); we
 * emit the `strb` alone, reusing the register that still holds the incoming
 * parameter, because `on == 0` is known on that path -- cse's record_jump_equiv
 * puts the parameter register in the same equivalence class as the constant
 * after the branch, and the store then costs nothing.  The first arm cannot show
 * this because its flag value is 1, so nothing is available to reuse.  THE TELL
 * IS GENERAL: where an arm of an `if (x != 0)` stores the very value x was
 * tested against, gcc will spend no instruction on it and the ROM may still have
 * one.
 *
 * The other visible residue is ordering, not length: the ROM materialises the
 * 0xe20 offset into its own scratch register BEFORE the flag store, so sched2
 * can slot it into the shadow of the pool load, and forms the pointer after.
 * Ours computes it after the flag store and reuses the register the pool load
 * freed, which leaves nothing for sched2 to hoist.
 *
 * MEASURED, aligned-equal as a percentage of the reference stream:
 *
 *     both arms as the ROM has them, `do/while`          50.0  (the old body)
 *     goto loops in both arms                            47.4
 *     goto loop in the first arm only                    50.0
 *     goto loops + the fill value named                  71.1  <- this file
 *     ... + the offset named and assigned first          71.1  (inert)
 *     ... + the pointer formed before the flag store     44.7
 *     ... + the flag value named too                     31.6  (an insn shorter)
 *     ... + an early return instead of an else           71.1  (inert)
 *     ... + the flag address named                       47.4
 *
 * NEXT: the second arm's fresh zero.  Every spelling that separates it from the
 * fill value also separates it from the parameter, and gcc takes the parameter.
 */
extern int iwram_3001e8c;

void Func_801e3c8(int on)
{
    char *p;
    char *q;
    int i;
    int z;

    p = (char *)iwram_3001e8c;
    if (on != 0) {
        *(unsigned char *)(p + 0xea2) = 1;
        z = 0;
        q = p + 0xe2 * 16;
        i = 0x80;
    loop_up:
        i++;
        *q = z;
        q++;
        if (i <= 0xff)
            goto loop_up;
    } else {
        *(unsigned char *)(p + 0xea2) = 0;
        z = 0;
        q = p + 0xe2 * 16;
        i = 0x7f;
    loop_down:
        i--;
        *q = z;
        q++;
        if (i >= 0)
            goto loop_down;
    }
}

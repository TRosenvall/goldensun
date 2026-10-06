/* asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_c_a_a_b.s -- Func_807a458 (0x0807a458).
 *
 * EXACT.  objcmp --func Func_807a458: 64 bytes, 30 encodings and 2 relocations
 * identical.  PINS: 0.  SHIMS: 0.  No device, no flag group -- production flags.
 * tools/datacheck.py prints nothing for the parent .s (text only, no data
 * section), so no export is needed, but the parent holds a SECOND function
 * (Func_807a3a8, 0x0807a3a8, still parked) so the landing DOES need a split.
 *
 * Split shape (tools/split_s.py --dry-run on
 * asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_c_a_a.s with Func_807a458):
 *     writes ..._a_a_a.s   (1 function, 99 lines -- Func_807a3a8, stays asm)
 *     writes ..._a_a_b.s   (1 function, 35 lines -- the target)
 *     removes ..._a_a.s, rewrites stage1.ld
 * The target is the SECOND function in the piece, so this .c installs as
 * src/rom_77000/rom_79460_c_c_c_c_a_c_c_c_c_a_a_b.c.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_77000/rom_79460_c_c_c_c_a_c_c_c_c_a_a_b.c asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_c_a_a_b.s --func Func_807a458
 *
 * ===========================================================================
 * THREE INDEPENDENT DIMENSIONS, PERFECTLY ADDITIVE.  THE PARK HAD VARIED NONE.
 *
 * The park sat at twenty differing of thirty for four batches and had correctly
 * read it as MISALIGNMENT -- we were TWO INSTRUCTIONS SHORT (ref 27, ours 25).
 * Its diagnosis was "two address forms collapsed into one": the ROM makes the
 * first of the four byte stores REGISTER-OFFSET (`strb r6, [r2, r3]`) and then
 * advances, where we fold the index in once and use immediate offsets
 * throughout.  That observation is exactly right.  Its verdict -- "gcc picks one
 * addressing form per address expression and reuses it ... not reachable by
 * naming or ordering here", with the same wall asserted for
 * src/non_matching/rom_f6000/80f7f30.c -- is REFUTED.  It is reachable by
 * naming AND ordering, and by one more thing the park never looked at.
 *
 * The park's two negatives ("the index named in its own local, assigned after
 * `e`" and "named and assigned BEFORE `e`") were both measured with `e` formed
 * AFTER `cnt`, which is the dimension that actually carries the two missing
 * instructions.  Crossed, the three levers are strictly additive:
 *
 *   int return | `e = p + 8` before `cnt` | `o = n * 4` named | insns | differ
 *   -----------+--------------------------+-------------------+-------+-------
 *   no         | no   (the park's body)   | no                | 25    | twenty
 *   no         | no                       | YES               | 25    | twenty
 *   YES        | no                       | no                | 25    | twenty
 *   YES        | no                       | YES               | 25    | twenty
 *   no         | YES                      | no                | 27    | eight
 *   YES        | YES                      | no                | 27    | six
 *   no         | YES                      | YES               | 27    | two
 *   YES        | YES                      | YES               | 27    | ZERO
 *
 * (All sixteen figures above were measured with tools/objcmp.py --func on the
 * reference .s; the three "twenty" rows are byte-identical to each other.)
 *
 * ---------------------------------------------------------------------------
 * LEVER 1 -- `e = p + 8` BEFORE `cnt`, which is worth BOTH missing instructions
 *
 * Both of them are register COPIES, and the second is forced by Thumb-1's
 * encoding once the first is paid for:
 *
 *     mov r3, r0      <- the table pointer copied out of the return register
 *     mov r2, r3      <- and copied again to form `e`
 *     add r2, #8
 *
 * `add Rd, Rn, #imm` in Thumb-1 is the 3-bit-immediate form, so `add r2, r3, #8`
 * CANNOT BE ENCODED (8 > 7); forming `e` in a register other than `p`'s
 * therefore costs mov+add, where gcc's default `add r0, r0, #8` is the 8-bit
 * two-address form and costs one.  With `e` formed before `cnt`, `p` is still
 * needed for `cnt = p + 0x108` after `e` exists, so `p` and `e` conflict, `e`
 * gets its own register, and both copies appear.  With `e` formed after `cnt`,
 * `e = p + 8` is `p`'s last use and gcc overwrites `p` in place.
 *
 * This is why none of the park's probes moved: every one of them varied the
 * STORES, and the length is decided at the two pointer FORMATIONS above them.
 * The same shape also lands with `cnt` derived from `e` (`(int *)(e + 0x100)`)
 * rather than from `p`, measured at eight differing -- it is the ORDER that
 * matters, not which pointer the offset hangs off.
 *
 * LEVER 2 -- the index named, worth six of the remaining eight
 *
 * With `o = n * 4` named and used for both the store and the advance, the first
 * store stays `strb r6, [r2, r3]` and the advance stays `add r2, r3`; written
 * inline twice, gcc folds `e + n * 4` into one register ahead of the stores and
 * every store becomes immediate-offset.  Naming it is NOT inert here, contrary
 * to the park -- it is inert only while lever 1 is absent, which is the shape
 * the park measured it in.
 *
 * LEVER 3 -- THE FUNCTION IS NOT `void`, AND ITS EPILOGUE SAYS SO
 *
 * This is the generally reusable finding.  The last two differences were the
 * epilogue: the ROM pops into r1 (`pop {r1} / bx r1`) and a void body pops into
 * r0.  That register is NOT an allocation accident -- gcc derives it from the
 * DECLARED RETURN SIZE, in `thumb_exit` (gcc/config/arm/arm.c:8215).  At :8302
 * it takes `mode = DECL_MODE (DECL_RESULT (current_function_decl))` and
 * `size = GET_MODE_SIZE (mode)`, and then:
 *
 *   :8306-8319  size == 0 and VOIDmode -> {ARG_REGISTER(1..3)} = r0, r1, r2
 *   :8320-8323  size == 0, not VOIDmode (struct returned on the stack)
 *                                       -> {ARG_REGISTER(2,3)}  = r1, r2
 *   :8324-8327  size <= 4               -> {ARG_REGISTER(2,3)}  = r1, r2
 *   :8328-8330  size <= 8               -> {ARG_REGISTER(3)}    = r2
 *
 * (`ARG_REGISTER(N)` is `N - 1`, gcc/config/arm/arm.h:861.)  At :8382 the return
 * address is popped into `number_of_first_bit_set (regs_available_for_popping)`,
 * the LOWEST member of that set.  So r0 is excluded from the set exactly when it
 * holds a return value, and:
 *
 *     pop {r0} / bx r0  <=>  the function was declared void
 *     pop {r1} / bx r1  <=>  it returns one to four bytes
 *     pop {r2} / bx r2  <=>  it returns five to eight bytes
 *
 * This path is reached only for an interworking, backtrace or ARM-mode-entry
 * function (the `else if` at :8265 returns with a plain `pop {pc}` otherwise),
 * and this build is `-mthumb-interwork`, so it applies to every thumb function
 * in the ROM that ends in `pop {lo} / bx lo`.  THE EPILOGUE REGISTER IS A FREE
 * READ ON THE ORIGINAL'S RETURN TYPE, available from the reference .s before any
 * C is written.
 *
 * Here the ROM says "one to four bytes" and the body never computes a value, so
 * the original was declared returning `int` and FELL OFF THE END -- which is
 * what ships.  Corroborated independently: src/non_matching/rom_a1000/80aa768.c
 * :321 declares `extern int _Func_807a458(int, int, int);` and :525 USES the
 * result (`n = _Func_807a458(...)`), and src/non_matching/rom_77000/807a0f4.c
 * :223 declares it `int` and records that doing so is load-bearing for its own
 * argument fill order.  (PASS-4 NOTE, no figure attached: the landed
 * src/rom_77000/rom_79460_c_c_c_c_a_c_c_c_c_a_b.c:40 and
 * src/rom_b5000/rom_b5a0c_c_a.c:126 declare it `void` and
 * src/non_matching/ovl_77a7c8/2009ca4.c:146 does too; those are per-TU externs
 * that discard the result, so they are a naming inconsistency to tidy, not a
 * figure.)
 */
extern void Func_807a3a8(void);
extern char *Func_8077330(int n);

int Func_807a458(int a, int b, int c)
{
    char *p;
    int *cnt;
    int n;
    char *e;
    int o;

    Func_807a3a8();
    p = Func_8077330((unsigned int)a <= 7 ? 0 : 1);
    e = p + 8;
    cnt = (int *)(p + 0x84 * 2);
    n = *cnt;
    o = n * 4;
    e[o] = b;
    e += o;
    e[1] = c;
    e[2] = a;
    e[3] = 0xff;
    *cnt = n + 1;
}

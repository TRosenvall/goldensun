/* Func_801e3c8 -- asm/rom_15000/rom_1de5c_a_c_c.s
 *
 * MATCHING, 0 of 38 encodings  (MEASURED, batch 342 brief A).
 *   Instruction counts EQUAL at 36/36; relocations ok; size ok.
 *   The park's figure at the head of this batch was 25 of 38 with
 *   instructions ref 36 / ours 35, i.e. PHASE with ours ONE short.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_1de5c_a_c_c.c \
 *     asm/rom_15000/rom_1de5c_a_c_c.s --func Func_801e3c8
 *
 * PIECE: tools/piece.py reports ONE function in rom_1de5c_a_c_c.s, so this is
 * a WHOLE-FILE conversion -- no tools/split_s.py cut is needed.
 * PINS: tools/shimcount.py reports 0.  Device-free: the one `do { } while (0)`
 * is an anchor, which shimcount does not count and which carries no pass-3
 * depin debt (docs/elevation.md, "THE ANCHOR IS NOT A STRONGER BARRIER").
 *
 * HOW THE LAST 25 CLOSED -- TWO LEVERS, AND THE PARK POINTED AT THE WRONG ARM.
 *
 * Region counts first (batch 339 brief C's rule, and it was decisive):
 *   prologue 5/5, arm 1 15/15, arm 2 13/14, epilogue 2/2.
 * The entire one-instruction deficit was in ARM 2's pre-loop region, and it is
 * the instruction the park names: the ROM's `mov r3, #0` ahead of
 * `strb r3, [r2]`, where we emitted the `strb` alone against the incoming
 * parameter register, which cse knows is zero on that path.
 *
 * LEVER 1 -- THE ANCHOR GOES IN THE *OTHER* ARM.  The park's closing line read
 * "NEXT: the second arm's fresh zero.  Every spelling that separates it from
 * the fill value also separates it from the parameter, and gcc takes the
 * parameter."  That is true of every spelling IN ARM 2 and it is the wrong
 * place to look.  `do { } while (0);` at the END OF ARM 1 fixes TWO independent
 * things at once and equalises the counts at 36/36:
 *   - arm 1's loop exit becomes the ROM's `ble .loop / b .end` in place of our
 *     inverted `bgt .end / b .loop`;
 *   - arm 2's flag store gains its own `mov r3, #0`.
 * An anchor emits loop notes which hang on the FOLLOWING insn
 * (haifa-sched.c:3735-3736) and INSN_PRIORITY is computed over the whole block,
 * so a fence at the tail of arm 1 reorders arm 1's exit; the knock-on in arm 2
 * is through the jump reorganisation that follows.  This is batch 339's
 * `2008d24` finding ("an anchor DOWNSTREAM of the residue fixed the residue")
 * one step stronger: here it is not downstream, it is IN THE SIBLING ARM.
 * THE ANCHOR IS NOT A STAND-IN FOR A CONTROL-FLOW SHAPE -- measured, all three
 * shapes read 17 of 38 without it and 0 with it: `if`/`else` (this file),
 * `if (on == 0) goto other; ... goto done;` and an early `return` in arm 1.
 *
 * LEVER 2 -- THE TWO ARMS OF THE `if`/`else` NEED *DIFFERENT* STATEMENT ORDERS.
 * At 2 of 38 the only residue left was arm 2's `mov r3, #0x7f` one slot early.
 * A 484-row cross of arm-1 order x arm-2 order x anchor-in-arm-1 x
 * anchor-in-arm-2 gives 18 byte-identical rows, and the rule is sharp:
 *     arm 1 must define `i` BEFORE `q`   (F Z I Q, F I Z Q, Z F I Q all work)
 *     arm 2 must define `q` BEFORE `i`   (F Z Q I, F Q Z I, F Q I Z all work)
 *     the anchor must sit at the end of arm 1; in arm 2 it is irrelevant
 * Every SYMMETRIC spelling reads 2 or worse.  The park had only ever written
 * the arms symmetrically, which is why its own table has nothing between 71.1
 * per cent aligned and a match.  Where two arms do the same work in two loop
 * directions, do not assume the source was symmetric.
 *
 * CARRIED FORWARD FROM THE PARK, both still correct and both still needed:
 *   the goto loops (a goto loop disables loop optimisation, so the count-up
 *     induction variable is never reversed -- the lever recorded in
 *     src/non_matching/ovl_78c76c/2008098.c);
 *   the fill value named `z` (gcc materialises the zero once before the loop,
 *     where the ROM has it, instead of folding it into the store).
 *
 * AUTOSWEEP (batch 342): flag axis all inert except -O1 at 17 of 38; anchor
 * axis 18 of 18 boundaries measured, four better, best `anchor09` at 15 -- that
 * row IS lever 1 and the tool found it before any hypothesis was formed.
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
        i = 0x80;
        q = p + 0xe2 * 16;
    loop_up:
        i++;
        *q = z;
        q++;
        if (i <= 0xff)
            goto loop_up;
        do { } while (0);
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

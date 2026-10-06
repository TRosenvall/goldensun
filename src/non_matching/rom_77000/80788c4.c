/* Func_80788c4 (0x080788c4) -- NON-MATCHING.
 *
 * NON-MATCHING, 14 of 62 encodings  (MEASURED, batch 332 D).
 *   SIZE EXACT: 132 bytes both sides, 62 encodings both sides, 57 instructions
 *   both sides.  Relocations identical.  So this figure IS A DISTANCE; the
 *   park's older claim was not, and is retracted below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/80788c4.c \
 *     asm/rom_77000/rom_78414_c_c_a_c_a_c_c.s --func Func_80788c4
 *
 * PINS: 0.  No shim, no fakematch row, no flag group.  No device.
 * PIECE: shares asm/rom_77000/rom_78414_c_c_a_c_a_c_c.s with Func_8078870,
 * which is held at five.  Both have to land before the piece can convert, and
 * at these two figures a split is not worth taking.
 *
 * ---------------------------------------------------------------------------
 * THE SUPERSEDED CLAIM AND WHAT REPLACED IT
 *
 * This park used to claim thirty-nine differing of sixty-two with the
 * reference one instruction LONGER, so that count was comparing two streams
 * out of phase and was measuring the offset, not the decisions.  The one
 * missing instruction was the compaction loop's widening shift, exactly where
 * the park said it was:
 *
 *     rom    ldrh r2, [r4] / lsl r3, r2, #16 / add r4, #2 / cmp r3, #0
 *     old    ldrh r3, [r1] / add r1, #2 / cmp r3, #0
 *
 * The park's verdict was that gcc knows the loaded value's range and folds a
 * narrowing shift before a zero test, that this is the same wall as
 * rom_15000/801f730.c one width up, and that there is therefore nothing
 * source-level.  THE FOLD IS REAL AND THE BOUND IS NOT.
 *
 * THE FOLD, WITH ITS GATE NAMED.  combine.c:10749-10774 is simplify_comparison's
 * ASHIFT case: a comparison of a left shift against zero becomes a comparison
 * of the shift's operand only when nonzero_bits of that operand fits the
 * shifted mask (the test is at :10763-10765).  A halfword load zero-extends,
 * so for any value living in an `int` gcc can always prove the fit and the
 * shift always dies.  Confirmed here against the compiler, EXACTLY INERT at
 * the old body's length, every one of them:
 *
 *     a cast of the int to unsigned short before the test        INERT
 *     a cast to short, and to signed short                       INERT
 *     either cast used as the whole condition                    INERT
 *     an explicit shift left by sixteen as the condition         INERT
 *     that shift compared against zero                           INERT
 *     a multiply by the same power of two                        INERT
 *     a mask with the low sixteen bits                           INERT
 *
 * So the park was right that no SPELLING OF THE TEST defeats the fold, and
 * wrong that nothing does.  What defeats it is THE TYPE OF THE HOLDER, because
 * nonzero_bits is a property of the value's definition, not of the comparison.
 *
 * TWO COMPILER FACTS DECIDE IT, AND THEY PULL AGAINST EACH OTHER.
 *
 *   * arm.h:597-606 defines PROMOTE_MODE, and for HImode it sets the
 *     unsignedness from TARGET_MMU_TRAPS, which this object does not enable.
 *     So EVERY DECLARED HALFWORD LOCAL is promoted to a word with a SIGN
 *     extension regardless of how it was declared.  That does produce the
 *     shift -- but the load becomes a sign-extending one, and Thumb-1's
 *     sign-extending halfword load has no immediate-offset form, so
 *     arm.md:3239 with :3289 emits a scratch move alongside it.  Net cost one
 *     instruction, so a halfword local lands one instruction LONG rather than
 *     one short.  Measured: a halfword local reads the reference's length plus
 *     one, in five spellings.
 *
 *   * explow.c:896-913 is promote_mode, and only the scalar type codes reach
 *     PROMOTE_MODE at :898-901; RECORD, UNION and ARRAY fall out of the
 *     default arm at :912 UNPROMOTED.  So a one-member aggregate holds the
 *     halfword in a pseudo of its own width, which is the thing a declared
 *     local cannot be.
 *
 * AND THE FIELD'S SIGNEDNESS DECIDES WHICH WAY COMBINE FOLDS.  With an
 * unsigned field the widening pair is a zero extension, combine collapses it
 * to a bare subreg copy and the reference's shift becomes a register move --
 * right length, wrong opcode.  With a SIGNED field the pair is a sign
 * extension, which does not collapse, so combine cannot merge the two insns
 * and instead reaches the comparison, where the ASHIFT gate above fails and
 * only the RIGHT shift is dropped.  What survives is the reference's
 * `ldrh / lsl #16 / add / cmp` byte for byte, including the pointer increment
 * sitting between the shift and the compare.
 *
 * The aggregate is doing real work and is not a device -- its member is read
 * and written and nothing fictitious ships -- but it IS the load-bearing part
 * of this body, and anyone landing this function should say so.  Note also
 * that the structure size boundary is a word by default (arm.h:686-701, and
 * only a command-line option moves it), so the aggregate must be used as a
 * HOLDER ONLY: the cursors have to stay halfword pointers or the stride
 * becomes four and the program is wrong.  That mistake measures the right
 * length and is worth knowing about.
 *
 * ---------------------------------------------------------------------------
 * WHAT THE REMAINING FOURTEEN ARE
 *
 *   (a) a src-against-counter register swap in the compaction loop.  The
 *       reference holds the source cursor in r4 and the countdown in r0; we
 *       hold them the other way round.  The destination cursor and the kept
 *       count already match.
 *   (b) the same rotation through the tail-zeroing loop, plus one opcode: the
 *       reference brings its zero in through a word-sized pool load and we
 *       bring it in through a halfword one.  arm.md:3037-3038 and :3258 both
 *       return the word load for a label address, so the reference's zero
 *       reaches its register as a WIDENED halfword constant; ours is a plain
 *       halfword move, and arm.md:4348 returns the halfword load there with no
 *       choice in it.  Same length, one differing encoding, and it is a
 *       separate question from (a).
 *
 * MEASURED IN BATCH 332 D, all at the reference's length unless marked:
 *   14   THIS BODY -- the four cursor inits as count, source, destination,
 *        counter
 *   17   the same four as source, count, destination, counter
 *   18   fourteen other orderings of those four
 *   20   the remaining six orderings
 *   14   all 24 permutations of the four pointer declarations -- EXACTLY FLAT
 *   14   all 720 permutations of the six integer declarations -- EXACTLY FLAT
 *   18   the aggregate's field left unsigned (the collapse described above)
 *   +1   a halfword local instead of the aggregate, five spellings, all one
 *        instruction LONG
 *
 * The two declaration sweeps are 744 rows and they are the useful negative:
 * declaration order is not a dimension in this function at all, and the
 * initialisation order is worth three.
 *
 * STILL RIGHT AND KEPT, from the park: the quantity decrement through the
 * packed high bits, the register-offset load and store at the slot's byte
 * offset, the three return values reaching one shared CalcStats tail through a
 * single result variable, the fifteen-iteration compaction with separate
 * cursors, and the tail-zeroing loop's counted form.
 *
 * NEXT: (a) is a live-range question of the same shape as the one in
 * src/non_matching/rom_77000/8079664.c -- see that park's note on
 * global.c:598-621, which is the sort that decides both.  Do not spend a round
 * on declaration order; that product is exhausted above.
 */
extern unsigned char *GetUnit(int who);
extern void CalcStats(int who);

struct Held { short v; };

int Func_80788c4(int who, int slot)
{
    unsigned char *u;
    unsigned short *base;
    unsigned short *src;
    unsigned short *dst;
    unsigned short *p;
    struct Held w;
    int off;
    int ret;
    int v;
    int n;
    int i;
    int k;

    u = GetUnit(who);
    off = slot * 2 + 0xd8;
    v = *(unsigned short *)(u + off);
    ret = -1;
    if (v != 0) {
        if ((v & 0xf800) != 0) {
            *(unsigned short *)(u + off) = v - 0x800;
            ret = 1;
        } else {
            base = (unsigned short *)(u + 0xd8);
            *(unsigned short *)(u + off) = 0;
            n = 0;
            src = base;
            dst = base;
            i = 0xe;
            do {
                w.v = *src++;
                if (w.v != 0) {
                    *dst++ = w.v;
                    n++;
                }
                i--;
            } while (i >= 0);
            if (n <= 0xe) {
                p = base + n;
                k = 0xf - n;
                do {
                    k--;
                    *p++ = 0;
                } while (k != 0);
            }
            ret = 2;
        }
    }
    CalcStats(who);
    return ret;
}

/* Func_80216b4 -- PARK AT 7 of 24 stream encodings.  UNCHANGED FIGURE.
 * PIN-FREE, DEVICE-FREE.  Body below is the park's body, unedited.
 *
 * MEASURED BATCH 350, BRIEF D (not inherited): 7 differing encodings of 24
 * (ref 24 / ours 24), 22 INSTRUCTIONS against 22, 52 bytes against 52, 2
 * relocations identical, first differing index 9 (ref 18d2 / ours 189b).
 * IN PHASE on all three length dimensions, so the figure IS a distance.
 * `--whole` agrees.  PINS 0.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" \
 *   -w /work goldensun-build python3 tools/objcmp.py \
 *   src/non_matching/rom_15000/80216b4.c \
 *   asm/rom_15000/rom_20198_c_c_c_a_a_c_a_a_c.s --func Func_80216b4
 *
 * SPLIT SHAPE: NONE.  One `.thumb_func_start`, no data, `datacheck.py` silent
 * (exit 0); a landing is a whole-file conversion to
 * src/rom_15000/rom_20198_c_c_c_a_a_c_a_a_c.c.  (Unchanged.)
 *
 * ===== FOUR RECORDED "EXACTLY INERT" ROWS ARE BIT-IDENTICAL TO BASE =====
 * Re-swept with tools/sweep_variants.py's object fingerprinting.  These four are
 * the SAME OBJECT as the body below, so they measured NOTHING and the levers
 * they stand for are ABSENT, not merely inert:
 *     `volatile unsigned int *w'       (b330 AND b337 both record it inert)
 *     `x' as `unsigned int'
 *     half 1 written `tb[v] + x'       (b337 recorded "byte-identical", correct)
 *     a second table pointer `tb2' used by half 2 only        (new this batch)
 * So no spelling of the read's volatility, of x's signedness, or of half 1's
 * operand order reaches the back end on this body.
 *
 * ===== THE REFERENCE STREAM, DECODED -- THIS SETTLES THE COUPLING =====
 *    1 push {r5, lr}
 *    2 ldr  r4, =iwram_3001800     w  -> r4
 *    3 ldr  r3, [r4]               *w, FIRST read
 *    4 ldr  r5, =.L37226           tb -> r5
 *    5 mov  r1, #7                 THE MASK, IN r1, LOADED ONCE
 *    6 lsr  r3, #2
 *    7 and  r3, r1                 v1
 *    8 ldrb r2, [r0, #8]           x       -> r2
 *    9 ldrb r3, [r5, r3]           tb[v1]  -> r3
 *   10 add  r2, r3                 sum -> r2, accumulating into x's register
 *   11 ldr  r3, [r4]               *w, SECOND read -- BETWEEN THE ADD AND THE STORE
 *   12 strb r2, [r0, #0x14]
 *   13 lsr  r3, #2
 *   14 ldr  r0, [r0]               a = *(unsigned char **)a
 *   15 and  r3, r1
 *   16 ldrb r2, [r0, #8]
 *   17 ldrb r3, [r5, r3]
 *   18 add  r2, r3
 *   19 strb r2, [r0, #0x14]
 *
 * TWO FACTS WORTH HAVING WRITTEN DOWN.  (a) THE MASK 7 LIVES IN r1 ACROSS BOTH
 * HALVES -- one `mov r1, #7' serves lines 7 and 15 -- which is why the literal
 * `& 7' spelling is load-bearing and why the `m = 7' local was worse.  (b) BOTH
 * index-9 encodings are Thumb format-1 ADD with Rd == Rn: ref 18d2 is
 * `adds r2, r2, r3' and ours 189b is `adds r3, r3, r2'.  So `regmove' tied the
 * sum to a DYING OPERAND IN BOTH STREAMS -- the ROM to `x' (r2), ours to `tb[v]'
 * (r3).  The only difference is which register was free, and r3 is not free in
 * the ROM precisely because line 11 writes it.  *** INDEX 9 AND INDICES 10-13
 * ARE ONE FACT, as batch 337 said; this is the independent confirmation. ***
 *
 * MEASURED THIS BATCH (10 spellings, SEVEN DISTINCT PROGRAMS):
 *     base                                                       7
 *     half 1 operands reversed                                   7  BIT-IDENTICAL
 *     `x' as unsigned int                                        7  BIT-IDENTICAL
 *     `volatile unsigned int *w'                                 7  BIT-IDENTICAL
 *     a second table pointer tb2 for half 2                      7  BIT-IDENTICAL
 *     `x' reused in half 2 so it outlives the add                9  worse
 *     second masked read v2 AFTER the store, volatile           10  worse
 *     the sum named in a local `s'                               12  whole-allocation
 *                                       rotation, first diff moves to index 2
 *     the same + `do { } while (0);' between add and store      12  distinct program,
 *                                       no gain.  THIS IS THE ONE PLACEMENT THE
 *                                       PARK NEVER TRIED (it tried a bare
 *                                       __asm__ volatile ("") there: 18 / 10).
 *     second masked read v2 BETWEEN add and store, volatile     15  worse
 *     `v2' before the store, NON-volatile    24 at dsize -12, RELOCDIFF -- a
 *                                       WRONG PROGRAM, not a figure
 *
 * STILL OPEN, UNCHANGED IN SHAPE.  Get the second `*w' load to be a real insn
 * between the first add and the first store at no extra quantity.  It is
 * circular at source level: the load cannot be hoisted over the `strb' (alias
 * set 0, and `true_dependence''s QImode escape fires before anything else can
 * help), and with the struct alias escape that breaks the MEMORY dependence the
 * REGISTER anti-dependence survives because the sum is in r3.  The park's own
 * named untried shape -- a struct whose 0x14 member keeps its member type
 * through a cast, leaving the `ldrb'/`strb' widths and the offsets 8, 0x14 and 0
 * intact -- is STILL UNTRIED and is the right next move.
 * The device instrument reaching 4 (struct store + volatile w +
 * `register int x __asm__("r2")') is b337's and is NOT re-proposed: it is a
 * figure about the blocker.  Device-free the park is 7.
 */
extern unsigned int iwram_3001800;
extern unsigned char L37226[] __asm__(".L37226");

void Func_80216b4(unsigned char *a)
{
    unsigned int *w;
    unsigned char *tb;
    unsigned int v;
    int x;

    w = &iwram_3001800;
    tb = L37226;
    v = (*w >> 2) & 7;
    x = a[8];
    a[0x14] = x + tb[v];
    a = *(unsigned char **)a;
    v = (*w >> 2) & 7;
    a[0x14] = a[8] + tb[v];
}

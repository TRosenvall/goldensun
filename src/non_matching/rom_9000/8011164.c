/* Func_8011164 (0x08011164) -- NON-MATCHING.
 *
 * NON-MATCHING, 27 of 37 encodings  (RE-MEASURED batch 324, brief G; the
 *   batch-319 figure reproduces exactly).
 *   COUNT DIFFERS (ref 37, ours 35) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  The honest statement is WE ARE TWO
 *   INSTRUCTIONS SHORT.  Read the count before the figure.
 *   SIZE ref 80 bytes, ours 76.
 *   RELOCATIONS differ, same symbols at a shifted offset -- a consequence of
 *   the length difference (batch-322 reclassification).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/8011164.c \
 *     asm/rom_9000/rom_108e4_c.s --func Func_8011164
 *
 * NOTE FOR COORDINATORS: `src/non_matching/rom_9000/80111b4.c` is NOT a second
 * park for this function.  It is the park for Func_80111b4, a 248-encoding
 * camera update that merely CALLS this one (declared `extern` at its line 164);
 * both functions happen to live in this same reference .s.  It must never
 * appear in this function's `parks_retire`.
 *
 * ==== THE PREVIOUS DIAGNOSIS IS REFUTED IN ITS VERDICT ====
 *
 * It read: "gcc HOISTS a pool load the original build reloaded every
 * iteration ... the motion ... belongs to loop.c's invariant hoisting, which
 * gcc-2.96 has no flag for.  NEXT: nothing."
 *
 * The OBSERVATION is right and the VERDICT is wrong.  loop.c does hoist, and
 * the original build hoisted too.  The hoist is not the difference.  Traced
 * with `-da` (dumps kept under scratch_elev/b324/G/d_y01):
 *
 *  1. `u.c.00.rtl` -- expand emits TWO SEPARATE POOL LOADS:
 *       (set 52 (mem (symbol_ref "*.LC1")))  REG_EQUAL (symbol_ref "gBuffer")
 *       (set 57 (mem (symbol_ref "*.LC2")))  REG_EQUAL (const (plus gBuffer 2))
 *  2. `u.c.03.cse` -- cse has already rewritten the second to
 *       (set 57 (plus 52 (const_int 2)))
 *     This is `cse.c:2074 use_related_value`, and it is reachable because
 *     `rtlanal.c:234 get_related_value` DOES return the base for a
 *     `CONST (PLUS symbol_ref CONST_INT)`.  It returns 0 only for a non-CONST.
 *     *** So the standing note "cse cannot derive one constant from another"
 *     needs qualifying: it cannot for a bare symbol_ref, and it CAN for
 *     symbol+N. ***  The consequence that matters here: reg 52 now has THREE
 *     refs instead of two.
 *  3. `u.c.08.loop` names both hoists:
 *       Insn 61: regno 52 (life 6), move-insn savings 2  moved to 132
 *       Insn 73: regno 57 (life 1), move-insn forces 61 savings 1  moved to 134
 *     The gate (`loop.c:1803`) is threshold*savings*lifetime >= insn_count with
 *     threshold = (has_call ? 1 : 2) * (1 + n_non_fixed_regs) (`loop.c:651`) --
 *     far too generous to be the discriminator.
 *  4. `u.c.18.greg` -- ";; 5 regs to allocate: 43 33 47 52 57" (N > 0, so the
 *     GLOBAL priorities are the ones in force and they are exact), dispositions
 *     "52 in 6  57 in 7".  BOTH CONSTANTS GOT HARD REGISTERS.  That, not the
 *     hoist, is the whole difference from the ROM.
 *
 * ==== WHAT THE ROM'S BYTES ACTUALLY REQUIRE ====
 *
 * The ROM's `add r3, r2, r5 / ldrh r3, [r3]` is not a legitimate-address
 * failure: `arm.h:2101 THUMB_GO_IF_LEGITIMATE_ADDRESS` accepts REG+REG for any
 * mode of size <= 4, which is exactly why ours folds to `ldrh r3,[r6,r2]`.  A
 * separate address `add` is what RELOAD emits when the base is a
 * reg_equiv_constant with no hard register: the MEM becomes
 * `(plus (reg t) (const symbol))`, which is not a legitimate address, so reload
 * loads the symbol and adds.  `reload_cse_move2add` (`reload1.c:8840`) then
 * collapses the second rematerialised `ldr r5,=gBuffer+2` into `add r5,#2`,
 * because both loads land in the SAME hard register -- its requirement.
 *
 * ONE mechanism therefore accounts for ALL THREE missing instructions, for the
 * single pool word, and for `add r5,#2`: the two gBuffer constants must get NO
 * hard register.
 *
 * ==== BOUND (read from source, with its evidence) ====
 *
 * The cheap route to that state is CLOSED.  `local-alloc.c:886` sets
 * `reg_equiv_replace` -- delete the init and let reload rematerialise -- only
 * when
 *     REG_N_REFS (regno) == 2  &&  REG_BASIC_BLOCK (regno) < 0
 *     &&  rtx_equal_p (XEXP (note, 0), SET_SRC (set))
 * On thumb a symbol constant's SET_SRC is a POOL-LOAD MEM, `(mem (symbol_ref
 * "*.LC1"))`, while the note is the bare `(symbol_ref "gBuffer")`.  rtx_equal_p
 * is false for EVERY thumb symbol constant, so that path can never fire in this
 * port.  (`local-alloc.c:871` REG_LIVE_LENGTH *= 2 still applies, which halves
 * the constant's GLOBAL priority since `global.c:607` divides by live_length --
 * so it is already the cheapest allocno to lose.  It just never loses while a
 * register is free.)
 *
 * ==== THE REMAINING CAUSE, NAMED ====
 *
 * greg has a free register and the original build did not.  INSTRUMENTED (these
 * are figures ABOUT THE BLOCKER, not production figures):
 *
 *   body                       prod      -ffixed-r7     -ffixed-r6 -ffixed-r7
 *   this park                  27, -4    24, 0 bytes    32, +8
 *   single `q` base, q[0]/q[1] 27, -4    27, -4         24, 0 bytes, RELOCS OK
 *
 * One register fewer closes the length gap exactly.  It is still not the ROM:
 * under -ffixed-r7 gcc parks gBuffer+2 in `ip` (`mov ip,r3` / `mov r3,ip`)
 * rather than rematerialising.  What the instrument establishes is that the
 * residue is an AVAILABLE-REGISTER COUNT, not loop.c.
 *
 * NEXT MOVE (replacing the old "nothing"): find a source shape that either
 * keeps one more value genuinely live across the loop body, or gives the two
 * gBuffer references two refs each WITHOUT cse relating them -- the `+2` must
 * not be a `CONST (PLUS sym int)`.  Both halves of that have been probed:
 *
 * MEASURED INERT / WORSE (all at production flags, rom 37 / 80 bytes):
 *   `gBuffer + t` and `gBuffer + 2 + t` inline ....... 29, -4
 *   `t + gBuffer` / `t + gBuffer + 2` (operand order)  29, -4
 *   `gBuffer + (t + 2)` (move the 2 to the index) .... 29, -4
 *   two hand-hoisted bases g0/g1 ..................... 29, -4
 *   `unsigned short gBuffer[]`, `g[0]` then `g++` .... 27, -4 (inert)
 *   one `q = gBuffer + t`, then `q[0]` / `*(q+2)` .... 27, -4 (inert)
 *   `g = gBuffer;` / `g = g + 2;` (this body) ........ 27, -4
 *   `do { } while (i <= 0x3f)` ....................... 29, -4
 *   one scratch int reused for all the reads ......... 30, +4 (worse)
 *   -fno-gcse / -fno-rerun-cse-after-loop /
 *     -fno-strength-reduce ........................... inert (prior park)
 *   -fno-schedule-insns2 ............................. worse (prior park)
 *
 * A DEVICE, kept as a figure about the blocker: two DISTINCT extern symbols
 * (`gBuffer` and a fictitious `gBuffer_PLUS2`) defeats cse's relation and reads
 * 28 WITH THE LENGTH EXACT -- but it pools FOUR words and allocates r6 AND r7,
 * so the relation is not the whole story either.  Not a result; the ROM pools
 * one word.
 *
 * WHAT IS RIGHT and should be kept: the signed `n / 2` expansion
 * (`lsr #31 / add / asr #1`), the `& 0x1f` and `& 0x3e` masks, the unsigned
 * `i <= 0x3f` bound, the 0x80 strides on both pointers, the `dst + 0x40` second
 * store, and the pool CONTENTS and ORDER (ewram_2020000, 0x06004000, gBuffer),
 * which already agree with the ROM's.
 */
extern unsigned char ewram_2020000[];
extern unsigned char gBuffer[];

void Func_8011164(int n)
{
    unsigned char *src;
    unsigned char *dst;
    unsigned int i;
    int t;
    unsigned char *g;

    src = ewram_2020000 + ((n / 2) & 0x1f) * 4;
    dst = (unsigned char *)0x6004000 + (n & 0x3e);
    for (i = 0; i <= 0x3f; i++) {
        t = *(unsigned short *)src * 4;
        g = gBuffer;
        *(unsigned short *)dst = *(unsigned short *)(t + g);
        g += 2;
        *(unsigned short *)(dst + 0x40) = *(unsigned short *)(t + g);
        dst += 0x80;
        src += 0x80;
    }
}

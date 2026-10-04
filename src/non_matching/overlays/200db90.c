/* OvlFunc_969_200db90  --  0x0200db90
 *
 * STILL NON-MATCHING, **2 of 43 encodings** (ref 43 / ours 43, first differing
 * index 31).  PIN-FREE, SHIM-FREE, FLAG-FREE.  Batch 321 brief E RE-MEASURED
 * the park's figure and CONFIRMED it, then closed the remaining question that
 * batch 271 left open -- so this park is now closed on PROOF rather than on an
 * exhausted list.  The BODY BELOW IS UNCHANGED from the parked one; everything
 * new here is the diagnosis.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/overlays/200db90.c \
 *     asm/overlays/rom_7f6e64/ovl_314_c_c_c.s --func OvlFunc_969_200db90
 *
 * (The park's own first line names asm/overlays/rom_7f8b34/ovl_2b_c.s.  That is
 * stale: a grep of all of asm/ finds OvlFunc_969_200db90 in exactly ONE file,
 * asm/overlays/rom_7f6e64/ovl_314_c_c_c.s, which is what the recipe uses.)
 *
 * THE RESIDUE, and it is two instructions TRANSPOSED, nothing more:
 *
 *     rom    str r0, [r5, #0x40] / ldr r1, =0xfffffe00 / ldrh r3, [r6] / add r3, r1
 *     ours   str r0, [r5, #0x40] / ldrh r3, [r6] / ldr r1, =0xfffffe00 / add r3, r1
 *
 * ========== BATCH 321: THE CHAIN IS NOW COMPLETE IN BOTH DIRECTIONS ==========
 *
 * Batch 271 closed this on "RELOAD MATERIALISES -512 IMMEDIATELY BEFORE THE ADD,
 * so it is always emitted after the ldrh".  That is right, and I reproduced the
 * dump it rests on verbatim -- .19.flow2 still reads
 *
 *     (insn 80  (set (reg:SI 3 r3) (zero_extend:SI (mem:HI (reg/v:SI 6 r6)))))
 *     (insn 113 (set (reg:SI 1 r1) (const_int -512)))
 *     (insn 82  (set (reg/v:SI 3 r3) (plus:SI (reg:SI 3 r3) (reg:SI 1 r1))))
 *
 * -- but it left the obvious follow-up unanswered: sched2 runs AFTER reload, so
 * why can sched2 not hoist insn 113 above insn 80?  Both halves are now read off
 * the compiler rather than inferred, and together they close the class.
 *
 * (1) WHY THE CONST INSN CANNOT EXIST BEFORE THE LDRH.  Thumb's addsi3 takes its
 *     second operand through a `nonmemory_operand` predicate, and gcc checks
 *     PREDICATES but NOT CONSTRAINTS before reload (insn_invalid_p only consults
 *     constraints once reload_completed).  So cse1's validate_change of
 *     `(plus (zero_extend (mem:HI ...)) (const_int -512))` into insn 82 ALWAYS
 *     succeeds, whatever the constant is and however the source spells it, and
 *     the standalone pre-reload materialisation is then dead and deleted.  There
 *     is no constant value and no statement order that survives this: the park's
 *     five measured spellings (`*p = *p - 0x200`, a named `int bias` assigned
 *     before the tail, the bare-pin form, two halfword variants) are not an
 *     exhausted list, they are five instances of one impossibility.
 *
 * (2) WHY sched2 CANNOT PUT IT BACK.  haifa's rank_for_schedule ends with two
 *     tiebreaks, in this order:
 *         * "Prefer the insn which has more later insns that depend on it" --
 *           depend_count2 - depend_count1; and
 *         * "If insns are equally good, sort by INSN_LUID (original insn order),
 *           so that we make the sort stable.  This minimizes instruction
 *           movement."
 *     Insn 80 has the LONGER INSN_DEPEND list (the park read it as `119 118 86
 *     82 113`), so it wins the first tiebreak outright; and if it did not, it
 *     wins the LUID tiebreak, because reload inserted 113 between 80 and 82 and
 *     LUIDs are assigned in chain order.  A reload-created insn can therefore
 *     NEVER be scheduled above an equal-priority insn that precedes it.  The
 *     park's "every insn in the tail has IDENTICAL priority 36" is the premise
 *     that makes both tiebreaks the whole decision.
 *
 *     SCHED_GROUP_P is NOT what does this, and that is worth recording because
 *     it is the natural guess: in gcc-2.96 set_sched_group_p is called from only
 *     two places, the cc0 user case and the post-call group, so reload insns are
 *     NOT glued to the insn they serve.  They do not need to be.
 *
 * SO THE FLOOR IS 2, AND IT IS A FLOOR, not a search frontier.  Do not spend a
 * round on another spelling of the tail, and do not add a fakematch row: as
 * batch 271 established, the pin is destroyed before scheduling.
 *
 * FLAGS RE-MEASURED AGAINST THIS BODY (batch 321).  This closed a real gap: the
 * park's own flag list is labelled "TRIED, all 11", i.e. it was measured against
 * the 11-DIFFERING body, before the batch-204 barrier and the batch-204 naming
 * of the a+8 value took it to 2.  Re-run on the 2-differing body, ALL of these
 * are EXACTLY INERT at 2 of 43, ref 43 / ours 43:
 *
 *     -fno-gcse                     2      -fno-strength-reduce      2
 *     -fno-rerun-cse-after-loop     2      -fno-schedule-insns       2
 *     -fno-cse-follow-jumps         2      -fno-strict-aliasing      2
 *     -fno-cse-skip-blocks          2      -fno-peephole             2
 *     -fno-expensive-optimizations  2
 *     -fno-gcse -fno-rerun-cse-after-loop (crossed)                  2
 *
 *   and ONE is worse: -fno-schedule-insns2 is 14 of 43.  So neither GCSE_CFLAGS
 *   nor CSE_CFLAGS is a candidate here, crossed or alone, and the cse/gcse
 *   bucket is empty for this function.  That is consistent with (1): the fold
 *   that removes the materialisation is cse1's, and cse1 has no flag.
 *
 * ========== EVERYTHING BELOW IS THE INHERITED, STILL-CORRECT RECORD ==========
 *
 * Its twin is OvlFunc_925_200b460 (0x0200b460) in
 * asm/overlays/rom_7b0400/ovl_314_c_c_c_c.s; the two differ in ONE constant
 * (0xa4 against 0x90), so one solution elevates both.
 *
 *   SOURCE ORDER OF TWO LOADS DECIDES WHICH GETS r8 AND WHICH GETS r10.  Two
 *   values are loaded before the first call and both survive it.  Written in the
 *   ROM's apparent order (halfword first, pointer second) gcc assigned them to
 *   the opposite high registers from the ROM.  Swapping the two assignment
 *   statements -- while the emitted load order stayed the ROM's, because that
 *   follows first USE, not source position -- fixed the allocation, and fixed
 *   the mul operand order with it.  20 differing -> 11.
 *
 *   MUL COPIES THE SECOND OPERAND.  `mul rD, rS` computes rD = rD * rS and gcc
 *   emits the copy for the RIGHT-hand operand, so the source wants `c * r`.
 *
 *   `do { } while (0)` IMMEDIATELY BEFORE THE TAIL takes 11 to 4.  It is the
 *   batch-189 scheduling barrier, and `__asm__ volatile("")` there is
 *   byte-identical.  NAMING THE a+8 VALUE takes 4 to 2: the ROM interleaves
 *   `ldr r3, [r5, #8]` into the build of `0xa4 << 16`, so reading a+8 into a
 *   local BEFORE the a+0x10 store and storing that local to a+0x38 afterwards
 *   puts the load where the ROM has it.
 *
 *   NOT AN ALIASING PROBLEM.  The hoist crosses stores to a+0x10/0x38/0x40 while
 *   loading from a+0x64, but all are constant offsets from the SAME base
 *   register, so gcc disambiguates by arithmetic and never consults alias
 *   analysis.  -fno-strict-aliasing changing nothing is the confirmation.
 *
 *   THE CURRENT SPELLING IS LOAD-BEARING, not merely equivalent.  `*p +=
 *   0xfffffe00;`, `{ int h = *p; *p = h + 0xfffffe00; }` and the fully
 *   spelled-out halfword version are all 42 lines and 11 differing.  The
 *   named-int-intermediate form below is one line SHORTER and is what holds the
 *   length at 41 text instructions.
 *
 * FOR WHOEVER LANDS THE TWIN PAIR: datacheck says this .s carries .bss AND
 * .data, so it needs a TEXT/DATA split, not just a text split.
 */


extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_969_200db90(unsigned char *a)
{
    unsigned short *p;
    unsigned char *q;
    int ang;
    int r;
    int c, s;
    int v;
    int w;

    p = (unsigned short *)(a + 0x64);
    q = *(unsigned char **)(a + 0x68);
    ang = *p;
    c = __cos(ang);
    r = *(int *)(a + 0x30) + 0x1c;
    *(int *)(a + 8) = *(int *)(q + 8) + c * r;
    s = __sin(ang);
    w = *(int *)(a + 8);
    *(int *)(a + 0x10) = (s << 4) + (0xa4 << 16);
    *(int *)(a + 0x38) = w;
    *(int *)(a + 0x40) = *(int *)(a + 0x10);
    do { } while (0);
    v = 0xfffffe00;
    v += *p;
    *p = v;
}

/* Func_80119cc -- NON-MATCHING.
 * 85 encodings of 86 differ; ALIGNED residue 60 of 86 equal, 26 differing in
 * 16 hunks.  INSTRUCTION COUNTS ref 79 / ours 78 -- STILL ONE SHORT, so the 85
 * is almost entirely PHASE.  Size ref 184 / ours 180.  PINS 0.  Batch 345
 * brief C: figure 86 -> 85, aligned residue 54 -> 60 of 86, and the gap can be
 * closed three ways none of which touches the real residue.
 *
 * Verify with (the PARK path, this file):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/80119cc.c \
 *     asm/rom_9000/rom_11568_c_c_a_c_c_a.s --func Func_80119cc
 * Aligned view (THE NUMBER TO MOVE -- objcmp's is positional and saturated):
 *   ... python3 tools/aligncmp.py src/non_matching/rom_9000/80119cc.c \
 *     asm/rom_9000/rom_11568_c_c_a_c_c_a.s Func_80119cc -v
 *
 * ===================================================================
 * BATCH 345: THE HOIST DIAGNOSIS IS CONFIRMED AND IT IS TWO INVARIANTS, NOT
 * ONE.  THE LENGTH GAP IS CLOSEABLE AND CLOSING IT DOES NOT HELP.
 * ===================================================================
 *
 * PER-REGION SPLIT, off an index-aligned dump (ref 79 insns, ours 78):
 *
 *   entry + both guards            ref idx 0..11   12 : 11   ref +1
 *   loop PREHEADER                 (ours only)      0 :  4   ours +4
 *   loop top, the wait test        ref idx 12..15   4 :  3   ref +1
 *   the 0xffff / 0xfe00 / 0x3000 arms and the default arm  equal
 *   the wait-decrement block       ref idx 74..76   3 :  2   ref +1
 *   epilogue                       ref idx 77..78   2 :  3   ours +1
 *
 * The gap is NOT one site: it is ref +3 spread over three places against
 * ours +5 in the preheader and the epilogue.  Everything the park says about
 * the hoist is right, and there are TWO hoisted invariants, not one:
 *
 *     ours  ldr r3,=0x103 / ldr r5,=0xffff / adds r3,r3,r2 / mov ip,r3
 *     ROM   mov ip,r3 / mov r4,ip            (base only; nothing else hoisted)
 *
 * The ROM hoists `base` into ip and REMATERIALISES both `base + 0x103`
 * (`ldr r3,=0x103 / add r3, ip`, twice) and `0xffff` (`ldr rX,=0xffff`, twice --
 * once for the `op == 0xffff` test and once as the -1 of `wait - 1`, because
 * the HImode narrowing turns -1 into 0xffff) INSIDE the loop.  gcc hoists both,
 * which is what buys the `push {r5}` / `pop {r5}` pair the ROM does not spend.
 *
 * WHAT CHANGED IN THE BODY, and it is the park's own second observation made to
 * pay: the ROM copies the wait counter before testing it (`ldrh r2,[r4,#8] /
 * adds r3,r2,#0 / cmp r3,#0`) and decrements THE ORIGINAL (`adds r3,r2,r1`
 * with r1 = 0xffff).  Two live names.  Seven spellings swept; the one that
 * helps is an INT holding the load and the u16 taking the copy:
 *     v2 = *(unsigned short *)(s + 8);
 *     wait = v2;
 *     if (wait != 0) { *(unsigned short *)(s + 8) = v2 - 1; return; }
 * aligned-equal 54 -> 60, hunks 18 -> 16, figure 86 -> 85, gap still -1.
 * With `v` declared `unsigned short` instead the copy costs an `ldrsh` plus a
 * `lsls`/`lsrs` pair (+3 instructions) and aligned-equal FALLS to 49 -- the
 * type of the carrier is the whole lever, not the copy.
 *
 * THREE WAYS TO CLOSE THE LENGTH GAP, ALL MEASURED, NONE AN IMPROVEMENT:
 *   anchor `do { } while (0);` after `if ((op & 0xff) == 0xff) return;`
 *     79 insns (+0), stream 86 = ref, figure 79, aligned 57
 *     -- and gcc then hoists a THIRD invariant (r6 = 255) and pushes
 *        {r5, r6, lr}: further from the ROM's `push {lr}`, not closer.
 *   the u16 wait copy (+3) crossed with the named constants (-4)   79, fig 75, aligned 49
 *   the anchor crossed with either wait reshape                    79, fig 79, aligned 57
 * ALIGNED RESIDUE BEFORE 54 of 86; AFTER, on every gap-closed body, 49 or 57.
 * The best aligned residue in the batch is 60 AT THE OPEN GAP, which is what
 * this body carries.  A gap-closed body here is a worse program.
 *
 * NAMING THE CONSTANTS IS THE WRONG DIRECTION HERE, which is worth recording
 * because it is the opposite of what it did for the piece's neighbours:
 *   `t = 0x103;` in each block, 0x103 read through the name    75 insns (-4), fig 81
 *   that plus `u = 0xffff;` before the `op ==` test            75 (-4), fig 81
 *   plus the decrement as `wait + u`                           76 (-3), fig 84
 * All three make gcc SHORTER, not longer: the named offset is hoisted too.
 *
 * BOTH CHEAP AXES CLOSED on the parked body (tools/autosweep.py, base 86):
 *   -O1 78 (its aligned residue is WORSE, 40 -> 43: not progress),
 *   -fno-gcse 86, -fno-schedule-insns2 86, -ffixed-r7 86,
 *   -fno-strict-aliasing 86, -fno-strength-reduce 86,
 *   -fno-rerun-cse-after-loop 86, -fno-rerun-loop-opt 86.
 *   anchor axis 24 of 27 boundaries; only anchor15 moves, to 79, and it is the
 *   gap-closing row above.
 *
 * NEXT, WITH ITS EVIDENCE AND NOT AS A BOUND.  The hoist is `move_movables`
 * (loop.c:1707) and its cost test at loop.c:1800-1805 is
 * `threshold * savings * m->lifetime >= insn_count` with
 * `threshold = (has_call ? 1 : 2) * (1 + n_non_fixed_regs)` (loop.c:651) -- a
 * large number against a 79-insn function, so the test is not the lever and no
 * source change reaches it.  What MIGHT reach it is loop.c:1900's
 * `threshold -= 3` after each move: the invariants are moved in order and a
 * body that offers MORE movables can push a later one past the test.  That is
 * the only mechanism found here that discriminates between two hoists, and it
 * is untested.  The shared brief's "a loop hoist is not reachable by a barrier
 * at all" is consistent with everything measured above: the one anchor that
 * changes the length adds a hoist rather than removing one.
 *
 * SIBLING: src/non_matching/rom_9000/80110e0.c, same class, three failed
 * formulations recorded there.
 *
 * ===================================================================
 * THE ORIGINAL PARK HEADER, CARRIED FORWARD.  Its two "read correctly off the
 * disassembly" observations are both now in the body; the first of them is the
 * lever above.
 * ===================================================================
 * Func_80119cc -- asm/rom_9000/rom_11568_c_c_a_c_c.s
 *
 * NON-MATCHING, 86 of 86 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 86, ours 84) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Batch 322's re-classification of this figure is WITHDRAWN. *** THAT CLAIM WAS FALSE HERE. *** Re-measured by the coordinator after batch 348: ref 79 / ours 78 16-bit instructions, a gap of -1, so this figure is a POSITIONAL count measuring PHASE, not distance. Batch 322's re-classification sweep made this claim on 54 parks and was WRONG ON 34 OF THEM, this one included; the 18 it got right keep the claim. Close the length gap before reading the figure, then rank on tools/aligncmp.py rather than on the raw count.
 *   SIZE ref 184 bytes, ours 180.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/80119cc.c \
 *     asm/rom_9000/rom_11568_c_c_a_c_c_a.s --func Func_80119cc
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * BLOCKER: LICM out of the interpreter loop. 86 of 87, two short.
 *
 * A blend-effect script interpreter: a wait counter, a 0xffff restart opcode,
 * an 0xfe00 jump opcode, a 0x3000 opcode that writes REG_BLDCNT, and a default
 * that writes REG_BLDALPHA or REG_BLDY and then takes a wait value.
 *
 * The high differing count is misleading -- it is ONE decision at the top,
 * taken before the loop, that shifts every instruction after it:
 *
 *     rom   push {r14}          computes base+0x103 and 0xffff INSIDE the loop
 *     ours  push {r5, r14}      hoists both into r12 and r5 ahead of the loop
 *
 * Both are loop-invariant, gcc lifts them, and lifting costs a callee-saved
 * register the ROM does not spend. This is the same class as
 * src/non_matching/rom_9000/80110e0.c, where gcc hoists a loop-invariant base
 * that the ROM rematerialises every iteration -- and there three separate
 * formulations all folded to the same hoisted output.
 *
 * TWO THINGS WERE READ CORRECTLY off the disassembly and should be kept if
 * this is retried:
 *
 *   - the ROM copies the wait counter before testing it (`ldrh r2,[r4,#8] /
 *     mov r3, r2 / cmp r3, #0`), which by the copy-then-modify tell means two
 *     live names, not one expression;
 *   - REG_BLDALPHA and REG_BLDY are selected into ONE register and stored
 *     through once (`ldr r3,=A / b / ldr r3,=B / strh r1,[r3]`), so the source
 *     picks a `vu16 *` and stores once. Writing two separate stores would give
 *     two `strh` and is wrong.
 *
 * Neither was the blocker. Not pursued further because the hoist is the whole
 * residue and the sibling park already records three failed formulations
 * against it.
 */
#include "gba/io.h"

extern int iwram_3001e70;

void Func_80119cc(void)
{
    char *base;
    char *s;
    unsigned short *pc;
    unsigned int op;
    vu16 *reg;
    unsigned short wait;
    int v2;

    base = (char *)iwram_3001e70;
    s = base + 0xd8;
    if (*(int *)s == 0)
        return;
    if (*(unsigned short *)(s + 0xa) != 0)
        return;
    for (;;) {
        v2 = *(unsigned short *)(s + 8);
        wait = v2;
        if (wait != 0) {
            *(unsigned short *)(s + 8) = v2 - 1;
            return;
        }
        pc = *(unsigned short **)(s + 4);
        op = *pc;
        pc++;
        if (op == 0xffff) {
            *(int *)(s + 4) = *(int *)s;
            continue;
        }
        if ((op & (0xff << 8)) == (0xfe << 8)) {
            if ((op & 0xff) == 0xff)
                return;
            *(int *)(s + 4) = *(int *)s + ((op & 0xff) << 2);
            continue;
        }
        if ((op & (0xf0 << 8)) == (0xc0 << 6)) {
            REG_BLDCNT = op;
            *(unsigned char *)(base + 0x103) = op;
            *(int *)(s + 4) = *(int *)(s + 4) + 2;
            continue;
        }
        if ((*(unsigned char *)(base + 0x103) & 0xc0) == 0x40)
            reg = &REG_BLDALPHA;
        else
            reg = &REG_BLDY;
        *reg = op;
        *(unsigned short *)(s + 8) = *pc;
        *(int *)(s + 4) = *(int *)(s + 4) + 4;
    }
}

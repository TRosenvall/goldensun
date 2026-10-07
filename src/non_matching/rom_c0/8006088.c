/* Func_8006088 (0x08006088) -- NON-MATCHING.
 *
 * NON-MATCHING, 24 of 44 stream encodings  (RE-MEASURED, batch 333).
 *   STREAM lengths differ, ref 44 against ours 42, and the 16-bit INSTRUCTION
 *   counts differ, ref 39 against ours 38 -- so this positional figure
 *   measures MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 96 bytes, ours 92.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c0/8006088.c \
 *     asm/rom_c0/rom_5cf8_a_a_a_c_c_a_c.s --func Func_8006088
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: register allocation -- one `mov` the source cannot ask for.
 *
 * The first SEVENTEEN instructions are exact. The whole residue is that the
 * ROM accumulates the result in r2 for its first two ORs and then copies it
 * into r0 for the last two:
 *
 *     rom    ldrb r3, [r5, #9] / mov r0, r2 / cmp r3, #0 / beq ...
 *     ours   ldrb r3, [r5, #9] /             cmp r3, #0 / beq ...
 *
 * gcc puts the accumulator in r0 from the start -- r0 is free once the
 * conditional call's argument is dead -- so our stream is one instruction
 * SHORT at 44 against 45 and everything after the join is offset by one.
 *
 * TWO SPELLINGS OF THE BIT EXTRACTION, and the ROM's is the two-shift one:
 *
 *     rom               lsl r3, r7, #26 / lsr r3, #30
 *     `(sio >> 4) & 3`  lsr r3, r7, #4 / mov r2, #3 / and r3, r2
 *     `(sio << 26) >> 30`  lsl r3, r7, #26 / lsr r3, #30      <- matches
 *     a 2-bit bitfield at bit 4                                <- also matches
 *
 * Note the trap this created. With the WRONG (three-instruction) extraction
 * the stream is 45 lines -- the ROM's length exactly -- because the extra
 * instruction there cancels the missing `mov r0, r2` here. It screens at 12
 * differing against 21 for the correct spelling, purely because the one-line
 * offset stops cascading. **A better differing count from a compensating pair
 * of errors is not progress**, and the length agreeing is not evidence when
 * two independent defects can cancel.
 *
 * MEASURED for the missing `mov`, all 44 lines and 21 differing unless noted:
 *   two variables, `w` for the first two ORs and `v = w;` before the rest --
 *     the disjoint-live-range spelling; gcc coalesces them straight back  21
 *   `unsigned int v`                                                      21
 *   `unsigned int` return type                                            21
 *   `(s[2] << 8) | s[3]` with the operands swapped                        22
 *
 * WHAT IS RIGHT AND SHOULD BE KEPT:
 *   - REG_SIOCNT read through `*(vu32 *)REG_ADDR_SIOCNT`. The header defines
 *     it as `vu16`, but the ROM reads it with `ldr`, and the 32-bit read is
 *     what puts the whole word in r7 for the two-shift extraction.
 *   - the argument in r0 across the guard: `mov r6, r0 / mov r0, r1` comes
 *     from passing the SECOND parameter to the first callee and the first
 *     parameter to the second, which is what the two `bl`s in the guarded
 *     block need.
 *   - the three OR constants as plain literals -- 0x80, 0x80 << 5, 0x80 << 6
 *     all give the ROM's mov/lsl pairs with no naming.
 *
 * NEXT: nothing source-level outstanding. This is one register-allocation
 * instruction, in the class docs/elevation.md records as the dominant wall.
 *
 * ---------------------------------------------------------------------------
 * BATCH 333: THE MISSING INSTRUCTION IS LOCATED AND REACHABLE, AND THE BLOCKER
 * CLASS IN THIS HEADER IS WRONG.
 *
 * Aligned against the reference, the whole gap is the ONE instruction this
 * header already names, and nothing else in the body is a shape difference.
 * Of the forty-four reference encodings, every one is identical except:
 *   - two pool-load displacements, a consequence of the length;
 *   - the accumulator's register in three instructions (r2 against r0);
 *   - the missing copy itself.
 * So once the copy lands this function is a candidate for zero.
 *
 * IT IS NOT A REGISTER-ALLOCATION WALL.  This header says "one `mov` the source
 * cannot ask for" and files it under register allocation.  The copy is in fact
 * deleted much earlier, by GCSE's COPY PROPAGATION in pass 07, long before
 * local-alloc runs.  Written with the two accumulator names and the copy at the
 * first if's join -- the spelling this header's table records as inert -- the
 * dump `cand.c.07.gcse` prints exactly:
 *
 *     COPY-PROP: Replacing reg 37 in insn 86 with reg 36
 *     CPROP of Func_8006088, pass 1: 0 const props, 1 copy props
 *
 * one copy propagated, and it is this one.  So the question is not "what asks
 * for a mov" but "what stops gcse.c's cprop_insn from propagating the join
 * copy".  The same pass eats the same kind of copy in
 * src/non_matching/rom_b5000/80b90ac.c, where the dump line was also read.
 *
 * AND THE COPY IS DEMONSTRABLY REACHABLE.  Writing the first guard with an
 * ELSE, so both arms assign the second name --
 *
 *     if (s[0] == 8) v = w | 0x80; else v = w;
 *
 * -- emits the reference's `mov r0, r2` in the right place and reads twenty of
 * forty-four, with the first twenty-one instructions exact.  It costs an
 * unconditional branch over the else arm, so it is forty instructions against
 * the reference's thirty-nine: one too LONG instead of one too short.  That is
 * recorded as evidence that the copy is reachable, NOT as progress -- this
 * park's own trap note applies, and the body below is kept.
 *
 * WHAT THE REFERENCE'S BLOCKS ARE, since the next attempt needs them: the copy
 * sits in the JOIN block of the first guard, where the accumulator has two
 * reaching definitions (the initial or, and the `|= 0x80` arm).  An if/else
 * puts the copy in an ARM instead, which is why cprop cannot reach it there.
 *
 * MEASURED AND EXACTLY INERT, all at twenty-four of forty-four: the two-name
 * spelling with the copy at the first join; the same with the declaration order
 * swapped; the same with every or written out as `v = v | K`; both accumulators
 * `unsigned int`; the first or split into two statements.
 * MEASURED WORSE: the copy placed BEFORE the first guard (twenty-seven); a
 * redundant zero test before the return (forty-one instructions).
 *
 * THE DECLARATION IS CONFIRMED BY THE EPILOGUE: `pop {r1} / bx r1` pops into r1,
 * which `thumb_exit` (arm.c:8302, :8306-8330, :8382) emits for a declared
 * return of one to four bytes.  The `int` return below is right.
 *
 * PIECE: `asm/rom_c0/rom_5cf8_a_a_a_c_c_a_c.s` also holds Func_8005fcc, and
 * THAT FUNCTION IS ALSO PARKED (src/non_matching/rom_c0/8005fcc.c).  One .c is
 * one TU, so neither converts alone -- a landing here must carry both.
 * ---------------------------------------------------------------------------
 */
#include "gba/types.h"
#include "gba/io.h"

extern unsigned char ewram_2002240[];
extern void Func_800615c(int a);
extern void Func_80060e8(int a);

int Func_8006088(int a, int b)
{
    unsigned char *s;
    unsigned int sio;
    int v;

    s = ewram_2002240;
    sio = *(vu32 *)REG_ADDR_SIOCNT;
    if (s[1] == 1) {
        Func_800615c(b);
        Func_80060e8(a);
        s[0xb]++;
    }
    v = s[3] | (s[2] << 8);
    if (s[0] == 8)
        v |= 0x80;
    if (s[9] != 0)
        v |= 0x80 << 5;
    if (((sio << 26) >> 30) > 1)
        v |= 0x80 << 6;
    return v;
}

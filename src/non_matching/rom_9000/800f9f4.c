/* DecodeMetatileset -- asm/rom_9000/rom_f9cc_a_c_b.s
 *
 * NON-MATCHING, 24 of 73 encodings  (MEASURED, batch 335 brief D).
 *   STREAM LENGTHS NOW AGREE: ref 73, ours 73, and the REAL instruction count
 *   agrees too -- seventy on both sides.  SIZE 152 bytes both.  Relocations
 *   clean.  So this figure IS a distance, and it is a register permutation.
 *   The previous body claimed forty-eight of the same seventy-three and was
 *   two encodings short; the body below is new and the gap is closed.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_9000/800f9f4.c \
 *     asm/rom_9000/rom_f9cc_a_c_b.s --whole
 *
 * A three-mode decoder: plain halfword copy, byte-plane merge with a running
 * xor, and halfword running xor.  The switch reproduces instruction for
 * instruction -- gcc builds the ROM's comparison chain (test 1, then >1, then
 * ==0, then ==2, with the same fallthroughs and the same branch to the common
 * exit).  The signed (n - 1) / 2 and the odd-n guard are exact.  The reference
 * epilogue pops into r0, which is the void return this declares.
 *
 * ========== WHAT CLOSED THE TWO-INSTRUCTION GAP: FOUR CROSSED LEVERS ========
 *
 * None of these works alone and two of them are WORSE alone.  The gap was two
 * instructions and they were the SAME construct in the two xor-delta modes.
 *
 *   1. mode 1's accumulator gets an explicit temp:
 *        t = ((hi << 8) | lo) ^ prev;   *d = t;   ...   prev = t;
 *      We had `prev = value ^ prev`, which regmove turns into a TWO-ADDRESS
 *      update of prev -- it stores prev and needs no copy back.  The ROM
 *      computes the xor into the register that held the freshly loaded value,
 *      stores THAT, and only then copies it into the loop-carried accumulator.
 *      Alone this is worth eight differences.
 *   2. mode 2's accumulator gets the same treatment.  Alone, or on top of 1
 *      with the old init order, it is WORSE -- the allocator then coalesces the
 *      mode-1 byte pointer with the halfword pointer and the pointer init
 *      vanishes, so the body is still one real instruction short.
 *   3. mode 1 initialises its POINTERS BEFORE its SCALARS:
 *        b = ewram_2010002;  b2 = b + count;  i = 0;  prev = 0;
 *      With 1 and 2 this is what keeps that pointer init alive, and it is what
 *      reaches the ROM's seventy real instructions.  `b2 = count + b` and
 *      `b2 = ewram_2010002 + count` are byte-identical to it.
 *   4. the two source bytes get their own temps:
 *        hi = *b;  lo = *b2;  t = ((hi << 8) | lo) ^ prev;
 *      The ROM loads BOTH bytes, then shifts, then ors; the inline expression
 *      interleaves the second load between the shift and the or.  Worth four.
 *
 * ========== THE ALIGNMENT-PAD TRAP, AND IT COST A ROUND HERE ================
 *
 * At sixty-nine real instructions this body is 138 bytes of code plus twelve
 * bytes of pool = 150, which the assembler pads to 152 -- and `objdump -dz`
 * COUNTS THE TWO-BYTE PAD AS AN ENCODING.  So objcmp reported equal stream
 * lengths and dropped its SIZE line while the body was still ONE REAL
 * INSTRUCTION SHORT of the reference.  An equal encoding count is not an equal
 * instruction count when the pool is 4-aligned and the body is odd.  Count real
 * instructions separately before believing a closed gap.
 *
 * ========== THE OLD BLOCKER SENTENCE WAS A CONSEQUENCE, NOT A CAUSE ========
 *
 * It read "REGISTER ROTATION (allocator preference)" and tabulated
 * count/src/dst.  The rotation was downstream of the accumulator's live range;
 * once the accumulator is spelled the ROM's way, three quarters of the rotation
 * goes with it.  Its figure line also claimed forty-one of seventy-eight, which
 * matches no measurement of this function.
 *
 * ========== WHAT IS LEFT: ONE COHERENT REGISTER PERMUTATION ================
 *
 *   ref   count r4   s r6   b r6 (A FRESH POOL LOAD)   b2 r0   prev r7 / r2
 *   ours        r6     r2     r7 (A COPY of s)            r4        r0 / r0
 *   and d r5, i r1 and both scratch temps already agree.
 *
 * The ROOT is the one remaining structural item.  The ROM's mode-1
 * `b = ewram_2010002` is a FRESH POOL LOAD -- the same pool word the pre-switch
 * load uses -- where ours is a copy of the halfword pointer.  A copy makes the
 * two pointers simultaneously live, so they CONFLICT and cannot share a hard
 * register; in the ROM they do not conflict and both get r6.  That single
 * conflict forces the whole permutation.  cse1 substitutes the available
 * register for the symbol_ref because a pseudo is cheaper than a CONSTANT_P
 * source, and nothing below reaches that choice.
 *
 * NEXT: ONE question -- what stops cse substituting the halfword pointer into
 * mode 1's pool load.  It is NOT the spelling of the initialiser and it is NOT
 * where the halfword pointer is assigned; both are measured below.  Candidates
 * not yet reached: whether the two pointers can be given live ranges that make
 * the substitution unprofitable, and whether the mode-1 block can be reached on
 * a cse path that never recorded the equivalence.
 *
 * ========== MEASURED, AND BIT-IDENTICALLY INERT OR WORSE ==================
 *
 * On the b-vs-s question, all with levers 1-4 applied:
 *   the far pointer pool-loaded independently                    inert
 *   the halfword pointer declared as a byte pointer and cast      inert
 *   the byte pointer assigned BEFORE the switch as well           +24
 *   the halfword pointer assigned inside cases 0 and 2            +33, one long
 *   the byte pointer assigned by a cast from the halfword one     +27
 *   one pointer reused for all three modes                       +26 to +30
 *   two separate accumulators, one per mode                      +19 to +28
 *   all ten declaration orders of the eight locals               INERT, every one
 *   the destination pointer assigned before the source pointer   +1, and dirty
 *                                                                relocations
 *   modes 0 and 2 indexing the bare array instead of a pointer   +38, one long
 * On the accumulator, before 3 and 4 were crossed in:
 *   the temp in mode 2 only                                      +17
 *   the temp assigned to the accumulator before the pointer
 *     bumps rather than after                                    +23
 *   the second pointer assigned inside the loop, four partitions
 *     by three slots spellings                                   +14 to +35
 * On the mode-1 expression:
 *   the low byte first in the or                                 -1 against the
 *                                                                inline form
 *   the accumulator first in the xor                             inert
 *
 * SOLVED and worth keeping: the whole comparison chain, the signed divide, the
 * odd-n guard, the three loop shapes (for, while, do-while, each as written),
 * and the void return the reference's epilogue confirms.
 */
extern unsigned char ewram_2010001;
extern unsigned char ewram_2010002[];
extern unsigned short ewram_2020000[];

void DecodeMetatileset(int n)
{
    int count;
    int i;
    unsigned short *s;
    unsigned short *d;
    unsigned char *b;
    unsigned char *b2;
    int prev;
    int t;
    int hi;
    int lo;

    count = (n - 1) / 2;
    s = (unsigned short *)ewram_2010002;
    d = ewram_2020000;
    if (n & 1) {
        switch (ewram_2010001) {
        case 0:
            for (i = 0; i < count; i++) {
                *d = *s;
                s++;
                d++;
            }
            break;
        case 1:
            b = ewram_2010002;
            b2 = b + count;
            i = 0;
            prev = 0;
            while (i < count) {
                hi = *b;
                lo = *b2;
                t = ((hi << 8) | lo) ^ prev;
                i++;
                *d = t;
                b2++;
                b++;
                d++;
                prev = t;
            }
            break;
        case 2:
            prev = 0;
            if (count > 0) {
                i = count;
                do {
                    t = *s ^ prev;
                    i--;
                    *d = t;
                    s++;
                    d++;
                    prev = t;
                } while (i != 0);
            }
            break;
        }
    }
}

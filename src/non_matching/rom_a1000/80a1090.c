/* Func_80a1090  --  NON-MATCHING, 7 of 29 encodings
 *                   (ref 29, ours 29 -- COUNT EQUAL, so this figure IS a
 *                    distance.  SIZE EXACT at 64 bytes.  RELOCATIONS IDENTICAL.)
 *
 *   Was parked at 16 of 29 with ref 29 / ours 27, size 60 against 64, and
 *   relocations differing.  MEASURED in batch 322, brief I.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a1090.c \
 *     asm/rom_a1000/rom_a1050_c_a_a.s --func Func_80a1090
 *
 * SPLIT SHAPE: asm/rom_a1000/rom_a1050_c_a_a.s holds ONE function
 * (`grep -c ^\.thumb_func_start` = 1) and carries no data section, so a
 * landing would convert WHOLE with no split and no exports.  PINS: 0.
 *
 * ------------------------------------------------------------------------
 * WHAT THE 16 WAS, PER INDEX.  Three of the sixteen were pool-offset
 * artefacts (indices 0, 7, 8: the same `ldr rX, [pc, #N]` with N off by 4),
 * four were a register exchange (11, 13, 15, 17) and NINE (20..28) were pure
 * misalignment from being ONE INSTRUCTION SHORT.  Two causes, and the short
 * instruction carried most of the number.
 *
 * CAUSE B, CLOSED -- combine folds whichever address add is adjacent to its
 * store.  `.12.life` holds BOTH adds:
 *
 *     (insn 50 (set (reg 33) (plus (reg 32) (reg 34))))     q = p + k
 *     (insn 68 (set (mem:QI (reg 33)) (subreg:QI (reg 36)))) *q = w
 *     (insn 71 (set (reg 33) (plus (reg 32) (reg 34))))     q = p + k
 *     (insn 75 (set (mem:QI (reg 33)) (subreg:QI (reg 36)))) *q = w
 *
 * `.13.combine` folds 71 into 75 and NOT 50 into 68, because `k` (reg 34) is
 * incremented between 50 and 68 and nothing at all lies between 71 and 75.
 * The result is the ROMs two-insn `add r2, r4, r1` + `strb r3, [r2]` becoming
 * the one-insn register-offset form `strb r3, [r4, r2]`.
 *
 * The named-pointer lever WAS already applied by the park; combine undoes it on
 * the adjacent one.  `*(volatile unsigned char *)q = w` on the LAST store stops
 * it, because combine will not substitute into an insn holding a volatile MEM.
 *
 * THE CAST MUST BE ON A STORE THROUGH A NAMED POINTER.  Measured:
 *     *(volatile unsigned char *)q = w;        (named pointer)      7
 *     *(volatile unsigned char *)(p + k) = w;  (inline address)    16
 * The inline form never produces two insns for combine to decline -- expand
 * folds it -- so the lever never engages.  That distinction is new.
 *
 * Two other spellings tie at 7 and are byte-identical to this body:
 *   - the same cast on BOTH byte stores;
 *   - declaring `volatile unsigned char *p, *q;` outright with no cast anywhere
 *     and `DMA3_FILL((unsigned char *)p, 0, 0xa70)`.
 * This body is the minimal one.
 *
 * ------------------------------------------------------------------------
 * CAUSE A, THE WHOLE REMAINING RESIDUE -- all 7 indices are one exchange:
 * the ROM keeps the byte offset `k` in r1 and the computed address in r2; we
 * have them the other way round.
 *
 *     rom   mov r1, #0x89 / lsl r1, #1 / add r2, r4, r1 / add r1, #1
 *     ours  mov r2, #0x89 / lsl r2, #1 / add r1, r4, r2 / add r2, #1
 *
 * `.18.greg` says `;; 1 regs to allocate: 33`, so r2-for-`k` is a LOCAL-ALLOC
 * decision and r1-for-the-address is what global-alloc has left over.
 *
 * `local-alloc.c:1496` -- and note this DOES use floor_log2, see FINDINGS.md:
 *
 *     QTY_CMP_PRI(q) = floor_log2(n_refs) * n_refs * size
 *                      / (qty[q].death - qty[q].birth) * 10000
 *
 * `death` and `birth` are local-allocs 2-per-insn slot numbers, so they are
 * about twice the "across N insns" figure `.17.lreg` prints (that one is
 * REG_LIVE_LENGTH, which is global-allocs denominator).  The factor of two
 * cancels in an ORDERING, so the ranking below is sound; only the absolute
 * numbers are in insn-span units, and the odd/even parity term is a sub-percent
 * perturbation.
 *
 *     pseudo  role  n_refs  span  floor_log2(R)*R/span    local?
 *       32     p       7     18       7777                yes -> r4
 *       37     w       5     14       7142                yes -> r3
 *       35     k       5     20       5000                yes -> r2
 *       36     v       2      6       3333                yes -> r3
 *       33     q       4      8      (10000)              NO
 *
 * `q` is excluded at `local-alloc.c:362`:
 *
 *     if (REG_BASIC_BLOCK (i) >= 0 && REG_N_DEATHS (i) == 1 && ...)
 *       reg_qty[i] = -2;    -- local-alloc may have it
 *     else
 *       reg_qty[i] = -1;    -- global-allocs problem
 *
 * `q` is set twice and each value dies at its own store, so REG_N_DEATHS is 2.
 * global-alloc then prints `;; 33 conflicts: 32 33 35 37 2 3 4 13` -- hard 2/3/4
 * there ARE `k`/`w`/`p` as local-alloc already placed them -- leaving only r0
 * and r1, and REG_ALLOC_ORDER {3,2,1,0,...} (arm.h:989) picks r1.
 *
 * SO THE DECIDING RUNG, STATED AS A TARGET: the address qty must be LOCAL
 * (REG_N_DEATHS == 1) AND score BELOW `w`s 7142 (so `w` takes r3 first) AND
 * ABOVE `k`s 5000 (so it takes r2 before `k` asks).  That window is
 * n_refs = 4 with span 12..15, or n_refs = 3 with span 5, or n_refs = 2 with
 * span 3.  Simply making `q` local is NOT enough: at 4 refs over 8 it scores
 * 10000 and would take r3 off `w`.
 *
 * MEASURED AND REFUTED on cause A (all at ref 29 / ours 29, size exact):
 *   two pointers `q`,`q2`, `q2` late                        8
 *      -- `q2` becomes 2 refs over 2 insns, scores 10000, is allocated first
 *         and SHARES r4 WITH `p` (`;; 34 in 4`), because `p`s last use IS
 *         `q2 = p + k`.  `k` still gets r2.
 *   two pointers, `q2` early                               13
 *   two pointers, both stores volatile                      8
 *   `q2` declared before `q`                                8
 *   block-scoped `q` in two `{}` blocks                     8
 *   `k` as `int` rather than `unsigned int`                  8 (and 16 on base)
 *   `p[k] = w` for the last store                          16 (folds)
 *   `q += 1; *q = w`                                       21
 *   `k += 1` moved between the two stores                  21
 *   volatile on the FIRST store only                       16
 *
 * A NEW NEGATIVE WITH ITS REASON -- the hard-reg-conflict route to r1 is shut
 * by the DMA preheader.  Hoisting `k = 0x89; k <<= 1;` above the DMA block was
 * meant to make `k`s live range reach back into the blocks hard r2
 * (`ldr r2, =0x8500029c`) so r2 would conflict and `k` would fall to r1.  It
 * does conflict -- and the DMA block then needs one register more than it has.
 * Seven placements, including putting `k` before `p` is even loaded:
 * 30, 30, 31, 31, 31, 31, 33 differing, ALL at size +4.
 *
 * CONFIRMED FROM THE PARK, as a dividend: `DMA3_FILL(p, 0, 0xa70)` is right,
 * and indices 0..10 are byte-identical in every variant measured above.  The
 * parks "the header is not a blocker; picking the wrong helper is" survives.
 */
#include "dma.h"

extern unsigned char *iwram_3001f2c;

void Func_80a1090(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned int k;
    int v;
    int w;

    p = iwram_3001f2c;
    DMA3_FILL(p, 0, 0xa70);
    k = 0x89;
    v = 0xff;
    k <<= 1;
    p[0x1c] = v;
    q = p + k;
    w = 1;
    k += 1;
    p[0x1e] = w;
    p[0x1f] = w;
    *q = w;
    q = p + k;
    *(volatile unsigned char *)q = w;
}

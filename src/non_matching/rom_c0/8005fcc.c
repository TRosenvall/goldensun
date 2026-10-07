/* Func_8005fcc (0x08005fcc) -- NON-MATCHING, 74 of 87 encodings differ.
 *
 * STREAM LENGTH AND SIZE NOW EQUAL THE REFERENCE (87 == 87, 188 bytes == 188),
 * so this figure is a TRUE DISTANCE.  Improved from seventy-seven in batch 334.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c0/8005fcc.c \
 *     asm/rom_c0/rom_5cf8_a_a_a_c_c_a_c.s --func Func_8005fcc
 *
 * HEADER RESTORED, batch 334.  The batch-334 body replaced this park and took
 * its header with it -- the recipe, the claim and every measured negative went
 * with the old body, and the coordinator installed it without checking, so
 * parkcheck read UNCHECKABLE ("no header comment") until this was written.
 * The content below is recovered from the previous revision and re-dated.
 * **A park body may be replaced; its header must be carried forward.**
 *
 * THE PIECE.  asm/rom_c0/rom_5cf8_a_a_a_c_c_a_c.s holds TWO functions and BOTH
 * are parked -- this one and Func_8006088 (at twenty-four, its instruction
 * counts differing).  One .c is one TU, so NEITHER CONVERTS ALONE.  This
 * function is the gate on that piece.
 *
 * WHAT BATCH 334 FIXED, and it is a MODE problem rather than a naming one.
 * The old header treated the four pooled constants as a naming lever costing an
 * instruction each, and concluded the length walks past the ROM's before the
 * count converges.  Two of the four are free: REG_IE is `vu16`, so the whole
 * read-modify-write was HImode, and in HImode a const_int ALWAYS pools --
 * *thumb_movhi_insn (arm.md:4318-4320) constrains operand 1 "l,mn,l,*h,*r,I"
 * with `mn` at alternative ONE and the immediate `I` at FIVE, and recog takes
 * the first match.  Routing the value through an `unsigned int` temp, mask
 * first, builds both in SImode exactly as the reference does.  (*thumb_movsi_insn
 * at :3838-3840 has that order REVERSED, which is why SImode never pools.)
 *
 * The 0xc0 is pooled BY THE REFERENCE TOO and must stay a halfword store.
 *
 * REMAINING RESIDUE: one allocno-order decision -- the serial pointer spills to
 * `ip`.
 *
 * STILL TRUE, from the earlier rounds.  No calls at all, so nothing forces a
 * callee-saved register and the residue is pure selection.  The ROM copies and
 * masks IN PLACE, keeping the result in the register it stores from, where gcc
 * masks into the constant's register:
 *
 *     rom    mov r3, #0x88 / mov r4, r5 / and r4, r3
 *     ours   mov r6, #0x88 / and r6, r5
 *
 * `t = s; t &= 0x88;` as two statements is byte-identical -- gcc folds it.
 *
 * MEASURED NEGATIVES on the OLD body, kept because the dimensions are still
 * real even though the baseline moved (figures are against that old baseline
 * and are NOT comparable with the seventy-four above):
 *   - `one = 1` named and block-scoped was the best of its round
 *   - `bb = 0x40` named was exactly inert
 *   - naming all three of `one`, the mask and 0x40 was worse than naming one
 *
 * WHAT IS RIGHT AND SHOULD BE KEPT: the 32-bit read of REG_SIOCNT through a
 * `vu32 *`; the byte access `((vu8 *)sio)[1] &= ~0x40` for the high half;
 * `REG_TM3CNT = 0xc963` with gcc deriving 0x400010c from REG_IF by itself
 * (`sub r2, #0xf6`); the `(s << 26) >> 30` bit extraction; and the three `|=`
 * flag bits in the tail.
 */

#include "gba/types.h"
#include "gba/io.h"

extern unsigned char ewram_2002240[];

int Func_8005fcc(void)
{
    unsigned char *p;
    vu32 *sio;
    unsigned int s;
    unsigned int v;
    unsigned int ie;
    int r;
    int t;

    p = ewram_2002240;
    sio = (vu32 *)&REG_SIOCNT;
    s = *sio;
    if (p[1] == 0) {
        t = s & 0x88;
        if (t == 8) {
            v = (unsigned char)(s & 4);
            if (v == 0 && *(int *)(p + 0x14) == -1) {
                int one = 1;
                REG_IME = v;
                ie = ~0x80;
                ie &= REG_IE;
                ie |= 0x40;
                REG_IE = ie;
                REG_IME = one;
                ((vu8 *)sio)[1] &= ~0x40;
                REG_IF = 0xc0;
                REG_TM3CNT = 0xc963;
                p[0] = t;
            }
            p[1] = 1;
        }
        p[0xb]++;
    }
    r = (p[2] << 8) | p[3];
    if (p[0] == 8)
        r |= 0x80;
    if (p[9] != 0)
        r |= 0x80 << 5;
    if (((s << 26) >> 30) > 1)
        r |= 0x80 << 6;
    return r;
}

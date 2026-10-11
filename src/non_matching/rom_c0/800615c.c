/* Func_800615c (0x0800615c) -- NON-MATCHING.
 * NON-MATCHING: 84 encodings of 109 differ (objcmp).
 * Blocker class: register allocation -- one allocno rotation, no instruction
 * of the ROM's stream is missing or spare.
 *
 * INSTRUCTION COUNT, SIZE AND SHAPE ARE ALL THE ROM'S: 109 encodings against
 * 109, and a line-for-line objdump diff shows the two streams differ only in
 * WHICH register each value sits in, plus two consequences of that:
 *
 *     rom   r9 = &ewram_2002240 (live out)   lr = &v    r7 = i    r6 = walker
 *     ours  r8 = &ewram_2002240              r9 = &v    r6 = i    r7 = walker
 *
 *     rom    subs r3, r1, #4        (base derived from &ewram_2002244)
 *     ours   ldr  r3, [pc, #148]    (base re-pooled)
 *
 *     rom    adds r4, r0, #0 ... mov ip, r4   (argument copied twice)
 *     ours   mov ip, r0                       (copied once)
 *
 * Three high registers are saved in both, and the pattern r9/sl/r8 in the ROM
 * against r8/r9/sl in ours is one step along REG_ALLOC_ORDER -- the signature
 * of ONE allocno's priority differing, not of a different source shape.
 *
 * WHAT IS ESTABLISHED AND SHOULD BE KEPT:
 *  - NO struct-pointer local. `ewram_2002240.` spelled out at every reference is
 *    what produces the ROM's TWO base pseudos: a loop-invariant one used inside
 *    the loop and a second, RE-MATERIALISED from the pool in both `if` bodies and
 *    read after the loop (`ldr r2,=ewram_2002240 / mov r9, r2`). With
 *    `struct Snd *s = &ewram_2002240;` gcc keeps one pseudo, the two `mov r9`
 *    and the tail's `mov r1, r9` vanish and the stream is 102 against 109.
 *  - `SET_IO(REG_IME, 1)`, not `REG_IME = 1`. The int carrier lets
 *    reload_cse_move2add chain the comparison constant off the stored 1
 *    (`movs r2,#1 / ... / subs r2,#2` = -1); as a bare HImode store the 1 pools
 *    and the -1 is built separately.
 *  - THE 8-BYTE LOCAL IS A BYTE ARRAY, not `u32 v[2]`. `unsigned char v[8]` with
 *    `*(u32 *)(v + 4) = 0` / `*(u32 *)v = ewram_2002244` and `c = v[i]` is what
 *    puts `mov lr, sp` in the ROM's block (after the swap loop) instead of
 *    before it: 93 differing -> 86. This is a live-range effect on the
 *    global-alloc priority formula, not a spelling preference.
 *  - A SEPARATE COUNTER for the swap loop (`k`) and the main loop (`i`): 86 -> 84.
 *  - `(short)sum == -1` for the checksum test, `for (j = 0; j < 14; j++)` with an
 *    UNSIGNED j for the 14-halfword sum (the ROM's `bls #0xd` is unsigned), and
 *    the guarded-then-unguarded pair of `(short)sum == -1` tests, which is what
 *    duplicates `lsl r5, r0, #16` into both arms and cross-jumps the `asr`.
 *  - `pop {r1}` in the epilogue: the function returns int with no `return`
 *    needed for the dead `ldrb r0, [r1, #3]` -- but `return ewram_2002240.f3;`
 *    after the `|=` reproduces it, including the second load of f3.
 *
 * MEASURED AND INERT (each tried on top of the above):
 *   a separate local for the walking DMA destination                      84
 *   that local assigned just before the main loop                         97
 *   `dst + i * 0x18` instead of walking the pointer                       97
 *   `v` declared last / `i` declared first                                84
 *   a union with u32 and u8 views                                         93
 *   two scalars instead of an array (grows the frame)               111 insns
 *   `unsigned int i` for the main counter                                 88
 *
 * NEXT: nothing source-level outstanding after twelve probes. This is the
 * register-allocation class docs/elevation.md records as the dominant wall; the
 * quantitative test is whether &ewram_2002240's live-out pseudo can be given a
 * priority above the frame-address pseudo's in .17.lreg.
 *
 * ===================================================================
 * BATCH 335: THE FIGURE IS NOT A DISTANCE, THE ONE MISSING INSTRUCTION IS
 * LOCATED, AND THE SEPARATE-POINTER NEGATIVE IS NOW CONFIRMED ON A METRIC
 * THAT CANNOT SATURATE.
 * ===================================================================
 *
 * objcmp's own note on this park says it: the streams are EQUAL at 109 and the
 * 16-bit instruction counts are 104 against 103, so a PAD is absorbing the
 * difference and the positional figure measures MISALIGNMENT.  Every row of the
 * MEASURED AND INERT table above was scored with that saturating number, so a
 * row reading the same value as the base is not evidence of inertness.  The
 * non-saturating view (tools/aligncmp.py, alignment-tolerant, no masking) puts
 * this body at 78 of 109 aligned-equal, 36 differing in 24 hunks, and that is
 * the number to move.
 *
 * THE ONE MISSING INSTRUCTION IS THE ARGUMENT'S SECOND COPY.  The reference
 * copies the incoming destination pointer TWICE -- once at the very top, into a
 * low register that survives the swap loop, and once again into the high
 * register the main loop walks:
 *     mov r4, r0        (before the swap loop)
 *     ... swap loop, which reuses r0 for the zero it stores ...
 *     mov r12, r4       (immediately before the main loop)
 * We emit only the second step, straight from the argument register.  So the
 * requirement is TWO pseudos for one value, with the copy between them
 * surviving -- the shape batch 334 established on Func_8006384's mask copies
 * and on Func_8006088's accumulator, where what decides it is a live-range
 * CONFLICT and not a spelling.
 *
 * AND THE SEPARATE POINTER DOES NOT CREATE ONE.  Re-measured on aligncmp, a
 * named walking destination initialised from the parameter is EXACTLY INERT at
 * all three positions tried -- declared first, declared last, assigned
 * immediately before the main loop -- all 78 of 109 aligned-equal with the
 * instruction count still 103.  The copy is coalesced away because the
 * parameter is dead at the assignment, so the two never conflict.  The
 * untried dimension is a spelling in which the parameter is still LIVE after
 * the walking pointer is initialised.
 *
 * THE `sub r3, r1, #4` DERIVE IS NOT THE MISSING INSTRUCTION.  It is one
 * instruction against our one pool load, so it costs nothing in count; it is a
 * cse effect on the base address, and the reference genuinely pools BOTH
 * symbols (its own source reads `ldr r1, =ewram_2002244` beside
 * `ldr r3, =ewram_2002240`), so folding the two into one struct with the u32 at
 * offset 4 would change our relocations away from the reference's.  It is a
 * shape question for pass 3, not a length one.
 *
 * SHARED MECHANISM WITH THE PIECE-MATE.  This park and
 * src/non_matching/rom_c0/8006240.c sit on opposite sides of ONE lever: which
 * of two nearby absolute addresses gets a pool word and which is derived at
 * runtime.  There the reference pools two control-register addresses and
 * derives the third as a +2 offset while we pooled the offset one; here the
 * reference pools one struct address and derives the other two offsets from
 * it while we re-pool.  A runtime derive proves ONE live base pseudo and a
 * pool word proves a second independent one, which is why the piece's two
 * members read in opposite directions from the same cause.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c0/800615c.c \
 *     asm/rom_c0/rom_5cf8_a_a_a_c_c_c.s --func Func_800615c
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

struct Snd {
    /* 0x00 */ unsigned char f0;
    /* 0x01 */ unsigned char f1;
    /* 0x02 */ unsigned char f2;
    /* 0x03 */ unsigned char f3;
    /* 0x04 */ unsigned char pad4[0x3c];
    /* 0x40 */ unsigned short *f40[4];
    /* 0x50 */ unsigned short *f50[4];
};

extern struct Snd ewram_2002240;
extern u32 ewram_2002244;

int Func_800615c(unsigned char *dst)
{
    unsigned char v[8];
    unsigned short *q;
    unsigned short *t;
    int sum;
    int i;
    int k;
    unsigned int j;
    unsigned char c;

    *(u32 *)(v + 4) = 0;
    REG_IME = 0;
    for (k = 0; k < 4; k++) {
        t = ewram_2002240.f50[k];
        ewram_2002240.f50[k] = ewram_2002240.f40[k];
        ewram_2002240.f40[k] = t;
    }
    *(u32 *)v = ewram_2002244;
    ewram_2002244 = 0;
    SET_IO(REG_IME, 1);
    ewram_2002240.f3 = 0;
    for (i = 0; i < 2; i++) {
        q = ewram_2002240.f50[i];
        sum = 0;
        for (j = 0; j < 14; j++)
            sum += q[j];
        c = v[i];
        if (c == 1 && (short)sum == -1) {
            DMA3_SET(ewram_2002240.f50[i] + 2, dst, 0x84000006);
            ewram_2002240.f3 |= c << i;
        }
        if ((short)sum == -1)
            ewram_2002240.f50[i][1] = ~ewram_2002240.f50[i][1];
        dst += 0x18;
    }
    ewram_2002240.f2 |= ewram_2002240.f3;
    return ewram_2002240.f3;
}

/* Func_8006088 (0x08006088) -- NON-MATCHING.
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

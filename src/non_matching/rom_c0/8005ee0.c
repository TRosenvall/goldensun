/* Func_8005ee0 (0x08005ee0) -- NON-MATCHING.  19 encodings of 108 differ
 * (objcmp: "ENCODINGS differ in 19 place(s) (ref 108, ours 108)").
 * Blocker class: register allocation -- the SAME one-`mov` tail as the parked
 * sibling Func_8006088 (src/non_matching/rom_c0/8006088.c).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c0/8005ee0.c asm/rom_c0/rom_5cf8_a_a_a_c_c_a.s --func Func_8005ee0
 *
 * Everything up to the status-word tail is EXACT (first 80 lines of 105). The
 * ROM builds the status word in r2 and copies it into r0 before the f9 test:
 *
 *     rom    ldrb r3, [r7, #9] / mov r0, r2 / cmp r3, #0 / beq / ... orr r0, r3
 *
 * With `r = v; if (f9) r |= 0x1000;` (the natural spelling, 22 differ, one
 * insn SHORT) global.c's expand_preferences propagates r's r0 preference to v
 * through the dying copy, so both land in r0 and the mov disappears. The
 * if/else spelling below (19 differ, right length) gets v into r2 but puts the
 * copy in the else arm instead of before the test.
 *
 * LEVERS THAT WERE REAL (keep them):
 *   - `(u8)(cnt & 0x30)` and `m = (u8)(cnt & 0x88)`: the ROM ties the AND's
 *     destination to a COPY of cnt (`mov r3, r5 / and r3, r2`), as it does for
 *     the visibly-narrowed `(u8)(cnt & 4)`. Unnarrowed, gcc ties it to the
 *     constant's register instead (`mov r3, #0x30 / and r3, r5`).  93 -> 27.
 *   - REG_IE through an `int t` and `one = 1` / `&= one - 0x42`: blocker 1b --
 *     SImode values give the ROM's `mov #0x81 / neg`, `mov #0x40`, `mov #1`
 *     and `sub r3, #0x42` instead of halfword pool loads.
 *   - `do {} while (0);` on BOTH sides of `t = REG_IE;` pins `ldr r1, =REG_IE`
 *     after the IME store and `mov r3, #0x81` after the ldrh.  27 -> 23.
 *   - `v = f3 | (f2 << 8)` operand order gives the ROM's ldrb order.  23 -> 22.
 *   - SIOCNT through a `vu32 *` local (reused for the byte RMW at +1); the
 *     struct accessed as a bare global (the ROM reloads its address after the
 *     calls).
 *
 * INERT for the tail mov (all 22 unless noted): `r = v` then `r = v | 0x1000`
 * / `r = 0x1000 | v` / `r |= 0x1000; else r = v`; `do{}while(0)` before or
 * after the test; ternary `v | (f9 ? 0x1000 : 0)`; ternary for the 0x80 OR; the
 * first part in a `static inline` returning the word; `int v`. Worse: `u16 v`
 * (53), `u16` return type (22, +2 insns zero-extend), `u16 r` (26).
 */
#include "gba/types.h"
#include "gba/io.h"

struct SndState {
    /* 0x00 */ unsigned char f0;
    /* 0x01 */ unsigned char f1;
    /* 0x02 */ unsigned char f2;
    /* 0x03 */ unsigned char f3;
    /* 0x04 */ unsigned char f4[4];
    /* 0x08 */ unsigned char f8;
    /* 0x09 */ unsigned char f9[2];
    /* 0x0b */ unsigned char fb;
    /* 0x0c */ unsigned char fc[8];
    /* 0x14 */ int f14;
    /* 0x18 */ unsigned char f18[0x10];
    /* 0x28 */ unsigned char *f28;
};

extern struct SndState ewram_2002240;
extern void Func_800615c(void *p);
extern void Func_80060e8(const void *src);

int Func_8005ee0(const void *src, void *dst)
{
    vu32 *sio = (vu32 *)&REG_SIOCNT;
    unsigned int cnt;
    unsigned int v;
    int r;

    cnt = *sio;
    switch (ewram_2002240.f1) {
    case 0:
        if ((u8)(cnt & 0x30) == 0) {
            unsigned int m = (u8)(cnt & 0x88);
            if (m != 8)
                goto done;
            if ((u8)(cnt & 4) == 0 && ewram_2002240.f14 == -1) {
                int one;
                int t;
                REG_IME = (u8)(cnt & 4);
                do {} while (0);
                t = REG_IE;
                do {} while (0);
                t &= ~0x80;
                t |= 0x40;
                REG_IE = t;
                one = 1;
                REG_IME = one;
                ((vu8 *)sio)[1] &= one - 0x42;
                REG_IF = 0xc0;
                *(vu32 *)&REG_TM3CNT_L = 0xc963;
                ewram_2002240.f0 = m;
            }
        }
        ewram_2002240.f1 = 1;
    case 1:
        Func_800615c(dst);
        Func_80060e8(src);
        break;
    }
done:
    ewram_2002240.fb++;
    v = ewram_2002240.f3 | (ewram_2002240.f2 << 8);
    if (ewram_2002240.f0 == 8)
        v |= 0x80;
    if (ewram_2002240.f9[0] != 0)
        r = v | 0x1000;
    else
        r = v;
    if (((cnt << 26) >> 30) > 1)
        r |= 0x2000;
    return r;
}

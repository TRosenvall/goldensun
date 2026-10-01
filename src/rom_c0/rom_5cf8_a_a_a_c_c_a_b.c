/* ===================== BATCH 316c -- Func_8005ee0: BYTE-IDENTICAL ============
 * 19 of 108 -> 0 of 108.  236 bytes, 108 encodings and 4 relocations identical.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_c0/rom_5cf8_a_a_a_c_c_a_b.c \
 *     asm/rom_c0/rom_5cf8_a_a_a_c_c_a.s --func Func_8005ee0
 *
 * LANDING SHAPE: a TEXT-ONLY split.  tools/datacheck.py prints nothing -- no
 * data section.  split_s.py --dry-run on asm/rom_c0/rom_5cf8_a_a_a_c_c_a.s with
 * Func_8005ee0 writes _b.s (this function, 120 lines) and _c.s (the two that
 * follow, 149 lines); there is no _a.s because this is the first function in the
 * file.  Installed path is src/rom_c0/rom_5cf8_a_a_a_c_c_a_b.c.
 *
 * SHIM: ONE register pin, `register unsigned int v __asm__("r2")`.
 * tools/shimcount.py counts it ("register pins : 1") and says the file needs a
 * fakematch.txt row.  It does.
 *
 * ---------------------------------------------------------------------------
 * THE PARK HAD THE MECHANISM RIGHT AND TESTED IT ONE CHANGE AT A TIME.
 *
 * The park's analysis is correct and worth keeping: the ROM's tail is
 *     ldrb r3, [r7, #9] / mov r0, r2 / cmp r3, #0 / beq / ... orr r0, r3
 * and with the natural spelling `r = v; if (f9) r |= 0x1000;` global.c's
 * expand_preferences propagates r's r0 preference to v across the dying copy,
 * both land in r0, and the `mov r0, r2` disappears -- 22 differing, ONE
 * INSTRUCTION SHORT.  The park then reached for the if/else spelling, which
 * keeps the length (19 differing) but emits the copy in the ELSE ARM instead of
 * before the test.  Measured here, the parked body's 19 is a pure
 * SIX-SLOT DISPLACEMENT of one instruction: indices 89-107 are the ROM's
 * stream with `adds r0, r2, #0` moved from index 89 to index 95.
 *
 * WHAT CLOSES IT: the natural spelling AND one pin, together.
 *   natural spelling alone          22  (WORSE than the park, and 4 bytes short)
 *   pin on v alone, if/else body    19  (exactly INERT -- the park's own figure)
 *   both                             0
 * Neither edit is worth anything by itself and one of them looks like a clear
 * regression.  This is the brief's "pins jointly load-bearing, individually
 * inert" row, and a one-at-a-time search over either axis cannot see it.
 *
 * WHY THE PIN IS THE RIGHT SHAPE RATHER THAN A WORKAROUND.  The defect is
 * exactly that v and r must NOT share r0, and expand_preferences will always
 * make them share while v dies into r and neither conflicts.  Pinning v to r2
 * removes v from the allocno set entirely, so there is no preference left to
 * propagate, and the copy survives where the ROM has it -- in the common path,
 * before the f9 test.  Pinning r to r0 as well is also 0, so the MINIMAL pin
 * set is ONE and this file ships that.  (Pinning r alone is 22: it is the v side
 * that matters.)
 *
 * ALSO MEASURED ON THE NATURAL SPELLING, ALL 22 (so all inert):
 *   `r = r | 0x1000` instead of `r |= 0x1000`;  `r += 0x1000`;  `int v` instead
 *   of `unsigned int v`.
 * WORSE: `volatile int r` -- 111 of 108, +16 bytes, relocations differ.
 *
 * The parked header's other levers are all still in this file and all still
 * load-bearing; nothing below them changed.  Its sibling park Func_8006088
 * (src/non_matching/rom_c0/8006088.c) is recorded as having the SAME one-`mov`
 * tail, so this pin is the first thing to try there.
 *
 * -- scratch_elev/b316c/v_p3
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
    register unsigned int v __asm__("r2");
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
    r = v;
    if (ewram_2002240.f9[0] != 0)
        r |= 0x1000;
    if (((cnt << 26) >> 30) > 1)
        r |= 0x2000;
    return r;
}

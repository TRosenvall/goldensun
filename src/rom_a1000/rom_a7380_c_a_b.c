/* Func_80a847c -- 0x080a847c, split out of asm/rom_a1000/rom_a7380_c_a.s.
 *
 * Picks the row geometry for a menu layout and hands it to Func_80a2268.
 * Layout 0 walks the flag bytes and takes the row width from .Laf2fc; the other
 * layouts use a fixed pair chosen on a threshold.
 *
 * FOUR LEVERS, 36 differing to exact, and three are recorded ones:
 *
 *   1. THE FUNCTION IS void. The ROM sets no return value -- its tail is
 *      `pop {r5, r6, r7} / pop {r0} / bx r0` with no `mov r0, #0` -- so a
 *      `return 0` is one instruction too many AND moves the pop to r1. The
 *      usual rule run backwards: `pop {r0}` means no return value.
 *   2. THE INEQUALITY MUST BE A NAMED VALUE. The ROM computes it branchlessly:
 *      `eor r3, r2 / neg r2, r3 / orr r2, r3 / lsr r2, #31`, which is gcc's
 *      `sne` expansion. Written inline as `0xf - (d != 1)` gcc BRANCHES instead
 *      (`cmp / beq / mov #14 / b / mov #15`), two instructions short and seven
 *      encodings out. Assigning it to a local first is what makes it a value
 *      rather than a condition.
 *   3. NAME THE COMPUTED ARGUMENT TOO. With `t` named but `0xf - t` written at
 *      the call, gcc schedules the `base->f24` LOAD ahead of the arithmetic
 *      where the ROM does it after -- five encodings. Naming `u = 0xf - t`
 *      before the call pushes the load back to argument setup, and that is
 *      exact. Naming the LOAD instead (`v = base->f24`) does not work and stays
 *      at five: it is the arithmetic that has to move, which is the batch-265
 *      sched2 result again.
 *   4. BRANCH POLARITY. `else if (b > 3)` gives `ble` plus an extra `add`;
 *      `else if (b <= 3)` with the arms swapped gives the ROM's `bgt`.
 *
 * THE LOOP IS ORDINARY. The ROM's first iteration is peeled and its increment
 * sits at the top, which reads like hand-written control flow; it is just gcc
 * rotating
 *
 *     for (i = 0; i <= 4; i++)
 *         if (p[i] != 0) { if (b == k) { z = tbl[i]; break; } k++; }
 *
 * and both the peel and the `bgt` bound fall out of it.
 *
 * tools/datacheck.py confirms the source .s carries no data section; .Laf2fc
 * lives in rom_a7380_c_c.s and is already exported there.
 *
 * EXACT: 140 bytes, 67 encodings, 3 relocations, measured three times.
 */
#include "gba/types.h"

extern unsigned char *iwram_3001f2c;
extern unsigned char L_af2fc[] __asm__(".Laf2fc");
extern void Func_80a2268(int a, int b, int c, int d, int e, int f);

void Func_80a847c(int a, int b, u8 *p, int d)
{
    u8 *base;
    int x;
    int y;
    int z;
    int i;
    int k;
    int t;
    int u;

    base = iwram_3001f2c;
    if (a == 0) {
        x = b * 2 + 5;
        y = 0;
        z = 5;
        k = 0;
        for (i = 0; i <= 4; i++) {
            if (p[i] != 0) {
                if (b == k) {
                    z = L_af2fc[i];
                    break;
                }
                k++;
            }
        }
    } else if (b <= 3) {
        x = b;
        y = 5;
        z = 0xd;
    } else {
        x = b + 4;
        y = 8;
        z = 0x14;
    }
    t = (d != 1);
    u = 0xf - t;
    Func_80a2268(*(int *)(base + 0x24), y, x, z, 1, u);
}

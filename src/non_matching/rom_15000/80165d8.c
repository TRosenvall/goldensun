/* Func_80165d8 -- 0x080165d8, asm/rom_15000/rom_15e8c_a_c_c_c_a.s
 * (two functions; tools/datacheck.py confirms no data section).
 *
 * BLOCKER CLASS: a three-way register rotation among three single-use
 * parameters. SIZE EXACT ON THE FIRST CANDIDATE -- 74 instructions against 74 --
 * with 30 encodings differing.
 *
 * WHAT IT DOES: finds the first free slot of three in the 0x28-byte table at
 * iwram_3001e8c + 0x620, fills eleven fields, then copies four halfwords in
 * from the caller's buffer or zeroes them.
 *
 * THE READING IS RIGHT. Instruction count, both copy loops, the slot scan and
 * every field offset reproduce. Two details settled on the way:
 *
 *   * `ldr r2, =0` in the zeroing arm is a POOLED ZERO, and it is the recorded
 *     halfword exception rather than a symbol: the store is `strh`, gcc narrows
 *     the constant to HImode, and HImode constants go to the pool. `*p = 0`
 *     through a `u16 *` reproduces it with no help.
 *   * The slot scan checks THREE slots, not four. It advances and tests the
 *     counter together (`add r1,#1 / add r4,#0x28 / cmp r1,#3 / beq`), so the
 *     fourth slot is stepped onto but never read.
 *
 * THE RESIDUE. The ROM holds the second parameter in r12, the fourth in r7 and
 * the first in r6; we get them in r7, r6 and ip. Each is stored exactly once,
 * so the three have identical reference counts and differ only in live length,
 * and the rotation follows from that.
 *
 * It cascades into the store ORDER, which is why 30 encodings move for what is
 * really one decision: THUMB `strh` CANNOT TAKE A HIGH REGISTER, so the ROM's
 * value in r12 needs `mov r3, r12` before its store and gets scheduled where
 * that mov fits, while ours sits in r7 and stores directly (`strh r7, [r0,#18]`)
 * so gcc moves it later. Every field offset is still correct on both sides.
 *
 * MEASURED AND INERT OR WORSE: `i < 3` instead of `i != 3` (32), an early
 * `return` instead of the guarded block (30, unchanged), and reassigning the
 * shifted parameter in place (62 -- much worse; the ROM computes `c << 8` into
 * a scratch and keeps the parameter).
 *
 * NEXT: this is the same allocation-order question as Func_80f6038 and
 * GetLocationName, in its smallest form yet -- THREE values, one use each,
 * nothing else competing. If a hypothesis about allocno ordering is ever
 * testable, test it here first.
 */
#include "gba/types.h"

struct Slot {
    int f0;
    u16 f4;
    u16 f6;
    u16 f8[4];
    u16 f10;
    u16 f12;
    u16 f14;
    u16 f16;
    u16 f18;
    u16 f1a;
    u16 pad1c;
    u16 f1e;
    u16 f20;
    u16 pad22;
    u16 f24;
    u16 pad26;
};

extern unsigned char *iwram_3001e8c;

void Func_80165d8(int a, int b, int c, int d, u16 *src, int f)
{
    struct Slot *e;
    struct Slot *s;
    u16 *p;
    int i;

    e = (struct Slot *)(iwram_3001e8c + (0xc4 << 3));
    s = NULL;
    for (i = 0; i != 3; i++) {
        if (e->f0 == 0) {
            s = e;
            break;
        }
        e = (struct Slot *)((u8 *)e + 0x28);
    }
    if (s != NULL) {
        s->f1e = c << 8;
        s->f4 = c << 8;
        s->f6 = d << 8;
        s->f12 = b;
        s->f16 = 0xf;
        s->f1a = 0xa;
        s->f0 = a;
        s->f14 = 0;
        s->f18 = 0;
        s->f20 = 0;
        s->f24 = f;
        if (src != NULL) {
            p = s->f8;
            for (i = 0; i <= 3; i++) {
                *p = *src;
                src++;
                p++;
            }
        } else {
            p = s->f8;
            for (i = 0; i <= 3; i++) {
                *p = 0;
                p++;
            }
        }
        s->f10 = 0;
    }
}

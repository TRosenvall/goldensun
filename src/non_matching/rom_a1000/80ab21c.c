/* Func_80ab21c (FillTilemapRect, 0x080ab21c) -- NON-MATCHING, 2 differing of 101.
 * Blocker class: sched2 PRIORITY. Never attempted before batch 276.
 *
 * asm/rom_a1000/rom_aa538_c_c_a_c_c.s (3 functions, so landing needs a split).
 *
 * 107 lines against the ROM's 107, EVERY REGISTER IDENTICAL, and the two differing
 * encodings are one adjacent swap at the outer-loop tail:
 *
 *     rom   strb r2,[r1] / sub r5,#1     / ldr r3,[sp]
 *     ours  strb r2,[r1] / ldr r3,[sp]   / sub r5,#1
 *
 * PRICED FROM `.23.sched2` WITH -fsched-verbose=6, block 16. insn 167 (`r5 = r5 - 1`,
 * the `h--`) has PRIO 2; insn 291 (`r3 = [sp]`, the `off` reload) has PRIO 4. Both
 * are ready at t=9 and rank_for_schedule takes 291. Both orders give total time 16,
 * so nothing shortens 291's chain (291 -> 170 -> 294) or lengthens 167's
 * (167 -> 177). A priority difference rank_for_schedule decides before any
 * tie-break is not reachable from source order, and eight statement orders confirm
 * it below.
 *
 * FOUR LEVERS GOT IT HERE, and two of them are new.
 *
 * 1. A REASSIGNED PARAMETER IS A GLOBAL ALLOCNO. `fill <<= 12;` reproduces the
 *    ROM's `mov rHi, rArg / mov rLo, rHi / lsl #0xc / mov rHi, rLo` where
 *    `tile = fill << 12;` collapses to one `lsl` plus one `mov`. 107/107 at 34
 *    differing, from 106 lines. This is "assigned ONCE is local-alloc, TWICE is
 *    global-alloc" applied to a PARAMETER -- the parameter's incoming value is the
 *    first assignment, so one reassignment is enough.
 *
 * 2. A HImode STORE TARGET TRUNCATES A MASK IN THE RHS. `*p = (v & 0xffff0fff) | x;`
 *    through a `u16 *` emits `& 0xfff` -- gcc narrows the mask to the store's mode.
 *    Assigning into an SImode local FIRST and storing that keeps the ROM's 32-bit
 *    mask. WHERE THE ROM MASKS WITH A VALUE WIDER THAN THE STORE, LOOK AT THE
 *    STORE'S MODE, not at nonzero-bits.
 *
 * 3. `unsigned` FOR `lsr` AND MASK PRESERVATION ARE COUPLED. `unsigned v` alone
 *    narrows the mask; `int v` alone gives `asr`; `unsigned v` PLUS the separate
 *    SImode assignment statements gives both. Splitting the masked store into
 *    `v = v & 0xffff0fff; v = v | fill; *p = v;` took it to 5 differing.
 *
 * 4. NAMING THE DIRTY POINTER BEFORE THE OFFSET. `d = base + 0xea3;` written
 *    BEFORE `off = ...` is 2 differing; after it, 5. The ROM hoists this invariant
 *    address into the preheader and wants its pool load FIRST there, and LICM
 *    appends rather than prepends -- so it has to be named in source order.
 *
 *    READ THAT AGAINST Func_8016f2c, elevated in this same round
 *    (src/rom_15000/rom_15e8c_c_a_a_a_c_a_b.c), WHICH IS THE SAME LEVER WITH THE
 *    OPPOSITE SIGN: there the ROM RECOMPUTES `base + 0xea3` inside the loop, so a
 *    `goto` loop is needed to suppress LICM entirely. The question to ask of a ROM
 *    invariant address is not which loop form but WHETHER IT IS INSIDE OR OUTSIDE.
 *
 * MEASURED (rom 107 lines / 101 encodings):
 *   plain `*p = (v & 0xffff0fff) | tile`      105 lines  (mask truncated to 0xfff)
 *   `volatile u16 *p`                         106 /  94  (+lsl/lsr pair, +spill)
 *   `int v` + named `int t`                   106 /  88  (mask kept, but `asr`)
 *   `unsigned v` + named `int t`              106 /  88  (`lsr` AND mask kept, but
 *                                                         `t`'s allocno displaced
 *                                                         `w` from r7 to r12, +3 movs)
 *   `unsigned v` reassigned to itself         105 /  96
 *   `fill <<= 12` instead of a new `tile`     107 /  34
 *   + split masked store                            5
 *   + `d` named before `off`                        2   <- best, below
 *
 * INERT ON THE LAST SWAP, all still 2 unless noted: `volatile u8 *d`; `unsigned off`;
 * a named `int k` for the shift value; `} while (--h != 0)`; `off += / y++ / h--`;
 * `h-- / y++ / off +=` (4); `while (h > 0)` (3).
 *
 * NEXT: nothing. The scheduler's own arithmetic says both orders cost 16 cycles and
 * 291 outranks 167 on priority, which source order does not reach. This is the same
 * terminal shape as the recorded "an ANTI-dependence gives an insn no priority"
 * class -- a priced residue, not an open question. Do not spend another round here.
 */
typedef unsigned char u8;
typedef unsigned short u16;

extern u8 *iwram_3001e8c;

void Func_80ab21c(int x, int y, int w, int h, int fill)
{
    u8 *base;
    u16 *p;
    int off;
    u8 *d;
    int n;
    unsigned int v;

    base = iwram_3001e8c;
    fill <<= 12;
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (x + w > 0x1d)
        w = 0x1e - x;
    if (y < 0) {
        h += y;
        y = 0;
    }
    if (y + h > 0x1d)
        h = 0x14 - y;
    if (w > 0 && h > 0) {
        d = base + 0xea3;
        off = (y << 6) + (x << 1);
        do {
            p = (u16 *)(base + off);
            n = w;
            while (n != 0) {
                v = *p;
                if (((v >> 12) & 0xf) == 0xf) {
                    v = v & 0xffff0fff;
                    v = v | fill;
                    *p = v;
                }
                n--;
                p++;
            }
            *d |= 2 << ((unsigned)y >> 2);
            h--;
            off += 0x40;
            y++;
        } while (h != 0);
    }
}

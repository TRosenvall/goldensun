// fakematch
/* ovl_30_a_a_a_c_a.c  --  OvlFunc_943_20080c4 + OvlFunc_943_20082ec
 *   [the WHOLE of asm/overlays/rom_7c7b9c/ovl_30_a_a_a_c_a.s -- both
 *    `.thumb_func_start`s, so NO SPLIT is required and
 *    overlays/rom_7c7b9c/overlay.ld:19 stays VERBATIM as
 *    `asm/overlays/rom_7c7b9c/ovl_30_a_a_a_c_a.o(.text)`]
 *
 *   OK WHOLE TU -- 1104 bytes, 493 encodings and 48 relocations identical
 *
 * TWO CONSTANT-SUBSTITUTION TWINS, 231 + 232 instructions, and the EIGHTH time
 * the twin play has paid.  Sibling of ovl_30_a_a_c.c in the same overlay, which
 * is itself a solved twin pair -- read that file's header first; this one only
 * records where the two pairs DIFFER.
 *
 * Shape: a nine-phase NPC script on the `unsigned char` at +0x62, with phase 0
 * being not a script step but an IDLE DRIFT -- two independent random walks, one
 * on the 20.12 word at +0x4c bounded by the sign flag at +0x64, one on +0x0c
 * bounded by +0x66 -- and a common tail that re-arms the script with probability
 * 1/0x64.  Phases 1 and 5 launch an `__Actor_TravelTo`, 3 and 7 wait for the
 * three velocity words at +0x38/+0x3c/+0x40 to settle to 0x80000000, 2/4/6 just
 * bump the counter, 8 wraps to zero.  The second function IS the first with nine
 * constants changed and the actor slot 0x15 -> 0x16.
 *
 * THE RESIDUE WAS ENTIRELY ARGUMENT-SETUP ORDER, THE SIGN SAID SO, AND THE
 * UNIFORM SIX-PIN FILL CLOSED BOTH ON THE FIRST TRY.  The unpinned draft was
 * 246 encodings against 246 with RELOCATIONS SILENT and 17 differing -- the
 * recorded "ordering" band, nothing in the CSE band -- and every differing hunk
 * had the one shape: the ROM completes r0 before it finishes r1, we finish r1
 * first.  `-fno-schedule-insns2` REGRESSED, 100 diff lines to 184, which is the
 * recorded sign test answering "sched2 is already producing the ROM's order, so
 * alias is the wrong axis"; the lever is an ordering pin and nothing else.  Six
 * ascending pins in argument order took both twins from 17 differing to exact.
 *
 * THE PIN SET TRANSFERS HERE -- ALL SIX LOAD-BEARING IN BOTH -- AND THE REASON
 * REFINES THE PREDICTION RULE.  Single-drop minimisation, twelve compiles:
 *
 *     20080c4   011111 3   101111 2   110111 2   111011 2   111101 4   111110 4
 *     20082ec   011111 3   101111 2   110111 2   111011 2   111101 4   111110 4
 *
 * Identical, site for site.  That is the OPPOSITE of the sibling pair, where
 * 2008724 dropped site 3 -- and the difference is predictable from the constants
 * alone, before compiling, exactly as recorded.  The sibling's inert site had a
 * bare `mov #imm8` argument (`0`); here NO site is all-cheap.
 *
 * THE NEW DATUM IS SITE 4, the `__MapActor_Emote(slot, 0x103, 0)` whose second
 * argument is a POOL LOAD, one instruction, the same width as a bare `mov`.
 * Dropping its pin costs 2 differing, and the first differing encoding says why:
 *
 *     ROM   2015  mov r0, #0x15        ours  4950  ldr r1, [pc, #0x140]
 *
 * The pool load FLOATS ABOVE the `mov`, precisely as a `mov`+`lsl` pair does.
 * So the ordering pin's cheap/expensive partition is NOT "one instruction versus
 * two" -- it is the SAME partition the CSE-hoisting entry already draws ("pool
 * loads and shifted builds; bare `mov rN, #imm8` is rematerialised for free").
 * A pooled argument is EXPENSIVE and its pin is load-bearing.  Restated for the
 * pin: a site is inert only when every non-r0 argument is a bare `mov #imm8`.
 *
 * The pooled/synthesised split still shows up, just not in the pin set: phase
 * 5's TRUE arm passes `0x2920000` in 20080c4, which is `0x292 << 16` and so
 * cannot be built with `mov`+`lsl`, against `0x96 << 18` in 20082ec.  Both are
 * expensive, both pins required, and NEITHER twin cross-jumps the two
 * `__Actor_TravelTo` arms -- 2 relocations each -- because the shifts differ
 * (17/16 and 17/16) so the common suffix is one instruction.  The sibling pair's
 * "same pin suppresses the merge in one twin and creates it in the other" does
 * not fire here; the tell was, again, the relocation COUNT.
 *
 * THINGS THAT NEEDED NO LEVER, and one that explains the fifth push:
 *
 *  - `push {r5, r6, r7, lr}` + `mov r7, r8 / push {r7}`.  The r8 is NOT a split.
 *    `if (a->f62) { switch (a->f62) ... } else { ... }` leaves the loaded phase
 *    byte in a pseudo, cse.c's `record_jump_equiv` notes it is 0 on the else
 *    edge, and the plain `a->f64 = 0;` there is rewritten to use that register
 *    -- `mov r2, r8 / strh r2, [r7]`.  That one use is live across three
 *    `__Random` calls, so it lands in a high callee-saved register and the ROM's
 *    fifth push appears on the first draft.  Write `0`; do not try to split.
 *  - THE SAME CSE IS WHY ONE PAIR OF STORES CROSS-JUMPS AND THE OTHER DOES NOT.
 *    The +0x66 arms end `mov r3, #0 / b` and `mov r3, #1`, share `strh r3, [r7]`
 *    and jump.c merges them.  The +0x64 arms end `mov r2, r8` and `mov r3, #1`
 *    -- different registers, no common suffix, no merge.  Both fall out free.
 *  - `if (a->f62)` around a `switch` on cases 1..8 is what produces the ROM's
 *    `cmp r1, #0 / bne` ahead of `sub r3, #1 / cmp r3, #7 / bls` and the
 *    EIGHT-entry jump table.  A ten-case `switch` including 0 would emit one
 *    nine-entry table instead.
 *  - Offset 0x62 is past `ldrb`'s 31-byte immediate, so gcc parks `a + 0x62` in
 *    r6 for the whole function by itself; 0x64/0x66 likewise get a fresh r7 per
 *    block, and `a->f66` is reached as `add r3, #0x64 / add r3, #2` by cse's own
 *    address folding.  No local, no lever.
 *  - The two range guards fold to the ROM's unsigned form unaided: written as
 *    `a->f8 > 0xf80000 && a->f8 < 0x1240000`, gcc emits `add r3, r2, #-0xf80001`
 *    / `cmp` / `bhi` against 0x2bfffe.
 *  - `a->fc = a->fc - v - (0x80 << 8)` gives the ROM's single store with `sub`
 *    then `add` of the pooled -0x8000.  Writing `a->fc -= v + 0x8000` would
 *    build the sum first and is wrong.
 *
 * Harness: scratch_elev/b256/twins/ -- gen.py (both twins, six-bit pin mask),
 * sweep.sh (single-drop minimisation), dif.py (normalised side-by-side),
 * objcmp_all.py (whole-TU objcmp, since --func filters only the reference).
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    unsigned char pad10[0x28 - 0x10];
    int f28;
    unsigned char pad2c[0x30 - 0x2c];
    int f30;
    int f34;
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0x4c - 0x44];
    int f4c;
    unsigned char pad50[0x62 - 0x50];
    unsigned char f62;
    unsigned char f63;
    short f64;
    short f66;
};

extern unsigned int __Random(void);
extern void __PlaySound(int id);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Func_8092adc(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int id, int n);
extern struct Actor *__MapActor_GetActor(int slot);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define TPIN1 register struct Actor *q0 __asm__("r0")
#define TPIN2 TPIN1; register int q1 __asm__("r1")
#define TPIN3 TPIN2; register int q2 __asm__("r2")
#define TPIN4 TPIN3; register int q3 __asm__("r3")

int OvlFunc_943_20080c4(struct Actor *a)
{
    if (a->f62) {
        switch (a->f62) {
        case 1:
            a->f30 = 0x80 << 11;
            a->f34 = 0x80 << 10;
            { TPIN4; q0 = a; q1 = 0x86 << 17; q2 = 0xa0 << 13; q3 = 0xad << 18;
              __Actor_TravelTo(q0, q1, q2, q3); }
            a->f62++;
            break;
        case 2:
            a->f62++;
            break;
        case 3:
            if (a->f38 == 0x80 << 24 && a->f3c == 0x80 << 24 && a->f40 == 0x80 << 24) {
                a->f62++;
                __PlaySound(0x92);
                if (a->f63) {
                    { PIN3; q0 = 0x15; q1 = 0xd0 << 8; q2 = 0;
                  __Func_8092adc(q0, q1, q2); }
                } else {
                    { PIN3; q0 = 0x15; q1 = 0xb0 << 8; q2 = 0;
                  __Func_8092adc(q0, q1, q2); }
                }
                if ((__Random() << 2) >> 16) {
                    __MapActor_GetActor(0x15)->f28 = 0x80 << 10;
                } else {
                    { PIN3; q0 = 0x15; q1 = 0x103; q2 = 0;
                  __MapActor_Emote(q0, q1, q2); }
                    __MapActor_GetActor(0x15)->f28 = 0xc0 << 11;
                }
            }
            break;
        case 4:
            a->f62++;
            break;
        case 5:
            if (a->f63) {
                { TPIN4; q0 = a; q1 = 0x8d << 17; q2 = 0; q3 = 0x2920000;
              __Actor_TravelTo(q0, q1, q2, q3); }
            } else {
                { TPIN4; q0 = a; q1 = 0xfe << 16; q2 = 0; q3 = 0xa7 << 18;
              __Actor_TravelTo(q0, q1, q2, q3); }
            }
            a->f62++;
            break;
        case 6:
            a->f62++;
            break;
        case 7:
            if (a->f38 == 0x80 << 24 && a->f3c == 0x80 << 24 && a->f40 == 0x80 << 24) {
                a->f30 = 0x80 << 10;
                a->f34 = 0x80 << 9;
                a->f64 = 0;
                a->f66 = 0;
                a->f62++;
                a->f4c = 0;
            }
            break;
        case 8:
            a->f62 = 0;
            break;
        }
    } else {
        if (a->f64) {
            a->f4c -= (__Random() << 12) >> 16;
            if (a->f4c < -(0x80 << 7))
                a->f64 = 0;
        } else {
            a->f4c += (__Random() << 12) >> 16;
            if (a->f4c > 0x80 << 7)
                a->f64 = 1;
        }
        if (a->f8 > 0xf80000 && a->f8 < 0x1240000)
            a->f8 += a->f4c;
        if (a->f66) {
            a->fc = a->fc - ((__Random() << 15) >> 16) - (0x80 << 8);
            if (a->fc < 0)
                a->f66 = 0;
        } else {
            a->fc = a->fc + ((__Random() << 15) >> 16) + (0x80 << 8);
            if (a->fc > 0x80 << 12)
                a->f66 = 1;
        }
    }
    if (((__Random() * 0x64) >> 16) == 0)
        a->f62 = 1;
    return 1;
}
int OvlFunc_943_20082ec(struct Actor *a)
{
    if (a->f62) {
        switch (a->f62) {
        case 1:
            a->f30 = 0x80 << 11;
            a->f34 = 0x80 << 10;
            { TPIN4; q0 = a; q1 = 0x80 << 17; q2 = 0xa0 << 13; q3 = 0xa0 << 18;
              __Actor_TravelTo(q0, q1, q2, q3); }
            a->f62++;
            break;
        case 2:
            a->f62++;
            break;
        case 3:
            if (a->f38 == 0x80 << 24 && a->f3c == 0x80 << 24 && a->f40 == 0x80 << 24) {
                a->f62++;
                __PlaySound(0x92);
                if (a->f63) {
                    { PIN3; q0 = 0x16; q1 = 0xd0 << 8; q2 = 0;
                  __Func_8092adc(q0, q1, q2); }
                } else {
                    { PIN3; q0 = 0x16; q1 = 0xb0 << 8; q2 = 0;
                  __Func_8092adc(q0, q1, q2); }
                }
                if ((__Random() << 2) >> 16) {
                    __MapActor_GetActor(0x16)->f28 = 0x80 << 10;
                } else {
                    { PIN3; q0 = 0x16; q1 = 0x103; q2 = 0;
                  __MapActor_Emote(q0, q1, q2); }
                    __MapActor_GetActor(0x16)->f28 = 0xc0 << 11;
                }
            }
            break;
        case 4:
            a->f62++;
            break;
        case 5:
            if (a->f63) {
                { TPIN4; q0 = a; q1 = 0x84 << 17; q2 = 0; q3 = 0x96 << 18;
              __Actor_TravelTo(q0, q1, q2, q3); }
            } else {
                { TPIN4; q0 = a; q1 = 0xf2 << 16; q2 = 0; q3 = 0x97 << 18;
              __Actor_TravelTo(q0, q1, q2, q3); }
            }
            a->f62++;
            break;
        case 6:
            a->f62++;
            break;
        case 7:
            if (a->f38 == 0x80 << 24 && a->f3c == 0x80 << 24 && a->f40 == 0x80 << 24) {
                a->f30 = 0x80 << 10;
                a->f34 = 0x80 << 9;
                a->f64 = 0;
                a->f66 = 0;
                a->f62++;
                a->f4c = 0;
            }
            break;
        case 8:
            a->f62 = 0;
            break;
        }
    } else {
        if (a->f64) {
            a->f4c -= (__Random() << 12) >> 16;
            if (a->f4c < -(0x80 << 7))
                a->f64 = 0;
        } else {
            a->f4c += (__Random() << 12) >> 16;
            if (a->f4c > 0x80 << 7)
                a->f64 = 1;
        }
        if (a->f8 > 0xe80000 && a->f8 < 0x1100000)
            a->f8 += a->f4c;
        if (a->f66) {
            a->fc = a->fc - ((__Random() << 15) >> 16) - (0x80 << 8);
            if (a->fc < 0)
                a->f66 = 0;
        } else {
            a->fc = a->fc + ((__Random() << 15) >> 16) + (0x80 << 8);
            if (a->fc > 0x80 << 12)
                a->f66 = 1;
        }
    }
    if (((__Random() * 0x64) >> 16) == 0)
        a->f62 = 1;
    return 1;
}

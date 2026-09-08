// fakematch
/* ovl_30_a_a_c.c  --  OvlFunc_943_2008598 + OvlFunc_943_2008724
 *   [the WHOLE of asm/overlays/rom_7c7b9c/ovl_30_a_a_c.s -- both
 *    `.thumb_func_start`s, so NO SPLIT is required and
 *    overlays/rom_7c7b9c/overlay.ld:22 stays VERBATIM as
 *    `asm/overlays/rom_7c7b9c/ovl_30_a_a_c.o(.text)`]
 *
 *   OK WHOLE TU -- 772 bytes, 338 encodings and 43 relocations identical
 *
 * TWO CONSTANT-SUBSTITUTION TWINS, 156 + 153 instructions.  One ten-case state
 * machine per NPC, driven by the signed halfword at +0x64: phases 0/1/3/5/7 just
 * bump the counter, 2 and 6 launch an __Actor_TravelTo, 4 and 8 wait for the
 * three 20.12 velocity words at +0x38/+0x3c/+0x40 to settle to 0x80000000, and 9
 * wraps to zero.  The second function IS the first with eight constants changed
 * and the actor slot 0x15 -> 0x16; nothing else differs.  It screened EXACT on
 * the FIRST try after substitution.  FAKEMATCH: matched with the register-pin
 * idiom, so both names go in fakematch.txt.
 *
 * THE WHOLE RESIDUE WAS ARGUMENT-SETUP ORDER, AND THE SIGN SAID SO.  The first
 * unpinned draft was 67 encodings differing with the relocation list ONE ENTRY
 * SHORT, and every single differing hunk had the same shape: the ROM completes
 * r0 BEFORE it finishes r1, we finish r1 first.
 *
 *     ROM   mov r1, #0xb0 / mov r0, #0x15 / lsl r1, #8  / mov r2, #0
 *     ours  mov r1, #0xb0 / lsl r1, #8    / mov r0,#0x15 / mov r2, #0
 *
 * `-fno-schedule-insns2` REGRESSED, 69 diff lines to 98 -- it loses the ROM's
 * own `mov r2, #0x80 / ldr r3, [r5, #0x38] / lsl r2, #0x18` interleave at the
 * velocity guards.  That is the recorded sign test answering "sched2 is already
 * right, alias is the wrong axis": the residue is in what sched2 is HANDED, not
 * in what it does with it, so the lever is an ORDERING pin and nothing else.
 * Six uniform ASCENDING pins -- `q0 = ...; q1 = ...; q2 = ...` in argument order,
 * `register int qN __asm__("rN")` -- took 598 from 67 differing to exact.
 *
 * THE PIN SET DOES NOT TRANSFER, AND THE CONSTANTS PREDICT IT.  Both twins were
 * minimised to a fixpoint independently:
 *
 *     598  all SIX required   (single drops: 2, 2, 2, 2, 4, 4 differing)
 *     724  FIVE of six        (site 3 INERT -- `110111` is exact)
 *
 * The inert one is the case-4 `__Func_8092adc` FALSE arm.  Its second argument
 * is `0xa0 << 7` in 598 -- a `mov`+`lsl` PAIR, so ordering it matters -- and a
 * bare `0` in 724, a single all-cheap `mov #imm8` the scheduler cannot get
 * wrong.  That is the recorded prediction rule confirmed on a clean pair: you
 * can name the site that moves from the CONSTANTS ALONE, before compiling.
 * Every 598 drop lands at 2-4 differing with RELOCATIONS SILENT -- the recorded
 * "ordering pin" band, nothing in the CSE band at all.
 *
 * THE RECORDED "CROSS-JUMPING IS A SYMPTOM OF ARGUMENT ORDER" ENTRY, WITH THE
 * FIX IT SAYS IS UNAVAILABLE -- AND IT POINTS BOTH WAYS ON ONE SOURCE.
 * Case 6 is an if/else over two `__Actor_TravelTo` calls, and gcc's LAST jump
 * pass (cross-jumping runs after sched2) merges whatever identical suffix the
 * scheduler happens to leave.  THE ROM DISAGREES WITH ITSELF ACROSS THE TWINS:
 *
 *   598 does NOT merge -- 2 `__Actor_TravelTo` relocations.  Its r3 arguments
 *       are POOL LOADS (`ldr r3, =0x2860000` / `=0x2ae0000`), which differ, and
 *       the pins push them to the last slot before the `bl`, leaving a common
 *       suffix of 2 (`bl`, `b`) -- below gcc's threshold.  UNPINNED the pool
 *       load floats up, `mov r0, r5 / mov r2, #0 / bl / b` becomes common, the
 *       4-insn suffix merges, and the object is 8 bytes SHORT with one
 *       `__Actor_TravelTo` relocation missing.  That missing relocation was the
 *       tell, before any encoding was read.
 *
 *   724 DOES merge -- 1 `__Actor_TravelTo` relocation, a shared
 *       `mov r0, r5 / lsl r1, #16 / mov r2, #0 / lsl r3, #18 / bl / b` tail.
 *       Its r3 arguments are `mov`+`lsl` pairs whose SHIFTS COINCIDE (both 18),
 *       so the pinned order makes the suffix 6 and cross-jumping fires.  Drop
 *       EITHER case-6 pin and the symmetry breaks, the merge is lost, and the
 *       object is 8 bytes LONG with an extra relocation -- 57 differing.
 *
 * So the same ordering pin SUPPRESSES the merge in one twin and CREATES it in
 * the other, and which way it goes is decided by whether the fourth argument is
 * pooled or synthesised.  Both case-6 pins are required in both functions, and
 * they are the only two sites where a drop leaves the 2-4 band.
 *
 * The recorded entry ("Cross-jumping is a SYMPTOM of argument order", on
 * `Func_801bcd4`) has the mechanism exactly right -- jump.c merges common
 * SUFFIXES, and the ROM's argument order is what keeps the suffix short -- but
 * closes with "Here the fix is not available", because the only lever tried
 * there was the callee's RETURN TYPE, which moves r0 relative to r1-r3 and
 * nothing else.  THE `register ... __asm__` ORDERING PIN IS THE LEVER THAT
 * ENTRY WAS MISSING: it fixes the whole argument order, not just r0's place in
 * it, and it reaches the suffix length directly.  Func_801bcd4's shape -- the
 * ROM setting r2, then r0, then r1 -- is exactly what a three-pin ascending
 * fill CANNOT produce, but a pin set written in the ROM's own emitted order
 * (2, 0, 1) can, and that is worth one round on that function before it stays
 * parked.  Not attempted here; it is a different overlay.
 *
 * SHIPPED AS THE UNIFORM SIX-PIN FILL on both functions.  724's site 3 is inert
 * rather than harmful (measured exact with and without), and the uniform fill
 * keeps the twin relationship legible in the source.
 *
 * THINGS THAT NEEDED NO LEVER AT ALL, worth recording because the draft got
 * them free:
 *
 *  - `short f64` gives `ldrsh` for the `switch` and `ldrh` for `f64++`, the
 *    recorded same-member-both-signednesses rule, as in this overlay's sibling
 *    ovl_30_a_a_a_a.c.  Offset 0x64 is past the `ldrh` immediate range, so gcc
 *    parks `a + 0x64` in r6 for the whole function by itself and the push mask
 *    is the ROM's `{r5, r6, lr}` on the first draft -- no pressure lever needed.
 *  - The ten-way `switch` emits the ROM's jump table unaided; all ten cases are
 *    present and dense.
 *  - The velocity guard is written as three separate `== 0x80 << 24` tests and
 *    gcc rotates the constant through r2/r3 by itself, reproducing the ROM's
 *    `cmp r2, r3` / `cmp r3, r2` chain.  No local, no CSE flag.
 *  - `b<cond> near / b far` pairs are pure branch-range fixups: case 0's exit is
 *    0x12c bytes away and case 4's is 0xe6, which is why one is split and the
 *    other is not.  Nothing in the source distinguishes them.
 *  - `a->f63` is read as `mov r3, r5 / add r3, #0x63 / ldrb r3, [r3]` because
 *    0x63 is past `ldrb`'s 31-byte immediate.  Also free.
 *
 * Harness: scratch_elev/b255/a4/ -- gen.py (one function, six-bit pin mask),
 * combine.py, sweep.sh (single-drop minimisation), dif.py (normalised
 * side-by-side against the .s), objcmp_all.py (whole-TU objcmp, since --func
 * filters only the reference).
 */
struct Actor {
    unsigned char pad00[0x28];
    int f28;
    unsigned char pad2c[0x30 - 0x2c];
    int f30;
    int f34;
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0x63 - 0x44];
    unsigned char f63;
    short f64;
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

int OvlFunc_943_2008598(struct Actor *a)
{
    switch (a->f64) {
    case 0:
        if (((__Random() * 40) >> 16) == 0)
            a->f64++;
        break;
    case 1:
        a->f64++;
        break;
    case 2:
        a->f28 = 0x80 << 11;
        a->f30 = 0x80 << 11;
        a->f34 = 0x80 << 10;
        { TPIN4; q0 = a; q1 = 0x84 << 17; q2 = 0; q3 = 0x2960000;
          __Actor_TravelTo(q0, q1, q2, q3); }
        a->f64++;
        break;
    case 3:
        a->f64++;
        break;
    case 4:
        if (a->f38 == 0x80 << 24 && a->f3c == 0x80 << 24 && a->f40 == 0x80 << 24) {
            a->f64++;
            __PlaySound(0x98);
            if (a->f63) {
                { PIN3; q0 = 0x15; q1 = 0xb0 << 8; q2 = 0;
                  __Func_8092adc(q0, q1, q2); }
            } else {
                { PIN3; q0 = 0x15; q1 = 0xa0 << 7; q2 = 0;
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
    case 5:
        a->f64++;
        break;
    case 6:
        a->f64++;
        a->f28 = 0x80 << 11;
        a->f30 = 0x80 << 10;
        a->f34 = 0x80 << 9;
        if (a->f63) {
            { TPIN4; q0 = a; q1 = 0xfc << 16; q2 = 0; q3 = 0x2860000;
              __Actor_TravelTo(q0, q1, q2, q3); }
        } else {
            { TPIN4; q0 = a; q1 = 0x80 << 17; q2 = 0; q3 = 0x2ae0000;
              __Actor_TravelTo(q0, q1, q2, q3); }
        }
        break;
    case 7:
        a->f64++;
        break;
    case 8:
        if (a->f38 == 0x80 << 24 && a->f3c == 0x80 << 24 && a->f40 == 0x80 << 24)
            a->f64++;
        break;
    case 9:
        a->f64 = 0;
        break;
    }
    return 1;
}

int OvlFunc_943_2008724(struct Actor *a)
{
    switch (a->f64) {
    case 0:
        if (((__Random() * 40) >> 16) == 0)
            a->f64++;
        break;
    case 1:
        a->f64++;
        break;
    case 2:
        a->f28 = 0x80 << 11;
        a->f30 = 0x80 << 11;
        a->f34 = 0x80 << 10;
        { TPIN4; q0 = a; q1 = 0xb0 << 16; q2 = 0; q3 = 0xae << 18;
          __Actor_TravelTo(q0, q1, q2, q3); }
        a->f64++;
        break;
    case 3:
        a->f64++;
        break;
    case 4:
        if (a->f38 == 0x80 << 24 && a->f3c == 0x80 << 24 && a->f40 == 0x80 << 24) {
            a->f64++;
            __PlaySound(0x98);
            if (a->f63) {
                { PIN3; q0 = 0x16; q1 = 0xd0 << 8; q2 = 0;
                  __Func_8092adc(q0, q1, q2); }
            } else {
                { PIN3; q0 = 0x16; q1 = 0; q2 = 0;
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
    case 5:
        a->f64++;
        break;
    case 6:
        a->f64++;
        a->f28 = 0x80 << 11;
        a->f30 = 0x80 << 10;
        a->f34 = 0x80 << 9;
        if (a->f63) {
            { TPIN4; q0 = a; q1 = 0xb8 << 16; q2 = 0; q3 = 0xa8 << 18;
              __Actor_TravelTo(q0, q1, q2, q3); }
        } else {
            { TPIN4; q0 = a; q1 = 0xca << 16; q2 = 0; q3 = 0xad << 18;
              __Actor_TravelTo(q0, q1, q2, q3); }
        }
        break;
    case 7:
        a->f64++;
        break;
    case 8:
        if (a->f38 == 0x80 << 24 && a->f3c == 0x80 << 24 && a->f40 == 0x80 << 24)
            a->f64++;
        break;
    case 9:
        a->f64 = 0;
        break;
    }
    return 1;
}

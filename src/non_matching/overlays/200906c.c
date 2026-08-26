/* OvlFunc_971_200906c -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7fb4a8/ovl_30_c_a_a_a_c.s
 *
 * BLOCKER CLASS: three NEARBY constants chosen by a switch get built by
 * add/sub from one pool load.
 *
 * The ROM selects one of 0x297f / 0x2982 / 0x2985 -- a base + 0/3/6 family --
 * and loads each from its own pool entry:
 *
 *      cmp r5, #0xd / beq .L1086        <- case 0xd
 *      cmp r5, #0xd / bgt .L108a        <- default
 *      cmp r5, #0xc / bne .L108a        <- default
 *      ldr r7, =0x2985  b .L108c        <- case 0xc, fallthrough
 *  .L1086: ldr r7, =0x297f  b .L108c
 *  .L108a: ldr r7, =0x2982
 *
 * Every spelling we compile shares ONE pool load and reaches the others with
 * an immediate: `ldr r7, =0x2985 / sub r7, #6`. This is the branchless-nearby-
 * constant behaviour already recorded in docs/elevation.md, here applied
 * across the arms of a switch rather than across the arms of an if.
 *
 * WHAT WAS TRIED (43-51 lines against the ROM's 49, 42-47 differing):
 *   - switch with cases 0xc, 0xd, default            (43 lines, 45 differ)
 *   - the same with `default:` written first          (identical output)
 *   - if / else-if / else, 0xd tested first           (43 lines, 44 differ)
 *   - if (slot != 0xc && slot != 0xd) first           (45 lines, 46 differ)
 *   - an extra `case 0xe:` carrying the default value (51 lines, 46 differ)
 *   - all of the above at -O1                         (45-46 lines, 42 differ)
 *   - -fno-gcse, -fno-rerun-cse-after-loop,
 *     -fno-cse-follow-jumps, -fno-cse-skip-blocks,
 *     -fno-thread-jumps, -fno-expensive-optimizations (no flag separates them)
 *
 * WHAT WAS LEARNED AND IS WORTH KEEPING -- gcc-2.96 emits the switch DECISION
 * TREE only when the switch has THREE OR MORE case labels. With two labels it
 * emits a plain equality chain:
 *
 *      two cases:    cmp #0xc / beq   cmp #0xd / beq   <fallthrough default>
 *      three cases:  cmp #0xd / beq   cmp #0xd / bgt   cmp #0xc / bne
 *
 * The ROM has the three-case form -- the repeated `cmp r5, #0xd` feeding first
 * a `beq` and then a `bgt` is the tree's signature, and it is a RELIABLE READ
 * on the source: this switch has at least one more case label than the two we
 * can see distinct bodies for. Adding a `case 0xe:` that carries the default
 * value reproduces the tree exactly, so the shape is available; what is not
 * available is a third case whose value we can justify. Anyone returning here
 * should look for the real third case (a sibling overlay in the 0x2985 message
 * family may name it) rather than re-testing flags.
 *
 * The rest of the body screens clean once the head is set aside: the
 * gState+0x1f4 load, the 0xc1<<2 flag test, the neg/orr/lsr#31 boolean and the
 * `2 - flag` all reproduce.
 */
typedef struct {
    unsigned char pad00[0x1f4];
    int f1f4;
} GlobalState;

extern GlobalState gState;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __Func_809280c(int a, int b, int c);

void OvlFunc_971_200906c(int slot)
{
    int base;
    int n;

    n = 0;
    __CutsceneStart();
    switch (slot) {
    case 0xd:
        base = 0x297f;
        break;
    case 0xc:
        base = 0x2985;
        break;
    default:
        base = 0x2982;
        break;
    }
    __Func_809280c(slot, gState.f1f4, 0);
    if (__GetFlag(0xc1 << 2)) {
        n = 2 - (__GetFlag(0x305) != 0);
    }
    __MessageID(base + n);
    __Func_8092c40(slot, 0);
    __CutsceneEnd();
}

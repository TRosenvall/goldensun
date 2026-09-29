/* OvlFunc_968_200cbd8 -- NON-MATCHING, 176 of 207 encodings differ.
 * Unattempted before batch 298.  Reference asm/overlays/rom_7f2f14/ovl_30_c_c_c_c_c.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7f2f14/200cbd8.c \
 *       asm/overlays/rom_7f2f14/ovl_30_c_c_c_c_c.s --func OvlFunc_968_200cbd8
 *
 * NOT a distance: size 460 against 452 and count 213 against 207.  aligncmp 31.9%,
 * which READS WORSE THAN IT IS: control flow and instruction sequence match nearly
 * everywhere, and one global register decision (r5 against r6 for the argument)
 * poisons every line after it.
 * SHIMS: 0 pins, 1 `.equ` measurement-only.  Its split needs EIGHT new exports --
 * .L51a4 .L51a8 .L51ac .L51b0 .L772c .L777c .L778c .L77ec -- the largest export set
 * in this brief.  Measured and closed: -fno-strict-aliasing is worse (223 lines).
 */
/* OvlFunc_968_200cbd8  --  0x0200cbd8   [PARK DRAFT -- 6 encodings long]
 *   [asm/overlays/rom_7f2f14/ovl_30_c_c_c_c_c.s, 1st of 1]
 *
 * REFERENCE: 191 instructions / 207 encodings / 452 bytes.  Anchored
 * thumb_func_start count = 1, and the .s also carries a large .data section, so
 * landing needs a TEXT/DATA SPLIT.  tools/datacheck.py:
 *   reads .L5128 .L5164 .L51a4 .L51a8 .L51ac .L51b0 .L772c .L777c .L778c .L77ec
 *   *** SPLIT MUST EXPORT: .global .L51a4 .global .L51a8 .global .L51ac
 *       .global .L51b0 .global .L772c .global .L777c .global .L778c
 *       .global .L77ec
 * (.L5128, .L5164 and gScript_968__0200d564 are already global; the other eight
 * are NOT and the split has to add them -- eight new exports, the largest
 * export set in this batch.)
 *
 * WHAT IT DOES -- this is an NPC SCRIPT SELECTOR, and the .s header's "CALL
 * TRACE rather than a description" can now be replaced.  Given an actor, it
 *   1. builds a 3-word probe point {a->x, a->y - 0x100000, a->z} on the stack
 *      and asks OvlFunc_968_200832c(v, 0) which actor stands there;
 *   2. requires that actor's ->p50->f28[0] == 0x100, else the default script;
 *   3. picks one of four byte lists (.L51a4/.L51a8/.L51ac/.L51b0) by QUADRANT:
 *      the dominant axis of {a->f24, a->f2c} by absolute value, then that
 *      axis's sign;
 *   4. scans the list for the found actor's ->p50->f24 (a kind/id byte); no
 *      match -> default script;
 *   5. looks the actor's coarse position (x >> 20, z >> 20) up in a 2-int table
 *      -- .L5128 with 4 rows when gState area == 0xb9, .L5164 with 8 rows
 *      otherwise -- yielding a row index i;
 *   6. ADVANCES A PERSISTENT CURSOR: .L772c (resp. .L778c) is an array of
 *      `unsigned char *`, and the loop STORES p+1 back into .L772c[i] on every
 *      step, counting steps in n.  So the list is consumed across calls -- this
 *      is a sequential-dialogue cursor, not a pure search.  The store-back is
 *      not an inference: `add r3, r1, #1 / str r3, [r0, r4]` writes to
 *      .L772c + i*4, and the loop RELOADS `ldr r0, =.L772c / ldr r1, [r0, r4]`
 *      every iteration, which is exactly the reload a mutated global forces.
 *   7. runs script .L777c[i][n] (resp. .L77ec[i][n]).
 *
 * STATE: objcmp  XX SIZE ref 452 bytes, ours 460
 *                XX ENCODINGS differ in 176 place(s) (ref 207, ours 213)
 *                first at index 1: ref 1c05 ours 1c06
 * SIZE AND INSTRUCTION COUNT BOTH DIFFER, so 176 is NOT a distance.
 * tools/aligncmp.py: aligned-equal 66 of 207 (31.9%), 199 differing/ins/del in
 * 22 hunks.  tools/tryc.py --full: "rom 216 lines, ours 221, first diff at 1".
 * SHIMS: 0 pins, 0 barriers, 1 `.equ` MEASUREMENT shim (_AREA_b9, which
 * ALREADY EXISTS at area.sym:173 -- strip the .equ before landing).
 *
 * READ THE 31.9% CORRECTLY.  It is low for a reconstruction whose CONTROL FLOW
 * and INSTRUCTION SEQUENCE match almost everywhere: the whole quadrant block
 * (abs, the two sign tests, the four table loads) is line-for-line identical,
 * and so are both position-table search loops.  The 31.9% is one global
 * register decision poisoning nearly every line -- see the blocker.
 *
 * THE BLOCKER, NAMED BY PASS: global register allocation, at the level of WHICH
 * pseudo gets which callee-saved register.  The ROM holds
 *      r5 = the actor argument,  r6 = its ->p50 then the step count n,
 *      r7 = the actor found by the probe,  r4 = the byte-list cursor
 * and gcc gives r6 to the argument, r12 (ip) to ->p50, r5/r7 to the table
 * pointer and i*4, and r2 to the cursor.  Every `ldr rX, [r5, #..]` in the ROM
 * therefore reads `[r6, #..]` here.  Two consequences make up the whole of the
 * six-encoding excess:
 *   (1) ip IS PRESSED INTO SERVICE for ->p50 (`mov r12, r0`), which the ROM
 *       never needs, and
 *   (2) gcc keeps i*4 in TWO registers (`lsl r7, r0, #2` then `mov r0, r7`)
 *       where the ROM computes it once in place (`lsl r4, #2`, destroying i,
 *       which it never needs again).
 * Downstream of the same pressure, gcc CROSS-JUMPS the two __Actor_SetScript
 * calls into one `bl` with a `b` into it; the ROM keeps three separate call
 * sites, and two of them fill their arguments in OPPOSITE order (.L4c74 does
 * `mov r0,r5 / ldr r1,=script`, .L4d4c does `ldr r1,=script / mov r0,r5`),
 * which is why the ROM's jump pass did not merge them either.
 *
 * WHAT WAS TRIED AND MEASURED (13 builds).  None of it moved 213/221:
 *   * THREE CONTROL-FLOW SHAPES for the four default-script exits.  The nested
 *     form shipped here is the BEST by a clear margin; a guard-clause form
 *     (early `return 0` at each failure) is WORST at 234 lines / 221 encodings,
 *     and an if/else form that puts the default call textually in the middle --
 *     which is where the ROM's .L4c74 physically sits -- is 228/216.  Placing
 *     the default block where the ROM has it makes the result WORSE, so the
 *     ROM's block order is NOT evidence about source order here.
 *   * DROPPING THE `sub` LOCAL so both halves spell `b->p50->f24` identically:
 *     BYTE-FOR-BYTE THE SAME OUTPUT (213 encodings, 221 lines, 175 differ).
 *     gcc CSEs `b->p50` across the scan loop either way, and the store to
 *     .L772c[i] does not defeat it -- `unsigned char *` and `struct Sub *` are
 *     in different alias sets.  The ROM DOES reload `[r7, #0x50]` inside the
 *     cursor loop, so the ROM's source had something in that alias set that
 *     this spelling does not reproduce.  THAT IS THE NEXT IDEA: try declaring
 *     .L772c/.L778c as `void *[]` (gcc-2.96 has put all pointer types in one
 *     alias set in some configurations).  THAT LEAD IS NOW CLOSED, MEASURED:
 *     a -fno-strict-aliasing probe build (DIAGNOSTIC ONLY -- not the production
 *     flags, and this stem has no Makefile rule, so -O2 with strict aliasing is
 *     what it must be built with) reads 223 lines, WORSE than the 221 of the
 *     production build.  Aliasing is NOT what makes the ROM reload
 *     `[r7, #0x50]` in the cursor loop, so do not spend a Makefile row on it.
 *   * THE CURSOR LOOP WRITTEN THREE WAYS -- `while ((c = *tbl[i]) != 0) {...}`,
 *     and `for (;;) { c = *tbl[i]; if (c == 0 || c == x) break; ... }` -- gives
 *     IDENTICAL output.  gcc normalises both to the same rotated, first-
 *     iteration-peeled shape, while the ROM's is the UNROTATED form
 *     (`b` to a top test, increment block above it).  Loop rotation here is
 *     not reachable from source spelling; it is the same class as the
 *     recorded rotation blockers.
 *
 * LEVERS THAT DID WORK, carried over from OvlFunc_967_200904c in this batch:
 *   * 0xb9 IS POOLED (`ldr r3, =0xb9`) where `mov r3, #0xb9` would do, so it is
 *     a SYMBOL: _AREA_b9, already in area.sym.  Same tell as _AREA_b3/_AREA_b4.
 *   * `mov r2, #0` before every `ldrsh` is FORCED -- Thumb-1 `ldrsh` has no
 *     immediate-offset form -- and must not be written out in the source.
 *   * The struct layout and the OvlFunc_968_200832c signature were taken whole
 *     from the same overlay's src/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_a_a_b.c,
 *     which is why the probe-point block matched first time.
 *   * `if (x < 0) x += 0xffff;` followed by a bare sign test, with NO shift, is
 *     gcc-2.96's signed `x / 0x10000` with the shift elided because only the
 *     sign is wanted.  Writing `x / 0x10000 < 0` reproduces it exactly; a plain
 *     `x >> 16 < 0` would drop the 0xffff bias.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
/* MEASUREMENT SHIM -- _AREA_b9 already exists in area.sym; STRIP BEFORE LANDING. */
__asm__(".equ _AREA_b9, 0xb9");
extern int _AREA_b9;

struct Sub {
    unsigned char pad00[0x24];
    unsigned char f24;
    unsigned char pad25[0x28 - 0x25];
    short *f28;
};
struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
    unsigned char pad14[0x24 - 0x14];
    int f24;
    unsigned char pad28[0x2c - 0x28];
    int f2c;
    unsigned char pad30[0x50 - 0x30];
    struct Sub *p50;
};

extern int L5128[][2] __asm__(".L5128");
extern int L5164[][2] __asm__(".L5164");
extern unsigned char L51a4[] __asm__(".L51a4");
extern unsigned char L51a8[] __asm__(".L51a8");
extern unsigned char L51ac[] __asm__(".L51ac");
extern unsigned char L51b0[] __asm__(".L51b0");
extern unsigned char *L772c[] __asm__(".L772c");
extern unsigned char *L778c[] __asm__(".L778c");
extern unsigned char **L777c[] __asm__(".L777c");
extern unsigned char **L77ec[] __asm__(".L77ec");
extern unsigned char gScript_968__0200d564[];

extern struct Actor *OvlFunc_968_200832c(int *v, struct Actor *a);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);

int OvlFunc_968_200cbd8(struct Actor *a)
{
    int v[3];
    struct Actor *b;
    struct Sub *sub;
    unsigned char *t;
    int c;
    int dx;
    int dz;
    int adx;
    int adz;
    unsigned int i;
    int n;
    unsigned char ***sel;
    unsigned int base;
    unsigned int off;

    v[0] = a->x;
    v[1] = a->y + 0xfff00000;
    v[2] = a->z;
    b = OvlFunc_968_200832c(v, 0);
    sub = b->p50;
    if (*sub->f28 == (0x80 << 1)) {
        dx = a->f24;
        adx = dx;
        if (dx < 0)
            adx = -dx;
        dz = a->f2c;
        adz = dz;
        if (dz < 0)
            adz = -dz;
        if (adx > adz)
            t = (dx / 0x10000 < 0) ? L51a4 : L51a8;
        else
            t = (dz / 0x10000 < 0) ? L51ac : L51b0;
        for (; (c = *t) != 0; t++)
            if (sub->f24 == c)
                break;
        if (c != 0) {
            base = (unsigned int)&gState;
            off = 0xe0;
            off <<= 1;
            if (*(short *)((char *)base + off) == (int)&_AREA_b9) {
                for (i = 0; i <= 3; i++)
                    if (L5128[i][0] == (a->x >> 20) && L5128[i][1] == (a->z >> 20))
                        break;
                n = 0;
                while ((c = *L772c[i]) != 0) {
                    if (c == b->p50->f24)
                        break;
                    L772c[i]++;
                    n++;
                }
                sel = L777c;
            } else {
                for (i = 0; i <= 7; i++)
                    if (L5164[i][0] == (a->x >> 20) && L5164[i][1] == (a->z >> 20))
                        break;
                n = 0;
                while ((c = *L778c[i]) != 0) {
                    if (c == b->p50->f24)
                        break;
                    L778c[i]++;
                    n++;
                }
                sel = L77ec;
            }
            if (c != 0) {
                __Actor_SetScript(a, sel[i][n]);
                return 0;
            }
        }
    }
    __Actor_SetScript(a, gScript_968__0200d564);
    return 0;
}

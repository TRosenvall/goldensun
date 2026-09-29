/* PARKED -- OvlFunc_934_2009984, --align 2 of 268 / objcmp 4 of 261
 * NON-MATCHING, 4 encodings of 261.  THIS IS A TRUE DISTANCE: ref 261 encodings
 * against ours 261, and tryc --align reads rom 268 lines / ours 268.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this file> \
 *     asm/overlays/rom_7bdeb0/ovl_169c_a_c_c_b.s --func OvlFunc_934_2009984
 *   [that .s is the file's ONLY function -- anchored thumb_func_start count = 1.
 *   238 instructions.  tools/datacheck.py reports NO data section, so NO SPLIT
 *   and no data export; the 10-word jump table at .L19b8 is gcc's own switch
 *   table, emitted from the `switch` below, not ROM data.]
 *
 * ========================= WHAT IS SETTLED =========================
 *
 * 1. THE AREA TEST IS TWO POOLED SYMBOLS, BOTH ALREADY IN area.sym.  The ROM
 *    does `ldr r3, =0x5e / cmp r1, r3` where `cmp r1, #0x5e` would do -- the
 *    area.sym criterion verbatim.  `_AREA_5e` and `_AREA_5f` are area.sym:86-87.
 *
 * 2. THE SUB-STATE SELECTOR IS A `switch`, cases 1..10 in three arms.  The ROM
 *    does `sub r3,#1 / cmp r3,#9 / bls / ldr r2,=table / lsl r3,#2 /
 *    ldr r3,[r3,r2] / mov pc,r3`, which is exactly gcc's dense-table form for
 *    a switch whose lowest case is 1.  Cases 1-4, 5-7 and 8-10 fall together.
 *
 * 3. `__SetFlag(0x201)` MUST NOT BE A FRESH POOL LOAD.  The ROM spells it
 *    `add r0, #0x3f` on the register that still holds the 0x1c2 byte offset of
 *    the sub-state field -- gcc reuses a constant already in a register and
 *    reaches the new one by a delta.  Getting it needs a FRESH base pointer
 *    local inside that switch arm (`g2 = (unsigned char *)&gState;`) so that
 *    arm re-materialises the base in a call-clobbered register, exactly as the
 *    ROM does with its second `ldr r3, =gState`.  Hoisting one function-wide
 *    `g` into r5 instead costs the delta AND the whole 0x5e-branch register
 *    map: 55 of 268 against 24.
 *
 * 4. ONE VARIABLE CARRIES FOUR ROLES IN r5.  The ROM's r5 holds, in turn, the
 *    literal 0 passed as the 6th argument of two OvlFunc_934_2008528 pairs, the
 *    `__GetFlag(0x205)` result, and 0xe.  Writing them as ONE local `t` (and
 *    the constant 4 as `four`, which lands in r6) is 22 of 268 -> 8.  Separate
 *    locals per role, in either declaration order, do not reach it.
 *
 * 5. THE ARGUMENT FILL IS r1,r2,r3 THEN r0.  Every OvlFunc_934_2008528 site
 *    fills r1/r2/r3 first and r0 LAST.  Pinning q1/q2 (and q3 where the 4th
 *    argument is not the reused 2) reproduces it; unpinned, gcc emits `mov r0`
 *    before the r1 fill.  8 of 268 -> 4.  The OvlFunc_common0_70 site fills
 *    r2, r1, r3, r0 -- a 4-wide pin in that order, and that order only.
 *
 * 6. `cmp r3, #1 / blt` IS NOT `sub < 1`.  Plain `if (sub < 1) return;`,
 *    `<= 0`, and `!(sub >= 1)` all canonicalise to `cmp #0 / ble`.  The ROM
 *    keeps the 1, which needs the constant behind a do{}while(0) barrier
 *    (`int one = 1; do { one = (int) one; } while (0);`).  4 -> 2.
 *
 * ========================== WHAT IS OPEN ===========================
 *
 * RESIDUE: TWO ENCODINGS, ONE INSTRUCTION, AND IT IS local-alloc / postreload
 * SCHEDULING OF A SINGLE `mov r0, #0`.  At the third OvlFunc_934_2008528 site
 * (the 0x204 arm) the ROM emits
 *     mov r3,#0x2 / str r3,[sp] / mov r1,#0xd / mov r2,#0xf / mov r3,#0x4 /
 *     mov r0,#0x0 / str r5,[sp,#4]
 * and we emit the `mov r0,#0x0` one slot earlier, before `mov r3,#0x4`.
 * Measured and NOT it: pinning r0 explicitly; a do{}while(0) barrier on the 0;
 * a named local for the 0; pinning r3 to the literal 2 and re-reading it into
 * an int; dropping the q3 pin at that site; a 4-wide ascending pin.  All six
 * read 2 of 268.  The next thing to try is the declaration-order sweep of
 * {t, four} AFTER this site's pin width is fixed, not before -- the same
 * coupling src/non_matching/ovl_7bc690/2008e2c.c records for its loop.
 *
 * SHIMS: 15 register pins (shimcount), no fakematch.txt row yet.  That count is
 * high and pin-width minimisation at the three 2008528 sites was only carried
 * to the point that stopped improving the distance -- it has NOT been pushed to
 * a fixpoint from above.
  *
 * NON-MATCHING, 4 of 261 encodings differ (a TRUE DISTANCE: size and count
 * both match; --align reads 2 instructions in disagreeing regions).
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7bdeb0/2009984.c \
 *       asm/overlays/rom_7bdeb0/ovl_169c_a_c_c_b.s --func OvlFunc_934_2009984
*/
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;   /* GlobalState @ 0x02000240 */
extern int _AREA_5e;
extern int _AREA_5f;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8092b08(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_common0_70(int a, int b, int c, int d);
extern void OvlFunc_934_2008528(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_934_2008ba4(int a);
extern void OvlFunc_934_2009770(void);
extern void OvlFunc_934_2008cf8(void);

void OvlFunc_934_2009984(void)
{
    unsigned char *g;
    int area;
    int sub;
    int t;
    int four;

    g = (unsigned char *)&gState;
    area = *(short *)(g + (0xe0 << 1));
    if (area == (int)&_AREA_5e) {
        switch (*(short *)(g + (0xe1 << 1))) {
        case 1:
        case 2:
        case 3:
        case 4:
            __Func_8092b08(0xf, 3);
            __Func_8092b08(0xd, 3);
            OvlFunc_common0_70(0xf0 << 15, 0, 0xe8 << 16, 0xdf);
            break;
        case 5:
        case 6:
        case 7:
            if (__GetFlag(0x70) != 0)
                break;
            if (__GetFlag(0x302) == 0)
                break;
            __SetFlag(0x80 << 2);
            { unsigned char *g2 = (unsigned char *)&gState;
              if (*(short *)(g2 + (0xe1 << 1)) == 5)
                  __SetFlag(0x201); }
            __WaitFrames(1);
            if (__GetFlag(0x109) != 0)
                break;
            { register int q0 __asm__("r0"); register int q1 __asm__("r1"); register int q2 __asm__("r2");
              q0 = 8; q1 = 0xc6 << 18; q2 = 0x8c << 17; __MapActor_SetPos(q0, q1, q2); }
            *(int *)(__MapActor_GetActor(8) + 0x6c) = (int)OvlFunc_934_2008cf8;
            break;
        case 8:
        case 9:
        case 10:
            { register int q0 __asm__("r0"); register int q1 __asm__("r1"); register int q2 __asm__("r2"); register int q3 __asm__("r3");
              q2 = 0x8a << 18; q1 = 0; q3 = 0x14; q0 = 0x2820000; OvlFunc_common0_70(q0, q1, q2, q3); }
            { int e5 = 0; int e6 = 0x22; __Func_8010704(0x17, 0x22, 0xd, 3, e5, e6); }
            OvlFunc_934_2009770();
            if (__GetFlag(0x80 << 2) != 0)
                { int e5 = 0x17; int e6 = 0x27; __Func_8010704(0x17, 0x29, 1, 1, e5, e6); }
            if (__GetFlag(0x201) == 0)
                break;
            { int e5 = 0x1b; int e6 = 0x29; __Func_8010704(0x1f, 0x27, 2, 1, e5, e6); }
            break;
        }
    } else if (area == (int)&_AREA_5f) {
        sub = *(short *)(g + (0xe1 << 1));
        if (sub > 3)
            return;
        { int one = 1;
          do { one = (int) one; } while (0);
          if (sub < one)
              return; }
        if (__GetFlag(0x202) != 0) {
            four = 4;
            t = 0;
            { register int q1 __asm__("r1"); register int q2 __asm__("r2"); register int q3 __asm__("r3");
              q1 = 0xc; q2 = 0x10; q3 = 1; OvlFunc_934_2008528(0, q1, q2, q3, four, t); }
            OvlFunc_934_2008528(0, 0xd, 0x10, 1, four, t);
        } else {
            OvlFunc_934_2008ba4(9);
        }
        if (__GetFlag(0x203) != 0) {
            four = 4;
            t = 0;
            { register int q1 __asm__("r1"); register int q2 __asm__("r2"); register int q3 __asm__("r3");
              q1 = 0x10; q2 = 0x10; q3 = 1; OvlFunc_934_2008528(2, q1, q2, q3, four, t); }
            OvlFunc_934_2008528(0, 0x10, 0x10, 1, four, t);
        } else {
            OvlFunc_934_2008ba4(0xa);
        }
        t = __GetFlag(0x205);
        if (t != 0) {
            OvlFunc_934_2008528(0, 0xd, 0x13, 4, 2, 0);
        } else if (__GetFlag(0x81 << 2) != 0) {
            { register int q1 __asm__("r1"); register int q2 __asm__("r2");
              q1 = 0xd; q2 = 0xf; OvlFunc_934_2008528(0, q1, q2, 4, 2, t); }
            t = 0xe;
            __Func_8010704(0xe, 0x11, 2, 1, t, 0x10);
            __Func_8010704(0xe, 0xd, 1, 1, t, 0xf);
        } else {
            OvlFunc_934_2008ba4(0xb);
            __Func_8092b08(0xb, 3);
        }
    }
}

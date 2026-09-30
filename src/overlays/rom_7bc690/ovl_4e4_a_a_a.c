/* OvlFunc_933_20084e4 (0x020084e4) and OvlFunc_933_200888c (0x0200888c) --
 * the WHOLE of goldensun/asm/overlays/rom_7bc690/ovl_4e4_a_a_a.s, which holds
 * exactly these two functions and nothing else.
 *
 * INSTRUCTION STREAMS EXACT.  objcmp --whole reports
 *     OvlFunc_933_20084e4   4 of 416 differ (ours 416), first at index 412
 *     OvlFunc_933_200888c   4 of 415 differ (ours 415), first at index 410
 * and in BOTH cases the four differing "encodings" are the LAST FOUR POOL WORDS:
 * the ROM object carries bare 0x59/0x5a/0x5b/0x5c where ours carries 0x00000000
 * plus an R_ARM_ABS32 to _AREA_59.._AREA_5c (area.sym:175-178, values 0x59..0x5c).
 * That is the recorded "a relocation FORM is not a residue" -- the linker fills
 * those words -- so this needs `make compare`, not more spellings.  `tryc --align`
 * prints OK for both functions and the whole-TU size and relocation SEQUENCE match.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this file> \
 *     asm/overlays/rom_7bc690/ovl_4e4_a_a_a.s --whole
 *   docker run ... python3 tools/tryc.py <this file> --align \
 *     --ref asm/overlays/rom_7bc690/ovl_4e4_a_a_a.s
 * (Do NOT use --func on this file: objcmp filters only the REFERENCE, so a
 * two-function candidate doubles the figure -- 419 of 416 here.  --whole's
 * per-function breakdown is the number.)
 *
 * SPLIT SHAPE: NONE NEEDED.  datacheck.py prints nothing for this .s (no data
 * section), the file's two functions are both converted here, and the stem is
 * named by exactly ONE overlay.ld row, overlays/rom_7bc690/overlay.ld:32
 * `asm/overlays/rom_7bc690/ovl_4e4_a_a_a.o(.text)` -- one .ld file names the stem.
 *
 * SHIMS: ONE register pin, `register int c8 __asm__("r8")` in OvlFunc_933_200888c's
 * _AREA_5b arm (shimcount: "register pins : 1"), and it needs a fakematch.txt row
 * before this lands.  It is NOT inert: removing it is 139 of 415 and TWO
 * INSTRUCTIONS SHORT, because the ROM's `8` lives in r8 and Thumb cannot
 * `mov r8,#8` or `str r8,[sp]`, so it pays `mov r1,#8 / mov r8,r1` at the def and
 * `mov r2,r8` at the far use.  SEVEN pin-free spellings were measured and ALL are
 * inert at 139/413: plain `int c8 = 8`; `register int c8 = 8`; a nested block
 * spanning only c1..c5; bare literals at both sites; reusing c8 as the
 * `__MapActor_SetPos(8, ...)` slot argument (an extra ref plus a hard-register copy
 * suggestion); `int c8; c8 = 8; ... c8 = 8;` (the "assign twice to reach
 * global-alloc" lever -- cse deletes the second set, so the quantity stays local);
 * and c8 declared at the top of the function, which is WORSE (159) because it is
 * rematerialised instead.  `.17.lreg` says the quantity is `Register 110 used 3
 * times across 72 insns in block 5`, priority 3/72 = 0.042 against 0x20's 3/20 =
 * 0.15 and 6's 8/138 = 0.058, so local-alloc hands out r5 then r6 and reaches 110
 * with r5 still free -- 110 would have to be a GLOBAL allocno for r8, and nothing
 * measured makes it one.  Only `;; 5 regs to allocate: 35 33 32 122 121` are global.
 *
 * ================== THE FIVE LEVERS, in the order they paid =================
 *
 * 1. THE gState BASE IS A LOCAL POINTER, RE-ASSIGNED AT EACH READ.  The park
 *    src/non_matching/ovl_7bc690/2009638.c already records that
 *    `typedef struct { unsigned char _bytes[704]; } GlobalState; extern
 *    GlobalState gState;` with `gs = (unsigned char *)&gState;` gives the ROM's
 *    `ldr r3, =gState` plus a reg+reg `add`.  What it does not say is that the
 *    cast must go through a LOCAL: written inline as
 *    `*(short *)((unsigned char *)&gState + (0xe0 << 1))` gcc folds the whole
 *    address into one `ldr r3, =gState+448` pool word and each read is THREE
 *    INSTRUCTIONS SHORT.  Two reads, so 6 short -- 408 differing of 416 became 77,
 *    with size and count exact.  `gs` is assigned twice, once before each read,
 *    which is what keeps it out of a callee-saved register: the ROM reloads
 *    `ldr r3, =gState` after the calls rather than holding it.
 *
 * 2. TWO PLAIN-LITERAL STACK ARGUMENTS MUST BE TWO NAMED LOCALS, DECLARED
 *    dx-THEN-dy.  This is the single biggest lever in both functions and it is
 *    new.  `__Func_80105d4(0x46, 0x44, 4, 2, 0x16, 7)` gives
 *        mov r3, #0x16 / str r3, [sp] / mov r3, #7 / str r3, [sp, #4]
 *    -- one pseudo, reused, because the first dies at its store.  The ROM issues
 *        mov r3, #0x16 / mov r2, #7 / str r3, [sp] / str r2, [sp, #4]
 *    -- two pseudos live at once.  `{ int x = 0x16, y = 7; f(0x46,0x44,4,2,x,y); }`
 *    reproduces it EXACTLY, including the registers (r3 then r2: the declaration
 *    order is the pseudo-creation order and REG_ALLOC_ORDER hands out r3 first).
 *    Reversing the declarations gives `mov r2,#7 / mov r3,#0x16` and is wrong.
 *    12 such sites in OvlFunc_933_20084e4: 77 differing -> 42.
 *    The rule is NARROW.  Apply it only where NEITHER value is reused: where one
 *    of the two is held in a callee-saved register for a later call, the ROM's
 *    order already falls out of bare literals, and naming it costs instructions.
 *    The `FULL`/`PAIR` macros below mark exactly the sites that need it.
 *
 * 3. A VALUE FIRST USED AS dy OF CALL N AND AGAIN AT CALL N+1 IS A NAMED LOCAL
 *    DECLARED BEFORE CALL N -- and the reason is PSEUDO BIRTH ORDER, not spelling.
 *    OvlFunc_933_20084e4's _AREA_5b arm ends `mov r5,#0x20` BEFORE `str r6,[sp]`,
 *    i.e. 0x20 is born while the `8` quantity is still live; they conflict, `8`
 *    loses r5 and takes r6, and 0xa -- whose range does not reach 0x20 -- keeps r5.
 *    With 0x20 left as a literal its `mov` lands AFTER `8`'s last store, the
 *    conflict disappears and the whole arm comes out with r5 and r6 exchanged.
 *    `{ int c20 = 0x20; ...two calls... }` is 28 differing -> 17.  The same lever
 *    fixes OvlFunc_933_200888c's 0xc and 0x17 (303 -> 129 in one step) and, with
 *    the declaration order reversed, its 9/5 pair (see 5).
 *    Declaring 0x20 at the top of the ARM instead is 422 encodings and wrong: the
 *    position is the lever, not the name.
 *
 * 4. TWO OR MORE SHIFTABLE CONSTANTS IN ONE CALL NEED DOMINATING-BLOCK LOCALS,
 *    OR `precompute_register_parameters` REORDERS THE ARGUMENT SET-UP.  A Thumb
 *    2-insn constant costs more than one insn, so calls.c hoists it into a pseudo
 *    BEFORE the stack stores and before the cheap arguments; the pseudo then holds
 *    its hard argument register across everything that follows.  Two consequences,
 *    both fixed the same way.
 *      * OvlFunc_933_20084e4's seven-argument `__Func_8094730(0, 0x80<<11, 0x80<<9,
 *        0x80<<6, 0x80<<9, 0x80<<8, 0x80<<7)`: with everything inline the two
 *        stack values 0x8000/0x4000 cannot have r3/r1 (held by the hoisted args 2
 *        and 4) and land in r0 and r4.  Declaring ARGUMENTS 2 AND 4 in a block that
 *        DOMINATES the `if` -- not inside it -- lets local-alloc delete their sets
 *        and rematerialise the constants at the argument slots, which is the ROM's
 *        late `mov r1,#0x80 / lsl r1,#11`; r3 and r1 are then free for the two
 *        stack values, which are themselves named locals per lever 2.  A 116-cell
 *        sweep over {which arguments are named} x {dominating block, top of
 *        function, inside the if} x {arg3 shared or written twice} plateaus at
 *        16-18 for every OTHER cell and drops to 4 for exactly this one.
 *      * OvlFunc_933_200888c's EIGHT `__MapActor_SetPos(slot, x, z)` calls: the ROM
 *        writes `mov r1,#0xb0 / mov r2,#0xcc / mov r0,#9 / lsl r1,#17 / lsl r2,#16`,
 *        with the CHEAP argument in the middle; gcc puts `mov r0,#9` last, because
 *        the hoisted x/z chains have lower LUIDs and sched2 breaks the
 *        priority-1 tie by LUID.  Six spellings that keep the coordinates in the
 *        SAME block as the call are all identical to the bare literals; both
 *        coordinates as DOMINATING-block locals reproduce the ROM exactly.  It is
 *        worth more than the two instructions it looks like: with `mov r0` in the
 *        middle, the _AREA_59 arm's lone SetPos and the _AREA_5a arm's last one
 *        share the three-insn tail `lsl r2,#17 / bl / b`, and gcc CROSS-JUMPS them.
 *        With `mov r0` last the common tail is two insns, saves nothing, and the
 *        cross-jump does not happen -- which is where OvlFunc_933_200888c's
 *        surplus two instructions were.  Nothing had to be written to get the
 *        cross-jump; it followed.
 *    52 .s files in the tree carry the ROM's `mov r1 / mov r2 / mov r0 / lsl r1`
 *    order and NOT ONE of them has a .c sibling, so before this the shape had never
 *    been reached.  It is reachable, and this is how.
 *
 * 5. AN ADJACENT TRANSPOSITION OF TWO EQUAL-PRIORITY GLOBAL ALLOCNOS IS FIXED BY
 *    MAKING THEIR LIVE RANGES EXACTLY EQUAL.  OvlFunc_933_200888c's 9 and 5 are
 *    global allocnos (`;; 5 regs to allocate: ... 122 121`) with four references
 *    each.  Expanded from bare literals the stores interleave as
 *    mov/str/mov/str, so 9 is born two insns before 5 and dies one before it: 76
 *    insns against 74, 5 wins on `allocno_compare` and takes r5.  Declared TOGETHER
 *    in a nested block immediately before the first of their four calls the two
 *    `mov`s are adjacent, births and deaths are one apart at both ends, the lengths
 *    are EQUAL, and the only remaining tie-break -- allocno number, i.e. which was
 *    declared first -- gives 9 the r5 the ROM gives it.  `int c9 = 9, c5 = 5;` is
 *    exact; `int c5 = 5, c9 = 9;` is 6 differing; either of them at the top of the
 *    arm instead is 417 encodings.
 *
 * TWO SMALLER READINGS.  Both functions have `pop {r1} / bx r1` rather than
 * `pop {r0} / bx r0`, so both are declared `int` and neither returns a value --
 * gcc keeps r0 reserved for the return value and takes r1 as the epilogue scratch.
 * That alone is 42 differing -> 40 on OvlFunc_933_20084e4.  And
 * `__Func_808edac(i, -1, -1)` in OvlFunc_933_200888c's tail loop is hoisted by LICM
 * into one `mov r6,#1 / neg r6,r6` before the loop, where the ROM builds -1 twice
 * INSIDE it; naming just ONE of the two (`int mone = -1;` at the top, the other
 * left as the literal) makes gcc rematerialise both in the loop, which is the ROM.
 * Naming both, or neither, hoists.
 *
 * The four pooled area ids need no new .sym work: the ROM does `ldr r3, =0x59 /
 * cmp r2, r3` where `cmp r2, #0x59` would do, which is area.sym's own criterion,
 * and _AREA_59.._AREA_5c are already there at lines 175-178.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;

extern void __ClearFlag(int id);
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void __Func_80105d4(int sx, int sy, int w, int h, int dx, int dy);
extern void __Func_8010704(int sx, int sy, int w, int h, int dx, int dy);
extern void __Func_8010788(int sx, int sy, int w, int h, int dx, int dy);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_808edac(int slot, int x, int z);
extern void __Func_808ee0c(void);
extern void __Func_8094730(int a, int b, int c, int d, int e, int f, int g);
extern void __Func_80947e4(void);
extern int _AREA_59;
extern int _AREA_5a;
extern int _AREA_5b;
extern int _AREA_5c;

#define FULL(dx, dy) { int x = (dx), y = (dy); __Func_80105d4(0x46, 0x44, 4, 2, x, y); }
#define PAIR(fn, sx, sy, w, h, dx, dy) \
    { int x = (dx), y = (dy); fn(sx, sy, w, h, x, y); }

int OvlFunc_933_20084e4(void)
{
    unsigned char *gs;
    int area;
    int i;

    __ClearFlag(0x201);
    gs = (unsigned char *)&gState;
    area = *(short *)(gs + (0xe0 << 1));
    if (area == (int)&_AREA_59) {
        FULL(0x16, 7)
        FULL(8, 0xa)
        __Func_80105d4(0x46, 0x44, 4, 2, 0x17, 0x15);
        __Func_8010704(0x46, 0x44, 4, 1, 0x17, 0x17);
        FULL(0x10, 0x2a)
        FULL(0x24, 0x2c)
        FULL(0xe, 0x37)
    } else if (area == (int)&_AREA_5a) {
        FULL(0x2a, 5)
        __Func_80105d4(0x46, 0x44, 4, 2, 0x14, 0xb);
        __Func_8010704(0x46, 0x44, 4, 1, 0x14, 0xd);
        FULL(0xe, 0xc)
        __Func_80105d4(0x46, 0x44, 4, 2, 0x38, 0x12);
        __Func_80105d4(0x46, 0x44, 4, 2, 7, 0x16);
        __Func_8010704(0x46, 0x44, 4, 1, 7, 0x18);
        __Func_80105d4(0x46, 0x44, 4, 2, 0x2c, 0x17);
        __Func_8010704(0x46, 0x44, 4, 1, 0x2c, 0x19);
        __Func_80105d4(0x46, 0x44, 4, 2, 0x26, 0x18);
        FULL(0x1a, 0x1c)
        FULL(0x11, 0x23)
        FULL(0x32, 0x24)
        __Func_80105d4(0x46, 0x44, 4, 2, 0x22, 0x2b);
        __Func_8010704(0x46, 0x44, 4, 1, 0x22, 0x2d);
        FULL(6, 0x2e)
        FULL(0x1b, 0x37)
        __Func_80105d4(0x46, 0x44, 4, 2, 0x2b, 0x38);
    } else if (area == (int)&_AREA_5b) {
        __Func_8010788(0x45, 0x63, 4, 2, 8, 0x10);
        __Func_8010788(0x45, 0x63, 4, 2, 6, 0x14);
        __Func_8010788(0x45, 0x63, 4, 2, 0xa, 0x17);
        __Func_8010704(0x45, 0x63, 4, 2, 8, 0xe);
        { int y = 0x12; __Func_8010704(0x45, 0x63, 4, 2, 6, y); }
        __Func_8010704(0x45, 0x63, 4, 1, 6, 0x14);
        __Func_8010704(0x45, 0x63, 4, 2, 0xa, 0x15);
        { int c20 = 0x20;
          __Func_80105d4(0, 0x79, 5, 7, 8, c20);
          __Func_80105d4(0, 0x79, 5, 7, 0x2b, c20); }
        __Func_80105d4(6, 0x78, 3, 1, 9, 5);
        __Func_80105d4(9, 0x78, 3, 1, 0x2c, 5);
        __Func_8010704(9, 0, 3, 3, 9, 6);
    }
    __MapActor_SetPos(8, 0, 0);
    __MapActor_SetPos(9, 0, 0);
    __MapActor_SetPos(0xa, 0, 0);
    __MapActor_SetPos(0xb, 0, 0);
    __MapActor_SetPos(0xc, 0, 0);
    __MapActor_SetPos(0xd, 0, 0);
    i = 0x64;
    do {
        __Func_808edac(i, 0, 0);
        i++;
    } while (i <= 0x6b);
    {
    int b = 0x80 << 11;
    int d = 0x80 << 6;
    gs = (unsigned char *)&gState;
    if (*(short *)(gs + (0xe0 << 1)) != (int)&_AREA_5c) {
        int f = 0x80 << 8;
        int g = 0x80 << 7;
        __Func_8094730(0, b, 0x80 << 9, d, 0x80 << 9, f, g);
    }
    }
}

int OvlFunc_933_200888c(void)
{
    unsigned char *gs;
    int area;
    int mone = -1;
    int i;
    int m9x = 0xc8 << 17, m9z = 0xb6 << 17;
    int n9x = 0xb0 << 17, n9z = 0xcc << 16;
    int nax = 0xb8 << 18, naz = 0xc6 << 17;
    int nbx = 0x90 << 16, nbz = 0xbe << 17;
    int ncx = 0x90 << 18, ncz = 0xb3 << 18;
    int ndx = 0xa2 << 18, ndz = 0xcc << 17;
    int o8x = 0xa8 << 16, o8z = 0xb8 << 15;
    int o9x = 0x80 << 16, o9z = 0x9e << 17;

    __ClearFlag(0x80 << 2);
    __SetFlag(0x201);
    gs = (unsigned char *)&gState;
    area = *(short *)(gs + (0xe0 << 1));
    if (area == (int)&_AREA_59) {
        __Func_80105d4(0x40, 0x7e, 4, 2, 0x16, 7);
        PAIR(__Func_80105d4, 0x44, 0x7e, 4, 2, 8, 0xa)
        __Func_80105d4(0x48, 0x7e, 4, 2, 0x17, 0x15);
        __Func_8010704(0x48, 0x7e, 4, 2, 0x17, 0x16);
        PAIR(__Func_80105d4, 0x4c, 0x7e, 4, 2, 0x10, 0x2a)
        PAIR(__Func_80105d4, 0x50, 0x7e, 4, 2, 0x24, 0x2c)
        PAIR(__Func_80105d4, 0x54, 0x7e, 4, 2, 0xe, 0x37)
        __MapActor_SetPos(9, m9x, m9z);
    } else if (area == (int)&_AREA_5a) {
        PAIR(__Func_80105d4, 0x40, 0x7e, 4, 2, 0x2a, 5)
        __Func_80105d4(0x44, 0x7e, 4, 2, 0x14, 0xb);
        { int c = 0xc;
          __Func_8010704(0x44, 0x7e, 4, 2, 0x14, c);
          __Func_80105d4(0x48, 0x7e, 4, 2, 0xe, c); }
        __Func_80105d4(0x4c, 0x7e, 4, 2, 0x38, 0x12);
        __Func_80105d4(0x50, 0x7e, 4, 2, 7, 0x16);
        { int c17 = 0x17;
          __Func_8010704(0x50, 0x7e, 4, 2, 7, c17);
          __Func_80105d4(0x54, 0x7e, 4, 2, 0x2c, c17); }
        __Func_8010704(0x54, 0x7e, 4, 2, 0x2c, 0x18);
        __Func_80105d4(0x58, 0x7e, 4, 2, 0x26, 0x18);
        PAIR(__Func_80105d4, 0x5c, 0x7e, 4, 2, 0x1a, 0x1c)
        PAIR(__Func_80105d4, 0x60, 0x7e, 4, 2, 0x11, 0x23)
        PAIR(__Func_80105d4, 0x64, 0x7e, 4, 2, 0x32, 0x24)
        __Func_80105d4(0x68, 0x7e, 4, 2, 0x22, 0x2b);
        __Func_8010704(0x68, 0x7e, 4, 2, 0x22, 0x2c);
        PAIR(__Func_80105d4, 0x6c, 0x7e, 4, 2, 6, 0x2e)
        PAIR(__Func_80105d4, 0x70, 0x7e, 4, 2, 0x1b, 0x37)
        __Func_80105d4(0x74, 0x7e, 4, 2, 0x2b, 0x38);
        __MapActor_SetPos(9, n9x, n9z);
        __MapActor_SetPos(0xa, nax, naz);
        __MapActor_SetPos(0xb, nbx, nbz);
        __MapActor_SetPos(0xc, ncx, ncz);
        __MapActor_SetPos(0xd, ndx, ndz);
    } else if (area == (int)&_AREA_5b) {
        register int c8 __asm__("r8") = 8;
        int ce = 0xe;
        __Func_80105d4(0x40, 0x7c, 4, 4, c8, ce);
        __Func_80105d4(0x44, 0x7c, 4, 4, 6, 0x12);
        __Func_8010704(0x44, 0x7c, 4, 1, 6, 0x14);
        PAIR(__Func_80105d4, 0x48, 0x7c, 4, 4, 0xa, 0x15)
        __Func_80105d4(0xa, 0x79, 5, 7, c8, 0x20);
        __Func_80105d4(5, 0x79, 5, 7, 0x2b, 0x20);
        { int c9 = 9, c5 = 5;
          __Func_80105d4(0, 0x78, 3, 1, c9, c5);
          __Func_80105d4(3, 0x78, 3, 1, 0x2c, c5);
          __MapActor_SetPos(8, o8x, o8z);
          __MapActor_SetPos(9, o9x, o9z);
          __Func_8010704(6, 0, 3, 3, c9, 6);
          if (__GetFlag(0x90a) == 0)
              __Func_80105d4(0, 0x77, 3, 1, c9, c5); }
    }
    i = 0x64;
    do {
        __Func_808edac(i, mone, -1);
        i++;
    } while (i <= 0x6b);
    __Func_808ee0c();
    gs = (unsigned char *)&gState;
    if (*(short *)(gs + (0xe0 << 1)) != (int)&_AREA_5c)
        __Func_80947e4();
}

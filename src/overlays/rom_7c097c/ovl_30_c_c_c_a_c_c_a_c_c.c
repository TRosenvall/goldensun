/* OvlFunc_936_2009930  --  0x02009930    *** BYTE-EXACT MATCH ***
 *
 * Cut out of asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_a_c_c.s, which holds
 * this function ALONE -- no text/data split is needed and no label needs
 * `.global` (datacheck.py and split_s.py both report nothing to do).
 * shimcount.py: PIN-FREE -- no inline asm, no barrier, no flag override in the
 * source.  Nothing for fakematch.txt.
 *
 * *** THIS FILE NEEDS A CSE_CFLAGS MAKEFILE RULE. ***  At plain -O2 it is
 * 17 of 578 differing (size and count exact); with -fno-rerun-cse-after-loop it
 * is byte-identical -- 1340 bytes, 578 encodings, 86 relocations.  Add, beside
 * the other CSE_CFLAGS rules:
 *
 *   asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_a_c_c.o: src/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_a_c_c.c
 *       $(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
 *       printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
 *       arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
 *
 * Why: the flag id 0x200 is used three times -- __GetFlag in the `t != 0` arm,
 * then __SetFlag and __ClearFlag in the `else` arm with the first dominating the
 * second.  That is exactly the documented two-part constant-CSE precondition
 * (see "Pool-constant CSE: the complete rule"): the mutually exclusive pair
 * reloads on its own, the DOMINATING pair inside the else arm does not.  gcc
 * hoists `mov #0x80 / lsl #2` into r7, pays `push {r7}`, and that one extra
 * allocno also rotates iwram_3001ebc's base and the __GetFlag(0x109) result
 * between r5 and r6.  -fno-gcse is byte-identical to the default (29 either
 * way); only -fno-rerun-cse-after-loop undoes it, as the rule says.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_a_c_c.c \
 *     asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_a_c_c.s --func OvlFunc_936_2009930
 * (objcmp picks the CSE_CFLAGS up from the Makefile rule once it is installed;
 * before then, append -fno-rerun-cse-after-loop by hand.)
 * FINAL INSTALLED PATH: src/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_a_c_c.c
 *
 * THE FOUR LEVERS THAT PAID, in the order they paid:
 *
 * 1. A CONTIGUOUS `switch` CASE RUN, not an `if` range test.  The ROM has
 *    `cmp r3,#2 / bgt out / cmp r3,#1 / blt out` twice.  elevation.md's
 *    "`cmp #K / bge` and `cmp #K / blt` with K>0 are UNREACHABLE" declares that
 *    a hard two-instruction floor per site and tells you to EXCLUDE such
 *    functions from the worklist.  THAT SECTION IS WRONG, and this function is
 *    the proof: writing the range as
 *        switch (*(short *)(g + (0xe1 << 1))) { case 1: case 2: ...; break; }
 *    reproduces `cmp #2 / bgt` + `cmp #1 / blt` exactly.  stmt.c's
 *    `emit_case_nodes` emits a case-node range test DIRECTLY as LT/GT; it never
 *    goes through combine.c's `simplify_comparison`, which is where the
 *    LT C -> LE C-1 rewrite lives.  Only an `if` written as a comparison
 *    reaches that rewrite.  See the corpus control in the batch report: 21 such
 *    sites survive in 15 MATCHED generated .s files, and every one of their .c
 *    files uses a switch.
 * 2. ONE LOCAL PER VALUE, NOT ONE REUSED SCRATCH.  First draft recycled four
 *    ints across the ~60 stack-argument sites; that gave them long live ranges,
 *    put them in callee-saved registers where the ROM uses r2/r3 literals, and
 *    read 552 of 578 with the count 2 over.  A fresh local per site, and only
 *    where the ROM actually holds the value in r5/r6, took it to 36 of 578 with
 *    SIZE AND COUNT EXACT.  Splitting the shared `one`/`two` pair so the 0x915
 *    block gets its own pair (the ROM allocates them r6=2/r5=1 there and
 *    r6=1/r5=2 in the 0x302 block -- opposite, therefore different variables)
 *    took 36 -> 29.
 * 3. THE ARGUMENT-INTERLEAVE LEVER (elevation.md "the zero interleaved into a
 *    shifted build"), 14 -> 8.  The ROM emits
 *        mov r1,#0xe8 / mov r2,#0xb7 / mov r0,#8 / lsl r1,#16 / lsl r2,#18
 *    with the plain argument INSIDE the split builds.  Literals put `mov r0,#8`
 *    last.  Naming the two shifted values ABOVE the enclosing __GetFlag guard
 *    (`c2`/`d2`, `c4`/`d4`) makes gcc rematerialise them at the call in the
 *    ROM's order.  Note the same call with literals MATCHED at the first site
 *    and not at the second and fourth: the first sits in a two-instruction
 *    basic block and the others open a 200-instruction one, so this is sched1
 *    block size, which is why the lever needs the dominating block.
 * 4. A SEPARATE `q` PER ARM, 7 -> 0.  The last residue was a whole-function
 *    r5<->r6 rotation between the iwram_3001ebc base and the __GetFlag(0x109)
 *    result.  Declaration order was inert in three orders; dropping the base's
 *    `p = bp[0]` local was worth 1; what closed it was giving the `else` arm its
 *    OWN `q2`.  The ROM's else-arm pointer lives in r3 -- it is not live across
 *    a call there -- so it is not the same quantity as the then-arm's r5 one.
 *    One variable across both arms makes one allocno that must be callee-saved
 *    in both, and that extra callee-saved conflict is what rotated the pair.
 *    This is the "a variable with DISJOINT live ranges should be two variables"
 *    rule reaching a register ROTATION rather than a spill.
 *
 * Readings worth keeping: `extern unsigned char *iwram_3001ebc[];` (the DECLARED
 * ARRAY form, so the base stays bare in r6 and both globals are reached with
 * immediate offsets -- `(&iwram_3001ebc)[9]` would fold to =sym+36); the store
 * of 0x204 is gcc's own `add r2,#0x44` off the 0x1c0 already in a register;
 * `a[0x23] = m | a[0x23]` with `unsigned char m = 2` for the orr destination;
 * `h = 0xa0; h <<= 7;` because a bare 0x5000 through a `short *` pools.
 */
extern unsigned char *iwram_3001ebc[];
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8091494(int a);
extern void __Func_8091ff0(int a);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int n);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __StartTask(void (*fn)(void), int prio);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_936_200b768(void);

void OvlFunc_936_2009930(void)
{
    unsigned char **bp;
    unsigned char *p;
    unsigned char *q;
    unsigned char *q2;
    unsigned char *a;
    unsigned char *g;
    int t;
    int x;
    int h;
    unsigned char m;
    int nine;
    int two9;
    int one9;
    int one;
    int two;
    int ten;
    int eight;
    int j0;
    int j1;
    int k0;
    int n0;
    int n1;
    int c2;
    int d2;
    int c4;
    int d4;

    bp = iwram_3001ebc;
    *(int *)(bp[0] + (0xe0 << 1)) = 0x204;
    __Func_8091494(0);
    t = __GetFlag(0x109);
    if (t != 0) {
        q = bp[9];
        x = __GetFlag(0x200);
        if (x != 0)
            x = (int)__MapActor_GetActor(0);
        *(int *)(q + 0x18) = x;
    } else {
        __SetFlag(0x200);
        g = gState;
        if (*(short *)(g + (0xe1 << 1)) == 4) {
            q2 = bp[9];
            *(int *)(q2 + 0x18) = t;
            __ClearFlag(0x200);
        }
    }
    if (__GetFlag(0x302) != 0) {
        __MapActor_SetPos(0xb, 0x96 << 16, 0xb6 << 18);
        if (__GetFlag(0x201) != 0) {
            __MapActor_GetActor(0xb);
            __MapActor_SetAnim(0xb, 5);
            nine = 9;
            __Func_8010704(0, 0, 1, 1, nine, 0xe);
            __Func_8010704(0, 0, 1, 1, nine, 0x2d);
            a = __MapActor_GetActor(0xb);
            m = 2;
            a[0x23] = m | a[0x23];
        }
    }
    __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
    __StartTask(OvlFunc_936_200b768, 0xc8 << 4);
    if (__GetFlag(0x915) != 0) {
        __MapActor_SetPos(0xa, 0xd5 << 17, 0x2da0000);
        p = __MapActor_GetActor(0xa);
        h = 0xa0;
        h <<= 7;
        *(short *)(p + 6) = h;
        two9 = 2;
        __CopyMapTiles(0x58, 0x30, 0x58, 0x2d, two9, 3);
        one9 = 1;
        __CopyMapTiles(0x18, 0x31, 0x18, 0x30, two9, one9);
        __CopyMapTiles(0x19, 0x2a, 0x19, 0x2f, one9, one9);
        j0 = 0x18;
        j1 = 0x31;
        __Func_8010704(0x16, 0x32, 2, 1, j0, j1);
    }
    c2 = 0xe8 << 16;
    d2 = 0xb7 << 18;
    if (__GetFlag(0x302) != 0) {
        __MapActor_SetPos(8, c2, d2);
        one = 1;
        __Func_8010704(7, 0x2c, 1, 1, 0, one);
        __CopyMapTiles(0x4a, 0x3a, 0x4e, 0x29, one, 5);
        two = 2;
        __CopyMapTiles(0x10, 0x6d, 0xd, 0x6d, 3, two);
        __CopyMapTiles(0x43, 0x40, 0x47, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x40, 0x48, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x44, 0x49, 0x2b, one, two);
        __CopyMapTiles(0x43, 0x44, 0x4a, 0x2b, one, two);
        __CopyMapTiles(0x43, 0x40, 0x4b, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x42, 0x4c, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x40, 0x4d, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x40, 0x4e, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x40, 0x4f, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x42, 0x50, 0x2c, one, two);
        __CopyMapTiles(2, 0, 9, 0x2a, two, two);
        __CopyMapTiles(0x44, 0x40, 0x47, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x40, 0x48, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x44, 0x49, 0x2b, one, two);
        __CopyMapTiles(0x44, 0x44, 0x4a, 0x2b, one, two);
        __CopyMapTiles(0x44, 0x40, 0x4b, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x42, 0x4c, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x40, 0x4d, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x40, 0x4e, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x40, 0x4f, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x42, 0x50, 0x2c, one, two);
        __CopyMapTiles(4, 0, 9, 0x2a, two, two);
        ten = 0xa;
        __CopyMapTiles(7, 0xb, 7, 0x2a, ten, 8);
        __CopyMapTiles(0x47, 0xc, 0x47, 0x2b, ten, 0xd);
        k0 = 0x2c;
        __Func_8010704(6, 0xd, 0xc, 0xc, 6, k0);
        __Func_8010704(0, 1, 1, 1, 7, k0);
    } else {
        g = gState;
        switch (*(short *)(g + (0xe1 << 1))) {
        case 1:
        case 2:
            __Func_8091ff0(0xaa);
            break;
        }
    }
    c4 = 0xae << 18;
    d4 = 0xb7 << 18;
    if (__GetFlag(0x303) != 0) {
        __MapActor_SetPos(9, c4, d4);
        one = 1;
        __CopyMapTiles(0x4a, 0x3a, 0x6b, 0x29, one, 5);
        two = 2;
        __CopyMapTiles(0x2d, 0x6d, 0x2a, 0x6d, 3, two);
        __CopyMapTiles(0x43, 0x40, 0x66, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x40, 0x67, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x40, 0x68, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x42, 0x69, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x40, 0x6a, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x40, 0x6b, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x40, 0x6c, 0x2c, one, two);
        __CopyMapTiles(0x43, 0x42, 0x6d, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x40, 0x66, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x40, 0x67, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x40, 0x68, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x42, 0x69, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x40, 0x6a, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x40, 0x6b, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x40, 0x6c, 0x2c, one, two);
        __CopyMapTiles(0x44, 0x42, 0x6d, 0x2c, one, two);
        eight = 8;
        __CopyMapTiles(0x26, 0xe, 0x26, 0x2c, eight, 4);
        __CopyMapTiles(0x66, 0xe, 0x66, 0x2c, eight, 0xc);
        n0 = 0x25;
        n1 = 0x2b;
        __Func_8010704(0x25, 0xd, 0xa, 0xc, n0, n1);
    } else {
        g = gState;
        switch (*(short *)(g + (0xe1 << 1))) {
        case 3:
        case 4:
            __Func_8091ff0(0xaa);
            break;
        }
    }
}

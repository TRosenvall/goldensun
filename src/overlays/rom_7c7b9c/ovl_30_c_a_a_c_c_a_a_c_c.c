/* OvlFunc_943_200b710
 *   [asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_c_a_a_c_c.s -- the file's ONLY
 *   function, confirmed with `grep -ci func_start` = 1.  224 instructions.
 *
 *   NO SPLIT NEEDED.  `python3 tools/datacheck.py` on the .s reports no data
 *   section, so the whole file converts: src/overlays/rom_7c7b9c/
 *   ovl_30_c_a_a_c_c_a_a_c_c.c with the .ld line left on the asm/ path
 *   (the build rule is `asm/%.o: src/%.c`).
 *
 *   IT DOES READ THREE .bss LABELS DEFINED ELSEWHERE, in
 *   asm/overlays/rom_7c7b9c/ovl_30_c_c_c_b.s:121-139 --
 *       .global .L5b40 / .lcomm .L5b40, 0x10    8 halfwords
 *       .global .L5b70 / .lcomm .L5b70, 0x20    8 words
 *       .global .L5b90 / .lcomm .L5b90, 4       DECLARED 4, written to +0x1c
 *   All three already carry `.global`, so plain `extern`s with an `__asm__`
 *   name reach them and nothing has to be added to that .s.  The names are
 *   five hex characters, far outside the `.L1`..`.L12` range gcc emits for this
 *   function, so none is captured -- see elevation.md's short-`.LN`-extern
 *   warning ("when the numbers overlap the assembler resolves the pool entry to
 *   the LOCAL label").  `.L5b90`'s declared size of 4 against eight words of
 *   stores is the ROM's own business: as an `extern` the size is never used.
 *
 *   makefile_flags(): NO FLAG GROUP.  rom_7c7b9c has exactly two explicit .c
 *   rules in the Makefile (lines 840 and 844) and BOTH name a different stem;
 *   there is no `%` pattern rule for the directory, so only the generic
 *   `asm/%.o: src/%.c` can reach this path.  objcmp prints no
 *   `(built with: ...)` line.]
 *
 * EXACT, measured with --whole against the original asm/ path, reproduced on
 * three consecutive runs:
 *
 *   OK whole file -- 576 bytes, 230 encodings and 56 relocations identical
 *
 * ------------------------------------------------------------------- NEW ----
 * AN UNSIGNED LOOP COUNTER IS IMMUNE TO check_dbra_loop's LOOP REVERSAL.
 *
 * Every signed spelling of the opening 8-iteration halfword fill came out
 * REVERSED -- `mov r3, #7 / sub r3, #1 / cmp r3, #0 / bge` -- against the ROM's
 * up-count `mov r2, #0 / add r2, #1 / cmp r2, #7 / bls`.  Six signed spellings
 * (`for (i=0;i<8;i++)` over a pointer, over the array, `do{}while(++i<8)`,
 * `while (p < L5b40+8)`, `i<=7`, `i=8;i!=0;i--`) all reversed or were worse.
 * The single change that fixes it is declaring the counter `unsigned`.
 *
 * The mechanism is in check_dbra_loop, loop.c:8004-8012:
 *
 *     if (comparison
 *         && (GET_CODE (comparison) == LT
 *             || (GET_CODE (comparison) == LE && no_use_except_counting)))
 *
 * -- the inc-to-dec rewrite is gated on the exit comparison being signed `LT`
 * (or signed `LE`).  An `unsigned` counter gives `LTU`, which is a DIFFERENT
 * rtx code, so the gate never opens and the loop keeps its up-count.  The
 * `bls`/`cmp #7` in the ROM is then just canonicalize_condition turning
 * `(ltu x 8)` into `(leu x 7)`.
 *
 * This is not the same thing as elevation.md's existing signed/unsigned notes,
 * which are all about load form (`ldrsh`/`ldrsb`) and PROMOTE_MODE.  Here the
 * signedness of a counter that is never loaded or stored decides whether a
 * whole loop runs forwards or backwards.  THE TELL IS A COUNTDOWN AGAINST THE
 * ROM'S COUNT-UP with the pointer still advancing forwards -- check_dbra_loop
 * reverses only the biv in the exit test, so the giv keeps its `add #2`.
 *
 * ------------------------------------------------------ CONFIRMATION --------
 * WRITE THE FILL AS AN ARRAY INDEX, NOT A WALKING POINTER.  With the counter
 * already `unsigned`, `L5b40[u] = 0xc000` is exact and `*p++ = 0xc000` with a
 * separate pointer local is 7 of 230.  The pointer form makes `p` a second biv
 * rather than a giv of `u`, and local-alloc then hands the pointer r2 and the
 * counter r3 where the ROM has them the other way round, with the pool loads
 * emitted in the opposite order.  Two hybrids that keep the pointer but move
 * its initialisation (`for (u = 0, p = L5b40; ...)`, and a `while` with the
 * increments spelled out) are 2 of 230 -- better, still not the ROM.
 *
 * ------------------------------------------------------ CONFIRMATION --------
 * THE HImode-LITERAL RULE, AND HERE IT HAS A DIRECTION.  0xc000 is shiftable
 * (0xc0 << 8), so an `int` carrier would give `mov r1, #0xc0 / lsl r1, #8`.
 * The ROM's `ldr r1, =0xc000` is the tell that the constant reaches the
 * HALFWORD store directly: `movhi` has no CONST_INT alternative, so expand
 * calls force_const_mem.  Writing the literal inline in the store is what
 * produces it, and gcc emits it as `ldrh r1, .L10` over the same pool word --
 * gas assembles `ldrh rD, <label>` and `ldr rD, =K` identically, so this is a
 * spelling difference and not a real one.
 *
 * MEASURED WORSE (objcmp --whole against 576 bytes / 230 encodings):
 *
 *   spelling                                                     differ
 *   ------------------------------------------------------------ ------
 *   `int` counter, array index  (loop REVERSED)                    7
 *   `unsigned` counter, walking pointer `*p++`                     7
 *   `for (u = 0, p = L5b40; u < 8; u++) *p++ = ...`                2
 *   `while (u < 8) { *p = ...; p++; u++; }`                        2
 *   `for (u = 8; u != 0; u--) *p++ = ...`  (4 bytes SHORT)        51 aligned
 *   `while (p < L5b40 + 8) *p++ = ...`                            19 aligned
 *
 * SHIMS: NONE.  No `register ... __asm__` declaration and no
 * `__asm__(".equ ...")` line anywhere in the file -- the three `__asm__` tokens
 * present are all asm-NAME clauses on `extern` declarations, which are how the
 * tree reaches a `.L` label and are not shims.
 *
 * ---------------------------------------------------------------- SHAPE -----
 * Eight map-actor slots, 8 and 0x0d..0x13, brought on stage: zero their two
 * shadow tables, cache each actor's +0x10 into .L5b90, give the four rotating
 * ones a -0x10000 angle, wait a frame, attach them all to slot 8 with
 * __Func_8092b54, set each actor's +0x5c flag, wait, place slot 8 and hand the
 * eight to OvlFunc_943_200b5ec in two groups (kind 2 and kind 3).
 */
extern unsigned short L5b40[8] __asm__(".L5b40");
extern int L5b70[8] __asm__(".L5b70");
extern int L5b90[8] __asm__(".L5b90");

struct Actor {
    unsigned char pad00[8];
    int f08;
    int f0c;
    int f10;
    int f14;
    int f18;
    unsigned char pad1c[0x5c - 0x1c];
    unsigned char f5c;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __WaitFrames(int n);
extern void __Func_8092b54(int slot, int a);
extern void OvlFunc_943_200b380(int slot);
extern void OvlFunc_943_200b5ec(int slot, int a, int b);

void OvlFunc_943_200b710(void)
{
    struct Actor *a;
    unsigned short *p;
    int i;
    unsigned int u;

    for (u = 0; u < 8; u++)
        L5b40[u] = 0xc000;

    OvlFunc_943_200b380(8);
    __MapActor_SetPos(9, 0, 0);
    __MapActor_SetPos(10, 0, 0);
    __MapActor_SetPos(11, 0, 0);
    __MapActor_SetPos(12, 0, 0);
    OvlFunc_943_200b380(13);
    OvlFunc_943_200b380(14);
    OvlFunc_943_200b380(15);

    L5b70[0] = 0;
    L5b70[1] = 0;
    L5b70[2] = 0;
    L5b70[3] = 0;
    L5b90[0] = __MapActor_GetActor(8)->f10;
    L5b90[1] = __MapActor_GetActor(13)->f10;
    L5b90[2] = __MapActor_GetActor(14)->f10;
    L5b90[3] = __MapActor_GetActor(15)->f10;

    OvlFunc_943_200b380(16);
    OvlFunc_943_200b380(17);
    OvlFunc_943_200b380(18);
    OvlFunc_943_200b380(19);
    __MapActor_GetActor(16)->f18 = 0xffff0000;
    __MapActor_GetActor(17)->f18 = 0xffff0000;
    __MapActor_GetActor(18)->f18 = 0xffff0000;
    __MapActor_GetActor(19)->f18 = 0xffff0000;
    L5b70[4] = 0;
    L5b70[5] = 0;
    L5b70[6] = 0;
    L5b70[7] = 0;
    L5b90[4] = __MapActor_GetActor(16)->f10;
    L5b90[5] = __MapActor_GetActor(17)->f10;
    L5b90[6] = __MapActor_GetActor(18)->f10;
    L5b90[7] = __MapActor_GetActor(19)->f10;

    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(8, a->f08, a->f10);

    __WaitFrames(1);
    __Func_8092b54(13, 8);
    __Func_8092b54(14, 8);
    __Func_8092b54(15, 8);
    __Func_8092b54(16, 8);
    __Func_8092b54(17, 8);
    __Func_8092b54(18, 8);
    __Func_8092b54(19, 8);

    __MapActor_GetActor(8)->f5c = 1;
    __MapActor_GetActor(13)->f5c = 1;
    __MapActor_GetActor(14)->f5c = 1;
    __MapActor_GetActor(15)->f5c = 1;
    __MapActor_GetActor(16)->f5c = 1;
    __MapActor_GetActor(17)->f5c = 1;
    __MapActor_GetActor(18)->f5c = 1;
    __MapActor_GetActor(19)->f5c = 1;

    __WaitFrames(1);
    __MapActor_SetPos(8, 0x840000, 0x2780000);
    __WaitFrames(1);
    OvlFunc_943_200b5ec(8, 0, 2);
    OvlFunc_943_200b5ec(13, 1, 2);
    OvlFunc_943_200b5ec(14, 2, 2);
    OvlFunc_943_200b5ec(15, 3, 2);
    OvlFunc_943_200b5ec(16, 4, 3);
    OvlFunc_943_200b5ec(17, 5, 3);
    OvlFunc_943_200b5ec(18, 6, 3);
    OvlFunc_943_200b5ec(19, 7, 3);
}

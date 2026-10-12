/* OvlFunc_924_20090c0 -- 0x020090c0, MATCHES.
 * asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a_b.s
 *
 * 0 of 61 stream encodings.  164 bytes against 164, 61 encodings and 16
 * relocations IDENTICAL, verified with `objcmp --whole`.  Pin-free and
 * device-free (`tools/shimcount.py` prints no rows).  No per-file flag.
 * The park it replaces read 34 of 61 at 39/38 instructions -- ONE SHORT.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a_b.c \
 *     asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a_b.s --whole
 *
 * Split shape: NONE.  tools/piece.py -> the .s holds ONE .thumb_func_start,
 * `all 1 parked, so ONE candidate could land them all`, so the whole file
 * converts and no `.global` export is needed.
 *   tools/datacheck.py asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a_b.s
 *     -> exits 0 silently, no data section.
 *
 * ============================================================
 * BATCH 344, BRIEF B.  THREE EDITS, AND THE PARK'S BOUND WAS WRONG.
 *
 * GAP ARITHMETIC FIRST (16-bit instructions; the 21/21 32-bit entries are 15
 * `bl` calls plus 6 pool words).  This function has no loop, so the regions
 * are the guard cascade, the guarded body and the else arm:
 *
 *     region                                 ref   park   landed
 *     guards  push .. the third `beq`         13     13      13
 *     body    `ldr r0,=0x876` .. `b .L1146`   20     19      20
 *     else + epilogue                          6      6       6
 *     TOTAL                                   39     38      39
 *
 * The whole -1 was in the GUARDED BODY, and it was one specific pair:
 *
 *     ROM    mov r0,#1 | mov r1,#1 | neg r0,r0 | neg r1,r1   (4)
 *     park   mov r1,#1 | neg r1,r1 | mov r0,r1               (3)
 *
 * ---- EDIT 1.  WHY THE SECOND -1 COLLAPSES, AND IT IS NOT THE SPELLING ----
 *
 * TEN spellings of the pair were measured and ALL TEN produce BIT-IDENTICAL
 * assembly -- `m = 1; m = -m; n = 1; n = -n;` (the park's), both negations
 * written as `-1` constants, `0 - 1`, `~0`, `0 - m`, both `mov`s hoisted above
 * both `neg`s, the pairs in the other order, `n = m;`, a literal
 * `__Func_8012330(-1, -1, 0xe666)`, and an anchor between them.  So this is the
 * "the edit never reached flow" class and the park's own `TRIED` list was
 * measuring nothing.
 *
 * NAMED FROM THE DUMPS: the collapse is **pass 03.cse**, and the fix is not to
 * stop cse -- it is to let RELOAD rematerialise.  Per-pass `neg:SI` counts on
 * the parked body: `.00.rtl` 2, `.02.jump` 2, **`.03.cse` 1**, and 1 from there
 * to `.18.greg`.  With both `-1`s written as plain constants the two
 * `(set (reg) (const_int -1))` insns are still unified: `.03.cse` keeps insn
 * 102 and DELETES insn 105 outright.
 *
 * THE DISCRIMINATOR IS THE PARK'S OWN `a1/a2/a3`, which it had already solved
 * without knowing why.  cse unifies those three `(set (reg) (const_int 65536))`
 * too -- `.03.cse` holds exactly one, insn 10 -- and the ROM's three
 * independent `mov r0,#0x80 / lsl r0,#9` builds come back anyway, because that
 * pseudo is **live across three calls** and never gets a hard register, so each
 * argument move is satisfied by rematerialising its REG_EQUIV constant.  By
 * `.12.life` the pseudo is gone entirely.  The parked `m`/`n` are assigned
 * immediately before their call, do NOT cross one, and so `.12.life` still has
 * `Register 37 used 3 times across 4 insns in block 3` and `.18.greg` gives it
 * r1 -- one build and a copy.
 *
 *   >> SO: HOIST THE CONSTANT ABOVE THE CALLS.  `m = -1; n = -1;` before the
 *      guard cascade takes the park 34 of 61 -> **8 of 61 with the instruction
 *      counts EQUAL at 39/39 and relocations clean**, and it is the same
 *      mechanism the park had already used for `a1/a2/a3`.  Measured: hoisting
 *      as `-1` constants 8; hoisting only `m` 8; hoisting `-1` above `a1` 8;
 *      hoisting the pair still written as `mov`+`neg` **43 at gap +1** (worse,
 *      because two separate two-insn pseudos then both survive). <<
 *
 * ---- EDIT 2.  THE PARK'S STATED BLOCKER, AND IT IS ONE STATEMENT ----
 *
 * The park closed with: *"BLOCKER: register roles in the iwram address chain
 * ... The ROM keeps the base in r3 and the offset in r2; ours swaps them"*, and
 * *"the source can move an allocation but cannot choose it."*  **REFUTED.**
 * Computing the OFFSET BEFORE THE BASE LOAD does choose it:
 *
 *     park   q = base; off = 0xe0; off <<= 1; q += off; ...
 *            -> ldr r2,[r3] | mov r3,#0xe0 | lsl r3,#1 | add r2,r2,r3 | ...
 *     here   off = 0xe0; off <<= 1; q = base; q += off; ...
 *            -> mov r2,#0xe0 | ldr r3,[r3] | lsl r2,#1 | add r3,r2 | ...
 *
 * Both are block-local quantities, so this is LOCAL alloc, and the ROM's shape
 * needs the BASE allocated first -- REG_ALLOC_ORDER is `3, 2, 1, 0`
 * (arm.h:989), so whichever quantity local-alloc takes first gets r3, and the
 * ROM's base is in r3.  Moving the base's definition later shortens its
 * death-birth span and it wins the ordering.  8 of 61 -> **2 of 61**.
 * The park had tried swapping the two locals' DECLARATION order, which is
 * exactly inert here (measured: 8, unchanged), and that is why it concluded
 * the allocation was unreachable.  Declaration order is not the lever;
 * DEFINITION order is.
 *
 * ---- EDIT 3.  THE LAST TWO ENCODINGS: ONE INSTRUCTION, ONE SLOT EARLY ----
 *
 *     ROM   mov r0,#1 | mov r1,#1 | neg r0,r0 | neg r1,r1 | ldr r2,=0xe666
 *     u3    mov r0,#1 | mov r1,#1 | neg r0,r0 | ldr r2,=0xe666 | neg r1,r1
 *
 * sched2 slots the third argument's pool load between the two `neg`s.  All 23
 * anchor placements are exactly inert at 2 of 61 -- the anchor cannot reach it,
 * because the `ldr` is a reload rematerialisation and the whole window is one
 * basic block of independent insns.  Naming 0xe666 in its OWN HOISTED LOCAL,
 * the same treatment as `m`, `n` and `a1..a3`, makes it a third
 * live-across-calls constant and it rematerialises in ROM position.  2 -> **0**.
 * Both placements of that assignment match (before and after `n = -1;`).
 *
 * ---- THE RULE THIS FUNCTION ESTABLISHES ----
 *
 * A Thumb constant that needs TWO instructions (`mov`+`lsl` for 0x10000,
 * `mov`+`neg` for -1, a pool `ldr` for 0xe666) and appears N times in one
 * argument list is NOT reproduced by N names next to the call: cse unifies
 * them whatever they are spelled as.  It is reproduced by N names DEFINED
 * BEFORE A CALL THEY ARE LIVE ACROSS, so the pseudo gets no hard register and
 * reload rematerialises the constant once per argument.  Five of this
 * function's six constant builds come from that one mechanism.
 */
extern unsigned char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneWait(int n);
extern void __Func_8012330(int a, int b, int c);
extern void __PlaySound(int id);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8012350(void);
extern void __Func_8091e9c(int n);

void OvlFunc_924_20090c0(void)
{
    unsigned int q;
    unsigned int off;
    int a1, a2, a3;
    int m, n;
    int k;

    a1 = 0x80 << 9;
    a2 = 0x80 << 9;
    a3 = 0x80 << 9;
    m = -1;
    n = -1;
    k = 0xe666;
    if (__GetFlag(0xc4 << 2) != 0 && __GetFlag(0x311) != 0 && __GetFlag(0x312) != 0) {
        __SetFlag(0x876);
        __CutsceneWait(0x1e);
        __Func_8012330(a1, a2, a3);
        __PlaySound(0x8d);
        __CutsceneWait(0x3c);
        off = 0xe0;
        off <<= 1;
        q = (unsigned int)iwram_3001ebc;
        q += off;
        off -= 0xc0;
        *(int *)q = off;
        __MapTransitionOut();
        __WaitMapTransition();
        __PlaySound(0x121);
        __Func_8012330(m, n, k);
        __Func_8012350();
        __Func_8091e9c(0xd);
    } else {
        __ClearFlag(0x876);
    }
}


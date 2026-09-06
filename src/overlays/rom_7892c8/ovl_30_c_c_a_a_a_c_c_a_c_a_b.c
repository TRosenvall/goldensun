/* OvlFunc_888_200a90c  --  0x0200a90c
 *   [asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a.s, 4th of 5]
 *
 * 737 instructions of straight-line cutscene script: 221 call sites, one basic
 * block from `push` to the pool jump, no branch and no conditional anywhere.
 * Two actors are walked, jumped, emoted and animated through a scene; two more
 * (slots 8 and 0xc) are handed a per-frame handler at +0x6c and taken back off
 * it later; five actor flag bytes at +0x5a are set or cleared.
 *
 * VERDICT: exact.
 *
 *   OK OvlFunc_888_200a90c -- 1932 bytes, 742 encodings and 224 relocations
 *   identical
 *
 * against a single-function extract AND against the original
 * `asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a.s` path.  NO FLAG GROUP:
 * `tryc.makefile_flags(...)` returns the EMPTY set -- no Makefile line mentions
 * rom_7892c8 at all -- so the TU falls to the tree default `asm/%.o: src/%.c`
 * at -O2, and objcmp against the original path prints no `(built with: ...)`
 * line.  The match does NOT depend on any flag.
 *
 * LANDING NEEDS A SPLIT, NOT A FILE.  The .s holds FIVE functions --
 * 200a6f0, 200a750, 200a7d4, 200a90c (this one) and 200b098 -- and TWO of the
 * other four are already parked (src/non_matching/overlays/200a750.c and
 * src/non_matching/ovl_7892c8/200a7d4.c), so they stay in asm.  Grepped across
 * the whole tree on the FULL PATH, exactly ONE linker line names the object:
 *
 *     overlays/rom_7892c8/overlay.ld:34
 *         \t\tasm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a.o(.text)
 *
 * `.text` is the object's ONLY section: the .s emits no `.section`, `.data`,
 * `.rodata` or `.bss` directive at all, and the `.data` block of that same
 * script names only `asm/overlays/rom_7892c8/ovl_30_c_c_c.o(.data)`.  So there
 * is no `.data`/`.rodata` line for this .o to remap, and none to add.  Split
 * three ways and replace line 34 with three lines IN ADDRESS ORDER:
 *
 *     asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_a.o(.text)   200a6f0,
 *                                                            200a750, 200a7d4
 *     src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_b.o(.text)   THIS FILE
 *     asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_c.o(.text)   200b098
 *
 * The park at src/non_matching/ovl_7892c8/200a7d4.c already worked this shape
 * out and its LANDING paragraph agrees line for line.
 *
 * -------------------------------------------------------------------------
 * TWENTY-SEVEN PINS, ASCENDING FILL, AND FOUR SPELLING LEVERS
 * -------------------------------------------------------------------------
 *
 * THE PROLOGUE READS AS THREE HELD VALUES, AND ONE OF THEM IS A HIGH REGISTER.
 * `push {r5, r6, lr}` + `mov r6, r8` / `push {r6}`: r5 carries 0xfe, then 1,
 * then the handler address (three values, one register, at three disjoint
 * ranges), r6 carries the 1 stored to +0x64, and r8 carries the 0 stored to
 * +0x6c.  Read by CONTENT that is the whole specification of the source's
 * locals -- and the SINGLE high-register value is a constant placed
 * deliberately, not pressure.  Plain C with no levers pushes r5, r6, r7, r8,
 * r9, sl and fp instead: 576 differing of 742.
 *
 * LEVER 1 -- THE ARGUMENT PINS.  27 sites of the 221.  The candidate set is
 * the sites carrying a REPEATED expensive argument (a shifted build, a pool
 * load or a symbol); 30 sites qualify and the fixpoint keeps 27.  Every pinned
 * site fills q0, q1, q2 in ARGUMENT order.  Confirmation of the template's
 * finding, exact in this function: of the 187 ALL-CHEAP sites -- every argument
 * a bare `mov rN, #imm8` -- not one appears in any fixpoint, and adding six of
 * them back (3, 4, 12, 46, 108, 187) is a TIE in every case.  The screen prunes
 * 221 candidates to 30 at no cost.
 *
 * A PIN AT AN EXPENSIVE SITE THAT IS *NOT* A CSE SITE CAN HURT.  Four sites
 * carry an expensive argument that appears only ONCE: 62 (OvlFunc_888_200b098,
 * three unique shifted words), 120 (`0x80 << 1`), 170 and 171 (the two script
 * symbols).  Pinning 62 costs 3 encodings and pinning 170 costs 2; 120 and 171
 * are ties.  So the candidate set has to be REPEATED-value sites, not
 * expensive-argument sites -- "all 221 sites pinned" is 5 differing, and all 5
 * of them are 62 and 170.  That is the sharper form of the cheapness screen:
 * the test is not "is the argument expensive" but "is the argument expensive
 * AND commoned".
 *
 * MINIMISED TO A FIXPOINT FROM THREE DIRECTIONS AND ALL THREE AGREE.  Starting
 * from the 30 candidates -- which also matches -- pins were dropped one at a
 * time under objcmp until no further drop held.  Ascending, descending and a
 * shuffled drop order converge on the SAME 27, and the second pass over the
 * survivors finds every one REQUIRED.  The three dropped (123, 198, 201) are
 * ties when added back, so 27 is minimal along these paths and not unique.
 *
 * LEVER 2 -- THE HImode STORE MUST GO THROUGH AN `int`.  `*(short *)(a + 0x64)
 * = 1;` does NOT give `mov r6, #1`; gcc-2.96 thumb has no immediate
 * alternative in `*thumb_movhi_insn`, so it emits `ldrh r6, .LC0` and puts a
 * `.word 1` in the pool.  That one extra pool word expires the pool's range
 * early, so gcc dumps a `b .L / .word 1 / .word OvlFunc_888_2008030` block in
 * the MIDDLE of the function where the ROM keeps a single pool at the end --
 * every instruction after it shifts and the residue is 350 of 742.  Writing
 * `k = 1; *(short *)(a + 0x64) = k;` with `int k` makes the value SImode, the
 * `strh` truncates it, and the pool word disappears with the mid-function dump.
 * docs/elevation.md has the mechanism at "gcc-2.96 has no immediate
 * alternative for an HImode constant" -- with this exact +0x64 offset.
 *
 * NEW (measured here): THAT SECTION'S TABLE SAYS ONE SHARED `int` LOCAL IS THE
 * FAILURE MODE, AND HERE IT IS THE ANSWER.  On OvlFunc_936_2009f14 four sites
 * sharing one `zero` coalesced into a pseudo living across a call and cost 83
 * of 103; FOUR separate locals matched.  This function's TWO +0x64 stores want
 * the OPPOSITE: the ROM holds the 1 in r6 ACROSS `bl __MapActor_GetActor` and
 * uses it at both stores, so ONE shared `int k` is correct and is what matches.
 * The rule is not "one local per site" -- it is "as many locals as the ROM has
 * REGISTERS", read off the push list.  Both spellings are the same lever
 * pointed by the reference.
 *
 * LEVER 3 -- THE ACTOR POINTER MUST NOT BE NAMED AT THE CHEAP SITES.
 * `p = __MapActor_GetActor(n); p[0x5a] &= 0xfe;` keeps `p` live and copies:
 * `adds r1, r0, #0 / adds r1, #0x5a`, where the ROM has `adds r0, #0x5a`.  One
 * instruction at each of nine sites.  Inlined -- `__MapActor_GetActor(n)[0x5a]
 * &= 0xfe;` -- r0 is dead after the add and the ROM's form appears.  Naming it
 * everywhere is 616 differing and SIX INSTRUCTIONS LONG.  Already in
 * docs/elevation.md as "A call result in a store expression must not go
 * through a named local", with the same +0x64 example.
 *
 * LEVER 4 -- THE ZERO IS BORN INSIDE THE FIRST MASK, BETWEEN THE `and` AND THE
 * `strb`.  The ROM defines r8:
 *
 *     rom    adds r0, #0x5a / ldrb r2, [r0] / movs r5, #0xfe /
 *            adds r3, r5, #0 / ands r3, r2 /
 *            movs r2, #0 / mov r8, r2 /                <-- here
 *            strb r3, [r0]
 *
 * and does not touch it again for 106 call sites.  The placement of the source
 * assignment resolves to ONE statement position, and every neighbouring one was
 * compiled:
 *
 *     `z = 0;` at the declaration                        39 differing
 *     `z = 0;` as its own statement BEFORE the mask     637, 8 insns SHORT
 *     `q = G(0); z = 0; q[0x5a] = 0xfe & q[0x5a];`        9
 *     `u = q[0x5a]; z = 0; q[0x5a] = 0xfe & u;`         686
 *     `u = q[0x5a] & 0xfe; z = 0; q[0x5a] = u;`        MATCH
 *     `z = 0;` as its own statement AFTER the mask       10
 *
 * so the assignment has to sit between the COMPUTE and the STORE, which needs
 * the compound assignment split into three statements.  That alone takes the
 * whole function from 12 differing to 2.  This is the sharpening of "Two constants in DIFFERENT
 * registers means they are simultaneously live", whose recipe is "assign the
 * zero BEFORE the mask": here before is not early enough OR late enough, and
 * the placement is INSIDE the statement, which needs the compound assignment
 * split into three.  The tell is the same one that section names -- the push
 * list -- but the resolution is one statement finer.
 *
 * LEVER 5 -- THE ORR DESTINATION, AND IT IS *NOT* THE DOCUMENTED FIX.  Three
 * sites do `p[0x5a] |= 1`.  At the first two the ROM ties the result to the
 * loaded byte (`orrs r3, r5`), which plain `|= 1` gives.  At the THIRD -- the
 * last use of the 1, so its register is free -- the ROM ties the result to the
 * CONSTANT (`orrs r5, r3 / strb r5`).  docs/elevation.md's fix for that shape
 * is a narrow constant: `unsigned char m = 1; lv |= m;`.  MEASURED HERE IT IS
 * NOT THE LEVER -- 611 differing, and 564 with the `lv = lv | m` spelling.
 *
 * NEW (measured here): THE SECOND PRESENTATION OF THE ORR-DESTINATION LEVER IS
 * TO NAME THE *RESULT*, NOT THE CONSTANT.
 *
 *     { unsigned char *q; int u;
 *       q = __MapActor_GetActor(1); u = q[0x5a] | 1; q[0x5a] = u; }
 *
 * matches.  `u = 1 | q[0x5a]` is byte-identical to it, so the operand order in
 * the source is NOT the lever (gcc's fold canonicalises it away); what moves is
 * that the IOR's result is a pseudo of its own rather than the load's, and
 * register allocation then ties it to the dying constant.  The existing note
 * says the `int` spelling of the CONSTANT is "no lever at all"; that stays
 * true, and this is a different knob on the same residue.  Add this before
 * concluding a two-line `orr` park is closed.
 *
 * NEW (measured here, and it is the difference between 650 and exact): THE TWO
 * REWRITTEN FLAG SITES MUST NOT SHARE THEIR TEMPORARIES.  Levers 4 and 5 each
 * need a `q` and a `u`.  Written with ONE outer pair used at both, gcc makes
 * one pseudo per NAME, whose two disjoint ranges fuse into a single range
 * spanning ~70 call sites: an extra callee-saved register, `push` four bytes
 * wider, 650 differing.  Giving either site its own braced scope separates them
 * and matches; giving both their own scope also matches.  This is
 * docs/elevation.md's "A variable with DISJOINT live ranges should be two
 * variables" reached from the other side -- there the two ranges were in
 * mutually exclusive switch arms, here they are 70 calls apart in ONE basic
 * block, which is the case where the fusion is most expensive and least
 * visible.
 *
 * -------------------------------------------------------------------------
 * TEARDOWN.  Every knob removed from the finished file and re-measured:
 * -------------------------------------------------------------------------
 *
 *     the 27 pins removed entirely             624 of 742, 8 insns SHORT,
 *                                              push {r5,r6,r7,lr}+r8..fp
 *     all 221 sites pinned                       5 differing (sites 62, 170)
 *     pin site 62 as well                        3
 *     pin site 170 as well                       2
 *     ROM-order fill at the 27 sites            23
 *     descending fill at the 27 sites            4
 *     `*(short *)(a+0x64) = 1` (HImode)        350, mid-function pool dump
 *     `z` hoisted to its declaration             39
 *     `z = 0;` after the whole mask              10
 *     `z = 0;` before the whole mask            637, 8 insns SHORT
 *     `z = 0;` before the mask's COMPUTE          9
 *     `z = 0;` after the mask's LOAD            686
 *     both flag sites left as plain `|=`/`&=`    12
 *     the actor pointer named at every site     616, 6 insns LONG
 *     one shared `q`/`u` for both flag sites    650, 2 insns LONG
 *     `unsigned char m = 1; lv |= m;`           611
 *     `unsigned char m = 1; lv = lv | m;`       564, 5 insns LONG
 *     the 42-style block form
 *       `u = q[..]; w = 1; w |= u; q[..] = w;`  649
 *
 * TIES, so none of these is a lever: `0xc0 << 8` against `0xc000` at every
 * pinned site (GEN_WHOLE, exact); `u = 1 | q[0x5a]` against `u = q[0x5a] | 1`;
 * blocking only ONE of the two flag sites rather than both; pinning any of
 * sites 3, 4, 12, 46, 108, 187 (all-cheap), 120, 171, 123, 198 or 201.
 *
 * OvlFunc_888_200b098 IS ARITY 4 BY THE CALL SITE, NOT BY ITS BODY.  Site 62
 * builds r1 = 0xb8<<16, r2 = 0xd8<<13 and r3 = 0xa8<<16 before r0 = 0xde, yet
 * the callee reads only r0 and clobbers r1-r3 in its first `bl __CreateActor`.
 * The declaration the caller was compiled against has four parameters and the
 * implementation ignores three; declaring two or three here loses the setup.
 *
 * -- generated from scratch_elev/b244/f200a90c/{extract,mkbody,gen}.py; the
 *    site table is calls.txt, the pin set is the second argument to gen.py, and
 *    the spelling knobs are the environment variables gen.py documents:
 *    GEN_INLINE=1 GEN_ZERO=pos GEN_ZWHERE=mid GEN_STRH=3
 *    GEN_FORM="0:6 4:6" GEN_BLOCK="0 4".
 */
extern unsigned char ActorCmd_ARRAY_888__0200b740[];
extern unsigned char gScript_888__0200b81c[];
extern int OvlFunc_888_2008030(int a);

extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_SetSpeed(int slot, int x, int z);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_888_200b098(int a, int b, int c, int d);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_888_200a90c(void)
{
    int k;
    int z;

    { PIN3; q0 = 0x0; q1 = 0xc0 << 9; q2 = 0xc0 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 9; q2 = 0xc0 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80925cc(0xc, 0x2);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xc, 0x3);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x0, 0x3);
    __CutsceneWait(0xf);
    __Func_809280c(0x0, 0x1, 0x0);
    __Func_809259c(0x0, 0x1);
    {
        unsigned char *q;
        int u;
        q = __MapActor_GetActor(0x0);
        u = q[0x5a] & 0xfe;
        z = 0;
        q[0x5a] = u;
    }
    __Func_809218c(0x0, 0xb8, 0xa8);
    __MapActor_GetActor(0x1)[0x5a] &= 0xfe;
    __Func_80921c4(0x1, 0xc8, 0xa8);
    __CutsceneWait(0x1);
    __MapActor_GetActor(0x1)[0x5a] |= 0x1;
    __MapActor_WaitMovement(0x0);
    __MapActor_SetAnim(0x0, 0x1);
    __MapActor_GetActor(0x0)[0x5a] |= 0x1;
    {
        unsigned char *q;
        int u;
        q = __MapActor_GetActor(0x1);
        u = q[0x5a] | 0x1;
        q[0x5a] = u;
    }
    __MapActor_Jump(0x1, 0x2, 0x0);
    __CutsceneWait(0xf);
    __Func_809280c(0x1, 0x8, 0x0);
    __CutsceneWait(0x5);
    __MapActor_Jump(0x1, 0x2, 0x0);
    __CutsceneWait(0x19);
    __Func_80925cc(0x1, 0x2);
    __Func_809280c(0x1, 0xc, 0x0);
    __CutsceneWait(0x5);
    __MapActor_DoAnim(0x1, 0x3);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x0, 0x3);
    __CutsceneWait(0x5);
    __MapActor_DoAnim(0x0, 0x3);
    __CutsceneWait(0xa);
    __Func_809280c(0x1, 0x0, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x1, 0x3);
    __CutsceneWait(0xf);
    __MapActor_SetAnim(0xb, 0x3);
    __MapActor_SetAnim(0xc, 0x3);
    __MapActor_SetAnim(0x8, 0x3);
    __MapActor_SetAnim(0x9, 0x3);
    __MapActor_DoAnim(0xa, 0x3);
    __CutsceneWait(0x14);
    __Func_809280c(0x0, 0xc, 0x0);
    __Func_809280c(0x1, 0xc, 0x0);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_DoAnim(0x1, 0x3);
    __CutsceneWait(0x14);
    __Func_809280c(0x0, 0xb, 0x0);
    __Func_809280c(0x1, 0xb, 0x0);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_DoAnim(0x1, 0x3);
    __CutsceneWait(0x14);
    __Func_8092adc(0x0, 0x0, 0x0);
    __CutsceneWait(0xf);
    __Func_80925cc(0x0, 0x2);
    __CutsceneWait(0xa);
    OvlFunc_888_200b098(0xde, 0xb8 << 16, 0xd8 << 13, 0xa8 << 16);
    __Func_809280c(0x1, 0x0, 0x0);
    __CutsceneWait(0xa);
    __Func_80925cc(0x1, 0x1);
    __CutsceneWait(0xa);
    __MapActor_Jump(0x1, 0x4, 0x0);
    { PIN3; q0 = 0x1; q1 = 0xd0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xf);
    { PIN3; q0 = 0x1; q1 = 0xb0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x1; q1 = 0xd0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x1; q1 = 0xb0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x1; q1 = 0xd0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    __MapActor_Jump(0x1, 0x4, 0x0);
    { PIN3; q0 = 0x1; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xf);
    { PIN3; q0 = 0x1; q1 = 0xa0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x1; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x1; q1 = 0xa0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x1; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x0; q1 = 0x81 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x81 << 1; q2 = 0x0;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    __Func_809280c(0x1, 0xc, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x1, 0x3);
    __CutsceneWait(0xa);
    k = 0x1; *(short *)(__MapActor_GetActor(0x8) + 0x64) = k;
    *(int *)(__MapActor_GetActor(0x8) + 0x6c) = (int)OvlFunc_888_2008030;
    k = 0x1; *(short *)(__MapActor_GetActor(0xc) + 0x64) = k;
    *(int *)(__MapActor_GetActor(0xc) + 0x6c) = (int)OvlFunc_888_2008030;
    __Func_80921c4(0x1, 0xc4, 0xb4);
    __Func_80921c4(0x1, 0xb8, 0xb8);
    __Func_80921c4(0x1, 0xb4, 0xb4);
    __Func_80921c4(0x1, 0xa8, 0xa8);
    __Func_80921c4(0x1, 0xb4, 0x9c);
    __Func_809218c(0x1, 0xc8, 0x68);
    __Func_80921c4(0x0, 0xc0, 0xa8);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_WaitMovement(0x1);
    __CutsceneWait(0x1e);
    __Func_80925cc(0x1, 0x1);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x1; q1 = 0xa0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xf);
    *(int *)(__MapActor_GetActor(0xc) + 0x6c) = z;
    *(int *)(__MapActor_GetActor(0x8) + 0x6c) = z;
    __Func_809259c(0x8, 0x2);
    __MapActor_Emote(0x8, 0x80 << 1, 0x0);
    __CutsceneWait(0x3c);
    __MapActor_SetAnim(0x8, 0x0);
    __MapActor_Emote(0x0, 0x81 << 1, 0x0);
    __CutsceneWait(0x3c);
    __Func_8092848(0x0, 0xb, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x0, 0x3);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xb, 0x3);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    __Func_80925cc(0x0, 0x2);
    __CutsceneWait(0xa);
    __MapActor_Jump(0x0, 0x2, 0x0);
    __CutsceneWait(0x14);
    __MapActor_Jump(0x0, 0x2, 0x0);
    __CutsceneWait(0x14);
    __CutsceneWait(0xf);
    __Func_8092848(0x0, 0xc, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x0, 0x3);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xc, 0x3);
    __CutsceneWait(0x3c);
    __Func_80921c4(0x1, 0xd0, 0xa8);
    __Func_809280c(0x0, 0xb, 0x0);
    __Func_809280c(0x1, 0xc, 0x0);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x0, 0x4);
    __MapActor_DoAnim(0x1, 0x4);
    __CutsceneWait(0xa);
    __Func_809259c(0x0, 0x1);
    __Func_80925cc(0x1, 0x1);
    __CutsceneWait(0xa);
    __Func_8092adc(0x1, 0x0, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x1, 0x3);
    __CutsceneWait(0xa);
    __Func_809280c(0x1, 0xc, 0x0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xc, 0x3);
    __CutsceneWait(0xa);
    __Func_8092adc(0x0, 0x0, 0x0);
    __Func_8092adc(0x1, 0x0, 0x0);
    __CutsceneWait(0xa);
    __Func_809259c(0x0, 0x2);
    __Func_80925cc(0x1, 0x2);
    __CutsceneWait(0xa);
    __MapActor_SetBehavior(0x0, ActorCmd_ARRAY_888__0200b740);
    __MapActor_SetBehavior(0x1, gScript_888__0200b81c);
    __MapActor_WaitScript(0x0);
    __MapActor_WaitScript(0x1);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 9; q2 = 0xc0 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xc0 << 9; q2 = 0xc0 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_Jump(0x0, 0x6, 0x0);
    __MapActor_Jump(0x1, 0x6, 0x0);
    __Func_809280c(0x0, 0x9, 0x0);
    __Func_809280c(0x1, 0x8, 0x0);
    __CutsceneWait(0x1);
    __Func_809280c(0x0, 0xc, 0x0);
    __Func_809280c(0x1, 0xb, 0x0);
    __CutsceneWait(0x1);
    __Func_809280c(0x0, 0x8, 0x0);
    __Func_809280c(0x1, 0x9, 0x0);
    __CutsceneWait(0x1);
    __Func_809218c(0x0, 0xc0, 0xa8);
    __Func_80921c4(0x1, 0xd0, 0xa8);
    __MapActor_WaitMovement(0x0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xd0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x0; q1 = 0xa0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xb0 << 8; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x0; q1 = 0xc0 << 6; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x1, 0xd0 << 8, 0x0);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0x0; q1 = 0xa0 << 7; q2 = 0x0;
      __Func_8092adc(q0, q1, q2); }
    __Func_8092adc(0x1, 0xb0 << 8, 0x0);
    __CutsceneWait(0x14);
    __Func_809280c(0x0, 0xb, 0x0);
    __Func_809280c(0x1, 0xc, 0x0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_DoAnim(0x1, 0x3);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0xb, 0x3);
    __MapActor_DoAnim(0xc, 0x3);
    __CutsceneWait(0x1e);
    __Func_8092848(0x0, 0x1, 0x0);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(0x0, 0x3);
    __MapActor_DoAnim(0x1, 0x3);
    __CutsceneWait(0xa);
    __Func_8092adc(0x1, 0x0, 0x0);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0x0, 0x2);
    __MapActor_SetAnim(0x1, 0x2);
    __CutsceneWait(0x3c);
}

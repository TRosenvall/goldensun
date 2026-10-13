/* Field_Halt -- 0x0809abb4.  EXACT.
 * ref (post-split): asm/rom_8a000/rom_9a44c_c_c_c_b.s
 * objcmp --func Field_Halt: OK -- 444 bytes, 191 encodings and 30 relocations
 *   identical.  objcmp --whole against the post-split function-only reference:
 *   OK whole file -- 444 bytes, 191 encodings and 30 relocations identical.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_8a000/rom_9a44c_c_c_c_b.c asm/rom_8a000/rom_9a44c_c_c_c_b.s --whole
 *
 * batch 349, brief A.  PIN-FREE, DEVICE-FREE, ANCHOR-FREE, PRODUCTION FLAGS
 * (shimcount.py reports nothing; no volatile, no do{}while(0), no .equ, no
 * fictitious symbol, no union).  Parked for nine passes at 1 differing encoding
 * of 191 -- the closest park in the tree -- and the one differing encoding was
 * index 98, ref `dbc9` (blt) against ours `d1c9` (bne).
 *
 * SPLIT: this .s held Field_Halt plus a 12-byte .rodata blob
 *   `.global .La012c / .La012c: .incrom 0xa012c, 0xa0138`
 * which is read by the ALREADY-LANDED sibling src/rom_8a000/rom_9a44c_a_a_a_c.c
 * (`extern struct Script *La012c[] __asm__(".La012c");`), so the conversion is a
 * TEXT/DATA split, not a whole-file one: `split_s.py asm/rom_8a000/rom_9a44c_c_c_c.s
 * Field_Halt` writes _b.s (the function, 198 lines, replaced by this .c) and
 * _c.s (the data, 6 lines).  `.La012c` is already `.global`, so the split needs
 * NO new export (datacheck.py says so).
 *
 * ===================== WHAT CLOSED IT =====================
 * THE LOOP EXIT IS A switch CASE RUN, NOT A COMPARISON.  The ROM's back edge is
 *     cmp r7, #0xb
 *     blt .L9ac1e        @ backward, imm8 0xc9 = -55
 * and `(lt X (const_int 11))` is UNREACHABLE from any comparison a human can
 * write, because fold-const.c:6269-6291 rewrites `X < C` to `X <= C-1` for every
 * positive C before RTL exists (gate: `TREE_CODE (arg1) == INTEGER_CST &&
 * TREE_CODE (arg0) != INTEGER_CST && tree_int_cst_sgn (arg1) > 0`), and
 * combine.c:10138-10158 performs the same rewrite a second time, with the
 * reverse LE->LT direction guarded by `const_op < 0`.  Eleven literal spellings,
 * eight constant-on-the-left spellings and six named-bound spellings were
 * measured against this body across batches 327-336 and none reached it.
 *
 * THE ONE PASS THAT EMITS A COMPARISON WITHOUT GOING THROUGH fold IS
 * `emit_case_nodes`.  stmt.c:6346-6369, the pure-leaf arm:
 *
 *     else
 *       {
 *         if (!node_has_high_bound (node, index_type))
 *           emit_cmp_and_jump_insns (index, node->high, GT, ..., default_label);
 *         if (!node_has_low_bound (node, index_type))
 *           emit_cmp_and_jump_insns (index, node->low,  LT, ..., default_label);
 *         emit_jump (label_rtx (node->code_label));
 *       }
 *
 * `node_has_high_bound` (stmt.c:6004-6042) returns 1 when
 * `tree_int_cst_equal (node->high, TYPE_MAX_VALUE (index_type))`, so a case node
 * whose high end is INT_MAX SUPPRESSES THE GT TEST ALTOGETHER;
 * `node_has_low_bound` (stmt.c:5955-5989) returns 0 for low == 11 with no left
 * child and no parent, so the LT test IS emitted -- as RTL, straight out of
 * `emit_cmp_and_jump_insns`, never through fold.  One case node, [11, INT_MAX],
 * therefore emits EXACTLY `cmp index,#11 / blt default_label` and nothing else.
 *
 * The rest is label placement: `default_label` must be the loop's back edge and
 * `node->code_label` the loop exit, so that the trailing `emit_jump (code_label)`
 * targets the next insn and jump.c deletes it.  Writing the switch as the last
 * statement of a `for (;;)` body with no `default:` arm puts the default label at
 * the end of the switch == the end of the loop body == the back edge, and
 * `goto loop1_done;` as the case's code sends the exit forward.  Result: one
 * instruction pair, `cmp r7,#0xb / blt .Lloop`, which is the ROM.
 *
 * `index_type` is `int` and cannot be narrowed: c-typeck.c's
 * `c_expand_start_case` does `exp = default_conversion (exp); type = TREE_TYPE
 * (exp);` before `expand_start_case`, and `default_conversion` converts an
 * ENUMERAL_TYPE to int, so an enum index type (whose TYPE_MAX_VALUE would be 11
 * and would read far more naturally) does NOT survive.  The `... 0x7fffffff`
 * upper end is load-bearing: it is what makes `node_has_high_bound` true.
 *
 * MEASURED, three spellings of the case run, and THEY ARE ONE PROGRAM (object
 * fingerprint, not .s text) -- all three are byte-exact:
 *     switch (i) { case 11 ... 0x7fffffff: goto loop1_done; } in a for(;;)   0
 *     the same switch with `default: continue;` / `case ...: break;`         0  bit-identical
 *     the same switch inside `do { ... } while (1);`                         0  bit-identical
 *     `0x7fffffff` written as `2147483647`                                   0  bit-identical
 * AND AN ENUMERATED RUN IS THE WRONG POLARITY: `case 0: ... case 10:
 * continue;` is 87 of 191 with the stream 4 bytes long, because the node is
 * [0,10] and `node_has_low_bound` is false for low == 0 (0 != INT_MIN), so gcc
 * emits `cmp #0xa / bgt` AND `cmp #0 / blt` -- two tests, not one.
 *
 * ALSO MEASURED AND INERT against this body, all BIT-IDENTICAL to it:
 *     dropping the now-dead `n = 11;` store      (it was the parked body's)
 *     dropping `int n;` entirely
 *     dropping the unused `int *vp;`
 *     dropping the unused `union blob` type      (copied in from the twin)
 * all four are taken here, so the shipped body carries no leftovers.
 *
 * WORSE, measured this pass: putting the increment BEFORE `WaitFrames(1)` in the
 * source (6 of 191, first at index 82) -- the `for` increment belongs after the
 * body and sched2 moves it in front of the call on its own.
 *
 * ===== THE NAMED-BOUND ROUTE IS REAL BUT COSTS FOUR INSTRUCTIONS HERE =====
 * Kept because the park spent four passes on it and the reason is structural,
 * not a spelling.  `n = 11; for (; i < n; i++)` is how the LANDED STRUCTURAL TWIN
 * Field_Whirlwind (src/rom_8a000/rom_9a44c_c_c_a_a.c, byte-exact) reaches its own
 * `cmp r3,#11 / blt`, and it works there because its bound pseudo gets NO HARD
 * REGISTER: reg 40 is LAST in `.18.greg`'s `;; 13 regs to allocate` list and is
 * ABSENT from `;; Register dispositions`, so reload substitutes its
 * `reg_equiv_constant` into `cbranchsi4`'s "rI" operand for free.  It is last
 * because update_equiv_regs does `REG_LIVE_LENGTH (regno) *= 2` for a
 * constant-equivalent pseudo (local-alloc.c:870), halving allocno_compare's
 * priority; and it loses because the twin's r11 is already held by reg 34
 * (`34 in 11`, `Hard regs used: ... 11 ...`).
 *
 * IN Field_Halt r11 IS FREE -- the ROM's prologue saves exactly r5-r10 -- so the
 * same construct hands the bound r11 instead of spilling it.  MEASURED, four
 * placements, `.18.greg` says `39 in 11` in every one:
 *     n = 11 where the park put it, `i < n`        179 of 195, +8 bytes, RELOCDIFF
 *     n = 11 before CreateParticleActor            178 of 195, +8 bytes
 *     i = 0; n = 11; both before the call          178            bit-identical to the above
 *     n = 11 immediately before the loop           180 of 195, +8 bytes
 * and the diff against this body is exactly `mov r7, r8 / push {r7}` +
 * `mov r2, #11 / mov fp, r2` + the matching pops, with the compare reading
 * `cmp r7, fp` -- NOT an immediate.  So it is four instructions AND still the
 * wrong encoding at index 97.
 *
 * WHY NO ALLOCATION LEVER CAN FIX THAT, with the lines: `find_reg`
 * (global.c:1010-1050) takes r11 whenever it is free; r11 is the last general
 * register in `REG_ALLOC_ORDER` (arm.h:989-995) but it is still in ALL_REGS; the
 * bound's alternate class IS ALL_REGS, because regclass.c:1217-1232 only yields
 * NO_REGS when every non-preferred class costs at least `mem_cost` and
 * `.17.lreg` measures `HI_REGS:2` against `MEM:72`; and global.c:565 runs the
 * alternate-class pass UNCONDITIONALLY.  The two escapes are both shut:
 * `update_equiv_regs`' substitute-and-delete path needs `REG_N_REFS == 2`
 * (local-alloc.c:885-889) and flow.c's loop-depth weighting makes it 3 for a
 * bound set at depth 0 and used in a loop test at depth 1; and the only setter
 * of the `REG_LIVE_LENGTH == -1` "refrain from putting this in a hard reg" flag
 * that global.c:421-424 honours is the setjmp path at flow.c:2820-2827.
 * Occupying r11 with a seventh call-crossing allocno costs the push/pop.
 * SO: the named bound is a LEVER ON A FUNCTION WITH r11 ALREADY TAKEN, and a
 * four-instruction tax on one without.  Do not retry it here.
 *
 * CORPUS CONTROL, re-run this pass over all 4,000+ generated .s files: exactly
 * ONE gcc-generated backward `cmp rN,#K / blt` with K > 0 exists in the tree, the
 * twin's, and 26 forward ones -- every forward one a switch range test.  Three
 * hand-written backward sites with #11 remain as targets:
 * asm/rom_8a000/rom_97b54_c_c_a.s:146 and :369 (`cmp r1,#11`) and
 * asm/rom_8a000/rom_9ad70_a_c.s:164 and :266 (`cmp r7,#11`), all in this bank.
 * They are the same animation shape; try this case run on them first.
 *
 * ===== NEGATIVE-LIST ACCOUNTING (batch 349's method), and two corrections =====
 * The park presents ~30 comparison/bound spellings as tested.  Counted as
 * DISTINCT PROGRAMS: the 11 literal spellings are 1 program; the 6
 * declaration-initialised bounds are at most 5 (five rows share 164/159 and 178
 * RELOCDIFF); the 4 body-assigned bounds re-tested this pass are 3 (the
 * "before the call" and "i = 0; n = 11; before the call" rows are BIT-IDENTICAL);
 * the 8 constant-on-the-left rows are 8 but every one is 161-163 instructions
 * against 159, so none was ever a distance; and of the 51 anchor placements 22
 * were bit-identical, leaving 29.  So well under half of the recorded rows are
 * distinct programs -- but the real gap was not the duplication.  EVERY recorded
 * row varied the COMPARISON or the BOUND.  Nothing had ever varied the
 * STATEMENT FORM, which is where the answer was.
 *
 * (1) The park's VERDICT was wrong while all of its OBSERVATIONS reproduced.
 *     Its rung 1 -- "`(lt X 11)` is UNREACHABLE as a tree" -- is TRUE, and was
 *     then used to conclude the ENCODING is unreachable.  emit_case_nodes never
 *     builds a tree comparison at all, so rung 1 never applied to it.
 * (2) `reload_cse_move2add` (reload1.c:8840-8990) is NOT a route to this
 *     encoding and should stop being named as one.  Read: it rewrites
 *     `(set REGX (CONST_INT A)) ... (set REGX (CONST_INT B))` into
 *     `(set REGX (plus REGX (B-A)))` for THE SAME REGISTER (:8863-8900).  It
 *     never touches a comparison operand.  The constant-substituting pass is
 *     reload's own `reg_equiv_constant`, which needs the pseudo SPILLED.
 *
 * THE GENERALISATION WORTH CARRYING OUT OF THIS PARK: a named bound reaches
 * `cmp #K / blt` only on a function whose r11 is ALREADY TAKEN by another
 * call-crossing allocno; on one whose r11 is free it is a four-instruction tax.
 * Check `Hard regs used` in .18.greg before spending a round on it.
 */
extern int *iwram_3001f30;
extern unsigned char *CreateParticleActor(int a, int b, int c, int d);
extern void Func_8097384(void);
extern void _Actor_SetAnim(unsigned char *p, int n);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern unsigned int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern void Func_80974d8(int *v);
extern void Func_809ba90(unsigned char *e, int a, int b, int c);
extern void Func_809ba7c(unsigned char *e, void (*f)(unsigned char *));
extern void _Sprite_SetColorswap(void *s, int n);
extern void _DeleteActor(unsigned char *p);
extern void Func_809748c(void);
extern void Func_809aa98(unsigned char *e);

void Field_Halt(void)
{
    int v[3];
    int from[3];
    int to[3];
    int *o;
    int *t;
    unsigned char *p;
    unsigned char *ep;
    int i;
    int k;
    int w;
    int c;

    o = iwram_3001f30;
    t = (int *)o[4];
    o[2] = t[3];
    p = CreateParticleActor(0xfa, 0, 0, 0);
    i = 0;
    _Actor_SetAnim(p, 0);
    if (p == 0)
        return;
    Func_8097384();
    from[0] = t[2];
    from[1] = t[3] + (0x80 << 13);
    from[2] = t[4];
    to[0] = o[1];
    to[1] = o[2] + (0x80 << 12);
    to[2] = o[3];
    for (;;) {
        *(int *)(p + 8) = from[0] + (to[0] - from[0]) * i / 10;
        *(int *)(p + 0xc) = from[1] + (to[1] - from[1]) * i / 10;
        *(int *)(p + 0x10) = from[2] + (to[2] - from[2]) * i / 10;
        c = 0xc0 << 8;
        w = c * i / 10 + (0x80 << 7);
        *(int *)(p + 0x18) = w;
        *(int *)(p + 0x1c) = w;
        WaitFrames(1);
        i++;
        switch (i) {
        case 11 ... 0x7fffffff:
            goto loop1_done;
        }
    }
loop1_done:
    ;
    WaitFrames(5);
    _Actor_SetAnim(p, 1);
    _PlaySound(0x6c);
    WaitFrames(10);
    _PlaySound(0x6c);
    WaitFrames(10);
    _PlaySound(0x6c);
    WaitFrames(10);
    _PlaySound(0x6d);
    ep = (unsigned char *)o + 0x58;
    for (k = 0; k < 16; k++) {
        v[0] = *(int *)(p + 8);
        v[1] = *(int *)(p + 0xc) + (0x80 << 12);
        v[2] = *(int *)(p + 0x10);
        Func_80974d8(v);
        vec3_translate(0x80 << 11, Random(), v);
        Func_809ba90(ep, 0x11d, v[0], v[2]);
        Func_809ba7c(ep, Func_809aa98);
        _Sprite_SetColorswap(*(void **)ep, 7);
        ep += 0x48;
    }
    v[0] = *(int *)(p + 8);
    v[1] = *(int *)(p + 0xc) + (0x80 << 12);
    v[2] = *(int *)(p + 0x10);
    WaitFrames(8);
    _DeleteActor(p);
    WaitFrames(4);
    WaitFrames(0x1e);
    Func_809748c();
}

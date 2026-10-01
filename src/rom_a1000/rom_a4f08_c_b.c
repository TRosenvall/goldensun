/* Func_80a524c -- ConfirmItemAction -- 0x080a524c, asm/rom_a1000/rom_a4f08_c.s
 *
 * *** THE FOUR CODE ENCODINGS ARE CLOSED (batch 315).  3 of 134, AND ALL THREE
 * ARE THE _MSG_* POOL PLACEHOLDERS -- THE CODE IS BYTE-IDENTICAL. ***
 *
 * 316 bytes against 316, 134 encodings against 134, and with the three
 * `_MSG_*` values supplied as `.equ` shims this file is 0 OF 134 with the
 * relocation table identical.  So the function is DONE as C; what remains is a
 * SYMBOL ADMISSION, not a codegen problem.
 *
 * objcmp, production flags, no shim, no flag added, verbatim:
 *   XX ENCODINGS differ in 3 place(s) (ref 134, ours 134)
 *      first at index 129
 *   XX RELOCATIONS differ      (ours has 23, ref 20)
 *   [129] ref .word 0x00000182 | ours .word 0x00000000 + R_ARM_ABS32
 *   [130] ref .word 0x00000ad4 | ours .word 0x00000000 + R_ARM_ABS32
 *   [131] ref .word 0x00000b2c | ours .word 0x00000000 + R_ARM_ABS32
 * objcmp's reference assembles `ldr r3, =0x182` to a literal; ours emits a zero
 * word plus an ABS32 the linker folds.  Both sides agree once the `.equ`s exist.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/rom_a1000/rom_a4f08_c_b.c (LANDED; was src/non_matching/rom_a1000/80a524c.c) \
 *     asm/rom_a1000/rom_a4f08_c.s --func Func_80a524c
 *
 * ============================= LANDING PREREQUISITES =============================
 *  1. `_MSG_ad4 = 0x0ad4` and `_MSG_b2c = 0x0b2c` admitted to message.sym.
 *     `_MSG_182 = 0x0182` is ALREADY admitted (message.sym:341), as is the
 *     neighbouring `_MSG_ad0 = 0x0ad0` (message.sym:233).  THIS IS THE ONLY
 *     THING LEFT, and it is the owner decision the previous revision raised.
 *     The evidence for the two proposals is POSITIONAL, not size: neither value
 *     is shiftable (0xad4 = 0x2b5 << 2, 0xb2c = 0x2cb << 2, both multipliers
 *     > 255), so neither carries the shiftability pool tell that admitted
 *     _MSG_182, _MSG_ad0 and _MSG_b20.  As CONST_INTs they cost SIX encodings
 *     each, independently and additively (13, 13, 19 -- measured in batch 295
 *     and unchanged by this batch's fix), because sched2 hoists a CONST_INT pool
 *     load above the preceding `bl _Func_801e7c0` while it does not move the
 *     SYMBOL_REF one.  They feed the same sink (_Func_801e7c0) in the same
 *     function as the already-admitted _MSG_182.
 *  2. A TEXT/DATA SPLIT of asm/rom_a1000/rom_a4f08_c.s -- three functions
 *     (Func_80a51d0, Func_80a524c, Func_80a5388) plus `.rodata`; the data keeps
 *     its own object.  tools/datacheck.py, re-run this batch:
 *     **ZERO new exports** (every one of the three reads no data label).  The
 *     other two functions are unattempted, so the .s does not convert whole.
 *  3. SHIMS: ZERO.  tools/shimcount.py reports none.  The three
 *     `extern char _MSG_*[]` declarations are the tree's ordinary symbol-address
 *     technique, not shims.
 *
 * ========================== HOW THE FOUR CODE ENCODINGS CLOSED ==========================
 *
 * THE FIX IS THE PLACEMENT OF THREE STATEMENTS.  `n = 2; sel = 1; moved = 1;`
 * used to sit just before `Func_80a1ac0(0x68, 0x56)`.  They now sit at the TOP
 * of the function, before `win = _CreateUIBox(...)`.  Nothing in them depends on
 * the call or on anything between, so it is the same program; it changes no
 * instruction at the move site, and it fixes all four differing encodings at
 * once.  Nothing else in this file changed.
 *
 *     installed park        7 = 4 code + 3 pool, first at index 64
 *     this file             3 = 0 code + 3 pool, first at index 129
 *     this file + .equ shims   0 of 134, size exact, relocations identical
 *
 * The four were two sites, both wanting r2 where we gave r3:
 *     [64] mov r2,#1   ours mov r3,#1    ) the Up block's `moved = 1`
 *     [67] mov r8,r2   ours mov r8,r3    )
 *     [86] mov r2,r8   ours mov r3,r8    ) the `if (moved)` test
 *     [87] cmp r2,#0   ours cmp r3,#0    )
 * `moved` lives in r8, so every touch of it needs a low reload register.  In
 * address order the reference's four are r2, r3, r2, r3; ours were r3 four times.
 *
 * THE PREVIOUS REVISION HAD THE MECHANISM RIGHT AND THE CURE WRONG.  Its
 * arithmetic is confirmed: `spill_regs` is built ASCENDING over
 * `used_spill_regs` (reload1.c:3527-3536), `last_spill_reg` is -1 at entry
 * (reload1.c:821), and `allocate_reload_reg` round-robins from there
 * (reload1.c:5003-5011).  With `used_spill_regs == {r3}` and `n_spills == 1`
 * there is nothing to rotate over and every reload gets r3; with `{r2,r3}` the
 * rotation is r2, r3, r2, r3.  What is new is that THE FOUR PICKS ARE IN
 * ADDRESS ORDER (the .18.greg trace runs insn 230, 252, 133, 138 = indices 64,
 * 74, 86, 89), which is what makes the identity exact rather than suggestive.
 *
 * *** THE PREVIOUS REVISION'S ROUTE (iii) -- "r3 in bad_spill_regs.  Not
 * reachable from source." -- IS THE ROUTE THAT WORKED. ***  reload1.c:1525-1537:
 *     COPY_HARD_REG_SET (bad_spill_regs, fixed_reg_set);
 *     REG_SET_TO_HARD_REG_SET (used_by_pseudos,  &chain->live_throughout);
 *     REG_SET_TO_HARD_REG_SET (used_by_pseudos2, &chain->dead_or_set);
 *     IOR_HARD_REG_SET (bad_spill_regs, used_by_pseudos);
 *     IOR_HARD_REG_SET (bad_spill_regs, used_by_pseudos2);
 * `bad_spill_regs` unions in every HARD REGISTER live at the insn, and find_reg
 * removes it from consideration entirely (reload1.c:1603-1605).  AN OUTGOING
 * ARGUMENT REGISTER IS SUCH A HARD REGISTER.  At the top of this function r3 is
 * live as _CreateUIBox's fourth argument (`movs r3, #10`, index 9), so with the
 * `moved = 1` reload placed there find_reg cannot see r3 and returns r2.
 * .18.greg, this file, verbatim -- the whole fix in one line:
 *     Spilling for insn 18.
 *     Using reg 2 for reload 0        <-- was `Using reg 3`
 *     Spilling for insn 22.
 *     Spilling for insn 230. / Using reg 3 for reload 0
 *     Spilling for insn 252. / Using reg 3 for reload 0
 *     Spilling for insn 133. / Using reg 3 for reload 0
 *     Spilling for insn 138. / Using reg 3 for reload 0
 *
 * AND IT IS FREE BECAUSE THAT RELOAD EMITS NO INSTRUCTION.  `sel = 1` and
 * `moved = 1` share the constant 1, so the reload is INHERITED from `sel`'s r6
 * and `allocate_reload_reg` is never called for it -- indices [48]/[51] are
 * `movs r6,#1 / mov r8,r6` before and after.  **A RELOAD WHOSE REGISTER IS
 * INHERITED STILL RUNS find_reg AND STILL WIDENS `used_spill_regs`.**  That is
 * what makes its register choice a cost-free source-level lever, and it is why
 * the previous revision's search -- which only ever looked at the two DIFFERING
 * sites -- could not find it.
 *
 * THE RULE, for docs/elevation.md:19794's existing section:
 *   TO WIDEN A TOO-NARROW RELOAD SPILL SET AT NO INSTRUCTION COST, MOVE A
 *   HI-REGISTER ASSIGNMENT NEXT TO A CALL THAT USES THE CONTENDED REGISTER AS AN
 *   OUTGOING ARGUMENT -- and prefer an assignment whose reload is inherited.
 * This reaches the case that section's own cure cannot: that cure needs an
 * out-of-range IWRAM whole-word store to put two reloads on one insn, and
 * Func_80a524c CONTAINS NO DATA STORE AT ALL.
 *
 * IT IS NOT "next to any 4-argument call".  Parking the trio before either of
 * the last two `_Func_801e7c0` calls measured 7 -- unchanged.  Those calls'
 * fourth arguments are small constants gcc materialises late, so r3 is not live
 * ACROSS the reload.  THE CONTENDED REGISTER MUST BE LIVE AT THE RELOAD, NOT
 * MERELY USED BY THE NEARBY CALL.
 *
 * ================== LOAD-BEARING, EACH REVERTED ALONE FROM THIS FILE ==================
 *   the trio at the top of the function ............... 7 of 134 when moved back
 *   `_MSG_ad4` as the CONST_INT 0xad4 ................. +6 (indices 20-26)
 *   `_MSG_b2c` as the CONST_INT 0xb2c ................. +6 (indices 32-38)
 *   both as CONST_INTs ................................ +12, additive
 *   `_MSG_182` as the CONST_INT 0x182 ................. 117 of 134 -- 0x182 IS
 *     shiftable (0xc1 << 1) so gcc builds it with mov+lsl instead of pooling it
 *     and the function unravels.  This is the pool tell message.sym exists for.
 *
 * ========================= MEASURED INERT AND MEASURED WORSE =========================
 * Inert from the PREVIOUS baseline (all still 7, each a single drop): `moved`
 * typed unsigned char / char / short / unsigned short / unsigned int / long;
 * `moved` declared first and last among the locals; `moved` read into a named
 * temp before `if (moved)`; `*keys` read into a named temp; `register int id`;
 * the gKeyPress masks in a named local; a shared `one = 1` feeding all three
 * `moved = 1`.  Added this batch: `(*keys & 0x40) != 0`; `moved = n - 1`;
 * `n - 1` in the Up block; `*(volatile int *)&gKeyPress`; declared-return-type
 * `void`->`int` on `_PlaySound`, `WaitFrames`, `Func_80a1a40`, `_CloseUIBox`;
 * `_GetItemInfo` as `void`; `_GetFlag` as `long`; the trio moved to before
 * either of the last two `_Func_801e7c0` calls.
 *
 * Worse: a shared `y = 0x30` local (90); a local for the 0x741 id (21);
 * `_UIDrawText` as `int` (9); the 0x40/0x80 masks in a named local (20);
 * `moved = n - 2` inside the `if` (13); `keys`/gKeyRepeat non-volatile (14);
 * `moved = 1` after `_PlaySound` in the Up block (10) and in both blocks (15);
 * `moved = 0` after the `% n` (13); `gKeyRepeat` read directly (20); `keys`
 * initialised at its declaration (128); `moved = 1` moved late WITHOUT `sel = 1`
 * (97, +4 bytes -- the inheritance from r6 is lost); keeping the `& 0x40` mask
 * live via `sel -= t40 >> 6` (84, +4 bytes); `Func_80a1ac0` as `int` (9);
 * `_Func_801e7c0` as `int` (13).
 *
 * NO FLAG REACHES IT and none is needed.  Recorded from the previous revision
 * because a flag inert alone can be load-bearing in a pair:
 *   -fno-rerun-cse-after-loop, -fno-regmove, -fno-strength-reduce,
 *   -fomit-frame-pointer, -fno-caller-saves                        7 (inert)
 *   -fno-expensive-optimizations 19 / -fno-schedule-insns2 37 / -fno-force-mem 20
 *   -fno-gcse 137 / -fno-gcse with -fno-expensive-optimizations 136
 * ALL FIGURES ARE PRODUCTION-FLAG FIGURES: batch.py reports `flags-adjust=[]`
 * for this .s, i.e. the plain GCC296_CFLAGS group with no Makefile substitution.
 */
extern char _MSG_182[], _MSG_ad4[], _MSG_b2c[];
extern volatile int gKeyRepeat;
extern volatile int gKeyPress;
extern int _CreateUIBox(int, int, int, int, int);
extern unsigned char *_GetItemInfo(int item);
extern void _Func_801e7c0(int id, int win, int x, int y);
extern void Func_80a1ac0(int x, int y);
extern void Func_80a1a40(int x, int y);
extern void _PlaySound(int id);
extern void WaitFrames(int n);
extern int _GetFlag(int id);
extern void _CloseUIBox(int win, int a);

int Func_80a524c(int id)
{
    volatile int *keys;
    int win;
    int sel;
    int moved;
    int n;

    n = 2;
    sel = 1;
    moved = 1;
    win = _CreateUIBox(0xd, 3, 0x11, 0xa, 2);
    id &= 0x1ff;
    _GetItemInfo(id);
    id += (int)_MSG_182;
    _Func_801e7c0(id, win, 0x18, 0);
    id = (int)_MSG_ad4;
    _Func_801e7c0(id, win, 0, 0x10);
    id++;
    _Func_801e7c0(id, win, 0, 0x18);
    id = (int)_MSG_b2c;
    _Func_801e7c0(id, win, 0x18, 0x28);
    id++;
    _Func_801e7c0(id, win, 0x18, 0x38);
    Func_80a1ac0(0x68, 0x56);
    while (!_GetFlag(0x150)) {
        if (moved) {
            moved = 0;
            sel = (sel + n) % n;
        }
        if (gKeyPress & 1) {
            _PlaySound(0x70);
            break;
        }
        if (gKeyPress & 2) {
            _PlaySound(0x71);
            sel = 1;
            break;
        }
        Func_80a1a40(0x68, sel * 16 + 0x46);
        keys = &gKeyRepeat;
        if (*keys & 0x40) {
            sel--;
            moved = 1;
            _PlaySound(0x6f);
        }
        if (*keys & 0x80) {
            sel++;
            moved = 1;
            _PlaySound(0x6f);
        }
        WaitFrames(1);
    }
    if (_GetFlag(0x150))
        sel = 1;
    _CloseUIBox(win, 1);
    return sel;
}

/* Func_80a90bc -- NON-MATCHING, 216 encodings of 303, size 696 against the ROM's
 * 692 (+4), 305 instructions against 303.
 *
 * Blocker class: BASIC-BLOCK LAYOUT of the refresh block, traced to two specific
 * places in the compiler.  Register allocation, frame size and BOTH literal pools
 * already match the ROM exactly.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_a1000/80a90bc.c \
 *     asm/rom_a1000/rom_a8604_a_a_c_c_c.s --func Func_80a90bc
 * The reference holds THREE functions (Func_80a8d34, Func_80a8f40, Func_80a90bc),
 * so a split is required to land this one.
 *
 * ================================================================
 * READ THIS BEFORE REUSING THE CANDIDATE: ITS BEST SPELLING IS A DIAGNOSTIC, NOT A
 * CREDIBLE ORIGINAL
 * ================================================================
 *
 * The 285 -> 216 step came from sharing the constant 1 between the
 * `*(unsigned char *)(x + 5) = 1` store and the `gKeyPress & 1` test, written here
 * as `mask = 1;`.  The ROM has `mov r7,#1 / strb r7,[r3,#5] / ... / and r3,r7`, and
 * that sharing flips the ENTIRE allocation to the ROM's while taking the frame from
 * 48 to 44.
 *
 * BUT `mask = 1;` IS ALMOST CERTAINLY NOT WHAT THE ORIGINAL SOURCE SAID.  gcc-2.96
 * will not unify two bare `const_int 1`s across basic blocks here -- cse's
 * extended-BB reach is broken by the `r==1 / r==0 / r==-1` diamonds, and gcse does
 * not hash bare constants -- so the ROM's source really does carry SOME variable at
 * that position, and the worker that found this did not identify what it naturally
 * is.  The number is real; the spelling is a placeholder that proves the allocation
 * is reachable.  Anyone finishing this function should look for the real value
 * first, because it is probably a named flag or mask with meaning.
 *
 * Sharing 0x100 instead does NOTHING (289, unchanged), so it is specifically the 1.
 *
 * ================================================================
 * THE LAYOUT DEFECT, WITH ITS MECHANISM
 * ================================================================
 *
 * ROM layout: [preheader b .La92c0][A handler .La917a][B handler .La9188]
 *             [body .La91a4][cond .La92ae][checks+refresh .La92c0][exit .La931c]
 * -- the two key handlers are hoisted ABOVE the body (backward `bne`), and the
 * checks FALL THROUGH into the refresh body, which then `b .La92ae`.
 *
 * Ours: [preheader b][A][B][refresh][b cond][body][cond][checks][b refresh] -- the
 * refresh body sits at the rotated loop's `newstart_label` instead of after the
 * checks, and the last check's branch is inverted so the exit is the fall-through.
 *
 * Traced in the compiler source:
 *   stmt.c:expand_end_loop rotates the outer for(;;) -- it moves
 *   start_label..last_test_insn to the end and emits `goto start_label` at entry,
 *   which is exactly what produces the ROM's `b .La92c0` entry and the
 *   checks-at-the-bottom placement.  THE SCAN STOPS AT THE INNER LOOP'S
 *   NOTE_INSN_LOOP_BEG, so the moved region is the two checks only and refresh
 *   stays at newstart_label.
 *
 *   flow.c:merge_blocks_move_successor_nojumps WOULD move refresh back down after
 *   the checks (refresh has one predecessor and no outgoing fallthru), but jump.c's
 *   "conditional jump jumping over an unconditional jump" (~line 437) FIRES FIRST:
 *   the checks' condjump targets end_label, which sits immediately after the
 *   `goto newstart_label`, so gcc inverts and deletes the jump and destroys the
 *   merge candidate.
 *
 * AND bbro IS NOT IN PLAY, which rules out the obvious suspect: flag_reorder_blocks
 * is 0 by default (toplev.c:443), is gated at toplev.c:3496, and -O2 does not set
 * it.  Verified in the image.
 *
 * ================================================================
 * WHAT ALREADY MATCHES EXACTLY -- reuse all of it
 * ================================================================
 *
 * BOTH LITERAL POOLS, contents and order.  The interior pool at 0x080a9134 holds
 * 0xfffffff0, iwram_3001f2c, 0x242, Func_80a19a0 in that order, preceded by the
 * `.short 0x0000` align pad, with the `b` around it -- byte-for-byte, and the rest
 * goes to the end pool.  THE HImode LEVER CONFIRMED IN REVERSE: the ROM WANTS the
 * early dump here, and `hv[i] = -16` (a HImode constant store) is what produces it.
 * No routing through an `int` is needed -- which is the third distinct case of this
 * rule in batches 280-281 and belongs with the note in
 * src/non_matching/ovl_7fb4a8/20092e0.c.
 *
 * FRAME: `sub sp, sp, #44` -- d[7] at sp+0x10..0x2b, `ret` spilled at sp+0xc,
 * `redraw` (r4, call-clobbered under -fcall-used-r4) spilled at sp+8 around every
 * call.
 *
 * REGISTER ALLOCATION, all of it: r4 redraw, r5 msg (0xb06 -> 0xb08) then
 * &gKeyRepeat, r6 -0x18 then dp, r7 the shared 1 then 0x100, r8 state, r9 pcur,
 * r10 full, r11 done.
 *
 * Loop shapes (`for (i = 3; i >= 0; i--)` -> sub/strh/sub/cmp/bge; `*q++` ->
 * `ldmia r1!,{r3}`), the __modsi3 call, and all 38 relocation symbols and their
 * order in the pools.
 *
 * TWO CONTENT FIXES, each measured in isolation:
 *
 * 1. `int *dp = d;` FOR THE ARGUMENT PASSES AND THE dp[4]/dp[5]/dp[6] READS, BUT
 *    `&d[4]` / `&d[2]` IN ARRAY FORM FOR Func_80a1fd4's POINTER ARGS.  Without it
 *    gcc keeps TWO pseudos holding sp+0x10 (r6 for the argument, r10 for the field
 *    reads) and emits `mov r0,sl / ldr r1,[r0,#20]` where the ROM has
 *    `ldr r1,[r6,#0x14]` -- the redundant-copy class of
 *    src/non_matching/rom_b5000/80ba2c0.c.  Using `dp` for the & args too is WORSE:
 *    gcc hoists dp+16 as a loop invariant into r8 where the ROM rematerialises
 *    `add r3, sp, #0x20`.  Measured: array-only 295 insns, all-dp 297, mixed 293.
 * 2. The shared 1 (above), with its caveat.
 *
 * ================================================================
 * MEASURED -- 16 structural candidates and a 9-flag sweep
 * ================================================================
 *   c1  natural nested, goto refresh into loop      289 / 294
 *   c2,c3  pure gotos, handlers hand-hoisted        290 / 289 -- gcc RELOCATED the
 *                                                   hand-hoisted handlers inline
 *   c4  for(;;) + leading breaks + inner while      285 / 295 -- handlers hoist
 *   c5  goto chk entry + for(;;) + inner while      ~285 / 295
 *   c6,c7  pure-goto outer + inner while            285 / 295
 *   c8  inner while first, checks+refresh last      292 / 295 -- checks+refresh
 *                                                   CONTIGUOUS (correct) but the
 *                                                   entry branch targets the inner
 *                                                   cond, not the checks
 *   c12 goto-based inner loop                       216 / 305, frame grew to 48
 *   c13,c14  goto chk entry + inner while           227 / 305
 *   c16 c4 shape + the two content fixes            216 / 305  BEST (this file)
 *
 * FLAG SWEEP, baseline 287, ALL worse or neutral and NONE touched the layout:
 * -fno-thread-jumps 287, -fno-cse-follow-jumps 287, -fno-gcse 286,
 * -fno-rerun-cse-after-loop 288, -fno-strength-reduce 273, -fno-schedule-insns2
 * 282, -fno-expensive-optimizations 252, -fno-regmove 287.  -fno-if-conversion is
 * not accepted by this gcc.  No flag row is requested.
 *
 * REMAINING CONTENT RESIDUE BEYOND THE LAYOUT, about 6 instructions:
 *   1. The move block at ref 0x080a9280-0x080a92ac.  The ROM keeps `state` copied
 *      into a low register and uses register-offset addressing for THREE SEPARATE
 *      `ldrh r3,[r1,r2]` of state[0x208 + cur*2], reloading after each intervening
 *      store.  We compute the full address and gcc then CSEs the load, emitting one
 *      `ldrh` reused for both the `str [r1,#8]` and the `strb`.  Needs a spelling
 *      that keeps the base/index split so the stores invalidate.
 *   2. `strb r7,[r3,#5]` / `movs r7,#1` scheduling, plus two sched2 swaps.  Not
 *      worth chasing before the layout.
 *   3. B handler at 0x080a9188: the ROM materialises `done = 1` into r1 and moves
 *      it to r11 BEFORE storing `ret = -1`; ours emits the -1 store first, although
 *      source order is already `done = 1; ret = -1;`.
 *
 * ANTI-TELL CONFIRMED BENIGN: `mov r0,#0x1c / ldrsb r0,[r2,r0]` is just
 * `*(signed char *)(state + 0x1c)`.
 *
 * NO SYMBOL TELLS AND NONE WARRANTED.  Every constant reproduces as a plain literal
 * (0x150, 0x242, 0x219, 0x21a, 0xb06, 0x1e, 0x37, 0xf5, 0xc80), and
 * 0x10c/0x1c8/0x208/0x218/0x260 are all emitted by gcc's own thumb_shiftable_const
 * path in the same mov/lsl form as the ROM.
 *
 * No per-file Makefile override applies to this stem -- the only rom_a8604* rule is
 * line 611 for rom_a8604_a_a_c_b.o (-fno-strength-reduce, for LoadMoveRangeIcons),
 * and objcmp picks the default group up correctly.
 *
 * NEXT: the layout, and specifically whether any source form makes the checks' last
 * condjump target something OTHER than the label sitting after the
 * `goto newstart_label` -- because that is what lets jump.c delete the jump and
 * destroy flow.c's merge candidate.  c8 got checks+refresh contiguous and lost the
 * entry branch, so the two halves have each been reached separately but not
 * together.  Find the real value behind `mask = 1;` first.
 */
extern unsigned char *iwram_3001f2c;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;

extern void LoadMoveRangeIcons(void);
extern void _Func_8016498(unsigned int win);
extern void _Func_80164ac(unsigned int win);
extern int Func_80a10d0(void *slot, int a, int b, int c, int d, int e);
extern void StopTask(void *task);
extern void StartTask(void *task, int priority);
extern void Func_80a19a0(void);
extern int Func_80a33d4(void *state, unsigned int win);
extern void _Func_801e7c0(int msg, unsigned int win, int x, int y);
extern void _PlaySound(int id);
extern int Func_80a1fd4(int swap, int total, int cols, int *col, int *row);
extern int Func_80a1a40(int a, int b);
extern void Func_80a8f40(unsigned int win, int a, int *d);
extern void Func_80a8d34(unsigned int win, int a, int *d);
extern void WaitFrames(int n);
extern void Func_80a1804(void *state, int h);
extern void Func_80a9cbc(void);
extern int _GetUnit(int id);
extern int Func_80a68ec(int unit, void *p, int c);
extern int Func_80a8b8c(int *dest, int cursor);
extern void Func_80a9374(unsigned int win, int idx);
extern void Func_80a345c(void);
extern int _GetFlag(int id);

int Func_80a90bc(void)
{
    unsigned char *state;
    unsigned char *pcur;
    unsigned char *node;
    void **q;
    short *hv;
    int d[7];
    int *dp;
    int mask;
    int ret;
    int done;
    int redraw;
    int full;
    int i;
    int r;
    int cur;
    int n;
    int h;
    int msg;

    state = iwram_3001f2c;
    ret = 0;
    done = 0;
    LoadMoveRangeIcons();
    _Func_8016498(*(unsigned int *)(state + 0x10c));
    Func_80a10d0(state + 0x2c, 0, 0, 0x1e, 5, 2);
    hv = (short *)(state + 0x23c);
    for (i = 3; i >= 0; i--)
        hv[i] = -16;
    q = (void **)(state + 0x48);
    for (i = 0x1f; i >= 0; i--) {
        node = *q++;
        if (node != 0)
            node[0xf] = 0xf5;
    }
    StopTask(Func_80a19a0);
    Func_80a33d4(state, *(unsigned int *)(state + 0x10c));
    msg = 0xb06;
    _Func_801e7c0(msg, *(unsigned int *)(state + 0x24), 0x50, -0x18);
    msg += 2;
    _Func_801e7c0(msg, *(unsigned int *)(state + 0x24), 0, -0x18);
    pcur = state + 0x21a;
    for (;;) {
        if (done)
            break;
        if (_GetFlag(0x150))
            break;
        Func_80a9cbc();
        _Func_8016498(*(unsigned int *)(state + 0x24));
        state[0x218] = Func_80a68ec(_GetUnit(*pcur), state + 0x1c8, 0);
        WaitFrames(1);
        dp = d;
        Func_80a8b8c(dp, 0);
        Func_80a9374(*(unsigned int *)(state + 0x24), *pcur);
        redraw = 1;
        full = 1;
        while (_GetFlag(0x150) == 0) {
            if (redraw) {
                redraw = 0;
                if (full) {
                    full = 0;
                    Func_80a8f40(*(unsigned int *)(state + 0x24), 0, dp);
                }
                Func_80a8d34(*(unsigned int *)(state + 0x24), 0, dp);
                WaitFrames(1);
            }
            WaitFrames(1);
            r = Func_80a1fd4(0, dp[5], 5, &d[4], &d[2]);
            mask = 1;
            *(unsigned char *)(*(unsigned int *)(state + 0x14) + 5) = mask;
            Func_80a1a40(0x37, (dp[4] << 4) + 0x3c);
            if (r == 1) {
                full = 1;
                redraw = 1;
            }
            if (r == 0)
                redraw = 1;
            if (r == -1)
                redraw = 0;
            if (gKeyPress & mask) {
                _PlaySound(0x70);
                ret = 1;
                done = 1;
                break;
            }
            if (gKeyPress & 2) {
                _PlaySound(0x71);
                done = 1;
                ret = -1;
                StartTask(Func_80a19a0, 0xc80);
                break;
            }
            if ((gKeyRepeat & 0x100) || (gKeyRepeat & 0x200)) {
                _PlaySound(0x6f);
                cur = *(signed char *)(state + 0x1c);
                h = *(unsigned short *)(state + 0x208 + cur * 2);
                state[h + 0x260] = dp[6];
                if (gKeyRepeat & 0x100)
                    cur++;
                else
                    cur--;
                n = state[0x219];
                cur = (cur + n) % n;
                *(unsigned int *)(state + 8) = *(unsigned short *)(state + 0x208 + cur * 2);
                *pcur = *(unsigned short *)(state + 0x208 + cur * 2);
                *(signed char *)(state + 0x1c) = cur;
                Func_80a1804(state, *(unsigned short *)(state + 0x208 + cur * 2));
                break;
            }
        }
    }
    _Func_80164ac(*(unsigned int *)(state + 0x2c));
    _Func_8016498(*(unsigned int *)(state + 0x2c));
    Func_80a345c();
    _Func_80164ac(*(unsigned int *)(state + 0x10c));
    _Func_8016498(*(unsigned int *)(state + 0x24));
    return ret;
}

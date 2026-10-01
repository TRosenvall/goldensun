/* Func_80a5388 -- NON-MATCHING: 159 encodings of 184 differ (objcmp).
 *
 * 159 IS NOT A DISTANCE: ref 184 instructions / ours 186, ref 428 bytes / ours 432.  This
 * body is TWO INSTRUCTIONS LONG and everything after the divergence is positional.  The
 * companion candidate (scratch, `goto cancel` into the loop body) is 149 of 184 with ref
 * 184 / ours 184 and ref 428 / ours 428 -- EQUAL SIZE AND COUNT -- but it relocates one
 * block, so neither number is a clean distance.  What is solid is the region-aligned
 * measure: 48 instructions in disagreeing regions of 188 for this body, 59 for the other.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a5388.c asm/rom_a1000/rom_a4f08_c_c.s --func Func_80a5388
 *
 * NOTE: rom_a4f08.s CARRIES A DATA SECTION and holds FOUR functions
 * (Func_80a4f08, Func_80a51d0, Func_80a524c, Func_80a5388), so landing this one needs a
 * TEXT/DATA SPLIT.  tools/datacheck.py says:
 *     data sections : .rodata
 *     functions     : Func_80a4f08, Func_80a51d0, Func_80a524c, Func_80a5388
 *     -> converting a function here needs a TEXT/DATA SPLIT; the data must keep its own
 *        object.
 * The section is a single `.Laf08c: .incrom 0xaf08c, 0xaf20c` (0x180 bytes) at the very
 * END of the file, after all four functions, with NO `.global` -- and it is referenced by
 * `ldr r0, =.Laf08c` from Func_80a4f08 ONLY (asm/rom_a1000/rom_a4f08.s:138).  So
 * Func_80a5388 itself needs NOTHING exported; the label becomes an export only when
 * Func_80a4f08 is the one that leaves the file.  (Batch 290's rule still applies in the
 * other direction: datacheck's EXPORTS line cannot see a label that is not already
 * `.global`, so read the definitions and the references, as done here.)
 *
 * THREE THINGS LANDED, 181 -> the current body:
 *
 * 1. THE MODULO'S DIVISOR IS A VARIABLE, AND THIS IS CONFIRMED THREE TIMES IN THIS BANK.
 *    The ROM calls `__modsi3` with `mov r1,#2`.  A literal `% 2` is expanded INLINE
 *    (lsr/add/asr/lsl/sub, five instructions) -- the libcall only appears if the divisor is
 *    a variable at expand, which cprop then folds to 2.  `int n; n = 2; sel = (sel + n) % n;`
 *    gives the ROM's `add r0,#2 / mov r1,#2 / bl __modsi3` exactly, and it FIXED THE
 *    INSTRUCTION COUNT (188 lines against 188) and put `sel` into r8.
 *    THE CROSS-CHECK: the same `(x + n) % n` appears in Func_80a4f08
 *    (`ldr r0,[sp,#0x18] / ldr r1,[sp,#0x18] / add r0,r8 / bl __modsi3`, n = parameter 1)
 *    and in Func_80a60d4 (same shape, n = `*(signed char *)(state + 0x1e)` spilled to
 *    sp+0x18).  In those two the divisor STAYS a load because it is genuinely variable; here
 *    it is a local constant so cprop turns it into `mov r1,#2` while the libcall remains.
 *    A `mov rN,#K` DIVISOR FEEDING __modsi3 IS THEREFORE NOT EVIDENCE OF A CONSTANT IN THE
 *    SOURCE -- it is evidence of a VARIABLE that cprop resolved.
 *
 * 2. THE INDIRECT-CALL TYPEDEF AGAIN, AND AGAIN THE CAST FORM FAILS.
 *    `CopyFn copy; copy = Func_8001af8; copy(...)` is required for
 *    `ldr r3,=Func_8001af8 / bl _call_via_r3`; `((CopyFn)Func_8001af8)(...)` folds to a
 *    direct `bl Func_8001af8` and loses two instructions per site.
 *
 * 3. STACK SLOT ORDER IS THE DECLARATION-ORDER LEVER AND ITS DIRECTION HELD.
 *    The ROM has `unit` at sp+4 and `moved` at sp+8.  Declaring `moved` BEFORE `unit` puts
 *    unit in the lower slot -- "later declaration takes the lower slot", confirmed again.
 *
 * 4. TWO MESSAGE IDS ARE ONE INCREMENTED VARIABLE (the 80a7d68 lever, reused):
 *    `id = 0xb2c; _Func_801e7c0(id,...); id++; _Func_801e7c0(id,...);` gives the ROM's
 *    single `ldr r5,=0xb2c` plus `add r5,#1`.
 *
 * 5. `sel` AND THE RETURN VALUE ARE THE SAME VARIABLE.  r8 is written with 0 at entry,
 *    incremented and decremented by the cursor keys, taken from __modsi3, set to 1 on the
 *    two cancel paths, tested by `cmp r3,#1` for the restore, and returned by `mov r0,r8`.
 *    One variable.  `pop {r1}` in the epilogue is the `int` return.
 *
 * BLOCKER: THE SCRATCH REGISTER FOR A CONSTANT INTO A HIGH REGISTER.  The ROM uses r3 for
 * ALL EIGHT of them -- `mov r3,#0 / mov r8,r3`, `mov r3,#1 / str r3,[sp,#8]`,
 * `mov r3,#1 / neg r3,r3 / add r8,r3`, `mov r3,#1 / add r8,r3 / str r3,[sp,#8]`,
 * `mov r3,#0 / str r3,[sp,#8]`, both `mov r3,#1 / mov r8,r3`, and `mov r3,r8 / cmp r3,#1`.
 * This C alternates r2 and r3.  It is NOT a reload register: the .18.greg dump already
 * shows `(set (reg/v:SI 8 r8) (reg:SI 2 r2))` with the REG_EQUAL const on it, so
 * local-alloc's find_free_reg picked r2 -- r3 was not free over that interval.
 *
 * AND THAT ONE REGISTER COSTS THE TWO INSTRUCTIONS.  The early-out (`_EquipItem` returned
 * -2 or -1) and the B-button path both do `sel = 1` and then fall into the shared tail; the
 * ROM merges them by cross-jumping, leaving `b .La54c6`.  Here the early-out emits
 * `mov r2,#1 / mov r8,r2` and the B path emits `mov r3,#1 / mov r8,r3` -- DIFFERENT
 * ENCODINGS, so find_cross_jump cannot merge them and the copy stays.  The same register
 * split also lets sched2 interleave `sel = 0` and `moved = 1` in the prologue, where the
 * ROM's shared r3 is a WAW dependence that pins the order
 * (`mov r3,#0 / mov r8,r3 / mov r3,#1 / str r3,[sp,#8]`).  ONE REGISTER CHOICE EXPLAINS
 * BOTH THE LENGTH AND THE PROLOGUE.
 *
 * CROSS-EVIDENCE FROM ITS FILE-MATE, WHICH SETTLES THE DIAGNOSIS.  Func_80a4f08 (landed
 * this batch) has the SAME `i--` under `gKeyRepeat & 0x20` with `i` in r8, and the ROM
 * there emits `sub r2, #0x21` -- reload_cse_move2add chaining the -1 off the `mov r2,#0x20`
 * the mask just left in r2 -- where THIS function's ROM emits `mov r3,#1 / neg r3,r3`.
 * ONE SOURCE IDIOM, TWO MATERIALISATIONS, DIFFERING ONLY IN WHICH REGISTER RECEIVED THE -1.
 * So the -1 is not a source question at all, and neither is the prologue order: the whole
 * residue is which low register local-alloc hands the constant.
 *
 * AND THE LEVER THAT WORKED ON Func_80a4f08 DOES NOT REACH THIS.  There the residue was
 * global_alloc (an EXACT priority tie at 14 refs each between `win` and `i`, broken by
 * live_length) and moving one assignment above the `state` load flipped it, 68 -> 19.  Here
 * the competing values already have the ROM's registers -- `sel` is r8, `win` is r7, `unit`
 * and `moved` are in the ROM's stack slots -- and the thing left is find_free_reg inside a
 * block.  A different pass, and statement order does not reach it.
 *
 * MEASURED NEGATIVES (all at 159, all inert):
 *  - EIGHT permutations of `sel = 0` / `n = 2` / `moved = 1` against the `state` load,
 *    including every order with the load first and with it last (one reads 155, the rest
 *    159; none changes the register);
 *  - `moved` declared first; `sel` declared last;
 *  - dropping the `r` local and testing `_EquipItem(...) + 2 <= 1U` inline.
 * The `goto cancel:` form INTO the loop body fixes the length (184/184, 428/428) but moves
 * the A-button `_PlaySound(0xaf)` block out of the position the ROM gives it (immediately
 * after the early-out branch, before the else body) -- visible as the only out-of-order
 * relocation, `_PlaySound` at 0x70 in the ROM against 0x126 here.  The two goals are
 * currently exclusive; the register is what would reconcile them.
 *
 * gKeyPress and gKeyRepeat MUST BE `volatile`: the ROM reloads gKeyPress twice with no call
 * between the two `ldr r3,[r1]`, which cse would collapse otherwise.
 */
typedef int (*CopyFn)(void *dst, void *src, int len);

extern unsigned char *iwram_3001f2c;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;
extern void *_GetUnit(int id);
extern void Func_80a3ef0(int chr, int slot, int action, int target);
extern void *Func_8004938(int size);
extern int Func_8001af8(void *dst, void *src, int len);
extern int _EquipItem(int id, int item);
extern void _Func_801e7c0(int msg, int win, int x, int y);
extern void _Func_80164d4(int win, int a, int b, int c, int d);
extern void Func_80a1ac0(int x, int y);
extern void Func_80a1a40(int x, int y);
extern int _GetFlag(int flag);
extern void _PlaySound(int id);
extern void WaitFrames(int n);
extern void free(void *p);
extern void _CalcStats(int id);
extern void _Func_8078bf0(int id);

int Func_80a5388(void)
{
    unsigned char *state;
    CopyFn copy;
    int moved;
    void *unit;
    void *buf;
    int win;
    int sel;
    int r;
    int id;
    int n;

    sel = 0;
    n = 2;
    moved = 1;
    state = iwram_3001f2c;
    unit = _GetUnit(state[0x21b]);
    Func_80a3ef0(state[0x21b], *(unsigned short *)(state + 0x176), 0,
                 state[0x21b]);
    buf = Func_8004938(0x14c);
    copy = Func_8001af8;
    copy(buf, unit, 0x14c);
    win = *(int *)(state + 0x10c);
    r = _EquipItem(state[0x21b], *(unsigned short *)(state + 0x176));
    if (r == -2 || r == -1) {
        sel = 1;
    } else {
        id = 0xb2c;
        _Func_801e7c0(id, win, 0x18, 0x18);
        id++;
        _Func_801e7c0(id, win, 0x48, 0x18);
        _Func_80164d4(win, 0x10, 0x10, 0x60, 0x18);
        _Func_801e7c0(0xad6, win, 0, 0x10);
        Func_80a1ac0(0x6e, 0x20);
        for (;;) {
            if (_GetFlag(0x150))
                break;
            if (moved != 0) {
                moved = 0;
                sel = (sel + n) % n;
            }
            if (gKeyPress & 1) {
                _PlaySound(0xaf);
                break;
            }
            if (gKeyPress & 2) {
                _PlaySound(0x71);
                sel = 1;
                break;
            }
            Func_80a1a40(sel * 48 + 0x6e, 0x20);
            if (gKeyRepeat & 0x20) {
                sel--;
                moved = 1;
                _PlaySound(0x6f);
            }
            if (gKeyRepeat & 0x10) {
                sel++;
                moved = 1;
                _PlaySound(0x6f);
            }
            WaitFrames(1);
        }
    }
    if (_GetFlag(0x150))
        sel = 1;
    if (sel == 1)
    {
        copy = Func_8001af8;
        copy(unit, buf, 0x14c);
    }
    free(buf);
    _CalcStats(state[0x21b]);
    _Func_8078bf0(state[0x21b]);
    return sel;
}

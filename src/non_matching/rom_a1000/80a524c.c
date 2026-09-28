/* Func_80a524c -- ConfirmItemAction -- NON-MATCHING, 7 encodings of 134 differ
 * (objcmp: ref 134 / ours 134, same size).  THREE of the 7 are pool words that are
 * symbol placeholders (_MSG_182 exists in message.sym; _MSG_ad4 and _MSG_b2c do NOT
 * yet) -- with those assembled as .equ the residue is 4 instructions.
 *
 * Verify with:
 *   python3 tools/objcmp.py /tmp/claude-0/-home-user-goldensun/ad08b1ee-c1a0-56c8-b3c2-c0ff6481844c/scratchpad/b287/V/Func_80a524c.park.c \
 *     asm/rom_a1000/rom_a4f08_c.s --func Func_80a524c
 *
 * FRESH TARGET (batch 287).  asm/rom_a1000/rom_a4f08.s holds 4 functions + .rodata.
 *
 * LEVERS THAT GOT HERE (each measured):
 *  - The message ids reuse ONE variable (`id`): id &= 0x1ff; id += _MSG_182; then
 *    id = 0xad4; f(id); id++; f(id) -- the ROM keeps every id in r5 and bumps it with
 *    `add r5,#1` scheduled before the bl.  f(id++) and f(id + 1) are both worse.
 *  - 0xad4 / 0xb2c must be SYMBOLS.  As CONST_INTs sched2 hoists the `ldr r5,=` above
 *    the previous _Func_801e7c0 call (a const set of a call-saved hard reg has no dep
 *    on the call after reload) -- the same mechanism that makes Func_80a9598 need
 *    _MSG_af7.  19 differing with literals.  _MSG_182 (existing) likewise, for the pool.
 *  - `(sel + n) % n` with `n = 2` in a variable: a literal `% 2` is expanded inline;
 *    the ROM calls __modsi3 with r1 = 2.
 *  - void (not int) _Func_801e7c0: int reorders `mov r0,r5` / `mov r1,r7`.
 *  - THE sel/keys r5<->r6 SWAP WAS AN EXACT allocno_compare TIE: sel
 *    floor_log2(18)*18/132 == keys 2*6/22 (both 0.54545), broken by allocno number, and
 *    sel's pseudo was older.  Declaring `volatile int *keys` FIRST (lowest pseudo number)
 *    and assigning it in the loop wins the tie: 20 -> 7.
 *
 * BLOCKER (4 instructions): reload-register ROUND ROBIN.  The ROM's four reloads of
 * `moved` (r8) -- moved=1 in the Up block, moved=1 in the Down block, `if (moved)`, and
 * moved=0 -- use r2, r3, r2, r3; ours use r3 each time.  allocate_reload_reg
 * (reload1.c) rotates through spill_regs[] from last_spill_reg, so the ROM had TWO
 * spill regs {r2,r3} (n_spills = 2) and ours only r3: somewhere in the ROM's function
 * find_reg had to pick r2, i.e. r3 was busy at some reload.  Not found which insn.
 *
 * INERT: moved as u8/s8/short/u16/unsigned; moved/sel declaration order; `moved != 0`;
 * a `press` pointer for gKeyPress; `moved = sel = 1`; for(;;)+break instead of while;
 * sel = sel -/+ 1; unsigned key globals; `r = sel` result temp; (sel << 4);
 * _CreateUIBox unprototyped or varargs.  goto into the final `sel = 1`: 73.
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
    n = 2;
    sel = 1;
    moved = 1;
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

/* Func_80a8d34 -- DrawEquipDetail -- NON-MATCHING: 172 encodings of 239 differ (objcmp).
 * Size and instruction count MATCH (ref 524 bytes / 239 encodings, ours 524 / 239).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a8d34.c \
 *     asm/rom_a1000/rom_a8604_a_a_c_c_c.s --func Func_80a8d34
 *
 * FRESH TARGET (batch 290). Shares its .s with Func_80a8f40 (parked at 6 of 167
 * this batch) and Func_80a90bc; datacheck.py clean.
 *
 * VERIFICATION SHIM, scratch only: `__asm__(".equ _MSG_53a, 0x53a")`. _MSG_53a is
 * ALREADY ADMITTED in message.sym:262 and this function uses it in exactly the
 * base-plus-arithmetic shape that entry exists for (`(id & 0x3fff) + 0x53a` into
 * _Func_801e7c0). A landed file must NOT carry the shim.
 *
 * FOUR THINGS ARE ESTABLISHED, and they took 227 -> 233 instructions (exact
 * length) and 207 -> 172 differing:
 *
 *  1. THE OFFSET IS RECOMPUTED, NOT KEPT. The ROM computes `d[6]*2 + 0xe4*2`
 *     TWICE -- once for the `!= 0` test (shared with the message id) and again
 *     after the _Func_801e7c0 call for the _GetMoveInfo lookup. That is six
 *     instructions the first candidate did not have. Assigning the offset local a
 *     SECOND time after the call is what produces it: `d[6]` is memory, the call
 *     kills it, so gcc must reload and re-shift.
 *  2. BUT THE OFFSET MUST STAY A VARIABLE, because the reg+reg addressing
 *     (`ldrh r2,[state,ofs]`) only survives when the offset is a live pseudo.
 *     Writing the whole expression inline makes gcc add state in FIRST and
 *     collapse the address to one register (`add r3,state / add r3,#0x1c8 /
 *     ldrh [r3]`). Confirmed inert across four spellings --
 *     `state + d[6]*2 + 0xe4*2`, `d[6]*2 + 0xe4*2 + (int)state`,
 *     `(d[6] + 0xe4)*2 + (int)state`, and `((unsigned short *)state)[d[6] + 0xe4]`
 *     -- all four fold to the SAME instruction stream, so fold normalises the
 *     operand order here and the member-array lever does NOT apply.
 *  3. THE STATE POINTER AND THE LOOP POINTER ARE ONE VARIABLE. The ROM's r8 holds
 *     iwram_3001f2c for the whole first half and is then OVERWRITTEN with the
 *     walking halfword pointer (`mov r8, r1`). Written as two locals, the state
 *     pointer lands in r6 (a low callee-saved register) and every
 *     `mov rX, r8` copy the ROM spends disappears: 12 bytes short. Reassigning
 *     `state` to become the pointer puts it back in r8 and recovers them.
 *  4. THE ITEM-KIND TEST IS `!= 4` WITH THE ARMS IN ROM ORDER. `cmp r3,#4 / beq`
 *     jumps AWAY to the kind==4 arm, so the fall-through is the normal path; the
 *     `== 4` spelling inverts the branch and stops gcc reusing the already-loaded
 *     f02 byte as the _Func_8019000 argument (`mov r1,r3 / add r1,#1` becomes a
 *     second `ldrb`).
 *
 * ALSO CONFIRMED: the flag accumulator is a short-circuit `||`
 * (`if (info->f0c != 0 || (info->f01 & 0x40)) k = 2;`) -- the ROM's stray
 * `b .La8dd6` after the `beq` is the second arm of the short circuit, and the
 * f01 byte is loaded separately on each path because the later `& 0x80` test
 * needs it live on both. The nested-if spelling gives the same code.
 *
 * BLOCKER -- A CALLEE-SAVED ROLE ROTATION, and the two halves of it are coupled.
 * The ROM's map is r5 info/0x3fff, r6 col, r7 win, r8 state-then-p, r9 the
 * constant 1, r10 the flag mask k AND the loop counter i, r11 d.
 *   * With k and i as SEPARATE locals (file a8d34_v6.c in scratch) state lands in
 *     r8 correctly but k gets r2, a LOW register, and the ROM's seven
 *     `mov rX, r10` / `mov r10, rX` copies vanish: 226 of the ROM's 233
 *     instructions, 200 of 239 differing.
 *   * With k AND i AS ONE VARIABLE (this file) the length is exact at 233 and
 *     239 encodings, but state rotates from r8 to r10 and the reload registers
 *     rotate with it: 172 of 239.
 * So k must be in a HIGH register while state stays in r8, and k does not cross a
 * call on any path, which means global_alloc gave it r10 only because it
 * CONFLICTS with all of r0-r3 somewhere in its range. Nothing in the source
 * reaches that: `allocno_compare` tiebreaks on allocno number only AFTER the
 * priority compare, so per docs/elevation.md the next attempt must change
 * REFERENCE COUNTS or LIVE LENGTHS -- for instance a shape in which the flag mask
 * is read once more, or one in which r1 and r2 carry a value across the
 * _Func_80164d4 call.
 *
 * NOT YET TRIED: declaring _Func_80164d4 / Func_80a2268 / _Func_8019000 `int`;
 * a third arm ordering for the 0xb13/0xb14/0xb15 chain (the ROM cross-jumps the
 * 0xb15 and 0xb14 arms onto one call tail and keeps 0xb13 separate, which this
 * candidate already reproduces).
 */
__asm__(".equ _MSG_53a, 0x53a");

struct MoveInfo {
    unsigned char pad_00;
    unsigned char f01;
    unsigned char f02;
    unsigned char pad_03[0x0c - 0x03];
    unsigned char f0c;
};

extern unsigned char *iwram_3001f2c;
extern struct MoveInfo *_GetMoveInfo(int id);
extern void WaitFrames(int n);
extern void _Func_8016498(unsigned int win);
extern void _Func_80164d4(unsigned int win, int x, int y, int w, int h);
extern void _Func_801e7c0(int id, unsigned int win, int x, int y);
extern void _Func_8019000(unsigned int win, int tile, int col, int row, int pal);
extern void Func_80a2268(unsigned int win, int a, int b, int c, int d, int e);
extern int _MSG_53a;

int Func_80a8d34(unsigned int win, int a1, int *d)
{
    unsigned char *state;
    struct MoveInfo *info;
    int ofs;
    int i;
    int col;

    state = iwram_3001f2c;
    d[6] = d[2] * 5 + d[4];
    _Func_8016498(*(unsigned int *)(state + 0x2c));
    WaitFrames(1);
    ofs = d[6] * 2 + 0xe4 * 2;
    if (*(unsigned short *)(state + ofs) != 0) {
        _Func_801e7c0((*(unsigned short *)(state + ofs) & 0x3fff) + (int)&_MSG_53a,
                      *(unsigned int *)(state + 0x2c), 0, 0);
        ofs = d[6] * 2 + 0xe4 * 2;
        info = _GetMoveInfo(*(unsigned short *)(state + ofs) & 0x3fff);
        _Func_80164d4(win, 0, 0x60, 0xe0, 0x68);
        i = 0;
        if (info->f0c != 0 || (info->f01 & 0x40))
            i = 2;
        if (info->f01 & 0x80)
            i |= 1;
        if (i == 3)
            _Func_801e7c0(0xb15, win, 0, 0x60);
        else if (i == 2)
            _Func_801e7c0(0xb14, win, 0, 0x60);
        else if (i == 1)
            _Func_801e7c0(0xb13, win, 0, 0x60);
    }
    state = state + d[2] * 5 * 2 + 0xe4 * 2;
    col = 2;
    for (i = 0; i <= 4; i++) {
        if (i == d[4]) {
            info = _GetMoveInfo(*(unsigned short *)state & 0x3fff);
            if (info->f02 != 4) {
                _Func_8019000(win, info->f02 + 1, 0x18, col, 0);
                Func_80a2268(win, 9, col, 0xf, 1, 0xe);
                Func_80a2268(win, 0x19, col, 3, 1, 0xe);
            } else {
                Func_80a2268(win, 9, col, 0x13, 1, 0xe);
            }
        } else {
            info = _GetMoveInfo(*(unsigned short *)state & 0x3fff);
            if (info->f02 != 4) {
                _Func_8019000(win, info->f02 + 1, 0x18, col, 4);
                Func_80a2268(win, 9, col, 0xf, 1, 0xf);
                Func_80a2268(win, 0x19, col, 3, 1, 0xf);
            } else {
                Func_80a2268(win, 9, col, 0x13, 1, 0xf);
            }
        }
        col += 2;
        state += 2;
    }
    WaitFrames(1);
    return 1;
}

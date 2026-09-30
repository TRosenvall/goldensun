/* Func_80a6ccc  --  0x080a6ccc  --  RunItemPicker
 *
 * NON-MATCHING, 736 of 774 encodings differ  (objcmp at production flags).
 *
 * SIZE  NOT exact: ref 1680 bytes, ours 1660  (20 short).
 * COUNT NOT exact: ref 774 instructions, ours 764  (10 short).
 * So the objcmp count above is SATURATED and must not be read as a distance --
 * first difference is at index 10, in the prologue, and every later index is
 * shifted. The figure that ranks candidates here is aligncmp:
 *
 *     aligncmp  540 aligned-equal  (69.8% of ref)  282 differing in 121 hunks
 *
 * RELOCATION SEQUENCE IS EXACT, and that is the strongest thing this candidate
 * has. ABS32 by symbol -- ref {iwram_3001f2c 1, gKeyPress 1, gKeyHeld 5,
 * gKeyRepeat 1}; ours the same, plus _CONST_2. The extra is CORRECT, not a
 * defect: see the _CONST_2 note below. All 52 THM_CALL targets appear in the
 * reference's order.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a6ccc.c \
 *     asm/rom_a1000/rom_a5534_c_c_c_a_c_c_c.s --func Func_80a6ccc
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_a1000/80a6ccc.c \
 *     asm/rom_a1000/rom_a5534_c_c_c_a_c_c_c.s Func_80a6ccc
 *
 * SPLIT SHAPE: NONE NEEDED. asm/rom_a1000/rom_a5534_c_c_c_a_c_c_c.s holds this
 * one function and no data section, so no split_s.py run is involved.
 *
 * SHIMS: none. tools/shimcount.py reports nothing, no pins and no flag rows are
 * used, and no fakematch.txt entry is due.
 *
 * ========================================================================
 * THE FRAME, WHICH WAS THE WHOLE JOB, AND IT IS EXACT
 * ========================================================================
 * `sub sp, #0x64` = 100 bytes, and it is now fully accounted for -- our frame
 * size matches from the first candidate onward. Reading the spill-slot map
 * DESCENDING gave the declaration list directly, exactly as the method claims:
 *
 *     sp+0x00,+0x04  outgoing args 5 and 6 (Func_80a10d0, Func_80a1fd4)
 *     sp+0x08        reload's slot for the `sp+0x34` address pseudo -- 60
 *                    accesses, the `str r4,[sp,#8] / bl / ldr r4,[sp,#8]`
 *                    idiom at all 46 calls, because -fcall-used-r4
 *     sp+0x0c        index*2        } CSE temps, NOT declarations -- see below
 *     sp+0x10        state + 2      }
 *     sp+0x14        index*4        }
 *     sp+0x18        helpShown      \
 *     sp+0x1c        prevIdx         |
 *     sp+0x20        unit            |  the six declared scalars, in
 *     sp+0x24        done            |  declaration order top-down
 *     sp+0x28        needsRedraw     |
 *     sp+0x2c        ret            /
 *     sp+0x30        the parameter `index` (lowest pseudo, highest spill slot)
 *     sp+0x34..0x64  int d[12] -- the picker descriptor, 48 bytes
 *
 * 8 + 4 + 40 + 48 = 100 EXACTLY. The recon's "40-byte hole" was the ten spill
 * slots; there is no hole and the aggregate is not padded.
 *
 * THE AGGREGATE IS AN `int` ARRAY, NOT A STRUCT, and it did not have to be
 * guessed: three landed siblings in this same cluster already declare it as
 * `int *d` and fix the indices between them --
 * src/rom_a1000/rom_a5534_c_c_c_a_b.c (Func_80a6a00) WRITES d[0],d[2..6];
 * rom_a5534_c_c_c_a_c_c_b.c (Func_80a6b64) and rom_a5534_c_c_c_a_c_b.c
 * (Func_80a6a98) read them. So d[4]=row, d[5]=total, d[6]=index, and this
 * function's `[r4,#0x10]/[r4,#0x14]/[r4,#0x18]` are d[4]/d[5]/d[6]. The
 * ONE aggregate takes the top of the frame at expand time and reload fills in
 * below it, so no aggregate-ordering direction had to be chosen.
 *
 * `add r1, sp, #0x50` passed to Func_80a6a98 is &d[7], and that callee's landed
 * signature shows the argument is UNUSED -- which is why a 48-byte array with
 * only six live members is right and a tighter struct would be wrong.
 *
 * ========================================================================
 * LEVERS THAT PAID, IN ORDER, WITH FIGURES
 * ========================================================================
 * 1. _PlaySound PLACEMENT IN THE 0x100 HANDLER.  41.9% -> 67.3%, +25.4 points,
 *    the single largest move and it was a control-flow READING error, not a
 *    codegen one. The 0x200 handler sounds 0x82 BEFORE Func_80a65e4
 *    (.La7214); the 0x100 handler does NOT (.La7290) -- it sounds 0x82 only
 *    inside the success arm, at .La6d8a, which it reaches by a long branch.
 *    Writing the two handlers symmetrically cost a whole extra call and
 *    destroyed the cross-jumping around it. The lesson is the brief's own: count
 *    the calls on both sides before believing a shape.
 *
 * 2. THE OFFSET AS A NAMED LOCAL, at every VARIABLE offset off `state`.
 *    68.7% -> 69.8%, hunks 129 -> 121, and it moved size from 1724 (44 long)
 *    to 1660 (20 short) and count from 795 to 764. This is lever 5 and it is
 *    what selects Thumb's REGISTER-OFFSET addressing: the ROM has
 *    `ldrh r3,[r7,r3]` / `ldr r2,[r7,r3]` / `ldrb r0,[r1,r3]` throughout, off
 *    `state` in r7, where an inline offset expression makes gcc pre-add the
 *    base and use `[rN,#0]`.
 *
 *    PROBED, because the first spelling I tried was inert (scratch probe, five
 *    spellings of one halfword read):
 *        s + d[6]*2 + 0x1c8              -> add r3,r3,r0 ; ldrh r0,[r3]
 *        (int)s + (d[6]*2 + 0x1c8)       -> IDENTICAL, byte for byte
 *        int o = d[6]*2 + 0x1c8; s + o   -> ldrh r0,[r0,r3]   <-- the ROM's form
 *    So the `(int)base + ofs` cast that the landed siblings use is NOT what
 *    earns the addressing mode -- the NAMED LOCAL is, and the cast rides along
 *    with it. A whole candidate (v5) was byte-identical to its predecessor
 *    proving the cast inert, which is worth recording so nobody spends the
 *    budget on it again.
 *
 * 3. CONSTANT OFFSETS MUST STAY AS POINTER ARITHMETIC. The same treatment
 *    applied to `state[0x268]`, `state + 0x21c`, `state + 8`, `state + 0x24`
 *    and `state + 0x44` is wrong -- the ROM uses `add r3,r7,rN` then `[r3,#0]`
 *    for the big ones and the immediate form `ldr r0,[r7,#0x24]` for the small
 *    ones. The discriminator is purely whether the offset is variable.
 *
 * ========================================================================
 * THINGS MEASURED AND REJECTED -- do not re-spend this budget
 * ========================================================================
 * A. gKeyPress THROUGH A POINTER LOCAL:  69.8% -> 63.3%.  REJECTED.
 *    The ROM holds `&gKeyPress` in r11 and reads it FIVE times through one
 *    pool word, while reading gKeyHeld's address FIVE separate times. That
 *    asymmetry looks like a source difference and is not: it is register
 *    pressure. Once lever 2 above freed registers, the single gKeyPress load
 *    and the five gKeyHeld loads BOTH fall out of the plain
 *    `extern volatile unsigned int` globals, and the relocation sequence went
 *    exact on its own. Forcing it with `volatile unsigned int *kp = &gKeyPress`
 *    costs 6.5 points.
 *
 * B. _CONST_1e / _CONST_1a FOR THE TWO REMAINING POOL WORDS:  69.8% -> 64.0%,
 *    and size moved AWAY from the reference (1660 -> 1644). REJECTED, and no
 *    const.sym entry should be added for either.
 *    The ROM pools 0x1e and 0x1a at .La71ac/.La71b0 and both are 8-bit-movable,
 *    so criterion 1 of const.sym's bar is met and the tell fires. Criterion 2
 *    is NOT met: the symbol spelling is measurably worse, so this is a case
 *    where the tell fires and the answer is still not a symbol. That is the
 *    same outcome const.sym's header already records for Func_80b9470's pooled
 *    0xf, and this is a second instance of it.
 *    Both are `strh` of a constant into a halfword array, which is close to the
 *    HALFWORD EXCEPTION in const.sym's header -- but the exception's signature
 *    is a HALFWORD pool load (`ldrh rN, .L5`) and the ROM here uses a WORD load
 *    (`ldr r2, .La71ac`), so the exception does not explain it either. The two
 *    words are left unexplained and they are 8 of the 20 missing bytes.
 *
 * C. SWAPPING THE i4 / i2 DECLARATION ORDER: byte-identical output, 121 hunks
 *    and 540 aligned either way. INERT, and that is itself the finding: the
 *    sp+0x0c / sp+0x10 / sp+0x14 pseudos are NOT declared locals, so no
 *    declaration order reaches them. Their slots sit BELOW every declared
 *    scalar, which is exactly the "slot order DATES the pass" rule -- they are
 *    late CSE/loop temps for index*2, state+2 and index*4, and this candidate
 *    gets them by writing the expressions, not by declaring variables. Only
 *    the SIX slots from 0x18 to 0x2c are declarations.
 *
 * D. NAMED OFFSET FOR THE p[] SITES VIA A SECOND TEMP (`ofs2`): 69.8% -> 43.0%
 *    and the frame GREW to 0x68. REJECTED. A second offset temp with a live
 *    range long enough to span the Func_80a65e4 argument list gets a stack
 *    slot, and the reference frame at 100 bytes has no spare word -- so any
 *    extra spilled scalar is immediately disqualifying. Reusing the single
 *    `ofs` for those sites instead ties at 540 (size 1672, hunks 117), so it
 *    is a legitimate alternative but not an improvement; v6's spelling is kept
 *    because it has the better size.
 *
 * ========================================================================
 * THE BLOCKER, ATTRIBUTED
 * ========================================================================
 * What is left is REGISTER ASSIGNMENT inside otherwise-correct instruction
 * sequences -- local.c/global.c allocation, not a structural error. The
 * residue after lever 2 is dominated by hunks of the shape
 *     ref  229a movs r2,#154 / 0092 lsls r2,r2,#2 / 18bb adds r3,r7,r2
 *     ours 219a movs r1,#154 / 0089 lsls r1,r1,#1 / 187b adds r3,r7,r1
 * -- same instructions, same order, one register number apart, which then
 * shifts every pc-relative pool offset after it and so reads as a difference
 * in aligncmp's raw comparison.
 *
 * Ruled out as the cause:
 *   * sched2 ordering -- the instruction ORDER inside these hunks matches; only
 *     the register numbers differ, and sched2 does not renumber registers.
 *   * sched1 -- confirmed not to run in this build, so it is not available as
 *     an explanation.
 *   * the frame -- exact, including the aggregate, at every candidate from v1.
 *   * the relocation sequence -- exact, so no call, global or pool SYMBOL is
 *     misplaced; only 8 bytes of unexplained pool CONSTANT remain (item B).
 * The two genuinely unexplained instruction-level items are the 0x1e/0x1a pool
 * words (B) and the single shared `bl Func_80a68ec`: the ROM cross-jumps the
 * two arms of the `state[0x268]` test onto ONE call at .La6dbe, and this
 * candidate emits the call in both arms because gcc placed the four
 * long-branch bodies (.La6d54/.La6d58/.La6d6e/.La6d8a) between them, which
 * blocks jump.c from redirecting. That is 1 of the 10 missing instructions.
 *
 * NEXT MOVE FOR WHOEVER TAKES THIS: the `bl Func_80a68ec` cross-jump. The ROM's
 * layout puts the THEN arm inline and the ELSE arm after those four bodies, so
 * the question is what statement order in the loop head makes gcc emit the
 * long-branch bodies before the else arm rather than between the two calls.
 * That is a block-ordering experiment on the loop's first statement, and it is
 * cheap -- unlike anything in the rejected list above.
 */
struct Unit { unsigned char pad_00[0x3a]; short f3a; };
struct MoveInfo { unsigned char pad_00[9]; unsigned char f9; unsigned char pad_0a[2]; unsigned char fc; };
struct Obj { unsigned char pad_00[5]; unsigned char f5; unsigned char pad_06[6]; unsigned short fc; unsigned char pad_0e; unsigned char ff; };

extern unsigned char *iwram_3001f2c;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyHeld;
extern volatile unsigned int gKeyRepeat;
extern int _CONST_2;
extern struct Unit *_GetUnit(int id);
extern struct MoveInfo *_GetMoveInfo(int id);
extern int _GetFlag(int id);
extern void _PlaySound(int id);
extern void WaitFrames(int n);
extern void _Sprite_SetAnim(int sprite, int anim);
extern void _Func_80164d4(unsigned int win, int x, int y, int w, int h);
extern void _Func_801e7c0(int msg, unsigned int win, int x, int y);
extern int Func_80a10d0(void *slot, int a, int b, int c, int d, int e);
extern void Func_80a112c(int win, int id, int a, int b);
extern void Func_80a17c4(struct Obj *p);
extern void Func_80a1804(unsigned char *state, int id);
extern void Func_80a1a40(int a, int b);
extern int Func_80a1fd4(int swap, int total, int cols, int *col, int *row);
extern int Func_80a65e4(int a, int b, int c);
extern void Func_80a68a8(unsigned char *list);
extern int Func_80a68ec(struct Unit *unit, unsigned char *list, int c);
extern int Func_80a6a00(int *d, int cursor);
extern int Func_80a6a98(unsigned int win, int *a1, int *d);
extern int Func_80a6b64(unsigned int win, int a1, int *d);
extern int Func_80a735c(int id);

int Func_80a6ccc(int index)
{
    unsigned char *state;
    unsigned int win2;
    int ret;
    int needsRedraw;
    int done;
    struct Unit *unit;
    int prevIdx;
    int helpShown;
    int i4;
    unsigned char *p;
    int i2;
    int ofs;
    int ofs2;
    int d[12];
    int k;
    int moved;
    unsigned char i;
    int move;
    struct MoveInfo *info;
    struct Obj *obj;
    signed char mode;
    int cur;
    unsigned short two;

    state = iwram_3001f2c;
    ret = 0;
    prevIdx = 0;
    helpShown = 0;
    i4 = index * 4;
    ofs = i4 + 0x14;
    (*(struct Obj **)(state + ofs))->f5 = 0xd;
    Func_80a10d0(state + 0x34, 0xd, 3, 0x11, 0xe, 2);
    win2 = *(unsigned int *)(state + 0x34);
    done = 0;
    p = state + 2;
    i2 = index * 2;
    while (!done && !_GetFlag(0x150)) {
        unit = _GetUnit(p[index + 0x86 * 4]);
        if (state[0x268])
            state[0x218] = Func_80a68ec(unit, state + 0xe4 * 2, 1);
        else
            state[0x218] = Func_80a68ec(unit, state + 0xe4 * 2, 2);
        Func_80a68a8(state + 0xe4 * 2);
        Func_80a6a00(d, index);
        needsRedraw = 1;
        moved = 1;
        ofs = i4 + 0x14;
        (*(struct Obj **)(state + ofs))->f5 = 1;
        while (!_GetFlag(0x150)) {
            Func_80a1a40(0x58, d[4] * 16 + 0x24);
            if (moved) {
                moved = 0;
                ofs = prevIdx * 2 + 0xe4 * 2;
                if (*(unsigned short *)(state + ofs) != 0) {
                    ofs = prevIdx * 4 + 0x48;
                    Func_80a17c4(*(struct Obj **)(state + ofs));
                }
                if (needsRedraw) {
                    needsRedraw = 0;
                    WaitFrames(1);
                    Func_80a6b64(win2, 0, d);
                }
                Func_80a6a98(win2, &d[7], d);
                ofs = d[6] * 2 + 0xe4 * 2;
                ofs2 = i2 + 0xbc * 2;
                *(unsigned short *)(state + ofs2) = *(unsigned short *)(state + ofs);
                (*(struct Obj **)(state + 0x21c))->f5 = 0xd;
                ofs = d[6] * 2 + 0xe4 * 2;
                if (*(unsigned short *)(state + ofs) != 0) {
                    ofs = d[6] * 4 + 0x48;
                    obj = *(struct Obj **)(state + ofs);
                    obj->f5 = 9;
                    obj->fc = 0;
                    obj->ff = 0xfa;
                }
                for (i = 0; i < state[0x219]; i++) {
                    ofs = i * 4 + 0x8a * 2;
                    _Sprite_SetAnim(*(int *)(state + ofs), 1);
                }
            }
            WaitFrames(1);
            prevIdx = d[6];
            if ((gKeyHeld & 4) == 0)
                k = Func_80a1fd4(0, d[5], 5, &d[4], &d[2]);
            else
                k = -1;
            if (k == 1) {
                needsRedraw = 1;
                moved = 1;
            }
            if (k == 0)
                moved = 1;
            if (k == -1)
                moved = 0;
            if (state[0x268] == 0) {
                if ((gKeyPress & 4) && helpShown == 0) {
                    ofs = d[6] * 2 + 0xe4 * 2;
                    info = _GetMoveInfo(*(unsigned short *)(state + ofs) & 0x3fff);
                    if (info->fc == 0) {
                        _PlaySound(0x72);
                    } else {
                        _PlaySound(0xae);
                        helpShown = 1;
                        two = (unsigned short)(int)&_CONST_2;
                        *(unsigned short *)(state + 0x220) |= two;
                        _Func_80164d4(win2, 0, 0x58, 0x78, 0x60);
                        _Func_801e7c0(0xae1, win2, 0, 0x58);
                    }
                }
                if ((gKeyHeld & 4) == 0 && helpShown == 1) {
                    helpShown = 0;
                    *(unsigned short *)(state + 0x220) &= 0xfffd;
                    _Func_80164d4(win2, 0, 0x58, 0x78, 0x60);
                    _Func_801e7c0(0xb89, win2, 0, 0x58);
                }
            }
            if (gKeyPress & 1) {
                if (state[0x268]) {
                    _PlaySound(0x82);
                    ofs = d[6] * 2 + 0xe4 * 2;
                    ret = *(unsigned short *)(state + ofs);
                    done = 1;
                    break;
                }
                ofs = d[6] * 2 + 0xe4 * 2;
                move = *(unsigned short *)(state + ofs);
                if (move != 0) {
                    if (Func_80a735c(move)) {
                        _PlaySound(0x72);
                        continue;
                    }
                    ofs = d[6] * 2 + 0xe4 * 2;
                    info = _GetMoveInfo(*(unsigned short *)(state + ofs) & 0x3fff);
                    if (info->f9 > unit->f3a) {
                        _PlaySound(0x72);
                    } else {
                        _PlaySound(0xad);
                        ofs = d[6] * 2 + 0xe4 * 2;
                        ret = *(unsigned short *)(state + ofs);
                        done = 1;
                        break;
                    }
                }
            }
            if (gKeyPress & 2) {
                _PlaySound(0x71);
                ret = -1;
                done = 1;
                break;
            }
            if (((gKeyRepeat & 0x100) || (gKeyRepeat & 0x200)) && (gKeyHeld & 4) == 0) {
                mode = state[0x268] != 0 ? 1 : 2;
                _PlaySound(0x6f);
                ofs = p[index + 0x86 * 4] + 0x98 * 4;
                *(unsigned char *)(state + ofs) = d[6];
                ofs = index + 0x1c;
                cur = *(signed char *)(state + ofs);
                do {
                    if (gKeyRepeat & 0x100)
                        cur++;
                    else
                        cur--;
                    cur = (cur + state[0x219]) % state[0x219];
                    ofs = cur * 2 + 0x82 * 4;
                    *(int *)(state + 8) = *(unsigned short *)(state + ofs);
                    state[0x21a] = *(unsigned short *)(state + ofs);
                    state[0x218] = Func_80a68ec(_GetUnit(state[0x21a]), state + 0xe4 * 2, mode);
                } while ((unsigned char)state[0x218] == 0);
                ofs = index + 0x1c;
                *(unsigned char *)(state + ofs) = cur;
                for (i = 0; i <= 3; i++) {
                    ofs = i * 2 + 0xa2 * 2;
                    *(unsigned short *)(state + ofs) = 0x1e;
                }
                ofs = cur * 2 + 0xa2 * 2;
                *(unsigned short *)(state + ofs) = 0x1a;
                ofs2 = cur * 2 + 0x82 * 4;
                Func_80a112c(*(int *)(state + 0x24), *(unsigned short *)(state + ofs2), 0, 0);
                Func_80a1804(state, *(unsigned short *)(state + ofs2));
                break;
            }
            if ((gKeyPress & 0x200) && (gKeyHeld & 4) == 0) {
                ofs = d[6] * 2 + 0xe4 * 2;
                info = _GetMoveInfo(*(unsigned short *)(state + ofs) & 0x3fff);
                if (info->fc == 0) {
                    _PlaySound(0x72);
                } else {
                    _PlaySound(0x82);
                    ofs = d[6] * 2 + 0xe4 * 2;
                    if (Func_80a65e4(p[index + 0x86 * 4], *(unsigned short *)(state + ofs), 0)) {
                        ofs = d[6] * 2 + 0xe4 * 2;
                        ret = *(unsigned short *)(state + ofs);
                        state[0x268] = 1;
                        done = 1;
                        break;
                    }
                }
            }
            if ((gKeyPress & 0x100) && (gKeyHeld & 4) == 0) {
                ofs = d[6] * 2 + 0xe4 * 2;
                info = _GetMoveInfo(*(unsigned short *)(state + ofs) & 0x3fff);
                if (info->fc == 0) {
                    _PlaySound(0x72);
                } else {
                    ofs = d[6] * 2 + 0xe4 * 2;
                    if (Func_80a65e4(p[index + 0x86 * 4], *(unsigned short *)(state + ofs), 1)) {
                        _PlaySound(0x82);
                        ofs = d[6] * 2 + 0xe4 * 2;
                        ret = *(unsigned short *)(state + ofs);
                        state[0x268] = 2;
                        done = 1;
                        break;
                    }
                }
            }
        }
    }
    *(unsigned short *)(state + 0x220) &= 0xfffd;
    Func_80a17c4(*(struct Obj **)(state + 0x44));
    ofs = i2 + 0xba * 2;
    *(unsigned short *)(state + ofs) = d[6];
    ofs = p[index + 0x86 * 4] + 0x98 * 4;
    *(unsigned char *)(state + ofs) = d[6];
    ofs = i2 + 0xbc * 2;
    *(unsigned short *)(state + ofs) = ret;
    if (_GetFlag(0x150))
        ret = -1;
    WaitFrames(1);
    return ret;
}

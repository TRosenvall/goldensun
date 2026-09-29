/* Func_80a38d0 -- 0x080a38d0, asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_c_a_a.s
 * (2 functions: this one and Func_80a3c08, which is parked at
 * src/non_matching/rom_a1000/80a3c08.c).
 *
 * NON-MATCHING, 199 of 362 encodings differ.
 * (objcmp's 199 SATURATES -- ours is 356 instructions against 362, six short, and
 * size is 808 against 824.  The honest figure is aligncmp's 63 differing, 305 of
 * 362 aligned-equal = 84.3%.  The relocation SEQUENCE matches across all 38
 * symbols; only the offsets move.) encodings differ.  Size 808 against the ROM's 824 --
 * SIX ENCODINGS SHORT, and five of the six are accounted for below, so the
 * shortfall is NOT a missing statement.  The relocation SYMBOL SEQUENCE is
 * identical, 38 for 38, in order; only the offsets move.  Zero shims.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a38d0.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_c_a_a.s --func Func_80a38d0
 *   -> XX SIZE  ref 824 bytes, ours 808
 *      XX ENCODINGS differ in 199 place(s) (ref 362, ours 356)
 *      first at index 37
 *      XX RELOCATIONS differ      (OFFSETS ONLY -- the symbol sequence matches)
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py src/non_matching/rom_a1000/80a38d0.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_c_a_a.s Func_80a38d0
 *   -> aligned-equal 305  (84.3% of ref)   differing/ins/del 63 in 35 hunks
 *
 * objcmp's 199 is NOT a distance here -- the counts differ, so it saturates.
 * Rank revisions on aligncmp's "differing/ins/del", and re-read the SIZE line.
 *
 * SPLIT SHAPE, when the body lands: plain code/code, NO data (datacheck.py is
 * silent), ZERO exports.
 *   src/rom_a1000/rom_a1814_c_a_c_c_c_c_a_c_a_a_a.c   Func_80a38d0
 *   asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_c_a_a_b.s   Func_80a3c08 (stays asm)
 * stage1.ld line 1273 becomes those two .o lines in that order.  Do not split
 * until the body lands -- Func_80a3c08 is parked too, and its own header
 * already says the same.
 *
 * WHAT IT IS: the Use / Give / Drop row on the item screen.  r0 = mode (1 = the
 * party-target sub-menu, 0 = the plain action row).  Resizes state+0x20, starts
 * Func_80a3c08 as a task so the party sprites nod, then loops on the d-pad
 * until save bit 0x150; returns the chosen slot as a SIGNED CHAR (`lsl #24 /
 * asr #24` at the exit), 0xff on B.
 *
 * ================= WHAT WAS WON, and what transfers =================
 * First draft 146 of 362 differing (68.0%), and the frame was already right.
 *
 *  1. THE `sel * 2` PRE LEVER, AGAIN, AND IT IS THE BIGGEST SINGLE STEP HERE
 *     TOO: 124 -> 75 differing.  See the twin Func_80a35f8
 *     (src/rom_a1000/rom_a1814_c_a_c_c_c_c_a_a_b.c) for the full statement of
 *     it.  Here the ROM's evidence is three copies of one value: sp+4, r9, and
 *     the live r2/r7 -- sp+4 is gcse's `reaching_reg` spilled, r9 is the copy
 *     inserted at the definition, and the two `mov r3, sl / lsl r3, #1 /
 *     str r3, [sp, #4]` blocks at .La3b08 and .La3bae are PRE's insertions on
 *     the two edges where `sel * 2` is not available.  A DECLARED `idx2`
 *     VARIABLE REPRODUCES NONE OF THAT and additionally steals r10 from `sel`,
 *     which is why `cmp sl, r3` reads `cmp r9, r3` in every draft that had one.
 *
 *  2. EVERY `0x82 * 4 + sel * 2` ADDRESS NEEDS ITS OWN NAMED OFFSET LOCAL.
 *     Six sites.  Inline, gcc reassociates to `(st + sel*2) + 0x208` and emits
 *     six instructions where the ROM has four (`mov #0x82 / lsl #2 / add r9 /
 *     ldrh [r6, rN]`).  Named, it emits the ROM's four.  Worth 15 encodings of
 *     length.  UNLIKE Func_80a112c's `s2 + 0xd8`, ONE NAME PER SITE IS SAFE
 *     HERE AND THE REASON IS WORTH KEEPING: 0xd8 fits an 8-bit immediate, so
 *     both sites hash as the same `(plus reg (const_int 216))` and gcse commons
 *     the whole sum; 0x208 does NOT fit, so it is materialised into a fresh
 *     pseudo per site and the `plus`es never match.  THE TEST FOR WHETHER A
 *     NAMED OFFSET WILL BE HOISTED IS WHETHER ITS CONSTANT FITS `adds rN, #imm8`.
 *
 *  3. THREE ONE-LINE STATEMENT MOVES, 132 -> 63 in total: read state+0x1d
 *     (`sel`) BEFORE state+0x219 (`cnt`); write `first = 0;` BEFORE the
 *     `(sel + cnt) % cnt` rather than after; and write `first = 1;` BEFORE
 *     `ret = 0; msg = 0;`.  Each was measured alone (70.7%, 82.9%, 83.4%) and
 *     together (84.3%).  Same mechanism as the twin's lever 4 -- statement order
 *     fixes which low register is free for the next constant.
 *
 *  4. NOT A LEVER, MEASURED AND REJECTED TWICE: a named `m = 0x1ff`.  The ROM
 *     holds 0x1ff in r11 across calls and reads it back for two of its four
 *     masks, which reads exactly like the "named base" tell.  It is not: on the
 *     pre-lever-2 base it cost 68% -> 61%, and on the post-lever-2 base
 *     69.3% -> 63.3% with one use and -> 58.8% with all four, SHRINKING the
 *     frame to 0x18.  r11 in the ROM is CSE's own choice of a callee-saved home
 *     for the bitfield store's mask, reached from plain literals; do not name it.
 *
 * ================= THE RESIDUE, FIVE CLUSTERS =================
 *
 * (A) THE TWO POOLED SMALL CONSTANTS, 4 lines and 2 of the 6 missing encodings.
 *     The ROM writes `ldr r0, =0xb30` and `ldr r3, =0x75` where we emit
 *     `mov r0, #0xb3 / lsl r0, #4` and `adds r0, #117`.  Both pool words are
 *     plain literals with NO relocation, so the const.sym symbol tell does not
 *     apply, and neither is an operand of a halfword expression, so the batch-
 *     291 halfword exception does not either.  Both are single-use, which is
 *     the case elevation.md says IS the tell -- and yet there is no symbol to
 *     name.  This is a THIRD case the pooled-small-constant rule does not cover
 *     and it wants a reading from arm.md's const_int split (which declines when
 *     the destination is r7), not another spelling sweep.
 *
 * (B) 0x174 DERIVED FROM 0x21a.  In the Func_80a3ef0 argument setup we emit
 *     `subs r1, #0xa6` to turn 0x21a into 0x174; the ROM builds 0x174
 *     independently as `mov #0xba / lsl #1`.  This is the constant-derivation
 *     peephole that elevation.md tells you NOT to fight -- except here the ROM
 *     is the one that did not derive.  Measured and rejected: hoisting the
 *     state+0x174 read into a local before the call (68.5%), hoisting the
 *     state+0x21a read instead (83.4%).  Both worse than leaving them inline.
 *
 * (C) THE 0xffff POOL WORD SORTS THIRD IN OURS AND FIRST IN THE ROM.  This is
 *     EXACTLY the class src/non_matching/rom_a1000/80a1a40.c characterises and
 *     closes as unreachable: for 0xffff to sort first its reference must have a
 *     narrow pool_range, i.e. be `*thumb_zero_extendhisi2`, and a HImode
 *     `& 0xffff` cannot be expressed in C (fold and simplify_binary_operation
 *     both delete it).  THE BITFIELD ITSELF IS ALREADY RIGHT -- `unsigned short
 *     a16 : 9` is what puts the mid-function pool here at all, and the struct
 *     below is 80a1a40.c's struct with a byte field added at +5.  Read that
 *     park's eleven-spelling list before trying a twelfth.
 *
 * (D) r2/r3 AND r0/r3 TRANSPOSITIONS, roughly 20 lines, in the `t` computation,
 *     the `first`/mode reloads and the tail's `0x208 + idx2` address.  The
 *     documented terminal class.
 *
 * (E) ONE SCHEDULE SLOT IN THE TAIL: the ROM issues `str r3, [r6, #8]` before
 *     `add r1, #0x13`, we issue it after.  The `o += 0x13;` derived-constant
 *     spelling is already in place and produces the ROM's `add`, only one slot
 *     late.
 *
 * NOT A RESIDUE: the frame (0x1c) and the whole spill map -- sp+4 idx2,
 * sp+8 msg, sp+0xc ret, sp+0x10 first, sp+0x14 cnt, sp+0x18 the incoming mode --
 * match the ROM exactly and did so from the first draft, which is what said the
 * variable set and declaration order were right and sent the search at the
 * addressing modes instead.
 */
extern int iwram_3001f2c;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;

struct Win {
    unsigned char pad00[0xc];
    unsigned short fc;
    unsigned short fe;
};

struct Cur {
    unsigned char pad00[5];
    unsigned char f5;
    unsigned short x;
    unsigned short y;
    unsigned char pad0a[0xa];
    unsigned short b14 : 8;
    unsigned short : 8;
    unsigned short a16 : 9;
};

extern void Func_80a23f4(void *win, int a, int b, int c, int d);
extern void _Func_8016498(void *win);
extern char *_GetUnit(int id);
extern int StartTask(void *fn, int pri);
extern void Func_80a3c08(void);
extern int _GetFlag(int flag);
extern void _ClearFlag(int flag);
extern void Func_80a3e88(int id, int f);
extern void _Func_801e41c(void *win, int a, int b, int c, int d);
extern void _Func_80164d4(void *win, int a, int b, int c, int d);
extern int Func_80a3d9c(int id, int item);
extern void _Func_801ea08(int v, int n, void *win, int x, int y);
extern void _Func_801e7c0(int id, void *win, int x, int y);
extern int Func_80a3d6c(int id);
extern void Func_80a3ef0(int a, int b, int c, int d);
extern int Func_80a3ce4(int item);
extern void Func_80a112c(void *box, int id, int slot, int mode);
extern void Func_80a1a40(int x, int y);
extern void WaitFrames(int n);
extern void _PlaySound(int sfx);
extern void Func_80a17c4(struct Cur *cur);
extern void Func_80a3c98(void);

int Func_80a38d0(int mode)
{
    int cnt;
    int first;
    int ret;
    int msg;
    char *st;
    void *win;
    struct Cur *cur;
    struct Win *w;
    int sel;
    int off;
    int t;
    int n;
    int o;
    int o2;
    int o3;
    int o4;
    int o5;
    int o6;
    int o7;

    st = (char *)iwram_3001f2c;
    win = *(void **)(st + 0x20);
    o = 0x1d;
    sel = *(signed char *)((int)st + o);
    cnt = *(unsigned char *)(st + 0x219);
    first = 1;
    ret = 0;
    msg = 0;
    Func_80a23f4(win, 0xd, 5, 0x11, 0xc);
    _Func_8016498(*(void **)(st + 0x20));
    o = 0x1c;
    o2 = *(signed char *)((int)st + o) * 2 + 0x82 * 4;
    _GetUnit(*(unsigned short *)((int)st + o2));
    StartTask(Func_80a3c08, 0xc8 * 16);
    while (_GetFlag(0xa8 * 2) == 0) {
        if (first != 0) {
            first = 0;
            sel = (sel + cnt) % cnt;
            win = *(void **)(st + 0x20);
            off = sel * 2 + 0x82 * 4;
            _GetUnit(*(unsigned short *)((int)st + off));
            w = *(struct Win **)(st + 0x10);
            cur = *(struct Cur **)(st + 0x18);
            t = (w->fc + (sel * 2 + sel)) * 8 - 2;
            cur->x = t;
            t &= 0xffff;
            cur->a16 = t;
            if (mode == 1) {
                Func_80a3e88(*(unsigned short *)((int)st + off), 1);
                _Func_801e41c(win, 0, 9, 0x10, 9);
                _Func_80164d4(win, 0, 0x48, 0x78, 0x50);
                o = 0x1c;
                if (sel != *(signed char *)((int)st + o)) {
                    n = Func_80a3d9c(*(unsigned short *)((int)st + off),
                                     0x1ff & *(unsigned short *)(st + 0xbc * 2));
                    if (n != 0) {
                        _Func_801ea08(n, 2, win, 8, 0x48);
                        _Func_801e7c0(0xb2f, win, 0x18, 0x48);
                    } else {
                        _Func_801e7c0(0xb31, win, 0x10, 0x48);
                    }
                    o3 = 0x82 * 4 + sel * 2;
                    if (Func_80a3d6c(*(unsigned short *)((int)st + o3)) == 0xf
                        && n == 0)
                        _Func_801e7c0(0xb30, win, 0, 0x48);
                }
                o4 = 0x82 * 4 + sel * 2;
                Func_80a3ef0(*(unsigned char *)(st + 0x21a),
                             *(unsigned short *)(st + 0xba * 2), 0,
                             *(unsigned short *)((int)st + o4));
            }
            if (mode == 0) {
                if (Func_80a3ce4(0x1ff & *(unsigned short *)(st + 0xbc * 2)) != 0) {
                    o5 = 0x82 * 4 + sel * 2;
                    Func_80a112c(*(void **)(st + 0x24),
                                 *(unsigned short *)((int)st + o5),
                                 *(unsigned short *)(st + 0xba * 2), 8);
                } else {
                    o6 = 0x82 * 4 + sel * 2;
                    Func_80a112c(*(void **)(st + 0x24),
                                 *(unsigned short *)((int)st + o6),
                                 *(unsigned short *)(st + 0xba * 2), 0);
                }
                if (_GetFlag(0x151) == 0 && msg == 0) {
                    _Func_8016498(*(void **)(st + 0x2c));
                    _Func_801e7c0((0x1ff & *(unsigned short *)(st + 0xbc * 2)) + 0x75,
                                  *(void **)(st + 0x2c), 0, 0);
                    msg = 1;
                } else {
                    _ClearFlag(0x151);
                }
            }
        }
        Func_80a1a40((sel * 2 + sel) * 8 - 0xa, 0x10);
        WaitFrames(1);
        if ((gKeyPress & 1) != 0) {
            o = 0x1c;
            if (mode == 1 && sel == *(signed char *)((int)st + o)) {
                _PlaySound(0x72);
                continue;
            }
            _PlaySound(0x70);
            o7 = sel * 2 + 0x82 * 4;
            ret = *(unsigned char *)((int)st + o7);
            goto out;
        }
        if ((gKeyPress & 2) != 0) {
            _PlaySound(0x71);
            ret = 0xff;
            goto out;
        }
        if ((gKeyRepeat & 0x20) != 0) {
            _PlaySound(0x6f);
            first = 1;
            sel--;
        }
        if ((gKeyRepeat & 0x10) != 0) {
            _PlaySound(0x6f);
            first = 1;
            sel++;
        }
    }
out:
    cur = *(struct Cur **)(st + 0x18);
    st[0x1d] = sel;
    Func_80a17c4(cur);
    cur->f5 = 0xd;
    Func_80a3c98();
    WaitFrames(1);
    st[0x1d] = sel;
    o = 0x82 * 4;
    o2 = sel * 2 + o;
    *(int *)(st + 8) = *(unsigned short *)((int)st + o2);
    o += 0x13;
    *(unsigned char *)((int)st + o) = *(unsigned short *)((int)st + o2);
    return (signed char)ret;
}

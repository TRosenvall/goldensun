/* Func_8092c40 -- OpenMessageBoxForSlot, 0x08092c40, the ONLY function in
 * asm/rom_8a000/rom_92950_c_a_c_a_c_a.s (no split needed; whole-file conversion).
 *
 * NON-MATCHING, 301 of 397 encodings differ.
 * (objcmp's 301 SATURATES on a 4-instruction difference -- ours is 401 against 397
 * and 844 bytes against 836.  The honest figure is aligncmp's 29 differing, 373 of
 * 397 aligned-equal = 94.0%.  The frame `sub sp, #0x40` and the whole 13-slot spill
 * map are already the ROM's.) encodings differ.
 * objcmp verbatim (its count SATURATES -- the two files differ by four
 * instructions, so read the aligncmp figure for distance):
 *     XX SIZE  ref 836 bytes, ours 844
 *     XX ENCODINGS differ in 301 place(s) (ref 397, ours 401)
 *        first at index 7: ref 4bc9  ours 4bcb
 * tools/aligncmp.py: 373 aligned-equal (94.0% of ref), 29 differing/ins/del in
 * 25 hunks.  FRAME `sub sp, #0x40` and the ENTIRE 13-slot spill map are the
 * ROM's.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/8092c40.c \
 *     asm/rom_8a000/rom_92950_c_a_c_a_c_a.s --func Func_8092c40
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py src/non_matching/rom_8a000/8092c40.c \
 *     asm/rom_8a000/rom_92950_c_a_c_a_c_a.s Func_8092c40 -v
 *
 * `python3 tools/datacheck.py asm/rom_8a000/rom_92950_c_a_c_a_c_a.s` is SILENT --
 * no data section, no exports to rehome, one stage1.ld pair of lines to repoint.
 *
 * THE CALL-SITE ENTRY IN docs/elevation.md IS ABOUT THIS FUNCTION AND IT IS
 * CONSISTENT WITH THE BODY.  "`__Func_8092c40` WANTS THE DESCENDING FILL" says it
 * takes `q1 = 0; q0 = N;` at six of eight sites and no pin at the two that end a
 * block, and "Identical counts across unrelated spellings: now look at a
 * SIGNATURE" says marking it `int` is what makes r0 live out.  The body confirms
 * both: the epilogue is `pop {r3, r5, r6, r7} ... pop {r1} / bx r1` with
 * `mov r0, r10` ahead of it, so it RETURNS r10 and the declaration must be `int`;
 * and the SECOND PARAMETER IS NEVER READ (`ldr r1,[r3]` overwrites r1 in the
 * third instruction), which is why the fill order at a site is free to be
 * whatever the surrounding block wants.  Two parameters are declared here to
 * match the sites; an unused register parameter costs nothing in the callee.
 *
 * ================================================================
 * WHAT CLOSED IT, BY PASS -- 224 -> 29
 * ================================================================
 *
 * Pass 1 (224 of 397, 52.9% aligned, 4 instructions short).  Structure, control
 * flow, every call and every relocation right from the start, read off the ROM
 * plus the landed sibling src/rom_8a000/rom_92950_c_c_c_c_a.c (Func_80931ec),
 * which supplied four oracles: `iwram_3001ebc` as an `unsigned char *`, the
 * `(*(short *)(p + (0xec << 1)))++` message-id idiom (0x1d8 is 0xec<<1 -- the
 * ROM's `sub r1,#0x24` off 0x1fc), `_Func_8017658`/`_Func_8019da8` declared `int`
 * because r0 is filled last, and `GetSpriteVoice` declared WITHOUT a prototype so
 * its argument is emitted at all.  `_DialogueBox` and `_TextBox` are
 * FIVE-argument (`id, &bx, &by, &w, &h`); the `str r4,[sp,#4]` beside the first
 * one is x's SPILL, not a sixth argument -- the outgoing area is one word.
 *
 * 224 -> 55 (88.2%), THE ONE THAT MATTERED:  `y = 0; flag = 0;` MUST BE SET
 * BEFORE THE FIRST GetFieldActor CALL, not after the `*(p + 0x1f4) = slot` store.
 * Moving those two statements up is a one-line edit and it rebuilt the whole
 * frame.  Mechanism, and it is a general rule: `-fcall-used-r4` leaves SEVEN
 * callee-saved registers (r5-r11) and this function has EIGHT values that want
 * one.  With `flag` born after the call it crosses none, global_alloc gives it
 * r2, and `x` -- which the ROM puts in the call-used r4 and saves around every
 * call -- wins a callee-saved register instead.  That drops x's four spill/reload
 * instructions AND shifts every one of the thirteen stack slots by four, because
 * the frame loses a word (0x3c against the ROM's 0x40).
 *
 * > WHEN A CANDIDATE IS EXACTLY ONE STACK SLOT SHORT AND EVERY OFFSET IS LOW BY
 * > THAT SLOT, THE MISSING SLOT IS A CALLER-SAVE SPILL, AND THE LEVER IS TO GIVE
 * > SOME OTHER LOCAL A LONGER LIVE RANGE -- BY global.c's refs^2/live_length
 * > PRIORITY A SHORT RANGE OUTRANKS A LONG ONE, so the variable you move UP is
 * > the one that should NOT get the register.
 *
 * 55 -> 32 (93.5%)  `gState + 0x1f4` MUST GO THROUGH A POINTER LOCAL.  The ROM
 * has `ldr r3,=gState / movs r2,#0xfa / lsls r2,#1 / adds r5,r3,r2`; every
 * pointer-arithmetic spelling off `extern unsigned char gState[]` folds to one
 * `ldr =gState+500` pool word.  `unsigned char *gs = gState;` then
 * `int *q = (int *)(gs + (0xfa << 1));` gives the ROM's shape and cse serves both
 * `*q` reads from r5 -- the lever recorded on 80a6614.c ("`unsigned char *g =
 * gState;` gives the ROM's shape"), and the "local pointer only when the offset
 * is constant" half of the entry at docs/elevation.md ~10121.  A local `int *q`
 * alone is INERT: the fold happens on the symbol, so the pointer has to be the
 * thing that stops it.
 *
 * 32 -> 29  `flag = 1;` SITS BETWEEN THE TWO HALVES OF y.  The ROM emits
 * `asr r5,r3,#3 / mov r7,#1 / sub r5,#2`, so the source is
 * `y = vec.y >> 3; flag = 1; y -= 2;` -- three statements, in that order.  Written
 * `y = (vec.y >> 3) - 2;` gcc emits the three-operand `subs r5,r3,#2`; written as
 * two statements with `flag = 1` after them, the `sub r5,#2` lands one insn early.
 *
 * ================================================================
 * FOUR BLOCKERS, ALL QUANTIFIED
 * ================================================================
 *
 * 1. `x != -1` WILL NOT EMIT `mvn`, AND EVERY `~x` SPELLING BREAKS CROSS-JUMPING.
 *    This is the largest single item, worth 3 instructions and 5 encodings.  The
 *    ROM computes the Func_8094154 result flag branchlessly as
 *        mvn r0,r0 / neg r3,r0 / orr r3,r0 / lsr r7,r3,#31
 *    i.e. `!!(~ret)`.  `flag = Func_8094154(...) != -1;` gives the same branchless
 *    `sne` tail but materialises the constant instead of complementing:
 *        movs r3,#1 / negs r3,r3 / eors r0,r3 / negs / orrs / lsrs / adds r7,r3,#0
 *    SIX spellings were measured and the `~` ones are all far WORSE, not better,
 *    because gcc-2.96 then abandons the branchless form entirely -- it emits
 *    `mvn r7,r0 / cmp r7,#0 / beq / mov r7,#1`, and the branch STOPS the ROM's
 *    cross-jump of the two arms (the ROM's `.L92cde` branches into `.L92d24`
 *    inside the other arm, sharing the call and the flag computation), duplicating
 *    about seven instructions:
 *        `flag = Func_8094154(...) != -1;`              401 insns, 29 differing  <- this file
 *        `flag = ~Func_8094154(...) != 0;`              412 insns, 199 differing
 *        `int rv = ...; flag = ~rv != 0;`               412 insns, 199 differing
 *        `flag = (Func_8094154(...) ^ -1) != 0;`        416 insns, 197 differing
 *        `flag = (unsigned)~Func_8094154(...) != 0;`    416 insns, 197 differing
 *        `flag = !(Func_8094154(...) == -1);`           401 insns, 29 differing (identical)
 *        unsigned return + `!= 0xffffffff`              401 insns, 29 differing (identical)
 *    > A `~` IN THE SOURCE IS NOT THE WAY TO REACH A ROM `mvn` HERE: it changes
 *    > which EXPANSION gcc picks for the comparison, and the branchy one it picks
 *    > instead is what loses the cross-jump.  Something else in the original
 *    > source produced a NOT-ed pseudo the comparison could consume.
 *
 * 2. A DEAD `ldrb` THE ROM KEEPS.  Between `_Func_8017658` and `_Func_8019da8`
 *    the ROM reads `A[0xea4]` into r3 and then OVERWRITES r3 with `t` -- four
 *    instructions whose value is discarded (`ldr r1,[sp,#32] / ldr r2,=0xea4 /
 *    adds r3,r1,r2 / ldrb r3,[r3]`).  gcc deletes a dead non-volatile load, so
 *    this file spells it `(void)*(volatile unsigned char *)(A + 0xea4);`, which
 *    reproduces the four instructions and is why we are four LONG rather than
 *    four short.  That is a fakematch-class device (though not a `register` pin,
 *    so `tools/shimcount.py` reports 0) and it is the thing to replace: the same
 *    four instructions with no volatile cast would come from a THIRD real use of
 *    that byte that gcc then optimises away, and the original source probably had
 *    one.  Dropping the cast scores 32 differing with the count coincidentally
 *    exact at 397 (scratch_elev/b300b/m1.c) -- exact only because it trades these
 *    four instructions against the three from blocker 1 and the one from
 *    blocker 3, so that 397 is NOT a distance.
 *
 * 3. `t = by - 5` IS CSEd INTO r12 AND THE ROM RECOMPUTES IT.  One instruction:
 *    ours `mov ip,r1` ... `mov r1,ip`, the ROM `subs r1,r2,#5` twice.  `by` is
 *    address-taken and still in r2 at the second site in both.
 *
 * 4. `g` AND `flags` EXCHANGE r9 AND r11.  Ten encodings, every one a
 *    `mov rX, r9` / `mov rX, fp` pair.  ref spends r9 on `g` and r11 on `flags`
 *    (with the 0xf000 constant pseudo tied into flags' allocno); we do the
 *    reverse.  MEASURED INERT: all SIX permutations of the `int h / int g /
 *    unsigned int flags` declarations, byte-identical at 29 -- which is the
 *    documented "declaration order is inert; allocation follows ASSIGNMENT
 *    position".  MEASURED WORSE: `g = 0; h = 0;` (30), `flags` assigned before
 *    n/t/pad (42), `int flags` with an `(unsigned)` cast at the `>> 15` (29,
 *    identical), `a` declared after `flags` (55).
 *
 * TWO TYPE FACTS worth keeping whatever happens to this file:
 *   - `flags` MUST BE UNSIGNED.  The ROM tests bit 15 with `lsr r3,r1,#15`, a
 *     LOGICAL shift, while bit 14 is tested with an explicit `0x4000` mask -- so
 *     the source really is `(flags >> 15) == 0` on an unsigned, and the test is
 *     dead code (flags is `arg & 0xf000`) that gcc cannot fold.
 *   - `arg` IS REUSED AS THE SLOT.  The ROM's `and r6, r3` overwrites the packed
 *     argument in place, so `arg &= 0xfff;` rather than a second local; a separate
 *     `slot` costs a callee-saved register and was part of pass 1's deficit.
 *
 * SHIMS: `tools/shimcount.py` reports 0 register pins.  The volatile read in
 * blocker 2 is the only shim-shaped construct and is documented above.
 */
#include "gba/types.h"

extern unsigned char *iwram_3001e8c[];
extern unsigned char gState[];

extern int Func_8092ba8(int id);
extern int GetFieldActor(int id);
extern void PhysMove(int src, vec3_t *dst);
extern int Func_8094154(int id, vec3_t *dst);
extern void _DialogueBox(int id, int *a, int *b, int *c, int *d);
extern void _TextBox(int id, int *a, int *b, int *c, int *d);
extern int _GetPortrait(int id);
extern int GetSpriteVoice();
extern int _Func_8017658(int id, int a, int b, int c);
extern int _Func_8019da8(int a, int b, int c, int d);
extern int _Func_8017364(void);
extern void WaitFrames(int n);

int Func_8092c40(int arg, int unused)
{
    vec3_t vec;
    int bx;
    int by;
    int bw;
    int bh;
    unsigned char *A;
    unsigned char *p;
    int a;
    int n;
    int t;
    int pad;
    int x;
    int y;
    int flag;
    int msg;
    int h;
    int g;
    unsigned int flags;
    int bx2;
    int port;
    int actor;

    A = iwram_3001e8c[0];
    p = iwram_3001e8c[0xc];
    h = 0;
    g = 0;
    a = Func_8092ba8(arg);
    n = 0;
    t = 0;
    pad = 4;
    flags = arg & 0xf000;
    msg = *(short *)(p + (0xec << 1));
    arg &= 0xfff;
    x = 0;
    y = 0;
    flag = 0;
    actor = GetFieldActor(arg);
    *(int *)(p + (0xfa << 1)) = arg;
    if (*(int *)(p + (0xe6 << 1)) == 0) {
        if (actor != 0) {
            if (*(short *)(p + (0xcf << 1)) == 3) {
                PhysMove(actor + 8, &vec);
                x = vec.x >> 3;
                y = vec.y >> 3;
                flag = 1;
                y -= 2;
            } else {
                flag = Func_8094154(arg, &vec) != -1;
                x = vec.x >> 3;
                y = vec.y >> 3;
            }
        } else if (arg <= 7) {
            unsigned char *gs = gState;
            int *q = (int *)(gs + (0xfa << 1));
            a = arg;
            actor = GetFieldActor(*q);
            if (*(short *)(p + (0xcf << 1)) == 3) {
                PhysMove(actor + 8, &vec);
                x = vec.x >> 3;
                y = vec.y >> 3;
                flag = 1;
            } else {
                flag = Func_8094154(*q, &vec) != -1;
                x = vec.x >> 3;
                y = vec.y >> 3;
            }
        }
        if (flag == 0) {
            bx = 0xf;
            by = 0xa;
        } else {
            bx = 0;
            by = 0;
            _DialogueBox(msg, &bx, &by, &bw, &bh);
            bx = x - bw / 2;
            if (flags & 0x4000) {
                by = y - bh - 1;
            } else if ((flags >> 15) == 0 && y <= 8) {
                by = y - bh - 1;
            } else {
                by = y + 4;
            }
        }
        if (A[0xea4] != 0) {
            pad = 5;
        }
        bx2 = x;
        if (flags & 0x1000) {
            bx2 = bx2 - pad - 2;
            if (bx2 < 0) {
                bx2 = 0;
            }
        } else if (flags & 0x2000) {
            bx2 += 2;
            if (bx2 + pad > 0x1d) {
                bx2 = 0x1d - pad;
            }
        } else if (bx2 <= 0xf) {
            bx2 = bx2 - pad - 2;
            if (bx2 < 0) {
                bx2 = x + 2;
            }
        } else {
            bx2 += 2;
            if (bx2 + pad > 0x1d) {
                bx2 = x - pad - 2;
            }
        }
        port = _GetPortrait(a);
        if (port != -1) {
            _DialogueBox(msg, &bx, &by, &bw, &bh);
            msg = -1;
            t = by - 5;
            if (by <= y) {
                t = by + bh;
            }
            if (t < 0) {
                t = by + bh;
            } else if (t + 5 > 0x13) {
                t = by - 5;
            }
            if (by < t) {
                int oldh = bh;
                _TextBox(msg, &bx, &by, &bw, &bh);
                n = oldh - bh + 1;
                msg = -1;
            }
        } else if (by < y) {
            int oldh = bh;
            _TextBox(msg, &bx, &by, &bw, &bh);
            n = oldh - bh + 1;
            msg = -1;
        }
        if (bx2 < 0) {
            bx2 = 0;
        } else if (bx2 + pad > 0x1d) {
            bx2 = 0x1d - pad;
        }
        if (A[0xea4] != 0) {
            WaitFrames(8);
            if (n != 0) {
                h = _Func_8017658(msg, bx, by + n - 1, 0x12);
            } else {
                h = _Func_8017658(msg, bx, by, 2);
            }
        } else {
            int voice = GetSpriteVoice(a);
            if (n != 0) {
                h = _Func_8017658(msg, bx, by + n - 1, (voice << 16) | 0x11);
            } else {
                h = _Func_8017658(msg, bx, by, (voice << 16) | 1);
            }
        }
        (void)*(volatile unsigned char *)(A + 0xea4);
        g = _Func_8019da8(a, 0, bx2, t);
        while (_Func_8017364() == 0) {
            WaitFrames(1);
        }
    }
    *(int *)(p + (0xfc << 1)) = h;
    *(int *)(p + (0xfe << 1)) = g;
    (*(short *)(p + (0xec << 1)))++;
    return h;
}

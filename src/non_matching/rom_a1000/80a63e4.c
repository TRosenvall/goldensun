/* Func_80a63e4 -- RunItemDetailLoop -- NON-MATCHING: 201 encodings of 226 differ (objcmp).
 * Size does NOT match: ref 512 bytes / 226 encodings, ours 500 / 219, so the
 * difference count is NOT a distance to exact -- ours is EIGHT INSTRUCTIONS SHORT
 * (ROM 213 insns, ours 205).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a63e4.c \
 *     asm/rom_a1000/rom_a5534_c_c_a_a.s --func Func_80a63e4
 *
 * FRESH TARGET (batch 290), ONE candidate plus one revision -- this is a first
 * structural reading, not a worked park. The .s holds four functions
 * (Func_80a63e4, Func_80a65e4, Func_80a6614, Func_80a6794); datacheck.py clean.
 *
 * VERIFICATION SHIM, scratch only: `__asm__(".equ _MSG_53a, 0x53a")`. _MSG_53a is
 * already admitted in message.sym:262 and the use here is the same
 * `(id & 0x3fff) + 0x53a` into _Func_801e7c0 shape.
 *
 * THE STRUCTURE IS READ AND BELIEVED CORRECT: a `while (_GetFlag(0x150) == 0)`
 * loop entered at the BOTTOM TEST (the ROM's `b .La6582` past the body), a
 * `moved` latch that gates the redraw half, `idx = (idx + count) % count` through
 * __modsi3 with `count = state[0x219]`, two stack flags (`ret` at sp+4, `drawn`
 * at sp) and the A/B/left/right key block. Two `goto done` exits out of the key
 * block reach the shared teardown.
 *
 * WHAT THE SECOND CANDIDATE FIXED, and it is the transferable part: THE ROSTER
 * OFFSET MUST BE A LIVE `int` VARIABLE, not an inline expression. The ROM keeps
 * `state` in r6 and `idx*2 + 0x208` in r7 and addresses the roster with the
 * REG+REG form `ldrh r0,[r6,r7]`; written inline, fold adds `state` FIRST and
 * collapses the address into one register. `ofs = idx * 2 + 0x208;` after the
 * modulo, with `*(unsigned short *)(state + ofs)` at all four uses, took 231
 * instructions/222 differing to 205/201. This is the SAME lever that closed
 * Func_80a6b64 exactly in this batch, except that there the winning operand order
 * was `ofs + (int)state` (offset as the addressing base) and here it is
 * `state + ofs`; the ROM's `ldrh` operand order is what decides which.
 *
 * WHAT IS STILL MISSING, eight instructions' worth:
 *  1. THE PRE-LOOP ROSTER READ STILL FOLDS. The ROM has
 *     `mov r3,#0x1c / ldrsb r3,[r6,r3] / sub r2,#0x11 / lsl r3,#1 / add r3,r2 /
 *     ldrh r0,[r6,r3]` -- the constant 0x208 DERIVED from the 0x219 already in r2
 *     by reload_cse_move2add, then reg+reg addressing. We reproduce the move2add
 *     chain (`sub r1,#0x11` is present) but then add `state` into the offset. It
 *     is a single-use address, so the live-variable trick above does not apply;
 *     the next thing to try is a second named offset local for it.
 *  2. `idx * 2` IS ITS OWN VALUE IN THE ROM, held in r10 and RECOMPUTED on each
 *     path where idx changes (three `lsl rX, idx, #1 / mov r10, rX` sites: entry,
 *     the moved==0 arm, and the loop exit). It feeds both `(idx*2 + idx) * 8` and
 *     the roster offset. Try naming it.
 *  3. REGISTER ROLES: ROM idx r8, idx*2 r10, ofs r7, state r6, spr r5,
 *     moved r9, count r11. Ours rotates idx to r7 and swaps ofs/idx*2 between
 *     r8 and r10, which is what most of the 201 differing encodings are.
 *
 * ESTABLISHED AND WORTH KEEPING: `extern volatile unsigned int gKeyPress;` and
 * `extern volatile unsigned int gKeyRepeat;` -- the ROM reads each register TWICE
 * off one held address (`ldr r1,=gKeyPress / ldr r3,[r1] ... ldr r3,[r1]`), which
 * is the established volatile idiom from
 * src/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_c_b.c and costs no fakematch row.
 * The sprite writes want an `unsigned short` carrier: the ROM's `ldr r3,=0xffff /
 * and r2,r3` between `strh r2,[r5,#6]` and the `& 0x1ff` is the HImode truncation
 * of one local reused for both stores.
 *
 * NOT TRIED AT ALL: any callee declared `int` instead of `void`; the loop as a
 * `goto` shape rather than `while`; separate locals for the two `idx*2 + idx`
 * cursor-position computations.
 */
__asm__(".equ _MSG_53a, 0x53a");

struct Spr {
    unsigned char pad_00[5];
    unsigned char f05;
    unsigned short f06;
    unsigned char pad_08[0x16 - 0x08];
    unsigned short f16;
};

extern unsigned char *iwram_3001f2c;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;
extern void *_GetUnit(int id);
extern int _GetFlag(int id);
extern void _ClearFlag(int id);
extern void _PlaySound(int sfx);
extern void WaitFrames(int n);
extern void _Func_8016498(unsigned int win);
extern void _Func_801e7c0(int id, unsigned int win, int x, int y);
extern void Func_80a1ac0(int x, int y);
extern void Func_80a1a40(int x, int y);
extern void Func_80a112c(unsigned int win, int id, int a, int b);
extern void Func_80a1804(unsigned char *state, int id);
extern void Func_80a17c4(struct Spr *s);
extern int _MSG_53a;

int Func_80a63e4(int quiet)
{
    unsigned char *state;
    struct Spr *spr;
    int idx;
    int count;
    int ret;
    int drawn;
    int moved;
    unsigned short v;
    int ofs;

    state = iwram_3001f2c;
    idx = *(signed char *)(state + 0x1d);
    count = state[0x219];
    ret = 0;
    drawn = 0;
    moved = 1;
    _GetUnit(*(unsigned short *)(state + *(signed char *)(state + 0x1c) * 2 + 0x208));
    Func_80a1ac0((idx * 2 + idx) * 8 - 0xa, 0x10);
    while (_GetFlag(0x150) == 0) {
        if (moved != 0) {
            moved = 0;
            idx = (idx + count) % count;
            ofs = idx * 2 + 0x208;
            _GetUnit(*(unsigned short *)(state + ofs));
            spr = *(struct Spr **)(state + 0x18);
            v = (*(unsigned short *)(*(int *)(state + 0x10) + 0xc) + (idx * 2 + idx)) * 8 - 2;
            spr->f06 = v;
            spr->f16 = (spr->f16 & ~0x1ff) | (v & 0x1ff);
            if (quiet == 0) {
                Func_80a112c(*(unsigned int *)(state + 0x24),
                             *(unsigned short *)(state + ofs), 0, 0);
                Func_80a1804(state, *(unsigned short *)(state + ofs));
                if (_GetFlag(0x151) == 0 && drawn == 0) {
                    _Func_8016498(*(unsigned int *)(state + 0x2c));
                    _Func_801e7c0((*(unsigned short *)(state + 0xbc * 2) & 0x3fff)
                                      + (int)&_MSG_53a,
                                  *(unsigned int *)(state + 0x2c), 0, 0);
                    drawn = 1;
                } else {
                    _ClearFlag(0x151);
                }
            }
        }
        Func_80a1a40((idx * 2 + idx) * 8 - 0xa, 0x10);
        WaitFrames(1);
        if (gKeyPress & 1) {
            _PlaySound(0x70);
            ret = *(unsigned short *)(state + ofs);
            goto done;
        }
        if (gKeyPress & 2) {
            _PlaySound(0x71);
            ret = -1;
            goto done;
        }
        if (gKeyRepeat & 0x20) {
            _PlaySound(0x6f);
            idx--;
            moved = 1;
        }
        if (gKeyRepeat & 0x10) {
            _PlaySound(0x6f);
            idx++;
            moved = 1;
        }
    }
done:
    ofs = idx * 2 + 0x208;
    spr = *(struct Spr **)(state + 0x18);
    state[0x1d] = idx;
    Func_80a17c4(spr);
    spr->f05 = 0xd;
    WaitFrames(1);
    state[0x1d] = idx;
    *(int *)(state + 8) = *(unsigned short *)(state + ofs);
    state[0x21b] = *(unsigned short *)(state + ofs);
    return ret;
}

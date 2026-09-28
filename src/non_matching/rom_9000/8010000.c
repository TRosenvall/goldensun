/* UpdateFieldScreen (0x08010000) -- NON-MATCHING: 252 encodings of 266 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/UpdateFieldScreen.c asm/rom_9000/rom_f9cc_c.s \
 *       --func UpdateFieldScreen
 *
 * 252 IS NOT A DISTANCE: size is 560 against 552 and the instruction count 266
 * against 262, so the two streams are misaligned and the raw encoding count is
 * dominated by the offset shift.  THE REAL MEASURE IS A REGISTER-BLIND ALIGNED
 * DIFF: 231 of the ROM's 266 instructions match by mnemonic and immediate once
 * register names are erased, `mov rLO,rLO` and `adds rLO,rLO,#0` are folded
 * together (they are the same halfword) and pc-relative loads are normalised.
 * Measured with scratch_elev/b291/D/dis5.sh, a hand-rolled shim named here
 * because it is not a project tool.  RELOCATIONS ARE THE RIGHT NINE IN THE RIGHT
 * ORDER (Random x4, UpdateScreenEdge_H, UpdateScreenEdge_V, iwram_3001e70,
 * Func_8000888, iwram_3001ad0); only their offsets shift.
 *
 * ====================================================================
 * BLOCKER CLASS: REG_ALLOC_ORDER, AND IT IS QUANTIFIED RATHER THAN GUESSED.
 * ====================================================================
 *
 * THE ROM PUTS THE MAP-STATE POINTER IN r8 AND PAYS FOR IT.  Thumb cannot use a
 * high register as a load base, so every one of its accesses at an offset that
 * fits the `ldr rd,[rn,#imm]` immediate costs an extra copy:
 *
 *     rom    mov r2, r8 / ldr r2, [r2, #4]
 *     ours   ldr r2, [r6, #4]
 *
 * There are THIRTEEN `mov rN, r8` copies in the ROM's 266 instructions, and the
 * difference between the two streams is almost exactly the subset of them whose
 * offset would have fitted an immediate: 266 against 262.  EVERY ONE OF THOSE IS
 * PURE COST.  The register-blind diff's 35 unmatched reference lines are these
 * copies plus the high-register `add rN, r8` form they force in place of
 * `adds rN, rM, rK`; there is no third kind of difference in the list.
 *
 * So this is not a park that a spelling can move.  A source shape can change
 * which VALUES compete, but it cannot ask gcc to spend two instructions where one
 * will do, and gcc's global-alloc priority (`floor_log2(n_refs) * n_refs /
 * live_length`) puts the state pointer -- roughly twelve references over the
 * pre-loop range -- above the two camera coordinates, which is why it wins a low
 * register here and loses one in the ROM.  This is the class HANDOFF.md names
 * under "A named mechanism for the register-allocation parks: REG_ALLOC_ORDER",
 * and it is the same finding as the file-mate park
 * src/non_matching/rom_9000/800fec8.c reached from the other side (there the ROM
 * spends ip and lr on values gcc keeps in r4/r5).  THREE OF THIS FILE'S FOUR
 * FUNCTIONS ARE NOW PARKED ON ONE COMPILER PROPERTY.
 *
 * WHAT IS RIGHT, and the reading is believed complete:
 *   - `iwram_3001e70` is a POINTER VARIABLE; the map state is its value.  The
 *     per-layer configuration block is state + 0x104, stride 0x30, three layers.
 *   - The camera comes from `*(int **)st` walked with `*p++` (the ROM's `ldmia
 *     r1!, {r3}`), and the null test on it is the early return.
 *   - The six clamps are `if (lo > hi) hi = lo;` on each axis followed by the two
 *     bounds tests on the camera value -- the ROM's exact `cmp/ble/mov` chain.
 *   - The two shake blocks are `Random() - Random()` scaled by the per-axis
 *     amplitude at st+4 / st+8 through `fx32_multiply`, with the amplitude decayed
 *     by st+0xc; the second block re-reads st+8 because the first block wrote it.
 *   - `fx32_multiply` from include/math.h: `.call_via r3` in the hand-written asm
 *     is `.align 2,0 / mov r12,pc / bx reg` (include/macros.inc:64), which is that
 *     inline asm exactly.  Its `.call_via r9` inside the loop is gcc keeping the
 *     one inlined function-pointer local in a callee-saved register across the two
 *     uses -- automatic, no source lever.
 *   - The per-layer scroll wrap is `a &= (*(u16 *)(cfg + 0x28) << 19) | 0x7ffff`
 *     and the screen coordinate is `a / 0x80000` (the `cmp/bge/add 0x7ffff/asr 19`
 *     rounding chain).
 *   - The edge redraws fire on `((old ^ a) & 0x80000)` and `((old ^ b) & 0x100000)`
 *     -- a metatile-crossing test -- with the +0x1e / +0x14 offset chosen by
 *     `old < a`.  The ROM's two call sites per axis merged into one `bl` is gcc's
 *     cross-jumping and needs no help.
 *   - `void UpdateFieldScreen(void)`: `pop {r0} / bx r0` in the epilogue clobbers
 *     r0, so there is no return value (the recorded `pop {r1}` = `int` rule, read
 *     the other way).
 *   - `iwram_3001ad0[(3 - i) * 2]` / `[... + 1]` for the recorded position pair.
 *
 * MEASURED NEGATIVES: naming the two `st + 0xe4` / `st + 0xe8` pointers explicitly
 * is inert (gcc builds them anyway, and the loop's re-read through them is FORCED
 * -- UpdateScreenEdge_H/V are calls, so gcc cannot cache the values); caching
 * st+0xc in a local is 262 instead of 260 and does not move the allocation;
 * declaring st and cfg last is inert.
 *
 * NEXT: nothing source-level.  This one belongs to the REG_ALLOC_ORDER experiment
 * in HANDOFF.md -- rebuild gcc-2.96 with the order starting at r4 and re-screen.
 */
#include "gba/types.h"
#include "math.h"

extern unsigned char *iwram_3001e70;
extern short iwram_3001ad0[];
extern u32 Random(void);
extern void UpdateScreenEdge_V(int page, int x, int y);
extern void UpdateScreenEdge_H(int page, int x, int y);

void UpdateFieldScreen(void)
{
    unsigned char *st;
    unsigned char *cfg;
    int *cam;
    int cx;
    int cz;
    int sx;
    int sz;
    int lox;
    int hix;
    int loz;
    int hiz;
    int a;
    int b;
    int t;
    int old;
    int X;
    int Y;
    int i;

    st = iwram_3001e70;
    cam = *(int **)st;
    cfg = st + 0x104;
    if (cam == 0)
        return;
    cx = *cam++ + 0xff880000;
    t = *cam++;
    cz = *cam - t + 0xffa00000;
    sx = *(int *)(st + 4);
    lox = *(int *)(st + 0xec) + sx;
    hix = *(int *)(st + 0xf4) - sx + 0xff100000;
    sz = *(int *)(st + 8);
    loz = *(int *)(st + 0xf0) + sz;
    hiz = *(int *)(st + 0xf8) - sz + 0xff600000;
    if (lox > hix)
        hix = lox;
    if (loz > hiz)
        hiz = loz;
    if (cx < lox)
        cx = lox;
    if (cx > hix)
        cx = hix;
    if (cz < loz)
        cz = loz;
    if (cz > hiz)
        cz = hiz;
    if (sx != 0) {
        t = Random();
        t = t - Random();
        sx = *(int *)(st + 4);
        cx += fx32_multiply(sx, t);
        *(int *)(st + 4) = fx32_multiply(sx, *(int *)(st + 0xc));
        sz = *(int *)(st + 8);
    }
    if (sz != 0) {
        t = Random();
        t = t - Random();
        sz = *(int *)(st + 8);
        cz += fx32_multiply(sz, t);
        *(int *)(st + 8) = fx32_multiply(sz, *(int *)(st + 0xc));
    }
    *(int *)(st + 0xe4) = cx;
    *(int *)(st + 0xe8) = cz;
    i = 0;
    do {
        a = fx32_multiply(*(int *)(st + 0xe4), *(int *)(cfg + 0x10));
        b = fx32_multiply(*(int *)(st + 0xe8), *(int *)(cfg + 0x14));
        if (*(int *)(cfg + 0x18) != 0) {
            t = *(int *)(cfg + 0x20) + *(int *)(cfg + 0x18);
            a += t;
            *(int *)(cfg + 0x20) = t;
            a &= (*(unsigned short *)(cfg + 0x28) << 19) | 0x7ffff;
        }
        if (*(int *)(cfg + 0x1c) != 0) {
            t = *(int *)(cfg + 0x24) + *(int *)(cfg + 0x1c);
            b += t;
            *(int *)(cfg + 0x24) = t;
            b &= (*(unsigned short *)(cfg + 0x2a) << 19) | 0x7ffff;
        }
        a += *(int *)(cfg + 8);
        b += *(int *)(cfg + 0xc);
        X = a / 0x80000;
        Y = b / 0x80000;
        old = *(int *)cfg;
        if (((old ^ a) & 0x80000) != 0) {
            if (old < a)
                UpdateScreenEdge_H(i, X + 0x1e, Y);
            else
                UpdateScreenEdge_H(i, X, Y);
        }
        old = *(int *)(cfg + 4);
        if (((old ^ b) & 0x100000) != 0) {
            if (old < b)
                UpdateScreenEdge_V(i, X, Y + 0x14);
            else
                UpdateScreenEdge_V(i, X, Y);
        }
        iwram_3001ad0[(3 - i) * 2] = a >> 16;
        iwram_3001ad0[(3 - i) * 2 + 1] = b >> 16;
        i++;
        *(int *)cfg = a;
        *(int *)(cfg + 4) = b;
        cfg += 0x30;
    } while (i <= 2);
}

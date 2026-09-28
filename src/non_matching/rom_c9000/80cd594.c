/* AnimStart  [rom_c9000]  --  asm/rom_c9000/rom_cd508_a_c_a.s
 *
 * NON-MATCHING: 291 encodings of 316 differ (objcmp).
 * Size 720 against the ROM's 728 (-8); 312 instructions against 316 (-4).
 * THE RELOCATION SEQUENCE IS IDENTICAL -- 17 symbols in the ROM's exact order.
 * The only symbol that differs is `_call_via_r5` against our `_call_via_r6`,
 * which is a real register difference (see the blocker), not a spelling.
 * 291 is NOT a distance: the four-instruction deficit renames everything after
 * the first queue push.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/cd594_AnimStart.c \
 *     asm/rom_c9000/rom_cd508_a_c_a.s --func AnimStart
 *
 * ================================================================
 * FIRST: THREE CLAIMS IN src/non_matching/rom_c9000/AnimEnd.c ABOUT THIS
 * FUNCTION ARE STALE OR WRONG, AND THE PARK SHOULD BE CORRECTED
 * ================================================================
 *
 * AnimEnd.c says: "AnimStart and AnimStart2 in this same .s are structurally
 * right and far out (244 of 284, 237 of 274) ... they will not close until this
 * one does.  Do AnimEnd first; the other two are mechanical from it."
 *
 *   1. THEY ARE NOT IN THE SAME `.s` ANY MORE.  AnimEnd now lives in
 *      asm/rom_c9000/rom_cd508_a_c_c.s; AnimStart and AnimStart2 are in
 *      rom_cd508_a_c_a.s together with Func_80cd52c.  A split happened after that
 *      note was written and the note was not repointed.
 *   2. THE COUNTS DO NOT MATCH ANY CURRENT MEASUREMENT.  The `.s` banners say
 *      276 and 266 instructions; objcmp says 316 and (unmeasured) for the
 *      reference encodings.  284/274 is neither, so those figures were taken on a
 *      different basis and cannot be compared with anything today.
 *   3. THE DEPENDENCY CLAIM IS FALSE AS STATED.  AnimStart's queue push is NOT
 *      AnimEnd's inline: AnimEnd's reads the count as an `s32` and guards
 *      `count < 32`; AnimStart's reads it with `ldrh` and guards
 *      `cmp r2,#0x1f / bgt`, i.e. `count <= 0x1f` on an `int`, and it writes the
 *      two task words with `stmia r3!, {r2}` rather than `str`.  Written that way
 *      ALL FOUR of AnimStart's pushes come out instruction-for-instruction on the
 *      first candidate, with AnimEnd still parked.  The two functions do not
 *      block each other; what is left here is unrelated to AnimEnd's residue.
 *
 * ================================================================
 * WHAT IS RIGHT ON THE FIRST CANDIDATE
 * ================================================================
 *
 * The four queue pushes, the entire register block
 * (REG_BLDCNT 0x3f44 / REG_BLDALPHA 0x100e / REG_BG2X 0 / REG_BG2Y 0xfffff000 /
 * REG_BG2PA 0x80 / REG_BG2PB 0 / REG_BG2PC 0 / REG_BG2PD 0x100 /
 * REG_WIN0H 0xf0 / REG_WIN0V 0x1088 / REG_WIN1H 0xf0 / REG_WIN1V 0x1088 /
 * REG_WININ 0x3537 / REG_WINOUT 0x3f21 -- read off the ROM's `add r2,#2`,
 * `sub r2,#0xc`, `add r3,#4`, `sub r3,#2` chains from REG_BLDCNT and REG_WIN0H),
 * the tile loop, and the tail are all in place.
 *
 * NOTES WORTH KEEPING:
 *   - EVERY HALFWORD CONSTANT STORE HERE IS A POOL LOAD, INCLUDING 0x80.  The ROM
 *     has `ldr r3, .Lcd730 @ 0x80` for REG_BG2PA even though `mov r3,#0x80` would
 *     fit, so this is not a "large constant" effect: a plain `REG_x = K;` on a
 *     `vu16` always gives gcc's `ldrh rX, label`, which ASSEMBLES IDENTICALLY to
 *     `ldr rX, label` (thumb has no PC-relative ldrh).  All five mid-function pools
 *     with a `b` over them follow from the narrow HImode fixup range.
 *   - `view` IS REACHED AS A NEGATIVE OFFSET FROM THE SAME SYMBOL:
 *     `*(u8 **)((char *)iwram_3001eec - 0x78)` for the ROM's
 *     `mov r2,r3 / sub r2,#0x78 / ldr r2,[r2]` -- one pool word plus a runtime
 *     subtract, the batch-290 "one symbol plus an offset" rule.
 *   - `st = (int *)iwram_3001eec[5]; st[3] = 1;` for `ldr r3,[r3,#0x14]` /
 *     `str r3,[r2,#0xc]`.
 *   - THE TILE LOOP KEEPS BASE AND OFFSET IN SEPARATE REGISTERS
 *     (`mov r3,r8 / add r2,r5,r3 / strh`), so it is written
 *     `*(unsigned short *)(vram + off) = v; off += 2;` with `vram` a local, not a
 *     pointer walk; and the stored value goes through a `short` LOCAL, which is
 *     what keeps the ROM's `lsl r3,#16 / asr r3,#16` narrowing alive.
 *   - THE THREE SHIFTED CONSTANTS IN THE TILE LOOP ARE HOISTED ASYMMETRICALLY:
 *     `0x80 << 2` (used at depth 2) goes to r14, `0x80 << 1` (depth 1) to r10, and
 *     `0x80 << 5` (also depth 1) is REBUILT every outer iteration.  That is the
 *     lifetime-asymmetry signature of the two-stage `move_movables` hoist, and it
 *     comes out right from the plain shift spellings -- do not reach for locals.
 *   - gcc-2.96 thumb really does allocate r12 and lr here (confirmed by AnimEnd.c's
 *     probe and by our own output), so the tile loop is ordinary allocation.
 *
 * ================================================================
 * THE BLOCKER: THE FOUR PUSHES' CONSTANTS ARE CSE'd ACROSS THE WHOLE FUNCTION
 * ================================================================
 *
 * The ROM rebuilds `mov r2,#0x80 / lsl r2,#0x13` (the 0x80<<19 destination) AND
 * reloads `ldr r2,=0x7741` / `=0x7341` AT EVERY ONE of the four push sites.  gcc
 * builds 0x80<<19 once, parks it in r11 and copies (`mov r2,r11`), and keeps the
 * two source words in r10 across the calls.  That costs the four instructions and
 * the two extra frame words (0x8 against the ROM's 0x4), and it is what pushes the
 * blit pointer from r5 to r6 (`_call_via_r6`).
 *
 * reports/arg-interleave.md's constant-CSE remedy -- "give each occurrence its own
 * named local in a dominating block" -- is MEASURED INERT HERE (u32 d1..d4 assigned
 * immediately before each push: exactly 291 of 316, unchanged).  The reason is in
 * that report's own limit clause read the other way: the remedy works when the
 * local's single use can be rematerialised, and here each local is live across two
 * or three intervening calls, so global_alloc gives it a register instead.  A
 * `-fno-gcse` row would be the honest next experiment, and per docs/elevation.md a
 * flag row is only admissible once spellings provably cannot differ -- which is not
 * yet established for the inline's own shape.
 *
 * INERT: per-site `u32` destination locals (above).  NOT YET TRIED: making the push
 * a macro rather than a `static inline` (same RTL, so unpromising); giving each
 * push its own copy of the inline body; `-fno-gcse`.
 *
 * AnimStart2 at 0x080cd86c in the same `.s` is 266 instructions and is the same
 * function with different constants -- it was NOT opened in batch 291.  Everything
 * above should transfer; do AnimStart's CSE question first.
 *
 * No new symbol is warranted.
 */
#include "gba/types.h"
#include "gba/io.h"

typedef void (*ClearFn)(void *dst, s32 len);

struct DmaTransfer {
    const void *src;
    void *dest;
    u32 control;
};

struct DmaQueue {
    u16 count;
    struct DmaTransfer tasks[32];
};

extern struct DmaQueue gDMATaskCount;
extern int *iwram_3001eec[];
extern unsigned short iwram_3001ad0[];

extern void Func_80cd508(void);
extern void WaitFrames(unsigned int n);
extern int _Func_80c0774(int a, unsigned short b, int c);
extern void StartTask(void *fn, int arg);
extern void Func_80cd4b4(void);
extern int _Func_80c0cec(int a, int b, int c, int d);
extern void Func_80008d4(void *dst, s32 len);

static inline void QueuePush(u32 src, u32 dest)
{
    struct DmaQueue *queue;
    u32 savedIme;
    int count;
    u32 *task;

    queue = &gDMATaskCount;
    savedIme = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    count = queue->count;
    if (count <= 0x1f) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = src;
        *task++ = dest;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

void AnimStart(int mode)
{
    unsigned char *base;
    unsigned char *view;
    void *arg;
    int *st;
    unsigned char *vram;
    ClearFn clear;
    int off;
    int row;
    int col;
    int n;
    int m;
    int a;
    int b;

    base = (unsigned char *)iwram_3001eec[0];
    view = *(unsigned char **)((char *)iwram_3001eec - 0x78);
    arg = (void *)iwram_3001eec[1];
    st = (int *)iwram_3001eec[5];
    Func_80cd508();
    st[3] = 1;
    WaitFrames(1);
    REG_BLDCNT = 0;
    QueuePush(0x7741, 0x80 << 19);
    iwram_3001ad0[3] = 0x20;
    WaitFrames(1);
    _Func_80c0774(1, *(unsigned short *)(view + (0xc9 << 3)), 0);
    *(int *)(base + 0x77b4) = 0;
    *(int *)(base + 0x77b8) = 0;
    StartTask(Func_80cd4b4, 0xc8 << 4);
    QueuePush(0x7341, 0x80 << 19);
    WaitFrames(1);
    REG_BG2CNT = mode | 0x784;
    QueuePush(0x7341, 0x80 << 19);
    _Func_80c0cec(0, 0, 0, 0x64);
    st[3] = 0;
    WaitFrames(1);
    REG_BLDCNT = 0x3f44;
    REG_BLDALPHA = 0x100e;
    REG_BG2X = 0;
    REG_BG2Y = 0xfffff000;
    REG_BG2PA = 0x80;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = 0x100;
    REG_WIN0H = 0xf0;
    REG_WIN0V = 0x1088;
    REG_WIN1H = 0xf0;
    REG_WIN1V = 0x1088;
    REG_WININ = 0x3537;
    REG_WINOUT = 0x3f21;
    QueuePush(0x7741, 0x80 << 19);
    off = 0;
    vram = (unsigned char *)0x6003800;
    n = 0;
    col = 0;
    row = 0;
    do {
        a = row + (0x80 << 1);
        m = 0;
        b = col << 1;
        do {
            short v = a | b;
            *(unsigned short *)(vram + off) = v;
            a += 0x80 << 2;
            b += 2;
            off += 2;
            m++;
        } while (m != 8);
        row += 0x80 << 5;
        n++;
        col += 8;
    } while (n != 0x10);
    clear = Func_80008d4;
    clear(arg, 0x80 << 7);
    clear((void *)0x6004000, 0x80 << 7);
    *(int *)(base + 0x77a8) = 0;
    *(int *)(base + 0x77a0) = iwram_3001ad0[2];
    *(int *)(base + 0x77a4) = iwram_3001ad0[3];
    WaitFrames(1);
}

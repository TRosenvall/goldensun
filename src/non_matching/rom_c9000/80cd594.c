/* AnimStart  [rom_c9000]  --  asm/rom_c9000/rom_cd508_a_c_a.s  @ 0x080cd594
 *
 * PARK UPDATE (batch 292, brief E).  BONUS -- this function was not a target;
 * it is target 3's (AnimStart2's) file-mate, and the levers found on AnimStart2
 * transfer to it.  It supersedes src/non_matching/rom_c9000/80cd594.c.
 *
 * NON-MATCHING: 71 encodings of 316 differ (objcmp).
 * WAS 291 of 316.  SIZE IS NOW EXACT (728 bytes) AND THE INSTRUCTION COUNT IS
 * EXACT (316 against 316); the previous candidate was 720 bytes and 312
 * instructions.  71 is therefore a TRUE DISTANCE IN AGGREGATE, but not a clean
 * one: two relocation OFFSETS are still shifted (gDMATaskCount's pool word by +4
 * and the first indirect call by +2), so there are local length shifts that
 * cancel out.
 * CORRECTION TO A CLAIM I FIRST WROTE HERE AND THEN CHECKED: `_call_via_r5` is
 * NOT reached.  Both indirect calls are still `_call_via_r6`, so the register
 * difference the old park named as "a real register difference, not a spelling"
 * SURVIVES -- freeing r11 was necessary but not sufficient.  It is the same
 * register the tile loop's third hoisted shift is contending for (component 1
 * below), which is consistent with that being the one remaining cause.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/80cd594.c \
 *     asm/rom_c9000/rom_cd508_a_c_a.s --func AnimStart
 *
 * ================================================================
 * THE OLD PARK'S NAMED BLOCKER IS RETIRED, AND ITS "UNPROMISING" NOTE WAS WRONG
 * ================================================================
 *
 * The old header named the blocker as "THE FOUR PUSHES' CONSTANTS ARE CSE'd
 * ACROSS THE WHOLE FUNCTION", recorded per-site named locals as measured inert,
 * and listed "giving each push its own copy of the inline body" as NOT YET TRIED,
 * dismissing it as "same RTL, so unpromising".  It is the lever:
 *
 *   one parameterised `static inline QueuePush(u32 src, u32 dest)`  291 of 316
 *   four `static inline QueuePush1..4(void)`, literals inside each  107 of 316
 *   + the `vu16` store on the tile loop's `short` (below)            76 of 316
 *   + three callee/argument spellings (below)                        71 of 316
 *
 * MECHANISM (not "the same RTL"): with the constant passed as a PARAMETER, the
 * inliner lands `(set (pseudo) (const_int K))` at the top of each inlined block
 * and cse2 (-frerun-cse-after-loop) equates the four pseudos, parking 0x80<<19 in
 * r11 and copying.  Written as a literal INSIDE each body it expands straight into
 * the store insn at that site and cse2 leaves all four alone.  Confirmed by the
 * flag: the single-inline form rebuilds all four sites only under
 * -fno-rerun-cse-after-loop.  Freeing r11 is what lets `st` take r11 and the blit
 * pointer take r5, which is the `_call_via_r5` relocation.
 *
 * AND THE OLD PARK'S TILE-LOOP NOTE IS ALSO WRONG.  It says the `short` narrowing
 * "comes out right from the plain shift spellings -- do not reach for locals".  It
 * does not: gcc drops the ROM's `lsl r3,#16 / asr r3,#16` because `strh` truncates
 * anyway.  The store must be VOLATILE as well as the local `short`:
 * `short v = a | b; *(vu16 *)(vram + off) = v;` is 107 -> 76.  Neither half alone
 * does it (seven spellings measured on AnimStart2; only this one keeps the pair).
 *
 * THREE CALLEE/ARGUMENT SPELLINGS, additive, 76 -> 71:
 *   `_Func_80c0774` declared `void`, not `int`                 76 -> 74
 *   `targ = 0xc8; targ <<= 4; StartTask(Func_80cd4b4, targ);`
 *       as its own statements rather than `0xc8 << 4` inline   76 -> 74
 *   `Func_80cd508` declared `int`, not `void`                  76 -> 75
 *   all three                                                  76 -> 71
 * `WaitFrames` declared `int` is INERT (76, unchanged).
 * The int-return lever runs in OPPOSITE directions on two callees of the same
 * function here, which is the per-callee direction rule confirmed inside one TU.
 *
 * ================================================================
 * WHAT IS LEFT, in three named components
 * ================================================================
 *
 * 1. loop.c move_movables HOISTS ONE SHIFT TOO MANY, and it carries an extra
 *    frame word.  The ROM hoists `0x80<<1` (r10) and `0x80<<2` (r14) out of the
 *    tile loop and REBUILDS `0x80<<5` every outer iteration; we hoist all three,
 *    the third into r8, which is the register the ROM gives the VRAM base.  The
 *    same asymmetry is the whole residue of AnimStart2 (see its park); the old
 *    header's claim that it "comes out right from the plain shift spellings" was
 *    written against a candidate that was four instructions short elsewhere.
 *    Here it also spills the shared zero (`str r6,[sp]`), which is why our frame
 *    is `sub sp,#8` against the ROM's `sub sp,#4`, and it is what still forces
 *    the blit pointer to r6 (`_call_via_r6`) instead of the ROM's r5.
 *
 * 2. THE FIVE MID-FUNCTION LITERAL POOLS ARE DUMPED AS ONE BLOCK EACH where the
 *    ROM splits them into individually labelled `.word`s followed by `.pool`.
 *    Per the old park's own note this is spelling, not bytes (Thumb has no
 *    PC-relative `ldrh`, so `ldrh rX,label` and `ldr rX,label` assemble
 *    identically) -- but the WORD ORDER inside each pool differs and that does
 *    move the PC-relative offsets.  Fixing component 1 is the prerequisite.
 *
 * 3. PROLOGUE SCHEDULE SWAPS: `mov r10,r2` (view) is sunk past `mov r7,r0`;
 *    `mov r3,#1 / mov r2,r11` comes out in the opposite order; REG_BLDCNT's
 *    address and REG_IME's share r5 in ours where the ROM uses r2 and r5.
 *
 * ================================================================
 * SPLIT SHAPE (unchanged from the old park, restated because it was got wrong once)
 * ================================================================
 * asm/rom_c9000/rom_cd508_a_c_a.s holds THREE functions (`grep -ci func_start` = 3):
 * Func_80cd52c @ 0x080cd52c, AnimStart @ 0x080cd594, AnimStart2 @ 0x080cd86c, and
 * NO data section.  AnimStart is the MIDDLE function, so landing it is a middle
 * cut and NO LABEL CROSSES THE BOUNDARY IN EITHER DIRECTION -- every `.L*` in its
 * range is an in-body branch target or a mid-function pool gcc regenerates.  ZERO
 * `.global` exports are required.  AnimEnd.c's claim that AnimStart and AnimEnd
 * share this `.s` is stale (AnimEnd is in rom_cd508_a_c_c.s) and its dependency
 * claim is false; both corrections stand from batch 291.
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

extern int Func_80cd508(void);
extern void WaitFrames(unsigned int n);
extern void _Func_80c0774(int a, unsigned short b, int c);
extern void StartTask(void *fn, int arg);
extern void Func_80cd4b4(void);
extern int _Func_80c0cec(int a, int b, int c, int d);
extern void Func_80008d4(void *dst, s32 len);

static inline void QueuePush1(void)
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
        *task++ = 0x7741;
        *task++ = 0x80 << 19;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

static inline void QueuePush2(void)
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
        *task++ = 0x7341;
        *task++ = 0x80 << 19;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

static inline void QueuePush3(void)
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
        *task++ = 0x7341;
        *task++ = 0x80 << 19;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

static inline void QueuePush4(void)
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
        *task++ = 0x7741;
        *task++ = 0x80 << 19;
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
    int targ;

    base = (unsigned char *)iwram_3001eec[0];
    view = *(unsigned char **)((char *)iwram_3001eec - 0x78);
    arg = (void *)iwram_3001eec[1];
    st = (int *)iwram_3001eec[5];
    Func_80cd508();
    st[3] = 1;
    WaitFrames(1);
    REG_BLDCNT = 0;
    QueuePush1();
    iwram_3001ad0[3] = 0x20;
    WaitFrames(1);
    _Func_80c0774(1, *(unsigned short *)(view + (0xc9 << 3)), 0);
    *(int *)(base + 0x77b4) = 0;
    *(int *)(base + 0x77b8) = 0;
    targ = 0xc8;
    targ <<= 4;
    StartTask(Func_80cd4b4, targ);
    QueuePush2();
    WaitFrames(1);
    REG_BG2CNT = mode | 0x784;
    QueuePush3();
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
    QueuePush4();
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
            *(vu16 *)(vram + off) = v;
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

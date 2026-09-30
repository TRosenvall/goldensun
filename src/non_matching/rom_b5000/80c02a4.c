/* Func_80c02a4 (0x080c02a4) -- NON-MATCHING, 250 of 468 encodings differ.
 * Reference asm/rom_b5000/rom_bffb8_a_c_a_a_a.s.  Intended park path:
 * src/non_matching/rom_b5000/80c02a4.c
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_b5000/80c02a4.c \
 *       asm/rom_b5000/rom_bffb8_a_c_a_a_a.s --func Func_80c02a4
 *
 * NON-MATCHING, 250 of 468 encodings differ.
 * NOT YET A TRUE DISTANCE: 1104 bytes against 1116 and 462 encodings against
 * 468, so 250 is not a ranking figure.  tools/aligncmp.py puts 370 of 468
 * aligned-equal (79.1%), 126 differing/ins/del in 58 hunks -- that is the
 * figure to move.  Ref instruction count (hand-written .s lines) is 414.
 *
 * SPLIT: NONE NEEDED.  asm/rom_b5000/rom_bffb8_a_c_a_a_a.s holds this function
 * and its pools only, tools/datacheck.py reports no data requirement, and
 * stage1.ld:1665 names the object in .text ONLY.  It converts whole.
 * Shim count: 8 register pins, all of them the four `register ... __asm__("rN")`
 * decls in each of the two LOCAL dma.h-shaped helpers below.  If those two
 * helpers are promoted to include/dma.h (as DMA3_COPY16_RW was in batch 299)
 * the file is pin-free; as written it is a fakematch-class shim with no
 * fakematch.txt row, so it must NOT be landed in this shape.
 *
 * ================================================================
 * WHAT CLOSED, BY PASS (436 -> 295 -> 289 -> 249 -> 250/370-aligned)
 * ================================================================
 *
 * 1. DMA3_SET MUST WITHDRAW THE PROMISE ON ITS *COUNT*, not just on r0.  The
 *    ROM re-issues `ldr r2, =0x84000008` before EVERY ONE of the seven
 *    transfers while carrying the destination forward with `add r1, #0x20`.
 *    Stock DMA3_SET keeps the count in one register for all seven, which is
 *    six instructions short.  A local variant with `"+r" (_cnt)` as an output
 *    -- the legal form, per dma.h's own note on DMA3_COPY16_RW -- reloads it
 *    each time.  This is the SECOND function to need the RW form and the first
 *    to need it on DMA3_SET; promote `DMA3_SET_RW` to dma.h when it lands.
 *
 * 2. A 4-BYTE DMA FILL SLOT AT THE TOP OF THE FRAME IS A `char` ARRAY, NOT A
 *    `u32`.  The ROM's `add r4, sp, #0x90` slot is the FIRST declared local
 *    (frame 0x94; ctx 0x3c, listA 0x20, listB 0x04, arg spill 0x00).  Neither
 *    `u32 value;` nor `u32 value[1];` reaches the top: gcc-2.96 keeps a
 *    word-sized single object as a pseudo through expand_decl and only puts it
 *    in memory later, via the temp region at the BOTTOM (sp+4), and NO
 *    declaration order moves it -- value-first, value-last and value-after-ctx
 *    all give sp+4.  `unsigned char value[4];` (or `volatile u32`) is
 *    allocated at decl time and lands at 0x90, and the whole frame then agrees
 *    with the ROM offset for offset.  THIS IS A NEW COROLLARY to the
 *    declaration-order rule: the rule governs objects that get a slot at
 *    expand_decl, and a word-sized scalar is not one of them.
 *
 * 3. THE FILL POINTER MUST BE A LOCAL ASSIGNED INSIDE THE BRANCH.  Passing the
 *    array (or assigning the pointer at the top of the function) makes gcc
 *    rematerialise `add rN, sp, #144` at each use -- cheaper than the ROM, and
 *    it also hoists the address to the entry block, which shifts every pool
 *    offset and collapses the positional score to 431.  `vp = (u32 *)value;`
 *    immediately before the first fill reproduces the ROM's `add r4, sp, #144`
 *    once plus `mov r0, r4` per call, in r4, exactly.
 *
 * 4. THE 32x32 TILE FILL'S TWO COUNTERS ARE `unsigned`.  Signed counters let
 *    gcc reverse the inner loop (`movs r3, #31` / `subs r3, #1` / `bge`); the
 *    ROM counts up with `bls`, which is the unsigned compare AND the thing
 *    that makes the reversal unsafe.  Both `bls`es come from the same change.
 *
 * 5. THE TILE VALUE IS AN `int`, NOT A `u16`.  A `u16 t = 0xf080` pools
 *    SIGN-EXTENDED (`.word 0xfffff080`); the ROM's pool word is 0x0000f080,
 *    which is the SImode constant.  A pool word that is the sign-extension of
 *    the value you meant is a TYPE tell and costs nothing to check.
 *
 * 6. REG_DISPCNT = 1 IS AN SImode STORE.  The ROM materialises it with
 *    `movs r2, #1` and then reuses r2 for `g[3] = 1; g[2] = 1;`.  A plain
 *    `REG_DISPCNT = 1` is HImode, comes out as a pooled `ldrh`, and scores
 *    401/336-aligned.  SET_IO() (io.h's `unsigned __value = value;`) and an
 *    explicit `int one` local score IDENTICALLY (250 / 370) -- so the int
 *    intermediate is settled and the spelling of it is not the residue.
 *
 * 7. `id = i + 0x78; if (i <= 7) id = i;` -- NOT `id = i; if (i > 7) id += 0x78;`.
 *    The ROM computes the biased form first and overwrites it on the cheap
 *    path, which is what `bgt` over a single `mov` means.
 *
 * 8. THE PARTY-LIST GLOBAL IS REACHED THROUGH A NAMED POINTER.  The ROM loads
 *    &iwram_3001f00 once (r6) and reaches iwram_3001e74 as `sub r3, #0x8c`
 *    off it -- 0x3001f00 - 0x8c == 0x3001e74.  Spelling it
 *    `*(char **)(fp - 0x8c)` with `fp` a named `char *` removes the third
 *    iwram_3001e74 relocation; the ROM has exactly two and a literal
 *    `iwram_3001e74[0]` gives three.  This is the negative-offset lever in its
 *    corrected (named-pointer) form.
 *
 * ================================================================
 * THE BLOCKER: SIX INSTRUCTIONS OF HIGH-REGISTER TRAFFIC IN THE
 * mode == 0x15b ARM, AND A LOW-REGISTER ROTATION THAT FOLLOWS IT
 * ================================================================
 *
 * We are 6 encodings SHORT (462 of 468) and every identified deficit is the
 * same event: the ROM holds a value in r8 or r10 and pays a `mov` to get it
 * out, where we keep it in a low register and pay nothing.
 *
 *     ref  adds r0, r4, #0   (2nd DMA3_FILL)        ours reuses r0     -1
 *     ref  mov  r3, sl       (0xff for ctx.list[k]) ours keeps it low  -1
 *     ref  mov  r8, r3       (listB address)        ours uses r7       -1
 *     ref  movs r1,#0 / mov r8,r1  (0 for *h = 0)   ours makes it late -2
 *     ref  adds r7, r0, #0   (2nd Func_80b6c08)     ours reuses r0     -1
 *
 * Against it: `"r0"` added to the fill's clobber list (270), a named `int zero`
 * for `*h = 0` in both arms (403), one shared count variable for the two
 * Func_80b6c08(listB) calls (254), a named `int term = 0xff` for the two
 * terminator stores (253).  ALL FOUR ARE WORSE, so the pressure is not coming
 * from any of those four quantities and the list above is a symptom, not the
 * cause.  Per docs/elevation.md this is one missing short-lived quantity in
 * the mode == 0x15b arm; the count is the discriminator and it has not moved.
 *
 * The one instruction we are LONG is REG_DISPCNT's address: the ROM writes
 * `subs r3, #212` off the 0x40000d4 the DMA asm leaves in r3 (move2add's
 * same-register path), and we build 0x4000000 with `movs r2,#128 / lsls r2,#19`
 * because the SImode `1` took r3 first.  That is downstream of the allocation,
 * not a separate defect -- do not spend spellings on it before the count.
 *
 * WHERE THE SIBLINGS CAME FROM.  src/rom_b5000/rom_bffb8_a_c_a_a_c.c
 * (Func_80c0774, matched) supplies struct DmaTransfer / struct DmaQueue, the
 * LOCK_IME macro and the whole DMA-queue inline; this function calls the real
 * global `SetRegAnimDest` in the mode != 0x15b arm AND has the same helper
 * INLINED in the other arm, so the TU needs both an `extern` for the global and
 * a differently-named static inline (QueueRegWrite here) for the inlined copy.
 * struct ShatterCtx's size 0x54 = 84 bytes is confirmed here independently of
 * src/non_matching/rom_c9000/cb1a4_EPowerUp.c, which the Anim_MoveIntro park
 * predicted: ctx sits at sp+0x3c with its u16 list at +0x24 == sp+0x60 and the
 * frame's next slot is 0x90.
 */
#include "dma.h"
#include "gba/io.h"

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

struct ShatterCtx {
    unsigned char pad00[0x14];
    int count;
    unsigned char pad18[0x0c];
    unsigned short list[24];
};

extern unsigned char iwram_3001f00[];
extern char *iwram_3001e74[];
extern unsigned short iwram_3001ad0[];
extern unsigned char L_c5b30[] __asm__(".Lc5b30");

extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern int WaitFrames(int n);
extern int StartTask(void *f, int prio);
extern int StopTask(void *f);
extern void SetIntrHandler(int a, int b, void *f);
extern void SetRegAnimDest(int dest, int val);
extern void Func_80c0cec(int a, int b, int c, int d);
extern void Func_80c01bc(void);
extern void Func_80c0228(void);
extern void Func_80c0298(void);
extern void Func_80b595c(int a);
extern int Func_80b6c08(int kind, unsigned short *buf);
extern void CreateBattleSpriteOverlays(unsigned short *buf, int b);
extern void _Anim_ScreenShatter(struct ShatterCtx *ctx);
extern void Func_80c0f98(int id, int flag);
extern int *GetBattleActor(int id);
extern unsigned char *_GetUnit(int id);
extern void _Func_801ef08(int a);
extern void Func_80039fc(void *reg, int val);
extern void Func_800393c(void *reg, int val);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void DMA3_SET_RW(const void *src, void *dst, u32 cnt)
{
    register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
    register const void *_src  __asm__("r0") = src;
    register void *_dst  __asm__("r1") = dst;
    register u32 _cnt  __asm__("r2") = cnt;
    __asm__ volatile (
        "stmia\tr3!, {r0, r1, r2}\n\t"
        "sub\tr3, #0xc"
        : "+r" (_cnt)
        : "r" (_base), "r" (_src), "r" (_dst)
        : "memory", "r0"
    );
}

static inline void DMA3_FILL_AT(u32 *slot, u32 v, void *dst, unsigned size)
{
    *slot = v;
    {
        register u32 * _src  __asm__("r0") = slot;
        register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
        register unsigned _dst  __asm__("r1") = (unsigned)(dst);
        register unsigned _cnt  __asm__("r2") = (unsigned)(0x85000000 | (size / 4));
        __asm__ volatile (
            "stmia\t%1!, {%2, %3, %0}\n\t"
            "sub\t%1, #0xc"
            : "+l" (_cnt)
            : "l" (_base), "l" (_src), "l" (_dst)
            : "memory"
        );
    }
}

static inline void QueueRegWrite(u32 dest, u32 src)
{
    struct DmaQueue *queue = &gDMATaskCount;
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = src;
        *task++ = dest;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

void Func_80c02a4(int a, int mode)
{
    unsigned char value[4];
    struct ShatterCtx ctx;
    unsigned short listA[14];
    unsigned short listB[14];
    int *g;
    int *h;
    char *base;
    char *fp;
    u32 *vp;
    unsigned short *p;
    unsigned int x, y;
    int n, i, k;
    int id;
    int *actor;
    unsigned char *u;

    fp = (char *)iwram_3001f00;
    g = *(int **)fp;
    h = galloc_iwram(0x2a, 4);
    if (mode != 0x15b) {
        DMA3_SET_RW(L_c5b30, (void *)0x6005020, 0x84000008);
        DMA3_SET_RW(L_c5b30 + 0x20, (void *)0x6005040, 0x84000008);
        DMA3_SET_RW(L_c5b30 + 0x40, (void *)0x6005060, 0x84000008);
        DMA3_SET_RW(L_c5b30 + 0x60, (void *)0x6005080, 0x84000008);
        DMA3_SET_RW(L_c5b30 + 0x80, (void *)0x60050a0, 0x84000008);
        DMA3_SET_RW(L_c5b30 + 0xa0, (void *)0x60050c0, 0x84000008);
        DMA3_SET_RW(L_c5b30 + 0xc0, (void *)0x60050e0, 0x84000008);
        SET_IO(REG_DISPCNT, 1);
        g[3] = 1;
        g[2] = 1;
        g[4] = 0;
        vp = (u32 *)value;
        DMA3_FILL_AT(vp, 0x33333333, (void *)0x6005000, 32);
        DMA3_FILL_AT(vp, 0, (void *)0x6005100, 32);
        REG_BG1CNT = 0xc04;
        REG_BG0CNT |= 2;
        g[2] = 2;
        p = (unsigned short *)0x6006000;
        for (y = 0; y <= 0x1f; y++) {
            int t = (y <= 0x14) ? 0xf080 : 0xf088;
            for (x = 0; x <= 0x1f; x++)
                *p++ = t;
        }
        iwram_3001ad0[1] = 0x20;
        iwram_3001ad0[3] = 0x20;
        iwram_3001ad0[2] = 8;
        WaitFrames(1);
        REG_WIN0H = 0xf0;
        REG_WIN0V = 0x88;
        REG_WIN1H = 0xf0;
        REG_WIN1V = 0x88;
        REG_WININ = 0x3537;
        REG_WINOUT = 0x3f21;
        SetRegAnimDest(0x80 << 19, 0x7741);
        Func_80c0cec(0, 0, 0, 0xb4);
        *h = 0;
        StartTask(Func_80c01bc, 0xc8 << 4);
        StartTask(Func_80c0228, 0x90 << 3);
        SetIntrHandler(2, 0x20, Func_80c0298);
        iwram_3001ad0[1] = 0x20;
        WaitFrames(1);
        _Func_801ef08(iwram_3001e74[0][0x41]);
        WaitFrames(0x14);
        Func_80039fc((void *)REG_ADDR_BG0CNT, 2);
        Func_800393c((void *)REG_ADDR_BG0CNT, 0);
        Func_80b595c(a);
        StopTask(Func_80c01bc);
        StopTask(Func_80c0228);
        iwram_3001ad0[1] = 0;
        SetIntrHandler(2, 0, 0);
    } else {
        base = *(char **)(fp - 0x8c);
        g[3] = 1;
        g[4] = 0;
        n = Func_80b6c08(3, listA);
        for (i = 0; i != n; i++) {
            id = i + 0x78;
            if (i <= 7)
                id = i;
            actor = GetBattleActor(id);
            u = _GetUnit(id);
            if (u[0x94 * 2] != 0x94)
                actor[6] = 0xb333;
        }
        QueueRegWrite(0x80 << 19, 0x6041);
        WaitFrames(1);
        *(unsigned short *)(base + 0xc9 * 8) = 0x21;
        k = Func_80b6c08(2, ctx.list);
        ctx.count = k;
        ctx.list[k] = 0xff;
        CreateBattleSpriteOverlays(ctx.list, 0);
        _Anim_ScreenShatter(&ctx);
        Func_80c0cec(0, 0, 0, 0x64);
        *h = 0;
        SetIntrHandler(2, 0x20, Func_80c0298);
        WaitFrames(1);
        WaitFrames(0x14);
        _Func_801ef08(iwram_3001e74[0][0x41]);
        Func_80039fc((void *)REG_ADDR_BG0CNT, 2);
        Func_800393c((void *)REG_ADDR_BG0CNT, 0);
        REG_BLDCNT = 0x3f40;
        k = Func_80b6c08(3, listB);
        listB[k] = 0xff;
        CreateBattleSpriteOverlays(listB, 0);
        n = Func_80b6c08(1, listB);
        for (i = 0; i != n; i++)
            Func_80c0f98(listB[i], 1);
        for (i = 0; i != 0x10; i++) {
            REG_BLDALPHA = 0x1000 | i;
            WaitFrames(1);
        }
        for (i = 0; i != n; i++)
            Func_80c0f98(listB[i], 0);
        Func_80b595c(a);
        iwram_3001ad0[1] = 0;
        WaitFrames(1);
        SetIntrHandler(2, 0, 0);
    }
    SetIntrHandler(2, 0, 0);
    REG_BG1CNT = 0x1f83;
    WaitFrames(1);
    REG_BG1CNT = 0x1f83;
    REG_BG0CNT &= ~2;
    iwram_3001ad0[2] = 8;
    REG_DISPCNT = 0x1541;
    gfree(0x2a);
}

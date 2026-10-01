/* Anim_MoveIntro -- NON-MATCHING, 155 of 274 encodings differ.
 * Reference asm/rom_b5000/rom_c10e8_a_a_c_c.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching//Anim_MoveIntro.c \
 *       asm/rom_b5000/rom_c10e8_a_a_c_c.s --func Anim_MoveIntro
 *
 * A TRUE DISTANCE: 636 == 636 bytes and 274 == 274 instructions.  Shim-free.
 * BLOCKER, loop.c's move_movables: it hoists (const_int 64) -- the dump says
 * `Insn 274: regno 85 (life 7) moved to 802` -- which then needs a callee-saved
 * register, takes r11, and spills ctx.
 * 250 -> 155 came from the negative-offset lever in its CORRECTED form (a named
 * pointer local, not the (&sym)[-N] spelling -- see docs/elevation.md).
 * ITS PROSE IS WRONG ABOUT ITSELF: the reference describes one behaviour, but the
 * function is a FOUR-WAY DISPATCH on r2 (cast intro / unleash intro / _Anim_EPowerUp
 * / _Anim_DjinnSet), and the 4th parameter is used by the r2 == 2 arm only.
 * Its frame also solves an unrelated park: src/non_matching/rom_c9000/cb1a4_EPowerUp.c's
 * State should be padded to 0x54 = 84 bytes (`sub sp, #0xf0` with all four slot
 * offsets right on the first try).
  *
 * *** BATCH-305 CORRECTION: THIS FILE ATTRIBUTES A RESIDUE TO sched1, AND sched1 DOES NOT
 * *** RUN IN THIS BUILD.  Verified with -da at production flags: the dump sequence is
 * *** 17.lreg 18.greg 19.flow2 20.ce2 23.sched2 25.jump2 26.mach -- there is NO sched1 dump,
 * *** because flag_schedule_insns is off at -O2 here, so only the post-reload scheduler runs.
 * *** Re-attribute to sched2 (rank_for_schedule), to combine, or to the ALLOCATION that fixed
 * *** the order.  Relatedly, any "-fno-schedule-insns is inert" note below rules nothing out:
 * *** that flag controls a pass that never runs.  The sched2 tie-break is priority ->
 * *** dependent count (more wins) -> INSN_LUID (lower wins), and LUID preserves EXPAND order.
 * *** See "sched1 DOES NOT RUN IN THIS BUILD" in docs/elevation.md.
*/
/* Anim_MoveIntro (0x080c1798) -- NON-MATCHING.
 * NON-MATCHING: 155 encodings of 274 differ (objcmp).
 * SIZE AND INSTRUCTION COUNT BOTH MATCH -- 636 bytes against 636 and 274
 * instructions against 274 -- so 155 IS a distance.  tools/aligncmp.py puts
 * 114 instructions in disagreeing regions of 258.
 *
 * ONE FUNCTION IN asm/rom_b5000/rom_c10e8_a_a_c_c.s and tools/datacheck.py
 * reports NO data requirement, so THIS ONE NEEDS NO SPLIT AT ALL -- it can be
 * landed on its own the moment it closes.  Shim count 0 (tools/shimcount.py):
 * pin-free, no fakematch.txt row.  The relocation SETS AND COUNTS ALREADY MATCH
 * THE REFERENCE EXACTLY -- 5 R_ARM_ABS32 (iwram_3001e80, iwram_3001eec, gPtrs,
 * gDMATaskCount, Func_8001af8-style Func_80008d4) and 19 R_ARM_THM_CALL, same
 * symbols, same multiplicities, only offsets drifting.
 *
 * ================================================================
 * THE BLOCKER: ONE LICM HOIST TAKES r11 AND `ctx` GOES TO THE STACK
 * ================================================================
 *
 * In the mode-0 frame loop the ROM materialises the constant 0x40 INSIDE the
 * body (`mov r4, #0x40`, one instruction serving two `sub r3, r4, r3` seven
 * instructions apart) and keeps the `ctx` parameter in r11 for the whole
 * function.  gcc HOISTS that 0x40 into the loop's preheader instead
 * (`.08.loop`: `Insn 274: regno 85 (life 7), move-insn savings 1  moved to
 * 802`, and `.07.gcse` shows insn 274 is `(set (reg:SI 85) (const_int 64))`).
 * Hoisted, it lives across the loop's three calls, so it needs a callee-saved
 * register; r11 is the only one free; and `ctx` -- the one long-lived value with
 * nowhere else to go -- is spilled to a new stack slot at sp+8.
 *
 * THAT ONE DECISION IS THE WHOLE RESIDUE.  It costs, in order: `str r1,[sp,#8]`
 * in the prologue and `ldr r0,[sp,#8]` at each of the three `ctx` uses where
 * the ROM has `mov r0, r11`; the frame growing 0xf0 -> 0xf4, which shifts EVERY
 * stack offset by 4 (`add r0, sp, #0x60` against the ROM's `#0x5c`,
 * `add r0, sp, #0xc` against `#0x8`, `add r7, sp, #0xb4` against `#0xb0`); the
 * r6/r7 role swap on the &v1 pointer and the REG_IME address; and `mov r1, r11`
 * inside the body where the ROM has `mov r4, #0x40`.
 *
 * THE DIAGNOSTIC THAT PROVES IT, and it is worth keeping: writing the two
 * `0x40 - v.x` / `0x40 - v.y` subtractions ADJACENTLY in the source shortens the
 * pseudo's REG_LIVE_LENGTH from 7 to 2, `move_movables` then reports "not
 * desirable", and the FIXED VERSION APPEARS IMMEDIATELY -- `mov fp, r1` for ctx,
 * `sub sp, #0xf0`, `add r6, sp, #0xbc`, `ldr r7, =REG_IME`, all exact
 * (scratch candidate mi5/mi7).  The loop gate is
 * `m->lifetime * threshold * savings >= insn_count`: life 7 x threshold x 1
 * against the loop's 67 real insns moves it, life 2 does not.
 *
 * AND THAT DIAGNOSTIC IS NOT THE ANSWER, because it changes the BODY ORDER.
 * The ROM interleaves: load v.x, build 0x40, build 0x13c4, subtract, add base,
 * shift, store, build 0x13c8, load v.y, add base, subtract, shift, store.  With
 * the subtractions adjacent gcc emits both `sub`s first and then both stores,
 * and NOTHING REORDERS IT BACK -- ARM gcc-2.96 runs NO sched1 at -O2 (the -da
 * dumps go 13.combine, 14.ce, 15.regmove, 17.lreg, 18.greg, 19.flow2, 20.ce2,
 * 23.sched2: there is no pre-reload scheduler), so instruction order inside a
 * block is RTL order, and sched2 does not undo it.  Four orderings measured, all
 * at 274 instructions and matching size: ROM order both loops 155; pointers
 * computed first 258 (272 insns); subtractions split in mode 1 only 159; split
 * in both 172.  A pin of ctx to r11 (`register void *c __asm__("r11")`) is
 * strictly worse at 235.
 *
 * So the residue needs the hoist declined WITHOUT touching the order, i.e.
 * insn_count above 7 x threshold, and nothing source-level found it.
 *
 * TWO SMALLER RESIDUES, both the SAME CLASS: a symbol address that the ROM keeps
 * in a register with the offset applied separately, where gcc folds the offset
 * into the pool word's addend.
 *   1. `ldr r7, =iwram_3001e80 / mov r3, r7 / sub r3, #0xc` -- we get the `sub`
 *      but not the `mov`, i.e. gcc coalesces the address pseudo with the result
 *      where the ROM keeps `base` live in a callee-saved register.  One
 *      instruction.
 *   2. `ldr r3, =gPtrs / add r3, #0x9c` against our `ldr r1, =gPtrs+156`.  One
 *      instruction plus a pool-word addend.  Five spellings inert
 *      (`gPtrs[0x27]`, `*(void**)((char*)gPtrs+0x9c)`, a named `void **gp`
 *      assigned in the loop, `gp = gPtrs + 0x27; *gp`, a
 *      `void * volatile *` load), and `-fno-rerun-cse-after-loop` is inert too.
 *      Minimal testcases show the fold is LOOP-SPECIFIC: outside a loop
 *      `gp[0x27]` emits the ROM's separate `add`; inside any loop it folds.
 *
 * ================================================================
 * WHAT CLOSED IT DOWN TO HERE -- five things, two of them new
 * ================================================================
 *
 * 1. NEW, AND IT CORRECTS A RECORDED LEVER.  docs/elevation.md's negative-offset
 *    entry gives `view = *(char **)((char *)&iwram_3001eec - 0x6c)` and
 *    `(&iwram_3001e80)[-5]` as the way to pool a symbol with a ZERO addend.
 *    BOTH OF THOSE FOLD AT -O2 when the symbol has only ONE use: measured,
 *    `(&sym)[-3]` and `*(void **)((char *)&sym - 12)` each emit
 *    `.word sym-12`.  What does NOT fold is the address held in a NAMED POINTER
 *    LOCAL first:
 *        void **base = &sym;   st = base[-3];        ->  ldr / sub #12 / ldr
 *    (also `char *b = (char *)&sym; *(void **)(b - 12)`).  The Func_80d6504
 *    park's spelling works there only because that function reads the symbol a
 *    SECOND time, which materialises the address anyway.  This one instruction
 *    was worth 272 -> 274 instructions and 250 -> 155 differing: it is what
 *    turned an unmeasurable count into a distance.
 *
 * 2. THE FRAME IS SOLVED AND IT PINS THE STRUCT SIZE.  `sub sp, #0xf0` with the
 *    descriptor at sp+0x5c, the second at sp+0x8, and the two Func_80b845c
 *    outputs at sp+0xbc and sp+0xb0.  ARM gcc-2.96 allocates stack locals
 *    FIRST-DECLARED-HIGHEST with reload's spill slots below all of them, so the
 *    gaps are readable: 0xf0-0xbc, 0xbc-0xb0, 0xb0-0x5c, 0x5c-0x8 = 0x34, 0xc,
 *    0x54, 0x54.  vec3_t is 0xc, so THE ANIM DESCRIPTOR IS 0x54 = 84 BYTES, not
 *    the 0x2c that src/non_matching/rom_c9000/cb1a4_EPowerUp.c's `State` models
 *    -- that park only declared the fields it saw (f0..f20 + short ids[4]) and
 *    its struct should be padded to 84.  With `State` at 0x54 and one 40-byte
 *    local declared ahead of everything, all four offsets and the frame total
 *    come out exact on the first try.  The 40 bytes are held here as
 *    `int pad[10]; (void)&pad;` -- the tree's own forced-slot idiom, from
 *    src/rom_b5000/rom_b8228_c_a_c_a_a_b.c's `vec3_t junk; (void)&junk;`.
 *    WHAT IT REALLY IS IS NOT KNOWN, and that is the one invented thing in this
 *    file.
 *
 * 3. SET_IO IS FOR EXACTLY THIS.  `REG_WIN0H = 0xf0` makes the constant HImode
 *    and gcc pools it (`ldr r3, =0xf0`); the ROM has `mov r2, #0xf0`.
 *    `SET_IO(REG_WIN0H, 0xf0)` -- whose whole body is
 *    `unsigned __value = value; register = __value;` -- forces SImode and emits
 *    the `mov`.  Four registers here (WIN0H / WIN0V / WININ / WINOUT), and
 *    writing them in address order reproduces the ROM's single pool load plus
 *    `add r3, #4 / #4 / #2` walk.
 *
 * 4. THE DMA-QUEUE APPEND IS THE LANDED ONE.  Lifted from the EXACT
 *    src/rom_f2000/rom_f2028_c_c_a_a_a_c_b.c (Func_80f2f10): the same
 *    `struct DmaQueue`, the same LOCK_IME macro, the same
 *    `task = (u32 *)(count * 12 + (u32)queue + 4)` and signed `count < 32`
 *    giving the ROM's `cmp r2, #0x1f / bgt`.  ONE ADAPTATION: this ROM writes
 *    the source word BEFORE bumping the count (`stmia r3!, {r0}` then
 *    `strh r2, [r1]`), where Func_80f2f10 bumps first.
 *
 * 5. THREE SEPARATE FUNCTION-POINTER LOCALS, as in
 *    src/non_matching/rom_c9000/d0468_ScreenMelt.c -- not needed here, since
 *    Func_80008d4 is called once, but the same `void (*fn)(int,int) =
 *    Func_80008d4; fn(0x6004000, 0x4000);` idiom as the file-mate
 *    src/rom_b5000/rom_c10e8_a_a_b.c (Func_80c16d0, landed) gives the ROM's
 *    `ldr r3, =Func_80008d4 / bl _call_via_r3` first time.
 *    NOTE ON Func_80c16d0's PROTOTYPE: the ROM sets r0 from ctx before calling
 *    it (`mov r0, r11 / bl Func_80c16d0`), so in THIS translation unit it is
 *    declared WITH an argument, even though the landed
 *    src/rom_b5000/rom_c10e8_a_a_b.c defines it as `int Func_80c16d0(void)`.
 *    That is not a contradiction -- it is what the ROM's caller shows.
 *
 * ================================================================
 * THE REFERENCE'S PROSE IS WRONG IN TWO PLACES
 * ================================================================
 *
 * `@ RunAnimationSequence / r0.. = parameters. Loads assets (Anim_Cast),
 *  projects the combatant's parts (.gcc2_compiled.), sets the scene up
 *  (Func_c0774) and runs it frame by frame, releasing with .gcc2_compiled..
 *  261 lines; traced structurally.`
 *
 *   - "261 lines" is the .s line count; the assembled object has 274 encodings.
 *     Two of the three `.gcc2_compiled.` placeholders are disassembler noise for
 *     calls it could not name.
 *   - MORE IMPORTANTLY, the prose describes ONE behaviour where the function is
 *     a FOUR-WAY DISPATCH on its third argument.  r2 == 0 is the cast intro
 *     (Anim_Cast, a 45-frame BG2X scroll with a palette fade over the first 25,
 *     then Func_80c16d0); r2 == 1 is the unleash intro (_Anim_UnleashIntro, 40
 *     frames counting DOWN, then _Func_80ccbdc); r2 == 2 builds an 84-byte
 *     descriptor on the stack and tail-calls _Anim_EPowerUp; anything else
 *     builds the same descriptor with field 0x18 zeroed and calls
 *     _Anim_DjinnSet.  Only the r2 == 0 arm is what the prose describes.
 *   - The fourth parameter (r3) is used ONLY by the r2 == 2 arm, as descriptor
 *     field 0x18; the r2 == 3 arm writes 0 there instead.  That asymmetry is the
 *     clearest thing in the function and the prose does not mention it.
 *
 * ================================================================
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80c1798.c \
 *     asm/rom_b5000/rom_c10e8_a_a_c_c.s --func Anim_MoveIntro
 */
#include "gba/types.h"
#include "gba/io.h"

typedef struct {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    int f20;
    short ids[4];
    int pad[10];
} State;

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
extern void *iwram_3001e80;
extern void *iwram_3001eec;
extern void *gPtrs[];

extern void WaitFrames(int n);
extern void Func_80c0774(int a, unsigned short b, int c);
extern void SetRegAnimDest(int reg, int val);
extern void Anim_Cast(void *ctx);
extern void UploadBGPalette(void *a, void *b, int c, int d);
extern int Func_80b845c(unsigned int unit, vec3_t *out);
extern int Func_80c16d0(void *ctx);
extern void _Anim_UnleashIntro(void *ctx);
extern void _Func_80ccbdc(void);
extern void _Anim_EPowerUp(State *s);
extern void _Anim_DjinnSet(State *s);
extern void Func_80008d4(int dst, int len);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void QueueBgDma(struct DmaQueue *queue, const void *src)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *task++ = (u32)src;
        *(u16 *)queue = count + 1;
        *task++ = REG_ADDR_BG2X;
        *task = 0x84000002;
    }
    SET_IO(REG_IME, savedIme);
}

void Anim_MoveIntro(unsigned int unit, void *ctx, int mode, int arg3)
{
    int pad[10];
    void **base;
    unsigned char *st;
    unsigned char *obj;
    int *p;
    int *q;
    int i;
    int acc;
    int v;
    void (*fn)(int, int);
    vec3_t v1;
    vec3_t v2;
    State d1;
    State d2;

    (void)&pad;

    base = &iwram_3001e80;
    st = (unsigned char *)base[-3];
    WaitFrames(1);
    Func_80c0774(1, *(unsigned short *)(st + (0xc9 << 3)), 0);
    fn = Func_80008d4;
    fn(0x6004000, 0x4000);
    SetRegAnimDest(REG_ADDR_DISPCNT, 0x3741);
    SetRegAnimDest(REG_ADDR_BG2CNT, 0x784);
    SetRegAnimDest(REG_ADDR_BLDCNT, 0x3f44);
    WaitFrames(1);
    SET_IO(REG_WIN0H, 0xf0);
    SET_IO(REG_WIN0V, 0x1088);
    SET_IO(REG_WININ, 0x3f);
    SET_IO(REG_WINOUT, 0x11);
    if (mode == 0) {
        SetRegAnimDest(REG_ADDR_BLDALPHA, 0x100e);
        Anim_Cast(ctx);
        p = (int *)(st + 0x644);
        i = 0;
        acc = 0;
        do {
            obj = (unsigned char *)gPtrs[0x27];
            if (i <= 0x18) {
                v = 0x10000 - acc;
                *p = v;
                UploadBGPalette(st + 0x544, (void *)0x50000c0, v, 0x80);
            }
            Func_80b845c(unit, &v1);
            q = (int *)(obj + 0x13c4);
            *q = (0x40 - v1.x) << 8;
            *(int *)(obj + 0x13c8) = (0x40 - v1.y) << 8;
            QueueBgDma(&gDMATaskCount, q);
            *(int *)(obj + 0x13cc) = 1;
            WaitFrames(1);
            i++;
            acc += 0x444;
        } while (i <= 0x2c);
        Func_80c16d0(ctx);
    } else if (mode == 1) {
        _Anim_UnleashIntro(ctx);
        i = 0x27;
        do {
            obj = (unsigned char *)iwram_3001eec;
            Func_80b845c(unit, &v2);
            q = (int *)(obj + 0x13c4);
            *q = (0x40 - v2.x) << 8;
            *(int *)(obj + 0x13c8) = (0x40 - v2.y) << 8;
            QueueBgDma(&gDMATaskCount, q);
            *(int *)(obj + 0x13cc) = 1;
            WaitFrames(1);
            i--;
        } while (i >= 0);
        _Func_80ccbdc();
    } else if (mode == 2) {
        d1.f1c = 0;
        d1.f0 = (int)ctx;
        d1.f18 = arg3;
        d1.f8 = unit;
        d1.ids[0] = unit;
        d1.fc = unit;
        d1.f14 = 1;
        d1.f10 = 1;
        _Anim_EPowerUp(&d1);
    } else {
        d2.f1c = 0;
        d2.f18 = 0;
        d2.f8 = unit;
        d2.fc = unit;
        d2.f0 = (int)ctx;
        d2.ids[0] = unit;
        d2.f14 = 1;
        d2.f10 = 1;
        _Anim_DjinnSet(&d2);
    }
}

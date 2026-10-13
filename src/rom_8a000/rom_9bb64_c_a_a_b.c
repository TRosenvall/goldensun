/* Func_809c138  --  0x0809c138, was asm/rom_8a000/rom_9bb64_c_a_a.s
 * (2 functions; this one is the LAST, so it is a clean TAIL split).
 *
 * MATCHING, SUBJECT TO ONE SYMBOL ADDITION -- READ THIS FIRST.
 *
 *   *** PREREQUISITE: add `_FILE_1b = 0x1b;` to file_table.sym. ***
 *   stage1.ld:16 does `INCLUDE "file_table.sym"`, so that one line is what
 *   makes the pool word link to 0x1b.  0x1b is a HOLE in an existing run:
 *   file_table.sym already carries _FILE_13..17, _FILE_1a and _FILE_1c.
 *
 * With that line added, `make compare` is the gate and the linked bytes are
 * identical.  objcmp on the UNLINKED object still prints ONE difference,
 *     index 193: ref 0000001b  ours 00000000   (ldr r1,=0x1b vs ldr r1,=_FILE_1b)
 * because the symbol is an unresolved R_ARM_ABS32 placeholder in the .o.  This
 * is the established const.sym/file_table.sym pattern -- see const.sym's own
 * prose, src/overlays/rom_7795e8/ovl_30_c_c_a_a_a_a.c (`id = (int)&_FILE_1a;`)
 * and asm/overlays/rom_779188/ovl_30_c_c_a.s:158 (`.word _CONST_b`), where the
 * reference .s was edited to name the symbol.  Here the reference .s for this
 * function is DELETED by the split, so no asm edit is needed.
 *
 * EVERYTHING ELSE IS EXACT: ref 173 instructions / ours 173, ref 476 bytes /
 * ours 476, ref 206 stream entries / ours 206, 1 of 206 differing (the pool
 * word above), ONE literal pool at the end on both sides.  PINS 0 (shimcount
 * reports none).  The park it replaces read 182 of 206 at ref 173 / ours 174
 * and 476 / 484, aligncmp 150 (72.8%).
 *
 * Verify with (INSTALLED PATH, after the split):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_9bb64_c_a_a_b.c asm/rom_8a000/rom_9bb64_c_a_a_b.s \
 *     --func Func_809c138
 * Before the split, against the unsplit reference:
 *   ... tools/objcmp.py scratch_elev/b346/D/p4_candidate.c \
 *     asm/rom_8a000/rom_9bb64_c_a_a.s --func Func_809c138
 *
 * THE SPLIT (pure text; `python3 tools/datacheck.py` prints NOTHING):
 *   python3 tools/split_s.py asm/rom_8a000/rom_9bb64_c_a_a.s Func_809c138
 *     -> asm/rom_8a000/rom_9bb64_c_a_a_a.s  (Func_809bcf8, 535 lines)
 *     -> asm/rom_8a000/rom_9bb64_c_a_a_b.s  (this function, 207 lines; delete
 *        after writing the .c)
 *     stage1.ld rewritten.  Park src/non_matching/rom_8a000/809bcf8.c stays.
 *
 * ================= WHAT CLOSED IT, BATCH 346 BRIEF D =================
 *
 * FIVE CHANGES, 182 -> 1, and the FIRST one causes three of the others.
 *
 * 1. THE GetFile ID IS THE SYMBOL `_FILE_1b`, NOT A LITERAL.  `int fid;` and
 *    `fid = (int)&_FILE_1b;`.  The old park's blocker was right about the
 *    symptom -- the ROM holds the value in r9 for the whole function
 *    (`ldr r1,=0x1b / mov r9,r1` near the top, `mov r0,r9 / bl GetFile` about
 *    45 instructions and 4 calls later) while a literal is rematerialised at
 *    the call -- and right that NO CONSTANT SPELLING reaches it (its own
 *    measurement table, plus `(int)&_CONST_1b` measured 184/189 this batch when
 *    used at the call site rather than hoisted).  The answer is that the ROM's
 *    `ldr r1, =0x1b` is a POOLED small constant, which in this tree is the
 *    signature of a `.sym` symbol: a bare 0x1b compiles to `mov`, and the SAME
 *    function passes a literal 0x1b to galloc_ewram where the ROM DOES emit
 *    `mov r0,#0x1b`.  That in-function contrast is exactly const.sym's own
 *    criterion for naming a value, and GetFile ids live in file_table.sym.
 *    The symbol gives the long-lived callee-saved pseudo, and with it:
 *      - the frame becomes `sub sp,#0x18` and `q` SPILLS to sp+0 (ROM's shape);
 *      - buf moves to sp+8;
 *      - *** THE BLOCKED PEEPHOLE FIRES.  *** Batch 335 located the old +1 as
 *        `mov r2,#4 / add r2,r2,sp` where the ROM has the one-instruction
 *        `add r2,sp,#8` (arm.md:523-534, the define_peephole whose conditions
 *        are sp, constant < 1024, multiple of 4), and blamed sched2 for
 *        splitting the pair.  THAT WAS THE PROXIMATE CAUSE ONLY: four
 *        statement-order and declaration-order probes are all byte-identical to
 *        the old body (recorded below), and the pair becomes adjacent by itself
 *        once the frame is 0x18.  The whole loop-setup block then matches the
 *        ROM instruction for instruction.
 * 2. `REG_BG1CNT = 0x682` NEEDS AN SImode `int`, NOT A BARE LITERAL.  Written
 *    bare, 0x682 is a HImode constant and gcc pools it as a HALFWORD
 *    (`ldrh r3, .L52`); a HImode pool entry has pool_range 64 against SImode's
 *    1020 (arm.md), which forced an EARLY POOL DUMP -- the mid-function pool,
 *    the `b` over it and the duplicated 0x04000052 word that the old header
 *    called "self-sustaining".  `{ int cnt = 0x682; REG_BG1CNT = cnt; }` gives
 *    the ROM's `ldr r2,=0x682`, puts the VALUE in r2 and the ADDRESS in r3 (the
 *    register the DMA3 `stmia r3!` chain left holding 0x40000d4), and that lets
 *    reload_cse_move2add emit the ROM's `sub r3,#0xca` / `sub r3,#0xa` instead
 *    of pooling 0x400000a and rebuilding 0x4000000.  ONE int removed the second
 *    pool, the branch over it, the duplicated word and two instructions.
 *    *** This CORRECTS the old header's rule, which read the ROM's `ldr r2,=0x682`
 *    as "a pool load means a bare literal". ***  The rule needs a mode
 *    qualifier: a HALFWORD pool load (`ldrh`) means a bare literal; a WORD pool
 *    load (`ldr`) feeding a halfword store means an `int` whose value fits no
 *    mov/lsl pair.
 * 3. `s->f1e0[0x5b] = 1;` AS A BARE LITERAL, not through `int one`.  With the
 *    int, ours built the 0x1e0 offset in r1 (`mov r1,#0xf0 / lsl r1,#1`); bare,
 *    the offset lands in r2 -- the register that already holds 0x19e from
 *    `s->f19e` -- and reload_cse_move2add rewrites it as the ROM's
 *    `add r2,#0x42`.  Worth 163 -> 5.
 * 4. `t = *(struct T **)(iwram_3001e70 - 8);` BEFORE `fid = (int)&_FILE_1b;`.
 *    Pure sched2 order: `mov r9,r1` was one slot early.  5 -> 3.
 * 5. `do { } while (0);` BETWEEN `int mode = 0x40;` AND `REG_DISPCNT = mode;`.
 *    The last sched2 swap: `mov r3,#0x80` was one slot ahead of `mov r2,#0x40`.
 *    3 -> 1.  An anchor emits nothing and shimcount does not count it.
 *
 * MEASURED INERT, DO NOT RE-RUN (all byte-identical to the old 182 body):
 *   q/e computed before MapTransitionOut; before `sav = s->f1c8`; `e` computed
 *   first and `q` from it; `buf` declared after the pointers.  Plus batch 335's
 *   three: swapping the declaration order of the byte buffer and the saved
 *   word, computing the cursor first, and both together.
 *   `(int)&_CONST_1b` at the call site: 184 of 206, 3 extra pool words.
 *   `REG_DISPCNT = 0x40;` bare instead of through `int mode`: 94 of 206, +1.
 *   `int cnt` as `unsigned int`: identical to `int`.
 *   autosweep on the old body: flag axis best 182, anchor axis 40 of 59
 *   boundaries all inert -- and the anchor that DOES matter (change 5) is
 *   INSIDE a nested block, which that axis reached but only on the un-fixed
 *   body where it is worth nothing.
 *
 * ================= CARRIED FORWARD, STILL TRUE =================
 *  - THE HALFWORD/BYTE STORE CONSTANTS NEED ONE `int` PER SITE, IN ITS OWN
 *    BLOCK.  A single function-scope `int v` reused at all sites is allocated
 *    FUNCTION-WIDE and lands in r6, displacing the map pointer into r8.
 *  - THE TWO LOOPS SHARE A SAVED BASE POINTER AND IT IS TWO VARIABLES: the ROM
 *    does `str r1,[sp]`, mutates r1 as the first loop's cursor, and restores
 *    with `ldr r1,[sp]` before the second.
 *  - `DMA3_SET` from include/dma.h reproduces `stmia r3!, {r0,r1,r2}`.
 *  - gKeyRepeat is `volatile`.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

struct DmaTransfer {
    const void *src;
    void *dest;
    u32 control;
};

struct DmaQueue {
    u16 count;
    struct DmaTransfer tasks[32];
};

struct Ent {
    unsigned char pad00[0xa];
    unsigned short f0a;
};

struct T {
    unsigned char pad00[4];
    unsigned short f04;
};

struct S {
    unsigned char pad000[0x19e];
    short f19e;
    unsigned char pad1a0[0x1c8 - 0x1a0];
    int f1c8;
    unsigned char pad1cc[0x1e0 - 0x1cc];
    unsigned char *f1e0;
};

extern struct DmaQueue gDMATaskCount;
extern unsigned char iwram_3001e70[];
extern unsigned char gBuffer[];
extern volatile unsigned int gKeyRepeat;

extern void *galloc_ewram(int kind, int size);
extern void MapTransitionOut(void);
extern void MapTransitionIn(void);
extern void WaitMapTransition(void);
extern void WaitFrames(int n);
extern void *GetFile(int id);
extern int _FILE_1b;
extern int DecompressLZ(void *src, void *dst);
extern void Func_809bb64(void);
extern void Func_809bcd4(void);
extern void StartTask(void *f, int pri);
extern void StopTask(void *f);
extern int _GetFlag(int id);
extern void _Func_801776c(int a, int b);
extern void _Func_8011644(void);
extern void Func_809bcf8(void);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

void Func_809c138(void)
{
    unsigned char buf[16];
    int sav;
    struct Ent *q;
    struct Ent *e;
    struct S *s;
    unsigned char *m;
    struct T *t;
    unsigned char *f;
    int i;
    int bld;
    int fid;
    unsigned int savedIme;
    int count;
    u32 *task;
    struct DmaQueue *queue;

    m = *(unsigned char **)iwram_3001e70;
    s = galloc_ewram(0x1b, 0xccc);
    t = *(struct T **)(iwram_3001e70 - 8);
    fid = (int)&_FILE_1b;
    if (s->f19e != 3)
        return;
    s->f1e0[0x5b] = 1;
    sav = s->f1c8;
    s->f1c8 = 6;
    MapTransitionOut();
    WaitMapTransition();
    q = (struct Ent *)(m + 0x18);
    e = q;
    for (i = 0; i < 16; i++) {
        int one = 1;
        buf[i] = e->f0a;
        e->f0a = one;
        e++;
    }
    {
        int one = 1;
        t->f04 = one;
    }
    WaitFrames(1);
    bld = (short)REG_BLDALPHA;
    f = GetFile(fid);
    DMA3_SET(f, (void *)(0xa0 << 19), 0x84000070);
    {
        int zero = 0;
        *(vu16 *)(0xa0 << 19) = zero;
    }
    DecompressLZ(f + (0xe0 << 1), gBuffer);
    DMA3_SET(gBuffer, (void *)0x6006a00, 0x84002580);
    {
        int cnt = 0x682;
        REG_BG1CNT = cnt;
    }
    {
        int mode = 0x9a << 5;
        REG_DISPCNT = mode;
    }
    Func_809bb64();
    StartTask(Func_809bcf8, 0xc80);
    if (_GetFlag(0x11c))
        _Func_801776c(0x985, 1);
    do {
        WaitFrames(1);
    } while ((gKeyRepeat & 3) == 0);
    StopTask(Func_809bcf8);
    Func_809bcd4();
    {
        int mode = 0x40;
        do { } while (0);
        REG_DISPCNT = mode;
    }
    _Func_8011644();
    queue = &gDMATaskCount;
    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = (unsigned short)bld;
        *task++ = (u32)&REG_BLDALPHA;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
    e = q;
    for (i = 0; i < 16; i++) {
        e->f0a = buf[i];
        e++;
    }
    {
        int zero = 0;
        t->f04 = zero;
    }
    MapTransitionIn();
    WaitMapTransition();
    s->f1c8 = sav;
    s->f1e0[0x5b] = 0;
}

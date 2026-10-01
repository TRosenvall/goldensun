/* OvlFunc_881_200b9fc -- 0x0200b9fc   (overlay 881, rom_77a7c8)
 *
 * ============================================================================
 * *** BYTE-IDENTICAL.  objcmp: 1300 bytes, 579 encodings and 45 relocations
 * *** identical.  BATCH 315 LANDED IT WITH ONE WORD.
 * ============================================================================
 *
 *     extern void __DecompressLZ(const void *src, void *dst);
 *   becomes
 *     extern int  __DecompressLZ(const void *src, void *dst);
 *
 * EVERY OTHER BYTE OF THE BODY BELOW IS THE BODY THAT HAD BEEN PARKED AT 2 OF
 * 579 SINCE BATCH 302.  Nothing else changed: no statement moved, no local was
 * added, no register was pinned.  The whole residue was a WRONG EXTERN.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_77a7c8/200b9fc.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_c.s --func OvlFunc_881_200b9fc
 *   OK OvlFunc_881_200b9fc -- 1300 bytes, 579 encodings and 45 relocations identical
 *   (reference path is the UNSPLIT .s as it stands today; after the text split
 *    below it becomes asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_c_b.s)
 *
 * ---------------------------------------------------------------------------
 * WHY ONE WORD WAS WORTH TWO ENCODINGS -- AND WHY THE PARK HAD THE MECHANISM
 * WRITTEN DOWN AND STILL CONCLUDED IT WAS UNREACHABLE
 * ---------------------------------------------------------------------------
 *
 * The residue was the second `__DecompressLZ` call's argument fill, one
 * adjacent transposition:
 *     ref   adds r1, r7, r2   /  ldr r0, [pc, #264]
 *     ours  ldr  r0, [pc, #268] /  adds r1, r7, r2
 * Two independent insns, same block, filling r0 and r1 for one call.  The
 * dependence table at production flags read:
 *     insn 40  ldr r0   prio 43  dependents {49, 59, 89} = 3
 *     insn 44  add r1   prio 43  dependents {49, 59, 89} = 3
 * Priority tied, dependent count tied, so rank_for_schedule fell through to
 * INSN_LUID and the lower-LUID `ldr` won.  arg0 is precomputed first (calls.c
 * walks args forward), so the ldr always has the lower LUID.
 *
 * THE DEPENDENT COUNT IS WHAT A RETURN TYPE CONTROLS.  A **void** call is a
 * plain `call_insn` whose r0 is only CLOBBERED, and a CLOBBER NEVER BECOMES
 * reg_last_sets -- so the arg0 setter stays the last SET of r0 and keeps
 * collecting an output dependence from every later r0 write in the block.  A
 * **value-returning** call is a `call_value_insn` that SETS r0, which becomes
 * the last setter and INTERCEPTS the chain.  Declaring __DecompressLZ to
 * return a value drops insn 40 to TWO dependents; insn 44 keeps three; rung 5
 * now decides and `add r1` wins -- the ROM's order, pool displacement and all.
 *
 * THE PARK HAD THIS.  Its paragraph (3) derived the asymmetry from a landed
 * sibling (ovl_30_c_a_c_c_a_c_a_c_c_b.c, whose __StartTask RETURNS A VALUE and
 * whose `lsl r1` therefore wins on dependent count), stated correctly that
 * "__DecompressLZ is void", and then wrote: "so neither half of that asymmetry
 * is available here" and "THE BASELINE SHAPE CANNOT SCHEDULE THE ROM'S WAY".
 * The asymmetry WAS available -- it lives in OUR declaration, not in the ROM's
 * instruction stream, and nothing but a header had to change.  Everything built
 * on top of that sentence is now void:
 *   - the "KEY SHAPE" (name the destination in a local) is NOT wanted.  It is
 *     3 of 579, and the 14 spellings recorded against it were a search of the
 *     wrong space.
 *   - the diagnostic that reached 0 by declaring a FAKE THREE-ARGUMENT
 *     `__DecompressLZ3` asm-label alias is obsolete.  DO NOT SHIP IT AND DO NOT
 *     RE-DERIVE IT; the honest one-word fix supersedes it entirely.
 *   - "make a conflict-free pseudo take r2 instead of REG_ALLOC_ORDER's first
 *     free register" was never the problem.  No allocation had to move.
 *   - "NO FLAG MOVES THE ALLOCATION. A Makefile row cannot land this" is true
 *     and now irrelevant.
 *
 * GENERAL RULE, the cheap half of which costs one build: CROSS-CHECK EVERY
 * `extern` IN A PARKED FILE AGAINST THE TREE'S OWN HEADERS AND AGAINST THE
 * CALLEE'S OTHER CALLERS.  A wrong return type is invisible to size, to the
 * instruction count, to the per-opcode histogram, to the call multiset AND to
 * the relocation sequence -- every rung of the measurement hierarchy is blind
 * to it -- yet it removes a scheduling escape on any r0 chain.
 *
 * ---------------------------------------------------------------------------
 * LANDING PREREQUISITES -- ALL CHECKED THIS BATCH
 * ---------------------------------------------------------------------------
 *
 * SHIMS: PIN-FREE.  tools/shimcount.py exits 0 and reports nothing -- no
 * register pins, no barriers, no per-file flag override.  NO fakematch.txt row
 * is needed.  Builds under the production -O2 flag set with no Makefile group.
 *
 * SPLIT SHAPE: a TEXT SPLIT IS REQUIRED; NO data split.
 *   asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_c.s holds TWO functions:
 *     line  9  .thumb_func_start OvlFunc_881_200b95c
 *     line 96  .thumb_func_start OvlFunc_881_200b9fc   (the target, and LAST)
 *   tools/datacheck.py on that file is SILENT (exit 0): no data section, no
 *   label needs `.global`, no new export.
 *   tools/split_s.py --dry-run (it DOES honour the flag -- the old header's
 *   warning that it does not was already corrected in batch 305) prints:
 *     would write asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_c_a.s  (1 fn,  82 lines)
 *     would write asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_c_b.s  (1 fn, 608 lines)
 *     would REMOVE asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_c.s
 *     would rewrite overlays/rom_77a7c8/overlay.ld
 *   LANDING FILE: src/overlays/rom_77a7c8/ovl_30_c_c_a_c_c_b.c
 *   Verify `make compare` is green AFTER the split and BEFORE writing the .c,
 *   and remember the generated .s beside the landed .c is TRACKED and belongs
 *   in the commit.
 *
 * ASM-LABEL CAPTURE: three asm-label externs (.L67a0, .L44ac, .L47a6), none
 * defined in our generated .s, so none captured.  Each contains a HEX LETTER
 * and gcc's own labels are DECIMAL, which is the real reason they are safe --
 * not their digit count.  gcc's counter reached .L302 on this function.
 *
 * ---------------------------------------------------------------------------
 * THE LEVERS THAT BUILT THIS BODY -- KEPT, BECAUSE THEY ARE LOAD-BEARING
 * ---------------------------------------------------------------------------
 *
 *  1. THE DMA QUEUE PUSH AS AN INLINE, from the landed sibling
 *     src/overlays/rom_7795e8/ovl_30_c_c_a_a_a_b.c: the DmaTransfer/DmaQueue
 *     layout, `count * 12 + queue + 4` for the task address, the count store
 *     BEFORE the source store, `SET_IO(REG_IME, REG_ADDR_IME)` for the
 *     `strh r5,[r5]` that disables interrupts by storing the port's own
 *     address into it, and `do/while(0)` on the save-and-disable pair.  This
 *     function pushes fourteen times in three shapes, so THREE specialised
 *     inlines rather than one general one.
 *
 *  2. DO NOT PASS A TABLE BASE AS AN INLINE'S ARGUMENT.   513 -> 492
 *     The palette push's source is `L44ac + (which << 5)`.  Passing `L44ac` as
 *     an argument made gcc materialise the symbol address AT THE CALL SITE,
 *     above the interrupt guard; the ROM loads it INSIDE the guard.  Referring
 *     to the symbol from within the inline body puts it where the ROM has it.
 *     The recorded trap ("an argument is not at the call site, it is before
 *     everything the inline does") reproduced on a SYMBOL rather than a
 *     computed value -- worth noting because a symbol address looks like a
 *     constant and reads as harmless.
 *
 *  3. A NAMED OFFSET LOCAL DEFEATS fold's SYMBOL+CONST COLLAPSE.  492 -> 2
 *     The ROM reads gState at +0x1f4 as
 *       ldr r3,=gState / mov r1,#250 / lsl r1,#1 / add r3,r1 / ldr r0,[r3]
 *     -- symbol plus a RUNTIME-BUILT shiftable constant, added at run time.
 *     Written `*(int *)(gState + (0xfa << 1))`, `fold` canonicalises the
 *     pointer PLUS into one `(const (plus (symbol_ref gState) 0x1f4))`: ONE
 *     pool word and THREE FEWER INSTRUCTIONS.  Naming the offset first --
 *     `off = 0xfa << 1; ... *(int *)(gState + off)` -- keeps it a runtime add.
 *     DISCRIMINATOR against the identical-looking case twelve instructions
 *     later: `iwram_3001ebc` is a POINTER, so `ldr r3,[r3]` then `add` is what
 *     a plain constant offset already produces and no local is needed.  Symbol
 *     base + constant needs the named offset; pointer load + constant does not.
 *     One `int off;` was worth 490 of the 492.
 *
 *  4. USED-TWICE VALUES ARE int LOCALS; USED-ONCE VALUES ARE IMMEDIATES.
 *     Seven fade values are stored to the halfword .L67a0.  Three (0xb00,
 *     0xa00, 0x900) appear TWICE -- once descending, once ascending -- and the
 *     ROM keeps each in a CALLEE-SAVED HIGH register (r11, r9, r10)
 *     materialised once as `mov #0xb0 / lsl #4`.  The other four appear once
 *     and are pool words.  So the three are `int` locals (b, a, n) and the rest
 *     are literals at the store.  This is cse1 commoning a twice-used value,
 *     not register pressure.
 *
 * READINGS WORTH KEEPING
 *  - `strh r5,[r5]` with r5 = &REG_IME disables interrupts by storing the
 *    port's own address (low bit clear).  SET_IO(REG_IME, REG_ADDR_IME).
 *  - `cmp r2,#0x1f / bgt` is `count < 32`, not `<= 31` spelled oddly.
 *  - The parameter arrives in r0 and is immediately parked in r8, whose last
 *    use is the palette push; r8 is then REUSED for &.L67a0.  Two live ranges
 *    in one register, not one variable.
 *  - `.L67a0` is written but never read here; the fade values are consumed by
 *    the task OvlFunc_881_200b8fc this function starts.
 *
 * ---------------------------------------------------------------------------
 * MEASURED NEGATIVE OR INERT (kept so it is not re-derived)
 * ---------------------------------------------------------------------------
 *   the "key shape" (destination named in a local) and all 14 spellings of its
 *     address/offset ................................................ 3 of 579
 *   source named first ............................ reverts to 2, first idx 22
 *   naming the `0xe4 << 1` offset . CATASTROPHIC: 577 of 579, 1296 bytes, 456
 *     differing, relocations shifted (a pointer base plus a constant offset
 *     already emits the runtime add)
 *   `register int off2 __asm__("r2")` ... ignored by gcc-2.96 (a local register
 *     asm variable appearing in no asm operand) ..................... still 3
 *   -fno-schedule-insns2 ......................... 112 differing (size/count ok)
 *   -fschedule-insns (switches sched1 ON) ................... byte-identical to
 *     the parked body, i.e. INERT -- sched1 does not run at -O2 here at all
 *   -fno-caller-saves -fno-regmove -fno-gcse -fno-strength-reduce
 *     -fno-expensive-optimizations -fno-cse-follow-jumps -fno-force-mem
 *     -fno-thread-jumps -fno-function-cse -fno-peephole -fno-reorder-blocks
 *     -fno-delete-null-pointer-checks ................................ all inert
 *   -fno-rerun-cse-after-loop ..... 585 differing, 1412 bytes, 621 encodings
 */
#include "gba/types.h"
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
extern char *iwram_3001ebc;
extern char gState[];
extern unsigned short L67a0 __asm__(".L67a0");
extern char L44ac[] __asm__(".L44ac");
extern char L47a6[] __asm__(".L47a6");
extern char gScript_943__0200c4ec[];

extern void *__Func_8004970(int size);
extern void __WaitFrames(int n);
extern void __ClearFlag(int id);
extern void __SetFlag(int id);
extern void __Func_8011590(void);
extern void __Func_8011644(void);
extern int __DecompressLZ(const void *src, void *dst);
extern int __StartTask(void *f, int n);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern char *__MapActor_GetActor(int slot);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __PlaySound(int id);
extern void __free(void *p);
extern void OvlFunc_881_200b8fc(void);
extern void OvlFunc_881_200b95c(void);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

/* The palette push: source is a table entry indexed by the parameter, and the
 * index arithmetic belongs INSIDE the interrupt guard, which is why the base
 * and the index are separate arguments rather than one computed pointer. */
static inline void PushPal(struct DmaQueue *queue, int idx)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = (u32)L44ac + (idx << 5);
        *task++ = 0x50001c0;
        *task = 0x80000010;
    }
    SET_IO(REG_IME, savedIme);
}

static inline void PushTiles(struct DmaQueue *queue, void *src)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = (u32)src;
        *task++ = 0x6001000;
        *task = 0x84000400;
    }
    SET_IO(REG_IME, savedIme);
}

/* The frame push: offset is built inside the guard, as the ROM does. */
static inline void PushFrame(struct DmaQueue *queue, void *base, u32 off)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = (u32)base + off;
        *task++ = 0x6002000;
        *task = 0x84000140;
    }
    SET_IO(REG_IME, savedIme);
}

void OvlFunc_881_200b9fc(int which)
{
    void *buf;
    char *actor;
    int b, a, n;
    int off;

    buf = __Func_8004970(0x80 << 7);
    __WaitFrames(1);
    __ClearFlag(0x109);
    __Func_8011590();
    __DecompressLZ(L47a6, buf);
    __DecompressLZ(gScript_943__0200c4ec, (char *)buf + (0x80 << 5));
    PushPal(&gDMATaskCount, which);
    PushTiles(&gDMATaskCount, buf);
    __StartTask(OvlFunc_881_200b8fc, 0xc8 << 4);
    __CutsceneStart();
    PushFrame(&gDMATaskCount, buf, 0xea << 6);
    off = 0xfa << 1;
    actor = __MapActor_GetActor(*(int *)(gState + off));
    actor[0x54] = 0;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    __MapTransitionIn();
    __WaitMapTransition();
    __PlaySound(0xf6);
    L67a0 = 0xe00;
    PushFrame(&gDMATaskCount, buf, 0xd2 << 6);
    __CutsceneWait(2);
    L67a0 = 0xd00;
    PushFrame(&gDMATaskCount, buf, 0xba << 6);
    __CutsceneWait(2);
    L67a0 = 0xc00;
    PushFrame(&gDMATaskCount, buf, 0xa2 << 6);
    __CutsceneWait(2);
    b = 0xb0 << 4;
    L67a0 = b;
    PushFrame(&gDMATaskCount, buf, 0x8a << 6);
    __CutsceneWait(2);
    a = 0xa0 << 4;
    L67a0 = a;
    PushFrame(&gDMATaskCount, buf, 0xe4 << 5);
    __CutsceneWait(2);
    n = 0x90 << 4;
    L67a0 = n;
    PushFrame(&gDMATaskCount, buf, 0xb4 << 5);
    __CutsceneWait(2);
    L67a0 = 0x800;
    PushFrame(&gDMATaskCount, buf, 0x84 << 5);
    __CutsceneWait(0x8c);
    PushFrame(&gDMATaskCount, buf, 0xb4 << 5);
    __CutsceneWait(4);
    PushFrame(&gDMATaskCount, buf, 0xe4 << 5);
    __CutsceneWait(4);
    PushFrame(&gDMATaskCount, buf, 0x8a << 6);
    __CutsceneWait(4);
    L67a0 = n;
    PushFrame(&gDMATaskCount, buf, 0xa2 << 6);
    __CutsceneWait(4);
    L67a0 = a;
    PushFrame(&gDMATaskCount, buf, 0xba << 6);
    __CutsceneWait(4);
    L67a0 = b;
    PushFrame(&gDMATaskCount, buf, 0xd2 << 6);
    __CutsceneWait(4);
    L67a0 = 0xc00;
    PushFrame(&gDMATaskCount, buf, 0xea << 6);
    __Func_8011644();
    __StartTask(OvlFunc_881_200b95c, 0xc8 << 4);
    __PlaySound(0x8d);
    L67a0 = 0xd00;
    __CutsceneWait(4);
    L67a0 = 0xe00;
    __CutsceneWait(4);
    L67a0 = 0xf00;
    __CutsceneWait(4);
    L67a0 = 0x1000;
    __CutsceneWait(0x2d);
    __MapTransitionOut();
    __WaitMapTransition();
    __free(buf);
    __SetFlag(0x101);
}

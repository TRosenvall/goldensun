/* OvlFunc_881_200b9fc -- 0x0200b9fc   (overlay 881, rom_77a7c8)
 *
 * NON-MATCHING, 2 of 579 encodings differ
 *
 * *** TWO INSTRUCTIONS FROM BYTE-EXACT. THIS IS THE CLOSEST PARK IN BATCH 302
 * *** AND IT IS A SINGLE ADJACENT TRANSPOSITION, NOT A CLASS BLOCKER.
 *
 * MEASUREMENT EXACTNESS:
 *   SIZE  is EXACT (objcmp prints no SIZE line: ref 1300 bytes, ours 1300)
 *   COUNT is EXACT (ref 579 encodings, ours 579)
 *   => the figure 2 IS A TRUE DISTANCE, not a saturated count.
 *   ALIGNCMP: not needed and not the ranking view here -- with size and count
 *   exact AND no insert/delete pair in the hunks (the two differing encodings
 *   are a swap of two adjacent instructions, so nothing shifts), objcmp's 2 is
 *   already the honest figure. Stated explicitly because the batch discipline
 *   asks for aligncmp whenever an insert/delete pair appears; there is none.
 *   RELOCATIONS: identical -- symbol SEQUENCE and OFFSETS both.
 *
 * THE ENTIRE REMAINING RESIDUE, verbatim from the objdump of both objects:
 *   ordinal 24   REF   18b9   adds r1, r7, r2
 *                OURS  4843   ldr  r0, [pc, #268]
 *   ordinal 25   REF   4842   ldr  r0, [pc, #264]
 *                OURS  18b9   adds r1, r7, r2
 * The same two instructions in the opposite order, at the second
 * `__DecompressLZ` call's argument fill. The ROM forms the destination pointer
 * (`buf + 0x1000`) and THEN loads the source symbol address; we do the reverse.
 * Both are independent, both feed the same call, and the pool offset differs by
 * the 4 bytes the swap itself causes (#268 vs #264) -- that is the swap, not a
 * second defect.
 *
 * BLOCKER, BY PASS: sched1 (haifa-sched.c), argument set-up order.
 * Two independent single-cycle insns with no dependency between them, in one
 * basic block, filling r0 and r1 for the same call. Nothing in the statements
 * orders them; the list scheduler's ready-list tie-break does.
 *
 * WHAT WAS MEASURED AGAINST IT (all on the v3 base, which reads 2):
 *   name the destination in a local before the call          3   WORSE
 *   name BOTH the destination and the source in locals       3   WORSE
 *   do { dst = ...; } while (0) scheduling barrier           5   WORSE
 *   name the 0x1000 offset in a local (`off2`)               2   INERT
 *   -fno-schedule-insns2 (SCHED2_CFLAGS, an existing group)  WORSE
 *        (tryc text screen: 439 differing lines vs 415 on base -- so sched2 is
 *         NOT the pass, which is what points at sched1)
 * The three local-naming spellings all perturbed which register receives the
 * shift constant (`movs r2,#0x80` became `movs r0,#0x80`), i.e. they bought the
 * order at the cost of an allocation change. The inert one is UNTESTED
 * elsewhere, not disproved -- it is evidence only against this base.
 *
 * NEXT THING TO TRY, for whoever picks this up: the two documented scheduling
 * levers not yet spent here -- `goto` into a do/while (whose NOTE_INSN_LOOP_BEG
 * rides as a barrier and reorders the code BEFORE it, which is exactly the
 * position of this pair), and moving the first __DecompressLZ call's statement
 * boundary. Two instructions is worth one more round.
 *
 * THE LEVERS THAT PAID, IN THE ORDER THEY PAID
 *
 *  1. THE DMA QUEUE PUSH AS AN INLINE -- the shape, from the landed sibling
 *     src/overlays/rom_7795e8/ovl_30_c_c_a_a_a_b.c: the DmaTransfer/DmaQueue
 *     layout, `count * 12 + queue + 4` for the task address, the count store
 *     BEFORE the source store, `SET_IO(REG_IME, REG_ADDR_IME)` for the
 *     `strh r5, [r5]` that disables interrupts by storing the port's own
 *     address into it, and `do/while(0)` on the save-and-disable pair.
 *     This function pushes fourteen times in three shapes, so three
 *     specialised inlines rather than one general one.
 *
 *  2. DO NOT PASS A TABLE BASE AS AN INLINE'S ARGUMENT.   513 -> 492
 *     The palette push's source is `L44ac + (which << 5)`. Passing `L44ac` as
 *     an argument made gcc materialise the symbol address AT THE CALL SITE,
 *     above the interrupt guard; the ROM loads it INSIDE the guard. Referring
 *     to the symbol from within the inline body put it where the ROM has it.
 *     This is the landed sibling's recorded trap ("an argument is not at the
 *     call site, it is before everything the inline does") reproduced on a
 *     SYMBOL rather than a computed value -- worth noting because a symbol
 *     address looks like a constant and reads as harmless.
 *
 *  3. A NAMED OFFSET LOCAL DEFEATS fold's SYMBOL+CONST COLLAPSE.  492 -> 2
 *     THE BATCH'S BIGGEST SINGLE MOVE, and it is what made size and count
 *     exact simultaneously. The ROM reads gState at +0x1f4 like this:
 *         ldr r3, =gState / mov r1, #250 / lsl r1, #1 / add r3, r1 / ldr r0,[r3]
 *     -- the symbol and a RUNTIME-BUILT shiftable constant, added at run time.
 *     Written as `*(int *)(gState + (0xfa << 1))`, `fold` canonicalises the
 *     pointer PLUS into a single `(const (plus (symbol_ref gState) 0x1f4))`,
 *     which becomes ONE pool word and THREE FEWER INSTRUCTIONS. Naming the
 *     offset in an int local first --
 *         off = 0xfa << 1;  ... *(int *)(gState + off)
 *     -- keeps it a runtime add and restores all three.
 *     THE TELL IS READABLE STRAIGHT OFF THE DISASSEMBLY, and it is the
 *     discriminator against the identical-looking case twelve instructions
 *     later: `iwram_3001ebc` is a POINTER, so `ldr r3,[r3]` then `add` is what
 *     a plain constant offset already produces and no local is needed. Symbol
 *     base + constant needs the named offset; pointer load + constant does not.
 *     One `int off;` was worth 490 of the 492.
 *
 *  4. USED-TWICE VALUES ARE int LOCALS; USED-ONCE VALUES ARE IMMEDIATES.
 *     Seven fade values are stored to the halfword .L67a0. Three of them
 *     (0xb00, 0xa00, 0x900) appear TWICE -- once descending, once ascending --
 *     and the ROM keeps each in a CALLEE-SAVED HIGH register (r11, r9, r10)
 *     materialised once as `mov #0xb0 / lsl #4`. The other four appear once and
 *     are pool words. So the three are `int` locals (b, a, n) and the rest are
 *     literals at the store. Reading the high-register traffic as register
 *     pressure would have been wrong: it is cse1 commoning a twice-used value,
 *     and declaring the locals reproduces it exactly.
 *
 * READINGS WORTH KEEPING
 *  - `strh r5, [r5]` with r5 = &REG_IME disables interrupts by storing the
 *    port's own address (low bit clear). SET_IO(REG_IME, REG_ADDR_IME).
 *  - `cmp r2, #0x1f / bgt` is `count < 32`, not `<= 31` spelled oddly.
 *  - The parameter arrives in r0 and is immediately parked in r8, whose last
 *    use is the palette push; r8 is then REUSED for &.L67a0. Two live ranges in
 *    one register, not one variable.
 *  - `.L67a0` is written but never read by this function; the fade values are
 *    consumed by the task OvlFunc_881_200b8fc this function starts.
 *
 * ASM-LABEL CAPTURE: CHECKED AND SAFE, with a sharpening of the recorded rule.
 *   This function needs three asm-label externs: .L67a0, .L44ac, .L47a6.
 *   Our generated .s was produced and grepped: NONE of the three is defined in
 *   it, so none is captured.
 *   BUT the recorded shorthand "four-digit labels are safe because gcc's
 *   counter does not reach them" is the wrong reason, and this function shows
 *   why: GCC'S COUNTER REACHED .L302 HERE. What makes these three safe is that
 *   each contains a HEX LETTER (67a0, 44ac, 47a6), and gcc's own labels are
 *   DECIMAL -- so they can never collide whatever the counter does. An
 *   ALL-DIGIT four-digit label is only safe if it exceeds the counter's
 *   high-water mark, and on a 579-instruction function that mark is 302, not
 *   the ~70 a smaller function suggests. Screen on "does it contain a letter",
 *   then on the number, not on the digit count.
 *
 * SHIMS: PIN-FREE. tools/shimcount.py reports nothing -- no pins, no barriers,
 * no per-file flag overrides. Builds under the production -O2 flag set with no
 * Makefile group. (The asm-label externs are not shims and shimcount does not
 * count them; they are checked separately above.)
 *
 * SPLIT SHAPE: a TEXT SPLIT IS REQUIRED; no data split.
 *   asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_c.s holds TWO functions:
 *     line   9  OvlFunc_881_200b95c   (ends line 84)
 *     line  96  OvlFunc_881_200b9fc   (the target; ends line 692 = end of file)
 *   The target is SECOND AND LAST, so
 *     tools/split_s.py asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_c.s OvlFunc_881_200b9fc
 *   yields _a.s (OvlFunc_881_200b95c) and _b.s (the target). There is no _c
 *   because nothing follows. It rewrites overlays/rom_77a7c8/overlay.ld.
 *   tools/datacheck.py reports NO data section in this file, so NO label needs
 *   `.global` and no new export is required. The three .L externs above are
 *   defined in OTHER objects of this overlay and are already reachable.
 *   WARNING: split_s.py has NO --dry-run -- it ignores the flag and performs
 *   the split, deleting the original .s and editing the linker script. Verify
 *   `make compare` is still green AFTER the split and BEFORE writing the .c.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_77a7c8/200b9fc.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_c.s --func OvlFunc_881_200b9fc
 *   (the reference path is the UNSPLIT .s as it stands today; after the split
 *   it becomes ..._b.s)
 *
 * FINAL INSTALLED PATH:
 *   src/non_matching/ovl_77a7c8/200b9fc.c
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
extern void __DecompressLZ(const void *src, void *dst);
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

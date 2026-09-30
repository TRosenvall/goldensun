/* OvlFunc_881_200b9fc -- 0x0200b9fc   (overlay 881, rom_77a7c8)
 *
 * ============================================================================
 * BATCH 305 UPDATE -- THE RESIDUE IS NOW ONE REGISTER-ALLOCATION CHOICE, AND
 * A DELIBERATE DIAGNOSTIC REACHES diff=0.  THREE CLAIMS BELOW ARE SUPERSEDED.
 * ============================================================================
 *
 * INSTALLED BODY RE-MEASURED AS FOUND: objcmp 2 of 579, size 1300 == 1300,
 * count 579 == 579, relocations identical.  aligncmp: aligned-equal 578
 * (99.8% of ref), 2 differing in 2 hunks.  shimcount clean (exit 0).
 * The header's figure is TRUE.  This body stays -- it is still the best
 * ranked candidate (2, and 578 aligned, against the shape below's 3/576).
 *
 * SUPERSEDED CLAIM 1 -- THE PASS IS sched2, NOT sched1.  sched1 DOES NOT RUN
 *   IN THIS BUILD AT ALL.  A -da compile emits no sched1 dump (the pass list is
 *   00.rtl 01.sibling 02.jump 03.cse 04.addressof 07.gcse 08.loop 09.cse2
 *   10.cfg 12.life 13.combine 14.ce 15.regmove 17.lreg 18.greg 19.flow2 20.ce2
 *   23.sched2 25.jump2 26.mach): flag_schedule_insns is off at -O2 here, so the
 *   ONLY scheduler is sched2.  The old inference -- "-fno-schedule-insns2 is
 *   worse, therefore not sched2" -- is invalid: turning sched2 off changes
 *   EVERY block in the function, so it cannot acquit the pass at one site.
 *   (-fno-schedule-insns2 on this body: 112 differing, size and count still
 *   exact.  -fschedule-insns, which switches sched1 ON, is byte-identical.)
 *
 * SUPERSEDED CLAIM 2 -- tools/split_s.py DOES have --dry-run.  The header below
 *   warns it "ignores the flag and performs the split".  It does not: DRY is a
 *   module global set by -n/--dry-run and it guards every mutation.
 *   `python3 tools/split_s.py --dry-run asm/overlays/rom_77a7c8/ovl_30_c_c_a_c_c.s
 *    OvlFunc_881_200b9fc` prints and writes nothing:
 *      would write ovl_30_c_c_a_c_c_a.s  (1 function,  82 lines)
 *      would write ovl_30_c_c_a_c_c_b.s  (1 function, 608 lines)
 *      would REMOVE ovl_30_c_c_a_c_c.s, would rewrite overlays/rom_77a7c8/overlay.ld
 *   tools/datacheck.py on the reference is SILENT -- no data section, no label
 *   needs .global.  Landing file: src/overlays/rom_77a7c8/ovl_30_c_c_a_c_c_b.c.
 *
 * SUPERSEDED CLAIM 3 -- "name the destination in a local" is NOT simply WORSE.
 *   It is the KEY SHAPE.  It buys the ROM's ORDER OUTRIGHT and leaves exactly
 *   one wrong register.  See below.
 *
 * ----------------------------------------------------------------------------
 * WHY THE TWO INSTRUCTIONS ARE IN THE WRONG ORDER -- MECHANISM, FROM THE RTL
 * ----------------------------------------------------------------------------
 *
 * (1) EXPAND.  calls.c's precompute_register_parameters hoists any register
 *     argument whose `rtx_cost (value, SET) > 2` into a pseudo BEFORE any hard
 *     argument register is touched, walking args FORWARD (arg0 first).  For
 *     `__DecompressLZ(gScript..., (char *)buf + 0x1000)` BOTH args qualify --
 *     arg0 is a SYMBOL_REF (ARM CONST_COSTS gives a symbol 6) and arg1 is a
 *     PLUS -- so 00.rtl reads, verbatim:
 *         insn 40  reg42 = MEM(*.LC2)          <- arg0, hoisted FIRST
 *         insn 42  reg43 = 4096
 *         insn 44  reg44 = reg33(buf) + reg43
 *         insn 46  r0 = reg42                  (deleted by reload)
 *         insn 48  r1 = reg44                  (deleted by reload)
 *     reload coalesces reg42->r0, reg43->r2, reg44->r1 and the thumb constant
 *     splitter turns reg43's set into `mov r2,#0x80 / lsl r2,#5` IN PLACE, so
 *     the pre-sched2 stream is  ldr r0 / mov r2 / lsl r2 / add r1.
 *     THE LDR IS FIRST BECAUSE arg0 IS PRECOMPUTED FIRST.  That is the whole
 *     source of the transposition.
 *
 * (2) sched2.  Read straight off `-fsched-verbose=5` (the dependence table
 *     prints insn/code/bb/dep/prio/cost and then INSN_DEPEND):
 *         insn 40  ldr r0   prio 43  cost 2  dependents {49, 59, 89}  = 3
 *         insn 44  add r1   prio 43  cost 1  dependents {49, 59, 89}  = 3
 *     PRIORITY IS TIED and the DEPENDENT COUNT IS TIED, so rank_for_schedule
 *     falls through to INSN_LUID and the LOWER LUID wins -- the ldr.  Trace:
 *         Ready list after queue_to_ready:  40  44
 *         Ready list (t =172):              44  40
 *         --> scheduling insn <<<40>>>
 *     (The list is sorted ascending and the LAST element is taken.)
 *     Insn 59 is the first insn after the NOTE_INSN_LOOP_BEG that LOCK_IME's
 *     `do {} while (0)` plants, and 89 ends the block; both are structural and
 *     BOTH ARE SHARED, so no spelling can unbalance them.  The ROM's stream has
 *     NO later write to r1 inside this basic block, so insn 44 can never earn a
 *     fourth dependent.  THE BASELINE SHAPE CANNOT SCHEDULE THE ROM'S WAY.
 *
 * (3) THE TIE CAN GO THE OTHER WAY, AND A LANDED SIBLING PROVES IT.
 *     src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_c_c_b.c (MATCHING) emits
 *         mov r1,#200 / lsl r1,r1,#4 / ldr r0,.L3+4 / bl __StartTask
 *     -- arg1's chain tail BEFORE the pool ldr, exactly the order we want.
 *     Its table:  insn 86 (ldr r0) prio 77 dependents {147,93} = 2
 *                 insn 168 (lsl r1) prio 77 dependents {147,98,93} = 3
 *     168 wins on the DEPENDENT COUNT.  The extra dependent is insn 98, a later
 *     `r1 = 0`; the ldr gets no matching later-r0 dependent because __StartTask
 *     RETURNS A VALUE, so the call_value_insn's own `set r0` intercepts r0's
 *     chain.  __DecompressLZ is void, and this function's block has no later r1
 *     write, so neither half of that asymmetry is available here.
 *     Corpus figure, for calibration: across the 4,371 generated .s files the
 *     r1-set-then-pool-ldr-then-bl order occurs 199 times and the opposite 335.
 *
 * ----------------------------------------------------------------------------
 * THE KEY SHAPE: NAME THE DESTINATION IN A LOCAL.  ORDER EXACT, ONE REGISTER.
 * ----------------------------------------------------------------------------
 *
 *     char *dst;
 *     dst = (char *)buf + (0x80 << 5);
 *     __DecompressLZ(gScript_943__0200c4ec, dst);
 *
 * arg1 is now already a REG at precompute time, so only arg0 is hoisted and the
 * pre-sched2 stream becomes  mov / lsl / add r1 / ldr r0 -- LUID(add) <
 * LUID(ldr), and sched2 emits the ROM'S EXACT ORDER, right down to the pool
 * displacement (`ldr r0, [pc, #264]`, matching the ROM; the installed body
 * reads #268).  It measures 3 of 579, size and count exact, aligned 576
 * (99.5%) in ONE hunk, and ALL THREE differing encodings are the same register:
 *       ref   mov r2,#0x80 / lsl r2,r2,#5 / add r1,r7,r2
 *       ours  mov r0,#0x80 / lsl r0,r0,#5 / add r1,r7,r0
 * WHY r0: in the baseline the 0x1000 pseudo overlaps the symbol pseudo (which
 * is coalesced to r0), so r0 is in its conflict set and find_reg takes r2.
 * Moving the address computation ahead of the call ends that overlap, r0 is
 * free, and REG_ALLOC_ORDER's first free register is r0.  The two requirements
 * are in direct opposition: the conflict that buys r2 needs the ldr EARLY, and
 * the order needs it LATE.
 *
 * *** DIAGNOSTIC, diff = 0.  DO NOT SHIP -- IT IS A PIN IN DISGUISE. ***
 *   Add, to the key shape, an asm-label alias with a bogus third argument so
 *   the 0x1000 pseudo inherits arg2's hard-register preference:
 *       extern void __DecompressLZ3(const void *, void *, int)
 *           __asm__("__DecompressLZ");
 *       off2 = 0x80 << 5;
 *       dst = (char *)buf + off2;
 *       __DecompressLZ3(gScript_943__0200c4ec, dst, off2);
 *   BYTE-IDENTICAL: diff=0, size 1300/1300, count 579/579, relocations same.
 *   It is a fake -- it lies about a real game function's arity purely to move a
 *   register -- and it is recorded ONLY because of what it proves: EVERY OTHER
 *   BYTE OF THIS FUNCTION IS ALREADY RIGHT IN THE KEY SHAPE.  The whole
 *   remaining problem is the sentence "the 0x1000 pseudo must be allocated r2".
 *   (`register int off2 __asm__("r2")` does NOT work -- gcc-2.96 ignores a local
 *   register asm variable that appears in no asm operand: still 3.)
 *
 * ----------------------------------------------------------------------------
 * WHAT BATCH 305 RULES OUT (all size- and count-exact unless stated)
 * ----------------------------------------------------------------------------
 * ON THE KEY SHAPE, ALL 3 -- the address spelling is IRRELEVANT to the
 * allocation; it only ever controlled the order:
 *   destination in a fresh `char *dst`                                3
 *   destination in the existing `off` (int, reused)                   3
 *   destination named, then source named after it                     3
 *   offset in its own local, then destination                         3
 *   offset in `off`, then destination                                 3
 *   offset built in TWO statements (`off2 = 0x80; off2 <<= 5;`)       3
 *   offset local declared BEFORE `dst` (allocno order)                3
 *   offset local as `unsigned int`                                    3
 *   `dst = (char *)((0x80 << 5) + (int)buf)` (reversed add)           3
 *   `dst = &((char *)buf)[0x80 << 5]`                                 3
 *   destination as an `int` and cast at the call                      3
 *   `0x1000` written literally instead of `0x80 << 5`                 3
 *   `dst = off2 + (char *)buf`                                        3
 *   `dst = (char *)buf; dst += off2;` (range extended backwards)      3
 * SOURCE NAMED FIRST reverts to the baseline exactly (2, first diff at index
 *   22) -- naming the source restores the r0 conflict AND the ldr's low LUID.
 * THE REUSE-A-VARIABLE DONOR HUNT FAILED, and the failure is informative.
 *   The lever needs a variable whose OTHER live range already lands in r2.
 *   `off` (the gState offset) lands in r1 in the ROM -- `movs r1,#250 / lsls
 *   r1,#1` at 0xd2 -- so it is the wrong donor.  Every other r2 in this
 *   function belongs to a PushPal/PushTiles/PushFrame inline (count, task, the
 *   DMA control words, `adds r2,r7,r0`), and an inline's locals get fresh
 *   pseudos per instantiation, so they cannot be shared.  The one remaining
 *   candidate -- sharing a variable with the `0xe4 << 1` offset, which the ROM
 *   DOES hold in r2 (`movs r2,#228` at 0xe6) -- is CATASTROPHIC: naming that
 *   offset at all REMOVES two instructions (577 of 579, 1296 bytes, 456
 *   differing, relocations shifted), with or without the destination change.
 *   That confirms this header's own lever-3 note: a POINTER base plus a
 *   constant offset already emits the runtime add, and a named local there
 *   costs instructions rather than buying them.
 * FLAGS ON THE KEY SHAPE, all still 3, size and count exact:
 *   -fno-caller-saves, -fno-regmove, -fno-gcse, -fno-strength-reduce,
 *   -fno-expensive-optimizations, -fno-cse-follow-jumps, -fno-force-mem,
 *   -fno-thread-jumps, -fno-function-cse, -fno-peephole, -fschedule-insns,
 *   -fno-reorder-blocks, -fno-delete-null-pointer-checks.
 *   WORSE: -fno-schedule-insns2 (112 differing); -fno-rerun-cse-after-loop
 *   (585 differing, 1412 bytes, 621 encodings, relocations differ).
 *   NO FLAG MOVES THE ALLOCATION.  A Makefile row cannot land this.
 *
 * NEXT, FOR WHOEVER PICKS THIS UP.  The question is now narrow and precise:
 * make a conflict-free pseudo take r2 instead of REG_ALLOC_ORDER's first free
 * register, WITHOUT an r0 conflict and WITHOUT lying about a callee's arity.
 * Two untried directions, both honest: (a) find a value in THIS function that
 * the ROM keeps in r2 across a range disjoint from the offset's -- the donor
 * search above covered the declared locals and the inline bodies but not the
 * possibility that one of the three PushFrame/PushPal/PushTiles inlines should
 * be spelled so one of ITS locals is a function-level variable; (b) attack the
 * dependent-count term instead of the allocation, by finding a spelling in
 * which `__DecompressLZ`'s return value or a later r1 write legitimately exists
 * inside this basic block, which is what the landed sibling in (3) has and this
 * function lacks.  The superseded "goto into a do/while" and "move the call's
 * statement boundary" suggestions are now pointless: the do/while barrier is
 * already there (LOCK_IME) and it feeds BOTH sides of the tie equally.
 * ============================================================================
 *
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

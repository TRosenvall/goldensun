/* BufferString -- 0x08018038, asm/rom_15000/rom_17e88_a_a_c.s
 *
 * NON-MATCHING, 325 of 745 encodings differ.
 *
 * SIZE IS EXACT (1620 bytes both sides).  COUNT IS NOT: 746 instructions
 * against the ROM's 745, so the objcmp figure above SATURATES and the
 * position-tolerant number is the one to rank by:
 *
 *   aligncmp  617 of 745 aligned-equal = 82.8%, 167 differing/ins/del in 61 hunks
 *
 * RELOCATIONS: the 35-entry symbol SEQUENCE is the ROM's, in order, with all
 * ten `_call_via_r9` veneers on the correct calls, EXCEPT one extra entry --
 * `R_ARM_ABS32 _FUNC_8015430_SIZE` at 0x144, where the hand-written reference
 * carries an unrelocated pool word 0x00000140.  See "THE SIZE POOL WORD" below:
 * that is an artifact of the reference being a DISASSEMBLY (the linker had
 * already resolved the symbol), not a defect, and the literal spelling measures
 * strictly worse on both axes.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/BufferString.c \
 *     asm/rom_15000/rom_17e88_a_a_c.s --func BufferString
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_15000/BufferString.c \
 *     asm/rom_15000/rom_17e88_a_a_c.s BufferString
 *
 * SPLIT SHAPE: NONE.  tools/datacheck.py on asm/rom_15000/rom_17e88_a_a_c.s
 * reports no data section and no required data export; the file holds this one
 * function.  Elevating it replaces that one .s in place -- no split_s.py run, no
 * stage1.ld edit.
 *
 * SHIMS: tools/shimcount.py reports NONE -- PIN-FREE.  The `register ... __asm__`
 * declarations reached through include/dma.h belong to the shared DMA3_SET
 * inline, exactly as in the already-landed src/rom_15000/rom_1908c_c_c_a.c,
 * which carries no fakematch.txt row.  This file needs no row either.
 *
 * INSTALL PREREQUISITE, one line outside this file:
 *   asm/rom_15000/rom_15430.s:92   .func_end Func_8015430
 *   becomes                        .func_end_emit_size Func_8015430, _FUNC_8015430_SIZE
 * That is the same macro rom_15430.s already uses at lines 121 and 494 for
 * Func_8015570 and Func_80158e8.  Without it the TU will not link.
 *
 * ========================================================================
 * WHAT IT DOES
 * ========================================================================
 * The layout engine of the text module.  It opens a Huffman stream on the
 * string id, walks it one code at a time, and fills the 0x200-entry halfword
 * ring at ctx+0xEB0 (ctx = iwram_3001e8c[0]), wrapping every write index with
 * `& 0x1ff`.  Codes above 0x1F are glyphs; codes 0x00..0x1F are control codes
 * dispatched through TWO switches -- one for the normal path and a reduced one
 * that runs after the engine has emitted an ellipsis and is only scanning for
 * the codes that end the run.  Control codes pull in party-member names
 * (_GetUnit), place names (_GetLocationName), numbers (PrintNum) and
 * article/plural decoration (Func_8017e88, already EXACT next door in
 * src/rom_15000/rom_17e88_a_a_b.c).  It returns the ring index the laid-out text
 * starts at, and publishes the new head at ctx+0x12B2 and that start index at
 * ctx+0x12B4.  strId == -1 means "re-read the last start index and do nothing".
 *
 * ========================================================================
 * LEVERS THAT PAID, IN ORDER, WITH FIGURES
 * ========================================================================
 * Draft 1 (naive, both switches written with stacked `case` labels and no
 * out-of-range case) measured 1740 bytes against 1620, 773 instructions against
 * 745, 747 differing -- and 34 `R_ARM_ABS32 .text` relocations the ROM does not
 * have.  Three edits took it to size-exact and 82.8% aligned:
 *
 * 1. A DECISION TREE INSTEAD OF A JUMP TABLE IS ITSELF A MISSING-CASE TELL,
 *    AND THE MISSING CASE IS `case -1:`.   Worth 124 bytes and 27 instructions
 *    in one edit -- 1740 -> 1616 bytes, 773 -> 744 instructions, 747 -> 641
 *    differing, and it deleted all 34 phantom relocations.
 *
 *    stmt.c's expand_end_case picks the DECISION TREE over `casesi` when
 *
 *        count < case_values_threshold ()
 *        || (unsigned) range > 10 * count          (range = maxval - minval)
 *
 *    The reduced switch here has 15 case labels over 0x00..0x1E: range 30,
 *    10 * count = 150, so the formula says TABLE and gcc duly emitted one --
 *    a 31-entry `.word` table plus `lsl r3,#2 / ldr r3,[r3,r2] / mov pc,r3`.
 *    The ROM has a balanced `cmp`/`beq` tree instead.  The ONLY way to get a
 *    tree with the same visible case values is a case value far outside them,
 *    and with an UNSIGNED selector `case -1:` is 0xFFFFFFFF, which makes range
 *    0xFFFFFFFF and flips the test.  The full main switch already had one, which
 *    is why the main switch was a tree in draft 1 and the reduced one was not.
 *
 *    AND ITS OWN TEST DISAPPEARS, which is why the node leaves no trace.  Node
 *    0xFFFFFFFF sorts LAST, so it becomes the right child of node 0x1E, and
 *    emit_case_nodes' single-valued/right-child-only path ends in
 *    `do_jump_if_equal(index, -1, that case's code_label)`.  In the reduced
 *    switch `case -1:` has an EMPTY body, so its label collapses onto the end of
 *    the switch -- which is also the default label, because that switch has no
 *    default body.  jump.c then deletes a conditional jump whose target equals
 *    the following unconditional jump's, and all that survives is
 *
 *        cmp r7, #0x1e / beq <body> / b <end>
 *
 *    In the MAIN switch the same node DOES survive, as `mov r2,#1 / neg r2,r2 /
 *    cmp r7,r2 / bne .. / b ..`, because there the default has a real body so
 *    the two labels differ.  Both shapes are in the ROM, twelve instructions
 *    apart, and they are the SAME source construct.
 *
 * 2. TWO CASES WHOSE VALUES ARE ADJACENT MUST BE WRITTEN AS TWO DUPLICATED ARMS,
 *    NOT ONE STACKED PAIR.  Worth 4 bytes and 2 instructions, and it is what
 *    made SIZE exact: 1616 -> 1620 bytes (the reference figure), 641 -> 327
 *    differing, 74.9% -> 82.0% aligned.
 *
 *    stmt.c's group_case_nodes merges two case nodes into one RANGE node when
 *    they are numerically contiguous AND SHARE A code_label.  Stacked labels
 *    (`case 0x11: case 0x12:` over one body) share a label, so they merge and
 *    emit `cmp #0x11 / bcs` -- one node where the ROM has two.  Writing the body
 *    TWICE gives two labels, no merge, two nodes, and jump.c cross-jumps the
 *    identical bodies back to one block for free.  The discriminator is purely
 *    numeric adjacency: in the same switch `case 8: case 9:` MUST stay stacked
 *    (the ROM's `cmp #9 / bhi` + `cmp #8 / bcs` IS a range node) while
 *    `case 0: case 2: case 0x1e:` may stay stacked because nothing is adjacent.
 *
 * 3. THE THREE 15-HALFWORD WIDENING COPIES MUST BE `do { } while (i <= 0xe)`,
 *    NOT `for (i = 0; i <= 0xe; i++)`.  82.0% -> 82.8% aligned, size and count
 *    unchanged.  `for` lets loop.c's check_dbra_loop reverse the counter -- the
 *    counter is dead in the body, so gcc emits `mov r0,#14 / sub r0,#1` counting
 *    DOWN.  Spelling the increment as a statement inside an explicit do-while
 *    blocks the reversal and gives the ROM's `add r4,#1 / cmp r4,#0xe / bls`
 *    exactly, including sched2 putting the increment between the `ldrb` and the
 *    `strh`.  Probed in isolation first (five spellings; only the do-while
 *    counts up).  `i` must be UNSIGNED for `bls`.
 *
 * 4. DECLARATION ORDER OF THE SPILLED SCALARS, which is the whole set of stack
 *    offsets.  Folded into step 2's measurement.  The frame is
 *
 *        sp+0x00..0x0b  outgoing stack args for Func_8017e88 (7 args)
 *        sp+0x0c        compiler temp holding &strbuf, hoisted into the loop
 *        sp+0x10..0x30  NINE spilled pseudos
 *        sp+0x34        endsInS (address-taken, so a real slot)
 *        sp+0x38..0x43  the 12-byte Huffman stream object
 *        sp+0x44..0x53  numbuf[16] for PrintNum
 *        sp+0x54..0x83  strbuf[24] halfwords
 *
 *    ARM's frame grows DOWNWARD, so the FIRST-declared local gets the HIGHEST
 *    sp offset and the reload spill area continues below the declared locals.
 *    That reads the declaration order straight off the disassembly: strbuf,
 *    numbuf, huff, endsInS for the aggregates, then -- because alter_reg walks
 *    pseudos in NUMBER order, which is declaration order -- ctx(0x2c), varA(0x28),
 *    varB(0x24), startIdx(0x20), count(0x1c), flag18(0x18), loopFlag(0x14),
 *    flag10(0x10) for the scalars.  strId at 0x30 is the incoming argument,
 *    spilled by the first instruction of the body.
 *
 * 5. ONE SHARED FUNCTION-LEVEL `i` FOR FOUR DISJOINT COUNTERS, not one per
 *    block.  MEASURED BOTH WAYS: shared is size-exact at 746/82.8%,
 *    brace-scoped per case is 1612 bytes (8 short), 742 instructions and 79.3%.
 *    This confirms UpdateSpriteAnim's "one `int i` for five disjoint counters"
 *    and bounds batch 307's brace-scope-per-case lever: that lever is about the
 *    SUBJECT of a switch arm, not about its loop counters.  The same result for
 *    `v`: per-case block-scoped `v` is 746/82.8%, hoisted to function scope it
 *    is 1616 bytes and 78.8%.
 *
 * ========================================================================
 * THE SIZE POOL WORD -- WHY THE EXTRA RELOCATION IS THE RIGHT ANSWER
 * ========================================================================
 * The prologue copies the ARM Huffman reader Func_8015430 into IWRAM with the
 * usual galloc + DMA3_SET pair, and the ROM keeps its size in a register:
 *
 *     ldr r5, =0x140 / mov r0,#0x32 / mov r1,r5 / bl galloc_iwram
 *     mov r2,#0x84 / lsr r5,#2 / lsl r2,#24 / orr r2,r5
 *
 * `orr r2,r5` is the tell.  0x84000000 | 0x50 is a compile-time constant if the
 * size is a literal, and gcc would emit it as one pool load; a RUNTIME `orr`
 * against `size >> 2` proves the size is not foldable, i.e. it is the linker
 * symbol, exactly as src/rom_15000/rom_1908c_c_c_a.c reads Func_8015570's.
 * MEASURED: `_FUNC_8015430_SIZE` gives 1620 bytes / 746 instructions / 82.8%;
 * `#define FUNC_8015430_SIZE 0x140` gives 1612 bytes / 742 instructions / 82.6%
 * and loses the `orr`.  Size-and-count ranks first, so the symbol wins, and the
 * one relocation objcmp flags exists only because the reference .s is a
 * disassembly in which the linker had already written 0x140 into the pool.
 *
 * ========================================================================
 * WHAT REMAINS -- 167 hunk entries, and ALL of it is one pass
 * ========================================================================
 * The residue is register assignment and insn placement out of local/global
 * register allocation and reload.  The one-instruction count surplus is fully
 * accounted for by exactly three items, and they cancel in bytes, which is why
 * SIZE comes out exact:
 *
 *   D1  cases 0x13 and 0x14: `v = fn(huff) - 1;` comes out as `mov r5,r0` at the
 *       call and `sub r5,r5,#1` only at the use, AFTER DecompressString, where
 *       the ROM has the single `sub r5,r0,#1` next to the call.  +2 insns,
 *       +4 bytes.  THREE spellings measured and ALL INERT to the last encoding:
 *       `v = fn(huff) - 1;`, `v = fn(huff); v--;`, and
 *       `t = fn(huff); v = t - 1;`.  Not reachable from the source.
 *   D2  case 0x14: the offset 0x182 comes out as `mov r2,#193 / lsl r2,#1` where
 *       the ROM spends a pool word on it.  +1 insn, -2 bytes net.  0x182 IS of
 *       the form imm8 shifted, so this build's constant synthesis prefers it and
 *       nothing in the source chooses the pool.  Compare the neighbouring 0x741
 *       and 0x333 and 0x99b, which are NOT of that form and are pooled on both
 *       sides and match.
 *   D3  case 0x11: `v = fn(huff) - 1; p = _GetUnit(v);` folds to `sub r0,#1`
 *       where the ROM spends `sub r2,r0,#1 / mov r0,r2`.  -1 insn, -2 bytes.
 *       The ROM's shape is the WORSE allocation; we cannot ask for it.
 *
 * Everything else in the 61 hunks is register LETTERS following from the same
 * allocator decisions -- `i`/`len` in r0 where the ROM uses r4 (so the three
 * copy loops each spend one extra `mov` to get the _GetUnit result out of r0,
 * and case 0x16's `subs r4,r0,r5` reads `subs r0,r0,r5`), `ctx` reloaded into r5
 * where the ROM uses r0, and the case 0x1a pair swapping r2 and r3 -- plus about
 * eight sched2 orderings in the prologue that hang off `mov r10,r0` being placed
 * early instead of late.  FOUR declaration positions for `i` were measured and
 * every one is byte-identical to this file, so declaration position does not
 * reach a pseudo that never spills.
 *
 * BLOCKER, attributed: local_alloc/global_alloc's choice of hard register for
 * the shared counter pseudo, and reload's insn placement for D1.  There is no
 * source-level handle on either; the structure, the frame, the switch trees, the
 * relocation sequence and the size are all the ROM's.
 *
 * NEXT, in order:
 *   1. Nothing structural.  Anyone resuming should start from the three items
 *      above and treat the rest as downstream of `i`'s register.
 *   2. Re-screen the other 34 hand-written functions carrying batch 307's
 *      lopsided-dispatch shape, and ALSO every parked function whose switch came
 *      out as a TABLE where the ROM has a TREE -- lever 1 above is a second,
 *      cheaper tell for the same missing case and it does not need the dispatch
 *      shape read at all.
 */
#include "dma.h"

extern void *iwram_3001e8c[];
extern unsigned char gState[];

extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void HuffStr_Start(void *state, int id);
extern int Func_8019944(int key, int flag);
extern void DecompressString(int id, void *dst, int mode);
extern char *PrintNum(char *dest, int num, unsigned int width);
extern unsigned char *_GetUnit(int id);
extern int _GetLocationName(int a, int b);
extern int Func_8017e88(int lineBreak, unsigned short *str, int idx,
                        unsigned short *ring, int mode, int plural, int *endsInS);
extern void Func_80198dc(void);
extern void Func_8015430(void);
extern char _FUNC_8015430_SIZE[];
#define FUNC_8015430_SIZE ((u32) _FUNC_8015430_SIZE)

int BufferString(int strId, int mode)
{
    unsigned short strbuf[24];
    char numbuf[16];
    int huff[3];
    int endsInS;
    unsigned char *ctx;
    int varA;
    int varB;
    int startIdx;
    unsigned int count;
    int flag18;
    int loopFlag;
    int flag10;
    unsigned short *ring;
    unsigned int (*fn)(void *);
    unsigned int c;
    int idx;
    unsigned int i;

    ctx = (unsigned char *)iwram_3001e8c[0];
    varA = 1;
    varB = 0;
    idx = *(unsigned short *)(ctx + 0x12b2);
    startIdx = idx;
    loopFlag = 1;
    ring = (unsigned short *)(ctx + (0xeb << 4));
    c = 0;
    count = 0;
    flag18 = 0;
    endsInS = 0;
    flag10 = 0;
    if (strId == -1) {
        startIdx = *(unsigned short *)(ctx + 0x12b4);
    } else {
        void *code;

        code = galloc_iwram(0x32, FUNC_8015430_SIZE);
        DMA3_SET(Func_8015430, code, 0x84000000 | (FUNC_8015430_SIZE / 4));
        fn = (unsigned int (*)(void *))iwram_3001e8c[0x23];
        HuffStr_Start(huff, strId);
        for (;;) {
            unsigned int prev;

            prev = c;
            c = fn(huff);
            if (c > 0xff)
                c = 0x40;
            if (flag10 != 0) {
                if (c <= 0x1f) {
                    switch (c) {
                    case 0:
                    case 2:
                    case 0x1e:
                        loopFlag = 0;
                        break;
                    case 0x13:
                        fn(huff);
                        Func_8019944(3, mode);
                        break;
                    case 0x16:
                        Func_8019944(5, mode);
                        break;
                    case 0x14:
                        fn(huff);
                        Func_8019944(2, mode);
                        break;
                    case 0x15:
                        Func_8019944(4, mode);
                        break;
                    case 0x17:
                        Func_8019944(6, mode);
                        break;
                    case 0x11:
                        fn(huff);
                        break;
                    case 0x12:
                        fn(huff);
                        break;
                    case 8:
                    case 9:
                    case 0x1d:
                        fn(huff);
                        break;
                    case 1:
                        loopFlag = 0;
                        c = 2;
                        break;
                    case 0x10:
                        break;
                    case -1:
                        break;
                    }
                }
            } else {
                if (ctx[0x12fa] != 0 && varA == 0 && c != 0xde && c != 0xdf) {
                    ring[idx] = 5;
                    idx = (idx + 1) & 0x1ff;
                }
                if (ctx[0x12fb] != 0 && varA == 0 && c != 0xde && c != 0xdf
                    && prev <= 0x100 && prev > 0x7f && prev != 0xde
                    && prev != 0xdf && prev != 0x20 && prev != 0xa5
                    && prev != 0xa1 && prev != 0xa4) {
                    ring[idx] = 0xde;
                    idx = (idx + 1) & 0x1ff;
                }
                if (c > 0x1f) {
                    if (ctx[0x12fa] != 0 && (c == 0x20 || count > 0xa)) {
                        ring[idx] = 0x2e;
                        idx = (idx + 1) & 0x1ff;
                        ring[idx] = 0x2e;
                        idx = (idx + 1) & 0x1ff;
                        ring[idx] = 0x2e;
                        idx = (idx + 1) & 0x1ff;
                        flag10 = 1;
                        if (count > 0xa)
                            c = 0x20;
                    }
                    if (c == 0x22) {
                        varB ^= 1;
                        if (varB != 0)
                            c = 0x8e;
                    }
                    ring[idx] = c;
                    idx = (idx + 1) & 0x1ff;
                    varA = 0;
                } else {
                    switch (c) {
                    case 0:
                    case 2:
                    case 0x1e:
                        loopFlag = 0;
                        break;
                    case 8:
                    case 9:
                    case 0x1d:
                        ring[idx] = c;
                        idx = (idx + 1) & 0x1ff;
                        ring[idx] = fn(huff) + 0xffff;
                        idx = (idx + 1) & 0x1ff;
                        break;
                    case 0x16:
                        {
                            int v;
                            int a;

                            v = Func_8019944(5, mode);
                            a = v;
                            if (v < 0)
                                a = -v;
                            flag18 = 1;
                            if (a <= 1)
                                flag18 = 0;
                            i = PrintNum(numbuf, v, 0) - numbuf;
                            while (i != 0x10 && numbuf[i] != 0) {
                                ring[idx] = numbuf[i];
                                idx = (idx + 1) & 0x1ff;
                                i++;
                            }
                        }
                        break;
                    case 0x13:
                        {
                            int v;

                            v = fn(huff) - 1;
                            DecompressString(Func_8019944(3, mode) + 0x741,
                                             strbuf, 0x18);
                            idx = Func_8017e88(0, strbuf, idx, ring, v, flag18,
                                               &endsInS);
                        }
                        break;
                    case 0x14:
                        {
                            int v;
                            int k;

                            v = fn(huff) - 1;
                            k = Func_8019944(2, mode) & 0x1ff;
                            DecompressString(k + 0x182, strbuf, 0x18);
                            idx = Func_8017e88(0, strbuf, idx, ring, v, flag18,
                                               &endsInS);
                        }
                        break;
                    case 0x15:
                        {
                            unsigned short *p;
                            short ch;
                            int n;

                            DecompressString(Func_8019944(4, mode) + 0x333,
                                             strbuf, 0x18);
                            p = strbuf;
                            n = idx;
                            while (*p != 0) {
                                ch = *p;
                                ring[n] = ch;
                                p++;
                                n = (n + 1) & 0x1ff;
                            }
                            idx = n;
                        }
                        break;
                    case 0x17:
                        {
                            unsigned short *p;
                            short ch;
                            int n;

                            DecompressString(_GetLocationName(Func_8019944(6, mode), 1)
                                             + 0x99b, strbuf, 0x18);
                            p = strbuf;
                            n = idx;
                            while (*p != 0) {
                                ch = *p;
                                ring[n] = ch;
                                p++;
                                n = (n + 1) & 0x1ff;
                            }
                            idx = n;
                        }
                        break;
                    case 0x10:
                        {
                            unsigned char *p;
                            unsigned short *q;
                            int koff;

                            koff = 0xfa << 1;
                            p = _GetUnit(*(int *)(gState + koff));
                            q = strbuf;
                            i = 0;
                            do {
                                *q = *p;
                                p++;
                                q++;
                                i++;
                            } while (i <= 0xe);
                            idx = Func_8017e88(0, strbuf, idx, ring, 0, 0,
                                               &endsInS);
                        }
                        break;
                    case 0x12:
                        {
                            unsigned char *p;
                            unsigned short *q;
                            int v;

                            v = fn(huff) - 1;
                            p = _GetUnit(Func_8019944(1, mode));
                            q = strbuf;
                            i = 0;
                            do {
                                *q = *p;
                                p++;
                                q++;
                                i++;
                            } while (i <= 0xe);
                            idx = Func_8017e88(0, strbuf, idx, ring, v, flag18,
                                               &endsInS);
                        }
                        break;
                    case 0x11:
                        {
                            unsigned char *p;
                            unsigned short *q;
                            int v;

                            v = fn(huff) - 1;
                            p = _GetUnit(v);
                            q = strbuf;
                            i = 0;
                            do {
                                *q = *p;
                                p++;
                                q++;
                                i++;
                            } while (i <= 0xe);
                            idx = Func_8017e88(0, strbuf, idx, ring, 0, 0,
                                               &endsInS);
                        }
                        break;
                    case 0x1a:
                        {
                            int t;

                            t = (fn(huff) - 1) << 1;
                            ring[idx] = t + 0x80;
                            idx = (idx + 1) & 0x1ff;
                            ring[idx] = t + 0x81;
                            idx = (idx + 1) & 0x1ff;
                        }
                        break;
                    case 0x18:
                        ring[idx] = 0x8f;
                        idx = (idx + 1) & 0x1ff;
                        ring[idx] = 0x2d;
                        idx = (idx + 1) & 0x1ff;
                        break;
                    case 0x19:
                        if (flag18 != 0) {
                            if (endsInS != 0) {
                                ring[idx] = 0x65;
                                idx = (idx + 1) & 0x1ff;
                            }
                            ring[idx] = 0x73;
                            idx = (idx + 1) & 0x1ff;
                        }
                        break;
                    case 0x1b:
                        ring[idx] = 0x27;
                        idx = (idx + 1) & 0x1ff;
                        if (endsInS == 0) {
                            ring[idx] = 0x73;
                            idx = (idx + 1) & 0x1ff;
                        }
                        break;
                    case 1:
                    case 3:
                        varA = 1;
                    default:
                        ring[idx] = c;
                        idx = (idx + 1) & 0x1ff;
                        if (c == 0x73 || c == 0x53)
                            endsInS = 1;
                        else
                            endsInS = 0;
                        break;
                    case -1:
                        break;
                    }
                }
            }
            count++;
            if (loopFlag == 0 || count > 0x1ff)
                break;
        }
        ring[idx] = c;
        idx = (idx + 1) & 0x1ff;
        ring[idx] = 0;
        idx = (idx + 1) & 0x1ff;
        *(unsigned short *)(ctx + 0x12b2) = idx;
        gfree(0x32);
        *(unsigned short *)(ctx + 0x12b4) = startIdx;
    }
    if (mode != 0)
        Func_80198dc();
    return startIdx;
}

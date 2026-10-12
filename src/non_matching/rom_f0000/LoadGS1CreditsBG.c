/* LoadGS1CreditsBG -- park. The subject line must stay FIRST: tools that identify a
 * park by its subject (piece.py, autosweep.py, shortrefs.py, scratchfill.py)
 * read line 1, and a note prepended above it makes them resolve the wrong
 * function or no function at all. Notes go UNDER this line. Added in batch
 * 341 after the pre-fix autosweep was found sweeping wrong subjects.
 */
/* NOTE (batch 336): THE SIBLING HAS LANDED, and this park's reference .s was
 * renamed by the split that did it.  Func_80f0254 is now
 * src/rom_f0000/rom_f0254_a_a_b.c (byte-identical, 92 bytes, 39 encodings);
 * this function is alone in rom_f0254_a_a_c.s, and the recipes below are
 * updated to it.  parkcheck read UNCHECKABLE until then.
 *
 * So the header line below saying the piece holds two functions and that the
 * other one blocks this one is HISTORY: this park now converts its piece
 * WHOLE on its own, and its figure is the only thing left.
 */
/* LoadGS1CreditsBG -- asm/rom_f0000/rom_f0254_a_a_c.s (2 functions; the sibling Func_80f0254 is
 *
 * NON-MATCHING, 3 of 87 encodings  (MEASURED, batch 336 brief C; was fourteen).
 *   STREAM terms: ref 87, ours 87; instructions 72/72; SIZE equal; first
 *   differing index 43.  Of the 3, TWO are the real residue and ONE is the pool
 *   word holding `_SIZE_80f0024`'s relocation placeholder -- with the symbol
 *   resolved (measured with `__asm__(".equ _SIZE_80f0024, 0x230")` as a labelled
 *   INSTRUMENT) the figure is 2 of 87 with the relocation sets IDENTICAL.
 *   Pin-free (tools/shimcount.py: no rows), device-free, no per-file flags.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f0000/LoadGS1CreditsBG.c \
 *     asm/rom_f0000/rom_f0254_a_a_c.s --func LoadGS1CreditsBG
 *
 * ***** THIS FUNCTION IS NOW THE WHOLE GATE ON A TWO-FUNCTION PIECE.
 * ***** asm/rom_f0000/rom_f0254_a_a_c.s holds exactly two functions, and the other
 * ***** one -- Func_80f0254 -- went 2 -> 0 in batch 336 and is BYTE-IDENTICAL.
 * ***** Closing these 2 lands TWO functions.  The merged TU was built and
 * ***** measured with `objcmp --whole`: `ok Func_80f0254  39 encodings` /
 * ***** `XX LoadGS1CreditsBG  2 of 87`.  No interaction between the two bodies.
 *
 * MEASUREMENT TRAP, for whoever picks this up: `objcmp --func NAME` on a
 * TWO-FUNCTION candidate compares the WHOLE candidate object against the single
 * reference function and reports ~124 differing of 87 with "ours 104"
 * instructions.  USE `--whole` FOR THE MERGED TU.
 *
 * ===== THE RELOCATION ALARM IN THE INHERITED HEADER IS WRONG. =====
 *
 * The old text says the figure "is NOT a distance" because the relocation sets
 * differ on `_SIZE_80f0024`, and that "`make compare` cannot pass a relocation
 * difference."  That is false for an ABSOLUTE LINKER-SCRIPT SYMBOL.  An
 * absolute assignment in a `.sym` emits no bytes and resolves at LINK time, so
 * the .o legitimately carries an R_ARM_ABS32 placeholder where the hand-written
 * .s has a literal.  tools/symrefs.py shows `_SIZE_8015afc` with TWO LANDED
 * users (src/rom_15000/rom_19ebc_c_b.c, src/rom_15000/rom_20198_c_c_c_c_a_a.c)
 * and size.sym carries five such entries with the build green.
 *
 * SO THE SYMBOL QUESTION IS BACK TO THE ORDINARY TEST, and the inherited
 * header's own reasoning still applies: owner-decisions.md standard 2 says a
 * `.sym` entry needs structural impossibility AND must COMPLETE its function.
 * The structural case is unchanged and strong (the arithmetic is verified:
 * Func_80f0024 @ 0x080f0024, Func_80f0254 @ 0x080f0254, gap exactly 0x230, and
 * four existing _SIZE_ entries sit on the same footing).  COMPLETION is now
 * TWO ENCODINGS away rather than thirteen -- and those two are bounded below.
 *
 * ===== WHAT CLOSED 14 -> 2, FROM THE LANDED CORPUS, NOT FROM SPELLING. =====
 *
 * The residue at 14 was three clusters: an adjacent swap at the indirect call's
 * argument setup (2); a pure r0 <-> r1 RENAME across the whole IME/queue block,
 * which also swapped two pool words and moved gDMATaskCount's relocation (9);
 * and an `adds r2,#1` / `stmia r3!,{r6}` swap (2).
 *
 * `gDMATaskCount` appears in TWELVE landed, byte-matching generated `.s` files,
 * and in almost all of them the pool order is `gDMATaskCount` then 0x04000208
 * (REG_IME) -- the reference order.  EVERY ONE of their sources uses the same
 * canonical queue-push body, and it differs from the inherited `PushDma` in
 * exactly two places:
 *
 *   1. the IME save-and-disable pair is wrapped in a `do { } while (0)` macro
 *      (`LOCK_IME`) -- which THIS FILE ALREADY DEFINED AND THEN DID NOT USE,
 *      substituting a named `vu16 *ime` pointer;
 *   2. the count store `*(u16 *)queue = count + 1;` comes FIRST, before
 *      `*task++ = (u32)src;`, not between the two task stores.
 *
 * src/overlays/rom_7795e8/ovl_30_c_c_a_a_a_b.c's header states mechanism (1) in
 * terms: "`do { } while (0)` IS A SCHEDULING BARRIER THAT COSTS NOTHING ... the
 * ROM materialises the queue address before the IME port address, gcc did the
 * reverse.  Wrapping the save-and-disable pair in `do/while(0)` splits the block
 * into two scheduling regions and closes it exactly -- with no instruction
 * emitted, unlike the volatile-asm barrier".  It also records that the
 * volatile-asm barrier lands the pool load too early (9) or still needs help
 * (3), and that PINNING THE QUEUE POINTER TO r4 IS BADLY INERT (68 of 73).
 *
 * BOTH ARE REQUIRED AND NEITHER ALONE DOES IT.  The 3x3 cross:
 *
 *                       count first   between   last
 *   LOCK_IME                   7         13      18
 *   named `ime` pointer       12*        13*     22*     (* relocations WRONG)
 *   named ptr in do/while      7         13      18
 *
 * The inherited body is the middle cell of the middle row.  At 7 the
 * relocations MATCH.
 *
 * ===== AND THE ARM ORDER HAD TO BE RE-CROSSED, NOT INHERITED. =====
 *
 * At 7 the whole residue was a pure r6 <-> r7 rename (`file` and `dst`
 * exchanged) at indices 11, 16, 19, 23, 64 plus the call swap.  The inherited
 * header measured all eleven arm-order permutations AGAINST ITS OWN 14-DIFFERING
 * BODY and concluded `(bias,dst,pal)` and `(bias,pal,dst)` were both fourteen.
 * CROSSED AGAINST THE COUNT-FIRST BODY THEY ARE NOT EQUAL:
 *
 *   bias, dst, pal (both arms)   2   <- THIS BODY
 *   bias, pal, dst               7
 *   dst, bias, pal               9
 *   bias/pal/dst mismatched     74
 *   dst, pal, bias              74
 *   pal, dst, bias              76
 *   pal, bias, dst              77
 *
 * A LEVER MEASURED INERT AGAINST A BROKEN BODY IS NOT MEASURED.
 *
 * ===== WHAT THE REMAINING 2 ARE, AND THE SCHEDULER'S OWN DUMP SAYS WHY. =====
 *
 *   rom   add r3,#0xc4 / add r1,r7,#0 / add r0,r6,r2 / ldr r3,[r3] / mov r2,r8
 *   ours  add r3,#0xc4 / add r0,r6,r2 / add r1,r7,#0 / ldr r3,[r3] / mov r2,r8
 *
 * `-fsched-verbose=4`, block 5 (insn 102 = `r0=r6+r2`, insn 106 = `r1=r7`):
 *
 *   ;;	Ready list (t = 51):    97  106  102
 *   ;;		--> scheduling insn <<<102>>> on unit core
 *   ;;		dependences resolved: insn 108 into ready
 *   ;;	Ready list (t = 52):    108  97  106
 *   ;;		--> scheduling insn <<<106>>> on unit core
 *
 * The list is sorted worst-first and schedule_block takes the last element, so
 * 102 outranks 106.  Walking rank_for_schedule (haifa-sched.c:4040-4114):
 *   rung 1 PRIORITY  -- equal, and UNMOVABLE: both insns' only path out is the
 *     `bl`, and arm_adjust_cost (arm.c:2429-2432) HARD-CODES cost 1 for any
 *     true dependence into a CALL_INSN ("Call insns don't incur a stall").
 *   rung 2 register pressure -- gated on `! reload_completed`; never applies in
 *     sched2.
 *   rung 3 interblock -- same block.
 *   rung 4 class vs last_scheduled_insn (insn 94, `r3=r3+0xc4`) -- neither is in
 *     its INSN_DEPEND, so both are class 3.  Tie.
 *   rung 5 depend_count -- THE DECIDER.  102 has TWO dependents: the call, and
 *     an ANTI-DEPENDENCE to insn 108 (`r2=r8`, the third argument's load),
 *     because 102 reads r2 and 108 writes it.  The dump confirms it in terms --
 *     scheduling 102 is what "resolved insn 108 into ready".  106 has ONE.
 *     rank_for_schedule prefers more dependents.
 *   rung 6 LUID -- never reached, and would pick 102 anyway:
 *     load_register_parameters (calls.c:1684-1697) walks i = 0..num_actuals-1
 *     because LOAD_ARGS_REVERSED is undefined here, so ARG 0's LOAD ALWAYS HAS
 *     THE LOWER LUID.
 *
 * READINESS WAS CHECKED, per the batch-335 rule: 102 enters the ready list at
 * t=50 (`lsl r2,r2,#1` issues at t=49, cost 1) and the choice is at t=51, so it
 * is genuinely in the list.  This is not a readiness artefact.
 *
 * THE BOUND, WITH ITS EVIDENCE ATTACHED: the reference's own text forces the
 * counts that make gcc prefer our order.  `add r0,r6,r2` must read a register
 * for the 0x100 (Thumb's three-operand add takes imm3 only) and `mov r2,r8`
 * must follow it (arg 2 comes from r8 in the reference), so 102 carries the
 * anti-dependence in ANY body reproducing the reference's registers.  106 can
 * gain a second dependent only from a later WRITER of r7, and nothing in the
 * reference block writes r7.
 *
 * MEASURED EXACTLY INERT on this body (2 of 87, first index 43): a named
 * `s = file + (0x80<<1)` before the call; the same placed before the gPtrs
 * load; a named `d2 = dst`; dropping the `fn` variable for
 * `(*(Fn *)g)(...)`; `(u16 *)file + 0x80` as the offset.
 * MEASURED WORSE: `do { } while (0)` around the call -- 5, and it moves
 * `ldr r3,[r3]` AHEAD of the argument setup; the same around the `fn` load --
 * 5; `do { s = ...; } while (0)` -- 5; a bare `__asm__ volatile ("")` before
 * the call -- 5; `fn = *(Fn *)(gPtrs + 0xc4)` as one expression -- 41 and a
 * different `bl` offset; the gPtrs load hoisted above galloc_iwram -- 90;
 * `queue->count = count + 1` for `*(u16 *)queue = count + 1` -- 4;
 * `file = GetFile(id)` moved after the page if -- 81.
 * Also inert at the 7 level, all identical: five declaration orders of
 * file/dst/pal/bias.
 *
 * INSTRUMENT, LABELLED, AND IT REFUTES THE OBVIOUS FIX.
 * `__asm__ volatile ("" : "+r"(dst));` after the call does manufacture a later
 * writer of dst's register, but it reads 8 -- it re-flips the r6/r7 allocation
 * (first difference back at index 11) instead of flipping the schedule.  Two
 * such outputs read 52.  So the extra dependent is not usable even as a device.
 *
 * NEXT, for whoever takes this: the only unexplored direction is making arg 1's
 * hard-register load carry a second dependent WITHOUT disturbing the r6/r7
 * allocation.  Everything that touches the call's own statement perturbs the
 * allocation first.
 *
 * [SUPERSEDED] The batch-319 backfill measured fourteen of eighty-seven.
 *   *** RELOCATIONS DIFFER IN THEIR SYMBOLS, NOT ONLY THEIR OFFSETS --
 *   _SIZE_80f0024
 *   *** THAT ALARM IS WRONG AND BATCH 336 DISPROVED IT -- see the
 *   RELOCATION paragraph at the top of this file. ***
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f0000/LoadGS1CreditsBG.c \
 *     asm/rom_f0000/rom_f0254_a_a_c.s --func LoadGS1CreditsBG
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * parked at src/non_matching/rom_f0000/80f0254.c, so landing needs a two-way split).
 *
 * NOT MATCHING: 14 differing of 87 encodings, SIZE IDENTICAL -- and 13 of those are instructions,
 * one is a pool word. Candidate below.
 *
 * ================ BLOCKED ON A size.sym ENTRY: _SIZE_80f0024 = 0x230 ================
 *
 * The ROM has `ldr r5, =0x230`. Spelled as the literal, the function COLLAPSES to 76 lines
 * against 82, because gcc builds 0x230 with `mov/lsl` (it is shiftable -- 0x8c << 2, or
 * equivalently 0x23 << 4) and then folds the runtime `lsr r5, #2 / orr r2, r5` count-word build
 * into a single pooled `=0x8400008c`.
 *
 * THE ARITHMETIC IS VERIFIED, not inferred: Func_80f0024 is at 0x080f0024
 * (asm/rom_f0000/rom_f0008.s:13) and Func_80f0254 at 0x080f0254, so the gap is exactly 0x230.
 * That is precisely size.sym's class -- the size of a routine the game copies into RAM and runs
 * there -- and there are four existing _SIZE_ entries on the same footing.
 *
 * NOT ADDED, because the entry would buy a NON-MATCH: 13 encodings still differ with it. Same
 * reasoning that kept _MSG_b24 out in batch 272 and the label.sym entry out of
 * src/non_matching/rom_77000/8078144.c in this one. A build input is worth adding when it
 * COMPLETES a function.
 *
 * IT ALSO RETIRES AN OPEN QUESTION ELSEWHERE. src/non_matching/rom_9000/8012388.c closes with
 * "NEXT: a way to keep a local constant unfolded, which nothing in the notebook currently
 * offers". Its 0x27c is this same class -- a _SIZE_8009e7c, not a fold problem. That park should
 * be re-read with this in mind.
 *
 * ================ THE -fno-gcse MEASUREMENT, ANSWERED IN THE NEGATIVE ================
 *
 * This file's sibling src/rom_f0000/rom_f0254_a_b.c carries GCSE_CFLAGS, which is why the flag was
 * checked here. It is BYTE-FOR-BYTE INERT -- the full instruction streams were diffed rather than
 * the counts, and they are IDENTICAL, on three separate candidates (13, 17 and 14 differing).
 * NO Makefile row is warranted. Recorded because "the neighbour has the flag" is exactly the kind
 * of inference that would otherwise get acted on.
 *
 * THREE LEVERS DID LAND, 67 differing to 14 to 13:
 *   - the park's DMA3_SET / DmaQueue idiom, transferred.
 *   - THE QUEUE POINTER AS AN INLINE'S PARAMETER, so it is materialised before REG_IME. That fixed
 *     the whole tail, 67 to 14.
 *   - `bias, pal, dst` ORDER IN BOTH IF-ARMS. An arm-order MISMATCH makes gcc emit `pal`'s
 *     high-register copy in each arm instead of once at the join, where jump2's cross-jumping
 *     merges it. All eleven arm-order permutations were measured: (bias,dst,pal) and
 *     (bias,pal,dst) are 14; (dst,bias,pal) in both arms is 21; everything else 62-68.
 *   - a named `vu16 *ime` pointer instead of `SET_IO(REG_IME, ...)` takes 14 to 13.
 *
 * REMAINING 13 WITH THE SYMBOL: a `mov r1, r7` / `add r0, r6, r2` adjacent swap at the indirect
 * call (2), an r0/r1 assignment swap across the whole IME/queue block (7+2), and the
 * `add r2, #1` / `stmia` order (2). `count++` before the src store fixes the last pair but flips
 * `file`/`dst` between r6 and r7 (17); declaration order, inline-parameter order and a named `src`
 * local are all inert on it.
  */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

struct DmaTransfer { const void *src; void *dest; u32 control; };
struct DmaQueue { u16 count; struct DmaTransfer tasks[32]; };

extern struct DmaQueue gDMATaskCount;
extern unsigned char gPtrs[];
extern unsigned char _SIZE_80f0024[];
extern unsigned char *GetFile(int id);
extern unsigned char *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void Func_80f0024(void);
extern void Func_80f0254(int page);
typedef void (*Fn)(void *src, void *dst, u32 bias);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void PushDma(struct DmaQueue *queue, const void *src, void *dest)
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
        *task++ = (u32)dest;
        *task = 0x84000040;
    }
    SET_IO(REG_IME, savedIme);
}

void LoadGS1CreditsBG(int id, int page)
{
    unsigned char *file;
    void *dst;
    void *pal;
    u32 bias;
    unsigned char *g;
    unsigned char *scratch;
    Fn fn;
    int n;

    if (id == 0) {
        Func_80f0254(page);
        return;
    }
    file = GetFile(id);
    if (page == 0) {
        bias = 0;
        dst = (void *)(0xc0 << 19);
        pal = (void *)(0xa0 << 19);
    } else {
        bias = 0x80808080;
        dst = (void *)0x6008000;
        pal = (void *)0x5000100;
    }
    n = (int)&_SIZE_80f0024;
    scratch = galloc_iwram(0x31, n);
    DMA3_COPY(Func_80f0024, scratch, n);
    g = gPtrs;
    g += 0xc4;
    fn = *(Fn *)g;
    fn(file + (0x80 << 1), dst, bias);
    gfree(0x31);
    PushDma(&gDMATaskCount, file, pal);
}

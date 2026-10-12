/* AnimEnd -- asm/rom_c9000/rom_cd508_a_c_c.s
 *
 * STILL NON-MATCHING, **2 of 142 STREAM encodings** (was 14).  MEASURED
 * batch 340 brief E.  Instruction counts 113 / 113 -- IN PHASE -- size equal,
 * and **RELOCATIONS ARE NOW IDENTICAL** (the pool-word transposition the park
 * described is gone).  First differing index 90.  PINS: 0.
 *   objcmp verbatim:
 *     XX ENCODINGS differ in 2 place(s) (ref 142, ours 142)
 *        [STREAM length, pool+pad INCLUDED; instructions are 113/113]
 *        first at index 90: ref 18ff  ours 2500
 *   tools/tryc.py --align: 6 instructions in disagreeing regions, of 126, and
 *   four of those six are the L3/L4 LABEL RENUMBERING the anchor causes --
 *   labels are not encodings, which is why objcmp reads 2 and align reads 6.
 *
 * *** TWO CORRECTIONS TO THE PARK, BOTH LOAD-BEARING. ***
 *
 * 1. **ITS INSTRUCTION COUNT IS THE STREAM COUNT.**  The park says
 *    "INSTRUCTION COUNT 142 = 142 and SIZE equal, so the 14 IS a true
 *    distance".  142 is the STREAM count; the function is **113** 16-bit
 *    instructions plus 29 32-bit entries (pool words and `bl` calls).  The
 *    conclusion survives -- 113 = 113, so the figure was and is a true
 *    distance -- but the number quoted was the wrong one, and three parks in
 *    batch 337 and two more in this one are in the same state.
 *
 * 2. **ITS FLOOR IS WRONG AND ITS CAUSE-1 BOUND IS REFUTED.**  The park ends
 *    *"SO AnimEnd'S FLOOR IS 12, NOT 0, UNTIL THE POOL-LOAD RULE ABOVE IS
 *    BROKEN."*  The measured figure is now **2**, so the floor was never 12,
 *    and the rule was not broken -- IT WAS SIDESTEPPED.  See below; this is
 *    the batch's main finding and it generalises.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/AnimEnd.c \
 *     asm/rom_c9000/rom_cd508_a_c_c.s --func AnimEnd
 * tools/piece.py: the .s holds exactly ONE function, so this converts
 * WHOLE-FILE and `--whole` works too (`2 of 142 differ (ours 142)`).  NO SPLIT.
 *
 * ======================================================================
 * THE LEVER: ONE `do { } while (0)` ANCHOR *AFTER* THE POOL LOAD, INSIDE THE
 * static inline.  14 -> 2, AND IT CLEANS THE RELOCATIONS.
 * ======================================================================
 *
 *     queue = &gDMATaskCount;
 *     do { } while (0);          <- the whole lever
 *     savedIme = REG_IME;
 *
 * The park's cause 1 was: the `ldr rX,=gDMATaskCount` pool load is the FIRST
 * insn of its block in the reference and the FIFTH in ours, at BOTH
 * SetRegAnimDest sites, dragging the pool-word order and the two `ldr`
 * pc-offsets with it -- 12 of the 14.  Its bound was correct **as a statement
 * about sched2 priority** and I reproduced every link of it:
 *   - prio(pool load) = cost + priority of its consumer's chain attach point,
 *     and it cannot acquire a new forward dependent by any source edit (no
 *     register inputs at all, so no anti-dependence; `DIFFERENT_ALIAS_SETS_P`
 *     plus `RTX_UNCHANGING_P` refuse a memory anti-dependence twice).
 *   - a tie does not help, because LUID then puts the 0x20 store first.
 * *** BUT "IT CANNOT WIN THE PRIORITY CONTEST" IS NOT "IT CANNOT ISSUE
 * FIRST".  AN ANCHOR DOES NOT RAISE ITS PRIORITY -- IT EMPTIES THE READY
 * LIST.  The loop notes hang on the FOLLOWING insn (haifa-sched.c:3735-3736),
 * so an anchor written immediately after the pool load fences immediately
 * before `savedIme = REG_IME`, which puts the pool load ALONE at the end of
 * its region with nothing to lose a rung to. ***  This is the same correction
 * batch 338's autosweep note records for Func_80a7440 -- *"you do not win
 * that tie, you empty the ready list"* -- and it had not been applied here.
 *
 * WHY THE ANCHOR MUST BE INSIDE THE INLINE AND AFTER THE LOAD.  Measured, all
 * 31 top-level anchor placements plus 31 more crossed on the best of them:
 *     anchor16, immediately BEFORE `SetRegAnimDest(0x80<<19, 0x7341)`   10
 *       (it fences before the pool load, so the pool load leads its region
 *        but still trails the pre-fence `mov r3,#0x20 / strh r3,[r6,#6]`
 *        -- 2 positions late at BOTH sites, with the relocations STILL dirty)
 *     anchor inside the inline, immediately AFTER the pool load            2
 *     ... and BEFORE the pool load instead                               10
 *     ... on BOTH sides of the pool load                                  5
 *     ... after the pool load AND at the call site                         5
 *     ... after `savedIme = REG_IME` / before `count = queue->count`     10
 *   Every other top-level placement: 14 (inert) at 0,3-7,17,19,20,28-30;
 *   16 at 8-11,15,18,24,25; 17 at 1,27; 18 at 2,21,22,23; 19 at 26;
 *   20 at 12; 21 at 13,14.
 * TWO ANCHORS ARE WORSE THAN ONE HERE (5 against 2), which is the same shape
 * batch 336 found for the barrier in a loop body: a fence that emits nothing
 * still lengthens every live range crossing it.
 *
 * >>> THE REUSABLE RULE, and it supersedes the park's: <<<
 * > A PC-RELATIVE LITERAL-POOL LOAD CANNOT ACQUIRE A FORWARD DEPENDENT AND SO
 * > CANNOT WIN A sched2 PRIORITY CONTEST -- THE PARK'S RULE IS RIGHT.  BUT
 * > WHERE THE REFERENCE SCHEDULES ONE EARLIER THAN ITS CONSUMER CHAIN CAN
 * > JUSTIFY, THE ANSWER IS AN ANCHOR IMMEDIATELY AFTER IT, WHICH PUTS IT
 * > ALONE IN ITS REGION.  "NOT REACHABLE THROUGH THE SCHEDULER" WAS A
 * > STATEMENT ABOUT PRIORITY MISTAKEN FOR ONE ABOUT ORDER.
 * The park directs this rule at Anim_CriticalHit's runs 3 and 4 (two pool-load
 * adjacent swaps) and at "every 'pool load one slot early/late' park in the
 * bank".  **All of those should be re-measured with this anchor**, and
 * AnimStart / AnimStart2 in this same `.s` inline this very queue four times
 * over, so they inherit the fix the moment this body installs.
 *
 * ======================================================================
 * WHAT THE REMAINING 2 IS -- CAUSE 2, AND THE PARK'S BOUND ON IT SURVIVES
 * ======================================================================
 *   ref    mov r2,#0x15 | mov r6,#0 | add r7,r3 | mov r5,#0  | mov r8,r2
 *   ours   mov r2,#0x15 | mov r6,#0 | mov r5,#0 | add r7,r3  | mov r8,r2
 * `add r7,r3` is LICM's hoist of `b + (0xc9 << 3)`; `mov r5,#0` is `k = 0`.
 * Both are region leaves at priority 1, `mov r2,#0x15` is the one prio-2 insn
 * (it feeds `mov r8,r2` in region), and within the prio-1 class INSN_LUID
 * decides, lowest first.  `move_movables` emits every hoist with
 * `emit_insn_before (..., loop_start)`, i.e. at the preheader's END, so
 * LUID(hoist) > LUID(k = 0) **by construction** and the reference's order is
 * not expressible while the quantity is a hoist.
 * The park's escape -- give the hoisted quantity its own source statement --
 * is still blocked by its own precondition, and I re-measured the block on
 * THIS body (the park measured it on the 14-body):
 *     `u16 *bp` named between the two zero inits   62 of 142, dsize -8, RELOCDIFF
 *     ... named before both inits                  62, -8, RELOCDIFF
 *     ... named after both inits                   62, -8, RELOCDIFF
 *     ... with the PRE-LOOP use also through bp    68, -4, RELOCDIFF
 * `b + (0xc9 << 3)` is used before the loop by `_Func_80c0774` too, so gcse
 * folds the two computations into one and the reference's second, independent
 * computation is lost.  **THE p2 TRICK FROM THIS BATCH DOES NOT TRANSFER**:
 * three distinct locals defeat gcse for a repeated CONSTANT (Func_80a33d4),
 * because each is an independent `(set (reg) (const_int))`; they do not defeat
 * it for a repeated EXPRESSION over a live base, where the value really is
 * available.  Worth stating, because the two cases look identical in source.
 * MEASURED INERT AT EXACTLY 2 (count 113/113, relocations clean): an anchor as
 * the loop body's first statement; `bq = b` named ahead of the inits (the
 * park's candidate prerequisite -- still exactly inert, now at 2).
 * MEASURED WORSE: an anchor between the two inits, after `k = 0`, or before
 * `i = 0` -- 6 each; `lim = 0x15` named ahead of the inits -- 6.
 * ALSO REFUTED, with figures: passing the queue pointer INTO the inline so the
 * pool load becomes an ordinary AnimEnd statement -- two distinct locals 23,
 * one shared local 23, two locals plus the call-site anchor 19, an anchor
 * before the first local 21.  The pool load belongs inside the inline.
 *
 * ======================================================================
 * AUTOSWEEP, AND A TOOL BUG WORTH FIXING
 * ======================================================================
 * `python3 tools/autosweep.py src/non_matching/rom_c9000/AnimEnd.c` prints
 * `function  SetRegAnimDest` / `BASE DOES NOT MEASURE` and sweeps NOTHING.
 * It takes the FIRST function definition in the park file, which here is the
 * `static inline` helper, not the reference symbol -- so a park with a static
 * inline above its subject reads as "too far gone to sweep" when the axis is
 * simply UNTESTED.  Same false-clean-negative class as the three autosweep
 * bugs batch 338's briefs found.  Both axes were run by hand instead:
 *   FLAG AXIS (OBJCMP_EXTRA, parked body): -fno-gcse 14, -fno-strength-reduce
 *     14 (inert); -fno-strict-aliasing 20, -fno-schedule-insns2 36 (worse);
 *     -O1 109 insns, -ffixed-r7 117, -fno-rerun-cse-after-loop 115,
 *     -fno-rerun-loop-opt 107 (all COUNT rows, they bound nothing).
 *   ANCHOR AXIS: 31 boundaries, table above.
 *
 * PROTOTYPE AND RETURN TYPE ARE UNCHANGED: `void AnimEnd(void)`.  The brief
 * flagged this because a return-type change rewrites `pop {rN}` at every
 * caller (arm.c:8302, :8382) and AnimEnd has many in the battle-animation
 * bank.  No caller is affected by this body.
 *
 * DEVICES: the `do { } while (0);` anchor is device-FREE by the batch-337
 * ruling -- it emits no instruction, tools/shimcount.py does not count it
 * (reports no rows for this body), and it carries no pass-3 depin debt.  It is
 * not a `__asm__` barrier, not an `.equ`, not a fictitious symbol.  It does
 * add one assembler LABEL (126 lines against 127) which costs no bytes.
 * NO fakematch row needed.
 *
 * ======================================================================
 * CARRIED FORWARD, STILL TRUE
 * ======================================================================
 * - KEEP THE ASYMMETRY: a SYMBOL address goes through `force_const_mem` and
 *   becomes a real pool MEM carrying memory dependences; a large INTEGER
 *   constant stays a bare `(set (reg) (const_int 0x4000208))` with no memory
 *   operand and no memory dependences.  That is why the gDMATaskCount load
 *   lost to the REG_IME address in the same block.
 * - THE LICM LEVER IS LOAD-BEARING AND MUST NOT BE TIDIED: leaving the full
 *   expression INSIDE the loop body makes LICM hoist it as a SECOND,
 *   independent computation, which is what the reference has.  Hoisting it by
 *   hand makes gcc fold both into one.
 * - MEASURED FLAT AT 14 on the parked body (14 variants, byte-identical):
 *   `queue` initialised at its declaration; `count` read from
 *   `gDMATaskCount.count` directly; `queue` as a `u32` with casts; `queue`
 *   declared last; a `+ 0` offset on the address; `for (i=0,k=0; ...)`;
 *   `k = i = 0`; `&b[0xc9<<3]` for `b + (0xc9<<3)`.
 * - MEASURED WORSE on the parked body: `k = 0; i = 0;` 18; `i = k = 0;` 18;
 *   the queue assignment moved after the IME dance 18; `count` read before the
 *   IME save 23; `if (queue->count < 32)` 16; a named `vu16 *ime` 68.
 * - REFUTED EARLIER AND STILL REFUTED: "4 lines at two sites: an `add` and a
 *   `strh` swapped" -- there is ONE such swap, worth 2, and it is all that is
 *   left.  "a redundant label" -- not reproduced.
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
extern unsigned char iwram_3001eec[];
extern unsigned short iwram_3001ad0[];
extern int gPhysVec[];

extern void _PlaySound(int id);
extern void Func_80008d4(void *dst, s32 len);
extern void StopTask(void *task);
extern void Func_80cd4b4(void);
extern void WaitFrames(unsigned int nframes);
extern int _Func_80c0774(int a, unsigned short b, int c);
extern void _Func_80c0700(unsigned short a, int b);

static inline void SetRegAnimDest(u32 dest, u32 src)
{
    struct DmaQueue *queue;
    u32 savedIme;
    s32 count;
    u32 *task;

    queue = &gDMATaskCount;
    do { } while (0);
    savedIme = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
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

void AnimEnd(void)
{
    unsigned char *a;
    unsigned char *b;
    void (*fp)(void *, s32);
    unsigned char *d;
    int i;
    int k;
    u32 off;

    a = *(unsigned char **)iwram_3001eec;
    b = *(unsigned char **)(iwram_3001eec - 0x78);
    _PlaySound(0x121);
    off = 0x77a0;
    d = a + off;
    iwram_3001ad0[2] = *(int *)d;
    off = 0x77a4;
    a += off;
    iwram_3001ad0[3] = *(int *)a;
    gPhysVec[3] = 0x78;
    gPhysVec[4] = 0x78;
    REG_BG2CNT = 0x787;
    fp = Func_80008d4;
    fp((void *)0x6004000, 0x80 << 7);
    StopTask(Func_80cd4b4);
    iwram_3001ad0[3] = 0x20;
    SetRegAnimDest(0x80 << 19, 0x7341);
    REG_BLDCNT = 0;
    WaitFrames(1);
    _Func_80c0774(2, *(unsigned short *)(b + (0xc9 << 3)), 7);
    WaitFrames(1);
    i = 0;
    k = 0;
    do {
        _Func_80c0700(*(unsigned short *)(b + (0xc9 << 3)), 0x15 - k);
        i += 1;
        WaitFrames(1);
        k += 3;
    } while (i != 8);
    SetRegAnimDest(0x80 << 19, 0x7541);
    WaitFrames(1);
}

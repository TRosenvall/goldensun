/* ScreenTransitionIn  --  asm/rom_8a000/rom_8d9a4_c_c_a_c_c_a.s  (0x0808fefc)
 * NON-MATCHING, 60 encodings of 309.  SIZE AND COUNT NOW EXACT -- 708 bytes and 309 entries both sides, with the pool dumped twice
 * in the ROM's two places, so the 28-byte gap the earlier park called an artefact is CLOSED.
 * DOWN FROM 147.  `--align` 35 of 302, from 60.
 *
 * CARRIES ONE VERIFICATION SHIM, legitimate in a park and NOT to be landed:
 * `__asm__(".equ _CONST_50, 0x50")`, absolute in-TU.  The agent's own recommendation, which I
 * accept, is NOT to add a const.sym row on this evidence -- the symbol may be standing in for a
 * third source reference nobody has found, since a HImode carrier does reproduce the pool and
 * only loses the register to local-alloc.c:886's REG_N_REFS == 2 deletion rule.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808fefc.c \
 *     asm/rom_8a000/rom_8d9a4_c_c_a_c_c_a.s --func ScreenTransitionIn
 *
 * NON-MATCHING: 60 encodings of 309 differ (objcmp --whole).  Was 147.
 * Working distance: 35 instructions in disagreeing regions of 302 (tryc --align).  Was 60.
 *
 * *** THE SIZE AND THE ENCODING COUNT ARE NOW BOTH EXACT: 708 bytes and 309 entries on
 * both sides, with the literal pool DUMPED TWICE in the same two places as the ROM. ***
 * The previous revision read that 28-byte gap as an artefact to be ignored until the
 * instruction stream matched.  It was not an artefact -- it was the measurement that
 * mattered, and closing it is what moved the whole function.  See B below.
 *
 * objcmp --whole, verbatim:
 *   XX ScreenTransitionIn           60 of 309 differ (ours 309), first at index 57
 *   XX RELOCATIONS differ
 * The reloc difference is ONE extra R_ARM_THM_CALL StartTask (we emit five `bl StartTask`,
 * the ROM four -- cluster 4 below) plus the 2-byte offset drift that extra call causes in
 * four later relocations.  Nothing else in the list differs in type or symbol, and
 * _CONST_50 carries NO relocation (see SHIMS).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808fefc.c asm/rom_8a000/rom_8d9a4_c_c_a_c_c_a.s --whole
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_8a000/808fefc.c --ref asm/rom_8a000/rom_8d9a4_c_c_a_c_c_a.s --align
 *
 * Whole-file conversion: one function, no data section.
 *
 * SHIMS -- code only, leading comment stripped, in the two classes:
 *   `register ... __asm__` declarations               0
 *   `__asm__(".equ ...")` lines                       1   -- `.equ _CONST_50, 0x50`
 * The `.equ` is a VERIFICATION SHIM and must not land: if _CONST_50 is admitted it belongs
 * in const.sym, and the shim would duplicate the definition (batch 293's lesson).  With the
 * shim in this translation unit the symbol is absolute and emits NO relocation, which is why
 * the pool word compares clean against the reference's plain `.word 0x50`.  WHETHER IT
 * SHOULD BE ADMITTED IS AN OWNER DECISION -- see the note under C.
 *
 * STRUCTURE: unchanged from the previous revision and still believed correct -- the packed r0,
 * the five-entry jump table, the four copies of the gDMATaskCount push as one static inline,
 * and r7 reloaded at the top of case 4 off the still-live pool register.  Not restated.
 *
 * ============================================================================
 * BATCH 294: THE 28-INSTRUCTION CASE-4 CLUSTER HAS A SOURCE ROUTE.  60 -> 35.
 * ============================================================================
 * The previous revision's cluster 4 -- "the one cluster I could not find any source route
 * to" -- is solved, and so is cluster 3.  Three mechanisms were needed.
 *
 * A. *** A ONE-MEMBER u16 STRUCT LOCAL IS A HImode PSEUDO, AND ASSIGNING A LITERAL TO IT
 *    GOES THROUGH THE POOL. ***  `promote_mode` promotes only INTEGER_TYPE, ENUMERAL_TYPE,
 *    BOOLEAN_TYPE, CHAR_TYPE, REAL_TYPE and OFFSET_TYPE; a RECORD_TYPE falls to
 *    `default: break` and keeps its own mode.  So `struct HWord { unsigned short v; }` gets a
 *    HImode pseudo where a bare `unsigned short` local is promoted to SImode (arm.h:597,
 *    batch 293's finding 5).  A HImode pseudo whose destination is a HIGH register has no
 *    matching alternative in `*thumb_movhi_insn` -- alternative 5 is `l <- I`, LOW register
 *    only -- so the literal is forced to memory and reaches the high register as
 *    `ldr rL, .Lpool / mov rH, rL`, which is exactly what the ROM writes for case 4's 0.
 *      case 4's z as `int`                    60 align / 147 objcmp / 680 bytes
 *      case 4's z as `struct HWord`           71 align / 159 objcmp / 704 bytes
 *    The two numbers disagree in SIGN.  The struct is right and align says it is wrong.
 *
 * B. *** WHY THAT CLOSES THE POOL GAP: A HImode POOL LOAD HAS pool_range 64, NOT 1020. ***
 *    `*thumb_movhi_insn`'s pool_range attribute is `*,64,*,*,*,*`; `*thumb_movsi_insn`'s is
 *    1020.  One HImode pool reference forces `arm_reorg` to dump the minipool at the next
 *    barrier -- here the barrier after case 4's `b` -- and everything still needed after that
 *    point is emitted AGAIN in the pool at the end of the section.  That is the ROM's
 *    two-pool layout and its duplicated words, and it is the whole 28-byte size gap.
 *    It also settles what the ROM's `ldr r1,.L900f4 @ 0` is: the ROM's pool word sits about
 *    0x28 bytes from its load -- inside the 64-byte HImode window, nowhere near the 1020-byte
 *    SImode one -- so the ROM's 0 IS a HImode pool load.  (The `ldr`/`ldrh` mnemonic is not
 *    evidence either way: gas assembles both identically over a pool word.)
 *
 * C. *** local-alloc.c:886 DELETES A POOLED CONSTANT THAT HAS EXACTLY TWO REFERENCES. ***
 *    This is why case 4's 0x50 would not stay in r9 through eleven earlier spellings.
 *      if (REG_N_REFS (regno) == 2 && REG_BASIC_BLOCK (regno) < 0
 *          && rtx_equal_p (XEXP (note, 0), SET_SRC (set)))
 *        reg_equiv_replace[regno] = 1;
 *    `update_equiv_regs` then either replaces the single use with the equivalent outright or,
 *    failing that, MOVES the initialising insn to sit just before the use (local-alloc.c:962).
 *    Case 4's 0 has three references (its set, `t[0x53b]`, `t[0x53d]`) and survives to win r8.
 *    Case 4's 0x50 has TWO (its set and `t[0x53a]`), so its load is moved past the calls,
 *    lands in the SECOND pool, and the allocator never sees an allocno to give r9 to.  The
 *    threshold is a raw count here because there are no loops; inside one it would be
 *    weighted by loop_depth + 1 (flow.c:4948).
 *    A pooled SYMBOL escapes the rule: `reg_equiv_replacement` is then a SYMBOL_REF,
 *    `validate_replace_rtx` cannot put one into a `strb`, and the move-before-use branch does
 *    not fire either, so the pseudo survives to allocation and takes r9.
 *      case 4's 0x50 as `int`                 r8/r9 hold the two constants the wrong way up
 *      case 4's 0x50 as `struct HWord`        its load is moved after the calls
 *      case 4's 0x50 as `(int)&_CONST_50`     the ROM's r9, and rom[250:266] -- the
 *                                             16-instruction strb block -- goes to ZERO
 *    OWNER DECISION.  const.sym's criterion 1 is met (the ROM pools a value one `mov` could
 *    build) and criterion 2 is now MEASURED rather than asserted: fourteen spellings of case
 *    4's two carriers are recorded here and in the inert list, and none reproduces both the
 *    pool and the allocation.  Against admitting it: mechanism B shows the ROM's word is a
 *    HImode pool entry, and a HImode carrier is a literal spelling that DOES reproduce the
 *    pool -- it just loses the register to the REG_N_REFS==2 rule.  So the symbol may be
 *    standing in for a third source reference I have not found.  I would not add the entry
 *    on this evidence alone.
 *
 * D. *** CASE 3's 0x52a GOES THROUGH ITS OWN u16 * LOCAL. ***  41 -> 35 align, 128 -> 60
 *    objcmp, and it is what made the SIZE and the COUNT exact.  Written
 *    `*(u16 *)(t + 0x52a) = hi;` the offset is derived from 0x528 by reload_cse_move2add in
 *    the same hard register; with the address in its own local it is materialised from the
 *    pool into a different register, which is what the ROM has and what move2add cannot chain
 *    across.  The same spelling at case 2's 0x534, case 2's 0x52a, case 2's 0x536, case 3's
 *    0x528 and case 4's two stores is inert or worse.  It is load-bearing at exactly one site.
 *
 * E. THE INLINE'S TWO LOADS: READ DISPCNT INTO A u32 TEMP FIRST, THEN OR THE FIELD INTO IT.
 *      *task++ = *(u16 *)(s + 0x14) | *(vu16 *)(0x80 << 19);        45 align
 *      u32 d = *(vu16 *)(0x80 << 19); *task++ = *(u16*)(s+0x14)|d;  41 align
 *    This is NOT the operand-order lever the previous revision measured (`dispcnt | field`
 *    was 111).  The OR's operand order stays field-first; what changes is the order the two
 *    MEMs are READ in, and that fixes the registers at all four sites -- the ROM's
 *    `ldrh r3,[r7,#0x14]` / `ldrh r1,[r0]` with the field tied to the OR's destination.
 *    Only a one-instruction sched2 interleave is left at each site.
 *
 * ADDED TO THE INERT LIST (each a single drop from the 35 / 60 file unless noted; the
 * previous revision's list still stands and is not restated)
 *   case 4 decl order `int z; int n;`                                   inert
 *   case 4 assign order z before n                                     128 align (much worse)
 *   case 4's two strh from the literal 0x50 instead of from k            inert
 *   case 4's halfword stores via a u16 index / plain 0x100 and 0x102 /
 *     w[0] and w[1] off one base / two separate u16 * locals
 *                                              inert / inert / 134 objcmp / 166 objcmp
 *   case 2's 0x534, 0x52a or 0x536 through its own u16 * local
 *                                              inert / 127 objcmp / 119 objcmp
 *   case 3's 0x528 through its own u16 * local                          inert
 *   case 2's 0x528/0x52a/0x536 off one u16 * base, 0x534 apart          241 objcmp (worse)
 *   the task cursor as (u32)queue + count*12 + 4                        inert
 *   the task cursor as count*12 + 4 + (u32)queue                        inert
 *   the task cursor as &queue->tasks[count]                             inert
 *   the task cursor built in two statements                              53 align (worse)
 *   the inline's field read into a u32 temp first                        97 align (worse)
 *   the inline's OR by compound assignment into a u32                    97 align (worse)
 *   the inline's DISPCNT read non-volatile                               inert
 *   the inline's field read volatile as well                             inert
 *   the inline's store split from the increment                          inert
 *   BOTH case-4 constants as pooled symbols                              45 align, and the
 *     pool does NOT split: an SImode symbol load has pool_range 1020, so it cannot force the
 *     early dump.  The HImode carrier for the 0 is what splits the pool; the symbol for the
 *     0x50 is what wins r9.  Neither alone reaches 35.  A COUPLED PAIR.
 *   both case-4 constants as HImode structs with 0x50 given a third
 *     reference (k = h50.v)                                             67 align
 *   the two strh reading h50.v directly (three refs, no k)              117 align, 724 bytes
 *   a two-member u16 struct carrying both case-4 constants              138 align, 672 bytes
 *   carriers assigned after WaitFrames, or after SetIntrHandler
 *                                                         71 / 73 align, 121 align (worse)
 *
 * THE REMAINING 35, IN FOUR CLUSTERS
 *  1. `add r2, r1` one position earlier than the ROM's, at all four inline sites (8 of the
 *     35).  Pure sched2 ordering: the ROM interleaves the task-cursor add between the two
 *     halfword loads, we put it before both.  Four spellings of the cursor do not move it.
 *  2. case 2, rom[88:98] (about 11).  The ROM pool-loads 0x534 into r3 and reuses r3 for the
 *     value 0x3f, chaining 0x528 -> +2 -> +0xc for the other three offsets; we chain all four
 *     in r1 and hold the value in r2.  Six spellings tried.  The previous revision's reading
 *     -- move2add chains only within ONE hard register, so this is local-alloc -- is
 *     confirmed; and since case 3 (D) DOES have a source route, the class is not closed,
 *     only this instance of it.
 *  3. case 4, rom[218:232] (about 11).  Both pooled loads land in r8 and r9 correctly now and
 *     the strb block is exact.  What is left: the ROM builds 0x102 independently
 *     (`mov r1,#0x81 / lsl r1,#1`) where move2add gives us `add r1,#2`, so we are ONE
 *     instruction short here, and the strh's value and address registers are swapped.
 *  4. the StartTask arms, rom[237:246] (about 5) plus the one extra relocation.  jump2's
 *     find_cross_jump merges the ROM's two `bl StartTask` into one and not ours.  The
 *     function-pointer spelling that does merge loses six instructions and is already on the
 *     previous revision's worse list.  The only cluster with no identified mechanism.
 */
#include "gba/types.h"
#include "gba/io.h"

/* VERIFICATION SHIMS, scratch only -- const.sym has no entry for either yet. */
__asm__(".equ _CONST_50, 0x50");
extern int _CONST_50;

struct HWord {
    unsigned short v;
};

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
extern unsigned char *iwram_3001e70;

extern void Func_8003b70(int a);
extern void Func_8003bb4(int a);
extern void WaitFrames(int n);
extern void Func_8091220(int a, int b);
extern void Func_8091254(int a);
extern void Func_8091240(int a);
extern signed char *AllocGlobal1F(void);
extern int StartTask(void *task, int priority);
extern void SetIntrHandler(int a, int b, void *f);
extern void Func_80907b0(int a);
extern void Task_ScreenWindowTransition(void);
extern void Func_808f498(void);
extern void Task_Transition300(void);
extern void Func_80903bc(void);
extern void Func_8090488(void);
extern void Func_8090584(void);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void QueueDispcntDma(struct DmaQueue *queue, unsigned char *s)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        {
        u32 d = *(vu16 *)(0x80 << 19);
        *task++ = *(u16 *)(s + 0x14) | d;
    }
        *task++ = 0x80 << 19;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

void ScreenTransitionIn(int a, int b);

void ScreenTransitionIn(int a, int b)
{
    unsigned char *s;
    signed char *t;
    int mode;
    int lo;
    int mask;

    mask = 0xff;
    mode = (a >> 8) & mask;
    s = *(unsigned char **)&iwram_3001e70;
    lo = a & mask;
    switch (mode) {
    case 0:
        Func_8003b70(0);
        Func_8003bb4(b);
        WaitFrames(1);
        break;
    case 1:
        Func_8091220(0x80 << 8, *(vu16 *)(0xa0 << 19));
        Func_8091254(b);
        WaitFrames(1);
        QueueDispcntDma(&gDMATaskCount, s);
        Func_8091240(0);
        return;
    case 2:
    {
        int z;
        int k;

        t = AllocGlobal1F();
        *(u16 *)(t + 0x528) = lo;
        z = 0;
        *(u16 *)(t + 0x52a) = z;
        k = 0x3f;
        *(u16 *)(t + 0x534) = k;
        k = 1;
        *(u16 *)(t + 0x536) = k;
        StartTask(Task_ScreenWindowTransition, 0xc8 << 4);
        StartTask(Func_808f498, 0x90 << 3);
        WaitFrames(1);
        QueueDispcntDma(&gDMATaskCount, s);
        t[0x53a] = z;
        t[0x53b] = 0x20;
        t[0x53c] = b;
        t[0x53d] = z;
        return;
    }
    case 3:
    {
        int hi;

        t = AllocGlobal1F();
        *(u16 *)(t + 0x528) = lo;
        {
            u16 *w = (u16 *)(t + 0x52a);

            hi = 0x20;
            *w = hi;
        }
        Func_80907b0(0xf);
        WaitFrames(1);
        StartTask(Task_Transition300, 0xc8 << 4);
        QueueDispcntDma(&gDMATaskCount, s);
        t[0x53a] = 0;
        t[0x53b] = hi;
        t[0x53c] = b;
        t[0x53d] = 0;
        return;
    }
    case 4:
    {
        int k;
        int n;
        struct HWord h0;

        s = *(unsigned char **)&iwram_3001e70;
        t = AllocGlobal1F();
        h0.v = 0;
        n = (int)&_CONST_50;
        k = 0x50;
        *(u16 *)(s + (0x80 << 1)) = k;
        *(u16 *)(s + (0x81 << 1)) = k;
        WaitFrames(1);
        if (lo == 0)
            StartTask(Func_80903bc, 0xc8 << 4);
        else
            StartTask(Func_8090488, 0xc8 << 4);
        SetIntrHandler(1, 0, Func_8090584);
        t[0x53a] = n;
        t[0x53b] = h0.v;
        t[0x53c] = b;
        t[0x53d] = h0.v;
        break;
    }
    }
    QueueDispcntDma(&gDMATaskCount, s);
}

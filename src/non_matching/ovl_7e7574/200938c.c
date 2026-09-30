/* OvlFunc_959_200938c  --  0x0200938c -- NON-MATCHING, 137 of 135 encodings differ.
 *
 * THE CLAIM LINE ABOVE IS THE DEFAULT-FLAGS FIGURE, BY DEFINITION.  This park's best
 * candidate is 49 of 135 under -fno-rerun-cse-after-loop, and the header below explains
 * why it needs that flag -- but parkcheck re-measures the claim line with objcmp at
 * PRODUCTION flags, so a claim of 49 reads as a park lying about its own body.  Quote
 * the flag figure everywhere else; never in the claim line.  (Fourth occurrence of this
 * mistake: three batch-301 drafts put their aligncmp figure here for the same reason.)
 *
 * SAY IT LOUDLY: THE 49-OF-135 RESULT IS UNDER -fno-rerun-cse-after-loop
 * (CSE_CFLAGS).  At the tree default it is 137 of 135 and SATURATED.  Both
 * figures are objcmp's, taken with --func:
 *
 *   default -O2          XX SIZE  ref 304 bytes, ours 320
 *                        XX ENCODINGS differ in 137 place(s) (ref 135, ours 141)
 *                           first at index 1: ref 4647  ours 4657
 *                        XX RELOCATIONS differ
 *   -fno-rerun-cse-      XX SIZE  ref 304 bytes, ours 312
 *   after-loop           XX ENCODINGS differ in 49 place(s) (ref 135, ours 137)
 *                           first at index 5: ref 4a43  ours 4a32
 *                        XX RELOCATIONS differ
 *
 * PARKCHECK WILL RE-MEASURE 137 of 135, not 49 of 135: this park's installed
 * path carries no Makefile flag row, so objcmp's cflags_for() finds nothing and
 * builds it at plain -O2.  The 49 figure requires the forced-flag wrapper (or a
 * CSE_CFLAGS row, if this is ever landed).  If it is landed it needs the object
 * added to CSE_CFLAGS in the Makefile and that must be stated in the commit --
 * it is NOT a default-flags match.
 *
 * SIZE IS NOT EXACT AND COUNT IS NOT EXACT (304/135 against 312/137), so the
 * objcmp count is SATURATED and does not rank.  aligncmp is the ranking view:
 *
 *   ref 135 encodings, ours 137
 *   aligned-equal 118  (87.4% of ref)   differing/ins/del 23 in 13 hunks
 *
 * (at plain -O2 aligncmp reads: aligned-equal 97 (71.9%), 48 in 19 hunks.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7e7574/200938c.c \
 *       asm/overlays/rom_7e7574/ovl_9dc_c_a_a_a_c.s --func OvlFunc_959_200938c
 *   (that is the 137-of-135 default-flags figure; for 49 of 135 add
 *    -fno-rerun-cse-after-loop to the compile, which the Makefile does not yet do)
 *
 * LANDING SHAPE: NO SPLIT.  asm/overlays/rom_7e7574/ovl_9dc_c_a_a_a_c.s is the
 * WHOLE file -- ONE function (`grep -c func_start` = 1), no data, so
 * datacheck.py reports nothing and split_s.py --dry-run is not applicable.
 * overlays/rom_7e7574/overlay.ld:57 names the object once,
 * `asm/overlays/rom_7e7574/ovl_9dc_c_a_a_a_c.o(.text)`, and no .ld .data/.bss
 * list names it.  The landing would be one line, asm/ -> src/.  NO LABEL NEEDS
 * `.global`.
 *
 * PIN-FREE: shimcount.py reports no register pins, no .equ shims, no "+r"
 * barriers, no empty asm.  No fakematch.txt row would be needed.
 *
 * ================ THE LEVERS THAT PAID, IN THE ORDER THEY PAID ================
 *
 * 1. THE gState OFFSET AS A LOCAL int -- the lever that landed the sibling
 *    OvlFunc_969_200871c byte-exact.  `*(short *)(gState + 0x24c)` folds
 *    base+offset into one pool word; the ROM pools the bare symbol and
 *    materialises 0x24c in a register (`mov r2, #0x93 / lsl r2, #2 / add r3, r2`).
 *    `k = 0x24c` as an int local reproduces that.
 *
 *    BUT THE LIVE RANGE IS THE WHOLE LEVER HERE.  A single `k` assigned once at
 *    the top is live across four calls, takes a callee-saved HIGH register and
 *    costs ONE EXTRA ALLOCNO: the ROM saves r8 alone (`mov r7, r8 / push {r7}`)
 *    and ours saved r8 AND r10 (`mov r7, sl / mov r6, r8 / push {r6, r7}`),
 *    which moves the prologue and saturates at 141 of 140 lines.  Assigning
 *    `k = 0x24c` INSIDE EACH ARM fixes it.  141 differ -> 39 (tryc lines).
 *
 * 2. -fno-rerun-cse-after-loop (CSE_CFLAGS).  The flag id 0x214 (`0x85 << 2`)
 *    is used at FOUR sites -- GetFlag x3 and SetFlag x1 -- and the ROM
 *    rematerialises `mov r0, #0x85 / lsl r0, #2` at every one.  At -O2 the
 *    rerun of cse after loop commons the constant into a callee-saved register
 *    (`mov r2, #133 / lsl r2, #2 / mov r8, r2`, then `mov r0, r8` at each site)
 *    and adds a push the ROM does not have.  This is the documented
 *    "GetFlag(id) guarding a block that ends SetFlag(id)" shape.  Measured
 *    141 differ -> 48 on the same source (c959_b).
 *
 *    NO OTHER FLAG REACHES IT.  Screened on the same candidate:
 *      -fno-cse-follow-jumps          151 differ  (inert)
 *      -fno-cse-skip-blocks           151 differ  (inert)
 *      -fno-gcse                      142 differ
 *      -fno-gcse -fno-cse-follow-jumps 142 differ
 *      -fno-expensive-optimizations   154 differ  (worse)
 *      -fno-rerun-cse-after-loop       39 differ
 *    -fno-gcse not helping is positive evidence FOR CSE_CFLAGS, as the document
 *    says; it is not evidence against.
 *
 * 3. THE TWO gState READS MUST BE TWO SOURCE READS IN IF/ELSE ARMS, WITH ONE
 *    COPY OF THE TAIL.  The ROM reads gState+0x24c TWICE with OPPOSITE branch
 *    senses -- arm A `beq .L1470`, arm !A `bne .L1488` -- and has ONE copy of
 *    the OvlFunc_959_20098e4 block that both arms reach.  Three source shapes
 *    were measured:
 *      sequential re-read (read, fall through, read again)   48 differ
 *      if/else with the tail DUPLICATED in both arms         48 differ
 *        (gcc did NOT merge the duplicate -- both copies survive, 9 extra insns)
 *      if/else + `goto` past the tail, ONE copy              39 differ  <-- this file
 *    The goto form is the only one that produces the ROM's CFG.
 *
 * 4. INERT, MEASURED, NOT ASSUMED (an inert spelling is untested, not
 *    disproved -- these were tested):
 *      an `int v` carrier for the two halfword store values      144 differ
 *      a separate `unsigned int off` + `unsigned char *d` pair   149 differ
 *        (the sibling OvlFunc_881_200b678's offset-reuse idiom -- it is the
 *         RIGHT idiom for that function and COSTS TWO ALLOCNOS in this one)
 *      reusing `k`/`q` as the offset-and-value carrier       41 / 48 / 117 differ
 *
 * ================ A REAL gcc-2.96 ICE, AND ITS TRIGGER ================
 *
 * THE OBVIOUS SPELLING OF LEVER 3 CRASHES THE COMPILER:
 *
 *   Internal compiler error in decode_rtx_const, at varasm.c:3421
 *
 * It fires whenever ONE `k` holding the gState offset is live into BOTH arms of
 * the if/else that reads gState -- AND ONLY under -fno-rerun-cse-after-loop.
 * At plain -O2 the same source compiles.  Six separate candidates hit it
 * (goto form, else-if form, duplicated-tail form, two-locals form, and with the
 * offset written as a bare literal in one arm).  The workaround is lever 1's
 * per-arm assignment, which is needed for the allocno count anyway: assigning
 * `k = 0x24c` separately in each arm compiles clean.  So the ICE and the extra
 * allocno have the same remedy, which is why this file has two assignments to
 * `k` that look redundant.  DO NOT "TIDY" THEM INTO ONE.
 *
 * ================ THE BLOCKER, BY PASS ================
 *
 * TWO RESIDUES REMAIN, AND THE RELOCATION LINE IS ONE OF THEM -- IT IS NOT A
 * FORM DIFFERENCE.  Read the SYMBOL SEQUENCE: ours has 16 relocations to the
 * ROM's 15, and `gState` appears TWICE in ours and ONCE in the ROM.
 *
 *   ref  [... iwram_3001e70, iwram_3001e40, gState]                  (15)
 *   ours [... iwram_3001e70, iwram_3001e40, gState, ..., gState]     (16)
 *
 * (A) CONSTANT-POOL PLACEMENT (varasm/final, the mid-function pool dump).
 *     gcc dumps a literal pool in the MIDDLE of this function and that pool
 *     carries its own copy of `.word gState`; the ROM has a single pool with
 *     one gState word.  That is the whole 8-byte size gap (312 vs 304).  It is
 *     a CONSEQUENCE, not a cause: the body is two instructions longer than the
 *     ROM's, which pushes a PC-relative load out of range and forces the early
 *     dump.  Close (B) and the pool question disappears with it.
 *
 * (B) jump.c CROSS-JUMPING MERGED ONE INSTRUCTION TOO MANY -- this is the
 *     actual blocker and the whole aligned residue is this one insert/delete
 *     pair.  The ROM's two arms each compute `q + off` themselves and share
 *     ONLY the `strh`:
 *       arm1  sub r2, #0xce / ldr r3, =0x2092 / add r2, r8 / b .L8
 *       arm2  mov r2, #0xc1 / lsl r2, #1 / add r2, r8 / mov r3, #0x5c
 *       .L8   strh r3, [r2]
 *     Ours shares `add r2, r8` as well:
 *       arm1  mov r2, #0xbf / lsl r2, #1 / ldr r3, =0x2092 / b .L8
 *       arm2  mov r2, #0xc1 / lsl r2, #1 / ldr r3, =0x5c
 *       .L8   add r2, r8 / strh r3, [r2]
 *     Cross-jumping merges maximal IDENTICAL tails, so the merge depth is
 *     decided by what sits immediately before the store in each arm.  In the
 *     ROM arm2 that is `mov r3, #0x5c` (the VALUE, computed last), in ours it
 *     is `add r2, r8` (the ADDRESS, computed last, because the value was
 *     already in a register).  The cause is one instruction earlier still:
 *
 *   WHAT RULES OUT THE ALTERNATIVES.  The value 0x5c reaches the store as a
 *   POOLED HImode constant in our build (`ldrh r3, .L19`, normalised by tryc to
 *   `=0x5c`, which is why tryc alone does not show it) where the ROM has
 *   `mov r3, #0x5c`.  0x5c fits Thumb's 8-bit immediate, so the ROM's value is
 *   an SImode move truncated by the strh; gcc's Thumb movhi sends a CONST_INT
 *   to the pool.  A pool LOAD is a memory reference and sched1 hoists it above
 *   the cheap address arithmetic; an immediate `mov` does not move.  So the
 *   merge depth is downstream of the store value's MODE, not of register
 *   allocation:
 *     - NOT allocno_compare / register allocation: the register assignment
 *       already matches the ROM exactly (r8 = q, r7 = act, r6 = p, r5 = the
 *       f5b address) once lever 1 is applied; no rotation remains.
 *     - NOT the scheduler on its own: the ROM's own order IS a scheduled order,
 *       and the three spellings that would give the value an SImode carrier
 *       (`int v`, an `off`/`d` pair, reusing `k`) each cost an allocno and score
 *       WORSE (144, 149, 41), so the mode cannot be fixed by a local carrier
 *       without paying more than it buys.
 *     - NOT the pool (A), which is downstream of the length gap this creates.
 *   THE OPEN QUESTION is therefore narrow and testable: a spelling that puts a
 *   Thumb-immediate-sized halfword store value in an SImode register WITHOUT
 *   adding a long-lived local.  The sibling idiom that does this in
 *   src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_b.c -- `off = 0xc1 << 1;
 *   p = base + off; off = 0x63; *(short *)p = off;`, on the SAME base and the
 *   SAME 0x182 offset -- is the right shape and was measured here at 149 of
 *   140 lines because this function has four more live values than that one.
 *   Try it again if the allocno pressure is ever reduced by other means.
 */
struct Slot {
    unsigned char pad00[0x18];
    int f18;
    int f1c;
    int f20;
    int f24;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    unsigned char padc[0x10 - 0xc];
    int f10;
    unsigned char pad14[0x5b - 0x14];
    unsigned char f5b;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001e70[];
extern int iwram_3001e40;
extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_8093554(struct Actor *a);
extern int OvlFunc_959_2009324(void);
extern void OvlFunc_959_2009980(int slot);
extern int OvlFunc_959_2009918(int slot);
extern int OvlFunc_959_20098e4(int slot);

void OvlFunc_959_200938c(void)
{
    struct Actor *act;
    struct Slot *p;
    unsigned char *q;
    int k;

    act = __MapActor_GetActor(0x11);
    p = (struct Slot *)(iwram_3001e70[0] + 0x164);
    q = iwram_3001e70[0x13];
    __Func_8093554(act);
    if (iwram_3001e40 & 1) {
        p->f18 = 1;
        p->f1c = 1;
    } else {
        p->f18 = -1;
        p->f1c = -1;
    }
    if (__GetFlag(0x106) || *(short *)(q + 0x17e) != 0
        || *(short *)(q + 0x180) != 0) {
        act->f5b = 1;
        return;
    }
    if (__GetFlag(0x214))
        return;
    act->f5b = 0;
    if (__GetFlag(0x214) == 0 && act->f5b == 0) {
        p->f20 = 0x3400000 - act->f8;
        p->f24 = 0x2400000 - act->f10;
    }
    if (OvlFunc_959_2009324())
        return;
    OvlFunc_959_2009980(0x11);
    if (OvlFunc_959_2009918(0x11)) {
        k = 0x24c;
        if (*(short *)(gState + k) != 0) {
            *(short *)(q + 0x17e) = 0x2092;
            return;
        }
    } else {
        k = 0x24c;
        if (*(short *)(gState + k) != 0)
            goto tail;
    }
    if (OvlFunc_959_20098e4(0x11)) {
        __SetFlag(0x215);
        __SetFlag(0x214);
    }
tail:
    if (__GetFlag(0x214))
        *(short *)(q + 0x182) = 0x5c;
}

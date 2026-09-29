/* Func_809c138 -- NON-MATCHING, 182 of 206 encodings differ.
 * Unattempted before batch 298.  Reference asm/rom_8a000/rom_9bb64_c_a_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_8a000/809c138.c \
 *       asm/rom_8a000/rom_9bb64_c_a_a.s --func Func_809c138
 *
 * NOT a distance: size 484 against 476 and count 208 against 206, so 182 is saturated.
 * aligncmp reads 150 of 206 (72.8%), 74 differing in 39 hunks.  SHIMS: 0.  Tail split
 * when it closes.
 * BLOCKER: cse/cse2 constant rematerialisation.  The ROM holds 0x1b in r9 for the
 * whole function -- `ldr r1,=0x1b / mov r9,r1`, reload's force_const_mem route, taken
 * because `mov r9,#imm` is not a Thumb encoding -- where we rematerialise `movs r0,#27`
 * at the call.  The consequence cascades: one fewer long-lived value competes for
 * r8-r11, so `q` never spills, the frame is 0x14 instead of 0x18, and EVERY sp
 * displacement in the function shifts.
 * A pin to r9 confirms the diagnosis (167 aligned) but is 4 instructions long, so it
 * is not a landing route.
 */
/* PARK -- Func_809c138  --  0x0809c138, asm/rom_8a000/rom_9bb64_c_a_a.s
 * (2 functions: Func_809bcf8 0x0809bcf8, Func_809c138.  Func_809c138 is the LAST,
 *  so it is a clean TAIL split -- ..._c_a_a.s keeps Func_809bcf8 and this file
 *  becomes src/rom_8a000/rom_9bb64_c_a_a_b.c.  Suffixes _a/_b are free.
 *  stage1.ld:1168 names asm/rom_8a000/rom_9bb64_c_a_a.o(.text), one line.
 *  `python3 tools/datacheck.py asm/rom_8a000/rom_9bb64_c_a_a.s` prints NOTHING.)
 *
 * NOT MATCHING.  objcmp:
 *     XX SIZE  ref 476 bytes, ours 484
 *     XX ENCODINGS differ in 182 place(s) (ref 206, ours 208)
 *   The 182 is NOT a distance -- the counts differ, so objcmp saturates.  The real
 *   figure is tools/aligncmp.py: aligned-equal 150 (72.8% of ref), 74
 *   differing/ins/del in 39 hunks.  SIZE 484 vs 476 (8 bytes long).
 *   tools/shimcount.py: 0 shims -- this candidate is PIN-FREE.
 *   Production flags (objcmp prints no "built with" line).
 *
 * THE BODY IS RIGHT.  Every call, every argument, every field offset, both
 * sixteen-iteration loops, the DMA3 pair, the do/while on gKeyRepeat and the
 * IME-locked DMA-queue push are all reproduced.  All 74 differences are one
 * register-allocation decision and its consequences.
 *
 * ============ THE BLOCKER, NAMED BY THE PASS ============
 *
 * cse / cse2 CONSTANT REMATERIALISATION, and through it the FRAME SIZE.
 *
 * The ROM keeps the value 0x1b in r9 for the whole function:
 *     ldr r1, =0x1b / mov r9, r1          (near the top, after galloc_ewram)
 *     ...  ~45 instructions and 4 calls later ...
 *     mov r0, r9 / bl GetFile
 * We emit `movs r0, #27` at the call site instead.  From the `-da` dumps (kept in
 * scratch_elev/b298d/da/): at .03.cse and .09.cse2 the constant 27 is ALREADY a
 * separate `(set (reg) (const_int 27))` at each of its two uses, and at .18.greg
 * both have become
 *     (insn 17  (set (reg:SI 0 r0) (const_int 27)) ... REG_EQUAL (const_int 27))
 *     (insn 193 (set (reg:SI 0 r0) (const_int 27)) ... REG_EQUAL (const_int 27))
 * -- rematerialised straight into the argument register at each site.  No
 * long-lived pseudo is ever created, so nothing competes for a callee-saved
 * register, and THAT is what changes the frame:
 *     ROM   r7=block r8=&buf r9=0x1b r10=bld r11=map-state, `q` SPILLED to sp+0,
 *           sav at sp+4, buf[16] at sp+8..0x17, `sub sp, #0x18`
 *     ours  r7=block r8=&buf r9=map-state r10=bld r11=q (NOT spilled),
 *           sav at sp+0, buf[16] at sp+4..0x13,        `sub sp, #0x14`
 * One fewer long-lived value means one more free high register means `q` stays in
 * r11 means the frame is one word smaller means EVERY sp displacement in the
 * function differs.  A frame that is exactly one word small next to a ROM that
 * spills a pointer is the signature of a missing long-lived value, not of a
 * missing variable.
 *
 * WHY THE ROM'S FORM IS A POOL LOAD, AND WHY THAT MATTERS.  `mov r9, #0x1b` is not
 * a Thumb instruction -- an immediate move only targets r0-r7 -- so a constant
 * destined for a high register must come through a low one.  reload takes the
 * `force_const_mem` route (`ldr r1, .LC` then `mov r9, r1`) rather than
 * `mov r1,#0x1b / mov r9,r1`; both are two instructions, and agbcc's
 * `*thumb_movsi_insn` would have given the `mov` for a constraint-I constant, so
 * the ROM's `ldr` is evidence that reload -- not expand -- materialised it.  That
 * in turn means the pseudo SURVIVED to reload, which is exactly what cse denies us.
 * cse propagates a constant into a use whenever the constant form is no more
 * expensive than the register form, and for 0x1b (`mov` immediate, one insn) it
 * always is.
 *
 * WHAT WOULD HAVE TO CHANGE: the value has to reach GetFile as something cse
 * cannot fold to an immediate.  Every constant spelling folds, which is the whole
 * of the measurement table below.  The productive next probe is NOT another
 * constant spelling -- it is to ask whether the original passed something that is
 * not a literal at all (a file-id enum read from memory, a caller's argument, or a
 * value shared with a neighbour in this same original file; asm/rom_8a000/
 * rom_9bb64*.s has unelevated siblings that also call GetFile and may show where
 * 0x1b comes from).
 *
 * ============ THE SECOND-ORDER RESIDUES (all downstream of the above) ============
 *
 *  R1. `ldr r2, =0x682 / sub r3, #0xca / strh r2, [r3]` then `sub r3, #0xa /
 *      strh r2, [r3]`: the ROM reaches REG_BG1CNT (0x400000a) and REG_DISPCNT
 *      (0x4000000) by SUBTRACTING from &REG_DMA3SAD (0x40000d4), which DMA3_SET
 *      left in r3.  That is reload_cse_move2add rewriting `set r3, 0x400000a` as
 *      `set r3, r3 - 0xca` because THAT HARD REGISTER already held 0x40000d4.  We
 *      pool both addresses because our allocation puts them in other registers.
 *      Same pass, same mechanism as the `sub r1, #2` in the Task_08097644 park.
 *  R2. OUR OBJECT HAS TWO LITERAL POOLS; the ROM has one, at the end.  Ours emits a
 *      mid-function pool (with a `b` over it, an alignment `.short`, and
 *      0x04000052 DUPLICATED in both pools) because the 8 extra bytes push a
 *      `ldr rN, [pc, #imm8*4]` out of its 1020-byte reach.  Those 8 bytes ARE the
 *      `b` + the `.short` + the duplicated word, so the pool split is
 *      self-sustaining: it is a CONSEQUENCE of the length, not an extra defect.
 *      Real instruction counts, pool artefacts excluded, already agree.
 *
 * ============ MEASURED -- DO NOT RE-RUN ============
 *
 *   0x1b as a plain literal at both call sites                150 aligned, 208 (SHIPPED)
 *   `short fid = 0x1b;` then GetFile(fid)                     150, 208 (byte-identical
 *     to the literal: cse folds the sign_extend of a known constant, then propagates)
 *   `unsigned short fid` / `int fid`, SHARED with
 *     galloc_ewram's first argument (so the value has TWO uses)  150, 208 each --
 *     all three byte-identical.  Two uses is not enough; cse propagates into both.
 *   `register int fid __asm__("r9") = 0x1b;`                   167, 210  <- BEST SEEN
 *     The pin proves the diagnosis (the frame grows to 0x18, `q` spills to sp+0,
 *     buf moves to sp+8, and 17 more encodings align) but costs 4 instructions:
 *     a pinned SImode high register still gets `mov r1,#0x1b / mov r9,r1` and adds
 *     moves at the call.  A pin that is 4 LONG is not a landing.
 *   `register short fid __asm__("r9")` / `register unsigned short`   163, 215 each --
 *     the narrow pinned register adds its conversions back at the call.
 *
 * ============ LEVERS THAT DID WORK (keep these in any re-attempt) ============
 *
 *  - THE HALFWORD/BYTE STORE CONSTANTS NEED ONE `int` PER SITE, IN ITS OWN BLOCK.
 *    109 -> 150 in two steps.  A single function-scope `int v` reused at all seven
 *    sites is allocated FUNCTION-WIDE and lands in r6, which then displaces the map
 *    pointer out of r6 into r8 and cascades; the ROM uses a different short-lived
 *    caller-saved register at each site (r2, r4, r3, r5, r2).  This is Task_Rain's
 *    recorded note ("one function-scope h is allocated function-wide and lands in
 *    r1") and it is worth restating as the general rule: a halfword-store `int`
 *    belongs in the INNERMOST block that needs it, never at function scope.
 *    Which sites need the int at all is READABLE OFF THE REFERENCE: a `mov` (or
 *    `mov`+`lsl`) means an int variable, a pool load means a bare literal.  Here
 *    `REG_BG1CNT = 0x682` and the final `f1e0[0x5b] = 0` are pooled, so they are
 *    literals; the other seven are `mov`s, so they are ints.
 *  - THE TWO LOOPS SHARE A SAVED BASE POINTER, AND IT IS TWO VARIABLES.  The ROM
 *    does `str r1, [sp]` and then MUTATES r1 as the first loop's cursor, restoring
 *    the saved copy with `ldr r1, [sp]` before the second loop.  Written as one
 *    cursor (the second loop continuing where the first stopped) the function is
 *    semantically wrong and 109 aligned; `q` (never mutated, spilled) plus `e`
 *    (the cursor, coalesced onto q's initial value) is right.
 *  - `cmp r2, r0 / ble` on a POINTER is a SIGNED compare, so the source loop
 *    variable is an `int` index: gcc's strength reduction turns `i < 16` into
 *    `p <= &buf[15]` and CARRIES THE SIGNEDNESS of the original comparison.  A
 *    `char *` loop written directly would give `bls`.
 *  - `DMA3_SET` from include/dma.h reproduces both `stmia r3!, {r0,r1,r2} /
 *    sub r3, #0xc` blocks exactly; shimcount does not charge dma.h's pins.
 *  - `struct DmaQueue { u16 count; struct DmaTransfer tasks[32]; }` and the
 *    LOCK_IME / SET_IO(REG_IME, REG_ADDR_IME) pair come VERBATIM from the matched
 *    sibling in this same bank, src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_a_c_a.c
 *    (Task_Transition300).  `strh r0, [r0]` storing the ADDRESS of REG_IME into
 *    REG_IME is that macro, not a ROM oddity.
 *  - `ldr r5, =iwram_3001e70 / ldr r6, [r5] / ... / sub r5, #8 / ldr r5, [r5]`:
 *    ONE base symbol with two offsets, the second derived.  `iwram_3001e70` as
 *    `unsigned char[]` with `*(T **)(iwram_3001e70 - 8)` gives it; the base
 *    survives galloc_ewram in a callee-saved register on its own.
 *  - `bld = (short)REG_BLDALPHA;` is what produces `ldrh / lsl #16 / asr #16`, and
 *    the queue entry stores `(unsigned short)bld` (`lsl #16 / lsr #16`) -- two
 *    different conversions of one value, so the variable is an `int` and the two
 *    casts are in the source.
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
    short fid;
    unsigned int savedIme;
    int count;
    u32 *task;
    struct DmaQueue *queue;

    m = *(unsigned char **)iwram_3001e70;
    s = galloc_ewram(0x1b, 0xccc);
    fid = 0x1b;
    t = *(struct T **)(iwram_3001e70 - 8);
    if (s->f19e != 3)
        return;
    {
        int one = 1;
        s->f1e0[0x5b] = one;
    }
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
    REG_BG1CNT = 0x682;
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

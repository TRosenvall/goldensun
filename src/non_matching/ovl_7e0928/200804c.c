/* OvlFunc_956_200804c  --  0x0200804c -- NON-MATCHING, 81 of 145 encodings differ.
 *
 * objcmp, verbatim, AT PLAIN -O2 -- NO FLAG ROW IS INVOLVED and none helps
 * (see the flag screen below), so this is the figure parkcheck re-measures:
 *
 *   XX SIZE  ref 316 bytes, ours 324
 *   XX ENCODINGS differ in 81 place(s) (ref 145, ours 147)
 *      first at index 6: ref 22fa  ours 20fa
 *
 * RELOCATIONS ARE SILENT -- objcmp prints no `RELOCATIONS differ` line, so the
 * symbol sequence, the count and every addend already match.
 *
 * SIZE IS NOT EXACT AND COUNT IS NOT EXACT (316/145 against 324/147), so the
 * objcmp count is SATURATED and does not rank.  aligncmp is the ranking view:
 *
 *   ref 145 encodings, ours 147
 *   aligned-equal 94  (64.8% of ref)   differing/ins/del 66 in 21 hunks
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7e0928/200804c.c \
 *       asm/overlays/rom_7e0928/ovl_30_a_c_c_a_a.s --func OvlFunc_956_200804c
 *
 * LANDING SHAPE: NO SPLIT.  asm/overlays/rom_7e0928/ovl_30_a_c_c_a_a.s is the
 * WHOLE file -- ONE function (`grep -c func_start` = 1), no data, so
 * datacheck.py reports nothing and split_s.py --dry-run is not applicable.
 * overlays/rom_7e0928/overlay.ld:20 names the object once,
 * `asm/overlays/rom_7e0928/ovl_30_a_c_c_a_a.o(.text)`, and no .ld .data/.bss
 * list names it.  The landing would be one line, asm/ -> src/.
 *
 * MIND THE BANK.  A file of the SAME NAME, ovl_30_a_c_c_a_a.s, also exists in
 * rom_7b4558 and is named at overlays/rom_7b4558/overlay.ld:35.  This park is
 * the rom_7e0928 one.  (An overlay address is not unique either -- every
 * overlay loads at the same base -- which is the census bug batch 302 fixed.)
 *
 * NO LABEL NEEDS `.global` FROM THIS LANDING.  The three statics this function
 * touches are already defined AND already exported by a sibling:
 * asm/overlays/rom_7e0928/ovl_30_c_c_c_c_c_c.s carries `.global .L4c20`
 * (line 8), `.global .L5480` (26, 30) and `.global .L5484` (31).  They are
 * reached from C by the established `__asm__(".Lxxxx")` spelling (see
 * src/rom_c9000/rom_dd2ac_c_c_b.c and friends), which is what the declarations
 * below do.  overlays/rom_7e0928/overlay.map puts them at 0x0200cc20,
 * 0x0200d480 and 0x0200d484.
 *
 * PIN-FREE: shimcount.py reports no register pins, no .equ shims, no "+r"
 * barriers, no empty asm.  No fakematch.txt row would be needed.
 *
 * ================ THE LEVERS THAT PAID, IN THE ORDER THEY PAID ================
 *
 * 1. THE gState OFFSET AS A LOCAL int.  137 of 145 -> 81 of 145; in tryc lines,
 *    137 differ -> 81.  THE SINGLE BIGGEST MOVE IN THIS FUNCTION.  Written
 *    `*(int *)(gState + 0x1f4)` gcc folds base+offset into one pool word
 *    (`ldr r3, =gState+500`); the ROM pools the bare symbol and materialises
 *    the offset (`mov r2, #0xfa / lsl r2, #1 / add r3, r2`).  `k = 0x1f4` as an
 *    int local reproduces it.  This is the same lever that landed the sibling
 *    OvlFunc_969_200871c BYTE-EXACT and that carried OvlFunc_959_200938c from
 *    141 to 39; it is now three-for-three on gState reads at a large offset and
 *    is the first thing to try on any of them.
 *
 * 2. A DECLARED LOCAL FOR THE REPEATED STACK ARGUMENT -- and it fixed a
 *    HIGH-REGISTER SWAP, which is worth recording because the swap looked like
 *    a coin flip in allocno_compare and was not.  The ROM keeps three
 *    loop-invariants in high registers: r9 = &.L4c20, r10 = 0xb, r8 = &.L5480.
 *    With `0xb` written as a literal at both __Func_8010704 call sites, gcc
 *    hoisted it as a cse-created pseudo and assigned r8, putting &.L5480 in
 *    r10 -- 0xb and &.L5480 SWAPPED against the ROM.  Giving the constant a
 *    declared local (`c = 0xb;` before the loop, passed as the 6th argument)
 *    makes it a user pseudo instead, and the assignment then matches the ROM
 *    on all three: r9, r10 and r8 as above.  81 differ -> 79 (tryc lines).
 *    The count barely moved; the RESIDUE CLASS changed completely, which is the
 *    reason to keep the spelling.  Declaration position was screened three ways
 *    (`c` first among the ints, last, and assigned before vs after `j`) and all
 *    three give 79 -- so it is the EXISTENCE of the declared local that moves
 *    allocno_compare here, not its order.
 *
 * 3. THE RANGE TEST WAS READ OFF THE ROM, NOT GUESSED.  The loop body's guard
 *    is `ldr r3,[r0,#8] / lsl r2,r6,#21 / sub r3,r2 / add r3,r14 / cmp r3,r7 /
 *    bhi` with r14 = 0x31ffff and r7 = 0x13fffe -- gcc's canonical
 *    `(unsigned)(x - LOW) <= (HIGH - LOW)`.  Subtracting LOW is an ADD of
 *    0x31ffff, so LOW = -0x31ffff and HIGH = -0x31ffff + 0x13fffe = -0x1e0001,
 *    over x = act->f8 - (i << 21).  Written `d > -0x3200000 && d < -0x1e0000`
 *    that folds back to exactly those two pool words.  (This is one of the
 *    `cmp K / bhi` range tests batch 302 established ARE reachable -- the old
 *    "unreachable comparison" advice is struck.)
 *
 * 4. INERT, MEASURED, NOT ASSUMED.  The index expression's operand order:
 *      L4c20[L5480 * 6 + (i - 0x12)]     79 differ   <-- this file
 *      L4c20[(i - 0x12) + L5480 * 6]     79 differ
 *      L4c20[i + L5480 * 6 - 0x12]       79 differ
 *    All three identical, which is `fold` canonicalising the PLUS -- the
 *    documented behaviour.  So the ROM's `add r3, r6, r3` (i as the first
 *    operand) CANNOT be reached by reordering the source addition, and this
 *    avenue is closed rather than untried.
 *
 *    FLAG SCREEN -- no flag row helps, all measured on this candidate:
 *      plain -O2                        79 differ   <-- best
 *      -fno-schedule-insns2            100 differ
 *      -fno-rerun-cse-after-loop       142 differ
 *
 * ================ THE BLOCKER, BY PASS ================
 *
 * sched1 -- THE FIRST SCHEDULING PASS (haifa, `schedule_insns`), NOT REGISTER
 * ALLOCATION.  Every remaining hunk is the SAME instructions in a DIFFERENT
 * ORDER, in two places:
 *
 *   (a) the loop-invariant setup, a straight 4-permutation:
 *         ROM   mov r6,#0x12 / mov r9,r3 / mov r10,r2 / mov r7,#0x21
 *         ours  mov r10,r2  / mov r7,#0x21 / mov r6,#0x12 / mov r9,r3
 *   (b) the copy of &.L5480 into its high register, which the ROM issues
 *       IMMEDIATELY after the load that uses the address and ours defers to the
 *       end of the index computation:
 *         ROM   ldr r3,=.L5480 / ldr r2,[r3] / mov r8,r3 / lsl r3,r2,#1 / ...
 *         ours  ldr r5,=.L5480 / ldr r2,[r5] / lsl r3,r2,#1 / ... / mov r8,r5
 *       The low-register temp choice (ROM r3 and r2, ours r5 and r0) follows
 *       from (b): the ROM frees its temp one instruction after the load, so the
 *       same register is available again for the index arithmetic.
 *
 * WHAT RULES OUT THE ALTERNATIVES:
 *   - NOT register allocation / allocno_compare.  After lever 2 the high-register
 *     ASSIGNMENT matches the ROM exactly -- r9 = &.L4c20, r10 = 0xb,
 *     r8 = &.L5480 -- and the prologue/epilogue register lists are identical
 *     (`push {r5,r6,r7,lr} / mov r7,r10 / mov r6,r9 / mov r5,r8 / push {r5,r6,r7}`
 *     on both sides).  There is no rotation and no extra allocno left to find;
 *     the count of high registers, and which value is in which, both agree.
 *   - NOT CSE or gcse.  -fno-rerun-cse-after-loop takes it from 79 to 142 and
 *     -fno-schedule-insns2 to 100; both directions are worse, so the ROM's order
 *     is a SCHEDULED order that differs from ours, not an UNSCHEDULED one.
 *     Turning the second scheduler off cannot produce it.
 *   - NOT the source's addition order, closed by lever 4's three-way screen.
 *   - NOT the relocation FORM: objcmp prints no relocation line at all here, so
 *     there is nothing of that class to discount.
 *
 * THE ONE ITEM I CANNOT ATTRIBUTE TO sched1, stated as an open question rather
 * than folded into the above: `add r3, r6, r3` (ROM, Thumb ADD(2), flag-setting)
 * against `add r3, r6` (ours, Thumb ADD(4)).  That is instruction SELECTION,
 * decided by which pseudo the arm backend ties to the destination, and lever 4
 * shows the source addition order does not reach it.  It is 1 encoding of the
 * 66, and it may well be downstream of (b) -- the ROM's r3 is dead-and-reborn
 * where ours is still live -- but that was not demonstrated and should not be
 * assumed.
 *
 * WHAT THE FUNCTION IS.  138 instructions, a per-frame two-phase animation
 * driver over map slots 0x12..0x16, keyed on a frame counter (.L5484, unsigned,
 * wrapping at 0x11 -- the tail's `bls` is what makes it unsigned) and a 2-bit
 * rotating index (.L5480).  On the counter's zero frame it advances the index
 * and re-animates the whole slot run from the 6-byte-stride table .L4c20; on
 * every other frame it walks the same table row and stamps a value at
 * base+0x182 when the actor is within a narrow depth band and the level nibble
 * and the table entry agree.  The two `if`s in that arm are deliberately NOT an
 * `else if`: the ROM tests 0xb/4 and then 0xc/5 unconditionally in sequence.
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
struct Actor {
    unsigned char pad00[8];
    int f8;
    unsigned char padc[0x10 - 0xc];
    int f10;
};

extern unsigned char gState[];
extern unsigned char iwram_3001ebc[];
extern signed char L4c20[] __asm__(".L4c20");
extern int L5480 __asm__(".L5480");
extern unsigned int L5484 __asm__(".L5484");

extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_956_200804c(void)
{
    struct Actor *act;
    unsigned char *base;
    signed char *t;
    short *dst;
    int lvl;
    int i;
    int j;
    int v;
    int d;
    int k;
    int c;

    k = 0x1f4;
    base = *(unsigned char **)iwram_3001ebc;
    act = __MapActor_GetActor(*(int *)(gState + k));
    lvl = act->f10 >> 20;
    if (L5484 == 0) {
        L5480 = (L5480 + 1) & 3;
        c = 0xb;
        j = 0x21;
        for (i = 0x12; i <= 0x16; i++) {
            v = L4c20[L5480 * 6 + (i - 0x12)];
            __MapActor_SetAnim(i, v);
            __MapActor_SetAnim(i + 5, v + 8);
            __Func_8010704(0x20, 0xb, 1, 2, j, c);
            if (v != 7)
                __Func_8010704(0x4a, 0xc, 1, 1, j, c);
            j += 2;
        }
        __MapActor_SetAnim(0x1c, L4c20[L5480 * 6 + 5]);
    } else {
        dst = (short *)(base + 0x182);
        t = &L4c20[L5480 * 6];
        for (i = 0x12; i <= 0x16; i++) {
            v = *t++;
            d = act->f8 - (i << 21);
            if (d > -0x3200000 && d < -0x1e0000) {
                if (lvl == 0xb && v == 4)
                    *dst = v;
                if (lvl == 0xc && v == 5)
                    *dst = v;
            }
        }
    }
    L5484++;
    if (L5484 > 0x11)
        L5484 = 0;
}

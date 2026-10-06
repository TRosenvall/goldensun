/* OvlFunc_956_2008ad4 -- NOT MATCHING. 9 of 36, same length.
 *
 * NON-MATCHING, 9 of 39 encodings  (MEASURED, batch 319 recipe backfill).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7e0928/2008ad4.c \
 *     asm/overlays/rom_7e0928/ovl_30_c_c_a_c_a.s --func OvlFunc_956_2008ad4
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * Source asm: goldensun/asm/overlays/rom_7e0928/ovl_30_c_c_a_c.s
 *
 * Blocker: register choice on two constant builds, and one of them is the
 * straight-line interleave.
 *
 *   the gState offset 0xfa << 1 goes into r0 in the ROM and r2 in ours -- r0 is
 *   free there, before the first call, and gcc simply picks differently
 *
 *   the 0xc0 << 13 addend is built EARLY in the ROM, its `mov r0, #0xc0` landing
 *   BEFORE the `and r3, r2` that masks the field it is added to. gcc emits the
 *   whole build after the mask. That is arg-interleave, and this function has no
 *   branch before the call for the basic-block lever to use.
 *
 * TRIED: the offset as an inline `(0xfa << 1)` rather than a variable shifted in
 * two statements -- 33 lines, gcc folds it to a single pool load and the
 * function gets three instructions shorter.
 *
 * The offset MUST stay a variable shifted in place, which is the same
 * requirement as the GetEntrances family. What is left is allocation plus one
 * unreachable interleave.
 

 *
 * ===== BATCH 329 BRIEF I: RE-DERIVED AT 9, AND BOTH RUNS ARE RELOAD =====
 *
 * Re-measured unfiltered: 9 differing encodings of 39, ref 39 ours 39, first at
 * index 2, no SIZE and no POOL WORD line, so the streams are aligned.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7e0928/2008ad4.c asm/overlays/rom_7e0928/ovl_30_c_c_a_c_a.s --func OvlFunc_956_2008ad4
 *
 * REFUTED, and it changes which pass you have to argue with. The header says
 * "r0 is free there, before the first call, and gcc simply picks differently",
 * i.e. it reads both runs as register ALLOCATION. They are not allocation at
 * all. `.18.greg` reads `;; 0 regs to allocate:` -- this function has NO global
 * allocnos -- and then
 *     Spilling for insn 19.  Using reg 3 for reload 0
 *     Spilling for insn 62.  Using reg 2 for reload 0
 * The 0xc0 << 13 addend is not a pseudo anywhere: insn 62 is
 * `(set (reg 35) (plus (reg 41) (const_int 1572864)))` with the constant as an
 * IMMEDIATE that thumb's addsi3 cannot take, so RELOAD materialises it. Same
 * for the 0xfa << 1 offset at insn 19, which `.19.flow2` shows as the two
 * inserted insns 95/96 writing r2. So both contested registers come from
 * `reload1.c:allocate_reload_reg` (:4962), whose loop is `i = last_spill_reg`
 * and then a round-robin over ASCENDING `spill_regs[]` -- meaning the register
 * depends on the PHASE left behind by every earlier reload in the function, not
 * on what is free at the insn. Note also that the printed `Using reg 3` for
 * insn 19 is not the emitted register: flow2 emits r2. Printed and emitted
 * differ here, exactly as the standing warning says.
 *
 * DECOMPOSITION, 9 in three runs:
 *   A  3 insns. `mov r0,#0xfa / lsl r0,#0x1 / add r6,r0` against r2. reload.
 *   B  4 insns. rom `mov r0,#0xc0 / and r3,r2 / lsl r0,#0xd / add r3,r0`
 *      against ours `and r3,r2 / mov r2,#0xc0 / lsl r2,#0xd / add r3,r2`.
 *      reload. ours takes r2 because the mask pseudo 42 (`;; Register 42 in 2.`)
 *      has just died in the `and`; the `mov` is then pinned after the `and` by
 *      its own anti-dependence, which is why sched2 cannot hoist it.
 *   C  2 insns. `ldr r1,[r5,#0x8] / mov r0,r5` against the reverse -- the ROM's
 *      `mov r0, r5` is blocked behind `add r3, r0` and ours is not. DOWNSTREAM
 *      OF B, not a third cause.
 *
 * RUN B IS THE SAME SHAPE AS ovl_7d0e88/200a1ac.c's FIRST RUN, in a different
 * overlay: a constant that the ROM materialises into a register free from
 * earlier and hoists above an `and`, against gcc taking the register the `and`
 * just freed. A generated-against-hand census of that shape over all of `asm/`
 * (4,468 gcc-generated files against 740 hand-written) finds the ROM's hoisted
 * form TWICE in gcc's own output against 66 of ours, and one of the two hits,
 * in `src/rom_8a000/rom_93304_c_a_c.s`, is this run exactly --
 * `ldr r2,.L3+4 / ldr r3,[r5,#8] / mov r0,#128 / and r3,r2 / lsl r0,#12 /
 *  add r3,r0 / str r3,[r5,#8] / ldr r3,[r5,#16] / and r3,r2`.
 * The distinguishing feature there is the LAST line: the mask in r2 is USED
 * AGAIN after the first `and`, so it does not die, its register is not freed,
 * and the constant cannot take it. This function's mask is read once.
 *
 * MEASURED AND INERT, both at 9 and both new dimensions:
 *   the constant FIRST in the addition, `(0xc0 << 13) + (field & 0xfff00000)`
 *     -- inert, and necessarily so: gcc canonicalises a constant to operand 2.
 *   a named `int k = 0xc0 << 13;` assigned at the top of the function, to give
 *     the addend a pseudo with an early definition instead of a reload -- inert,
 *     the reload is reinstated.
 *
 * SO THE NAMED CAUSE IS: a reload register for a large immediate, chosen by
 * round-robin phase, where the ROM's build had the mask still live. Giving the
 * mask a second reader is not available in this program.
*/
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern void *__MapActor_GetActor(int slot);
extern void __MapActor_Surprise(int slot, int a);
extern void __Actor_SetAnim(void *a, int anim);
extern void __Actor_TravelTo(void *a, int x, int y, int z);
extern void __Actor_WaitMovement(void *a);

void OvlFunc_956_2008ad4(void)
{
    unsigned char *q;
    unsigned char *a;
    unsigned int off;
    int t;

    q = (unsigned char *)&gState;
    off = 0xfa;
    off <<= 1;
    q += off;
    a = (unsigned char *)__MapActor_GetActor(*(int *)q);
    *(int *)(a + 0x34) = 0x80 << 9;
    *(int *)(a + 0x30) = 0x80 << 10;
    __MapActor_Surprise(*(int *)q, 0x81 << 1);
    __Actor_SetAnim(a, 5);
    t = (*(int *)(a + 0x10) & 0xfff00000) + (0xc0 << 13);
    __Actor_TravelTo(a, *(int *)(a + 8), *(int *)(a + 0xc), t);
    __Actor_WaitMovement(a);
}

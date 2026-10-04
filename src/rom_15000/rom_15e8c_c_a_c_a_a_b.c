/* Func_80173f4 -- 0x080173f4, the only function in
 * asm/rom_15000/rom_15e8c_c_a_c_a_a_b.s (grep -c func_start = 1), so it
 * converts WHOLE-FILE: no data section (datacheck.py exits 0), NO SPLIT.
 *
 * MATCHED. 112 bytes, 46 encodings and 4 relocations identical, --whole green.
 * NO PINS, no shim, no fakematch row, no per-file flag.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_15e8c_c_a_c_a_a_b.c \
 *     asm/rom_15000/rom_15e8c_c_a_c_a_a_b.s --whole
 *
 * A task setup: upload sprite graphics, write six halfwords into the module
 * block at iwram_3001e8c, then register a per-frame task.
 *
 * ============================= WHAT CLOSED IT =============================
 *
 * The park (src/non_matching/rom_15000/80173f4.c) read 18 of 46 and named the
 * blocker "gcc HOISTS the pooled address offsets". That observation is right.
 * Its VERDICT -- that a pointer local per store "does the opposite, because
 * each pointer is then a named value gcc must place and the pressure goes UP"
 * -- is REFUTED. A pointer local per store is HALF OF THE FIX, and it was in
 * the park's rejected-because-worse list.
 *
 * THREE LEVERS, two of them in the park's own negatives:
 *
 * 1. KEPT FROM THE PARK -- the stored constants through `int` locals. A bare
 *    literal into a halfword store is HImode, and gcc pools it. This is now
 *    settled from the compiler source rather than from dumps:
 *    arm.md:4318 `*thumb_movhi_insn` has constraints "=l,l,m,*r,*h,l" /
 *    "l,mn,l,*h,*r,I". Alternative 1's `n` matches ANY const_int and sits
 *    BEFORE alternative 5's `I` (the 8-bit immediate `mov`), and recog takes
 *    the first match -- so **alternative 5 is unreachable for a HImode
 *    const_int and even the value 9 is forced through force_const_mem**.
 *    An `int` local keeps the move SImode, where *thumb_movsi_insn's immediate
 *    alternatives are reachable, and gives the ROM's `mov r3, #9`.
 *
 * 2. SIX DISTINCT POINTER LOCALS, one per store, each assigned BEFORE its
 *    value local. 18 -> 2. The park tried this and measured 32; the difference
 *    is that the park reused ONE pointer local and/or ordered the value first.
 *    Both re-measured here:
 *        one pointer local `q` reused for all six      21  (-4 bytes)
 *        value local assigned BEFORE the pointer       20
 *        six pointer locals, pointer assigned first     2  <- this
 *
 *    THE MECHANISM, read off `.15.regmove` and the park's own symptom. The
 *    park blamed sched2. It is NOT sched2: `-fno-schedule-insns2` measures 23,
 *    WORSE than the base, and sched2 already produces the ROM's INSTRUCTION
 *    ORDER (offset load, add, value mov, store). The defect is REGISTER
 *    ALLOCATION, which runs on the UNSCHEDULED stream. There,
 *    `v1 = 9; *(u16*)(p + 0x12b0) = v1;` emits
 *
 *        (set p_val 9) (set p_off 0x12b0) (set p_sum (plus p p_off)) (store)
 *
 *    so at the `add` THREE pseudos are live -- value, offset, sum -- and the
 *    offset cannot have the register the value holds. The ROM needs only TWO
 *    live at once, which is why it can write `ldr r3,=0x12b0 / add r2,r5,r3 /
 *    mov r3,#9` and REUSE r3 for the value the instant the offset dies.
 *    Naming the pointer moves the address computation into its own earlier
 *    statement, so the offset is dead before the value is born:
 *
 *        (set p_off 0x12b0) (set q (plus p p_off)) (set p_val 9) (store)
 *
 *    Two live, and gcc then produces the ROM's register pattern exactly --
 *    r2/r3 ping-ponging with r1 holding the shared zero, r0 never touched
 *    after the call. Indices 8-26 all close together.
 *
 *    So the park's "shortening a live range by naming it is self-defeating"
 *    is the wrong generalisation. Naming the POINTER shortens the OFFSET's
 *    range, which is the one that was too long.
 *
 * 3. THE PRIORITY THROUGH AN `int` LOCAL. 2 -> 0. The last two indices were a
 *    straight swap of `lsls r1,r1,#4` and `ldr r0,=Func_801789c` before the
 *    `bl StartTask`. Both are at sched2 priority 65 (`.23.sched2` dependence
 *    table at -fsched-verbose=6: insn 98 prio 65, insn 139 prio 65), so
 *    rank_for_schedule falls through to the INSN_LUID rung and the lower LUID
 *    wins -- and expand emits the function-address load BEFORE the priority
 *    arithmetic. Computing the priority into its own statement first gives it
 *    the lower LUID and the tie resolves the ROM's way. `fn = Func_801789c`
 *    named INSTEAD is exactly inert (2); named AS WELL is also 0.
 *
 * ========================= MEASURED INERT / WORSE =========================
 *   bare literals for the stored values           10, 39 insns (length wrong)
 *   one int local reused for all stored values    25
 *   -fno-schedule-insns2                          23   (worse: not a sched2 bug)
 *   one pointer local reused for all six stores   21, -4 bytes
 *   value local before the pointer local          20
 *   -fno-schedule-insns / -fno-gcse / -fno-force-mem / -fno-cse-follow-jumps
 *     / -fno-rerun-cse-after-loop / -fno-strict-aliasing / -fno-caller-saves
 *     / -fno-expensive-optimizations             all 18, exactly inert
 *   priority written `3200` instead of `0xc8 << 4`  2, exactly inert
 *   `fn = Func_801789c` local without the priority local   2, exactly inert
 *
 * The pool WORDS were never the problem: the park's own compile already had
 * all seven in the ROM's order (idx 39-45 identical), so `.26.mach` was not
 * needed here.
 */
extern int iwram_3001e8c;
extern int UploadSpriteGFX(int a, int b, int c);
extern void StartTask(void *fn, int prio);
extern void Func_801789c(void);

void Func_80173f4(void)
{
    char *p;
    unsigned short *q1;
    unsigned short *q2;
    unsigned short *q3;
    unsigned short *q4;
    unsigned short *q5;
    int v1;
    int v2;
    int v3;
    int zero;
    int pr;

    p = (char *)iwram_3001e8c;
    *(unsigned short *)(p + 0x12b8) = UploadSpriteGFX(0x5f, 0x80 << 6, 0);
    q1 = (unsigned short *)(p + 0x12b0);
    v1 = 9;
    *q1 = v1;
    q2 = (unsigned short *)(p + 0xea8);
    v2 = 0xa;
    *q2 = v2;
    q3 = (unsigned short *)(p + 0xeac);
    zero = 0;
    *q3 = zero;
    q4 = (unsigned short *)(p + 0xeae);
    v3 = 0xf;
    *q4 = v3;
    q5 = (unsigned short *)(p + 0x12b2);
    *q5 = zero;
    pr = 0xc8 << 4;
    StartTask(Func_801789c, pr);
}

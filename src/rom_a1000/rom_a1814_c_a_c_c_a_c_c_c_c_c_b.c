/* Func_80a33d4 -- 0x080a33d4.  *** MATCHING, byte-identical. ***
 *
 * LANDED batch 340 brief E.  objcmp verbatim, re-derived on this body:
 *     OK Func_80a33d4 -- 136 bytes, 65 encodings and 3 relocations identical
 *
 * The park it replaces read 16 of 65 STREAM encodings, 61/61 16-bit
 * instructions (IN PHASE, a true distance), relocations identical, PINS 0.
 * Both figures re-derived at the head of this batch and both confirmed; the
 * batch-338 recovered body and its figure were exactly right.
 *
 * INSTALL AS:  src/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c_b.c
 *
 * SPLIT.  The reference holds two functions and the other, Func_80a3354, is
 * still parked at 25 of 58, so this ships on a cut.  Func_80a33d4 is the
 * LAST function in the file (lines 81-149 of 149), so the cut is two-way:
 *     python3 tools/split_s.py \
 *       asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c.s Func_80a33d4
 *     [dry-run] would write ..._a.s  (1 function(s), 72 lines)   Func_80a3354
 *     [dry-run] would write ..._b.s  (1 function(s), 75 lines)   Func_80a33d4
 *     [dry-run] would REMOVE ..._c_c.s ; would rewrite stage1.ld
 * tools/datacheck.py on the reference exits 0 with no output -- no data
 * between the two functions to strand.  The surviving park
 * src/non_matching/rom_a1000/80a3354.c must have its reference repointed to
 * the new ..._a.s.
 *
 * Verify with (AFTER the split, on the installed path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c_b.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c_b.s --whole
 * Verify BEFORE the split (what was actually run to land it):
 *   ... tools/objcmp.py <this file> \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c.s --func Func_80a33d4
 *
 * PINS: 0 (tools/shimcount.py reports no rows).  DEVICES: 0 -- no __asm__, no
 * do{}while(0) anchor, no .equ, no fictitious symbol.  Default flags.
 * NO fakematch row needed.
 *
 * ======================================================================
 * WHAT THE PARK GOT RIGHT, AND THE ONE WORD THAT WAS WRONG
 * ======================================================================
 *
 * The park's observation was exact: all 16 encodings were the interleaving of
 * a four-instruction loop-preheader setup, repeated three times.  Per loop:
 *     ref   mov r3,#0xa8 | mov r6,r8 | mov r5,#N | mov r10,r3 | add r6,#off
 *     ours  mov r6,r8 | mov r3,#0xa8 | add r6,#off | mov r5,#N | mov r10,r3
 * (5 encodings x 3 loops, plus one extra in loop 3, where our r10 shuttle got
 * r8 instead -- `mov r8,r3` / `mov r3,r8` against the ref's `mov r10,r3` /
 * `mov r3,r10` -- because r8 held `a` and was dead after the last `mov r6,r8`.
 * That 16th encoding was a CONSEQUENCE of the order, and it closed with it.)
 *
 * The park's verdict was: "NAMING IT MAKES IT WORSE ... So the 'name a
 * repeated stack argument' lever inverts here: the ROM's repetition is the
 * thing that cannot be asked for."  *** THAT IS WRONG, AND THE WORD THAT MADE
 * IT WRONG IS "A": one named local is commoned, THREE DISTINCT ONES ARE
 * NOT. ***  Re-measured this batch on the recovered body:
 *     one local `e`, reassigned before each loop   42 of 65, dsize -8, RELOCDIFF
 *     three locals `e1 e2 e3`, same placement       0  -- byte-identical
 * The park tried "one named local (63/45); three named locals (63/45)" and
 * recorded them as the same number.  Either the three-local row was never
 * actually run or it reused one name; it is not reproducible -- the two rows
 * differ by 42 encodings and 12 bytes on this body.
 *
 * ======================================================================
 * THE MECHANISM: A PREHEADER LUID TIE, AND TWO EDITS ARE NEEDED
 * ======================================================================
 *
 * Every insn in one of these preheaders is a scheduling-region LEAF or feeds
 * exactly one in-region consumer, so sched2 ranks them almost entirely on
 * INSN_LUID.  Priorities, read off the five insns' forward chains
 * (`priority`, haifa-sched.c:3129-3163, maximising insn_cost + priority over
 * INSN_DEPEND; arm.md:254 gives each of these `core_cycles single` insns
 * latency 1):
 *     mov r3,#0xa8 -> mov r10,r3     prio 2   (true dep inside the block)
 *     mov r6,r8    -> add r6,#off    prio 2
 *     mov r5,#N                      prio 1   (consumers are in the LOOP
 *     mov r10,r3                     prio 1    block, a different region, so
 *     add r6,#off                    prio 1    they contribute nothing)
 * so the two prio-2 heads issue first and the three prio-1 insns follow, and
 * WITHIN each priority class rank_for_schedule's last tie-break is INSN_LUID,
 * lowest first (haifa-sched.c:4029ff; its register-pressure rung is gated
 * `!reload_completed` and is dead in sched2).  Simulating the ready list from
 * the parked body's LUID order [r6=r8, r6+=off, r5=N, r3=0xa8, r10=r3]
 * reproduces our stream exactly -- r6=r8, r3=0xa8, r6+=off, r5=N, r10=r3 --
 * and the ROM's stream requires LUID order [r5=N, r3=0xa8, r10=r3, r6=r8,
 * r6+=off]: the COUNTER INIT FIRST, THEN THE 0xa8, THEN THE POINTER.
 *
 * That is two independent moves, and NEITHER ALONE REACHES IT:
 *   1. The 0xa8 must stop being a LICM hoist.  As a literal fifth argument it
 *      is loop-invariant, so move_movables lifts it with
 *      `emit_insn_before (..., loop_start)` -- at the preheader's END, i.e.
 *      ABOVE every ordinary preheader statement in LUID terms, which is why
 *      its two insns sort last in the parked body.  Writing it to a local
 *      before the loop makes it an ORDINARY preheader statement at the LUID
 *      its source position gives it.  Three distinct locals because gcse
 *      commons three assignments of the same constant to the SAME name back
 *      into one, hoists that to the function top, and then the ROM's three
 *      re-materialisations are gone (-8 bytes, RELOCDIFF).
 *   2. The counter init must precede it.  `for (i = N; i <= M; i++)` puts
 *      `i = N` in the preheader AFTER whatever precedes the for-statement, so
 *      the explicit `i = N;` ahead of the named constant is what lands it.
 *
 * MEASURED, this batch, 21 variants in two sweeps (sweep_variants.py):
 *     counter init first, pointer second, 0xa8 still a literal      13
 *     ... as a for(;;) / do-while / ptr-add / 3 pointers / decl order  13 each
 *     three named locals, for(i=N;...) unchanged (no explicit init)   9
 *     named 0xa8 BEFORE the counter init                              6
 *     block-scoped `int e = 0xa8;` per loop (same name each time)     6
 *     pointer derived BEFORE the counter init, named 0xa8 after      16 (inert)
 *     three locals initialised AT DECLARATION                        66, +4 bytes
 *     two locals (one shared by loops 2 and 3)                       25, -4 bytes
 *     ONE local reassigned per loop                                  42, -8 bytes
 *   BYTE-IDENTICAL, three spellings, all three measured 0:
 *     `i = N; eK = 0xa8; p = ...; while (i <= M) { ...; i++; }`   <- shipped
 *     `i = N; eK = 0xa8; p = ...; for (; i <= M; i++) ...`
 *     `i = N; eK = 0xa8; p = ...; do { ...; i++; } while (i <= M);`
 *   and `0x100` for `0x80 << 1` is byte-identical too, so that spelling is
 *   free; `0x80 << 1` is kept because the ROM's `mov r3,#0x80 / lsl r3,#1`
 *   is what the shift form documents.
 *
 * AUTOSWEEP (tools/autosweep.py, run FIRST, raw in scratch_elev/b340/E/work):
 *   FLAG AXIS, parked body, all eight groups: -fno-gcse 16, -fno-strict-
 *   aliasing 16, -fno-strength-reduce 16, -fno-rerun-cse-after-loop 16,
 *   -fno-rerun-loop-opt 16 (all inert); -O1 30, -fno-schedule-insns2 30,
 *   -ffixed-r7 64 (all worse).  The SCHED2 row is the positive control for
 *   the diagnosis above: turning sched2 off nearly doubles the figure, so the
 *   residue was sched2 ORDER and not allocation.
 *   ANCHOR AXIS: 13 boundaries, anchor05 and anchor09 each 16 -> 15, six
 *   produced no row (untested, not inert).  Not needed -- the source form
 *   above reaches 0 with no anchor at all.
 *
 * ======================================================================
 * PASS 4 (humanization): `e1 e2 e3` is almost certainly not the original
 * spelling.  Three locals that all hold 0xa8 reads as an artifact; the real
 * source more likely had three separately-named quantities (a per-row height,
 * say) that happen to share a value.  The REQUIREMENT is only that they be
 * three distinct automatics assigned between the counter init and the pointer
 * derivation; any names satisfy it.  Recorded for docs/directory/03-humanize.
 * ======================================================================
 */
extern int _Func_801eb64(int a, int b, int c, int d, int e);

void Func_80a33d4(unsigned char *a, int b)
{
    int i;
    int *p;
    int e1, e2, e3;

    i = 0;
    e1 = 0xa8;
    p = (int *)(a + 0x48);
    while (i <= 7) {
        *p++ = _Func_801eb64(2, i, b, 0xf8, e1);
        i++;
    }
    i = 8;
    e2 = 0xa8;
    p = (int *)(a + 0x68);
    while (i <= 0xf) {
        *p++ = _Func_801eb64(2, i, b, 0x80 << 1, e2);
        i++;
    }
    i = 0x10;
    e3 = 0xa8;
    p = (int *)(a + 0x88);
    while (i <= 0x1f) {
        *p++ = _Func_801eb64(2, i, b, 0x80 << 1, e3);
        i++;
    }
}

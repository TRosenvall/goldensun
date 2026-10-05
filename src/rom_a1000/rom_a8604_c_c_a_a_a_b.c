/* Func_80a9a5c @ 0x080a9a5c -- MATCHING, 0 differing encodings of 59.
 * *** AND SYMBOL-FREE: 12 relocations, NOT the 13 the parked body needed. ***
 *
 * FIGURE (re-derived this batch, batch 327 brief E, never inherited):
 *   OK Func_80a9a5c -- 144 bytes, 59 encodings and 12 relocations identical
 *   SIZE 144/144.  INSTRUCTION COUNT: 59 against 59, no objcmp COUNT line.
 *   PIN COUNT 0.  tools/shimcount.py: "empty asm : 1 (NOT treated as a
 *   fakematch in this tree)".
 *
 * Verify with -- NAMES THE INSTALLED PATH, ONE LINE:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_a1000/rom_a8604_c_c_a_a_a_b.c asm/rom_a1000/rom_a8604_c_c_a_a_a_b.s --whole
 *
 * SPLIT SHAPE (the reference holds 2 functions; this is one of them):
 *   python3 tools/datacheck.py asm/rom_a1000/rom_a8604_c_c_a_a_a.s   -> CLEAN (exit 0)
 *   python3 tools/split_s.py asm/rom_a1000/rom_a8604_c_c_a_a_a.s Func_80a9a5c --dry-run
 *     [dry-run] would write asm/rom_a1000/rom_a8604_c_c_a_a_a_b.s  (1 function, 67 lines)  <- THIS ONE
 *     [dry-run] would write asm/rom_a1000/rom_a8604_c_c_a_a_a_c.s  (1 function, 101 lines)
 *     [dry-run] would REMOVE asm/rom_a1000/rom_a8604_c_c_a_a_a.s
 *     [dry-run] would rewrite stage1.ld
 *   Text-only split, no exports needed.  Installs to
 *   src/rom_a1000/rom_a8604_c_c_a_a_a_b.c.
 *
 * ================================================================
 * THE OWNER'S `_MSG_b24` RULING IS HONOURED AND NOT RE-OPENED
 * ================================================================
 * docs/owner-decisions.md entry 1 DECLINED `_MSG_b24 = 0xb24;` in message.sym,
 * because 0xb24 is neither 8-bit-movable nor shiftable, so gcc pools it as a
 * literal anyway and the ROM's pool word carries NO relocation -- the bytes are
 * equally consistent with a plain literal.  **THAT RULING IS NOW MOOT RATHER
 * THAN OVERTURNED.**  This body uses the plain literal `0xb24`, emits the ROM's
 * 12 relocations and no 13th, and still matches.  `message.sym` is untouched.
 *
 * ================================================================
 * WHAT THE PARK GOT WRONG, AT THE RUNG LEVEL
 * ================================================================
 * Symbol-free, barrier-free, the body measures **9 differing encodings of 59**
 * (ref 59, ours 59; relocations identical in SYMBOL and ORDER, the first three
 * `bl` offsets shifted +2).  The 9 IS a distance, and it is ONE RUN:
 * **ONE INSTRUCTION MOVED EIGHT SLOTS.**  The ROM holds `ldr r5,=0xb24` at
 * index 13, after the three leading `bl`s; we hold it at index 5, between
 * `ldr r3,=iwram_3001f2c` (4) and `ldr r3,[r3]` (ROM 5).  Indices 5..13 all
 * shift by one.  Nothing else differs.
 *
 * The park said the hoist happens because insn 29 "competes with `ldr r3,[r3]`
 * (also 11) and WINS THE TIE", and concluded "no priority or tie-break spelling
 * reaches the ROM's placement".  Read off `.23.sched2`
 * (-da -fsched-verbose=6), block 0:
 *
 *   ;;      insn  code    bb   dep  prio  cost   units
 *   ;;       16   173     0     0    13     2   core : 88 22 137      <- ldr r3,=iwram_3001f2c
 *   ;;      137   173     0     1    11     2   core : 88 38 26 24 22 18   <- ldr r3,[r3]   (6 deps)
 *   ;;       29   173     0     0    11     2   core : 88 66 54 42 32      <- ldr r5,=0xb24 (5 deps)
 *   ;;  Ready list (t = 0):  8  4  29  16   --> schedules 16   (rung 1, prio 13)
 *   ;;  Ready list (t = 2):  8  4 137  29   --> schedules 29   *** THE CONTEST ***
 *
 *   rung 1  `:4041` INSN_PRIORITY ......... 11 == 11.  TIES.
 *   rung 2  `:4046` INSN_REG_WEIGHT ....... DEAD, gated `!reload_completed`.
 *   rung 3  `:4067-4094` last_scheduled_insn CLASS, last_scheduled_insn = 16:
 *             137: find_insn_list HITS (16 -> 137, true dep), insn_cost 2 != 1,
 *                  REG_NOTE_KIND == 0                       => class 1
 *              29: find_insn_list returns 0 -- insn 29 HAS NO PREDECESSOR AT ALL
 *                  (`dep 0`)                                => class 3
 *           `(val = tmp2_class - tmp_class)` is nonzero.  *** RUNG 3 DECIDES. ***
 *   rung 4  `:4100-4110` dependent count ... NEVER REACHED -- and it would have
 *           given 137 SIX against 29's FIVE, i.e. **THE ROM'S ANSWER**.
 *
 * So the park was wrong twice: the contest is decided ABOVE the tie-breaks, and
 * the tie-break it dismissed would have decided it correctly.  The blocker was
 * never the dependent count.  It is that insn 29 carries **dep 0** -- which is
 * simultaneously why it is class 3 and why it is ready at t = 0.
 *
 * ================================================================
 * WHY THE FIX IS A PREDECESSOR AND NOTHING ELSE
 * ================================================================
 * A volatile ASM makes `sched_analyze` set `reg_pending_sets_all`, so insn 29
 * gains a predecessor.  It is then not ready at t = 0, and `find_insn_list` no
 * longer returns 0, so rung 3 stops promoting it to class 3.
 *
 * AND BARRIER-FREE + SYMBOL-FREE IS STRUCTURALLY IMPOSSIBLE, with citations:
 *   (a) insn 29 is `(set (reg:SI 5) (const_int 2852))`.  The pool load is chosen
 *       at OUTPUT time by `*thumb_movsi_insn` alternative 6 (constraint `mi`),
 *       so at sched2 the insn is NOT a MEM and `sched_analyze_insn` can give it
 *       no memory predecessor.  This is the same fact the owner's ruling rests
 *       on, read in the other direction.
 *   (b) priority(29) = insn_cost + priority(first consumer) = 2 + 9 = 11, and
 *       prio(insn 32 `mov r0,r5`) = 1 + prio(insn 39) = 1 + 8, where prio(39) = 8
 *       is fixed by the ROM's OWN chain of eight calls in block 0.  So
 *       priority(29) = 11 is forced by the ROM's instruction stream, not by our
 *       spelling.
 *   (c) to reach ROM index 13, insn 29 must lose to insns 22, 24 and 26 -- the
 *       three `bl`s, priority 9.  A dep-0 insn of priority 11 cannot lose to
 *       priority 9 at rung 1 (`haifa-sched.c:4041` returns immediately).
 *   => the only two routes are a MEM predecessor (the DECLINED symbol) or a
 *      scheduling barrier.
 *
 * MEASURED THIS BATCH:
 *   base, plain literal 0xb24 ........................ 9 of 59   (one run, above)
 *   + `__asm__ volatile ("")` before `msg = 0xb24;` ... 0  *** MATCH, 12 relocs ***
 *   `static const int kMsgBase = 0xb24;` ............. SIZE 148 vs 144, still 9,
 *       RELOCATIONS differ -- cprop folds it back to a const_int AND the .rodata
 *       word is emitted, so it pays 4 bytes for nothing.  The ROM's .s has no
 *       data section (datacheck CLEAN), so this route is also wrong about the TU.
 *   Inherited and not re-run (park, 58 lines throughout): chained leading calls
 *       plus the literal 6; four separate literals 57; `register int msg
 *       __asm__("r5")` 36; -fno-schedule-insns, -fno-sched-interblock,
 *       -fno-sched-spec, -fsched-spec-load all identical to default.
 *   REJECTED, unchanged: `msg = (int)&_MSG_b20 + 4;` also reaches 1 and is a lie
 *       about the source.
 *
 * ON THE CONSTRUCT, reported as a construct and not smuggled as a device.
 * `tools/shimcount.py` gives PIN COUNT 0 and classes the empty asm as "NOT
 * treated as a fakematch in this tree".  **18 LANDED (matching) sources under
 * `src/` already ship `__asm__ volatile ("")`**, plus 6 non-matching ones --
 * including this same bank's `src/non_matching/rom_a1000/80a96d8.c`, whose park
 * header calls it "a scheduling barrier" and also reports PIN COUNT 0.  It is an
 * accepted shipped construct here.  It is nonetheless a matching artifact and
 * belongs on the pass-3/4 list, so BOTH figures are recorded: **0 with the
 * barrier, 9 without the barrier and without the symbol.**
 *
 * The file-mate heuristic (park, still load-bearing):
 * src/rom_a1000/rom_a8604_a_c_b.c gave `extern unsigned int iwram_3001f2c;`, the
 * ignored `_GetUnit` result and the `0xe4 << 1` spelling.
 */

extern unsigned int iwram_3001f2c;
extern void _GetUnit(int id);
extern void Func_80a9cbc(void);
extern void Func_80a345c(void);
extern void _Func_801e7c0(int id, unsigned int win, int x, int y);
extern void Func_80a9aec(unsigned int win, unsigned int list);
extern void WaitFrames(int n);
extern void Func_80a3e28(unsigned int list, int flag);
extern void Func_80a9c18(unsigned int list);

void Func_80a9a5c(unsigned int win, unsigned int id, unsigned int skip)
{
    unsigned int base;
    unsigned int list;
    int msg;

    base = iwram_3001f2c;
    _GetUnit(id);
    Func_80a9cbc();
    Func_80a345c();
    __asm__ volatile ("");
    msg = 0xb24;
    _Func_801e7c0(msg, win, 0, 0);
    _Func_801e7c0(msg + 1, win, 0, 0x20);
    _Func_801e7c0(msg + 2, win, 0, 0x10);
    _Func_801e7c0(msg + 3, win, 0, 0x30);
    list = base + (0xe4 << 1);
    Func_80a9aec(win, list);
    if (skip == 0) {
        WaitFrames(1);
        Func_80a3e28(list, 1);
        Func_80a9c18(list);
    }
}

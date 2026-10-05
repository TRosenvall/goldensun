/* Func_80286a0  @  0x080286a0  [rom_15000]
 * *** 1 of 85 -- AND THE ONE IS THE `_CONST_1f` RELOCATION PLACEHOLDER. ***
 * A LANDING PENDING `make compare`, WHICH IS THE ONLY THING THAT CAN ADJUDICATE
 * IT.  The park's three REAL differences are closed.  PIN-FREE by this tree's
 * own tool.  No flag group.  No device.
 *
 * FIGURE (measured, batch 328 brief E, on this body):
 *   objcmp --func : 1 of 85 (ref 85, ours 85).  SIZE exact (no SIZE line).  No
 *                   INSTRUCTION COUNT line.  Sole differing encoding index 83:
 *                       ref  0000001f      <- the assembled literal
 *                       ours 00000000 + R_ARM_ABS32 _CONST_1f
 *                   and the relocation lists differ by that one ABS32 entry and
 *                   nothing else.
 *   `const.sym:122` has `_CONST_1f = 0x1f;` already, so THE LINKED WORD IS THE
 *   ROM'S.  `const.sym:113` records this exact signature for the symbol's other
 *   user: "reports ONE difference, `=0x1f` against `=_CONST_1f`, which resolve
 *   to the [same]".  This is the established, accepted shape of that symbol.
 *   The park this replaces measured 4 of 85, re-derived first (ref 85 / ours 85,
 *   first at index 21, SIZE 188 = 188) -- figure CONFIRMED before it was beaten.
 *   PINS: shimcount.py reports `empty asm : 1  (NOT treated as a fakematch in
 *   this tree)`.  20 landed .c files already carry a bare one.
 *
 * Verify with (INSTALLED PATH, one line):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_15000/rom_23178_a_a_a_a_c_c_a_c_b.c asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c_b.s --func Func_80286a0
 *
 * SPLIT SHAPE: TWO-WAY SPLIT REQUIRED (the reference holds two functions).
 *   datacheck.py asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c.s -> no output, rc=0
 *   split_s.py  <that .s> Func_80286a0 --dry-run ->
 *       rom_23178_a_a_a_a_c_c_a_c_a.s (1 function, 150 lines)  [Func_8028574]
 *       rom_23178_a_a_a_a_c_c_a_c_b.s (1 function,  92 lines)  [Func_80286a0]
 *   EXPORTS: NONE NEEDED.  `.L373ef` is ALREADY `.global`, at
 *   asm/rom_15000/rom_23178_c_c_c_c.s:213, where it is also defined (:245).
 *   INSTALL PATH: src/rom_15000/rom_23178_a_a_a_a_c_c_a_c_b.c
 *   RETIRES the park src/non_matching/rom_15000/80286a0.c.
 *
 * ===========================================================================
 * WHAT CLOSED THE 3, AND WHY THE PARK'S REQUIREMENT WAS THE WRONG ONE
 * ===========================================================================
 * The park's reading of the rung is CONFIRMED NUMBER FOR NUMBER from my own
 * `.23.sched2` -- block 2 is `38 / 252 / 41 / 255 / 43` with dep 0/0/1/1/4,
 * prio 1/3/2/1/1, cost all 1, and `Ready list (t = 0): 38 252 -> 252` losing on
 * rank_for_schedule's FIRST rung.  Batch 327's two corrections are confirmed
 * too: the `last_scheduled_insn` class rung is dead at t=0 (schedule_block sets
 * `last_scheduled_insn = 0`, haifa-sched.c:5963), and priority 4 is unreachable
 * for a register move with one dependent.
 *
 * ** BUT THE REQUIREMENT IS NOT "A SECOND IN-BLOCK DEPENDENT". **  It is
 * ANY ZERO-INSTRUCTION SERIALISATION that puts insn 38 ahead of the `m` build,
 * and a bare `__asm__ volatile ("")` is one.  My own dump shows it does NOT
 * work through the priority rung at all:
 *     ;;  Ready list (t = 0):    38            <- 38 is ALONE on the list
 *     ;;      --> scheduling insn <<<38>>> on unit core
 *     ;;      dependences resolved: insn 40 into ready
 *     ;;  Ready list (t = 1):    40            <- the asm, "on unit none"
 *     ;;      --> scheduling insn <<<40>>>
 *     ;;  Ready list (t = 2):    254 ...
 * `sched_analyze_insn` sets `reg_pending_sets_all` for a volatile asm, so every
 * live register gains a dependence into it and everything after depends on it.
 * The block is therefore TOTALLY ORDERED, 252 is not even ready at t = 0, and
 * rank_for_schedule is never consulted.  The asm occupies `unit none` and emits
 * no bytes: SIZE and ENCODING COUNT are both unchanged at 188 / 85.
 *
 *   >> SO THE PARK'S BOUND WAS SOUND AS A READING OF THE RUNG AND WRONG AS A
 *      REQUIREMENT.  Three batches searched for a zero-cost second DEPENDENT,
 *      which is one sufficient condition.  The cheaper sufficient condition is a
 *      zero-cost BARRIER, and it does not need a dependent, a priority, or a
 *      tie-break at all.
 *
 * ---- THE ISOLATION, DONE THE WAY THE COORDINATOR ASKED --------------------
 * Sufficiency was tested with devices BEFORE looking for a clean route, because
 * the park's h1/h2/h3 crossings all read 87 instructions and so never tested the
 * hypothesis at equal length.  Measured (base 4 of 85; every row RELOC
 * `_CONST_1f`):
 *   m3  bare `__asm__ volatile ("")` after `cur = start`   1  85/85  *** THIS ***
 *   m1  DEVICE: `__asm__ ("" : : "r" (cur))`, same place  40  85/85  +4 bytes
 *   m2  the same instrument after the `m` build           41  85/85  +4 bytes
 *   k1  DEVICE: `*(volatile int *)(u+0x50) = cur;`        67  +4 bytes
 *   k2  the same store after the `m` build                66  +4 bytes
 * k1/k2 are instructive failures and the reason the figures are useless on their
 * own: `tryc --align` shows the volatile store forced `cur` OUT OF r8 into r7
 * (Thumb `str` cannot encode a high register), which swapped `cur` and `target`
 * throughout the function.  A device that perturbs the allocation is not an
 * isolation.  m1/m2 fail differently -- NAMING an operand makes the asm a real
 * reference (and a fakematch), and it then costs bytes.  **Only the bare,
 * operand-less form is both zero-byte and non-perturbing.**
 *
 * ---- THE GENERAL RESULT, WHICH IS WORTH MORE THAN THIS FUNCTION -----------
 * Three parks were recorded as wanting "a zero-instruction extra reference or
 * dependent on one allocno": this one, `Debug_WarpMenu_UI` (pseudo 38's eighth
 * reference) and `FieldMove_NoTarget`.  THE ANSWER IS SPLIT, and the split
 * matters:
 *   * A zero-instruction **ORDERING / DEPENDENCE** does exist -- the bare
 *     `__asm__ volatile ("")`, via `reg_pending_sets_all`.  It constrains the
 *     SCHEDULER.  That is what this function needed.
 *   * It supplies NO **REFERENCE** to any particular pseudo, because it names no
 *     operand -- which is exactly why shimcount.py does not count it.  So it
 *     canNOT raise a `REG_N_REFS` count, and a park whose requirement is an
 *     Nth reference on a named allocno (`Debug_WarpMenu_UI`) is NOT closed by
 *     it.  Naming the operand buys the reference and costs the fakematch, which
 *     is m1 above at 40.
 *   >> STATED WITH ITS EVIDENCE so it can be refuted: m3 at 1 of 85 with
 *      `empty asm : 1 (NOT treated as a fakematch)` is the dependence half;
 *      m1 at 40 of 85 with +4 bytes is the reference half.
 * Also confirmed from brief G's relay and not retried here: the `REG_N_REFS`
 * loop weight is not reachable at zero cost via `do { } while (0)`.
 *
 * ---- WHAT IS KEPT FROM THE PARK -------------------------------------------
 * All three of the park's landed levers reproduce and are unchanged: the
 * `goto`-into-`do/while` loop form, the explicit if/else for `j`, and
 * `step`/`extra` before `c`.  The park's OBSOLETE 25-line "DO NOT TRUST
 * objcmp's INSTRUCTION COUNT LINE HERE" section is STRUCK -- batch 326's
 * relocation-placeholder fix removed that false positive and objcmp now prints
 * no INSTRUCTION COUNT line on this function.  (tools/crossfire.py's own INSNS
 * flag was the remaining stale copy; batch 327 reports it fixed.)
 */
struct Ui {
    unsigned char pad0[0x78];
    void *box;
};

extern unsigned char *iwram_3001f38;
extern unsigned char L373ef[] __asm__(".L373ef");
extern int _CONST_1f;

extern void Func_8016478(void *w);
extern void Func_801e7c0(int id, void *w, int a, int y);
extern void WaitFrames(int n);
extern void _PlaySound(int id);

int Func_80286a0(int start, int target)
{
    struct Ui *u;
    short *c;
    short *m;
    unsigned char *t;
    int cur;
    int step;
    int extra;
    int id;
    int k;
    int d;
    int j;

    u = (struct Ui *)iwram_3001f38;
    step = 1;
    extra = 0xc;
    c = (short *)((unsigned char *)u + 0x8c);
    *c = start;
    if (target < start)
        step = -1;
    cur = start;
    __asm__ volatile ("");
    m = (short *)((unsigned char *)u + 0x92);
    goto entry;
    do {
        *c += step;
        _PlaySound(0x6f);
        extra = 0;
        cur += step;
    entry:
        Func_8016478(u->box);
        if (*m != 0) {
            id = *m + *c;
        } else {
            k = *c + 0x84;
            id = *((unsigned char *)u + k) + (int)&_CONST_1f;
        }
        Func_801e7c0(id, u->box, 0, 0);
        t = L373ef;
        d = *c - target;
        if (d >= 0)
            j = d;
        else
            j = target - *c;
        WaitFrames(t[j] + extra);
    } while (cur != target);
    WaitFrames(0x30);
    _PlaySound(0x70);
    return target;
}

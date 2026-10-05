/* Field_Move -- 2 differing encodings of 26.  PARKED, PIN-FREE, DEVICE-FREE.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/809802c.c asm/rom_8a000/rom_97b54_a_c_a_a_a_c_a_c_a.s --func Field_Move
 * (The park's old title line named rom_97b54_a_c_a_a.s, which is NOT the
 * reference.  The path in this recipe is the one that works.)
 * install, if ever landed: src/rom_8a000/rom_97b54_a_c_a_a_a_c_a_c_a.c
 *
 * RE-DERIVED batch 327, brief F: 2 differing encodings of 26 (ref 26, ours 26),
 * first at index 3; `--whole` adds no SIZE and no RELOCATIONS line, so the figure
 * IS a distance.  PINS 0.
 *
 * THE RESIDUE IS ONE ADJACENT TRANSPOSITION, and nothing else:
 *   rom   ldr r3,[r3] | sub sp,#0xc        | ldr r5,[r3,#0x10]
 *   ours  ldr r3,[r3] | ldr r5,[r3,#0x10]  | sub sp,#0xc
 * indices 3 and 4.  One run, one cause.
 *
 * ============== THE PARK'S ARITHMETIC IS REFUTED (its FIGURE stands) ==============
 * The old header, from batch 271, said `sub sp` loses "by EXACTLY ONE priority
 * unit, with a tie also losing on the LUID tiebreak".  Both halves are wrong.
 *
 * `.23.sched2` region table for bb0, `-da -fsched-verbose=6` (10 insns):
 *     insn  code  prio  cost   dependents        what
 *      77     5     35     1   24 20 16          sub sp,#0xc
 *      10   173     41     2   24 16 12          ldr r3,=iwram_3001f30
 *      12   173     39     2   24 20 16 14       ldr r3,[r3]
 *      14   173     37     2   24 22 20 19 16    ldr r5,[r3,#0x10]
 *      16   239     35    32   24 20 19          bl Func_8097384
 *      19   173     35     1   24 22 20          mov r0,r5
 *      20   240     34    32   24 22             bl Func_8098070
 *      22   173      2     1   26 24             mov r5,r0
 *      24   239      1    32   26                bl Func_8098184
 *      26   203      1     1   -                 cmp r5,#0 / beq (cbranchsi4)
 * Schedule: 10@t0, 12@t2, 14@t4, 77@t6.  77 is REQUEUED at t=1,3,5 because a
 * `load` holds the `core` unit for 2 cycles -- arm.md:262-263, the
 * `(and (eq_attr "ldsched" "!yes") (eq_attr "type" "load,store1")) 2 2` entry,
 * which is the arm7tdmi case -- so **77 can only issue on an EVEN cycle.**  At
 * t=4 the ready list is {77(35), 14(37)} and 14 wins.
 *
 * 1. THE GAP IS TWO, NOT ONE: 35 against 37.
 * 2. A TIE WOULD WIN, AND THE LUID RUNG IS NEVER REACHED.  `rank_for_schedule`
 *    (haifa-sched.c) has NO rung above priority.  On a tie it stops at the
 *    `last_scheduled_insn` CLASS rung: at t=4 last_scheduled_insn is insn 12;
 *    insn 14 is a DATA dependent of 12 with `insn_cost == 2` (!= 1) -> class 1;
 *    insn 77 is not in `INSN_DEPEND (12)` -> class 3; `return tmp2_class -
 *    tmp_class` takes class 3.  **So the actionable target is priority 37, not
 *    38, and a tie is enough.**
 *
 * ================== THE CEILING, WITH ITS EVIDENCE ==================
 * `priority(77) = max over dependents D of (insn_cost (77, link, D) +
 * priority (D))`.  77 sets `sp`, so its only possible dependents are insns that
 * reference sp: the three calls (35, 34, 1) and any frame reference.
 * `arm_adjust_cost` (config/arm/arm.c) returns 0 for REG_DEP_ANTI/OUTPUT, 1 for
 * a data dependence whose CONSUMER is a CALL_INSN, and otherwise the default --
 * **it never returns more than the default.**  The measured 35 means 77->16 is
 * anti/output (cost 0); the best a data dependence into a call could give is
 * 1 + 35 = 36.  **priority(77) <= 36 < 37 via the calls alone.**
 * Reaching 37 needs a NON-CALL dependent of priority >= 36, i.e. a real frame
 * reference high in the chain -- a 27th instruction.
 * Equally, delaying 14 past t=4 needs `cost(12->14) >= 3`; the producer is a
 * `load` whose core-unit entry is `2 2` and arm_adjust_cost can only reduce it.
 *
 * PROVED BY PROBE, not argued.  INSTRUMENT (labelled; 27 instructions, so its
 * number is a figure about the blocker): `*(int *)buf = 0;` before the load
 * reads 23 at dsize +4, and its `.23.sched2` says
 *     85 (sub sp)  prio **42**  dependents 32 28 24 **15**
 * where 15 is the `str r3,[sp]`, priority 41: `priority = 1 + 41 = 42`, and
 * `sub sp` issues FIRST.  A non-call dependent is exactly what raises it, and a
 * non-call dependent of an `sp` write is a frame reference.
 *
 * MEASURED batch 327, 13 variants, all 26 insns / 60 bytes / relocations clean /
 * first=3 -- the frame object's TYPE, SIZE SPELLING, SCOPE and DECLARATION ORDER
 * are all irrelevant, as predicted, because none of them references the frame:
 *   INERT at 2: `int buf[3]`; `short buf[6]`; `struct {int x,y,z;}`;
 *     `union {char c[12]; int i[3];}`; `volatile char buf[12]`; three separate
 *     `char b[4]`s; `struct {double d; int i;}`; buf declared after a/r;
 *     r declared before a; `char *bp = buf;` (address taken into an unused
 *     local -- gcc emits nothing); a and r merged into ONE variable; the global
 *     retyped `char **` with `iwram_3001f30[4]`.
 *   WORSE: `Func_8097384()` moved before the load -- 24, dsize -4 (one insn
 *     short), RELOCDIFF.  The ROM's load-before-call order is forced.
 *
 * ALSO STILL TRUE, from batch 321: the alias-set-0 union lever that closed
 * Func_8095938 and Func_8095fcc cannot reach here, for the same reason as the
 * ceiling above -- the twelve bytes are never accessed, and an instruction with
 * no memory successors cannot be given any by changing an alias set.
 * tools/dupfuncs.py: neither Field_Move nor Field_Halt is in any of the 8
 * duplicate groups; what transfers between the rom_8a000 field functions is the
 * LEVER, not the body.
 *
 * DO NOT re-sweep the frame object's spelling (13 rows above).  The park's old
 * "DO NOT SWEEP THIS AGAIN" was right about spellings and wrong about the
 * arithmetic.
 * -- scratch_elev/b327/F
 */
extern char *iwram_3001f30;
extern void Func_8097384(void);
extern void *Func_8098070(void *a);
extern void Func_8098184(void);
extern void _Actor_SetAnim(void *a, int n);
extern void WaitFrames(int n);
extern void Func_809748c(void);
extern void Func_80981b0(void *a);

void Field_Move(void)
{
    char buf[12];
    void *a;
    void *r;

    a = *(void **)(iwram_3001f30 + 0x10);
    Func_8097384();
    r = Func_8098070(a);
    Func_8098184();
    if (r != 0) {
        _Actor_SetAnim(r, 4);
        WaitFrames(0x1e);
    }
    Func_809748c();
    Func_80981b0(r);
}


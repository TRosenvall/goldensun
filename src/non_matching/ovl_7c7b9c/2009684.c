/* OvlFunc_943_2009684 -- NON-MATCHING, 3 differing STREAM encodings of 111.
 *
 * ===== BATCH 340, BRIEF A: 16 -> 3, DEVICE-FREE, PIN-FREE, NO FLAG =====
 *
 * FIGURE RE-DERIVED AT THE HEAD OF THE BATCH, and the brief's table was
 * incomplete: the park body is 16 differing STREAM encodings of 111
 * (ref 111 / ours 111), 80 / 80 16-bit instructions -- in phase -- BUT objcmp
 * ALSO reports RELOCATIONS DIFFER, which TARGETS.md did not say and autosweep
 * flags with `!!`.  First differing index 28.
 *
 * NEW FIGURE: 3 differing encodings of 111, 80 / 80 instructions, first
 * differing index 96, and THE RELOCATION FLAG IS GONE -- objcmp now prints only
 *     ~~ relocation _umodsi3_RAM / __umodsi3 is ONE symbol (same address in the
 *        linked ELF)
 * twice, which is objcmp's own benign note.  PINS: 0 (tools/shimcount.py).
 *
 * >>> AND THAT IS A CORRECTION TO HOW THE PARK'S RELOCATION DIFF WAS READ.  The
 * >>> park prose says the `bl __umodsi3` / `bl _umodsi3_RAM` pair "is the linker
 * >>> alias ... so 15 real", and THAT HALF IS RIGHT -- `__umodsi3 = _umodsi3_RAM;`
 * >>> is at overlays/rom_7c7b9c/overlay.ld:90, verified.  But the `RELOCATIONS
 * >>> differ` line on the park figure was NOT the symbol names: objcmp already
 * >>> knows the alias.  It was the OFFSETS -- the ROM has `__Random` at 0x8e and
 * >>> the helper at 0x94 where the park body had 0x90 and 0x96, a two-byte local
 * >>> phase shift inside that window.  Closing the schedule closed the relocation
 * >>> table with it.  A relocation diff can be a SCHEDULE defect, not a symbol one.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7c7b9c/2009684.c \
 *     asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_a_c.s --func OvlFunc_943_2009684
 *
 * SPLIT SHAPE ON A LANDING: none.  tools/piece.py says the reference .s holds
 * exactly ONE function, so this converts the whole TU; install path would be
 * src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_a_c.c.  datacheck is
 * silent (no data).
 *
 * ================ THE THREE THINGS THAT MOVED, 16 -> 3 ================
 *
 * (1) THE PARK'S STATED BLOCKER NO LONGER DESCRIBED ITS OWN BODY -- 6 encodings.
 * The park says "ours mov r5,#0x80 / lsl r5,#8 / bl GetActor", i.e. the value
 * built BEFORE the call.  The recovered body does not do that: the mov/lsl are
 * already AFTER the `bl __MapActor_GetActor`, in the ROM's position, and only the
 * REGISTER was wrong (r5 where the ROM has r3), twice.  The park's reasoning was
 * still the right one -- the pseudo is allocated before sched2 runs, so it
 * crossed the call at allocation time and took a callee-saved register -- and its
 * conclusion ("naming the POINTER moves the problem, then the pointer takes r5")
 * was true only because `q`, the pointer local it reused, is ALSO the pointer for
 * the two random stores and so spans three calls.  A FRESH POINTER LOCAL PER
 * STORE SITE (`p1`, `p2`) fixes both sites: the pointer is dead by the next
 * statement, nothing crosses the call, and the value lands in scratch r3.
 *
 * (2) THE SCRIPT POINTER IS LOADED TOO EARLY -- 5 encodings.  The ROM has
 * `ldr r6, =gScript_943__0200c4d8` AFTER `bl _umodsi3_RAM`; we hoisted it above
 * `mov r5, r0`.  Moving `s = gScript_943__0200c4d8;` BELOW the random store puts
 * it where the ROM has it.  (Deleting `s` and naming the array at both
 * `__MapActor_SetBehavior` call sites is worth the same 5; the local is kept
 * because the ROM holds the pointer in r6 across both uses.)
 *
 * (3) THE TWO ADDS AT THE FIRST RANDOM STORE -- 2 encodings, and this is the one
 * with a mechanism worth reusing.
 *     rom   bl helper / ldr r6,=gScript / add r0,#0x3c / add r5,#0x64 / strh r0,[r5]
 *     ours  bl helper / ldr r6,=gScript / add r5,#0x64 / add r0,#0x3c / strh r0,[r5]
 * and at the SECOND random store the ROM has `add r5,#0x64` FIRST -- the opposite
 * order from the same source shape.  Both are the same sched2 decision under
 * different readiness.  `rank_for_schedule` (haifa-sched.c:4040-4115) runs out of
 * discriminators here: INSN_PRIORITY ties because `arm_adjust_cost`
 * (arm.c, the `GET_CODE (insn) == CALL_INSN` arm) returns 1 for every data
 * dependence into a call, so both adds price the same path; INSN_REG_WEIGHT is
 * skipped once reload_completed; both insns are in the same bb; both are
 * independent of the last-scheduled insn so both are class 3; and both have
 * exactly one dependent (the `strh`).  THE TIE FALLS TO INSN_LUID, lower first
 * (haifa-sched.c:4113) -- so the RTL ORDER OF THE TWO ADDS IS THE WHOLE RESIDUE.
 * `expand_assign` expands the destination address before the source, so
 * `*(short *)(q + 0x64) = <value>` always emits `add r5,#0x64` first.  Writing it
 *     rv = __Random() % 0x5a + 0x3c;   q += 0x64;   *(short *)q = rv;
 * puts the value's add first.  At the SECOND store there is no `ldr r6` to fill
 * the post-call slot, so `add r5,#0x64` is the only ready insn at that cycle and
 * goes first regardless of LUID -- which is exactly why one source shape produces
 * two different orders.
 *
 * >>> AND WHY SITE 2 KEEPS THE ORIGINAL SPELLING.  Applying the value-first
 * >>> spelling at BOTH sites gets BOTH schedules right and then LOSES THE
 * >>> REGISTERS: `q` takes r6 and `s` takes r5, the ROM's pair swapped, and the
 * >>> figure is 12.  (.17.lreg: q 8 refs / 22 insns, s 3 refs / 48.)  That is the
 * >>> callee-saved coin flip this brief landed twice elsewhere, arriving here as a
 * >>> side effect, and it is the open question on this park -- see below.
 *
 * ================ WHAT IS LEFT: 3 ENCODINGS, ONE sched2 TIE ================
 *
 *     rom   mov r1,#0xf6 / mov r2,#0x80 / mov r0,#0x14 / lsl r1,#0x10 / lsl r2,#0x12
 *     ours  mov r1,#0xf6 / mov r2,#0x80 / lsl r1,#0x10 / lsl r2,#0x12 / mov r0,#0x14
 *
 * in the `__GetFlag(0x923)` arm.  Same rung as (3) and the arithmetic is written
 * out: in `.19.flow2` the block is insn 344 (mov r1), 345 (lsl r1), 346 (mov r2),
 * 347 (lsl r2), 294 (mov r0,#0x14), 299 (call) -- the arg-0 move is LAST in the
 * chain because `precompute_register_parameters` (calls.c:805-858) copies an
 * argument into a pseudo only when `rtx_cost (value, SET) > 2`, which the two
 * shifted constants satisfy and `const_int 20` does not, so args 1 and 2 are
 * materialised before arg 0 is stored.  Scheduling it by hand reproduces our
 * output exactly: cycle 0 mov r1 (priority 2), cycle 1 mov r2 (priority 2), then
 * at cycle 2 lsl r1 / lsl r2 / mov r0 are all priority 1, all class 3, all with
 * one dependent, so LUID orders them 345, 347, 294.  To reach the ROM the arg-0
 * move needs a LOWER LUID, i.e. it must be emitted before the two constant
 * syntheses, and nothing in the source reached that.
 *
 * MEASURED INERT AT 3, all on this body (eight spellings of that arm):
 *   `int x = 0xf6 << 16; int y = 0x80 << 18;` then `SetPos(0x14, x, y)`;
 *   either one of the two named alone (`y` alone is 4, WORSE);
 *   `int slot = 0x14;` used for both the SetPos and the GetActor;
 *   a named pointer for the `strh` target;
 *   `0xf6 * 0x10000, 0x80 * 0x40000` instead of the shifts;
 *   `do { } while (0);` before the SetPos.
 *   (Reusing the function's existing `v`/`rv` int locals for the two shifted
 *   arguments is 81 instructions of 80 -- a LENGTH defect, not a figure.)
 *
 * ANCHOR AXIS RE-RUN ON THIS BODY, not inherited: 30 placements, best 3, base 3,
 * all inert or worse (`scratch_elev/b340/A/b340A_sweep.py` reuses autosweep's own
 * boundary generator).  Re-running it mattered in principle -- the park's anchor
 * negatives were recorded against a body whose residue was 16 -- and it paid
 * nothing.
 * FLAG AXIS RE-CROSSED ON THIS BODY: -fno-gcse, -fno-strict-aliasing,
 * -fno-strength-reduce, -fno-rerun-cse-after-loop, -fno-rerun-loop-opt and
 * -ffixed-r7 ALL INERT AT 3; -fno-schedule-insns2 22 (worse).  On the park body
 * every one of those was inert at 16, so the flag axis is closed on both.
 *
 * ================ THE OPEN QUESTION, STATED WITH ITS EVIDENCE ================
 *
 * The symmetric body (value-first at BOTH random stores) has the ROM's schedule
 * everywhere and swaps r5/r6 between `q` and `s`.  The lever that landed
 * OvlFunc_946_2009c84 and OvlFunc_946_200a16c this batch -- split a long-lived
 * pseudo's last in-place definition into its own local so it LEAVES the
 * allocno race (global.c:598-620) -- is the thing to try on it, and I did not get
 * to it.  `.18.greg` on the symmetric body reads `34 in 6  35 in 5`, with
 * pseudo 34 = `q` (8 refs / 22) and 35 = `s` (3 refs / 48); on this body it reads
 * `34 in 5  35 in 6`.  Note that pseudo 34's priority is the HIGHER of the two in
 * BOTH bodies, so whatever flips them is not the priority sort alone and the
 * conflict/preference side of find_reg needs reading before the next probe.
 *
 * The park's two transferable findings are both still correct and both still
 * load-bearing in this body: the division helper names the signedness
 * (`extern unsigned int __Random(void);` -> `__umodsi3`, not `__modsi3`), and the
 * halfword stores need an int intermediate rather than an `unsigned short`
 * destination.  The original prose park is preserved below.
 */
extern unsigned char gScript_943__0200c4d8[];

extern int __GetFlag(int id);
extern unsigned int __Random(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __Func_8092b08(int a, int b);

void OvlFunc_943_2009684(void)
{
    unsigned char *p1;
    unsigned char *p2;
    unsigned char *q;
    unsigned char *s;
    int f;
    int v;
    int rv;

    __Func_8092b08(0x1b, 1);
    __Func_8092b08(0x17, 1);
    __Func_8092b08(0x16, 1);
    __Func_8092b08(0x1a, 1);
    __Func_8092b08(0x18, 1);
    if (__GetFlag(0x92 << 4)) {
        __MapActor_SetPos(0x16, 0xa2 << 16, 0x29a0000);
        p1 = __MapActor_GetActor(0x16);
        v = 0x80 << 8;
        *(unsigned short *)(p1 + 6) = v;
        __MapActor_SetPos(0x17, 0, 0);
        __MapActor_SetPos(0x14, 0, 0);
    }
    f = __GetFlag(0x922);
    if (f != 0) {
        __MapActor_SetPos(0x15, 0x84 << 17, 0x2be0000);
        p2 = __MapActor_GetActor(0x15);
        v = 0xa0 << 7;
        *(unsigned short *)(p2 + 6) = v;
        q = __MapActor_GetActor(0x15);
        rv = __Random() % 0x5a + 0x3c;
        q += 0x64;
        *(short *)q = rv;
        s = gScript_943__0200c4d8;
        __MapActor_SetBehavior(0x15, s);
        __MapActor_SetPos(0x18, 0xf8 << 16, 0xaa << 18);
        q = __MapActor_GetActor(0x18);
        *(short *)(q + 0x64) = __Random() % 0x5a + 0x3c;
        __MapActor_SetBehavior(0x18, s);
        __MapActor_SetPos(0x16, 0, 0);
    } else if (__GetFlag(0x923)) {
        __MapActor_SetPos(0x14, 0xf6 << 16, 0x80 << 18);
        *(short *)(__MapActor_GetActor(0x14) + 6) = f;
    }
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_943_2009684 -- 0x02009684, asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a.s
 *
 * 109 of 109 lines, 17 differing -- of which TWO are not real: `bl __umodsi3`
 * against the ROM's `bl _umodsi3_RAM` is the linker alias this overlay already
 * carries (`__umodsi3 = _umodsi3_RAM;` in overlay.ld, line 71).  So 15 real.
 * Candidate: scratch/Y9684_F.c.
 *
 * TWO FINDINGS WORTH REUSING, both of which moved the count a long way:
 *
 *   1. THE DIVISION HELPER NAMES THE SIGNEDNESS.  The first transcription used
 *      `__Random() % 0x5a` with __Random declared `int`, which emits __modsi3.
 *      The ROM calls the UNSIGNED helper.  Declaring `extern unsigned int
 *      __Random(void);` took the screen from 55 differing to 17 in one edit.
 *      Read which of divsi3/udivsi3/modsi3/umodsi3 the ROM calls and let that
 *      decide the operand types -- it is a free signedness oracle and it is
 *      cheaper than reasoning about the values.
 *
 *   2. THE HALFWORD STORES NEED AN INT INTERMEDIATE, not an unsigned short
 *      destination.  `*(unsigned short *)p = 0x80 << 8` POOLS the constant
 *      (`ldr r3, =0x8000`) where the ROM builds `mov r3,#0x80 / lsl r3,#8`;
 *      routing it through an int local builds it.  That is the opposite of what
 *      the HImode note in docs/elevation.md predicts for a sign-extending value,
 *      and it is a second case where that rule's two remedies are not
 *      interchangeable.
 *
 * BLOCKER: the int intermediate wins the build and loses the register.
 *      rom   bl GetActor / mov r3,#0x80 / lsl r3,#8 / strh r3,[r0,#6]
 *      ours  mov r5,#0x80 / lsl r5,#8 / bl GetActor / strh r5,[r0,#6]
 * The ROM computes the value AFTER the call, so it lives in scratch r3.  A named
 * int is assigned before the call, so it must survive it and gcc gives it
 * callee-saved r5.  Naming the POINTER instead moves the problem rather than
 * solving it -- then the pointer takes r5 and the store becomes `strh r3,[r5,#6]`.
 * Something has to cross the call, and the ROM has nothing crossing it.
 *
 * TRIED: unsigned short destination with the shift inline (64, pooled); signed
 * short the same (64); an `unsigned short *` pointer local (64); one shared int
 * local and two separate ones (17 both).
 *
 * ALSO LEFT: the argument-setup interleave at the last __MapActor_SetPos, and
 * the placement of the gScript pool load relative to `mov r5, r0`.  Those are
 * downstream of the register question and are not worth attacking until it moves.
 *)

*/
#endif

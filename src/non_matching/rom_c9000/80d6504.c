/* Task_SpinCamera -- 0x080d6504, 37 ROM instructions (41 encodings).
 *
 * NON-MATCHING, 18 differing encodings of 41.   [batch 325H: was 23]
 *
 * MEASUREMENT -- THIS COUNT IS A TRUE DISTANCE.  SIZE IS EXACT (88 bytes both
 * sides, objcmp prints no SIZE line), the instruction COUNT IS EXACT (41 / 41)
 * and relocations are identical (objcmp prints no RELOCATIONS line), so the 18
 * ranks directly.  First differing index 0.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/80d6504.c asm/rom_c9000/rom_d6504_a_a.s --func Task_SpinCamera
 *
 * SHIMS: NONE.  PIN-FREE, no register pin, no barrier, no per-file flag
 * override, no fakematch.txt row.  The figure above is a PRODUCTION-FLAG
 * figure.
 *
 * SPLIT SHAPE: none needed.  asm/rom_c9000/rom_d6504_a_a.s holds exactly ONE
 * function (`grep -c func_start` = 1), so elevating this is a file rename with
 * no split and no exports.  That also means a per-file flag group for this
 * object would affect THIS FUNCTION ONLY -- there is no sibling in the object
 * to contradict the assertion, which is the counter-evidence that sank the
 * `-ffixed-r11` request in docs/owner-decisions.md #2.
 *
 * ================================================================
 * BATCH 325H -- THE BLOCKER IS ALIAS SETS, AND THE PARK'S TWO VERDICTS WERE
 * BOTH WRONG
 * ================================================================
 *
 * THE PARK SAID: "Blocker class: REGISTER CHOICE, plus one extra callee-saved
 * register ... the ROM overwrites the state pointer with the amount it loads
 * from it, so state is dead from that point; in C the two have different types
 * and cannot share a variable, so the source cannot express the reuse."
 * REFUTED.  Nothing here needs two types in one variable.  The extra
 * callee-saved register, the r3-vs-r4 rotation and the branchless mode update
 * are ONE cause with ONE name.
 *
 * > THE ROM RE-READS THE `int` MODE WORD AT `st+0x77b0` AFTER STORING A
 * > HALFWORD THROUGH `view+0x36`, AND WE DO NOT.  That is the recogniser
 * > docs/elevation.md already carries verbatim ("A MISSING RELOAD after a store
 * > of a different width is an ALIASING tell"), with `short`-against-`int`
 * > named as the paying case and two functions closed on it (`Func_808d828`
 * > 68 -> 7, `Func_80935d4` 54 -> 4).
 *
 * `true_dependence` (alias.c:1573) returns 0 at `DIFFERENT_ALIAS_SETS_P`
 * BEFORE it ever looks at the addresses, so under `-fstrict-aliasing` (on at
 * -O2) the halfword store cannot invalidate the SImode load and cse commons the
 * two reads.  `get_alias_set` (alias.c:351) returns 0 for everything when the
 * flag is off, which is why the flag moves it.  `lang_get_alias_set`
 * (c-common.c:3328) hands back 0 in exactly two reachable cases -- a
 * COMPONENT_REF taken DIRECTLY through a UNION_TYPE (:3344), and any reference
 * of char precision (:3348) -- and an `int`-width field can be neither, so
 * THERE IS NO SOURCE SPELLING THAT REACHES ALIAS SET 0 HERE.  Both exclusions
 * recorded for the recogniser are clear: there is no CALL between the store and
 * the re-read, and the store is `short`, not a character type.
 *
 * MEASURED, as a figure ABOUT THE BLOCKER and not a result:
 *
 *     this body                       18 pin-free   /  12 with ALIAS_CFLAGS
 *     the `best10` variant below      23 pin-free   /  10 with ALIAS_CFLAGS
 *
 * `Makefile:235` already defines `ALIAS_CFLAGS := $(GCC296_CFLAGS)
 * -fno-strict-aliasing` and TWENTY OBJECTS use it; `tools/tryc.py:183` maps it.
 * This is an owner-facing call, not an agent one, so the body shipped here is
 * the one with the best PRODUCTION-FLAG figure.
 *
 * ================================================================
 * THE PARK'S SECOND VERDICT, ALSO REFUTED: THE TEMPORARY IS THE DEFECT
 * ================================================================
 *
 * THE PARK SAID: "the mode assignment written without a temporary,
 * `if (...) *mode = 2; else *mode = 0;`: THREE lines short instead of two, so
 * the temporary is load-bearing and is kept."
 *
 * With the temporary, gcc-2.96's ifcvt (`.14.ce` / `.20.ce2`) if-converts
 * `v = (*mode != 2) ? 2 : 0` into SIX arithmetic instructions --
 * `movs r2,#2 / eors r2,r4 / negs r3,r2 / orrs r3,r2 / lsrs r3,#31 /
 * lsls r3,#1` -- where the ROM branches.  `noce_*` only handles a REGISTER
 * destination, so writing the two stores out (`*mode = 0;` / `*mode = 2;`)
 * cannot be if-converted at all; cross-jumping then merges the two stores onto
 * one shared `str`, which is EXACTLY the reference's idx 31-34
 * (`movs r3,#0 / b / movs r3,#2 / str r3,[r0]`).
 * The park measured the no-temp form WITHOUT the alias prerequisite and read
 * its 2-short length as a refutation.  Another recorded "worse" that was a
 * MISSING PREREQUISITE.
 *
 * ================================================================
 * THE FOUR LEVERS, AND WHY ONE-AT-A-TIME SCREENING FINDS NONE OF THEM
 * ================================================================
 *
 *   (1) the two-store inner update, not a temp          -- blocks ifcvt
 *   (2) `*mode = 0;` BEFORE the y update in the outer branch
 *       -- without it cross-jumping merges ALL THREE `str rX,[r0]` onto one
 *          shared store and the body comes out TWO INSTRUCTIONS SHORT (34)
 *   (3) `amt` declared BLOCK-SCOPED in each branch, not one function-scope
 *       `int amt;` -- worth 3 encodings on its own once (1) and (2) are in, and
 *       it is what puts the signed halving in the ROM's registers and order
 *       (`lsr #31 / add / asr #1` interleaved with the `ldrh`)
 *   (4) `view` initialised BEFORE `mode` -- the reference loads the 0x77b0 pool
 *       constant into r3, REUSING the register the symbol address died in, so
 *       the constant's pseudo must be born after that death.  With `mode`
 *       first, the constant must take r1 and the whole head mis-schedules.
 *
 * Lever (4) is the one that TRADES: it fixes the head completely but exchanges
 * r0/r1 between `view` and `mode`.  Read off `.18.greg`, which is the right
 * instrument here and not the figure:
 *
 *     view-first  `;; 3 regs to allocate: 32 34 33`  -> mode r1, view r0  WRONG
 *     mode-first  `;; 3 regs to allocate: 32 33 34`  -> mode r0, view r1  RIGHT
 *
 *   (33 is `view`, 34 is `mode`, 32 is `st`; all three conflict with hard r2/r3
 *   so the only question is which takes r1 and which r0, and REG_ALLOC_ORDER
 *   is `{3, 2, 1, 0, ...}` (arm.h:989) so the EARLIER-allocated allocno gets
 *   **r1**.)  Initialising `mode` second shortens its live range, raises its
 *   `allocno_compare` priority above `view`'s and swaps them.
 *
 * ================================================================
 * BATCH 325H -- THE .18.greg RELOAD TRIAGE, FOR THE RECORD
 * ================================================================
 * This park does NOT cite `REG_ALLOC_ORDER` and nothing above rests on it, but
 * the triage is cheap to record and it CONFIRMS the reading above.
 * `Using reg N for reload M` in `.18.greg` is `find_reg`'s decision
 * (reload1.c:1664, inside find_reg at :1588); find_reg reads `REG_ALLOC_ORDER`
 * explicitly (:1645-1662) with `inv_reg_alloc_order` breaking ties among equal
 * `spill_cost`; and `choose_reload_regs_init` (:5129) leaves ONE BIT and
 * therefore NO freedom when an insn carries one reload.  So: one `Using reg`
 * per `Spilling for insn` block means REG_ALLOC_ORDER is blamed CORRECTLY and
 * the actionable quantity is `spill_cost`, i.e. the LIVE SET at that insn; two
 * or more reloads on one insn, or inheritance, is where a round-robin cursor
 * reading applies; and NO `Using reg` at all means there is no reload register
 * and it is an ALLOCNO question.
 *
 * This function: 16 `Spilling for insn` blocks -- 10 with no reload, 6 with
 * exactly one, NONE with two, and ZERO `Reusing reg` lines.  The registers in
 * the residue above (`view`/`mode` over r0/r1, the outer zero over r2/r3) have
 * NO `Using reg` at all, so they are an allocno question -- which is where this
 * header already works them, off `;; 3 regs to allocate:`.  The cursor reading
 * has no purchase here and neither does REG_ALLOC_ORDER.
 *
 * ================================================================
 * WHAT THE REMAINING 18 IS -- TWO CAUSES, AND BOTH ARE REGISTER CHOICE
 * ================================================================
 *
 *   A. `view` and `mode` over r0/r1, as above.  Everything that reads or writes
 *      either pointer differs by that one swap and nothing else.
 *   B. the outer `*mode = 0;`: the ROM materialises `movs r2,#0` between the
 *      `adds` and the `strh` and stores `str r2,[r0]` AFTER the `strh`; we emit
 *      `movs r3,#0 / str r3,[r0]` before the `ldrh`.  The two halves are in
 *      DIFFERENT LISTS -- the POSITION wants the statement after the y update,
 *      the cross-jump wants it before -- and what separates them in the
 *      reference is that the outer zero lives in **r2** while the inner shared
 *      store uses r3.  Give the outer zero r2 and the statement can go back to
 *      its natural place; that is the whole residue.
 *
 * The signed halving (ref idx 22-27) is EXACT in this body and needed no work.
 *
 * MEASURED INERT (all at this body's figure, with and without the flag):
 *   - declaration order of `view` / `mode` -- free either way
 *   - dropping the `st` local and reading the global twice
 *   - `amt /= 2` against `amt = amt / 2`
 *   - `view` as `unsigned short *` with `view[0x1b]`
 *   - `amt + y` against `y + amt`, and the `+=` form
 *   - `0x77b0` through a named `int off` local
 * MEASURED WORSE:
 *   - `*mode = 0;` after the y update            34, and 2 instructions SHORT
 *   - `amt = *(int *)(st + 0x77ac) / 2;` in one statement     +2
 *   - no `mode` variable, the expression written out at all four uses   +2
 *   - `mode = (int *)st;` then re-assigned after `view`       +24
 *   - `view` initialised before `st`                          +30
 *
 * THE LEVER THE OLD PARK FOUND IS KEPT AND IS STILL RIGHT: the second global is
 * reached as an offset FROM THE FIRST SYMBOL'S ADDRESS, which is why the head
 * is `ldr r3,=iwram_3001eec / ldr r2,[r3] / sub r3,#0x6c / ldr r1,[r3]` and not
 * two pool loads.
 *
 * ALTERNATIVE BODY, 10 with ALIAS_CFLAGS and 23 without -- the same body with
 * the two initialisers swapped:
 *     st   = iwram_3001eec;
 *     mode = (int *)(st + 0x77b0);
 *     view = *(char **)((char *)&iwram_3001eec - 0x6c);
 * Its residue is cause B above plus the head schedule, and NOT cause A.
 */
extern char *iwram_3001eec;

void Task_SpinCamera(void)
{
    char *st;
    char *view;
    int *mode;

    st = iwram_3001eec;
    view = *(char **)((char *)&iwram_3001eec - 0x6c);
    mode = (int *)(st + 0x77b0);
    if (*mode == 1) {
        int amt = *(int *)(st + 0x77ac);
        *mode = 0;
        *(unsigned short *)(view + 0x36) = *(unsigned short *)(view + 0x36) + amt;
    } else {
        int amt = *(int *)(st + 0x77ac);
        amt = amt / 2;
        *(unsigned short *)(view + 0x36) = *(unsigned short *)(view + 0x36) + amt;
        if (*mode == 2)
            *mode = 0;
        else
            *mode = 2;
    }
}

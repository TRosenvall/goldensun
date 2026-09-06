/* OvlFunc_924_20095e0  --  0x020095e0
 *
 * VERDICT: MATCH.
 *   OK OvlFunc_924_20095e0 -- 228 bytes, 104 encodings and 9 relocations identical
 * Confirmed twice under the current objcmp: against the extracted single-function
 * scratch_elev/b239/f20095e0/ref.s AND against the ORIGINAL two-function path
 * asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c.s, same line both times.
 *
 * ---------------------------------------------------------------- provenance
 *
 * b236 (scratch_elev/b236/f20095e0/) had already reached this body and screened
 * ~40 variants around it with tryc. Batch 236 shipped five other functions and
 * this one was never landed -- no file for it exists anywhere under src/, and
 * no report and no docs/elevation.md line mentions `OvlFunc_924_20095e0`. So
 * the whole b236 write-up, including its NEW finding, is still uncashed and is carried below.
 * The BODY of this candidate is the b236 body, verbatim; what is added here
 * is authority (objcmp, not tryc), a minimality proof, and a bound that
 * corrects the mechanism b236 stated for its own lever.
 *
 * The `park_b.c` / `pb1-4.c` files in that directory are the SIBLING, NOT this
 * target: they are drafts of `OvlFunc_924_20096c4`, the second function in the
 * same .s, which is parked at src/non_matching/ovl_7ac2d8/20096c4.c. `ref_b.s`
 * is its reference. Re-screened under the current objcmp (see below) -- it is still
 * blocked and its park still reads correctly.
 *
 * tools/solved_twins.py: "solved shapes 2108, remaining functions 1517 ...
 * REMAINING FUNCTIONS WITH A SOLVED TWIN: 0 across 0 templates". No twin.
 *
 * the templated.py 1.00 on five shared symbols was worth what its own note says:
 * the neighbour src/overlays/rom_78dee8/ovl_30_c_c_a_c_a.c closes its crossed
 * fills with three-register `register ... __asm__` pins in the MOV order of the ROM,
 * that spelling DOES match here (v2.c in b236), and it is pure scaffolding --
 * the same defect closes with two plain `int`s. Sufficient, not necessary.
 *
 * ------------------------------------------------------------------- levers
 *
 * THREE, all re-verified load-bearing under objcmp (table below).
 *
 * 1/2. ONE NAMED LOCAL PER `__Func_8012330` CALL, `c0` and `n0`. Both sites are
 *      the argument-list form of constant CSE, and the text of the ROM itself says the
 *      defect is the DIRECTION of a copy the ROM also makes, not its existence:
 *
 *          rom   mov r0,#0xa0 / lsl r0,#0xb / mov r2,#0x80 / mov r1,r0 / lsl r2,#9
 *          ours  mov r1,#0xa0 / lsl r1,#0xb / mov r2,#0x80 / mov r0,r1 / lsl r2,#9
 *
 *          rom   mov r0,#1 / mov r1,#1 / neg r0,r0 / neg r1,r1 / ldr r2,=0xe666
 *          ours  mov r1,#1 / neg r1,r1 / ldr r2,=0xe666 / mov r0,r1
 *
 *      MECHANISM: REG_ALLOC_ORDER (arm.h:989) starts {3,2,1,0}, so the pseudo
 *      that holds the commoned value outranks the argument register it has to
 *      end up in and the copy runs r1->r0 instead of r0->r1. Giving the value a
 *      name in a block OTHER than the one the two uses sit in gives it a pseudo
 *      of its own that survives into the call block; it takes r0 and the copy
 *      turns round.
 *
 * 3.   THE POST-LOOP STACK-ARGUMENT PAIR, `s0`/`s1`. The ROM materialises both
 *      words before storing either (`mov r3,#0x2a / mov r2,#0x21 / str r3,[sp]
 *      / str r2,[sp,#4]`); literals reuse one register across the two stores.
 *      Standard stack-arg-pair lever. It works HERE because the site is OUTSIDE
 *      the loop -- inside the loop it backfires, which is the register-pressure
 *      proviso the sibling park at src/non_matching/ovl_7ac2d8/20096c4.c
 *      already wrote down and which this file confirms from the other side.
 *
 * ------------------------------------------------- NOT INERT: minimality proof
 *
 * Every element of the shipped body was removed one at a time and re-measured
 * with objcmp. There is no `register ... __asm__` anywhere and nothing spare.
 *
 *   variant            file            objcmp
 *   ---------------------------------------------------------------------
 *   SHIPPED            cand.c          OK, 228 bytes, 104 enc, 9 reloc
 *   drop `n0`          m1_no_n0.c      31 encodings + RELOCATIONS differ
 *   drop `c0`          m2_no_c0.c      3 encodings, first at index 68:
 *                                        ref 20a0 ours 21a0   (mov r0/r1 #0xa0)
 *   drop both          m3_none.c       RELOCATIONS differ (= naive)
 *   drop `s0`/`s1`     m4_no_stack.c   3 encodings, first at index 86:
 *                                        ref 2221 ours 9300   (mov r2,#0x21
 *                                        replaced by str r3,[sp,#0])
 *   `i * 2` for `i<<1` m5_mul.c        OK -- free, cosmetic only
 *
 * READ THE TEXT OF THE LENGTH TELL, and note which of the three outcomes this is.
 * `m1_no_n0` prints NO `XX SIZE` line -- the object is 228 bytes either way and
 * the encoding COUNT is 104 either way -- yet the last four call relocations sit
 * two bytes earlier than the reference. The body IS one instruction shorter (the
 * `mov`/`neg` pair collapses to one `neg` plus a copy) and the pool word for
 * 0xe666 realigns to absorb it. That is "POOL-ALIGNMENT PADDING ABSORBS A SMALL
 * ODD LENGTH CHANGE" (docs/elevation.md:16574) doing exactly what it says; a
 * reader who checks only the size and the encoding count sees two equal numbers
 * and concludes the length is right. It is not.
 *
 * ------------------------------------------------------------- the b236 finding
 *
 * Still NEW -- grepped by concept ("one live range the loop does not own", "ONE
 * named local is enough", "commoned PAIR", "the slot does not matter"): nothing
 * in docs/elevation.md, because batch 236 never shipped this function.
 *
 *   ONE named local is enough for a commoned PAIR, and the argument slot does
 *   not matter. The tree records the remedy as "giving EACH occurrence its own
 *   named local in a dominating block" (docs/elevation.md:1714, and the HImode
 *   section as "each site needs its own local", :3506). Here each site has TWO
 *   occurrences of one value and ONE local closes it; moving that local from
 *   argument slot 0 to slot 1 at BOTH sites is still exact (n1_slot1.c, OK).
 *   So the lever is not "rematerialise every copy" -- it is "give the value one
 *   pseudo the call block does not own", and one local does that for the pair.
 *
 *   Second half: this ALSO fixes a copy THE ROM ITSELF KEEPS. Site 1 is not a
 *   rematerialisation case -- the ROM commons its two `0xa0 << 11` arguments
 *   exactly as gcc does, `mov r1, r0`, and only the direction was wrong. The
 *   copy entry in the tree says a pure duplicate of a live value is an allocator
 *   artifact; that is true of the EXISTENCE of the copy, not of its DIRECTION,
 *   which one local does reach. This is "A PIN SET CAN NEED A HOLE, AND THE HOLE IS
 *   WHERE THE ROM ITSELF COMMONS" (:16871) in its named-local dress. Read the
 *   ROM for whether the copy is there before choosing how many locals to write.
 *
 * ------------------------------------------------------ NEW (this batch), and
 * it CORRECTS the b236 mechanism and BOUNDS a recorded clause
 *
 *   THE COPY-DIRECTION FORM OF THE BASIC-BLOCK LEVER *DOES* REACH INSIDE A LOOP
 *   BODY. THE BOUNDARY IS THE BLOCK OF THE CALL ITSELF, NOT THE LOOP.
 *
 *   Grepped by concept first -- "dominating block", "does not reach inside a
 *   loop body", "back edge", "the loop the call sits in", "same basic block as
 *   the call": docs/elevation.md:1771 says the opposite in as many words --
 *
 *       "AND IT DOES NOT REACH INSIDE A LOOP BODY. The assignment has to be in
 *        a block that dominates the call, and in a loop every such block is
 *        also reachable across the BACK EDGE ... So the clause is: a dominating
 *        block that is not part of the loop the call sits in."
 *
 *   Three placements of the SAME two assignments, measured with objcmp:
 *
 *     entry block, before the guard       (cand.c)        OK
 *     TOP OF THE LOOP BODY, inside the loop (n2_inloop.c) OK
 *     inside the guard, adjacent to the calls (n3_adjacent.c)
 *                                         34 encodings + RELOCATIONS differ
 *                                         -- identical to the naive literal form
 *
 *   b236 explained the lever as giving the value "its own live range BORN
 *   OUTSIDE THE LOOP". n2_inloop refutes that: the assignment sits inside the
 *   loop, is re-executed every iteration, and is exact. What n3_adjacent shows
 *   is where the real edge is -- once the assignment and both uses share ONE
 *   basic block, the gcse cprop pass substitutes the constant straight back into the
 *   uses and the name is gone, which is "gcse CPROP IS WHAT KILLS A HELD
 *   CONSTANT" (:15871) and "Naming a value gcc already CARRIES destroys the
 *   carry" (:16624) meeting at the same place. So the requirement is a
 *   DIFFERENT BLOCK, not an OUT-OF-LOOP block.
 *
 *   WHY :1771 IS STILL RIGHT WHERE IT STANDS, AND WHY THIS IS A BOUND AND NOT A
 *   CONTRADICTION. That clause is about the SPLIT-PAIR form, where the job of
 *   the lever is to make gcc REMATERIALISE a two-instruction constant at the
 *   call so another argument schedules into the gap; a loop-carried live range defeats
 *   that by parking the value in a callee-saved register. Its evidence
 *   (`OvlFunc_935_2008b8c`, 2 / 7 / 9) is two placements OUTSIDE the loop, so it
 *   never measured the inside. Here the ROM does NOT carry the value either --
 *   it rebuilds `mov r0,#0xa0 / lsl r0,#11` on every iteration -- and the defect
 *   is only the direction of a copy both sides make. The clause should read: a
 *   block DIFFERENT FROM THE ONE THE CALL IS IN, and additionally OUTSIDE THE
 *   LOOP only when the lever is being asked to force rematerialisation.
 *
 *   PRACTICAL FORM: this is the named-local mirror of the recorded pin rule that
 *   the first-use boundary is ADJACENCY rather than the basic block (:15901).
 *   For a pin, an intervening CALL is as good as a branch; for a NAME, a branch
 *   is required and an intervening call is not enough -- n3_adjacent has three
 *   calls between its assignment and the later use and still folds.
 *
 * ------------------------------------------------------------------- sibling
 *
 * OvlFunc_924_20096c4 STAYS PARKED, and its parking is NOT a tool artefact --
 * its residue is ENCODINGS at exact length, never relocations, so none of the
 * three recorded objcmp false negatives (`_call_via_*` aliases, zero-run
 * elision, the flat alias map fixed today) can be hiding a match. Re-screened
 * under the current objcmp against ref_b.s:
 *
 *   src/non_matching/ovl_7ac2d8/20096c4.c   13 of 91, first ref 2106 ours 2306
 *   b236 park_b.c / pb1.c / pb4.c           13 of 91, same first difference
 *   b236 pb2.c                              88 of 91, and 8 bytes SHORT
 *   b236 pb3.c                              12 of 91   <- one better, still no
 *
 * pb3 is the park shape plus `register int t3 __asm__("r3"); register int t1
 * __asm__("r1")` on ONE in-loop `__CopyMapTiles` stack pair. Buying one encoding
 * with two pins is not a lead, and the park diagnosis -- stack-argument
 * register allocation inside a loop that is already spending r8/r9/r10 -- is
 * unchanged.
 *
 * ------------------------------------------------------------- landing shape
 *
 * FLAG GROUP: DEFAULT, and the match does NOT depend on one. tryc.makefile_flags
 * returns the empty set for all three relevant paths -- the existing
 * asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c.s, the post-split
 * asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c_b.s, and
 * src/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c_b.c. The pattern rules -O2 build
 * is what matches; no Makefile edit.
 *
 * THE .s HOLDS TWO FUNCTIONS. asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c.s:
 *   line   9  .thumb_func_start OvlFunc_924_20095e0   <- this target, 228 bytes
 *   line 125  .thumb_func_start OvlFunc_924_20096c4   <- parked sibling
 * No `.data`, no `.section`, no blob directive, no stranded `.L` label; the only
 * pooled word is the 0xe666 the assembler dumps after the
 * `.func_end`. Neither function calls the other, so nothing blocks the split
 * ("A SAME-FILE CALLEE NEVER BLOCKS A SPLIT", :16285).
 *
 * SO THIS IS A SPLIT, not a whole-file landing. The target is FIRST, so
 * split_s.py writes no `_a` part (empty parts are not written): the target goes
 * to `<stem>_b.s`, to be replaced by src/overlays/rom_7ac2d8/
 * ovl_f84_a_c_c_c_c_c_b.c, and the parked sibling to `<stem>_c.s`.
 *
 * EVERY .ld LINE NAMING THE .o, matched on FULL PATH
 * (`asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c\.o`, not a substring -- the
 * neighbours `..._a_c_c_c_c_a_c.o` and `..._a_c_c_c_c_b.o` are different
 * objects) -- there is EXACTLY ONE, and it is a `.text` line:
 *
 *   overlays/rom_7ac2d8/overlay.ld:60
 *           asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c.o(.text)
 *
 * There is NO `.data` line for this object. The `.data` block of that file is lines
 * 121-124 and names only ovl_f84_c_c_a_a.o, ovl_f84_c_c_a_b.o, ovl_f84_c_c_b.o
 * and ovl_f84_c_c_c.o -- consistent with the .s having no `.data`, and with
 * "A `.data` LINE IN THE LINKER SCRIPT IS NOT EVIDENCE OF DATA" (:16507) read
 * in the other direction.
 *
 * That one line becomes two, in this order, preserving ROM layout:
 *
 *           asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c_b.o(.text)
 *           asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c_c.o(.text)
 *
 * BOTH still say `asm/`, and that is correct, not an oversight: in this overlay
 * the linker script never names a `src/` object for elevated code. Cited by
 * content -- src/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_b.c EXISTS and its
 * linker line, overlay.ld:59, reads `asm/overlays/rom_7ac2d8/
 * ovl_f84_a_c_c_c_c_b.o(.text)`, with asm/overlays/rom_7ac2d8/
 * ovl_f84_a_c_c_c_c_b.s present beside it as the generated gcc output. The build
 * goes src/x.c -> asm/x.s -> asm/x.o, so the C landing needs NO path retarget;
 * the split adds one line and that is the whole linker edit. The only `src/`
 * lines in overlays/rom_7ac2d8/overlay.ld are 16 (exports.o), 107 (imports.o)
 * and 108 (src/lib/call_via.o).
 *
 * THE SPLIT IS SCAFFOLDING (:16989) and must be COLLAPSED by whichever batch
 * finally closes OvlFunc_924_20096c4 -- at that point this file and `_c` become
 * one TU again and this function must be RE-SCREENED after the merge, not
 * assumed still exact.
 *
 * Screened with tools/objcmp.py inside goldensun-build; `make` was not run.
 */
extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);

void OvlFunc_924_20095e0(int a, unsigned int start, unsigned int end)
{
    unsigned int i;
    int s0, s1;
    int c0, n0;

    c0 = 0xa0 << 11;
    n0 = -1;
    if (a != 0)
        __PlaySound(0xdb);
    for (i = start; i < end; i++) {
        __CopyMapTiles(0x2d - (i << 1), 0x20, 0x2c - (i << 1), 0x20, i + 1, 6);
        __CopyMapTiles(0x2d - i, 0x33, 0x2d - i, 0x20, 1, 6);
        __CopyMapTiles(0x6d - i, 0x20, 0x6c - i, 0x20, 1, 4);
        __CopyMapTiles(0x6d - i, 0x33, 0x6d - i, 0x20, 1, 4);
        if (a != 0) {
            __Func_8012330(c0, 0xa0 << 11, 0x80 << 9);
            __Func_8012330(n0, -1, 0xe666);
            __CutsceneWait(a);
        }
    }
    s0 = 0x2a;
    s1 = 0x21;
    __Func_8010704(0x2a, 0x34, 4, 5, s0, s1);
}

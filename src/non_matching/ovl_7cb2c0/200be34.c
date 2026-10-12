/* OvlFunc_945_200be34 -- NON-MATCHING, 14 differing STREAM encodings of 70.
 *
 * FIGURE RE-DERIVED AT THE HEAD OF BATCH 340, not inherited:
 *   ref 70 / ours 70 STREAM encodings, 14 differing, first at index 15;
 *   instructions 48/48 -- IN PHASE, so a true DISTANCE -- and objcmp reports
 *   RELOCATIONS IDENTICAL.
 *
 * *** 0 OF 70 IS REACHED WITH A REGISTER PIN AT FIVE CALL SITES (15 pins,
 * tools/shimcount.py), AND IS NOT SHIPPED.  Candidate kept at
 * scratch_elev/b340/B/v34b/x04_PIN_DEVICE.c.  Per the pin policy that is a
 * figure ABOUT THE BLOCKER, not a landing; this park ships the device-free
 * body and belongs to pass 3. ***
 *
 * PIECE: the reference holds TWO functions, this one and OvlFunc_945_200beec
 * (14 of 63), BOTH parked and BOTH the same residue class.  This park carries
 * the full spelling table for the pair.
 *
 * THE ORIGINAL PROSE'S CORPUS COUNT IS CONFIRMED AND ITS CONCLUSION SHARPENED.
 * It had measured "split mov / mov / lsl : 51 of 2987" generated .s files and
 * concluded "the spelling exists and I have not found it".  Re-counted in
 * batch 340 over 4,549 generated files with the first half of a split build
 * excluded from the cheap-mov count, and partitioned by shimcount: the shape
 * is overwhelmingly PINNED or MULTI-BLOCK.  See the corpus bound below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7cb2c0/200be34.c \
 *     asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a_c_c.s \
 *     --func OvlFunc_945_200be34
 *
 * THE RESIDUE IS ONE CLASS, AND IT IS THE SAME CLASS IN ALL THREE OF THIS
 * BANK'S PARKS (OvlFunc_945_200bd10, _200be34, _200beec -- batch 340 brief B
 * measured all three).  Every differing encoding is the POSITION of a cheap
 * `mov r0, #<slot>` inside a call's argument-setup window.  The ROM emits the
 * cheap argument in ARGUMENT ORDER; we emit it LAST, after every wide
 * argument's build.  Four shapes, all the same decision:
 *
 *     rom   mov r0,#S / ldr r1,=0xcccc / ldr r2,=0x6666
 *     ours  ldr r1 / ldr r2 / mov r0,#S
 *     rom   mov r1,#0x96 / mov r0,#S / lsl r1,#16 / ldr r2,=0x24a0000
 *     ours  mov r1 / lsl r1 / ldr r2 / mov r0,#S
 *     rom   mov r0,#S / mov r1,#0xa8 / ldr r2,=0x242
 *     ours  ldr r2 / mov r0,#S / mov r1,#0xa8
 *     rom   mov r1,#0x80 / mov r2,#0x14 / mov r0,#S / lsl r1,#8
 *     ours  mov r1 / mov r2 / lsl r1 / mov r0,#S
 *
 * Note the third: our `mov r0` is NOT always last -- the CHEAP arguments are in
 * argument order among themselves and the EXPENSIVE ones precede all of them.
 * That is the signature of the pass responsible.
 *
 * WHY, WITH LINE NUMBERS, DERIVED FROM gcc's OWN -da DUMPS.
 *
 * 1. THE CHAIN ORDER IS SET AT EXPAND BY precompute_register_parameters,
 *    calls.c:805-854.  For each register argument in argument order it does
 *        if (! (GET_CODE (args[i].value) == REG ...)
 *            && rtx_cost (args[i].value, SET) > 2
 *            && ((SMALL_REGISTER_CLASSES && *reg_parm_seen) || ...))
 *          args[i].value = copy_to_mode_reg (args[i].mode, args[i].value);
 *    i.e. it materialises the EXPENSIVE arguments into pseudos BEFORE the
 *    hard-register loading loop; the cheap ones are only materialised in that
 *    later loop.  For Thumb, arm.c:2077-2084 gives a CONST_INT under SET
 *        cost 0                 when (unsigned) value < 256
 *        COSTS_N_INSNS (2) = 8  when thumb_shiftable_const (arm.c:8578-8593)
 *        COSTS_N_INSNS (3) = 12 otherwise
 *    so a slot 0x10 costs 0 and goes last, while 0x960000 (8), 0x8000 (8),
 *    0x242 (12), 0xcccc (12) all cost > 2 and go first.  `-fno-schedule-insns2`
 *    shows this chain order directly: the pre-scheduling output already reads
 *    `mov r1,#150 / lsl r1,#16 / ldr r2 / mov r0,#16`.
 *
 * 2. THE SCHEDULER THEN TIES ALL THE WAY DOWN TO INSN_LUID, which is the chain
 *    order.  rank_for_schedule (haifa-sched.c:4029-4113) has four rungs after
 *    reload: priority, then the class relative to last_scheduled_insn
 *    (:4068-4095), then the dependent count (:4096-4109), then INSN_LUID
 *    (:4110-4113).  Read off the .23.sched2 dump for OvlFunc_945_200be34's
 *    `__MapActor_SetPos(0x10, 0x96 << 16, 0x24a0000)` window:
 *        insn 148  mov r1,#0x96   prio 78  cost 1  depends: 140 149
 *        insn 149  lsl r1,#16     prio 77  cost 1  depends: 140 152 44
 *        insn  37  ldr r2,=...    prio 77  cost 2  depends: 140 154 44
 *        insn  39  mov r0,#0x10   prio 77  cost 1  depends: 140 150 44
 *    148 wins on priority; the other three TIE on priority (77), on class
 *    (all 3 -- 149's dependence on 148 has insn_cost 1, which :4076 maps to
 *    class 3 as well) and on dependent count (3, 3, 3), so :4110 decides and
 *    the chain order 149 < 37 < 39 is emitted verbatim.  The trace confirms it:
 *        Ready list (t =138):    39  37  149
 *        --> scheduling insn <<<149>>>
 *    The dependent counts tie because each window's successor call sets the same
 *    three argument registers; where it does NOT tie the rung is live and gcc
 *    DOES hoist a cheap mov -- see OvlFunc_965_20091c4 in
 *    src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_a_b.c, which is byte-exact,
 *    pin-free and single-block, and whose `mov r1,#0 / mov r3,#223` jump a
 *    `lsl r2` on a 5-against-3 dependent count.
 *
 * 3. NAMING THE ARGUMENTS IS INERT, AND cse.c SAYS WHY.  Writing
 *    `q0 = 0x10; q1 = 0x96 << 16; q2 = 0x24a0000;` into plain locals DOES
 *    defeat clause one of calls.c:849 -- .00.rtl shows the expand order as
 *    strict argument order, p32=0x10 first -- and it still measures exactly the
 *    base figure, byte for byte.  cse1 undoes it.  COST (cse.c:509-514) scores
 *    a pseudo REG at 1 and a non-register at notreg_cost = rtx_cost (x, SET) * 2
 *    (cse.c:725-741), so for `(set (reg rK) (reg p))`
 *        cheap const:  0 * 2 = 0  <  1  -> SUBSTITUTED, p's def dies
 *        wide const :  8 or 12 doubled  -> NOT substituted, p survives
 *    and .03.cse shows precisely that: insn 43 becomes `(set (reg r0) 0x10)`
 *    while insns 45/47 keep their pseudos.  The surviving pseudos are then
 *    coalesced into r1/r2 IN PLACE, at their early chain positions, so the
 *    cheap mov is late again.  THE FOLD CONDITION DEPENDS ONLY ON THE
 *    CONSTANT'S MAGNITUDE, so no spelling of a < 256 argument escapes it.
 *
 * 4. THE REMATERIALISATION ROUTE NEEDS A SECOND BASIC BLOCK, AND THESE
 *    FUNCTIONS HAVE ONE BLOCK.  The route that DOES work elsewhere in this bank
 *    is the one recorded in src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_c_c_a_c_a.c
 *    lever 2 -- "declare the constant as a local ABOVE the switch and pass it by
 *    name ... gcc will not keep the value live across it and rematerialises at
 *    each site -- and the rematerialised sequence carries the ROM's interleave
 *    for free".  The pass is update_equiv_regs, and BOTH of its arms are gated
 *    on the same thing:
 *        local-alloc.c:884-889   REG_N_REFS (regno) == 2
 *                                && REG_BASIC_BLOCK (regno) < 0   -> replace
 *        local-alloc.c:960-964   depth == 0 && REG_BASIC_BLOCK (regno) < 0
 *                                -> move the init just before the use
 *    REG_BASIC_BLOCK < 0 means "referenced in more than one basic block"; the
 *    landed sibling's jump-table dispatch is what supplies it.  These three
 *    parks are straight-line call sequences -- the .23.sched2 header reads
 *    "basic block 0 from 145 to 161" and there is no label anywhere -- so
 *    REG_BASIC_BLOCK >= 0 for every local and NEITHER arm can fire.  That is
 *    why "the basic-block lever has nothing to bite on", which the original
 *    prose said, is right for a reason it did not give.
 *
 * 5. THE MEASURED ESCAPE IS A REGISTER PIN, AND IT IS A DEVICE, SO IT IS
 *    RECORDED AS A FIGURE ABOUT THE BLOCKER AND NOT SHIPPED.  The pin makes
 *    `q0 = 0x10` expand straight to `(set (reg:SI 0 r0) (const_int 16))` at the
 *    statement, so there is no pseudo for cse to fold and no copy to coalesce,
 *    and the chain order becomes source order.  This is exactly what the bank's
 *    byte-exact src/overlays/rom_78ac38/ovl_30_c_c_c_b.c does for the IDENTICAL
 *    shape at 53 pin sites (`{ PIN3; q0 = 0xb; q1 = 0xa0 << 8; q2 = 0; ... }`
 *    -> `mov r1,#160 / mov r0,#11 / lsl r1,#8 / mov r2,#0`), and that file ships
 *    with a fakematch.txt row.
 *
 * THE CORPUS BOUND, measured in batch 340; the scripts are kept at
 * scratch_elev/b340/B/b340B_corpus*.py.  Over the 4,549 GENERATED (and so
 * byte-matching by construction) .s files in asm/, restricting to PURE argument
 * windows and excluding the first half of a split build from the "cheap mov"
 * count:
 *     .c carries a register pin      17 single-block functions, 31 windows
 *     .c is PIN-FREE, single block    3 single-block functions,  4 windows
 *     .c is PIN-FREE, multi block   153 functions, 314 windows
 * and all four single-block pin-free windows were inspected: two are
 * OvlFunc_965_20091c4/_200919c, where the dependent-count rung (2 above) is
 * live rather than LUID, and two are OvlFunc_897_200ac1c, where the values come
 * from PARAMETERS and are genuine REGs rather than constants.  None is a cheap
 * CONSTANT argument hoisted over a wide CONSTANT build.  So the shape is
 * reachable pin-free only through rungs these windows do not offer.
 *
 * MEASURED ON THIS BODY, batch 340, all at production flags (base 14 of 70,
 * 48/48 instructions, relocations identical).  EVERY ROW BELOW IS BYTE-FOR-BYTE
 * IDENTICAL TO THE BASE unless marked, which is the point -- the space is flat:
 *   each residue site's three arguments named in plain locals, in argument
 *     order, immediately before the call (all five sites)            14
 *   the same at one site only, each of the five taken separately     14
 *   the same at four of five, both ways round                        14
 *   the actor slot named in ONE local used at all eight sites         14
 *   the wide argument named in a local at the SetPos site             14
 *   plain hex literals (0x960000, 0x8000, 0x9c0000, 0x2180000)
 *     instead of the shifted forms                                   14
 *   *** the five sites with PIN3 blocks                               0 ***
 * CROSSED WITH THE FLAG AXIS, because a flag inert on a park body need not be
 * inert after a source change (batch 337's -O1 landing).  Both the all-locals
 * and the slot-carrier spellings, under each of the eight per-file groups plus
 * `-fno-gcse -fno-rerun-cse-after-loop` together: -O1 22, -fno-schedule-insns2
 * 22, all seven others 14, in every combination.  Nothing crossed.
 * AUTOSWEPT (tools/autosweep.py, batch 340): BOTH CHEAP AXES ARE CLOSED --
 *   15 anchor placements all inert at 14, and the flag figures above.
 *
 */
extern void __CutsceneStart(void);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void OvlFunc_945_200c8ac(int a, int b, int c, int d);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __MessageID(int id);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_945_200be34(void)
{
    __CutsceneStart();
    OvlFunc_945_200c8e8(0x18, 0, 0);
    OvlFunc_945_200c8e8(0x12, 0, 0);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(0x10, 0x96 << 16, 0x24a0000);
    OvlFunc_945_200c8ac(0x9c << 16, -1, 0x86 << 18, 0x1000001);
    OvlFunc_945_200c8e8(8, 0, 0);
    __MapActor_SetSpeed(0x10, 0xcccc, 0x6666);
    __Func_80921c4(0x10, 0xa8, 0x242);
    __Func_80921c4(0x10, 0xa8, 0x22a);
    __Func_8092adc(0x10, 0x80 << 8, 0x14);
    __Func_809259c(0x10, 2);
    __MessageID(0x1e3c);
    __Func_8093040(0x10, 0, 0x14);
    OvlFunc_945_200c8e8(9, 0xc, 0);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_945_200be34 -- asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c.s
 *
 * BLOCKER: THE SPLIT SHIFTED BUILD -- unreachable, proved by corpus test
 *
 * 14 of 63, same length.  49 lines exact, including the four-argument
 * OvlFunc_945_200c8ac call with its interleaved lsl/neg/lsl, which matches
 * byte for byte.  The entire residue is one transposition:
 *
 *     rom  mov r1,#0x96 / mov r0,#0x10 / lsl r1,#0x10 / ldr r2,=0x24a0000
 *     ours mov r1,#0x96 / lsl r1,#0x10 / ldr r2,=0x24a0000 / mov r0,#0x10
 *
 * The ROM slots `mov r0, #0x10` between the two halves of the `0x96 << 16`
 * build; gcc completes the build first.  Structurally identical to the
 * `mov #K / mov #0 / neg / mov #0` family in ovl_7c460c/2008c74.c -- a
 * multi-instruction constant build split by an unrelated `mov`.
 *
 * CORRECTED: this is NOT unreachable.  I first measured "0 of 2987 generated
 * .s files contain mov rA,#K / mov rB,#K2 / lsl rA,#n" and concluded the class
 * was closed.  The detector was broken: generated .s files are gcc's own output
 * and use DECIMAL immediates and the THREE-operand shift (`lsl r2, r2, #1`),
 * while my pattern required the ROM's two-operand form.  It matched nothing.
 *
 * Re-measured with both forms accepted and a positive control:
 *     adjacent mov/lsl build (control) : 777 of 2987
 *     split mov / mov / lsl            :  51 of 2987
 *
 * gcc DOES emit this shape, 51 times.  So the spelling exists and I have not
 * found it.  This park is "not yet solved", not "cannot be solved".
 *
 * Best C: scratch/rbe34.c.
 *)

*/
#endif

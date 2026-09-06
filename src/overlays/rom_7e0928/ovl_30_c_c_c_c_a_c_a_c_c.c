/* OvlFunc_956_2009c20  --  0x02009c20    EXACT
 *
 * Cut out of asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c_c.s, which holds
 * this ONE function and NO data (no .section, .data, .word, .byte, .short or
 * .incbin).  187 instructions / 187 encodings / 472 bytes.  Cutscene script:
 * the gState 0xe1 guard, __CutsceneStart, a three-way dispatch on
 * OvlFunc_common1_4cc(a, 1), a four-leg two-actor walk driven by
 * __Actor_TravelTo, and the shared OvlFunc_common1_5e4 / __CutsceneEnd tail.
 *
 * VERDICT
 *   OK OvlFunc_956_2009c20 -- 472 bytes, 187 encodings and 45 relocations identical
 *   Measured with tools/objcmp.py against BOTH scratch_elev/b238/f2009c20/ref.s
 *   and the ORIGINAL asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c_c.s
 *   (--func OvlFunc_956_2009c20).  The two references AGREE, so no Makefile
 *   pattern rule is biting on this path.
 *
 * FLAG GROUP: NONE.  `grep -n rom_7e0928 Makefile` is empty -- there is no
 * per-file rule for this bank, so the TU builds under plain GCC296_CFLAGS at
 * -O2 and screening green here means building green.  Nothing below depends on
 * a flag.
 *
 * TWINS: none.  `python3 tools/solved_twins.py` reports "REMAINING FUNCTIONS
 * WITH A SOLVED TWIN: 0 across 0 templates" over 2089 solved shapes.
 *
 * SELECTION.  The nominated template was
 * src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c_b.c (0.76, 21 shared symbols)
 * and it was read first, but its levers were RE-MEASURED rather than copied and
 * most of them are NOT needed here.  What actually transferred is one line: the
 * `g = gState;` local.  Everything the neighbour fights over -- PIN23/PIN3 fills
 * at __Func_80933d4, the first __Func_80933f8, __MapActor_SetSpeed, the split
 * vs whole-value spelling of a shifted pair -- is exact here as a BARE CALL with
 * whole-value arguments and a full prototype.  "SUFFICIENT NOT NECESSARY",
 * confirmed a sixth time.
 *
 * READING THE PROLOGUE BY CONTENT.  `push {r5, r6, r7, lr}` plus the two-step
 * `mov r7,r10 / mov r6,r8 / push {r6,r7}` is FIVE callee-saved registers and,
 * counted BY DESTINATION, exactly two high ones -- r8 and r10, skipping r9 and
 * r11.  Each is a value, not scratch:
 *     r5  = e1/e2  the actor-0xb pointer, alive across __Actor_TravelTo
 *     r6  = r      the OvlFunc_common1_4cc result, live to the very last call
 *     r7  = a      the parameter
 *     r8  = 0x6666 the field-0x34 constant, gcc-hoisted (NOT a source local)
 *     r10 = 0xcccc the field-0x30 constant, gcc-hoisted (NOT a source local)
 * r8-before-r10 in the push and r10 = the FIRST-loaded constant are both what
 * `REG_ALLOC_ORDER` {3,2,1,0,12,14,4,5,6,7,8,10,9,11} predicts, so no source
 * construct is needed for either.  Per "A value in a callee-saved register is
 * NOT evidence the source named it": the literal spelling `= 0xcccc` at all four
 * sites reproduces r10 on the first screen, and naming the two constants was
 * never tried because the un-named form was already exact.
 *
 * FOUR LEVERS, EACH WITH ITS MECHANISM
 *
 * 1. TWO PINS ON r2, AT THE FIRST USE OF EACH SHIFTED CONSTANT.  The four
 *    __Actor_TravelTo calls take y = 0x80<<11, 0x80<<14, 0x80<<14, 0x80<<11 --
 *    two distinct values, each used twice, all four inside ONE basic block.
 *    With plain literals gcc hoists BOTH into callee-saved highs, and the
 *    prologue grows from `mov r7,r10 / mov r6,r8 / push {r6,r7}` to a four-high
 *    save: +12 bytes, +6 encodings, 179 differing.  Binding y to a hard r2 --
 *    call-clobbered, so it cannot survive the `bl` -- destroys the CSE and makes
 *    gcc rebuild `mov r2,#0x80 / lsl r2,#N` in the argument register, which is
 *    what the ROM does four times.
 *
 *    ONLY THE FIRST USE OF EACH VALUE IS PINNED.  Sites 3 and 4 are the SECOND
 *    use of 0x80<<14 and of 0x80<<11; with the first use written straight into
 *    r2 there is no pseudo left for CSE to hand them, and their pins are inert.
 *    This is the recorded "ONE PIN AT THE FIRST USE COVERS THE LATER ONES", and
 *    the adjacency boundary recorded beside it does not bite because two `bl`s
 *    and a __CutsceneWait separate each pair.
 *
 *    THE SWEEP HAS A DIRECTION, and it cost a round here.  Starting from four
 *    pins, dropping site 3 is inert but dropping site 4 costs 66.  Dropping
 *    site 3 FIRST and then re-opening site 4 makes site 4 inert too.  A
 *    one-lever-at-a-time sweep reported "worse" and was wrong; only re-opening
 *    every later site after each successful drop reaches the two-pin fixpoint.
 *
 * 2. `e` IS TWO VARIABLES, `p` IS ONE.  Four __MapActor_GetActor results.  The
 *    two actor-0xa pointers die at their __Actor_TravelTo and live in r0; the
 *    two actor-0xb pointers cross it to reach __Actor_WaitMovement and live in
 *    r5.  Written as ONE `e` spanning both 0xb legs, that pseudo's live range
 *    covers the whole middle of the function, its priority
 *    (floor_log2(n_refs)*n_refs/live_length) falls below `r` and `a`, and the
 *    three low callee-saved registers rotate: ours a=r6/r=r5/e=r7 against the
 *    ROM's a=r7/r=r6/e=r5.  172 differing, and 4 BYTES SHORTER -- the length
 *    tell pointing the wrong way again.  Split into `e1` and `e2` the two short
 *    ranges are disjoint, both take r5, `r` takes r6 and `a` takes r7.
 *
 *    THE SPLIT IS ONLY FOR THE CALL-CROSSING POINTER.  Splitting `p` the same
 *    way is measured INERT (w3.c).  So the recorded "a variable with DISJOINT
 *    live ranges should be two variables" needs the value to actually compete
 *    for a callee-saved register; a pointer that dies before the next `bl` never
 *    enters the contest and one variable is enough.
 *
 * 3. THE TWO FIELD STORES ARE WRITTEN IN THE OPPOSITE ORDER TO THE ROM'S.
 *    The ROM emits, in every leg, `str rX,[rB,#0x30]` (0xcccc) BEFORE
 *    `str rY,[rB,#0x34]` (0x6666).  The source that produces that writes
 *        *(int *)(p + 0x34) = 0x6666;
 *        *(int *)(p + 0x30) = 0xcccc;
 *    Writing them in the ROM's emitted order costs 10 differing WITH THE SAME
 *    SIZE AND ENCODING COUNT and, invisibly to tryc, ROTATES THE LITERAL POOL:
 *        ROM / ours:  gState, 0x20b2, 0xcccc, 0x6666, 0x20b1
 *        wrong order: gState, 0x20b2, 0x6666, 0xcccc, 0x20b1
 *    Both constants are SImode word references with the same `pool_range`, so
 *    the recorded max_address sort cannot separate them and the tie breaks on
 *    reference address -- which follows the scheduled order, which sched2
 *    REVERSES relative to the source.  (See lever-4 note below; grepped as new.)
 *
 * 4. PIN r1 AND r2 AT OvlFunc_common1_5e4.  The ROM's tail is
 *    `mov r1,r7 / mov r2,#1 / mov r0,r6` -- r0 LAST.  Bare, gcc emits
 *    r0/r1/r2 and leaves 3 differing.  `{ q1 = a; q2 = 1; f(r, q1, q2); }` with
 *    q1/q2 bound to r1/r2 is exact.  Both individual pins are load-bearing
 *    (dropping r1's costs 3, r2's costs 2).
 *    EQUALLY EXACT ALTERNATIVE, measured: deleting the OvlFunc_common1_5e4
 *    prototype (the recorded "r0 is LAST -> drop the prototype" cure) also
 *    reaches zero -- w1.c.  The pinned form ships because it keeps the file's
 *    declaration set complete and matches the same-overlay siblings' idiom.
 *
 * WHAT NEEDED NOTHING, and is worth recording because a template would have
 * added it:
 *   * __ActorMessage is called FIVE times as a bare `__ActorMessage(a, 0)` with
 *     a full prototype, and the ROM spells it BOTH ways -- `mov r0,r7/mov r1,#0`
 *     at three sites and `mov r1,#0/mov r0,r7` at two.  sched2 produces both
 *     from the one source form.  The no-prototype lever is NOT needed and would
 *     have broken three sites to fix two.
 *   * OvlFunc_common1_1490 (r1,r2,r0) and OvlFunc_common1_14f4 (r2,r1,r0 at two
 *     sites, r1,r2,r0 at one) both put r0 LAST at every site and are exact as
 *     BARE CALLS WITH `void` PROTOTYPES.  The recorded argument-order table is
 *     measured on ONE-LINE functions; deep inside a 90-call straight-line block
 *     the scheduler already delivers r0-last, and reaching for the return-type
 *     or no-prototype lever here is unnecessary.
 *   * __Func_80933d4(0xc0<<10, 0xc0<<7) and
 *     __Func_80933f8(0x98<<16, -1, 0xc8<<16, 1) -- the neighbour's PIN4 shape --
 *     are exact bare, with `-1` written as the literal (never `x=1; x=-x`).
 *   * `p[0x55] = 0;` compiles to `strb r6,[r3]` because cse's
 *     `record_jump_equiv` knows r == 0 inside the `if (r == 0)` arm and prefers
 *     the register it is already in.  Writing `p[0x55] = r;` is byte-identical
 *     (w4.c); the honest `0` ships.
 *
 * MEASURED-WORSE TABLE (all one-lever strips from the SHIPPED source unless
 * noted; the function is 187 encodings, so a count is out of 187):
 *   spelling                                                        differing
 *   ---------------------------------------------------------------  -------
 *   drop the r2 pin at travel site 1 (0x80<<11, first use)   178, +8 BYTES
 *   drop the r2 pin at travel site 2 (0x80<<14, first use)   180, +8 BYTES
 *   drop BOTH travel pins                                    180, +12 BYTES
 *   pins written as `int q2` instead of `register ... ("r2")` 180, +12 BYTES
 *   merge e1/e2 into one `e`                                 172, -4 BYTES
 *   delete the `g` local, fold to *(short*)(gState + 0x1c2)  172, -4 BYTES
 *   write the 0x30 store before the 0x34 store                10 (pool rotates)
 *   OvlFunc_common1_5e4 bare, no pins                          3
 *   OvlFunc_common1_5e4 drop the r1 pin (keep r2)               3
 *   OvlFunc_common1_5e4 drop the r2 pin (keep r1)               2
 *   -- and, from the FOUR-pin intermediate, the direction trap:
 *   drop the r2 pin at travel site 4 while site 3 is pinned    66, +4 BYTES
 *   drop the r2 pin at travel site 3 (inert; enables site 4)     0
 *
 * ALSO MEASURED INERT, and therefore NOT SHIPPED (each re-measured against the
 * final source): a third and fourth r2 pin at travel sites 3 and 4; splitting
 * `p` into p1/p2; `p[0x55] = r` in place of `= 0`; the split fill
 * `q2 = 0x80; q2 <<= 11;` in place of the whole-value `q2 = 0x80 << 11;`;
 * dropping the OvlFunc_common1_5e4 prototype instead of pinning it.
 *
 * THE `g` LOCAL IS LOAD-BEARING, as both same-overlay siblings record.
 * `extern unsigned char gState[]` plus `g = gState;` keeps the base in one
 * register and the 0xe1<<1 offset in another, which is what Thumb-1's
 * immediate-less `ldrsh` needs.
 *
 * LANDING: WHOLE FILE, NO LINKER EDIT, NO SPLIT, NO LABEL EXPORT.
 * asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c_c.s holds ONE function
 * (`grep -c thumb_func_start` = 1, line 8 .thumb_func_start
 * OvlFunc_956_2009c20 .. line 196 .func_end) and no data at all.  Its five
 * local labels (.L1c40 .L1c54 .L1db8 .L1dca .L1dd8) are defined and referenced
 * only inside it -- a grep of asm/overlays/rom_7e0928/ finds each in that file
 * and nowhere else -- so nothing has to be exported.
 * EXACTLY ONE linker-script line in the whole tree names this .o, matched on the
 * FULL PATH (the same basename also appears under rom_7d95dc with _a/_b
 * suffixes and is a different file):
 *     overlays/rom_7e0928/overlay.ld
 *         "\t\tasm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c_c.o(.text)"
 *     -- it sits between the lines naming
 *        asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c_b.o(.text)  (above) and
 *        asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_b.o(.text)      (below).
 * That line needs NO change: Makefile's cross-dir rule
 *     asm/%.o: src/%.c
 *         $(GCC296_CC) $(GCC296_CFLAGS) -S -o $(@:.o=.s) $<
 * builds asm/<bank>/X.o from src/<bank>/X.c, and the immediate sibling
 * ovl_30_c_c_c_c_a_c_a_c_b is already landed exactly this way (git ls-files
 * shows src/.../_b.c with no committed asm/.../_b.s).  So the landing is:
 * add src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c_c.c, delete
 * asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c_c.s, touch no .ld.
 *
 * OvlFunc_954_20093e4 (asm/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_c.s, also one
 * function) IS A SIBLING, NOT A TWIN.  Same skeleton, same prologue and
 * epilogue, same four-leg two-actor walk, same 0xcccc/0x6666 pair -- but 201
 * instructions to 187 (an extra __Func_80933d4/__Func_80933f8/__Func_8093530
 * group), mode 2 instead of 1 at 4cc/588/5e4, actors 0xd/0xe instead of
 * 0xa/0xb, and one leg whose y is a plain `mov r2,#0` rather than a shift.  The
 * REGISTER ROLES DIFFER, which is what makes it a sibling rather than a twin:
 * there `r` lives in the HIGH r10 (`mov r2,r10 / strb r2,[r3]`,
 * `mov r2,r10 / cmp r2,#1`) and 0xcccc lives in the LOW r6, where here `r` is
 * r6 and 0xcccc is r10.  Levers 1 and 4 should transfer; lever 2 must be
 * re-derived against that rotation, and lever 3 is INVERTED there -- its ROM
 * emits `str [+0x34]` BEFORE `str [+0x30]` at all four sites, so it wants the
 * opposite source order to this one.  No sed pair will do it.
 *
 * NEW FINDING (grepped by concept first: "pool order", "pool word order",
 * "POOL POSITION IS SET BY THE MODE OF THE REFERENCE", "Pool ORDER is a readout
 * of operand modes" -- all of which cover pooled constants whose references
 * differ in MODE).  WHEN TWO POOLED CONSTANTS ARE REFERENCED IN THE SAME MODE,
 * max_address CANNOT SEPARATE THEM AND THE TIE BREAKS ON SOURCE STATEMENT
 * ORDER -- reversed, because sched2 swaps the adjacent pair.  Two `int` stores
 * to adjacent fields is the minimal case, it is worth 10 differing encodings
 * plus a pool rotation, and the fix is a one-line statement swap.  A pool-order
 * residue is therefore not always a mode question; check the SAME-mode tie
 * before varying types.
 */
extern unsigned char gState[];

extern void OvlFunc_common1_2c4(void);
extern int OvlFunc_common1_4cc(int a, int b);
extern void OvlFunc_common1_588(int a, int b);
extern void OvlFunc_common1_5e4(int a, int b, int c);
extern void OvlFunc_common1_1490(int a, int b, int c);
extern void OvlFunc_common1_14f4(int a, int b, int c);
extern void OvlFunc_common1_1550(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetCameraTarget(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_TravelTo(unsigned char *e, int x, int y, int z);
extern void __Actor_WaitMovement(unsigned char *e);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

/* call-clobbered destination: r2 cannot survive a `bl`, so the shifted
   constant is rebuilt at the site instead of being hoisted to a high reg */
#define PINY register int q2 __asm__("r2")

void OvlFunc_956_2009c20(int a)
{
    unsigned char *g;
    unsigned char *p;
    unsigned char *e1;
    unsigned char *e2;
    int r;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
    } else {
        __CutsceneStart();
        r = OvlFunc_common1_4cc(a, 1);
        if (r == 0) {
            __MessageID(0x20b2);
            __Func_80933d4(0xc0 << 10, 0xc0 << 7);
            __Func_80933f8(0x98 << 16, -1, 0xc8 << 16, 1);
            __Func_8093530();
            __CutsceneWait(0x1e);
            __ActorMessage(a, 0);
            OvlFunc_common1_1490(0x68, 0x44, 0);
            __CutsceneWait(0x3c);
            OvlFunc_common1_14f4(0xa8, 0x60, 0xa);
            __CutsceneWait(0x46);
            __ActorMessage(a, 0);
            OvlFunc_common1_1550();
            __WaitFrames(2);

            p = __MapActor_GetActor(0xa);
            p[0x55] = 0;
            *(int *)(p + 0x34) = 0x6666;
            *(int *)(p + 0x30) = 0xcccc;
            { PINY; q2 = 0x80 << 11;
              __Actor_TravelTo(p, *(int *)(p + 8), q2, *(int *)(p + 0x10)); }

            e1 = __MapActor_GetActor(0xb);
            e1[0x55] = 0;
            *(int *)(e1 + 0x34) = 0x6666;
            *(int *)(e1 + 0x30) = 0xcccc;
            { PINY; q2 = 0x80 << 14;
              __Actor_TravelTo(e1, *(int *)(e1 + 8), q2, *(int *)(e1 + 0x10)); }
            __Actor_WaitMovement(e1);
            __CutsceneWait(0x2d);

            p = __MapActor_GetActor(0xa);
            p[0x55] = 0;
            *(int *)(p + 0x34) = 0x6666;
            *(int *)(p + 0x30) = 0xcccc;
            __Actor_TravelTo(p, *(int *)(p + 8), 0x80 << 14, *(int *)(p + 0x10));

            e2 = __MapActor_GetActor(0xb);
            e2[0x55] = 0;
            *(int *)(e2 + 0x34) = 0x6666;
            *(int *)(e2 + 0x30) = 0xcccc;
            __Actor_TravelTo(e2, *(int *)(e2 + 8), 0x80 << 11, *(int *)(e2 + 0x10));
            __Actor_WaitMovement(e2);
            __CutsceneWait(0xf);

            __ActorMessage(a, 0);
            OvlFunc_common1_1490(0x68, 0x44, 0);
            __CutsceneWait(0x1e);
            OvlFunc_common1_14f4(0xa8, 0x60, 0xa);
            __CutsceneWait(0x28);
            OvlFunc_common1_14f4(0x68, 0x44, 0xa);
            __CutsceneWait(0x46);
            __ActorMessage(a, 0);
            OvlFunc_common1_1550();
            __WaitFrames(2);
            __SetCameraTarget(0, 0);
            OvlFunc_common1_588(a, 1);
        } else if (r == 1) {
            __MessageID(0x20b1);
            __ActorMessage(a, 0);
        }
        { register int q1 __asm__("r1"); register int q2 __asm__("r2");
          q1 = a; q2 = 1; OvlFunc_common1_5e4(r, q1, q2); }
        __CutsceneEnd();
    }
}

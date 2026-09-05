/* OvlFunc_961_2008194 + OvlFunc_961_2008120  --  0x02008194 / 0x02008120  EXACT
 *
 * Whole-file replacement for asm/overlays/rom_7ebdfc/ovl_30_c_c_c_a.s.
 *
 * BATCH 234.  Batch 233 reached this same source and died before landing it;
 * everything below is RE-MEASURED from scratch in b234, not carried over.  Two
 * of batch 233's claims did not survive re-checking and are corrected at the
 * bottom.
 *
 * VERDICT (tools/objcmp.py, run against the ORIGINAL asm/ path):
 *
 *   OK OvlFunc_961_2008194 -- 116 bytes, 48 encodings and 7 relocations identical
 *   OK OvlFunc_961_2008120 -- 116 bytes, 48 encodings and 7 relocations identical
 *
 * objcmp's --func cannot isolate a function in a MULTI-function candidate, so
 * each was screened from a single-function extract (v194.c / v120.c) and the
 * two-function file was then checked as a WHOLE OBJECT against the whole .s:
 *
 *   size 232/232, 96 encodings, 0 differing, relocations identical -> OK
 *
 * The scratch single-function ref.s and the original asm/ path both resolve to
 * the SAME flag list, so no Makefile pattern rule is biting:
 *
 *   -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi -fno-builtin -nostdinc
 *   -ffreestanding -fcall-used-r4
 *
 * ============================================================================
 * THE SITE.  One interleave site, straight-line, the `neg` variant:
 *
 *   rom  mov r2,#0x10 / mov r0,#0 / mov r1,#0 / neg r2,r2 / bl __Func_80922c4
 *
 * -- two single-instruction zero arguments landing INSIDE the mov+neg split
 * build of the third.  This closes src/non_matching/ovl_7ebdfc/2008120.c, a
 * park worked four times (original, 94, 196, 207) and last declared "the one
 * remaining lever ruled out by measurement".
 *
 * THE BARE UNIFORM WHOLE-VALUE CALL WAS TRIED FIRST AND IT IS NOT ENOUGH here:
 * `__Func_80922c4(0, 0, -0x10);` under a `void` prototype scores 3 differing.
 * The documented straight-line cure ("Re-derived: the wall hid 230 functions")
 * cites OvlFunc_954_2008270 closing the `neg` variant from a bare call with no
 * pins; that is not this function.  What this one needs is the DOCUMENTED
 * REFINEMENT -- pin the single-instruction arguments, leave the split build
 * bare -- TOGETHER WITH the callee's return type, and neither half alone.
 *
 * MEASURED-WORSE TABLE.  Differing encodings of 48, OvlFunc_961_2008194 against
 * asm/overlays/rom_7ebdfc/ovl_30_c_c_c_a.s.  Rows re-run in b234; "decl" is
 * __Func_80922c4's declaration, "fill" the argument spelling at the site.
 *
 *   decl          fill                                         differing
 *   ------------  -------------------------------------------  ---------
 *   void          q0,q1 pinned, split build bare  (SHIPPED)         0
 *   void          q0,q1,q2 all pinned (PIN3)                        0  INERT
 *   void          declarations swapped, assignments q0-then-q1      0  (equiv)
 *   void          pins carry their initialisers                     0  (equiv)
 *   void          third argument written -16 not -0x10              0  (inert)
 *   void          BARE CALL                                         3
 *   void          plain non-register `int q0 = 0; int q1 = 0;`      3
 *   void          q0 pinned only                                    2
 *   void          q1 pinned only                                    3
 *   void          q0,q2 pinned, q1 bare                             2
 *   void          assignments reversed to `q1 = 0; q0 = 0;`         2
 *   void          `int m = -0x10;` named, passed by name            3
 *   void          third argument written `0 - 0x10`                 3
 *   (none)        BARE CALL      <-- THE PARKED SOURCE              2
 *   (none)        q0,q1 pinned                                      3
 *   (none)        q0,q1,q2 pinned                                   3
 *   (none)        q0 pinned only / q0,q2 pinned / plain locals      2
 *   int f()       every fill above                                  2 or 3
 *
 * Structural rows, dropped from the SHIPPED form:
 *
 *   no `tbl` local, `.L5d0` indexed directly                        2
 *   `tbl + off + 2` inline instead of `off += 2`         35, 4 bytes LONGER
 *   `int x, y` instead of `unsigned short x, y`          31, 8 bytes SHORT
 *   `int off` instead of `unsigned int off`                         0
 *
 * `int off` is exact too, so the `unsigned` is a free choice, not scaffolding;
 * it is kept because "Address arithmetic in `unsigned int` locals" is the
 * corpus convention for a byte offset added to a pointer.  The q2 pin IS inert
 * scaffolding and is NOT shipped.  Both remaining pins are load-bearing (2 and
 * 3 differing when dropped), so the shipped pin set is minimal.
 *
 * MECHANISM.  Both halves decide the same sched1 tie, which is why they compose
 * and why neither works alone:
 *
 *   - RETURN TYPE.  An implicitly-int callee carries `(set (reg r0) (call))`.
 *     That is the next real write of r0, so it TRUNCATES the dependent list of
 *     the `mov r0,#0` feeding it and the mov loses the tie ("The return-type
 *     lever's mechanism").  Declaring the callee `void` restores the dependent
 *     and hands the decision back to LUID -- i.e. to source order.
 *   - THE PINS.  With the tie handed back to LUID, source order is what LUID
 *     reads, and the uniform whole-value fill is what makes the two seeds tie
 *     in the first place ("Re-derived: the wall hid 230 functions").
 *
 * ASSIGNMENT ORDER IS THE PIN, DECLARATION ORDER IS NOT -- re-confirmed here in
 * both directions.  Swapping the two `register` DECLARATIONS while keeping
 * `q0 = 0; q1 = 0;` is still exact; swapping the two ASSIGNMENTS costs 2.  The
 * recorded "pin DECLARATION order is argument-setup order" describes a fill
 * whose declarations carry their initialisers -- that spelling is also exact
 * here -- but where the assignments are separate statements it is those that
 * set LUID.
 *
 * ============================================================================
 * NEW FINDING (grepped by concept first).  One of batch 233's two claims is
 * NOT new and is corrected here.
 *
 * NOT NEW -- "the prototype lever runs in both directions".  docs/elevation.md
 * "A rotation in the argument moves: the callee's RETURN TYPE" already states
 * it, and states it ABOUT THIS FUNCTION BY NAME: "adding a prototype to a
 * callee that had none moves r0 EARLIER only if the prototype you add says
 * `void`.  `OvlFunc_961_2008120` improved on deleting a `void` declaration and
 * would have improved just as much on changing it to `int`."  My measurements
 * agree exactly (void+bare 3 -> none+bare 2).  Do not write that up again.
 * What is worth adding is only the TOOLING half: `tools/protolever.py` screens
 * "with callee prototypes DELETED", one direction only, so no run of it can
 * reach a cure that is an ADDITION -- and a park whose declarations are already
 * gone is exactly where it cannot help.
 *
 * NEW -- AT AN INTERLEAVE SITE THE RETURN TYPE AND THE PIN FILL ARE ONE LEVER,
 * AND EACH HALF MEASURES ACTIVELY WORSE ALONE.  Grepped by concept for "worse
 * alone", "neither alone", "one lever at a time", "in combination", "two levers
 * at once", "actively worse": no hits anywhere in docs/elevation.md.  From the
 * table above, on the parked source (prototype withheld, bare call, 2
 * differing): adding the pins alone -> 3.  Adding the `void` prototype alone ->
 * 3.  Adding both -> EXACT.  A sweep that moves one lever at a time therefore
 * reports not "inert" but "worse" -- the strongest signal there is to stop --
 * and it is wrong.  That is precisely how four passes missed this: batch 94
 * swept the prototype with no pins, batches 196/207 swept the pins with no
 * prototype, and each concluded correctly from its own numbers that its lever
 * was dead.
 *
 *   OPERATIONAL RULE: WHEN AN INTERLEAVE RESIDUE SURVIVES A FULL PIN SWEEP,
 *   RE-RUN THE SWEEP UNDER THE OPPOSITE RETURN TYPE BEFORE RECORDING A BLOCKER.
 *
 * It also supplies the missing third leg of the batch-147 paragraph that says a
 * straight-line function wanting `mov r0` in the MIDDLE of another argument's
 * split build has only two levers -- the withheld prototype (which moves r0
 * only LATER) and the dominating-block lever (which needs a block).  The pinned
 * uniform whole-value fill under a RESTORED `void` prototype is a third, needs
 * no block, and puts `mov r0` exactly in the middle.  That paragraph's two
 * measured casualties, OvlFunc_967_2008308 and OvlFunc_911_20082b4, were both
 * measured with the NAMED-LOCAL spelling and not with a pinned fill under a
 * restored `void`; the named-local spelling scores 3 here too, so they are
 * worth one screen each.
 *
 * ============================================================================
 * LANDING: WHOLE FILE, NO LINKER EDIT.  The .s holds EXACTLY these two
 * functions (`grep -c thumb_func_start` = 2) and NO data -- no .section/.data/
 * .word/.byte/.incbin and no local labels of its own.  .L5d0/.L5e8/.L5fe live
 * in asm/overlays/rom_7ebdfc/ovl_30_c_c_c_c_c.s and are already `.global`
 * there (lines 4-6), so no export work is needed.
 *
 * EVERY linker line naming the object, matched on FULL PATH (the basename
 * `ovl_30_c_c_c_a` recurs under rom_77a7c8, rom_7b4558, rom_7cb2c0, rom_7a7298,
 * rom_780898 and others, so a basename grep is not the test) -- there is
 * exactly one, and it is UNCHANGED:
 *
 *     overlays/rom_7ebdfc/overlay.ld:24
 *         asm/overlays/rom_7ebdfc/ovl_30_c_c_c_a.o(.text)
 *
 * (The only other full-path hit in the tree is a prose comment in
 * src/overlays/rom_7ebdfc/ovl_30_c_c_c_b.c:5.)
 *
 * FLAG GROUP: default.  `grep -n rom_7ebdfc Makefile` is empty and no explicit
 * or wildcard rule matches this object, so it falls to the generic
 * `asm/%.o: src/%.c` at Makefile:146, which puts the object exactly where the
 * script already looks.  Land as src/overlays/rom_7ebdfc/ovl_30_c_c_c_a.c,
 * delete the .s, and delete the park src/non_matching/ovl_7ebdfc/2008120.c.
 *
 * THE OTHER FUNCTION IN THE .s IS SOLVED BY THE SAME MEANS.  OvlFunc_961_2008120
 * is instruction-identical to OvlFunc_961_2008194 apart from the descriptor
 * handed to __Func_8010560 (.L5e8 vs .L5fe); both are included below and both
 * verify exact, so this file is two-for-two.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char L5d0[] __asm__(".L5d0");
extern unsigned char L5e8[] __asm__(".L5e8");
extern unsigned char L5fe[] __asm__(".L5fe");
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *p, int a, int b);
extern void __Func_8091e9c(int a);
extern void __Func_80922c4(int a, int b, int c);

void OvlFunc_961_2008120(void)
{
    unsigned char *tbl;
    int idx;
    unsigned int off;
    unsigned short x;
    unsigned short y;

    idx = *(short *)(iwram_3001ebc + (0xb6 << 1));
    tbl = L5d0;
    off = idx << 2;
    x = *(short *)(tbl + off);
    off += 2;
    y = *(short *)(tbl + off);
    __PlaySound(0x9e);
    __Func_8010560(L5e8, x, y);
    { register int q0 __asm__("r0"); register int q1 __asm__("r1");
      q0 = 0; q1 = 0; __Func_80922c4(q0, q1, -0x10); }
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    __Func_8091e9c(idx);
}

void OvlFunc_961_2008194(void)
{
    unsigned char *tbl;
    int idx;
    unsigned int off;
    unsigned short x;
    unsigned short y;

    idx = *(short *)(iwram_3001ebc + (0xb6 << 1));
    tbl = L5d0;
    off = idx << 2;
    x = *(short *)(tbl + off);
    off += 2;
    y = *(short *)(tbl + off);
    __PlaySound(0x9e);
    __Func_8010560(L5fe, x, y);
    { register int q0 __asm__("r0"); register int q1 __asm__("r1");
      q0 = 0; q1 = 0; __Func_80922c4(q0, q1, -0x10); }
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    __Func_8091e9c(idx);
}

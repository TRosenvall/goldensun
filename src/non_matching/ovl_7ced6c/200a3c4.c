/* OvlFunc_946_200a3c4 (0x0200a3c4) -- NON-MATCHING.
 *
 * NON-MATCHING, 20 of 62 STREAM encodings, NO PINS, NO DEVICES
 *   (IMPROVED FROM 29 THIS BATCH -- 343, brief B.)
 *   IN PHASE: 53 reference instructions against 53, size equal, so the 20 is a
 *   TRUE DISTANCE and not a misalignment.
 *   RELOCATIONS: 8 symbols, same list and same order on both sides; exactly ONE
 *   offset differs -- `OvlFunc_946_2009774` at ref 0x46 against ours 0x4c -- and
 *   the other seven agree exactly, so the relocation state is a 6-byte internal
 *   displacement of ONE call that heals, not an unresolved symbol.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7ced6c/200a3c4.c \
 *     asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_a_c_c.s \
 *     --func OvlFunc_946_200a3c4
 *
 * RECIPE REPOINTED.  The previous header named
 * `.../ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_a_c.s`, which a later
 * split consumed; the live file is `..._a_c_c_a_c_c.s`.  `tools/piece.py` reports
 * ONE function in it, so this park SHIPS ALONE with no split and no stage1.ld edit.
 *
 * ============ WHAT CHANGED: THE TWO-REGISTER COIN FLIP IS CLOSED ============
 *
 * ONE TOKEN did it -- `e = a - 1;` into a FRESH LOCAL instead of `a -= 1;`.
 * 29 of 62 -> 20 of 62, and the first differing index moves from 6 to 15, i.e.
 * the `asrs r5` / `asrs r6` pair and every later reference to them now agree:
 * `a` takes r5 and `b` takes r6 exactly as the ROM has them.
 *
 * This is the route batch 340 brief A landed on BOTH surviving members of the
 * callee-saved coin flip in this same bank (ovl_7ced6c).  The mechanism, stated
 * as that batch found it: the same `sub r5,#1` is emitted either way, but the
 * ALLOCNO SET is completely different -- the long-lived value drops to two
 * references and DIES AT THE NEW PSEUDO'S DEFINITION, so it stops conflicting
 * with it, and the short dense pseudo is the one that takes r5.
 *
 * >> THE PREVIOUS PARK'S DIAGNOSIS 1 IS THEREFORE REFUTED.  It read: "The two
 *    `>> 20` values land in r5/r6 the opposite way round from the ROM ...
 *    Thirteen spellings are screened across that pair -- all six declaration
 *    orders, the reused local split, THE DECREMENT IN THREE POSITIONS AND TWO
 *    FORMS, the stack arguments named at one site and both, the fetches inlined
 *    and through a named pointer -- and none moves it."  The decrement in a
 *    FRESH LOCAL moves it.  Declaration order is separately confirmed inert
 *    here: `b`/`a` swapped on top of this edit is 20, the same number. <<
 *
 * ============ WHAT THE REMAINING 20 IS: BLOCK ORDER, NOT SPELLING ============
 *
 * Counted off both streams:
 *
 *   ROM block order   [b==0x13 arm][b==0x12 arm][negs + call][b==0xf test][TAIL]
 *   ours              [b==0x13 arm][b==0x12 arm][b==0xf test][negs + call][TAIL]
 *
 * and the two visible defects are ONE FACT, coupled by length:
 *
 *   (A) the b==0x13 arm.  ROM 7 instructions, ours 6:
 *         ROM   subs r3,#3 / cmp r3,#2 / bhi L / movs r2,#16 / b negs
 *                                             / L: movs r2,#64 / b negs
 *         ours  subs r3,#3 / movs r2,#16 / cmp r3,#2 / bls negs
 *                                             / movs r2,#64 / b negs
 *       gcc puts `movs r2,#16` in the DOMINATOR block (it is dead on the other
 *       path, which overwrites r2) and inverts the branch, saving one `b`.
 *   (B) the b==0xf test.  ROM 2 instructions, ours 3:
 *         ROM   cmp r6,#15 / beq EPILOGUE      and FALLS THROUGH into TAIL
 *         ours  cmp r5,#15 / bne TAIL / b EPILOGUE
 *       Ours cannot fall through because the negs+call block sits between the
 *       0xf test and TAIL.
 *
 * -1 from (A) and +1 from (B) is why the streams are 53/53 and the figure is a
 * distance.  So the residue has the right COUNT and the wrong DISTRIBUTION.
 *
 * ======== REFUTED: THE ARM SPELLING IS A FALSE LEVER.  SEVEN SPELLINGS
 * ======== COMPILE TO A BIT-IDENTICAL OBJECT. ========
 *
 * Crossed 7 arm forms x 2 block orders = 14 rows this batch.  All seven arm
 * forms, on this body, are 20 of 62 / 53/53 / first index 15 / size equal --
 * the same number in every column, i.e. BYTE-EQUAL, not merely equal-scoring:
 *
 *     the park's `if (<=2) v = 0x10; else v = 0x40;`              20
 *     `do { } while (0);` at the head of EACH inner arm           20
 *     `do { } while (0);` after each assignment instead           20
 *     explicit per-arm `goto wide` / `goto apply`                 20
 *     `v = 0x10; if (> 2) v = 0x40;`  (gcc's own shape)           20
 *     the ternary `v = (<=2) ? 0x10 : 0x40;`                      20
 *     a separate `int n` / `int m` per arm, then `v = n` / `v = m` 20
 *
 * The nested `do { } while (0);` placements are worth calling out: autosweep's
 * `boundaries()` (tools/autosweep.py:217) already sweeps EVERY statement
 * boundary at ANY nesting depth, so they were not a new axis -- but they are now
 * measured individually on the POST-coin-flip body, which the sweep was not.
 *
 * ======== AND THE BLOCK-ORDER AXIS IS MEASURED SHUT: EVERY WAY OF PUTTING THE
 * ======== CALL BLOCK BEFORE THE 0xf TEST COSTS AN INSTRUCTION ========
 *
 * THREE source shapes that emit the ROM's block order were tried, each crossed
 * with three arm forms (9 rows).  All nine land on the same two numbers:
 *
 *   `} else goto other;  apply: call; goto tail;  other: if (0xf) return;`
 *                                            46 of 62, 53/52 insns, 140/136 B
 *   the 0x13 arm does `goto apply` INTO the 0x12 block, 0xf as the else-if
 *                                            46 of 62, 53/52 insns, 140/136 B
 *   the same with an explicit `goto tail` and the 0xf test standalone
 *                                            46 of 62, 53/52 insns, 140/136 B
 *   `if (b == 0x13 || b == 0x12) { ... }` with the call inside
 *                                            48-49 of 62, 53/54 insns
 *
 * >> SO MOVING THE CALL BLOCK FIXES (B) AND IMMEDIATELY LOSES THE INSTRUCTION
 *    BACK: 52 instead of 53.  Making the 0x12 arm adjacent to the negs block
 *    lets it fall through, which removes OUR `b` -- so to reach the ROM you need
 *    BOTH halves at once: the call block adjacent to the 0x12 arm AND the 0x13
 *    arm un-hoisted.  Neither half alone is a step toward it.  The arm half is
 *    the one with no known door, because all seven spellings are byte-equal,
 *    which means a pass canonicalises them before the layout is decided. <<
 *
 *   >> NAMED REMAINING QUESTION, and it is narrower than the park's two:
 *      WHICH PASS MOVES `movs r2,#16` OUT OF THE CONDITIONAL ARM AND INTO ITS
 *      DOMINATOR?  It is not spelling (7 byte-equal rows) and it is not
 *      scheduling (`-fno-schedule-insns2` is 41 of 62, far worse).  The next
 *      agent should read `.00.rtl` and `.09.jump`/`.13.combine` on this body and
 *      name it with a file and line, rather than screening more spellings.
 *      gcc-2.96 has no `-freorder-blocks`, so the LAYOUT follows RTL emission
 *      order and the question really is which pass rewrites the RTL. <<
 *
 * BOTH CHEAP AXES ARE CLOSED (tools/autosweep.py, batch 343):
 *   FLAG axis    all 8 per-file groups inert or worse on the parked body:
 *                -O1 44, -fno-schedule-insns2 41, everything else 29 (inert).
 *   ANCHOR axis  15 of 16 boundaries measured, all inert at 29; 1 did not
 *                compile, so that single placement is UNTESTED, not inert.
 * Neither was re-crossed with the `e = a - 1;` body for the FLAG axis -- that
 * crossing is the one cheap thing still open here, and autosweep's own docstring
 * says a flag inert on the parked body may not be inert on a changed one.
 *
 * WHAT IS RIGHT and must be kept: the three `>> 20` reads and their call order,
 * the range tests spelled `(unsigned int)(x - 3) <= 2` (which is what gives the
 * ROM's `subs r3,#3 / cmp r3,#2 / bhi`), the `negs r2,r2` sharing one call, the
 * b == 0xf arm returning without the call, the two six-argument tails sharing
 * `a - 1` as both the first register argument and the [sp] slot, and now the
 * r5/r6 assignment.
 *
 * HARNESS NOTE for whoever reads a side-by-side here: `tools/objcmp.py` appends
 * `.text / .align 2, 0` to the candidate (objcmp.py:440-441) so the candidate's
 * trailing pad is ZERO-filled to match the reference's.  A hand-rolled dumper
 * that omits that append shows a spurious final row (ref `.short 0x0000` against
 * ours `46c0 nop`).  That row is the harness, not the function.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_200a3c4(void)
{
    int a;
    int b;
    int d;
    int v;
    int t;
    int e;

    a = *(int *)(__MapActor_GetActor(0x11) + 8) >> 20;
    b = *(int *)(__MapActor_GetActor(0x11) + 0x10) >> 20;
    d = *(int *)(__MapActor_GetActor(0x13) + 8) >> 20;
    if (b == 0x13) {
        if ((unsigned int)(d - 3) <= 2)
            v = 0x10;
        else
            v = 0x40;
    } else if (b == 0x12) {
        if ((unsigned int)(d - 3) <= 2)
            return;
        v = 0x30;
    } else {
        if (b == 0xf)
            return;
        goto tail;
    }
    OvlFunc_946_2009774(0x11, 0, -v);
tail:
    __WaitFrames(2);
    t = *(int *)(__MapActor_GetActor(0x11) + 0x10) >> 20;
    e = a - 1;
    __Func_8010704(e, b, 3, 1, e, t);
    __Func_8010704(0, 0, 3, 1, e, b);
}

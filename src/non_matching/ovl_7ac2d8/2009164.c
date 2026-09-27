/* OvlFunc_924_2009164 -- NON-MATCHING, 12 encodings of 205.  205 instructions against 205,
 * 476 bytes against 476, EVERY RELOCATION IDENTICAL.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7ac2d8/2009164.c \
 *     asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a_c_a.s --func OvlFunc_924_2009164
 *
 * ITS .s HOLDS ONLY THIS FUNCTION AND IS CLEAN OF DATA, SO 12 ENCODINGS ARE ALL THAT STAND
 * BETWEEN IT AND A WHOLE-FILE CONVERSION.  Worth another agent's time.
 *
 * The six-int-struct-by-value shape reproduced on the FIRST screen at 16 of 205.
 *
 * BLOCKER: a local-alloc tie in `thumb_expand_movstrqi`'s pointer pair -- six sites, each
 * {r2,r3} in one order or the other.  ROM `r2 r2 r3 r2 r2 r2`, ours `r2 r2 r3 r3 r3 r3`.
 *
 * THE LEVER (16 -> 12) AND THE RULE THAT PREDICTS ALL SIX SITES:
 *
 *   `int g;` ASSIGNED IN **BOTH** ARMS of case 10, holding __GetFlag's result.  Assigned
 *   twice it STOPS BEING a local-alloc quantity, so that block drops from four quantities
 *   to three.  Two separate locals, or naming only one site, are both 16 -- THE SHARING IS
 *   THE LEVER, NOT THE NAMING.
 *
 *   And read out of .17.lreg: **A BLOCK WITH EXACTLY THREE QUANTITIES (dst, src, two-word
 *   temp) GIVES dst = r3; FOUR OR MORE GIVES dst = r2.**  Verified BY CONSTRUCTION --
 *   adding one unrelated call to one arm flips that site and leaves its sibling alone.
 *
 * So the ROM's remaining three blocks each had a FOURTH QUANTITY THAT EMITS NO INSTRUCTION,
 * and no construct supplying one was found: `int id = 0x312;` is constant-propagated and
 * `0x310 | 1` folds.  That is the shape of the next attempt.
 *
 * Two further tells: `unsigned` on the switch selector (the `bcs` against 9), and gcc
 * SUBSTITUTING THE CASE VALUE for the field reload (`mov r1,#9` instead of
 * `ldr r1,[r6,#4]`) -- that is cse off the switch compare, NOT an assignment in the source.
 *
 * ALL SIX Makefile FLAG GROUPS ARE INERT ON THIS AXIS (-fno-schedule-insns2,
 * -fno-rerun-cse-after-loop, -fno-gcse, -fno-strength-reduce, -ffixed-r7,
 * -fno-strict-aliasing), so no per-file flag group reaches it either.
 */
/* PARK -- OvlFunc_924_2009164, asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a_c_a.s.
 *
 * 12 of 205 encodings, 476 bytes against 476, 205 instructions against 205, every
 * relocation identical at every offset.  This .s holds ONLY this function and no
 * data, so it WOULD convert whole -- it does not, on 12 encodings.
 *
 * WHAT THE FUNCTION IS.  A six-int struct filled through a pointer by
 * OvlFunc_924_2008758 and then passed BY VALUE to OvlFunc_924_20088ec six times --
 * four words in r0-r3 and the two-word tail block-copied into the argument area, the
 * shape src/overlays/rom_7bdeb0/ovl_1300_c_c_b.c already records.  This reproduced
 * with no help on the first screen: 16 of 205.
 *
 * TWO THINGS ARE WORTH KEEPING FROM THE RECONSTRUCTION:
 *
 *  1. `unsigned b` -- THE SWITCH SELECTOR IS UNSIGNED, and `bcs` against 9 is the
 *     tell, exactly as docs/elevation.md records.  Cases 9/10/11 with a signed int
 *     give `bge`.
 *
 *  2. gcc SUBSTITUTES THE CASE VALUE FOR THE FIELD RELOAD.  The ROM reloads
 *     s.a/s.c/s.d from the frame at each call but writes `mov r1, #9` / `#0xa` /
 *     `#0xb` for s.b, because cse knows the switch compare proved s.b's value in
 *     that arm.  That is NOT an assignment in the source; plain
 *     `OvlFunc_924_20088ec(s);` in every arm produces it.  Worth knowing before
 *     anyone invents `s.b = 9;`, which would emit a store the ROM does not have.
 *
 * THE BLOCKER IS A LOCAL-ALLOC TIE IN THE BLOCK-MOVE POINTER PAIR, and it is a new
 * named mechanism.  `thumb_expand_movstrqi` makes two pseudos, dst then src; six
 * sites each get {r2,r3} in one order or the other:
 *
 *     ROM   r2 r2 r3 r2 r2 r2      (only the case-10 THEN arm is dst=r3)
 *     ours  r2 r2 r3 r3 r3 r3
 *
 * 4 instructions per wrong site, 16 encodings; with the lever below, 12.
 *
 * THE LEVER THAT MOVED IT, and the only one of ~20 that did: `int g;` ASSIGNED IN
 * BOTH ARMS of case 10 to hold __GetFlag's result, rather than testing the call
 * directly.  Assigned twice, it can no longer be a local-alloc QUANTITY (the
 * "assign a value twice to push it out of local-alloc into global-alloc" entry), so
 * the case-10 THEN block drops from four quantities to three and its pair flips to
 * the ROM's r3.  16 -> 12, and the first difference moves from index 94 to 156.
 * Two separate locals g/g2, or naming only one of the two sites, are both 16 --
 * THE SHARING IS THE LEVER, NOT THE NAMING.
 *
 * AND THE RULE THAT PREDICTS ALL SIX SITES, read out of the `.17.lreg` dump:
 *
 *     A BLOCK WITH EXACTLY THREE QUANTITIES (dst, src, and the two-word temp)
 *     GIVES dst = r3.  A BLOCK WITH FOUR OR MORE GIVES dst = r2.
 *
 * Verified by construction: adding one unrelated call to the case-11 THEN arm flips
 * that site from r3 to r2 and leaves its sibling alone.  So the ROM's blocks 15, 17
 * and 18 each had a FOURTH quantity that emits no instruction, and I could not find
 * a source construct that supplies one: `int id = 0x312; __SetFlag(id);` is
 * constant-propagated away, and `0x310 | 1` folds.
 *
 * MEASURED INERT (single drops, all 16 or worse): a named `int t` for s.c in case
 * 11, a named `int ok` for the 2008758 result, a shared `int k` for the three
 * `>> 20` tests (in every subset), a named `int one` for the CopyMapTiles pair
 * (function-scope is 189, catastrophic), named shift operands for the 200bc48
 * calls, unused extra locals (optimised away, so not even a pseudo), and splitting
 * the callee into `(int,int,int,int,struct{int,int})` instead of one six-int struct
 * by value (48).
 *
 * FLAGS ALL INERT ON THIS AXIS -- the six-site pattern is unchanged under
 * -fno-schedule-insns2, -fno-rerun-cse-after-loop, -fno-gcse, -fno-strength-reduce,
 * -ffixed-r7 and -fno-strict-aliasing.  So no Makefile flag group reaches it either.
 */
struct S { int a; unsigned b; int c, d, e, f; };

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern int __GetFlag(int id);
extern int OvlFunc_924_2008758(struct S *s);
extern void OvlFunc_924_20088ec(struct S s);
extern void OvlFunc_924_200bc48(int a, int b, int c, int d);
extern void OvlFunc_924_20090c0(void);

void OvlFunc_924_2009164(void)
{
    struct S s;
    int g;

    __CutsceneStart();
    if (OvlFunc_924_2008758(&s)) {
        switch (s.b) {
        case 9:
            if ((s.e >> 20) == 8) {
                OvlFunc_924_20088ec(s);
                __CutsceneWait(0x14);
                __CopyMapTiles(0x77, 9, 0x6d, 0xb, 1, 1);
                OvlFunc_924_200bc48(0x2d60000, 0, 0xb4 << 16, 0x80 << 8);
                __SetFlag(0xc4 << 2);
            } else {
                __CopyMapTiles(0x75, 9, 0x68, 7, 1, 1);
                __CopyMapTiles(0x77, 8, 0x6d, 0xb, 1, 1);
                __CopyMapTiles(0x76, 8, 0x68, 0xd, 1, 1);
                OvlFunc_924_20088ec(s);
                __ClearFlag(0xc4 << 2);
            }
            break;
        case 10:
            if ((s.e >> 20) == 0xc) {
                OvlFunc_924_20088ec(s);
                __CutsceneWait(0xa);
                g = __GetFlag(0xc4 << 2);
                if (g) {
                    __CopyMapTiles(0x76, 9, 0x68, 0xd, 1, 1);
                    OvlFunc_924_200bc48(0xa1 << 18, 0, 0xd2 << 16, 0x80 << 7);
                }
                __SetFlag(0x311);
            } else {
                __CopyMapTiles(0x77, 8, 0x6d, 0xb, 1, 1);
                g = __GetFlag(0xc4 << 2);
                if (g) {
                    __CopyMapTiles(0x77, 9, 0x6d, 0xb, 1, 1);
                    __CopyMapTiles(0x76, 8, 0x68, 0xd, 1, 1);
                }
                OvlFunc_924_20088ec(s);
                __ClearFlag(0x311);
            }
            break;
        case 11:
            if ((s.c >> 20) == 0x28) {
                OvlFunc_924_20088ec(s);
                __SetFlag(0x312);
            } else {
                OvlFunc_924_20088ec(s);
                __ClearFlag(0x312);
            }
            break;
        }
        OvlFunc_924_20090c0();
    }
    __CutsceneEnd();
}

/* OvlFunc_968_2009af0 -- park.  The subject line must stay FIRST: piece.py,
 * autosweep.py, shortrefs.py and scratchfill.py identify a park by line 1.
 *
 * NON-MATCHING, 13 of 262 stream encodings.  INSTRUCTIONS 223 / 223 (equal, so
 * this is a TRUE DISTANCE and not a phase measurement), size silent, and
 * RELOCATIONS IDENTICAL.  Re-derived with tools/objcmp.py in batch 341:
 *     XX ENCODINGS differ in 13 place(s) (ref 262, ours 262)
 *        [STREAM length, pool+pad INCLUDED; instructions are 223/223]
 *        first at index 80: ref ae04  ours 468b
 * PRODUCTION-FLAG FIGURE: this stem matches only the generic `asm/%.o: src/%.c`
 * rule -- no O1, no per-file flag group.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7f2f14/2009af0.c \
 *     asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.s --func OvlFunc_968_2009af0
 *
 * SHIMS: 12 register pins (shimcount: `12  via PIN1, PIN3, PIN3`), DOWN FROM 15
 * in batch 341 at the identical figure -- see THE PIN LADDER below.  A landing
 * still costs a fakematch.txt row, so by the standing pin policy this HOLDS as a
 * park and is pass-3 depin material (reports/pass3-depin.md).
 * Function split only; datacheck.py is silent, no new exports.
 *
 * ===== BATCH 341: A DUPLICATE PARK FILE RETIRED, AND THE PIN LADDER MEASURED ==
 *
 * (A) THERE WAS A SECOND COPY OF THIS PARK, at
 *     `src/non_matching/ovl_7b0400/2009af0.c`, and batch 341 retired it.  It was
 *     a near-verbatim copy: strip comments and the two files are CODE-IDENTICAL
 *     (2985 characters each), it names THIS function and THIS reference .s in its
 *     own `Verify with:` recipe, and `tools/piece.py` run on it resolves to
 *     `asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.s`.  The directory was simply
 *     wrong: the function at 0x02009af0 in overlay rom_7b0400 is
 *     `OvlFunc_925_2009af0` (asm/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_a_a_c.s,
 *     lines 8-1909, a ~1900-line MapActor_SetPos cutscene), an ENTIRELY DIFFERENT
 *     function that has no park at all.  Its batch-316 content is merged below so
 *     nothing is lost.  THE LESSON: a park in `src/non_matching/<ovl_X>/<addr>.c`
 *     whose recipe names a DIFFERENT overlay's .s is a misfiled file, not a twin,
 *     and `grep -n '_<addr>' asm/overlays/<ovl_X>/` settles it in one command.
 *
 * (B) THE PIN LADDER -- each pin group dropped from the body ONE AT A TIME,
 *     objcmp encodings against this reference (base 13 of 262, dsize 0, rel ok):
 *
 *       drop `v` pin   (r2, the a->f12 read)        13   dsize 0  rel ok  INERT
 *       drop `e1` pin  (r2, CopyMapTiles arm 1)     13   dsize 0  rel ok  INERT
 *       drop the __SetFlag PIN1 block               13   dsize 0  rel ok  INERT
 *       drop `e2` pin  (r1, CopyMapTiles arm 2)     16   dsize 0  rel ok
 *       drop `w` pin   (r3)                         17   dsize 0  rel ok
 *       drop `m` pin   (r5)                         23   dsize 0  rel ok
 *       drop the __GetFlag PIN1 block               25   dsize 0  RELOCDIFF
 *       drop the second __Func_8012330 PIN3 block   65   dsize -4
 *       drop `f` pin   (r11)                       171   dsize -4
 *       drop the first __Func_8012330 PIN3 block   199   dsize -4
 *       drop `u` pin   (r5)                        242   dsize 0
 *       ---- aggregates ----
 *       all four PIN blocks dropped                210   dsize -8
 *       all seven declaration pins dropped         212   dsize -8
 *       FULLY PIN-FREE                             240   dsize -12  RELOCDIFF
 *
 *     THE PIN-FREE FIGURE IS 240 OF 262 AND IT IS NOT A DISTANCE: dsize -12, so
 *     the pin-free body is SIX INSTRUCTIONS SHORT and the 240 is measuring
 *     misalignment.  (The older header's "pin-free is 255 of 262" was a figure
 *     for a different draft; 240 is this body's.)  So there is no pin-free
 *     landing here and the pin-free number is evidence about the blocker only.
 *
 *     THE THREE INERT PINS CROSS CLEANLY -- `v` + `e1` + the __SetFlag block
 *     together still measure 13 of 262, dsize 0, relocations identical, and
 *     `e1` can then be deleted outright (its argument written as the literal 3)
 *     for another 13.  That is the body installed here: 12 pins, one local fewer,
 *     same figure.  Crossing one more off regresses: + `e2` is 16, + `w` is 17.
 *     CORRECTION to the old header, which read "a pinned local for the FIFTH
 *     argument at each of the two sites, e1->r2 and e2->r1 (the ROM's own
 *     registers): 16 -> 13".  Only e2's pin does that work; e1's pin is
 *     decoration, and e1's LOCAL is decoration too.
 *
 * (C) AUTOSWEPT, BOTH CHEAP AXES CLOSED (tools/autosweep.py, batch 341).  Flag
 *     axis, unchanged body: -fno-gcse / -fno-strict-aliasing /
 *     -fno-strength-reduce / -fno-rerun-cse-after-loop / -fno-rerun-loop-opt all
 *     13; -fno-schedule-insns2 81; -ffixed-r7 206; -O1 223.  Anchor axis: 38 of
 *     40 `do { } while (0);` placements measured, all 13 or worse.  TWO OF THE 40
 *     PRODUCED NO ROW, so that sliver of the axis is UNTESTED, not inert.
 *
 * (D) REGION (1) HAS A NEW NEGATIVE.  The first diff, 2 encodings at index
 *     80-81, is the ROM emitting `add r6, sp, #0x10` (the address of `s`) BEFORE
 *     `mov r11, r1` (the `f = 4` high-register copy, which is two insns in Thumb
 *     because there is no `mov rH, #imm`), where we emit them the other way
 *     round.  `&s` is only used inside the loop, so the obvious handle is to give
 *     it an earlier expand order with a pointer local -- `struct Cfg *cp = &s;`
 *     before `f = 4`, passing `cp` to OvlFunc_968_2008118.  ALL FIVE PLACEMENTS
 *     REGRESS AND THEY ADD CODE: 199 of 262 at dsize +12 (six EXTRA instructions)
 *     for placements before `f = 4`, before `i = 0`, before `z = ...` and after
 *     `f = 4`; 201 at dsize +16 before `t = 0x28`.  The pointer becomes a pseudo
 *     that must stay live across the loop's four calls, so it takes a
 *     callee-saved register and pays for the push.  Region (1) is now closed from
 *     two directions: the r6 pin (batch 316, position-swept) and the pointer
 *     local (batch 341).
 *
 * ===== BATCH 316: THE SUPPORT IS A PROOF RATHER THAN A LIST ====================
 * The 13 sit in three regions and none of them has a source handle left:
 *   (1) 2 encodings, idx 80-81 -- see (D) above; the r6 pin lever the region was
 *       shaped for is position-swept and FAILS because the pin register is
 *       contested across the loop's four calls.
 *   (2) 6 encodings, idx 156-170 -- at each __CopyMapTiles site the `f` copy goes
 *       to r3 (ROM) or r2 (gcc) in the first arm and r2 (ROM) or r3 (gcc) in the
 *       second, and the ROM emits the copy BEFORE the `sub r5, #4`, gcc after.
 *       Keeps this header's extensive negatives, below.
 *   (3) 5-6 encodings, idx 178-183 -- NOT "allocator temps".  BOTH VALUES ARE
 *       CONST_INT OPERANDS THAT NEVER BECOME PSEUDOS AT ALL, so there is no
 *       allocno, no preferred class, no live range and no REG_EQUIV for any
 *       source-level lever to act on.  A structural impossibility argument, the
 *       same mechanism as src/non_matching/ovl_7aa430/200a030.c's region (b).
 *
 * TWO LEVERS WORTH REUSING ELSEWHERE.  if/else ARM POLARITY is fixed by the
 * ROM's branch direction (`bhi` means the source tests the other way first), and
 * writing it backwards cost 11 of 35 encodings while LOOKING like a dozen
 * unrelated register diffs; and the `neg`-built mask means a BITFIELD
 * (`q[9] = (q[9] & ~0xc) | 8` folds to `mov #243`, where a bitfield gives the
 * ROM's `mov #0xd / neg`).
 *
 * WHAT IT DOES.  A 480-frame particle/dust emitter cutscene, gated on the player
 * standing in an 8-unit-wide strip: `(unsigned)(ent->fa - 0x134) <= 7` and
 * `0x214 <= ent->f12 < 0x21c`, with save bit 0x300 unset.  It sets 0x300, swaps a
 * map tile block, then runs 0x1e0 frames in which each frame scrolls a map
 * pointer's +8 word by +0x3333, walks an emitter height `z` DOWN by 0x3333, and
 * spawns one particle via OvlFunc_968_2008118 with two random 0.2-step offsets
 * and a random 12-bit angle.  Two discrete events punctuate it: at frame 0xf0
 * exactly, `z` drops by a further 0x300000; and every 0x28 frames a tile strip is
 * copied, marching LEFT by 4 while frame <= 0xf0 and RIGHT by 4 after -- so the
 * effect sweeps out and back.  It closes by snapping the scrolled word to a whole
 * 16.16 unit, two __Func_8010704 calls, two sounds, a fade, and state 0x202.
 *
 * REFERENCE: asm/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_a.s, 1st of 2 functions
 * (the other is OvlFunc_968_2009d48, also parked, at 40 of 206).  No
 * `.section .data`, no `.incbin`; tools/datacheck.py prints nothing and exits 0.
 * A landing therefore needs a FUNCTION SPLIT ONLY, with no new exports, and
 * OvlFunc_968_2009d48 stays as assembly in its own object keeping its slot.
 *
 * HOW THE 13 WAS REACHED, from the landed near-twin
 * src/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_a.c (OvlFunc_968_200a6f8, EXACT),
 * which is the same emitter with different constants:
 *   * `int` locals for the two `ldrsh` reads, pinned u->r5, v->r2, w->r3:
 *     first diff index 16 -> 41.  Without the int locals the range fold stays in
 *     HImode; the pins put the callee-saved set right.  (v's pin is now known
 *     INERT -- see the ladder.)
 *   * PIN3 on BOTH __Func_8012330 sites: 255 -> 260 encodings.  The ROM builds
 *     `0x80 << 9` THREE separate times (six instructions); gcc CSEs it to one
 *     register and two copies.
 *   * `f = 4` as a LOCAL PINNED TO r11 for __CopyMapTiles' sixth argument:
 *     260 -> 262, WHICH IS WHERE THE COUNT BECAME EXACT.  r11 is contested: gcc
 *     otherwise hoists the loop-invariant 0xcccc into r11 and rematerialises `f`,
 *     where the ROM holds 4 in r11 and pool-loads 0xcccc TWICE inside the loop.
 *     `m` is pinned to r5 alongside -- the same register `u` uses earlier, the
 *     twin's trick of reusing one pin register for two disjoint live ranges.
 *   * PIN1 on __GetFlag (the __SetFlag one is now known INERT): 35 differing ->
 *     17.  The ROM rematerialises `0xc0 << 2` at both call sites; gcc CSEs it
 *     into r5 and thereby also displaces `m`.
 *   * THE IF/ELSE ARMS WERE WRITTEN BACKWARDS and that alone was 11 of the 35.
 *   * `i = 0;` written OUT of the for-init so `f = 4` is the last setup
 *     statement: 17 -> 16.
 *   * a pinned local for __CopyMapTiles' FIFTH argument at the SECOND site,
 *     e2->r1: 16 -> 13.  (The first site's e1 is decoration -- batch 341.)
 *
 * MEASURED INERT OR WORSE, so do not re-try (24 builds, batch 305; plus batch
 * 341's ladder and pointer-local sweep above):
 *   * a second pinned local per site for the `f` COPY: all nine register
 *     combinations, 13 at best and 115-122 at worst.
 *   * pinning the loop variables to the ROM's registers -- t->r9 and z->r8 are
 *     exactly inert (13); i->r7 REGRESSES to 181 differing and 260 encodings, and
 *     all three together to 244.  The loop variables are already where the ROM
 *     puts them and pinning them only disturbs the temps.
 *   * `d = -1;` before the loop with `t += d` in the for-increment, `d` pinned to
 *     r1, r2 or r3: ALL THREE regress to 104 differing AND lose the exact count
 *     (260).  Taking `t--` out of the for-increment undoes the twin's
 *     `for (i = ...; i++, t--)` lever.
 *
 * BATCH-305d ADDENDUM -- THE VARIABLE-REUSE LEVER DOES NOT REACH REGION (2), and
 * the reason is the boundary condition on that lever.  This park declares donors
 * pinned to exactly the two registers region (2) wants, with live ranges disjoint
 * from the loop (`w` on r3 and `v` on r2, both dead after the outer test), so
 * reusing them needs no new pin:
 *     w = f; ... __CopyMapTiles(m, 0x32, 0xf, 0x20, e1, w);   arm 1 only   13
 *     v = f; ... __CopyMapTiles(m, 0x2d, 9,   0x20, e2, v);   arm 2 only   13
 *     both arms together                                                   13
 *     both arms, the arm-2 assignment after `e2 = 3`                       13
 *     the donors SWAPPED (v on arm 1, w on arm 2)                         121
 * The first four are BYTE-IDENTICAL to the installed park.  gcc COALESCES the
 * assignment straight into the argument store and discards the donor's pin,
 * because the donor's only use is the argument itself.  RULE: the reuse-a-variable
 * lever needs the donor to have a use of its OWN; a variable written only to be
 * passed once is coalesced away and inherits nothing.  Region (3) additionally
 * cannot use the lever: the 0x1df bound is live across the whole loop and
 * conflicts with `e2` (r1), which is WRITTEN inside the loop.
 */
struct MapEnt {
    unsigned char pad00[0xa];
    short fa;
    int fc;
    unsigned char pad10[2];
    short f12;
};

struct Cfg {
    int f00;
    int f04;
    int f08;
    int f0c;
    unsigned char pad10[0x22 - 0x10];
    unsigned short f22;
    unsigned char pad24[4];
};

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

extern unsigned char *iwram_3001e70;
extern unsigned char *iwram_3001ebc;
extern unsigned int iwram_3001e40;

extern void *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8091e9c(int n);
extern unsigned int __Random(void);
extern void OvlFunc_968_2008118(int a, int b, int c, int d,
                                int e, int f, int g, struct Cfg *s);

void OvlFunc_968_2009af0(void)
{
    struct Cfg s;
    struct MapEnt *a;
    unsigned char *p;
    register int u __asm__("r5");
    int v;
    register unsigned int w __asm__("r3");
    unsigned int i;
    int z;
    int t;
    register int m __asm__("r5");
    register int f __asm__("r11");
    register int e2 __asm__("r1");
    int g;

    p = iwram_3001e70 + (0xb2 << 1);
    a = __MapActor_GetActor(0);
    u = a->fa;
    v = a->f12;
    a->fc = 0;
    w = u - 0x134;
    if (w <= 7 && v >= 0x85 << 2 && v < 0x87 << 2) {
        a->fc = 0xfffe0000;
        { PIN1; q0 = 0xc0 << 2; u = __GetFlag(q0); }
        if (!u) {
            __CutsceneStart();
            __PlaySound(0xa1);
            __SetFlag(0xc0 << 2);
            __CopyMapTiles(0x1a, 0x21, 0x13, 0x21, 1, 1);
            __CutsceneWait(0x1e);
            __PlaySound(0xef);
            { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 9; q2 = 0x80 << 9;
              __Func_8012330(q0, q1, q2); }
            __CutsceneWait(0x14);
            z = 0x90 << 17;
            m = 0x1d;
            t = 0x28;
            i = 0;
            f = 4;
            for (; i <= 0x1df; i++, t--) {
                *(int *)(p + 8) += 0x3333;
                z -= 0x3333;
                s.f00 = 2;
                s.f08 = (__Random() * 3 >> 16) * 0x3333 + 0xcccc;
                s.f0c = (__Random() * 3 >> 16) * 0x3333 + 0xcccc;
                s.f22 = (__Random() * 0x1000 >> 16) + (0xf8 << 8);
                OvlFunc_968_2008118(z, 0, 0x84 << 18, 0,
                                    -((iwram_3001e40 & 1) * 3 << 16), 0,
                                    0x8a << 16, &s);
                if (i == 0xf0)
                    z -= 0x300000;
                if (t == 0) {
                    t = 0x28;
                    if (i <= 0xf0) {
                        m -= 4;
                        __CopyMapTiles(m, 0x32, 0xf, 0x20, 3, f);
                    } else {
                        m += 4;
                        e2 = 3;
                        __CopyMapTiles(m, 0x2d, 9, 0x20, e2, f);
                    }
                }
                __WaitFrames(1);
            }
            *(int *)(p + 8) += 0x80 << 8;
            *(int *)(p + 8) = *(int *)(p + 8) / 0x10000 * 0x10000;
            g = 0x20;
            __Func_8010704(0xf, 0x20, 3, 1, 9, g);
            __Func_8010704(0xc, 0x20, 3, 1, 0xf, g);
            __PlaySound(0x90 << 1);
            __PlaySound(0xbc);
            { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
              __Func_8012330(q0, q1, q2); }
            __Func_8012350();
            *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
            __Func_8091e9c(0xb);
            __CutsceneEnd();
        }
    }
}


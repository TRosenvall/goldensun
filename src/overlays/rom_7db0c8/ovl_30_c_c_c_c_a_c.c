/* OvlFunc_954_20093e4  --  0x020093e4    EXACT
 *
 * Whole-file conversion of goldensun/asm/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_c.s,
 * which holds this ONE function and no data (tools/split_s.py: "holds only
 * OvlFunc_954_20093e4 and no data; convert it directly, no split needed").
 * 201 lines / 202 encodings / 508 bytes.  Cutscene script: the gState 0xe1
 * guard, __CutsceneStart, a three-way dispatch on OvlFunc_common1_4cc(param, 2),
 * a four-leg two-actor walk driven by __Actor_TravelTo, and the shared
 * OvlFunc_common1_5e4 / __CutsceneEnd tail.
 *
 * VERDICT
 *   OK OvlFunc_954_20093e4 -- 508 bytes, 202 encodings and 48 relocations identical
 *
 * SELECTION.  tools/solved_twins.py reports zero for the whole remaining set
 * (2089 solved shapes, 1536 remaining), so this was written by IDIOM instead.
 * The nominated template src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c_b.c
 * supplied the frame (gState guard, the `g` local, the 4cc dispatch, the tail)
 * but NONE of its per-site cures apply: it pins eight blocks and seventeen
 * registers, and this function ships TWO blocks and FOUR registers.  Its
 * `__Func_80933f8` reading in particular is actively wrong here.
 *
 * The body idiom came from grepping the corpus for the CONSTANTS, not the
 * position: `src/non_matching/ovl_7d0e88/200a1ac.c` carries the exact travel
 * preamble --
 *     *(int *)(a + 0x34) = 0x6666;
 *     *(int *)(a + 0x30) = 0xcccc;
 *     __Actor_TravelTo(a, *(int *)(a + 8), K, *(int *)(a + 0x10));
 * -- including the 0x34-before-0x30 source order, which is what the ROM's
 * `str r3,[rX,#0x34]` / `str r6,[rX,#0x30]` pair wants at all four sites.  A
 * PARKED function was the right template for the idiom even though it is
 * blocked on something else entirely.
 *
 * READING THE PROLOGUE BY CONTENT.  `push {r5,r6,r7,lr}` plus r8 and r10
 * through the two-instruction dance is FIVE callee-saved registers -- r5, r6,
 * r7, r8, r10, which is the first five slots of gcc-2.96 thumb
 * REG_ALLOC_ORDER's call-saved sequence (r5, r6, r7, r8, r10, r9, r11) taken in
 * order.  Each one is a source variable:
 *     r5  = e1/e2  the slot-0xe actor, live across its own __Actor_TravelTo
 *     r6  = 0xcccc  the field-0x30 constant, gcc-hoisted, LOW so `str r6` works
 *     r7  = a       the parameter
 *     r8  = 0x6666  the field-0x34 constant, gcc-hoisted, HIGH so each use
 *                   costs a `mov r3, r8` first
 *     r10 = r       the OvlFunc_common1_4cc result, live to the last call
 * Nothing here needs a named constant: the ROM is showing gcc's OWN hoist of
 * two pool constants, so 0xcccc and 0x6666 ship as bare literals at all four
 * sites.  That is the opposite reading to the usual "repeated constant" park
 * and it is decided by the push list -- the ROM already pushes the registers
 * the hoist needs.
 *
 * THE LEVER THAT WAS WORTH 35 OF 201: A VARIABLE WITH DISJOINT LIVE RANGES
 * SHOULD BE TWO VARIABLES.  The slot-0xe actor is fetched twice, and each fetch
 * dies at its own __Actor_WaitMovement.  Written as ONE local it is a single
 * allocno spanning legs 2 through 4; its live_length is long, its priority
 * (floor_log2(n_refs) * n_refs / live_length) is low, and it lands one slot too
 * far down REG_ALLOC_ORDER -- which then displaces EVERY other callee-saved
 * value.  Two locals give two short disjoint ranges that share r5, and all five
 * registers snap into the ROM's assignment at once:
 *     one `e`   44 differing, with a=r6 r=r8 0xcccc=r5 0x6666=r10 e=r7
 *     e1 + e2    9 differing, every register exact
 * The slot-0xd actor needs NO such split: it never crosses a call, local-alloc
 * leaves it in r0 at both sites, and splitting it is measured inert.
 * (docs/elevation.md "A variable with DISJOINT live ranges should be two
 * variables" -- this is a five-register instance of it, larger than the
 * three-register one recorded there.)
 *
 * THE -1 IS CSE'd ACROSS TWO CALLS, AND ONE PIN AT THE FIRST SITE FIXES BOTH.
 * `__Func_80933f8` is called twice, each with -1 as argument 2.  gcc builds it
 * once (`mov r5,#1 / neg r5,r5`) and copies `mov r1,r5` at both sites; the ROM
 * rebuilds `mov r1,#1 / neg r1,r1` at each.  The recorded cure applies:
 *     { register int q0 __asm__("r0"); register int q1 __asm__("r1");
 *       q0 = 0x94 << 18; q1 = -1; __Func_80933f8(q0, q1, 0xf0 << 15, 1); }
 * r1 is call-clobbered and dead across the intervening `bl`s, so there is no
 * pseudo left for CSE to hand the second site, and the SECOND site ships as a
 * bare `__Func_80933f8(0x98 << 18, -1, 0xd8 << 16, 1)`.
 *
 * AND IT REFINES THE RECORDED BOUNDARY.  "THE FIRST-USE PIN RULE HAS A
 * BOUNDARY: ADJACENT SITES" says that when two calls using the same value sit
 * adjacent in ONE BASIC BLOCK, both need pinning.  These two sites ARE in one
 * basic block -- there is no branch anywhere between them -- and one pin is
 * enough.  What separates them is three intervening calls (__Func_8093530,
 * __CutsceneWait, __Func_80933d4).  So the discriminator is NOT the basic
 * block, it is ADJACENCY: an intervening call that clobbers the pinned hard
 * register is as good as a branch.  Measured both ways:
 *     pin the FIRST site only        EXACT
 *     pin the SECOND site only       EXACT   (see below -- and this one is
 *                                             the surprise)
 *     pin NEITHER                    21 differing -- the hoist returns
 *     pin BOTH                       EXACT, but the second pin is inert
 *
 * THE SECOND-SITE-ONLY PIN ALSO WORKS, AND IT NEEDS A DIFFERENT SPELLING.
 * NEW, and it does not contradict "pin the first use" so much as bound it: with
 * only the SECOND site pinned the whole-value `q1 = -1;` is NOT enough (22
 * differing -- the hoist survives); it takes the split build `q1 = 1; q1 = -q1;`
 * plus `q0 = 0x98; q0 <<= 18;`.  At the FIRST site the two-statement
 * whole-value form is exact.  Reading: breaking the EARLIEST occurrence
 * destroys the CSE class outright, so any spelling that lands the value in r1
 * suffices; breaking a LATER occurrence has to out-compete a class that already
 * exists, and only the per-instruction build does.  The first-site pin is what
 * ships because it is two statements against four.
 *
 * PINS, MINIMISED TO A FIXPOINT.  The first exact candidate had three pinned
 * blocks and seven pinned registers.  Three greedy rounds, re-measuring after
 * every drop, removed one whole block (the second __Func_80933f8), two
 * statements from the survivor, and the r0 pin at OvlFunc_common1_5e4.  What
 * ships is TWO blocks and FOUR pinned registers and nothing further is inert:
 * the cheapest surviving strip costs 2 differing.
 *
 * MEASURED-WORSE TABLE (all against the final source unless noted; the aligned
 * count is out of 201 instructions):
 *   spelling                                                        differing
 *   ---------------------------------------------------------------  -------
 *   one `e` local instead of e1/e2                                        37
 *   gState read without the `g` pointer local   197, and 3 INSTRUCTIONS SHORTER
 *   -1 bare at BOTH __Func_80933f8 sites (the hoist)                      21
 *   -1 pinned at the SECOND site only, `q1 = -1;` whole-value              22
 *   -1 pinned at the SECOND site only, no r0 pin, split build              4
 *   site-1 pin: drop the r0 pin (`q1 = -1` alone)                          2
 *   site-1 pin: drop the r1 pin (`q0 = 0x94 << 18` alone)                  2
 *   site-1 pin: `q1 = -1;` written BEFORE `q0 = 0x94 << 18;`               2
 *   site-1 pin: split build `q0=0x94; q1=1; q1=-q1; q0<<=18;`              2
 *   site-1 pin: full four-register transcription of all seven insns        6
 *   OvlFunc_common1_5e4: no pins at all      57, and 1 INSTRUCTION SHORTER
 *   OvlFunc_common1_5e4: drop the r1 pin     59, and 1 INSTRUCTION SHORTER
 *   OvlFunc_common1_5e4: drop the r2 pin     58, and 1 INSTRUCTION SHORTER
 *   OvlFunc_common1_5e4: keep the r0 pin as well                           3
 *
 * ALSO MEASURED INERT, recorded so the next reader does not re-derive them:
 * splitting the slot-0xd actor into two locals; writing `*(p + 0x55) = r;`
 * instead of `p[0x55] = r;`; pinning the second __Func_80933f8 site as well.
 *
 * THE `g` LOCAL IS LOAD-BEARING, as every function in this family records.
 * `extern unsigned char gState[]` plus `g = gState;` keeps the base in one
 * register and the 0xe1<<1 offset in another, which is what Thumb-1's
 * immediate-less `ldrsh` needs; folding it to `*(short *)(gState + (0xe1 << 1))`
 * collapses to one pool word and loses three instructions.
 *
 * SIBLING, NOT TWIN.  OvlFunc_956_2009c20
 * (asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c_c.s, 189 lines) has the
 * identical skeleton -- same prologue register set, same gState guard, same
 * 4cc three-way dispatch, the same 80933d4/80933f8/8093530 opening, the same
 * 1490/14f4/1550/__WaitFrames beat, and the same four-leg two-actor walk with
 * the 0x55 / 0x30 / 0x34 travel preamble -- but every constant differs
 * (0x20b2, 4cc(a,1), 0x98<<16, actors 0xa/0xb) and, decisively, its register
 * ROLES differ: the 4cc result lives in r6 and 0xcccc in r10, so its 0x30 store
 * costs a `mov r2, r10` where ours is a direct `str r6`.  Different allocation,
 * different local set: the levers here are candidates for it, not answers.
 *
 * LANDING.  No split.  The .s holds one function and no data at all -- no
 * `.section`, no `.data`, no `.word`, no `.incbin`.  Exactly TWO linker-script
 * lines name the .o, matched on the FULL PATH, and both are in
 * overlays/rom_7db0c8/overlay.ld:
 *         asm/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_c.o(.text)
 *     and asm/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_c.o(.data)
 * -- the .data one is a placeholder for a section this .o does not have, and it
 * sits between the ovl_30_c_c_c_c_a_b.o(.data) and ovl_30_c_c_c_c_b.o(.data)
 * lines.  Both must be repointed at src/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_c.o.
 * A grep of every .ld in the tree finds three same-BASENAME hits in rom_780898,
 * rom_7c5974 and rom_7a04ac; they are different files and must not be touched.
 *
 * NO FLAG GROUP.  The match is at plain GCC296_CFLAGS -O2.  The Makefile has no
 * rule mentioning rom_7db0c8, so nothing narrows to CSE_CFLAGS or GCSE_CFLAGS
 * here -- which is worth saying out loud because the -1 residue is exactly the
 * shape `-fno-rerun-cse-after-loop` is usually reached for, and it is NOT what
 * cures it.  Screened with tools/objcmp.py against the ORIGINAL asm path
 * (asm/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_c.s, with and without --func) and
 * against a scratch ref.s copy; all three agree, so no Makefile pattern rule is
 * biting.
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

void OvlFunc_954_20093e4(int a)
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
        r = OvlFunc_common1_4cc(a, 2);
        if (r == 0) {
            __MessageID(0x2090);
            __Func_80933d4(0xc0 << 10, 0xc0 << 7);
            { register int q0 __asm__("r0"); register int q1 __asm__("r1"); q0 = 0x94 << 18; q1 = -1;
              __Func_80933f8(q0, q1, 0xf0 << 15, 1); }
            __Func_8093530();
            __CutsceneWait(0x3c);
            __Func_80933d4(0xc0 << 9, 0xc0 << 6);
            __Func_80933f8(0x98 << 18, -1, 0xd8 << 16, 1);
            __Func_8093530();
            __ActorMessage(a, 0);
            OvlFunc_common1_1490(0x38, 0x40, 0);
            __CutsceneWait(0x3c);
            OvlFunc_common1_14f4(0xa0, 0x60, 0xa);
            __CutsceneWait(0x46);
            __ActorMessage(a, 0);
            OvlFunc_common1_1550();
            __WaitFrames(2);
            p = __MapActor_GetActor(0xd);
            p[0x55] = r;
            *(int *)(p + 0x34) = 0x6666;
            *(int *)(p + 0x30) = 0xcccc;
            __Actor_TravelTo(p, *(int *)(p + 8), 0x80 << 12, *(int *)(p + 0x10));
            e1 = __MapActor_GetActor(0xe);
            e1[0x55] = r;
            *(int *)(e1 + 0x34) = 0x6666;
            *(int *)(e1 + 0x30) = 0xcccc;
            __Actor_TravelTo(e1, *(int *)(e1 + 8), 0x80 << 14, *(int *)(e1 + 0x10));
            __Actor_WaitMovement(e1);
            __CutsceneWait(0x2d);
            p = __MapActor_GetActor(0xd);
            p[0x55] = r;
            *(int *)(p + 0x34) = 0x6666;
            *(int *)(p + 0x30) = 0xcccc;
            __Actor_TravelTo(p, *(int *)(p + 8), 0xc0 << 13, *(int *)(p + 0x10));
            e2 = __MapActor_GetActor(0xe);
            e2[0x55] = r;
            *(int *)(e2 + 0x34) = 0x6666;
            *(int *)(e2 + 0x30) = 0xcccc;
            __Actor_TravelTo(e2, *(int *)(e2 + 8), 0, *(int *)(e2 + 0x10));
            __Actor_WaitMovement(e2);
            __CutsceneWait(0xf);
            __ActorMessage(a, 0);
            OvlFunc_common1_1490(0x38, 0x40, 0);
            __CutsceneWait(0x1e);
            OvlFunc_common1_14f4(0xa0, 0x60, 0xa);
            __CutsceneWait(0x28);
            OvlFunc_common1_14f4(0x38, 0x40, 0xa);
            __CutsceneWait(0x46);
            __ActorMessage(a, 0);
            OvlFunc_common1_1550();
            __WaitFrames(2);
            __SetCameraTarget(0, 0);
            OvlFunc_common1_588(a, 2);
        } else if (r == 1) {
            __MessageID(0x208f);
            __ActorMessage(a, 0);
        }
        { register int q1 __asm__("r1"); register int q2 __asm__("r2");
          q1 = a; q2 = 2; OvlFunc_common1_5e4(r, q1, q2); }
        __CutsceneEnd();
    }
}

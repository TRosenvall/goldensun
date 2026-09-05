/* OvlFunc_956_200a0f0  --  0x0200a0f0    EXACT
 *
 * Cut out of goldensun/asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c.s.
 * 187 instructions / 191 encodings / 468 bytes.  Cutscene script: the gState
 * 0xe1 guard, __CutsceneStart, a three-way dispatch on
 * OvlFunc_common1_4cc(param, 4), then a four-leg walk driven by
 * __Actor_TravelTo, and the shared OvlFunc_common1_5e4 / __CutsceneEnd tail.
 *
 * VERDICT
 *   OK OvlFunc_956_200a0f0 -- 468 bytes, 191 encodings and 41 relocations identical
 *
 * SELECTION.  Written from src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_b.c
 * (OvlFunc_956_2009df8), the solved sibling in the SAME overlay and the same
 * cutscene frame, not from the nominated rom_7ddb88 template.  Same-overlay
 * siblings carry the right declaration set and the right guard idiom; the
 * rom_7ddb88 header's per-site prototype findings (OvlFunc_common1_1078
 * undeclared, __Func_80921c4 declared) do NOT transfer -- every callee here is
 * declared with a full prototype and the result is exact.  RE-DERIVE, never
 * copy, an inherited "callees wanting r0 last" list.
 *
 * READING THE PROLOGUE BY CONTENT.  `push {r5,r6,r7,lr}` plus r8-r11 saved
 * through the three-instruction dance is SEVEN callee-saved registers, and each
 * one is a source variable, not scratch:
 *     r5  = e   the actor pointer from __MapActor_GetActor
 *     r6  = x   e[8],  updated IN PLACE before the last travel
 *     r7  = a   the parameter
 *     r8  = y   e[0xc], updated IN PLACE before the third travel
 *     r9  = y2  a COPY of y plus 0xc0<<11
 *     r10 = x2  a COPY of x plus 0x80<<15
 *     r11 = r   the OvlFunc_common1_4cc result, live to the very last call
 * So this is a WIDE push that is still a pin function: the wide push is paid
 * for by real locals and the fills still need pinning.  Declaring exactly these
 * seven and nothing else reproduces the push list on the first screen.
 *
 * THE COPY-THEN-MODIFY TELL, BOTH COORDINATES.  For x and for y alike the ROM
 * first derives a NEW register (`mov r2,#0xc0 / lsl r2,#11 / add r2,r8 /
 * mov r9,r2`) and later updates the original IN PLACE (`add r8, r2`).  That is
 * two named values for the first move and `y += K;` for the second -- the
 * recorded reading.  MEASURED CAVEAT (new): the pure-expression form, with x2
 * and y2 deleted and `x + (0x80 << 15)` written out at both of its uses, is
 * ALSO byte-identical here; gcc CSEs the repeat back into r10.  The named form
 * is kept because it mirrors the ROM's registers and does not depend on that
 * CSE, but the tell is weaker than "two named values are required".
 *
 * THE ONE REAL FIGHT: __MapActor_SetSpeed(0, 0x80<<8, 0x80<<7) APPEARS TWICE,
 * WITH THE SAME ARGUMENTS, AND THE TWO SITES WANT OPPOSITE SPELLINGS.
 *   site 2 (before __MapActor_GetActor):
 *       mov r1,#0x80 / mov r2,#0x80 / lsl r1,#8 / lsl r2,#7 / mov r0,#0
 *     -> { PIN23; q1 = 0x80; q2 = 0x80; q1 <<= 8; q2 <<= 7; SetSpeed(0,q1,q2); }
 *        the SPLIT build, one statement per instruction, r0 left unpinned.
 *   site 3 (interleaved with the two actor loads):
 *       mov r1,#0x80 / mov r2,#0x80 / lsl r2,#7 / mov r0,#0 / lsl r1,#8
 *     -> { PIN2; q0 = 0; q1 = 0x80 << 8; SetSpeed(q0, q1, 0x80 << 7); }
 *        the WHOLE-VALUE build with `q0 = 0;` written FIRST and r2 unpinned.
 * Cross-applying either spelling to the other site costs: site-3 spelling at
 * site 2 = 108 differing; site-2 spelling at site 3 = 2 differing.
 *
 * WHY, AND THIS IS THE MECHANISM THAT COST THE ROUND.  Site 3 is scheduled
 * against `ldr r3,[r5,#0xc]` and `ldr r6,[r5,#8]`, which changes the ready
 * list.  Two independent orders have to come out right and the levers for them
 * pull against each other:
 *   - the SEED movs sort by the position of each seed's FIRST CONSUMER.  With
 *     the split build, `lsl r2,#7` precedes `lsl r1,#8`, so r2's seed sorts
 *     first and you get `mov r2 / mov r1` -- backwards.
 *   - the OPS then follow written statement order.
 * The whole-value spelling `q1 = 0x80 << 8; q2 = 0x80 << 7;` splits each
 * constant into mov+shift after expand, which puts BOTH seeds at depth 2 where
 * they tie and break by argument order -- `mov r1 / mov r2`, correct -- but it
 * also surrenders control of the ops and emits `lsl r2 / lsl r1 / mov r0`
 * where the ROM has `lsl r2 / mov r0 / lsl r1`.  THE STATEMENT THAT DECIDES IT
 * IS `q0 = 0;`: written LAST it is the last depth-1 op; written FIRST it lands
 * between the two shifts and the site is exact.  Eighteen spellings were
 * measured for this one block; only `q0 = 0; q1 = 0x80 << 8; q2 = 0x80 << 7;`
 * reaches zero.  NEW FINDING, and it sharpens the recorded rule "where the
 * seeds tie, statement order is the lever": here the tie is MANUFACTURED by
 * moving to the whole-value spelling, and the zero is then placed by hand.
 *
 * NEW, AND IT REOPENS A CLOSED BUCKET.  docs/elevation.md "Argument-setup
 * order: the zero interleaved into a shifted build" records this exact
 * instruction sequence -- `mov r1,#0x80 / mov r2,#0x80 / lsl r2,#7 /
 * mov r0,#0 / lsl r1,#8` -- and gives ONE cure: name the two shifted constants
 * in a dominating block with A BRANCH between the assignments and the call, so
 * gcc rematerialises them at the use.  It then sizes the population at 248
 * functions, of which "98 are straight-line at every site and are, for now, out
 * of reach."  Site 3 here is straight-line: it sits deep inside the single
 * `r == 0` basic block with no branch to dominate from, and the branch cure is
 * unavailable.  IT IS STILL REACHABLE.  The straight-line cure is a PINNED
 * WHOLE-VALUE FILL with the zero written FIRST --
 *     { PIN2; q0 = 0; q1 = 0x80 << 8; SetSpeed(q0, q1, 0x80 << 7); }
 * -- pins on r0 and r1 only, the third argument left as a bare literal.  That
 * is a second, independent cure for the shape and it does not need a branch,
 * so the 98-function "out of reach" bucket should be re-screened.
 *
 * A BARRIER IS THE WRONG TOOL AT AN INTERLEAVED SITE.  The obvious cure for
 * "seed r1 needs an earlier consumer" is a volatile-asm barrier on q1.  It
 * costs 7 differing wherever it is placed in this block, because it also
 * reschedules the two `ldr`s that share the block.  Prefer the spelling.
 *
 * THE OTHER __Func_80933f8 SITE NEEDED NOTHING.  The sibling
 * ovl_30_c_c_c_c_a_c_b.c cures its `mov r0/mov r1/mov r2/mov r3/lsl r2/neg r1/
 * lsl r0` fill with two barriers.  The IDENTICAL shape here
 * (0xd6<<18, -1, 0xa8<<16, 1) is exact as a BARE CALL with whole-value
 * arguments; adding the sibling's PIN4 + two barriers is not merely inert, the
 * pinned split spelling COSTS 2 differing.  A SIBLING'S CURE FOR A
 * SAME-SHAPED BLOCK CAN BE ACTIVELY WRONG -- confirmed again.
 *
 * PINS, MINIMISED TO A FIXPOINT.  The first exact candidate had 11 pinned
 * blocks / 33 pinned registers.  Four greedy rounds, re-measuring after every
 * drop and re-testing the survivors after each round, removed three whole
 * blocks (__Func_80933d4, the first __Func_80933f8, the closing
 * __ActorMessage) and eight individual register pins.  What ships is 8 blocks
 * and 17 pinned registers, and the last round found nothing further inert:
 * the cheapest surviving strip costs 2 differing, the dearest 150.
 * "N PINS IS A SIZE, NOT A SET" -- the per-register pass mattered as much as
 * the per-block one, and it only became visible AFTER the block pass:
 *   __Func_80921c4 needs r0 pinned and NOTHING else (dropping r1's pin was
 *   inert only once r2's pin had gone, so re-test drops after every edit).
 *
 * MEASURED-WORSE TABLE (all against the final source unless noted; the
 * function is 191 encodings, so a count is out of 191):
 *   spelling                                                        differing
 *   ---------------------------------------------------------------  -------
 *   site-3 SetSpeed: split build, ops in ROM order (first candidate)        2
 *   site-3 SetSpeed: q1=0x80 q2=0x80 q0=0 q2<<=7 q1<<=8                     2
 *   site-3 SetSpeed: q2/q1 seeds swapped in source                          2
 *   site-3 SetSpeed: whole-value with q0=0 written LAST                     2
 *   site-3 SetSpeed: whole-value with q0=0 written in the middle            2
 *   site-3 SetSpeed: volatile-asm barrier on q1, any placement              7
 *   site-3 SetSpeed: q2 = 0x80<<7 first, q1 = 0x80<<8 last                  4
 *   site-3 SetSpeed: no pins at all                                         2
 *   site-3 SetSpeed: drop the r1 pin                                        2
 *   site-2 SetSpeed: whole-value spelling (site-3's)                      108
 *   site-2 SetSpeed: no pins at all                                       112
 *   site-2 SetSpeed: drop the r2 pin                                      108
 *   site-1 SetSpeed: no pins at all           130, and 4 BYTES / 2 ENCODINGS LONGER
 *   site-1 SetSpeed: drop the r0 pin                                        3
 *   site-1 SetSpeed: drop the r1 pin                                        4
 *   __Func_80933f8 (0xd6<<18,..): PIN4 + 2 barriers, split build            0
 *   __Func_80933f8 (0xd6<<18,..): PIN4, split build, NO barriers            2
 *   __Func_80933f8 (-1,-1,-1,0): no pins  150, and 4 BYTES / 2 ENCODINGS SHORTER
 *   __Func_80933f8 (-1,-1,-1,0): drop the r0 pin                           40
 *   __Func_80933f8 (-1,-1,-1,0): drop the r2 pin                           44
 *   OvlFunc_common1_1078: no pins                                           2
 *   OvlFunc_common1_1078: drop the r1 pin                                   2
 *   OvlFunc_common1_1078: drop the r2 pin                                   2
 *   __Func_80921c4: no pins                                                 2
 *   __Func_80921c4: drop the r0 pin (keep r1)                               2
 *   __Func_8092adc: no pins                                                 2
 *   __Func_8092adc: drop the r2 pin                                         2
 *   OvlFunc_common1_5e4: no pins                                            3
 *   OvlFunc_common1_5e4: drop the r2 pin                                    2
 *   OvlFunc_common1_5e4: drop the r1 pin                                    3
 *   gState read without the `g` pointer local  168, and 8 BYTES / 4 ENC SHORTER
 *
 * INERT, AND THEREFORE NOT SHIPPED (each measured exact, and re-measured exact
 * with the others already removed):  PIN2 on __Func_80933d4; PIN4 plus two
 * volatile-asm barriers on the first __Func_80933f8; PIN2 on the closing
 * __ActorMessage; the r0 pin at OvlFunc_common1_1078, __Func_8092adc and the
 * second __MapActor_SetSpeed; the r3 pin at the second __Func_80933f8; the r0
 * pin at OvlFunc_common1_5e4; the r2 pin at the third __MapActor_SetSpeed and
 * at __Func_80921c4; the r1 pin at __Func_80921c4.
 *
 * ALSO MEASURED INERT (source shape, not scaffolding), recorded so the next
 * reader does not re-derive them:  swapping the source order of the two actor
 * loads (`x` before `y`); writing `y = y + (0xd8 << 14);` instead of `y +=`;
 * deleting x2/y2 in favour of repeated expressions.
 *
 * THE `g` LOCAL IS LOAD-BEARING, as the sibling records.  `extern unsigned char
 * gState[]` plus `g = gState;` keeps the base in one register and the 0xe1<<1
 * offset in another, which is what Thumb-1's immediate-less `ldrsh` needs;
 * folding it to `*(short *)(gState + (0xe1 << 1))` collapses to one pool word,
 * loses 8 bytes and 168 encodings.
 *
 * LANDING NEEDS A SPLIT.  asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c.s holds
 * TWO functions and no data at all -- no `.section`, no `.data`, no `.word`,
 * no `.incbin`:
 *     line   8 .thumb_func_start OvlFunc_956_2009f90   (ends line 151)
 *     line 157 .thumb_func_start OvlFunc_956_200a0f0   (ends line 350)
 * Their local labels are disjoint (.L1fae/.L205a/.L206e/.L20b2/.L20c4/.L20d2
 * against .L2116/.L212a/.L2284/.L2298/.L22a6), so no label export is needed.
 * Exactly ONE linker-script line names the .o and it is a .text line:
 *     overlays/rom_7e0928/overlay.ld:57
 *         asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c.o(.text)
 * A grep of every .ld in the tree finds no other reference (the
 * same-basename hits are in rom_7a7298, rom_7d95dc, rom_7c097c and rom_77dd1c
 * and are different files -- match on the FULL PATH, never the basename).
 * So the split replaces that one line with, in order,
 *     asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c_a.o(.text)   (2009f90, asm)
 *     src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c_b.o(.text)   (200a0f0, this)
 * Both names are free in rom_7e0928.
 *
 * Screened with tools/objcmp.py against the ORIGINAL asm path
 * (asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c.s, --func) and against a
 * scratch ref.s cut of the function alone; the two agree, so no Makefile
 * pattern rule is biting.  No per-file flag rule exists for rom_7e0928: this
 * is plain GCC296_CFLAGS at -O2.
 */
extern unsigned char gState[];

extern void OvlFunc_common1_2c4(void);
extern int OvlFunc_common1_4cc(int a, int b);
extern void OvlFunc_common1_588(int a, int b);
extern void OvlFunc_common1_5e4(int a, int b, int c);
extern void OvlFunc_common1_1078(int a, int b, int c);
extern void OvlFunc_common1_1254(int a);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetCameraTarget(int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_TravelTo(unsigned char *e, int x, int y, int z);
extern void __Actor_WaitMovement(unsigned char *e);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_8093fa0(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN23 register int q1 __asm__("r1"); \
              register int q2 __asm__("r2")

void OvlFunc_956_200a0f0(int a)
{
    unsigned char *g;
    unsigned char *e;
    int r;
    int x, y, x2, y2;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
    } else {
        __CutsceneStart();
        r = OvlFunc_common1_4cc(a, 4);
        if (r == 0) {
            __MessageID(0x20bf);
            __Func_80933d4(0xc0 << 10, 0xc0 << 7);
            __Func_80933f8(0xd6 << 18, -1, 0xa8 << 16, 1);
            __Func_8093530();
            __CutsceneWait(0x1e);
            __ActorMessage(a, 0);
            __ActorMessage(a, 0);
            { PIN23; q1 = 0xcc; q1 <<= 2; q2 = 0xc8;
              OvlFunc_common1_1078(0, q1, q2); }
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8;
              __MapActor_SetSpeed(q0, q1, q2); }
            { register int q0 __asm__("r0"); q0 = 0;
              __Func_80921c4(q0, 0xd2 << 2, 0xc8); }
            { PIN23; q1 = 0xc0; q1 <<= 8; q2 = 0x14;
              __Func_8092adc(0, q1, q2); }
            __Func_8093fa0();
            { PIN3; q0 = 1; q1 = 1; q2 = 1;
              q0 = -q0; q1 = -q1; q2 = -q2;
              __Func_80933f8(q0, q1, q2, 0); }
            { PIN23; q1 = 0x80; q2 = 0x80; q1 <<= 8; q2 <<= 7;
              __MapActor_SetSpeed(0, q1, q2); }
            e = __MapActor_GetActor(0);
            y = *(int *)(e + 0xc);
            x = *(int *)(e + 8);
            { PIN2; q0 = 0; q1 = 0x80 << 8;
              __MapActor_SetSpeed(q0, q1, 0x80 << 7); }
            __MapActor_SetAnim(0, 0xa);
            y2 = y + (0xc0 << 11);
            __Actor_TravelTo(e, x, y2, *(int *)(e + 0x10));
            __Actor_WaitMovement(e);
            __MapActor_SetAnim(0, 0xe);
            x2 = x + (0x80 << 15);
            __Actor_TravelTo(e, x2, y2, *(int *)(e + 0x10));
            __Actor_WaitMovement(e);
            __MapActor_SetAnim(0, 0xa);
            y += 0xd8 << 14;
            __Actor_TravelTo(e, x2, y, *(int *)(e + 0x10));
            __Actor_WaitMovement(e);
            __MapActor_SetAnim(0, 0xf);
            x += 0xc0 << 14;
            __Actor_TravelTo(e, x, y, *(int *)(e + 0x10));
            __Actor_WaitMovement(e);
            __MapActor_SetAnim(0, 0xc);
            __ActorMessage(a, 0);
            OvlFunc_common1_1254(0);
            __SetCameraTarget(0, 0);
            OvlFunc_common1_588(a, 4);
        } else if (r == 1) {
            __MessageID(0x20be);
            __ActorMessage(a, 0);
        }
        { PIN23; q1 = a; q2 = 4; OvlFunc_common1_5e4(r, q1, q2); }
        __CutsceneEnd();
    }
}

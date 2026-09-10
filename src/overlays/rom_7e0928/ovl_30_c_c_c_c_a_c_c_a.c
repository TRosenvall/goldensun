/* OvlFunc_956_2009f90  --  0x02009f90    EXACT
 *
 * Cut out of goldensun/asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c.s; it is
 * already split, and this is the `_a` half -- ONE function, NO data, so it
 * lands WHOLE at src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c_a.c with NO
 * linker-script edit (overlays/rom_7e0928/overlay.ld:57 already names
 * asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c_a.o(.text) and the Makefile's
 * cross-dir `asm/%.o: src/%.c` rule builds it from src/).  136 instructions /
 * 141 encodings / 352 bytes.
 *
 * VERDICT
 *   OK OvlFunc_956_2009f90 -- 352 bytes, 141 encodings and 32 relocations
 *   identical (measured 3x, plus 3x on an earlier identical spelling)
 *
 * SELECTION.  Written from its own file-mate
 * src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_c_b.c (OvlFunc_956_200a0f0), the
 * `_b` half of the SAME split -- same overlay, same cutscene frame, same
 * declaration set, same gState guard idiom.  That header's own findings carried
 * over almost verbatim: the `g = gState;` local, the guarded three-way dispatch
 * on OvlFunc_common1_4cc, and its __MapActor_SetSpeed cure
 * `{ PIN3; q1 = K; q2 = K; q0 = 0; q1 <<= a; q2 <<= b; }` transplanted EXACTLY
 * (0x80 -> 0xc0) and was right first time.  The near-twin route works again.
 *
 * THE TWO LEVERS THAT MATTERED, IN ORDER OF SIZE.
 *
 * 1.  CONTROL FLOW -- the wait loop is a GUARDED `do { } while`, NOT a `while`,
 *     and the guard's address expression must be WRITTEN OUT AGAIN rather than
 *     held in a `short *`.  Worth 140 -> 15 of 141 in one step.
 *
 *       if (*(short *)(w + (0xc1 << 1)) != 5) {
 *           do { __WaitFrames(1); r++; if (r > 0xef) break; }
 *           while (*(short *)(w + (0xc1 << 1)) != 5);
 *       }
 *
 *     The ROM computes `w + 0x182` THREE times -- r3 for the guard, r5 in the
 *     loop pre-header, r2 for the closing `strh` -- and holds only the BASE in
 *     r8.  That is exactly what this spelling gives, and the mechanism is worth
 *     recording because it is the opposite of the usual "name the value":
 *       * the loop body's copy sits in a block with TWO predecessors (entry and
 *         back-edge), so cse_main's extended basic block STOPS there and cse1
 *         cannot common it with the guard's copy.  loop.c then hoists it into
 *         the pre-header AFTER cse1 has run -- r5.
 *       * the `strh` after the loop is in a block with multiple predecessors
 *         too, so it gets a third computation -- r2.
 *     Naming a `short *p` in the pre-header instead lets cse1 common guard and
 *     loop into ONE register: 81 of 141 differing and, critically, the PUSH
 *     MASK loses r8 (4 callee-saved values become 3).  Pinning that `p` to a
 *     hard register to defeat the commoning is INERT (81, unchanged) -- the
 *     third pin device does NOT reach this shape.  MEASURED: `while` loop 132,
 *     named `short *p` 73, pinned `short *p` 81, this spelling 15.
 *
 *     > A PUSH THE ROM HAS AND YOU LACK, WHERE THE MISSING VALUE IS AN ADDRESS
 *     > THE ROM REBUILDS, IS A CONTROL-FLOW QUESTION BEFORE IT IS AN ALLOCATION
 *     > ONE.  Do not reach for a pin; put a basic-block boundary between the
 *     > two computations by spelling the loop as a guarded do-while.
 *
 * 2.  THE `strh` OF A LITERAL ZERO needs an `int` local at the TOP of the
 *     function -- the recorded lever, confirmed again.  `*(short *)(...) = 0;`
 *     pool-loads the zero (`ldrh r3, .Ln / .word 0`) and drags the whole pool
 *     mid-function, inserting a branch over it: 122 of 144 differing and FOUR
 *     BYTES LONGER than the ROM.  `int zero = 0;` at the top, stored through,
 *     gives the ROM's `mov r3, #0 / strh r3, [r2]` and returns the pool to the
 *     end of the function.  Declaring the zero in its own nested block next to
 *     the store instead costs 2 -- the "dominating block" half of the rule is
 *     real here, not just the type half.
 *
 * THE SIX ARGUMENT-SETUP SITES, AND THE ONE THAT BROKE THE PATTERN.  The
 * residue after the two levers above was 15 differing across six call sites,
 * all of it `mov r0` landing on the wrong side of a shift.  Five took the
 * obvious pin; the sixth did not:
 *
 *   site                         ROM                      pin that works
 *   ---------------------------  -----------------------  ----------------
 *   common1_1078(0,0xfc<<1,0xc8) r1 lsl r2 r0             q1;q1<<=;q2;  (-2)
 *   Func_8092adc(0,0,0)          r1 r2 r0                 q1;q2;q0;     (-3)
 *   SetSpeed(0,0xc0<<9,0xc0<<8)  r1 r2 r0 lsl lsl         q1;q2;q0;<<;<<(-3)
 *   MapActor_Emote(0,0x103,0x3c) r2 r0 ldr r1             q0;q1;q2;     (-2)
 *   common1_5e4(r,a,3)           r1 r2 r0                 q1=a;q2=3;    (-3)
 *   Func_8092adc(0,0xc0<<8,0x14) r1 r0 lsl r2             see below
 *
 * Func_8092adc's SECOND site is the interesting one.  The sibling's cure for
 * the identical (0, K<<8, imm) shape -- `{ PIN23; q1 = K; q1 <<= 8; q2 = imm; }`
 * with r0 unpinned -- is not merely inert here, it is WORSE: 16 against the bare
 * call's 15, and applied with the other five pins in place it leaves 3 rather
 * than 0.  What works is the WHOLE-VALUE build with the zero pinned and written
 * FIRST and the third argument left a bare literal:
 *
 *       { PIN2; q0 = 0; q1 = 0xc0 << 8; __Func_8092adc(q0, q1, 0x14); }
 *
 * Two other spellings also reach zero -- `q1 = 0xc0; q0 = 0; q1 <<= 8; q2 = 0x14`
 * and `q0 = 0; q1 = 0xc0; q1 <<= 8; q2 = 0x14` (both PIN3) -- while
 * `q1 = 0xc0 << 8; q0 = 0; q2 = 0x14` costs 2 and the PIN23 split costs 3.  So
 * the discriminating factor is again WHERE THE ZERO IS WRITTEN, not how the
 * shifted constant is built.  A SIBLING'S CURE FOR A SAME-SHAPED BLOCK CAN BE
 * ACTIVELY WRONG -- third independent confirmation.
 *
 * DIAGNOSTIC.  `-fno-schedule-insns2` REGRESSES at every stage -- 81 -> 96 on
 * the first structurally-right candidate and 0 -> 35 on the final source.  So
 * sched2 was producing the ROM's order throughout and alias was never the axis.
 * The flag is not shipped.
 *
 * PINS, MINIMISED TO A FIXPOINT.  The first exact candidate pinned 6 blocks /
 * 15 registers.  Two greedy rounds, re-measuring after every drop, removed six
 * individual register pins and nothing else; what ships is 6 blocks / 9 pinned
 * registers and the second round found nothing further inert.  "N PINS IS A
 * SIZE, NOT A SET" again, and sharply: at __MapActor_SetSpeed, dropping q1's
 * pin is inert and dropping q2's is inert, but q2's drop becomes WORSE (4) once
 * q1's has been taken -- a one-shot pass over the full set would have kept both.
 * Every one of the six BLOCKS is load-bearing (2 to 3 differing if removed).
 *
 * MEASURED TABLE (out of 141 encodings unless noted)
 *   spelling                                                      differing
 *   ------------------------------------------------------------  ---------
 *   first transcription, no levers                          140, 24 BYTES SHORT
 *   + guarded do-while + int zero                                        15
 *   wait loop as a plain `while`                            132, 28 BYTES SHORT
 *   wait loop with a named `short *p`                        73, 4 BYTES SHORT
 *   ... with that `p` pinned to r5                            81, 4 BYTES SHORT
 *   literal 0 in the closing strh                           122, 8 BYTES LONG
 *   that zero declared in its own nested block                            2
 *   `g = gState;` folded to *(short *)(gState + 0x1c2)      129, 4 BYTES SHORT
 *   counter bump written BEFORE __WaitFrames(1)                            2
 *   final source                                                          0
 *   final source with -fno-schedule-insns2                                35
 *
 * ALSO MEASURED INERT (recorded so the next reader does not re-derive them):
 * the third pin device on the loop pointer; `do { } while (0)` around the strh.
 *
 * LANDING.  asmfacts.py says `WHOLE  convert directly`.  No per-file Makefile
 * rule and no pattern rule reaches rom_7e0928 -- tryc.makefile_flags() returns
 * the empty set, so this is plain GCC296_CFLAGS at -O2 with -fcall-used-r4.
 * The only symbols referenced outside the TU are gState and iwram_3001ebc, both
 * absolute in wram.sym (0x02000240 and 0x03001EBC), both already used this way
 * by the `_b` sibling.  No `.L` label, no `.word`, no `.section`, no `.incbin`.
 * CAUTION: a file with this same BASENAME also exists under rom_77dd1c; the
 * linker lines must be matched on the FULL PATH.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;

extern void OvlFunc_common1_2c4(void);
extern int OvlFunc_common1_4cc(int a, int b);
extern void OvlFunc_common1_588(int a, int b);
extern void OvlFunc_common1_5e4(int a, int b, int c);
extern void OvlFunc_common1_1078(int a, int b, int c);
extern void OvlFunc_common1_1254(int a);
extern void OvlFunc_common1_1578(int a, int b, int c);

extern void OvlFunc_956_2008188(void);
extern void OvlFunc_956_20081b4(void);
extern void OvlFunc_956_20081c8(void);
extern void OvlFunc_956_2008b30(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetCameraTarget(int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __WaitFrames(int n);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);


void OvlFunc_956_2009f90(int a)
{
    unsigned char *g;
    unsigned char *w;
    int r;
    int zero;

    zero = 0;
    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
    } else {
        __CutsceneStart();
        r = OvlFunc_common1_4cc(a, 3);
        if (r == 0) {
            w = iwram_3001ebc;
            __MessageID(0x20bb);
            OvlFunc_956_2008188();
            __Func_80933d4(0xc0 << 10, 0xc0 << 7);
            __Func_80933f8(0x9a << 18, -1, 0xb8 << 16, 1);
            __Func_8093530();
            __CutsceneWait(0x1e);
            __ActorMessage(a, 0);
            OvlFunc_956_20081b4();
            __CutsceneWait(0x3c);
            __ActorMessage(a, 0);
            { int q1; register int q2 __asm__("r2"); q1 = 0xfc; q1 <<= 1; q2 = 0xc8;
              OvlFunc_common1_1078(0, q1, q2); }
            { int q0; register int q1 __asm__("r1"); register int q2 __asm__("r2"); q1 = 0; q2 = 0; q0 = 0;
              __Func_8092adc(q0, q1, q2); }
            OvlFunc_956_20081c8();
            { register int q0 __asm__("r0"); int q1; register int q2 __asm__("r2"); q1 = 0xc0; q2 = 0xc0; q0 = 0; q1 <<= 9; q2 <<= 8;
              __MapActor_SetSpeed(q0, q1, q2); }
            OvlFunc_common1_1578(0, 0xaa << 2, 0xc8);
            if (*(short *)(w + (0xc1 << 1)) != 5) {
                do {
                    __WaitFrames(1);
                    r++;
                    if (r > 0xef)
                        break;
                } while (*(short *)(w + (0xc1 << 1)) != 5);
            }
            OvlFunc_956_2008b30();
            { register int q0 __asm__("r0"); int q1; q0 = 0; q1 = 0xc0 << 8;
              __Func_8092adc(q0, q1, 0x14); }
            { register int q0 __asm__("r0"); int q1; int q2; q0 = 0; q1 = 0x103; q2 = 0x3c;
              __MapActor_Emote(q0, q1, q2); }
            __ActorMessage(a, 0);
            OvlFunc_common1_1254(0);
            __SetCameraTarget(0, 0);
            OvlFunc_common1_588(a, 3);
            *(short *)(w + (0xc1 << 1)) = zero;
        } else if (r == 1) {
            __MessageID(0x20ba);
            __ActorMessage(a, 0);
        }
        { register int q1 __asm__("r1"); register int q2 __asm__("r2"); q1 = a; q2 = 3; OvlFunc_common1_5e4(r, q1, q2); }
        __CutsceneEnd();
    }
}

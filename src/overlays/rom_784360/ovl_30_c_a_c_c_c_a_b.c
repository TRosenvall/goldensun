/* OvlFunc_884_2009084  --  0x02009084   [3rd of 6 in
 *   asm/overlays/rom_784360/ovl_30_c_a_c_c_c_a.s, lines 751-934 -- 177
 *   instructions of straight-line cutscene script, no branches]
 *
 *   OK OvlFunc_884_2009084 -- 496 bytes, 188 encodings and 50 relocations identical
 *
 * re-measured four times after narrowing and twice before; objcmp prints no
 * `(built with: ...)` line, so adjust=set() -- the tree default -O2 -mthumb
 * -mthumb-interwork -fcall-used-r4.  makefile_flags() on
 * src/overlays/rom_784360/ovl_30_c_a_c_c_c_a*.c is the EMPTY set and no `%`
 * pattern rule fires, so there is no wildcard hazard.  (The Makefile's only
 * rom_784360 rule names ovl_30_c_a_a_a_c_c_a_c_c_b.o literally.)
 *
 * LANDING: SPLIT FIRST.  tools/asmfacts.py says "6 functions  split first".
 * The .s carries NO .section .data, no .bss, no .lcomm and no .word -- every
 * `.L` symbol in it is a branch target, and a per-function check finds ZERO
 * cross-function label references, so tools/split_s.py can cut it cleanly and
 * nothing needs exporting.  Three pieces:
 *
 *     asm/overlays/rom_784360/ovl_30_c_a_c_c_c_a_a.s   2008940, 2008bbc
 *     src/overlays/rom_784360/ovl_30_c_a_c_c_c_a_b.c   2009084   <- this file
 *     asm/overlays/rom_784360/ovl_30_c_a_c_c_c_a_c.s   2009274, 20095b4, 20097c8
 *
 * ONE linker line names the object today,
 *
 *     overlays/rom_784360/overlay.ld:46   asm/overlays/rom_784360/ovl_30_c_a_c_c_c_a.o(.text)
 *
 * and it becomes THREE lines in order _a_a, _a_b, _a_c, every one of them
 * keeping the `asm/` prefix VERBATIM.  The build rule is `asm/%.o: src/%.c`, so
 * the middle object still lives at asm/overlays/rom_784360/ovl_30_c_a_c_c_c_a_b.o
 * even though its source is a .c; a line rewritten to `src/...o` matches nothing
 * and is SILENTLY IGNORED.  The suffixes are free under asm/overlays/rom_784360/
 * (only _a, _b and _c_b exist).  Verify `make compare` byte-neutral WITH ALL SIX
 * FUNCTIONS STILL IN ASSEMBLY before swapping the middle piece for the .c.
 *
 * READ THE PROLOGUE BY CONTENT.  `push {r5, r6, lr} / mov r6, r8 / push {r6}`,
 * and every saved register is a real value:
 *
 *     r5   __Func_8093554()'s pointer, then 0xb0 << 8   TWO disjoint roles
 *     r6   0x8017                                       a repeated script constant
 *     r8   0x2018                                       a second one
 *
 * hi = 6, hiv = 1.  Plain C is 155 of 188 differing at the SAME 496 bytes --
 * the length is right from the first candidate, so this is entirely an
 * allocation and ordering problem.
 *
 * SEVEN PINS OF FORTY-TWO, MINIMAL BY MEASUREMENT AND ALL NARROWED TO PIN2
 * (PIN1 at the one single-argument site).  All 42 pinnable sites were pinned
 * first, which is 6 differing, not 0 -- see the next paragraph; with the three
 * OvlFunc_884_200a2e0 sites released it is 0, and a greedy drop-to-fixpoint
 * then a width pass reduce it to seven.  A strict re-drop over the seven
 * removes none: each costs between 2 and 175 differing.
 *
 * THREE SITES MUST NOT BE PINNED, AND THEY ARE ONE CLASS.  The three
 * `OvlFunc_884_200a2e0(slot, K << n, delay)` calls emit
 * `mov r1 / mov r2 / lsl r1 / mov r0` in the ROM -- the r0 seed LAST, after the
 * shift.  An r0 pin necessarily puts `mov r0` first, so each costs 2 and the
 * three together are the entire 6-differing residue of the all-pinned form.
 * Unpinned, gcc's own order is the ROM's.  This is the recorded "a pin can be
 * worse than no pin" in its width-zero form: the correct width here is NONE.
 *
 * THE TWO SCRIPT CONSTANTS COME OUT IN THE WRONG REGISTERS, AND ONE HARD PIN
 * FIXES BOTH (see the note below).  `c1 = 0x8017` and `c2 = 0x2018` are both
 * live across the last third of the function; the ROM puts c1 in r6 and c2 in
 * r8, gcc does the reverse and also hoists c2's pool load one call early --
 * 26 differing with the pin set otherwise right.  `register int c1
 * __asm__("r6")` is exact.  Declaration order does not reach it (swapping
 * `int c1; int c2;` is 26, unchanged).
 *
 * r5's TWO ROLES MUST BE TWO LOCALS -- `p` for the __Func_8093554() pointer and
 * `s` for 0xb0 << 8.  Merging them is not even expressible here (different
 * types), but merging c1 and c2 into one recycled local IS, and it is 168
 * differing and TWELVE BYTES SHORT.  Same rule, same direction as the sibling
 * ovl_30_c_c_c_a_a_c_a_a.c records: the ROM sharing a register is the
 * ALLOCATOR, not the source.
 *
 * `p[0x55] = 0` IS A BARE ZERO, NOT A NAMED VALUE.  The ROM builds it in r3, a
 * scratch register (`mov r3, #0 / strb r3, [r5]`).  The sibling's 20091c4 has
 * the same store taking a NAMED local, and the discriminator there was that the
 * value survived a call in a callee-saved register.  Here it does not, so the
 * literal ships -- the two readings agree.
 *
 * `0x2018` IS WRITTEN TWICE, ONCE AS A LITERAL AND ONCE AS `c2`, AND GCC KEEPS
 * BOTH.  The ROM has two separate pool loads (`ldr r0, =0x2018` at the first
 * __ActorMessage and `ldr r3, =0x2018 / mov r8, r3` twelve instructions later);
 * writing the first as a bare literal and the second as the named local
 * reproduces that without a barrier of any kind.  Its pin is the r0-only one.
 *
 * WHAT NEEDED NOTHING.  `__Func_80933f8(0x80 << 15, 0x90 << 16, 0xaf << 17, 0)`
 * and its 0xda twin bare (seeds, then shifts, exactly as the sibling records);
 * `__Func_80921c4(0, 0x5d, 0x157)`; the eight __MapActor_DoAnim / SetAnim /
 * CutsceneWait calls; `s = 0xb0 << 8` placed between the first __ActorMessage
 * and __Func_80925cc(0x17, 2), which is where the ROM's `mov r5, #0xb0` sits --
 * gcc splits the build across the call by itself.
 *
 * MEASURED WORSE / INERT (against 188 encodings / 496 bytes):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   no pins, plain `int c1` (the plain reading)           155
 *   no pins, c1 pinned to r6                              155
 *   all 42 sites pinned, c1 pinned to r6                    6
 *   all 42 sites pinned, plain `int c1`                    26
 *   final 7 pins, plain `int c1`                           20
 *   final 7 pins, c1 and c2 merged into one local         168 (-12 bytes)
 *   drop the __Func_80933f8(-1,-1,-1,0) pin               168 (-4 bytes)
 *   drop the __Func_80933d4(0xc0<<10,0xc0<<7) pin         175 (+4 bytes)
 *   drop the __ActorMessage(0x2018,0) pin                  20
 *   drop any of the other four                          2 to 3
 *   pinning any ONE of the three OvlFunc_884_200a2e0 sites  2
 *   pinning all three                                       6
 *
 *   INERT (tie at 0, so the simpler form ships):
 *     `register int c2 __asm__("r8")` INSTEAD of the c1 pin
 *     both c1 and c2 pinned together
 *     the 35 dropped pins, individually or all together
 *
 * -- worked in scratch_elev/b252/zhi; gen2.py rebuilds any pin set,
 *    sweep_t2.py / narrow_t2.py / final_t2.py are the drop, width and
 *    confirmation passes.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __PlaySound(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_WaitMovement(int slot);
extern unsigned char *__Func_8093554(void);
extern void __Func_800fe9c(void);
extern void __Func_80917d0(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093530(void);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8095268(void);
extern void OvlFunc_884_2009274(void);
extern void OvlFunc_884_200a2c8(int a, int b);
extern void OvlFunc_884_200a2e0(int a, int b, int c);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_884_2009084(void)
{
    unsigned char *p;
    register int c1 __asm__("r6");
    int c2;
    int s;

    p = __Func_8093554();
    __CutsceneStart();
    { PIN2; q0 = -1; q1 = -1;
      __Func_80933f8(q0, q1, -1, 0); }
    __WaitFrames(1);
    __Func_80933f8(0x80 << 15, 0x90 << 16, 0xaf << 17, 0);
    __Func_800fe9c();
    __WaitFrames(1);
    __Func_80917d0(1, 0);
    __MapTransitionIn();
    __PlaySound(0x11);
    __Func_8095268();
    __MapActor_SetPos(0x17, 0xd2 << 15, 0x10b0000);
    __WaitFrames(1);
    { PIN2; q0 = 0; q1 = 0x13333;
      __MapActor_SetSpeed(q0, q1, 0x9999); }
    __Func_80921c4(0, 0x5d, 0x157);
    __MessageID(0xed6);
    __ActorMessage(0x17, 0);
    __PlaySound(0x3d);
    p[0x55] = 0;
    { PIN2; q0 = 0xc0 << 10; q1 = 0xc0 << 7;
      __Func_80933d4(q0, q1); }
    __Func_80933f8(0xda << 15, 0xb0 << 16, 0x1190000, 1);
    __Func_8093530();
    __CutsceneWait(0x28);
    { PIN2; q0 = 0x18; q1 = 0x87 << 16;
      __MapActor_SetPos(q0, q1, 0xb1 << 16); }
    { PIN2; q0 = 0x18; q1 = 0xcccc;
      __MapActor_SetSpeed(q0, q1, 0x6666); }
    { PIN2; q0 = 0x18; q1 = 0x7e;
      __Func_809218c(q0, q1, 0x81 << 1); }
    __CutsceneWait(0x28);
    __Func_8092adc(0x17, 0xd0 << 8, 0);
    __MapActor_WaitMovement(0x18);
    __MapActor_SetAnim(0x18, 1);
    OvlFunc_884_200a2e0(0x18, 0xe0 << 7, 0xa);
    __MapActor_DoAnim(0x17, 3);
    __MapActor_DoAnim(0x18, 4);
    c1 = 0x8017;
    { PIN1; q0 = 0x2018;
      __ActorMessage(q0, 0); }
    __Func_80925cc(0x17, 2);
    s = 0xb0 << 8;
    OvlFunc_884_200a2c8(c1, 0x1e);
    OvlFunc_884_200a2e0(0x18, s, 0x14);
    c2 = 0x2018;
    OvlFunc_884_200a2c8(c2, 0xa);
    OvlFunc_884_200a2e0(0x17, s, 0x28);
    __ActorMessage(c1, 0);
    __MapActor_DoAnim(0x18, 4);
    __ActorMessage(c2, 0);
    OvlFunc_884_200a2e0(0x17, 0xf0 << 8, 0xa);
    __Func_80925cc(0x17, 2);
    __ActorMessage(c1, 0);
    OvlFunc_884_200a2e0(0x18, 0xc0 << 7, 0x14);
    __MapActor_DoAnim(0x18, 3);
    __CutsceneWait(0x14);
    OvlFunc_884_200a2c8(c2, 0x14);
    OvlFunc_884_2009274();
    __CutsceneEnd();
}

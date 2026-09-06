/* OvlFunc_968_200ca2c
 *   [asm/overlays/rom_7f2f14/ovl_30_c_c_c_c.s, lines 502-682 of 1036.  THIS
 *   FILE DOES NOT LAND AS ONE .c AND A SPLIT IS UNAVOIDABLE.  It holds FIVE
 *   functions --
 *
 *     line   14  OvlFunc_968_200c610
 *     line  211  OvlFunc_968_200c7c0
 *     line  399  OvlFunc_968_200c968
 *     line  502  OvlFunc_968_200ca2c   <-- this one
 *     line  689  OvlFunc_968_200cbd8
 *
 *   -- AND a real `.section .data` at line 908 carrying 46 `.incbin` chunks and
 *   the overlay's exported script symbols (gScript_968__0200d21c and friends).
 *
 *   TWO .ld LINES NAME THIS .o, BOTH ON THE FULL PATH, AND BOTH MUST BE
 *   REMAPPED -- the second one is exactly the "a .data line for a section it
 *   does not have" hazard, except here the .o DOES have the data and the piece
 *   that keeps it is not the piece that keeps this function:
 *
 *     overlays/rom_7f2f14/overlay.ld:94   asm/.../ovl_30_c_c_c_c.o(.text)
 *     overlays/rom_7f2f14/overlay.ld:100  asm/.../ovl_30_c_c_c_c.o(.data)
 *
 *   No other .ld in the tree names `asm/overlays/rom_7f2f14/ovl_30_c_c_c_c.o`;
 *   the four `ovl_30_c_a_c_c_c_c*` lines at 71-74 of the same file are other
 *   objects and must not be touched.
 *
 *   LANDING SHAPE (tools/split_s.py, three ways, order preserved):
 *     asm/overlays/rom_7f2f14/ovl_30_c_c_c_c_a.s  200c610, 200c7c0, 200c968
 *     src/overlays/rom_7f2f14/ovl_30_c_c_c_c_b.c  THIS FILE
 *     asm/overlays/rom_7f2f14/ovl_30_c_c_c_c_c.s  200cbd8 + the whole .data
 *   and overlay.ld:94 becomes three lines in that order (`_a.o(.text)`,
 *   `src/..._b.o(.text)`, `_c.o(.text)`) while line 100 must be repointed to
 *   `asm/overlays/rom_7f2f14/ovl_30_c_c_c_c_c.o(.data)`.  LEAVING LINE 100
 *   ALONE SILENTLY DROPS EVERY SCRIPT IN THIS OVERLAY: after the split there is
 *   no ovl_30_c_c_c_c.o at all, and a linker script entry that matches no input
 *   section is not an error.  None of the three names collides -- asm/overlays/
 *   rom_7f2f14 currently holds only ovl_30_c_c_c_c.{s,o,d} for that stem.
 *   Run `make compare` after the split and BEFORE writing the .c: split_s is
 *   byte-neutral by construction, and a layout mistake and a bad decompilation
 *   look identical at the end.
 *
 *   (Solving all five would still not remove the split -- the .data has to stay
 *   in an .s either way -- but it would reduce it to a .text/.data cut.  The
 *   other four are untouched.)]
 *
 * EXACT, measured as a single-function extract (objcmp --func cannot isolate
 * one function inside a multi-function candidate, so this .c holds only
 * 200ca2c):
 *
 *   OK OvlFunc_968_200ca2c -- 428 bytes, 182 encodings and 29 relocations identical
 *
 * Holds against the real asm/ path and against a scratch copy of the same .s,
 * and objcmp prints no `(built with: ...)` line -- adjust=set(), the tree
 * default -O2 -mthumb -mthumb-interwork -fcall-used-r4.  NO FLAG GROUP.  The
 * only Makefile rules whose targets match this .o (or any of the three split
 * names) are the generic `%.o: %.s` (95), `%.o: %.c` (135) and `asm/%.o:
 * src/%.c` (146); every rom_7f2f14 wildcard in the Makefile is scoped to
 * `ovl_30_c_a_c_a_c_a%` or `ovl_30_c_a_c_a_c_c%` and cannot reach this stem.
 *
 * 178 instructions: two actor pointers fetched up front, a screen-shake played
 * out as five hand-unrolled position blocks and then a 128-iteration loop, and
 * a __SetFlag(0x101) tail.
 *
 * READ THE PROLOGUE BY CONTENT.  `push {r5, r6, r7, lr} / mov r7, r10 /
 * mov r6, r8 / push {r6, r7}` then `sub sp, #4` -- FIVE callee-saved registers
 * AND a four-byte frame.  The frame is the whole diagnosis, and it is what the
 * first draft got wrong:
 *
 *     r5   0x80 << 10, then 0x80 << 9   the shake step, live INTO the loop
 *     r6   __MapActor_GetActor(0x14)
 *     r7   __MapActor_GetActor(0)
 *     r8   that pointer + 0x55
 *     r10  a plain 0
 *     [sp] the loop counter, caller-saved around __WaitFrames in r2
 *
 * THE SPILL IS THE SPECIFICATION.  `str r2,[sp] / bl __WaitFrames / ldr r2,[sp]`
 * is the -fcaller-saves sequence, and `sub sp, #4` is its slot.  gcc emits it
 * only when the counter could not get a call-saved register -- i.e. only when
 * r5, r6 and r7 are ALL still occupied at the loop.  Any spelling in which the
 * shake step is dead by the loop gives the counter r5, rematerialises the
 * constant inside the loop body instead, and produces a 428-byte function with
 * NO stack frame that is 162 differing.  Same size, completely different
 * allocation.
 *
 * ------------------------------------------------------- THE TWO LEVERS -----
 *
 * 1. THE LOOP'S ADDEND MUST BE THE LITERAL, NOT THE NAMED LOCAL.  This is the
 *    exact inverse of the sibling file's "one materialisation is not one
 *    variable", and it reads backwards until the size is checked:
 *
 *      loop body written `p->fc += w;`            175 differing, TWO INSNS SHORT
 *      loop body written `p->fc += 0x80 << 9;`      0
 *
 *    With a named `w` used both before and inside the loop, the pseudo's live
 *    range covers everything from the fifth shake block to the loop exit; its
 *    global-alloc priority (log2(n_refs) * freq / live_length) loses to the
 *    counter's short, hot range, it gets no hard register, and reload notices
 *    REG_EQUIV and rematerialises `mov/lsl` inside the loop -- two insns in,
 *    two spill insns out, so the SIZE stays right and only the shape moves.
 *    Writing the addend as a literal at each of the six sites lets CSE build
 *    ONE pseudo whose live range starts at the fifth block, and that pseudo
 *    wins r5.  The sibling's diagnostic ("when the output is SHORT, something
 *    that should be live is missing") is what points at this; the cure here is
 *    to stop naming the value, not to start.
 *
 * 2. THE HIGH-REGISTER PINS ARE HARMFUL AND DELETING THEM IS THE LEVER.
 *    r8 and r10 hold `p + 0x55` and a plain 0 across the whole function, which
 *    is the textbook profile for `register unsigned char *q __asm__("r8")` +
 *    `register int z __asm__("r10")` -- the template's own recipe, transplanted
 *    from the recorded r8/r10 entries.  Measured:
 *
 *      both pins            154 differing (+2 insns)
 *      r8 pin only           66
 *      r10 pin only         154 (+2 insns)
 *      NEITHER                0
 *
 *    gcc's Thumb call-saved order is r5, r6, r7, r8, r10 and it reaches r8 and
 *    r10 unaided as soon as five values need saving; forcing them instead
 *    reserves the registers across the WHOLE function and pushes the allocator
 *    into r9 and into swapping r6/r7.  This is the recorded "it also makes
 *    register pins evaporate" corollary of the alias entry, arriving here from
 *    the register side: treat every template lever as SUFFICIENT NOT NECESSARY
 *    and re-measure it in place.
 *
 * WHAT IS ACTUALLY LOAD-BEARING BESIDES THOSE.  `z` is: a plain `int z = 0`
 * whose only uses are the three `f55` byte stores.  Replacing it with the
 * literal 0 at all three is 10 differing -- the ROM materialises 0 ONCE into a
 * scratch register, copies it to r10, stores that scratch to `f44`, and then
 * copies BACK from r10 for the byte store, which is one materialisation serving
 * two source objects.  `p->f44 = 0` (the literal) and `p->f55 = z` (the local)
 * is the split that reproduces it; `p->f44 = z` is inert.
 *
 * THREE PINS OF TWENTY-ONE, MINIMAL BY MEASUREMENT (all 21 pinned is also 0, so
 * the small set ships).  Greedy drop plus an ADD pass to a fixpoint from both
 * ends; a strict pass over the survivors drops none:
 *
 *   __Func_80933f8(-1, -1, -1, 0)      PIN4   dropping it costs 163
 *   __MapActor_Surprise(0, 0x101)      PIN2   dropping it costs 165
 *   __MapActor_Surprise(0, 0x80 << 1)  PIN2   dropping it costs   2
 *
 * The two __MapActor_Surprise sites both need a pin while the two
 * __Func_8092b08 sites, the two __PlaySound literals and every __CutsceneWait
 * need none.  Uniform whole-value ASCENDING fill at all three.
 *
 * DECLARATION ORDER IS COMPLETELY INERT HERE, and that is worth recording
 * because it is the cheapest thing to reach for when two pointers come out in
 * the wrong registers: ALL 5040 permutations of the seven locals measure the
 * same, with `p` in r6 every time.  The r6/r7 swap was a symptom of lever 1,
 * not of declaration order, and it disappeared with it.
 *
 * WHAT NEEDED NOTHING.  `__Func_80933f8` by a PIN4 ascending fill; the five
 * shake blocks as plain `-= 0x30000` / `+= 0x80 << 10` compound assignments in
 * the ROM's order (actor 0x14 first, then 0x0c and 0x14 of actor 0); the
 * function pointer store `p->f6c = OvlFunc_968_200c968;` as a bare
 * `void *` field; `p->f55 &= 0xfe;` for the read-and-store; the loop as a plain
 * `for (i = 0; i <= 0x7f; i++)` over an `unsigned int`.
 *
 * PROTOTYPES, 2 OF 14 LOAD-BEARING: dropping __MapActor_Surprise costs 4 and
 * __Func_8092b08 costs 2.  The other twelve are inert; __MapActor_GetActor's
 * is inert for CODEGEN but obviously kept, since it is what types `p` and `r`.
 *
 * MEASURED WORSE (against 182 encodings / 428 bytes):
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   no pins at all                                          177 (+2 insns)
 *   loop addend as the named local `w`                       175 (-2 insns)
 *   + register q __asm__("r8") and z __asm__("r10")          154 (+2 insns)
 *   + register z __asm__("r10") only                         154 (+2 insns)
 *   one recycled local for the two shake steps               149
 *   + register q __asm__("r8") only                           66
 *   the three f55 stores written as the literal 0              10
 *   dropping the pin at 80933f8 / Surprise(0,0x101)     163 / 165
 *   dropping the pin at Surprise(0, 0x80 << 1)                 2
 *
 *   INERT (tie at 0, so the plainer form ships):
 *     all 21 sites pinned instead of the minimal 3
 *     `unsigned char *q = &p->f55` instead of the `p->f55` field accesses
 *     the pre-loop shake block written with a named local
 *     `p->f44 = z` instead of `p->f44 = 0`
 *     every one of the 5040 declaration orders
 *
 * FLAGS: -fno-gcse, -fno-cse-follow-jumps, -fno-expensive-optimizations and
 * -fno-strict-aliasing are all BYTE-IDENTICAL, so unlike its rom_7d95dc
 * counterpart this TU does NOT depend on alias separation.  -fno-caller-saves
 * is 157 differing (it is what emits the spill), -fno-schedule-insns2 is 37 and
 * -fno-rerun-cse-after-loop is 44, so those two passes are live.
 */
struct A {
    unsigned char pad00[0xc];
    int fc;
    unsigned char pad10[4];
    int f14;
    unsigned char pad18[0x2c];
    int f44;
    int f48;
    unsigned char pad4c[9];
    unsigned char f55;
    unsigned char pad56[0x16];
    void *f6c;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __WaitFrames(int n);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __MapActor_Surprise(int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);
extern void __Func_8092b08(int slot, int a);
extern void __Func_8091e9c(int n);
extern void OvlFunc_968_200c968(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_968_200ca2c(void)
{
    struct A *p;
    struct A *r;
    int z;
    unsigned int i;

    p = __MapActor_GetActor(0);
    r = __MapActor_GetActor(0x14);
    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    p->fc = 0x82 << 16;
    p->f48 = 0x80 << 8;
    z = 0;
    p->f44 = 0;
    p->f55 = z;
    __MapTransitionIn();
    __WaitMapTransition();
    __PlaySound(0xcc);
    __CutsceneWait(0x1e);
    p->f55 = 3;
    __CutsceneWait(0x18);
    { PIN2; q0 = 0; q1 = 0x101;
      __MapActor_Surprise(q0, q1); }
    __MapActor_SetAnim(0, 0x16);
    p->f55 &= 0xfe;
    r->fc -= 0x30000;
    p->fc -= 0x30000;
    p->f14 -= 0x30000;
    __WaitFrames(2);
    r->fc -= 0x20000;
    p->fc -= 0x20000;
    p->f14 -= 0x20000;
    __WaitFrames(0xa);
    r->fc += 0x80 << 10;
    p->fc += 0x80 << 10;
    p->f14 += 0x80 << 10;
    __WaitFrames(4);
    r->fc += 0x80 << 10;
    p->fc += 0x80 << 10;
    p->f14 += 0x80 << 10;
    __WaitFrames(4);
    r->fc += 0x80 << 9;
    p->fc += 0x80 << 9;
    p->f14 += 0x80 << 9;
    p->f55 = z;
    r->f55 = z;
    { PIN2; q0 = 0; q1 = 0x80 << 1;
      __MapActor_Surprise(q0, q1); }
    __MapActor_SetAnim(0, 1);
    __CutsceneWait(0x28);
    p->f6c = OvlFunc_968_200c968;
    __CutsceneWait(0x3c);
    __Func_8092b08(0, 1);
    __Func_8092b08(0x14, 1);
    __PlaySound(0x11);
    __PlaySound(0x9a << 1);
    __SetFlag(0x101);
    for (i = 0; i <= 0x7f; i++) {
        p->fc += 0x80 << 9;
        p->f14 += 0x80 << 9;
        r->fc += 0x80 << 9;
        __WaitFrames(1);
    }
    __Func_8091e9c(0x15);
}

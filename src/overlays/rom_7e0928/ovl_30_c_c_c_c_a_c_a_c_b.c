/* OvlFunc_956_2009a0c -- batch 228, elevated byte-exact.
 *
 * VERDICT
 *   OK OvlFunc_956_2009a0c -- 532 bytes, 213 encodings and 47 relocations identical
 *   Measured with tools/objcmp.py against BOTH scratch_elev/b228/f2009a0c/ref.s
 *   and the original asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c.s. The two
 *   references AGREE, so no Makefile pattern rule is biting on this path.
 *
 * TWIN HYPOTHESIS: HELD, exactly.
 *   This function is a twin of OvlFunc_955_20090dc
 *   (src/overlays/rom_7ddb88/ovl_30_c_c_c_c_a_c_b.c, solved in this same batch).
 *   Diffing the two ROM bodies with the symbol names normalised away leaves
 *   exactly ONE semantic difference across all 207 instructions:
 *
 *       ldr r0, =0x20e9      ->      ldr r0, =0x20ed        (__MessageID)
 *
 *   Everything else that differs is the three .L label NAMES (.L1238/.L1258/
 *   .L1278 -> .L1b68/.L1b88/.L1ba8), which are addresses, not content.
 *   This is the documented "WHEN A FUNCTION HAS A TWIN, DIFF THE DISASSEMBLY
 *   FIRST" path (docs/elevation.md): two seds -- the function name and the one
 *   immediate -- and the first objcmp was exact. No statement was moved, no
 *   pin was re-derived, no shift order was corrected.
 *
 * LEVERS, AND THE MECHANISM OF EACH
 *
 * 1. FIVE __MapActor_SetSpeed PINS ({ PIN3; q0..q2 = ...; call }).
 *    MECHANISM: the five sites take the SAME constant pair (0x80<<9, 0x80<<8).
 *    Without pins gcc commons the pair once and reloads it, collapsing the
 *    per-site `mov #0x80 / lsl` pairs into shared registers. Binding q0/q1/q2
 *    to r0/r1/r2 at each site forces the constant to be REBUILT in the
 *    argument register at that site, which is what the ROM does five times
 *    over. The pin destroys the CSE; it is not about argument ordering here.
 *    The UNIFORM spelling is used at all five sites even though the ROM emits
 *    two different mov/lsl orders -- sched2 produces both from one source form
 *    (documented; confirmed again by the drop_3 residue below).
 *
 * 2. THE NAMED LOCAL `t` (= 0xc0 << 8).
 *    MECHANISM: the value is stored through a call result
 *    (*(short *)(__MapActor_GetActor(0) + 6) = t) and then read again much
 *    later as an argument to __Func_8092adc. It must survive ~40 instructions
 *    and ~20 calls, so the ROM keeps it in callee-saved r6. Naming it makes it
 *    one live pseudo across the whole span; inlining 0xc0<<8 at both sites
 *    makes it two independent constant builds and gcc re-materialises it,
 *    growing the function by 8 bytes.
 *
 * 3. PROLOGUE READ BY CONTENT, NOT WIDTH.
 *    push {r5,r6,r7,lr} + push {r5,r6,r7} for r8/r9/r10 is a WIDE push, but it
 *    is still a pin function. The saved registers are ordinary long-lived
 *    values -- r7 = the parameter `a`, r9 = x, r10 = y, r6 = t, r5 = a reused
 *    scratch ((y<<16) accumulator early, then x-0x10 late), r8 = a recycled
 *    scratch slot holding (y<<16)-0x280000 across two SetPos calls. Four plain
 *    named locals (a, x, y, t) plus gcc's own scratch produce all of it; the
 *    width bought nothing extra beyond the pins.
 *
 * MINIMISATION -- "N PINS IS A SIZE, NOT A SET"
 *   A full one-at-a-time strip was run on THIS function's own shape (not
 *   inherited from the twin's sweep -- a measurement is only valid in the shape
 *   it was taken in). ALL SIX candidate drops FAIL individually, so the
 *   candidate set is EMPTY, and the greedy pass and its fixpoint are both
 *   no-ops. Nothing here is inert scaffolding; every construct ships because it
 *   was measured, not because the twin had it.
 *
 *   dropped            size (ref 532)   encodings (ref 213)   differ   relocs
 *   -----------------  ---------------  --------------------  -------  ------
 *   PIN at SetSpeed a  520              207                   171      differ
 *   PIN at SetSpeed 0  524              209                   187      differ
 *   PIN at SetSpeed 1  528              211                   183      differ
 *   PIN at SetSpeed 2  532 (MATCHES)    213 (MATCHES)          17      differ
 *   PIN at SetSpeed 3  532 (MATCHES)    213 (MATCHES)           3      identical
 *   named local `t`    540              215                   138      differ
 *
 * FINDINGS (corroborations of existing docs/elevation.md entries, sharpened)
 *
 *   a) THE LENGTH TELL FAILED TWICE OUT OF SIX, ON ONE FUNCTION.
 *      Dropping pin 2 and dropping pin 3 each reproduce the ROM's size AND its
 *      encoding count exactly while still being wrong. drop_3 is wrong in
 *      THREE encodings and its relocations are identical -- it is as close to a
 *      false positive as this shape allows. This is the documented "constant-
 *      CSE length tell can cancel exactly" behaviour, here at small scale and
 *      in the region a greedy sweep probes first. Read the diff TEXT.
 *
 *   b) THE RESIDUE OF drop_3 IS THE SCHED2 MOV/LSL ORDER.
 *      First differing encoding, index 40: ref 2003 (mov r0, #3) vs ours 0249
 *      (lsl r1, r1, #9). Removing the pin does not delete the site, it lets
 *      sched2 pick the other of the two emitted orders. That is the same
 *      mechanism as the recorded five-site sched2 entry, seen from the pin
 *      side rather than the spelling side.
 *
 *   c) REFINEMENT of "the pins that fall are typically the LAST use":
 *      here the load-bearing gradient runs first-to-last and reaches nearly
 *      zero without reaching zero -- 171, 187, 183, 17, 3 differing encodings
 *      for pins a, 0, 1, 2, 3. The LAST pin is the closest to inert and is
 *      still not inert. "Nearly inert" is not a category; only a measured 0
 *      licenses a drop.
 *
 * LANDING: NEEDS A SPLIT (3 pieces).
 *   asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c.s holds THREE functions:
 *     lines   8-548  OvlFunc_956_2009474   -> stays asm
 *     lines 554-765  OvlFunc_956_2009a0c   -> becomes this C file
 *     lines 771-959  OvlFunc_956_2009c20   -> stays asm
 *   The .ld line to replace is overlays/rom_7e0928/overlay.ld:52
 *     \t\tasm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_a_c.o(.text)
 *   which becomes the three pieces in that order.
 *   The cut is LABEL-CLEAN: this function's only local labels (.L1b68, .L1b88,
 *   .L1ba8) are defined and referenced entirely within it, and neither
 *   neighbour references them. No .global exports are needed. (The file's other
 *   labels -- .L14b6/.L14be/.L5b80/.L18e8 in the first function, .L1c40/.L1c54/
 *   .L1db8/.L1dca/.L1dd8 in the third -- do not cross either boundary.)
 *   No suffixed piece of this base exists yet in asm/ or src/, so the first
 *   three letters _a/_b/_c are all free; do not name the .c after the .s.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetCameraTarget(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_956_2009a0c(int a)
{
    unsigned char *p;
    int x, y, t;

    p = __MapActor_GetActor(a);
    x = *(short *)(p + 0xa);
    y = *(short *)(p + 0x12);
    __CutsceneStart();
    { PIN3; q0 = a; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetPos(0, x << 16, (y << 16) - 0x300000);
    __MapActor_SetPos(1, (x << 16) - 0x100000, (y << 16) - 0x280000);
    __MapActor_SetPos(2, (x << 16) + 0x100000, (y << 16) - 0x280000);
    __MapActor_SetPos(3, x << 16, (y << 16) - 0x200000);
    __MapActor_SetPos(a, x << 16, (y << 16) - 0x500000);
    t = 0xc0 << 8;
    *(short *)(__MapActor_GetActor(0) + 6) = t;
    __SetCameraTarget(0, 0);
    __MapTransitionIn();
    __WaitMapTransition();
    __MessageID(0x20ed);
    __MapActor_DoAnim(a, 3);
    __ActorMessage(a, 0);
    __Func_80925cc(a, 2);
    __ActorMessage(a, 0);
    __Func_80925cc(a, 2);
    __ActorMessage(a, 0);
    __Func_80925cc(a, 2);
    __ActorMessage(a, 0);
    __MapActor_SetAnim(3, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(6);
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_SetAnim(2, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(2, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_SetAnim(3, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(3, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __Func_80921c4(a, x - 0x10, y - 0x40);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_SetPos(3, 0, 0);
    __Func_80921c4(a, x - 0x10, y - 0x10);
    __Func_80921c4(a, x, y);
    __Func_8092adc(a, t, 0xa);
    __CutsceneEnd();
}

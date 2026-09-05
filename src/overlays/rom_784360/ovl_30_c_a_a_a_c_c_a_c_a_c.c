/* OvlFunc_884_20084d4  --  0x020084d4    EXACT
 *
 * From asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_a_c.s.  106 instructions,
 * 105 encodings, 276 bytes.  A cutscene script: two actors posed, a 40-frame
 * animation loop, two tasks started and stopped around a second loop that
 * walks the actor's +0xc field, then dialogue and __CutsceneEnd.
 *
 * VERDICT
 *   OK OvlFunc_884_20084d4 -- 276 bytes, 105 encodings and 30 relocations identical
 * (against the ORIGINAL asm/ path, not only a scratch cut reference.)
 *
 * WHY THIS FUNCTION WAS PICKED.  It is a member of the population docs/
 * elevation.md "Argument-setup order: the zero interleaved into a shifted
 * build" calls out as out of reach -- "98 are straight-line at every site".
 * Its first interleave site is the canonical shape and sits four instructions
 * into the function, behind nothing but `bl __CutsceneStart`:
 *
 *     mov r2, #0xbe / mov r0, #0 / mov r1, #0x52 / lsl r2, #2
 *     bl  __Func_80921c4
 *
 * There is no branch anywhere before it, so the recorded cure -- name the
 * shifted constants in a dominating block and let gcc rematerialise them
 * across the guard -- has nothing to dominate from.
 *
 * THE STRAIGHT-LINE CURE WORKS, AND IT IS SMALLER THAN THE ONE RECORDED.
 * What produces that sequence is
 *
 *     { register int q0 __asm__("r0"); register int q1 __asm__("r1");
 *       q0 = 0; q1 = 0x52; __Func_80921c4(q0, q1, 0xbe << 2); }
 *
 * -- a PINNED WHOLE-VALUE FILL, arguments assigned in ASCENDING q0, q1 order,
 * the zero written FIRST, and the SPLIT-BUILD ARGUMENT LEFT BARE.  This is the
 * cure OvlFunc_956_200a0f0 found, with one correction: the pin on the split
 * build's own register is NOT part of it.  Dropping r2's pin here is exactly
 * inert; dropping r0's costs 3 and dropping r1's costs 2.  The two registers
 * that must be pinned are the ones holding the SINGLE-INSTRUCTION arguments,
 * i.e. the ones the ROM interleaves INTO the split build -- the split build
 * itself wants to be a plain `0xbe << 2` in the argument list.
 *
 * SECOND SITE, SAME SHAPE, ONE PIN.  `mov r1,#0xa0 / mov r2,#0xa / mov r0,#0 /
 * lsl r1,#8 / bl __Func_8092adc` needs ONLY the r0 pin:
 *     { register int q0 __asm__("r0"); q0 = 0; __Func_8092adc(q0, 0xa0 << 8, 0xa); }
 * Dropping it costs 2; pinning r1 or r2 as well is inert.  So the two sites in
 * one function want DIFFERENT PIN SIZES for the same instruction shape -- "N
 * pins is a size, not a set" applies within a single function, and the size is
 * not predictable from the ROM listing.  Measure it.
 *
 * THE OTHER SIX PINNED BLOCKS MEASURED EXACTLY INERT and are not shipped:
 * __Func_8092848 at both of its sites, OvlFunc_884_200a2e0, __MapActor_Surprise
 * and the second __Func_8092950, plus the r2 pin at __Func_80921c4 and the r1
 * and r2 pins at __Func_8092adc.  Four greedy rounds, re-measured after every
 * drop and re-tested as a set afterwards; the surviving set is at a fixpoint.
 * Note OvlFunc_884_200a2e0's fill (`mov r1,#0xa0 / mov r2,#0x14 / lsl r1,#8 /
 * mov r0,#0xf`) is ALSO an interleave and needs NO pin at all -- a bare call
 * with whole-value arguments produces it.  Do not pin every interleave you see.
 *
 * THE ONE THING THAT IS NOT THE INTERLEAVE: the two __StartTask calls share
 * `0xc8 << 4` and gcc hoists it into r5 behind a push the ROM does not have.
 * The recorded tell reads correctly.  Neither remedy from the record works:
 * -fno-rerun-cse-after-loop changes nothing, and two separately named locals
 * change nothing.  A PIN ON r1 AT EACH SITE IS EXACT, and it is a THIRD remedy
 * for that tell, which the record says it has only two of.  Both pins are
 * load-bearing -- dropping either costs 2.
 *
 * MEASURED-WORSE TABLE (out of 105 encodings, against the final source):
 *   spelling                                                        differing
 *   ---------------------------------------------------------------  -------
 *   __Func_80921c4 site: no pins                                            3
 *   __Func_80921c4 site: r0 only (drop r1)                                  2
 *   __Func_80921c4 site: r1 only (drop r0)                                  3
 *   __Func_80921c4 site: add the r2 pin, split build spelling               0 (inert)
 *   __Func_8092adc site: no pins                                            2
 *   __Func_8092adc site: add the r1 pin                                     0 (inert)
 *   __Func_8092adc site: add the r2 pin                                     0 (inert)
 *   __StartTask: first pin dropped                                          2
 *   __StartTask: second pin dropped                                         2
 *   __StartTask: two named locals instead of pins                           6
 *   __StartTask: -fno-rerun-cse-after-loop, no pins                         6
 *
 * LANDING NEEDS NO SPLIT.  The .s holds exactly ONE function
 * (.thumb_func_start at line 7, .func_end at line 114) and no data -- no
 * .section, .data, .word, .byte or .incbin.  Exactly one linker-script line
 * names the object, and it is a .text line:
 *     overlays/rom_784360/overlay.ld:29
 *         asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_a_c.o(.text)
 * A grep of every .ld in the tree on the FULL PATH finds no other reference.
 * Landing is: add this .c as src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_a_c.c,
 * delete the hand-written .s, and change that one line to
 *     src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_a_c.o(.text)
 * No Makefile flag group is needed; the default -O2 rule builds it.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_Surprise(int slot, int a);
extern void __StartTask(void (*f)(void), int n);
extern void __StopTask(void (*f)(void));
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092950(int slot, int a);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_884_200a2c8(int a, int b);
extern void OvlFunc_884_200a2e0(int a, int b, int c);
extern void OvlFunc_884_200a2f8(unsigned char *p);
extern void OvlFunc_884_200a564(void);
extern void OvlFunc_884_200a574(void);
extern void OvlFunc_884_200a580(void);
extern void OvlFunc_884_200a5a0(void);

#define PIN3  register int q0 __asm__("r0"); \
              register int q1 __asm__("r1"); \
              register int q2 __asm__("r2")

void OvlFunc_884_20084d4(void)
{
    unsigned char *e;
    unsigned char *q;
    int t;
    unsigned int i;

    __CutsceneStart();
    { register int q0 __asm__("r0"); register int q1 __asm__("r1"); q0 = 0; q1 = 0x52; __Func_80921c4(q0, q1, 0xbe << 2); }
    __Func_8092848(0xf, 0, 0x1e);
    __MessageID(0xeae);
    OvlFunc_884_200a2c8(0xf, 0x14);
    OvlFunc_884_200a2e0(0xf, 0xa0 << 8, 0x14);
    __MapActor_Surprise(0xf, 0x81 << 1);
    __CutsceneWait(0x14);
    OvlFunc_884_200a564();
    for (i = 0; i <= 0x27; i++) {
        OvlFunc_884_200a2f8(__MapActor_GetActor(0xf));
        __WaitFrames(1);
    }
    { register int q1 __asm__("r1"); q1 = 0xc8; q1 <<= 4; __StartTask(OvlFunc_884_200a580, q1); }
    { register int q1 __asm__("r1"); q1 = 0xc8; q1 <<= 4; __StartTask(OvlFunc_884_200a5a0, q1); }
    { register int q0 __asm__("r0"); q0 = 0; __Func_8092adc(q0, 0xa0 << 8, 0xa); }
    e = __MapActor_GetActor(0x14);
    q = e + 0x55;
    t = *q;
    *q = 0;
    for (i = 0; i <= 0x27; i++) {
        *(int *)(e + 0xc) += 0xc0 << 5;
        __WaitFrames(1);
    }
    *q = t;
    __StopTask(OvlFunc_884_200a580);
    __StopTask(OvlFunc_884_200a5a0);
    __WaitFrames(1);
    __PlaySound(0xa1);
    __Func_8092950(0xf, 0);
    __Func_8092950(0x14, 0);
    __CutsceneWait(0x28);
    OvlFunc_884_200a574();
    __Func_8092848(0, 0xf, 0x1e);
    __ActorMessage(0xf, 0);
    __CutsceneEnd();
}

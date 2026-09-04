// fakematch
/* OvlFunc_890_2009380  --  0x02009380
 *
 * Cut out of goldensun/asm/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_c_a_c.s.
 *
 * 145 instructions, exact on the first screen. FIVE IDENTICAL DO-WHILE LOOPS
 * differing only in their limit and their wait -- (4, 0xc), (6, 8), (8, 6),
 * (0xa, 4), (0xc, 2) -- driving a rhythm of alternating sounds.
 *
 * THIS IS THE ONE PLACE A MACRO IS SAFER THAN TRANSCRIPTION, and it is the
 * mirror of the rule this tree keeps confirming. A LOOP over a table would not
 * match: the five bodies are emitted in full, and writing them as one loop
 * would collapse them. But writing them out five times by hand risks a typo in
 * fifty near-identical lines, and every one of the five IS byte-identical apart
 * from the two parameters. A macro expanded five times gives the ROM's five
 * copies with the parameters visible at the call. The distinction is whether
 * the repetition is in the OUTPUT (macro is fine) or only in the SOURCE (a loop
 * changes the output).
 *
 * The counter is pinned to r5 because the ROM keeps it there across all five
 * loops, and `i++` sits between the second __PlaySound and the second helper
 * call, which is where the ROM's `add r5, #1` is -- not at the bottom of the
 * body where a `for` would put it.
 *
 * The tail carries the same four-derived-constants iwram chain as
 * src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_c_a_b.c from batch 212 -- offset,
 * value reached by subtracting from it, next offset reached by adding to that,
 * next value -- with all three registers pinned. Copied across unchanged, which
 * is what a neighbour in the same overlay is for.
 */
extern unsigned char *iwram_3001ebc;
extern void OvlFunc_890_2009264(void);
extern void OvlFunc_890_2008238(void);
extern void OvlFunc_890_2008360(void);
extern void OvlFunc_890_200a5fc(int a, int b);

extern void __CutsceneStart(void);
extern void __MessageID(int id);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __SetFlag(int id);
extern void __Func_8091e9c(int a);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")
#define BEAT(lim, w) i = 0; \
    do { \
        __PlaySound(0xf6); \
        OvlFunc_890_2008238(); \
        __CutsceneWait(w); \
        __PlaySound(0xf6); \
        i++; \
        OvlFunc_890_2008360(); \
        __CutsceneWait(w); \
    } while (i != lim)

void OvlFunc_890_2009380(void)
{
    register int i __asm__("r5");

    __CutsceneStart();
    OvlFunc_890_2009264();
    __MessageID(0x1018);
    { PIN3; q1 = 0x80; q0 = 0x10; q1 <<= 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x10; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __MapActor_Jump(0x10, 6, 0x1e);
    { PIN4; q1 = 1; q2 = 0xae; q1 = -q1; q2 <<= 16; q3 = 1; q0 = 0x23e0000;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x1e);
    { PIN2; q0 = 0x8010; q1 = 0x14; OvlFunc_890_200a5fc(q0, q1); }
    BEAT(4, 0xc);
    BEAT(6, 8);
    BEAT(8, 6);
    BEAT(0xa, 4);
    BEAT(0xc, 2);
    OvlFunc_890_2008238();
    __CutsceneWait(6);
    { PIN2; q0 = 0x8010; q1 = 6; OvlFunc_890_200a5fc(q0, q1); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x10; q1 <<= 10; q2 <<= 9;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x90; q2 = 0x8c; q0 = 0x10; q1 <<= 2; q2 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    {
        register unsigned char *b __asm__("r1");
        register unsigned char *r __asm__("r2");
        register int v __asm__("r3");
        b = iwram_3001ebc;
        v = 0xe0; v <<= 1;
        r = b + v;
        v -= 0xc0;
        *(int *)r = v;
        v += 0xc8;
        r = b + v;
        v = 0x20;
        *(int *)r = v;
    }
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x813);
    __Func_8091e9c(3);
}

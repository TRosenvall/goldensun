// fakematch
/* OvlFunc_954_20096ec  --  0x020096ec
 *
 * Cut out of goldensun/asm/overlays/rom_7db0c8/ovl_30_c_c_c_c_c.s, which held
 * this one function AND a tail of `.incbin` data in an explicit `.section
 * .data`. The data now lives in ovl_30_c_c_c_c_c_c.s and the linker script
 * takes .text from here and .data from there.
 *
 * A NOTE ON THAT, because it cost a red build. The file was vetted as "alone in
 * its file" by counting `.thumb_func_start`, and converted whole -- which
 * dropped four global `gOvl_...` symbols another TU links against.
 * `tools/split_s.py` already knows the difference: given a single function WITH
 * a data tail it splits code from data, and only says "convert it directly, no
 * split needed" when there is genuinely none. RUN IT ON EVERY TARGET; its
 * no-split verdict IS the check, and a hand count of function starts is not.
 *
 * 172 instructions. A close twin of
 * src/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c_b.c (OvlFunc_956_2009df8) from
 * batch 208 -- same OvlFunc_common1_* helper family, same three-way branch on
 * that family's return value, same gState opening read with an explicit zero
 * offset. Everything structural was copied across.
 *
 * TWO CROSSED __Func_80933f8 FILLS, identical to each other, each taking two
 * barriers. They are written once as a macro rather than twice by hand, because
 * the two sites are the same seven instructions with the same constants; that
 * is the one place in this tree where a macro is safer than transcription,
 * since the risk it removes is a typo rather than a lost ordering.
 *
 * A DEFERRED SHIFT NEEDED TWO BARRIERS TO SIT LAST, and that is the entry. The
 * ROM builds a callee-saved constant across another call's argument fill:
 *
 *     mov r5, #0xfa / mov r1, #0xc0 / mov r2, #0xc0 / mov r0, #0 /
 *     lsl r1, #9 / lsl r2, #8 / lsl r5, #2 / bl __MapActor_SetSpeed
 *
 * -- r5's shift comes AFTER both of the argument shifts. Written in exactly
 * that source order it comes out FIRST, at 4 of 172: a pinned callee-saved
 * register has no dependence holding its shift down, so the scheduler is free
 * to hoist it. One barrier on q2 after its shift moves it to the right place
 * but pulls `mov r5, #0xfa` ahead of the two argument movs, 3 of 172. Barriers
 * on BOTH q0 (after its mov) and q2 (after its shift) are exact.
 *
 * So the batch-207 per-mov rule extends past argument fills: n values needing a
 * given order need n-1 barriers, and it does not matter whether they are
 * arguments or a long-lived local being built alongside them. What is being
 * ordered is materialisation, not argument setup.
 */
extern unsigned char gState[];

extern void OvlFunc_common1_2c4(void);
extern int OvlFunc_common1_4cc(int a, int b);
extern void OvlFunc_common1_588(int a, int b);
extern void OvlFunc_common1_5e4(int a, int b, int c);
extern void OvlFunc_common1_1078(int a, int b, int c);
extern void OvlFunc_common1_1254(int a);
extern void OvlFunc_common1_1490(int a, int b, int c);
extern void OvlFunc_common1_1550(void);
extern void OvlFunc_common1_15b8(int a, int b, int c);
extern void OvlFunc_954_200833c(int a, int b, int c);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetCameraTarget(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")
#define BUMP4 PIN4; \
              q0 = 0x88; __asm__ volatile ("" : : "r" (q0)); \
              q1 = 1; __asm__ volatile ("" : : "r" (q1)); \
              q2 = 0xa8; q3 = 1; q2 <<= 16; q0 <<= 19; q1 = -q1; \
              __Func_80933f8(q0, q1, q2, q3)

void OvlFunc_954_20096ec(int a)
{
    unsigned char *g;
    int r;
    register int m __asm__("r5");

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
    } else {
        __CutsceneStart();
        r = OvlFunc_common1_4cc(a, 4);
        if (r == 0) {
            __MessageID(0x2099);
            { PIN2; q0 = 0xc0; q1 = 0xc0; q0 <<= 10; q1 <<= 7;
              __Func_80933d4(q0, q1); }
            { BUMP4; }
            __Func_8093530();
            __ActorMessage(a, 0);
            { PIN3; q2 = 0; q1 = 0x48; q0 = 0x78;
              OvlFunc_common1_1490(q0, q1, q2); }
            { PIN2; q1 = 0; q0 = a; __ActorMessage(q0, q1); }
            OvlFunc_common1_1550();
            __CutsceneWait(0xf);
            { PIN3; q1 = 0xf6; q1 <<= 2; q2 = 0xc8; q0 = 0;
              OvlFunc_common1_1078(q0, q1, q2); }
            { PIN3; q2 = 0xa; q0 = 0; q1 = 0; __Func_8092adc(q0, q1, q2); }
            __ActorMessage(a, 0);
            { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0x1e;
              __Func_8092adc(q0, q1, q2); }
            { PIN3; q1 = 0x83; q0 = 0; q1 <<= 1; q2 = 0x3c;
              __MapActor_Emote(q0, q1, q2); }
            m = 0xfa;
            {
                PIN3;
                q1 = 0xc0; q2 = 0xc0; q0 = 0;
                __asm__ volatile ("" : : "r" (q0));
                q1 <<= 9; q2 <<= 8;
                __asm__ volatile ("" : : "r" (q2));
                m <<= 2;
                __MapActor_SetSpeed(q0, q1, q2);
            }
            { PIN3; q1 = m; q2 = 0xc0; q0 = 0;
              OvlFunc_common1_15b8(q0, q1, q2); }
            { PIN3; q1 = m; q2 = 0xb0; q0 = 0;
              OvlFunc_common1_15b8(q0, q1, q2); }
            { PIN3; q1 = 0xfe; q1 <<= 2; q2 = 0xa8; q0 = 0;
              OvlFunc_common1_15b8(q0, q1, q2); }
            __CutsceneWait(0xf);
            { PIN3; q1 = 0xa0; q2 = 0; q0 = 0x12;
              OvlFunc_954_200833c(q0, q1, q2); }
            { BUMP4; }
            { PIN2; q1 = 1; q0 = 0; __MapActor_SetAnim(q0, q1); }
            __CutsceneWait(0xa);
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8;
              __MapActor_SetSpeed(q0, q1, q2); }
            { PIN3; q1 = 0x95; q1 <<= 3; q2 = 0xa8; q0 = 0;
              __Func_80921c4(q0, q1, q2); }
            __CutsceneWait(0xa);
            { PIN3; q1 = 0x80; q0 = 0; q1 <<= 8; q2 = 0x1e;
              __Func_8092adc(q0, q1, q2); }
            { PIN3; q1 = 0x81; q2 = 0x3c; q0 = 0; q1 <<= 1;
              __MapActor_Emote(q0, q1, q2); }
            { PIN2; q1 = 0; q0 = a; __ActorMessage(q0, q1); }
            OvlFunc_common1_1254(0);
            __SetCameraTarget(0, 0);
            { PIN3; q1 = 0xfe; q2 = 0xa8; q0 = 0x12; q1 <<= 18; q2 <<= 16;
              __MapActor_SetPos(q0, q1, q2); }
            OvlFunc_common1_588(a, 4);
        } else if (r == 1) {
            __MessageID(0x2098);
            __ActorMessage(a, 0);
        }
        { PIN3; q1 = a; q2 = 4; q0 = r; OvlFunc_common1_5e4(q0, q1, q2); }
        __CutsceneEnd();
    }
}

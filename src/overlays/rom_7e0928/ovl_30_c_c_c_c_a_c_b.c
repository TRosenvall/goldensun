// fakematch
/* OvlFunc_956_2009df8  --  0x02009df8
 *
 * Cut out of goldensun/asm/overlays/rom_7e0928/ovl_30_c_c_c_c_a_c.s.
 *
 * 163 instructions, three-way branch on a helper's return, EXACT ON THE FIRST
 * SCREEN. Everything it needed was already on file; the value is in what the
 * transcription had to preserve.
 *
 * A NAMED LOCAL WHOSE SHIFT IS DEFERRED ACROSS TWO CALLS. The ROM sets
 * `mov r5, #0x8c`, then calls __Func_8093530 and __CutsceneWait, and only then
 * issues `lsl r5, #1` before feeding r5 to three later calls. Written as
 * `m = 0x8c; ...; m <<= 1;` with the two calls between them, and with `m`
 * pinned to r5 so constant propagation cannot fold the pair back into a single
 * `mov #0x118`. The gap between the assignment and the shift is load-bearing
 * source structure, not scheduling.
 *
 * THREE IDENTICAL FOUR-ARGUMENT FILLS, and they are NOT crossed --
 * `mov r0 / mov r1 / mov r2 / mov r3 / neg r0 / neg r1 / neg r2` runs its movs
 * and its negations in the same order -- so plain pins reach all three and no
 * barrier is needed. Read the two orders before reaching for the lever; this
 * shape looks like the crossed one and is not.
 *
 * The remaining __Func_80933f8, by contrast, IS crossed and takes two barriers.
 * Both forms appear in this one function, which makes it a good place to see
 * the difference.
 *
 * The `ldrsh` on gState takes the register-offset form with an explicit zero
 * (`mov r2, #0 / ldrsh r3, [r3, r2]`) because Thumb-1 has no immediate-offset
 * encoding for a signed halfword load; that needs no source handle. The global
 * is the array idiom, `extern unsigned char gState[]` with `g = gState;`, which
 * is what keeps the base and the offset in separate registers instead of
 * folding to one pool word.
 */
extern unsigned char gState[];

extern void OvlFunc_common1_2c4(void);
extern int OvlFunc_common1_4cc(int a, int b);
extern void OvlFunc_common1_588(int a, int b);
extern void OvlFunc_common1_5e4(int a, int b, int c);
extern void OvlFunc_common1_1078(int a, int b, int c);
extern void OvlFunc_common1_1254(int a);
extern void OvlFunc_common1_1490(int a, int b, int c);
extern void OvlFunc_common1_14f4(int a, int b, int c);
extern void OvlFunc_common1_1550(void);
extern void OvlFunc_common1_15b8(int a, int b, int c);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetCameraTarget(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __Func_8093c00(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")
#define NEG4 PIN4; q0 = 1; q1 = 1; q2 = 1; q3 = 0; q0 = -q0; q1 = -q1; q2 = -q2; \
             __Func_80933f8(q0, q1, q2, q3)

void OvlFunc_956_2009df8(int a)
{
    unsigned char *g;
    int r;
    register int m __asm__("r5");

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        OvlFunc_common1_2c4();
    } else {
        __CutsceneStart();
        r = OvlFunc_common1_4cc(a, 2);
        if (r == 0) {
            __MessageID(0x20b7);
            { PIN2; q0 = 0xc0; q1 = 0xc0; q0 <<= 10; q1 <<= 7;
              __Func_80933d4(q0, q1); }
            { PIN4;
              q0 = 0xbc; __asm__ volatile ("" : : "r" (q0));
              q1 = 1; __asm__ volatile ("" : : "r" (q1));
              q2 = 0x98; q3 = 1; q2 <<= 16; q1 = -q1; q0 <<= 17;
              __Func_80933f8(q0, q1, q2, q3); }
            m = 0x8c;
            __Func_8093530();
            __CutsceneWait(0x1e);
            m <<= 1;
            __ActorMessage(a, 0);
            { PIN3; q1 = m; q2 = 0xc8; q0 = 0; OvlFunc_common1_1078(q0, q1, q2); }
            { PIN3; q1 = 0xc0; q2 = 0xc0; q0 = 0; q1 <<= 9; q2 <<= 8;
              __MapActor_SetSpeed(q0, q1, q2); }
            { PIN3; q1 = m; q2 = 0x98; q0 = 0; OvlFunc_common1_15b8(q0, q1, q2); }
            { PIN3; q1 = 0x94; q1 <<= 1; q2 = 0x98; q0 = 0;
              OvlFunc_common1_15b8(q0, q1, q2); }
            __CutsceneWait(0xa);
            __Func_8093c00();
            { NEG4; }
            { PIN3; q1 = 0xc0; q1 <<= 8; q2 = 0xf; q0 = 0;
              __Func_8092adc(q0, q1, q2); }
            __Func_8093c00();
            { NEG4; }
            { PIN3; q1 = 0; q2 = 0xf; q0 = 0; __Func_8092adc(q0, q1, q2); }
            __Func_8093c00();
            { NEG4; }
            { PIN3; q1 = 0x80; q2 = 0xf; q0 = 0; q1 <<= 7;
              __Func_8092adc(q0, q1, q2); }
            __ActorMessage(a, 0);
            { PIN3; q1 = 0x28; q2 = 0; q0 = 0x60; OvlFunc_common1_1490(q0, q1, q2); }
            { PIN3; q1 = 0x28; q2 = 0xa; q0 = 0x80; OvlFunc_common1_14f4(q0, q1, q2); }
            __CutsceneWait(0x1e);
            { PIN3; q1 = 0x28; q2 = 0xa; q0 = 0xa0; OvlFunc_common1_14f4(q0, q1, q2); }
            __CutsceneWait(0x1e);
            { PIN3; q2 = 0xa; q1 = 0x48; q0 = 0xa0; OvlFunc_common1_14f4(q0, q1, q2); }
            __CutsceneWait(0x1e);
            { PIN2; q1 = 0; q0 = a; __ActorMessage(q0, q1); }
            OvlFunc_common1_1550();
            OvlFunc_common1_1254(0);
            __SetCameraTarget(0, 0);
            OvlFunc_common1_588(a, 2);
        } else if (r == 1) {
            __MessageID(0x20b6);
            __ActorMessage(a, 0);
        }
        { PIN3; q1 = a; q2 = 2; q0 = r; OvlFunc_common1_5e4(q0, q1, q2); }
        __CutsceneEnd();
    }
}

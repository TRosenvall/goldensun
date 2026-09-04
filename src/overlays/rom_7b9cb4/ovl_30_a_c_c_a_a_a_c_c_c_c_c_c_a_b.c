/* OvlFunc_932_20087e8 -- 0x020087e8
 *
 * A cutscene beat: fade a map rect in, play a sting, pan the camera, drift one
 * object down over 24 frames, run a scanline handler for 101 frames, then pan
 * back and set the story flag.
 *
 * THREE LEVERS, all already in docs/elevation.md, applied together:
 *
 *  - The two stack arguments of __Func_80105d4 are named locals PINNED to the
 *    ROM's scratch registers (r3 for sp+0, r2 for sp+4). Ordinary locals take
 *    callee-saved registers and widen the prologue. The two calls fill them in
 *    OPPOSITE orders -- r2 then r3 the first time, r3 then r2 the second --
 *    and the source has to say so.
 *
 *  - The first __Func_8012330 passes 0x20000 TWICE. Written as one expression
 *    gcc commons it and copies (`mov r0, r1`); two locals pinned to r0 and r1
 *    make it rematerialise. Pinning also fixes the crossed mov/shift order the
 *    ROM has here -- movs r0, r1, r2 but shifts r2, r0, r1 -- without needing
 *    a barrier, because the pins alone decide the schedule.
 *
 *  - The ROM materialises .L5238 into r2, stores the zero through r2, and only
 *    THEN copies it to r5 for the loop. A single pointer local is loaded
 *    straight into r5 and the copy disappears. TWO locals, `t` pinned to r2 and
 *    `q` pinned to r5, keep it: gcc will coalesce them away if either is left
 *    free, so both pins are load-bearing.
 *
 * The loop counter's `i--` reads AFTER the __WaitFrames call even though the
 * ROM issues it before; the scheduler hoists it one slot. Writing it before the
 * call in the source instead pushes it too far, ahead of the store.
 */
extern unsigned char L5238[] __asm__(".L5238");
extern unsigned char *iwram_3001e70;

extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int x, int y, int z);
extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __SetIntrHandler(int a, int b, void (*f)(void));
extern void __SetFlag(int id);
extern void OvlFunc_932_20086a0(void);

void OvlFunc_932_20087e8(void)
{
    register unsigned char *p __asm__("r5");
    register int i __asm__("r6");
    register int e __asm__("r3");
    register int f __asm__("r2");
    register unsigned short *q __asm__("r5");
    register unsigned short *t __asm__("r2");
    int n;
    register int x __asm__("r0");
    register int y __asm__("r1");

    p = iwram_3001e70;
    f = 0x1c;
    e = 0x4d;
    __Func_80105d4(0x5d, 0x29, 0x10, 4, e, f);
    __PlaySound(0xe6);
    x = 0x80 << 10;
    y = 0x80 << 10;
    f = 0x80 << 9;
    __Func_8012330(x, y, f);
    __CutsceneWait(0xa);
    p += 0xb2 << 1;
    i = 0x17;
    do {
        *(int *)(p + 0xc) -= 0x10000;
        __WaitFrames(4);
        i--;
    } while (i >= 0);
    __SetIntrHandler(1, 0, OvlFunc_932_20086a0);
    t = (unsigned short *)L5238;
    *t = 0;
    q = t;
    do {
        __WaitFrames(1);
        n = *q + 1;
        *q = n;
    } while ((unsigned short)n <= 0x64);
    __WaitFrames(1);
    __SetIntrHandler(1, 0, 0);
    __PlaySound(0x121);
    x = -1;
    y = -1;
    __Func_8012330(x, y, 0xe666);
    __CutsceneWait(0x1e);
    e = 0x4d;
    f = 0x1c;
    __Func_80105d4(0x4d, 0x29, 0x10, 4, e, f);
    __SetFlag(0x8fe);
}

// fakematch
/* OvlFunc_890_2009be8  --  0x02009be8
 *
 * Cut out of goldensun/asm/overlays/rom_78b2ac/ovl_30_c_c_a_c_b.s.
 *
 * A cutscene: place two actors, walk one through a jump and an animation, speak
 * a line, and set a flag. Nineteen calls, no branches.
 *
 * CHOSEN ON A DIFFERENT AXIS, and that is the point of the file. The last two
 * rounds spent themselves on parks whose residues all reduced to allocation
 * order, which the pin does not reach. Everything at 60 instructions or fewer
 * in this shape group is parked, so the survey was widened to 120 and filtered
 * for functions with NO PARK AT ALL. Two came out; this is the one
 * tools/crossed.py cleared, and it matched on the first screen.
 *
 * Size was never the blocker -- shape is. A 72-instruction function with no
 * accumulated park was easier than any 20-instruction park left in the band.
 *
 * NOTHING NEW WAS NEEDED. Every call is the ordinary interleave, pinned in its
 * own ROM order, and the orders differ site by site as usual: `mov r0` before
 * both shifts at some calls, between them at others, and two calls take their
 * two arguments reversed. Four calls need no pin at all and are written plainly.
 *
 * ONE DETAIL WORTH KEEPING: the final __MapActor_SetPos passes the SAME value
 * for x and y, and the ROM says so directly --
 *
 *     mov r1, #0xc9 / lsl r1, #19 / mov r2, r1 / mov r0, #0x10
 *
 * -- building it once and copying the register. Writing `q2 = q1` reproduces
 * that. Compare src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_c_c_a_c_a.c, where
 * the same situation appears and the ROM builds the value TWICE instead; there
 * the copy had to be forced apart with pins. The ROM does it both ways and the
 * listing is the only thing that says which.
 */

extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void OvlFunc_890_200a5fc(int a, int b);

void OvlFunc_890_2009be8(void)
{
    __PlaySound(0x15);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xbc; q2 = 0xb8; q0 = 0; q1 <<= 1;
        __Func_80921c4(q0, q1, q2);
    }
    __MapActor_SetAnim(0, 0);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xbc; q2 = 0xb8; q0 = 0x10; q1 <<= 17; q2 <<= 16;
        __MapActor_SetPos(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80; q2 = 0x80; q0 = 0x10; q1 <<= 9; q2 <<= 8;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xc4; q0 = 0x10; q1 <<= 1; q2 = 0xa8;
        __Func_80921c4(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80; q2 = 0x1e; q0 = 0x10; q1 <<= 8;
        __Func_8092adc(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        q1 = 1; q0 = 0x10;
        __MapActor_SetAnim(q0, q1);
    }
    __MessageID(0x102b);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0x1e; q0 = 0x10; q1 = 4;
        __MapActor_Jump(q0, q1, q2);
    }
    OvlFunc_890_200a5fc(0x10, 6);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        q1 = 2; q0 = 0;
        __Func_80925cc(q0, q1);
    }
    __CutsceneWait(6);
    __MapActor_DoAnim(0x10, 3);
    OvlFunc_890_200a5fc(0x10, 6);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xbc; q0 = 0x10; q1 <<= 1; q2 = 0xb8;
        __Func_80921c4(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xc9; q1 <<= 19; q2 = q1; q0 = 0x10;
        __MapActor_SetPos(q0, q1, q2);
    }
    __CutsceneWait(4);
    __SetFlag(0x811);
}

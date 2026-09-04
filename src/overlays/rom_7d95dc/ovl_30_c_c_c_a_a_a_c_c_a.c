// fakematch
/* OvlFunc_953_2008648  --  0x02008648
 *
 * From goldensun/asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_a_c_c_a.s, which held
 * this function alone, so no split was needed.
 *
 * PARKED AT 6 OF 43 ON ARGUMENT FILL ORDER. Three __Func_8092adc sites, each
 * carrying the split-constant interleave:
 *
 *     rom   mov r1, #K / mov r0, #0x12 / lsl r1, #8 / mov r2, #V
 *     ours  mov r1, #K / lsl r1, #8    / mov r0, #0x12 / mov r2, #V
 *
 * Pinning the three argument registers and assigning them in the ROM's order at
 * each site matches -- two instructions per site, three sites, six differing.
 *
 * THE PARK CALLED ITS OWN RESIDUE IN ADVANCE. Its opening line is "SCREENED AS
 * A PREDICTION, NOT AS A SEARCH": the argument-temporary boundary predicted the
 * exact shape before the C was written, and the screen confirmed it. That
 * prediction was right about the residue and, like several others corrected in
 * batches 193-204, wrong only in treating it as unreachable -- the lever that
 * closes it did not exist when the park was written.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_DoAnim(int slot, int anim);
extern void OvlFunc_953_2009c48(int a);
extern void OvlFunc_953_2009c5c(int a, int b);

void OvlFunc_953_2008648(void)
{
    __CutsceneStart();
    __Func_8092848(0x12, 0, 0x14);
    __MessageID(0x2122);
    OvlFunc_953_2009c48(0x12);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xd0;
        q0 = 0x12;
        q1 <<= 8;
        q2 = 0x14;
        __Func_8092adc(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0xb0;
        q0 = 0x12;
        q1 <<= 8;
        q2 = 0x14;
        __Func_8092adc(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q0 = 0x12;
        q1 <<= 8;
        q2 = 0x28;
        __Func_8092adc(q0, q1, q2);
    }
    __Func_8092848(0x12, 0, 0x14);
    OvlFunc_953_2009c48(0x12);
    __MapActor_DoAnim(0x12, 3);
    OvlFunc_953_2009c48(0x12);
    OvlFunc_953_2009c5c(0x12, 0xa0 << 7);
    __CutsceneEnd();
}

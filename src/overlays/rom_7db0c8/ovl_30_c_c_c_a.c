// fakematch
/* OvlFunc_954_2008974  --  0x02008974
 *   [asm/overlays/rom_7db0c8/ovl_30_c_c_c_a.s, 1st of 1 -- NO SPLIT NEEDED]
 *
 * A TWIN, FOUND BY tools/twins.py.  Its opcode sequence is identical to the
 * already-solved OvlFunc_955_2008b38 (src/overlays/rom_7ddb88/ovl_30_c_c_c_c_a_b.c)
 * across all 61 instructions, and the callee sets match exactly.  Written by
 * copying that body and changing only the operands the references disagree on:
 *
 *   two __DeleteFieldActor slots   0x28 0x29  ->  0x18 0x19
 *   SetPos #1  0xb0 << 15, 0x80 << 17  ->  0xa5 << 19, 0xc0 << 16
 *   SetPos #2  0xf0 << 15, 0x80 << 17  ->  0xa1 << 19, 0xc0 << 16
 *   __Func_80933f8  0xd0 << 15, 0xc0 << 16  ->  0xa3 << 19, 0x80 << 16
 *
 * Nothing else moved: no pin re-derived, no fill re-ordered, no statement
 * added or removed.  The pins and the q0 barrier are inherited from the twin
 * because the SHAPE is what they were measured on -- but they were re-measured
 * here rather than assumed, since a sibling's cure can be actively wrong when
 * register roles differ.
 */
extern void OvlFunc_common1_fac(int a);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __DeleteFieldActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_807808c(int a);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_954_2008974(int a)
{
    __DeleteFieldActor(0x18);
    __DeleteFieldActor(0x19);
    __Func_807808c(1);
    __CutsceneStart();
    { PIN3; q1 = 0xa5; q2 = 0xc0; q0 = 8; q1 <<= 19; q2 <<= 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xa1; q2 = 0xc0; q0 = 0; q1 <<= 19; q2 <<= 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 8; q1 <<= 7; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0; __Func_809280c(q0, q1, q2); }
    if (a < 0) {
        __MapActor_SetAnim(8, 0xa);
        __MapActor_SetAnim(0, 0x23);
    } else {
        __MapActor_SetAnim(8, 8);
        __MapActor_SetAnim(0, 0x1c);
    }
    __WaitFrames(1);
    {
        PIN4;
        q0 = 0xa3; __asm__ volatile ("" : : "r" (q0));
        q2 = 0x80; q1 = 0; q2 <<= 16; q3 = 0; q0 <<= 19;
        __Func_80933f8(q0, q1, q2, q3);
    }
    OvlFunc_common1_fac(a);
    __CutsceneEnd();
}

// fakematch
/* OvlFunc_945_200c670  --  0x0200c670
 *
 * Was the whole of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_c_a_c.s;
 * split_s.py confirmed one function and no data tail.
 *
 * 141 instructions, exact on the first screen. Four guarded blocks of the same
 * shape -- test a save flag, spawn something, place it, register it, park a
 * slot -- followed by four tail calls, and the whole function is transcription.
 *
 * THE FOUR BLOCKS ARE NOT INTERCHANGEABLE and writing them as a loop over a
 * table would not match. Blocks two and four carry an extra field store that
 * one and three do not, and the ROM interleaves that store into the FOLLOWING
 * call's argument fill rather than emitting it as its own group. The four tail
 * calls differ too: three fill r2, r0, r1 and the fourth fills r0, r1, r2.
 * Fifth batch running that a transcribed sequence is what matches where a loop
 * would not.
 *
 * THE SPAWNED HANDLE STAYS IN r0 ACROSS THE NEXT CALL'S SETUP. The ROM does
 * `bl OvlFunc_945_200cfa8 / mov r1, #0xcd / mov r2, #0xac / mov r5, r0 /
 * lsl r1, #17 / lsl r2, #16 / bl __MapActor_SetPos` -- the returned value is
 * saved to a callee-saved register AND left in r0 to serve as the next call's
 * first argument. Pinning only r1 and r2 and passing the named local as the
 * first argument reproduces that; anchoring r0 as well would emit a redundant
 * `mov r0, r5`.
 */
extern int OvlFunc_945_200cfa8(int a, int b);
extern void OvlFunc_945_200c8e8(int a, int b, int c);

extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern int __GetFlag(int id);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_945_200c670(int a)
{
    int s;

    if (__GetFlag(0x928) != 0) {
        { PIN2; q1 = 0; q0 = 0; s = OvlFunc_945_200cfa8(q0, q1); }
        { register int q1 __asm__("r1");
          register int q2 __asm__("r2");
          q1 = 0xcd; q2 = 0xac; q1 <<= 17; q2 <<= 16;
          __MapActor_SetPos(s, q1, q2); }
        OvlFunc_945_200c8e8(7, s, a);
        __MapActor_SetPos(0xa, 0, 0);
    } else {
        OvlFunc_945_200c8e8(5, 0xa, a);
    }
    if (__GetFlag(0x929) != 0) {
        { PIN2; q1 = 0; q0 = 1; s = OvlFunc_945_200cfa8(q0, q1); }
        { register int q1 __asm__("r1");
          register int q2 __asm__("r2");
          q1 = 0xeb; q2 = 0xac; q1 <<= 17; q2 <<= 16;
          __MapActor_SetPos(s, q1, q2); }
        {
            PIN3;
            *(int *)(__MapActor_GetActor(s) + 0x18) = 0xffff0000;
            q1 = s; q2 = a; q0 = 7;
            OvlFunc_945_200c8e8(q0, q1, q2);
        }
        __MapActor_SetPos(0xb, 0, 0);
    } else {
        OvlFunc_945_200c8e8(6, 0xb, a);
    }
    if (__GetFlag(0x92a) != 0) {
        { PIN2; q1 = 0; q0 = 2; s = OvlFunc_945_200cfa8(q0, q1); }
        { register int q1 __asm__("r1");
          register int q2 __asm__("r2");
          q1 = 0xcd; q2 = 0xcc; q1 <<= 17; q2 <<= 16;
          __MapActor_SetPos(s, q1, q2); }
        OvlFunc_945_200c8e8(7, s, a);
        __MapActor_SetPos(0xc, 0, 0);
    } else {
        OvlFunc_945_200c8e8(5, 0xc, a);
    }
    if (__GetFlag(0x92b) != 0) {
        { PIN2; q1 = 0; q0 = 3; s = OvlFunc_945_200cfa8(q0, q1); }
        { register int q1 __asm__("r1");
          register int q2 __asm__("r2");
          q1 = 0xeb; q2 = 0xcc; q1 <<= 17; q2 <<= 16;
          __MapActor_SetPos(s, q1, q2); }
        {
            PIN3;
            *(int *)(__MapActor_GetActor(s) + 0x18) = 0xffff0000;
            q1 = s; q2 = a; q0 = 7;
            OvlFunc_945_200c8e8(q0, q1, q2);
        }
        __MapActor_SetPos(0xd, 0, 0);
    } else {
        OvlFunc_945_200c8e8(6, 0xd, a);
    }
    { PIN3; q2 = a; q0 = 5; q1 = 0xe; OvlFunc_945_200c8e8(q0, q1, q2); }
    { PIN3; q2 = a; q0 = 6; q1 = 0xf; OvlFunc_945_200c8e8(q0, q1, q2); }
    { PIN3; q2 = a; q0 = 5; q1 = 0x10; OvlFunc_945_200c8e8(q0, q1, q2); }
    OvlFunc_945_200c8e8(6, 0x11, a);
}

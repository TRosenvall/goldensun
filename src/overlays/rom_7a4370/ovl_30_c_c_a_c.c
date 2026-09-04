// fakematch
/* OvlFunc_917_2008158  --  0x02008158
 *
 * Was the whole of goldensun/asm/overlays/rom_7a4370/ovl_30_c_c_a_c.s;
 * split_s.py confirmed one function and no data tail.
 *
 * A three-armed dialogue guard, 83 instructions, single exit. Two of the three
 * arms fall through to one `bl __CutsceneEnd` and the third reaches it too, so
 * the teardown is written once at the bottom rather than per arm -- the lever
 * measured at 86 instructions in batch 209.
 *
 * ONE VARIABLE, THREE ROLES, ONE REGISTER. The halfword poke at the end of the
 * middle arm is
 *
 *     mov r1, #0xb9 / ldr r3, [r3] / lsl r1, #1 / add r2, r3, r1 /
 *     mov r3, #1 / strh r3, [r2]
 *
 * -- r3 holds the iwram base, then is REFILLED with the stored value one
 * instruction after the `add` consumes it, while r1 carries the offset and r2
 * the address. Written with the value as a plain literal it goes to the pool
 * (`ldr r3, =0x1`, one differing); written as a fourth named local it takes r1
 * and the store reads the wrong register (two differing). Declaring a second
 * pin ON THE SAME REGISTER as the base -- `register int v __asm__("r3")` beside
 * `register unsigned char *b __asm__("r3")` -- spells the reuse exactly and is
 * exact.
 *
 * That is the one-variable-two-ranges shape from
 * src/overlays/rom_7d768c/ovl_30_c_a_a_a_c_b.c, but where that function reuses a
 * single C variable, this one needs two variables sharing a pinned register,
 * because the two roles have different types -- a pointer and an int -- and C
 * has no way to say "the same storage" across a type change.
 */
extern unsigned char *iwram_3001ebc;
extern void OvlFunc_917_20092f4(int a, int b);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern int __GetFlag(int id);
extern int __CheckPartyItem(int item);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_80925cc(int a, int b);
extern void __Func_8093040(int a, int b, int c);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_917_2008158(void)
{
    __CutsceneStart();
    if (__GetFlag(0x845) != 0) {
        { PIN2; q1 = 1; q0 = 0xa; OvlFunc_917_20092f4(q0, q1); }
        __MessageID(0x151c);
        __ActorMessage(8, 0);
        OvlFunc_917_20092f4(0xa, 0);
    } else if (__GetFlag(0x844) != 0) {
        { PIN2; q1 = 1; q0 = 0xa; OvlFunc_917_20092f4(q0, q1); }
        __MessageID(0x14eb);
        __ActorMessage(8, 0);
        { PIN2; q1 = 0; q0 = 0xa; OvlFunc_917_20092f4(q0, q1); }
        if (__CheckPartyItem(0xb8) != -1) {
            register unsigned char *b __asm__("r3");
            register int o __asm__("r1");
            register unsigned char *p __asm__("r2");
            register int v __asm__("r3");
            b = iwram_3001ebc;
            o = 0xb9; o <<= 1;
            p = b + o;
            v = 1;
            *(short *)p = v;
        }
    } else {
        __MessageID(0x14c9);
        __ActorMessage(8, 0);
        { PIN2; q1 = 1; q0 = 0x406218; __Func_8091200(q0, q1); }
        __Func_8091254(0x14);
        __WaitFrames(0x28);
        { PIN3; q2 = 0xa; q0 = 0x200e; q1 = 0; __Func_8093040(q0, q1, q2); }
        __Func_80925cc(0, 2);
        { PIN2; q0 = 0x200e; q1 = 0; __ActorMessage(q0, q1); }
        { PIN2; q0 = 0x80; q0 <<= 9; q1 = 1; __Func_8091200(q0, q1); }
        __Func_8091254(0x14);
        __WaitFrames(0x28);
    }
    __CutsceneEnd();
}

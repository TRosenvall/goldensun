/* OvlFunc_932_20086dc  --  0x020086dc  [asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_a.s]
 *
 * NOT MATCHING. Best 35 of 100, LENGTH EXACT. The candidate below is that form.
 *
 * A twenty-four iteration loop that walks a field down, with one iteration
 * doing extra work, then a second timed loop driven by an interrupt handler.
 *
 * THREE VALUES SHARING TWO CALLEE-SAVED REGISTERS, and getting that far took
 * three pins. The ROM's setup is
 *
 *     ldr r5, [r3]        <- the iwram base
 *     ldr r7, =0x1999
 *     add r6, r5, r2      <- the walked pointer, a NEW register
 *     mov r5, #0          <- the loop counter, REUSING the base's register
 *
 * Written plainly, gcc loads the base straight into r6 and advances it in
 * place, then puts the counter in r7 and the constant elsewhere: two pushed
 * registers against the ROM's three, 82 of 96. Pinning the pointer to r6 gives
 * 83; pinning the pointer AND the counter gives 100 lines and 36; pinning the
 * base to r5 as well -- so that base and counter share one register, spelled as
 * two `register` declarations naming r5 -- gives 35 with the length exact.
 *
 * That is the one-register-two-roles shape again, and it is the third distinct
 * form of it this tree has needed: one C variable reused (2008108), two
 * variables of different TYPES on one pin (2008158), and here two variables
 * whose live ranges are adjacent across a three-operand `add` that reads the
 * first while defining the second.
 *
 * WHAT REMAINS -- A LOOP ENTRY GUARD THE ROM DOES NOT HAVE:
 *
 *     rom   .L0: <body> ... add r5, #1 / cmp r5, #0x17 / ble .L0
 *     ours  cmp r5, #0x17 / bgt L0 / .L0: <body> ...
 *
 * gcc cannot prove the first iteration runs once the counter is PINNED, so it
 * emits an entry test that the ROM does not. The two are in direct tension: the
 * pin is what puts the counter in r5, and the pin is what costs the guard.
 *
 * MEASURED, and this is the tension in numbers:
 *
 *     for-loop, counter unpinned          83 differ,  96 lines
 *     for-loop, counter pinned to r5      35 differ, 100 lines
 *     do-while, counter pinned to r5      66 differ,  97 lines
 *
 * Rewriting the loop as an explicit `do { } while (i <= 0x17)` DOES remove the
 * guard -- the first divergence moves from instruction 17 to 34 -- but costs
 * three instructions inside the `if (i == 8)` arm, where the ROM interleaves
 * two argument movs around a store and the do-while form does not. So each
 * spelling fixes what the other breaks.
 *
 * NEXT: the question is whether the counter can reach r5 without a pin. It
 * would if the BASE were the thing pinned and the counter simply inherited the
 * register when the base died -- which is what the ROM's own allocation looks
 * like. Pinning the base alone, with the counter and pointer left free, was not
 * tried; only base+pointer+counter together were. That is the cheap next test.
 */
extern unsigned char *iwram_3001e70;
extern int L5238 __asm__(".L5238");
extern unsigned char gScript_932__0200bd48[];
extern void OvlFunc_932_20086a0(void);

extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, int s);
extern void __SetFlag(int id);
extern void __SetIntrHandler(int a, int b, void (*f)(void));
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_932_20086dc(void)
{
    register unsigned char *w __asm__("r5");
    register unsigned char *p __asm__("r6");
    unsigned short *q;
    register int s __asm__("r7");
    register int i __asm__("r5");
    int t;

    w = iwram_3001e70;
    __PlaySound(0xe6);
    { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q0 <<= 10; q1 <<= 10; q2 <<= 9;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0xa);
    p = w + (0xb2 << 1);
    s = 0x1999;
    for (i = 0; i <= 0x17; i++) {
        *(int *)(p + 0xc) += 0xffff0000;
        __WaitFrames(4);
        if (i == 8) {
            *(int *)(__MapActor_GetActor(8) + 0x18) = s;
            {
                PIN3;
                q1 = 0x98; q2 = 0xd8;
                *(int *)(__MapActor_GetActor(8) + 0x1c) = s;
                q1 <<= 16; q0 = 8; q2 <<= 16;
                __MapActor_SetPos(q0, q1, q2);
            }
            __MapActor_SetBehavior(8, (int)gScript_932__0200bd48);
        }
    }
    __SetIntrHandler(1, 0, OvlFunc_932_20086a0);
    q = (unsigned short *)&L5238;
    *q = 0;
    do {
        __WaitFrames(1);
        t = *q;
        t += 1;
        *q = t;
    } while ((unsigned)(t << 16) <= (unsigned)(0xc8 << 15));
    __WaitFrames(1);
    { PIN3; q1 = 0; q2 = 0; q0 = 1; __SetIntrHandler(q0, q1, (void (*)(void))q2); }
    __PlaySound(0x121);
    { PIN3; q0 = 1; q1 = 1; q1 = -q1; q2 = 0xe666; q0 = -q0;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x1e);
    {
        register int e0 __asm__("r3");
        register int e1 __asm__("r2");
        e0 = 3; e1 = 0xe;
        __Func_8010704(0, 0, 1, 2, e0, e1);
    }
    __SetFlag(0x8fd);
}

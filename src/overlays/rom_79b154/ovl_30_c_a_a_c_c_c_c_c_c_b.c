// fakematch
/* OvlFunc_907_2008404  --  0x02008404
 *
 * Cut out of goldensun/asm/overlays/rom_79b154/ovl_30_c_a_a_c_c_c_c_c_c.s.
 *
 * A guarded cutscene that alternates two values into one actor field six times.
 * 138 instructions.
 *
 * PINNING A STORE'S TWO OPERANDS ALSO FIXED A HOISTED POOL LOAD, which is the
 * entry. Two residues showed up together at 6 of 138:
 *
 *     rom  mov r3, r5 / add r3, #0x55 / mov r2, #0 / strb r2, [r3]
 *     ours mov r2, r5 / add r2, #0x55 / mov r3, #0 / strb r3, [r2]
 *
 * -- address and value in each other's registers -- and `ldr r6, =0xffff0000`
 * issued one instruction ahead of the previous statement's `strb`. A
 * `do { } while (0)` wall fixes the load and leaves the store swapped: 4 of 138.
 * Pinning the store's address to r3 and its value to r2, WITH NO WALL, fixes
 * both and is exact. The load was not being hoisted for its own reasons; it was
 * filling a slot that the wrongly-allocated store had left open. FIX THE
 * ALLOCATION BEFORE ADDING A BARRIER -- a barrier that papers over a slot is
 * how a function ends up with scaffolding it does not need.
 *
 * The rest is on file. Two callee-saved locals hold the alternating values and
 * are pinned to the ROM's r5 and r6, and r5 is REUSED: it holds the second
 * actor pointer until that pointer's last use, then becomes the shifted
 * constant one instruction later. That is the one-variable-two-ranges shape
 * from src/overlays/rom_7d768c/ovl_30_c_a_a_a_c_b.c, and here separate locals
 * are enough because the two ranges are already adjacent -- the pin on the
 * second one does not disturb the first.
 *
 * Each of the three `*(int *)(a + 0x18) = ...` stores sits INSIDE the argument
 * fill of the call that follows it, so each is written between that call's
 * pinned assignments rather than as a statement before the block.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Surprise(int slot, int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern void __Field_MindRead(int a, int b);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8097608(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_907_2008404(void)
{
    unsigned char *a;
    unsigned char *b;
    register int n __asm__("r6");
    register int v __asm__("r5");

    a = __MapActor_GetActor(0);
    b = __MapActor_GetActor(0xb);
    if ((*(int *)(b + 8) >> 20) == 6) {
        __CutsceneStart();
        __Func_8092b08(0xb, 1);
        { PIN2; q1 = 2; q0 = 0; __Func_80925cc(q0, q1); }
        __CutsceneWait(0x14);
        { PIN3; q0 = 0; q1 = 0x3333; q2 = 0x1999;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q1 = 0x3333; q2 = 0x1999; q0 = 0xb;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_GetActor(0)[0x5a] &= 0xfe;
        n = 0xffff0000;
        {
            register unsigned char *r3 __asm__("r3");
            register int r2 __asm__("r2");
            r3 = b + 0x55; r2 = 0;
            *r3 = r2;
        }
        {
            PIN2;
            q1 = 0x81; q0 = 0; q1 <<= 1;
            *(int *)(a + 0x18) = n;
            __MapActor_Surprise(q0, q1);
        }
        v = 0x80;
        __MapActor_SetAnim(0, 0x10);
        v <<= 9;
        __Func_8092158(0xb, 0x6f, 0xc4);
        {
            PIN3;
            q2 = 0xb9; q1 = 0x80; q0 = 0;
            *(int *)(a + 0x18) = v;
            __Func_80921c4(q0, q1, q2);
        }
        __CutsceneWait(0x14);
        {
            PIN2;
            q1 = 0x81; q0 = 0; q1 <<= 1;
            *(int *)(a + 0x18) = n;
            __MapActor_Surprise(q0, q1);
        }
        __MapActor_SetAnim(0, 0x10);
        __Func_8092158(0xb, 0x79, 0xbe);
        {
            PIN3;
            q2 = 0xbd; q1 = 0x8d; q0 = 0;
            *(int *)(a + 0x18) = v;
            __Func_80921c4(q0, q1, q2);
        }
        __CutsceneWait(0x14);
        {
            PIN2;
            q1 = 0x81; q0 = 0; q1 <<= 1;
            *(int *)(a + 0x18) = n;
            __MapActor_Surprise(q0, q1);
        }
        __MapActor_SetAnim(0, 0x10);
        { PIN3; q1 = 0x84; q2 = 0xba; q0 = 0xb; __Func_8092158(q0, q1, q2); }
        *(int *)(a + 0x18) = v;
        {
            unsigned char *q = __MapActor_GetActor(0);
            int u, k;
            u = q[0x5a]; k = 1; k |= u;
            q[0x5a] = k;
        }
        { PIN3; q1 = 0x9999; q0 = 0; q2 = 0x4ccc;
          __MapActor_SetSpeed(q0, q1, q2); }
        __Func_80921c4(0, 0xa6, 0xb9);
        { PIN3; q1 = 0x80; q2 = 0x14; q0 = 0; q1 <<= 8;
          __Func_8092adc(q0, q1, q2); }
        __Func_8092b08(0xb, 2);
        { PIN2; q1 = 0xb; q0 = 0; __Field_MindRead(q0, q1); }
        __WaitFrames(0xa);
        __MessageID(0x1774);
        { PIN2; q1 = 0; q0 = 0xb; __ActorMessage(q0, q1); }
        __Func_8097608();
        __WaitFrames(0xa);
        __SetFlag(0x848);
        __CutsceneEnd();
    }
}

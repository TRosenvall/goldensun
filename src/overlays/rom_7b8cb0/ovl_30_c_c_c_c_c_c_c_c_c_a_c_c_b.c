// fakematch
/* OvlFunc_931_20087b8  --  0x020087b8
 *
 * Cut out of goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c_c.s.
 *
 * Fifty-three instructions: zero two halfwords on an actor, set two fields, hand
 * it a task, and play it out. Small, and it still needed three of this tree's
 * levers stacked on one pair of stores.
 *
 * THE OPENING STORE PAIR wanted all of: the zero NAMED, both operands PINNED,
 * and the address ADVANCED IN PLACE. The ROM is
 *
 *     mov r3, r1 / mov r2, #0 / add r3, #0x64 / strh r2, [r3] /
 *     add r3, #0x2 / strh r2, [r3]
 *
 * -- one zero in r2 feeding two stores through an address in r3 that walks
 * forward. Written as `*(short *)(a + 0x64) = 0;` twice, the zero goes to the
 * POOL (`ldr r3, =0x0`, the store-width pooling already on file), the address
 * and value land in each other's registers, and the function comes out 55
 * against 53 with a stray branch: 31 differing. Naming the zero as its own
 * assignment, pinning it to r2 and the pointer to r3, and advancing the pointer
 * with `r3 += 1` rather than re-deriving it, is exact.
 *
 * Worth noting WHY all three are needed together: the pooled zero and the
 * swapped registers are the same defect seen twice. gcc puts the value in the
 * pool because it never gave the value a register of its own, and it never gave
 * it one because the address expression claimed the low register first.
 *
 * THE ACTOR POINTER IS A NAMED LOCAL HERE, which is the opposite of the usual
 * advice. Elsewhere in this tree subscripting __MapActor_GetActor's result
 * directly is what keeps the address in r0; here the ROM moves it OUT with
 * `mov r1, r0` because r0 is needed for the slot number of the call that
 * follows, and it is read at three different offsets in between. Read the ROM's
 * register, not the habit.
 *
 * The last __Func_8012330 is a crossed site in the NEGATION form -- movs r0
 * then r1, negations r1 then r0 -- and takes one barrier after `q0 = 1`.
 * `tools/crossed.py` did NOT flag it: its scan looks for `lsl` only.
 */
extern void OvlFunc_931_20086f0(void);

extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __DeleteFieldActor(int slot);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8092158(int a, int b, int c);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_931_20087b8(void)
{
    unsigned char *a;

    a = __MapActor_GetActor(0x12);
    {
        register short *r3 __asm__("r3");
        register int r2 __asm__("r2");
        r3 = (short *)(a + 0x64); r2 = 0;
        *r3 = r2;
        r3 += 1;
        *r3 = r2;
    }
    *(int *)(a + 0x48) = 0x6666;
    {
        PIN3;
        q0 = 0x12;
        *(void **)(a + 0x6c) = OvlFunc_931_20086f0;
        q2 = 0x9999; q1 = 0x13333;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    { PIN3; q2 = 0xe6; q0 = 0x12; q1 = 0x1c; q2 <<= 1;
      __Func_8092158(q0, q1, q2); }
    { PIN3; q2 = 0xe0; q1 = 0x18; q2 <<= 1; q0 = 0x12;
      __Func_8092158(q0, q1, q2); }
    __PlaySound(0xe5);
    __DeleteFieldActor(0x12);
    { PIN3; q1 = 0x80; q2 = 0x80; q1 <<= 9; q2 <<= 9; q0 = 0;
      __Func_8012330(q0, q1, q2); }
    __CutsceneWait(4);
    {
        PIN3;
        q0 = 1; __asm__ volatile ("" : : "r" (q0));
        q1 = 1; q1 = -q1; q2 = 0xe666; q0 = -q0;
        __Func_8012330(q0, q1, q2);
    }
    __CutsceneWait(0x28);
    __MapActor_SetAnim(0x12, 1);
}

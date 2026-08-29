/* OvlFunc_921_2008abc  --  0x02008abc
 *
 * Cut out of goldensun/asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_a_a_c.s.
 *
 * The sanctum attendant. Which line he says depends on which way the player is
 * facing when they talk to him -- turned toward the altar he opens the sanctum
 * menu, otherwise he just talks.
 *
 * BUILT WITH CSE_CFLAGS. Under the default flags this is 54 differing of 63,
 * because 0x82d is read then written inside one block and gcc holds it in a
 * register across the two calls between. -fno-rerun-cse-after-loop takes it
 * straight to 4 with no change to the C, which is the flag-first rule from
 * batch 106 working exactly as written.
 *
 * The four it leaves are two argument rotations and they need two different
 * levers:
 *
 *   __Func_8093054 is declared `int`. The ROM emits `mov r1, #0 / mov r0, #0x13`
 *   -- r0 last -- which is the return-type lever's `int` row (batch 99, and the
 *   full table in reports/batch-106.md).
 *
 *   The 0xc0 << 6 argument to __Func_8092adc needs the BASIC-BLOCK LEVER. The
 *   ROM splits its `mov`/`lsl` pair around `mov r0, #0x13`; assigned to a local
 *   at the top of the function and used once, three blocks down, gcc
 *   rematerialises it at the call in exactly that split form.
 *
 * THE OPENING TEST IS A FACING RANGE CHECK, the batch-29 family:
 *
 *     ldrh r3, [r0, #6] / ldr r2, =0x5fff / add r3, r2
 *     lsl r3, #16 / ldr r2, =0x3ffe0000 / cmp r3, r2 / bhi
 *
 * `unsigned short d = actor->facing + 0x5fff; if (d <= 0x3ffe)`. The `lsl #16`
 * against a pre-shifted bound is the narrowing-cast tell -- leaving `d` an
 * `int` drops the shift and the whole test changes shape.
 *
 * The two `__MessageID(id); __ActorMessage(0x13, 0);` tails in the else arm are
 * written out in full. gcc cross-jumps them into one `bl` pair by itself, which
 * is why the ROM has `ldr r0, =0x1671 / b` reaching a shared block.
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short facing;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __UI_Sanctum(int slot);
extern int __Func_8093054(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_921_2008abc(void)
{
    unsigned short d;
    int v;

    v = 0xc0 << 6;
    d = __MapActor_GetActor(0)->facing + 0x5fff;
    if (d <= 0x3ffe) {
        __CutsceneStart();
        if (__GetFlag(0x82d) == 0) {
            __MessageID(0x1553);
            __ActorMessage(0x13, 0);
            __SetFlag(0x82d);
        }
        __CutsceneEnd();
        __UI_Sanctum(0x13);
    } else {
        __CutsceneStart();
        if (__GetFlag(0x881)) {
            __MessageID(0x1671);
            __ActorMessage(0x13, 0);
        } else if (__GetFlag(3)) {
            __MessageID(0x1572);
            __ActorMessage(0x13, 0);
        } else {
            __MessageID(0x1554);
            __Func_8093054(0x13, 0);
            __Func_8092adc(0x13, v, 0xa);
        }
        __CutsceneEnd();
    }
}

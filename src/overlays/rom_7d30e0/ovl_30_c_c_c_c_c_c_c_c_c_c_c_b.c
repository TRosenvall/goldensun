/* OvlFunc_948_200a290 -- 0x0200a290
 *
 * Scene entry: flag four actors, give five of them the same speed word, start
 * three per-frame tasks at the same priority, and set the blend registers.
 * Nine calls, almost no control flow -- and it still needed four levers,
 * because everything here is a value REUSED across a call.
 *
 * THE ACTOR POINTER MUST NOT BE NAMED. Assigning each result to one local `p`
 * gives `mov r3, r0 / add r3, #0x59 / strb` at every site: the local takes a
 * register of its own and the returned pointer is copied into it. Using the
 * call expression directly -- `__MapActor_GetActor(8)[0x59] = v;` -- lets gcc
 * advance the return register in place, which is the ROM's `add r0, #0x59 /
 * strb r5, [r0, #0x0]`. Nine sites, and it is the reverse of the usual advice:
 * here NOT naming the intermediate is what matches.
 *
 * THE TASK PRIORITY IS REBUILT AT ALL THREE CALLS. `0xc8 << 4` written as a
 * literal three times is commoned into a callee-saved register and fed to each
 * call with `mov r1, r5`; the ROM emits `mov r1, #0xc8 / lsl r1, #0x4` every
 * time. A local pinned to r1, assigned and shifted per site, rematerialises it.
 * Same lever as the focal length in src/rom_f4000/rom_f4008_a_a_c.c this batch.
 *
 * THE BLEND CONSTANTS NEED NAMING to get the ROM's register roles. Written as
 * `REG_BLDCNT = 0x3f42;` gcc puts the ADDRESS in r2 and the VALUE in r3; the
 * ROM has them the other way round. Naming both values swaps them back. (Both
 * constants are pooled either way -- neither is buildable by `mov`/`lsl` -- so
 * this is purely about which register holds which, not about the pool.)
 *
 * The two values in r5 -- 1 for the four byte flags, then 0xb333 for the five
 * word stores -- are one local reassigned, matching the ROM's single pushed
 * register.
 */
#include "gba/io.h"

extern unsigned char *__MapActor_GetActor(int slot);
extern void __StartTask(void (*f)(void), int prio);
extern void OvlFunc_948_20097ac(void);
extern void OvlFunc_948_200941c(void);
extern void OvlFunc_948_2009308(void);

void OvlFunc_948_200a290(void)
{
    int v;
    int c0, c1;
    register int pr __asm__("r1");

    v = 1;
    __MapActor_GetActor(8)[0x59] = v;
    __MapActor_GetActor(9)[0x59] = v;
    __MapActor_GetActor(0xa)[0x59] = v;
    __MapActor_GetActor(0xb)[0x59] = v;
    v = 0xb333;
    *(int *)(__MapActor_GetActor(8) + 0x18) = v;
    *(int *)(__MapActor_GetActor(9) + 0x18) = v;
    *(int *)(__MapActor_GetActor(0xa) + 0x18) = v;
    *(int *)(__MapActor_GetActor(0xb) + 0x18) = v;
    *(int *)(__MapActor_GetActor(0xc) + 0x18) = v;
    pr = 0xc8;
    pr <<= 4;
    __StartTask(OvlFunc_948_20097ac, pr);
    pr = 0xc8;
    pr <<= 4;
    __StartTask(OvlFunc_948_200941c, pr);
    pr = 0xc8;
    pr <<= 4;
    __StartTask(OvlFunc_948_2009308, pr);
    c0 = 0x3f42;
    REG_BLDCNT = c0;
    c1 = 0x607;
    REG_BLDALPHA = c1;
}

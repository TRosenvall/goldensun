/* Cluster Func_80ccbdc..Func_80ccbdc extracted from goldensun/asm/rom_c9000/rom_cc5d8_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_c9000/rom_cc5d8_a_a.o and asm/rom_c9000/rom_cc5d8_a_c.o in
 * goldensun/stage1.ld.
 *
 * A teardown: stop three tasks, clear the 0x4000-byte tile block at 0x6004000,
 * release two allocator tags. Eighteen instructions.
 *
 * Fourth out of the `_call_via_rN` class. This one wants the pointer typed
 * `int (*)(void *, s32)` -- the ROM fills r0 LAST:
 *
 *     mov r1, #0x80 / ldr r3, =Func_80008d4 / lsl r1, #7 / ldr r0, =0x6004000
 *
 * which is the opposite of its neighbour src/rom_c9000/rom_cd508_a_b.c, calling
 * the SAME callee with a `void` pointer type. The two together are the clearest
 * statement of the rule: the pointer's return type is a per-CALL-SITE choice
 * that reports what the original translation unit declared, not a property of
 * the callee. Func_80008d4 is one function and these two files disagree about
 * it, exactly as two different original .c files would.
 *
 * The three StopTask arguments are plain function addresses, and 0x4000 is
 * built at runtime as 0x80 << 7 rather than pooled.
 */
#include "gba/types.h"

extern int Func_80008d4(void *dst, s32 len);
extern void StopTask(void *task);
extern void gfree(s32 tag);
extern void Func_80cc960(void);
extern void Task_BlitAnim(void);
extern void Func_80cd4b4(void);

void Func_80ccbdc(void)
{
    int (*fp)(void *, s32);

    StopTask(Func_80cc960);
    StopTask(Task_BlitAnim);
    fp = Func_80008d4;
    fp((void *)0x6004000, 0x80 << 7);
    StopTask(Func_80cd4b4);
    gfree(0x28);
    gfree(0x27);
}

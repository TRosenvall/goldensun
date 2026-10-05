/* Anim_UnleashIntro  @ 0x080ccaec  [rom_c9000]
 *
 * Source asm: goldensun/asm/rom_c9000/rom_cc5d8_a_a.s
 *
 * BLOCKER CLASS: arg-interleave. TWO instructions of 80, one adjacent pair:
 *
 *     rom    mov r0, #0xa0 / ldr r3, =Func_8001af8 / mov r2, #0x80 / lsl r0, #0x13
 *     ours   mov r0, #0xa0 / ldr r3, =Func_8001af8 / lsl r0, #0x13 / mov r2, #0x80
 *
 * The ROM schedules the third argument INTO the two-instruction build of the
 * first; gcc completes the first and then sets the third.
 *
 * TWO READINGS THAT ARE SETTLED:
 *
 *   1. `case 4:` MUST BE WRITTEN OUT, even though it does exactly what
 *      `default:` does. The ROM's jump table has five entries and its fifth
 *      points at the default block. With only four labelled cases plus a
 *      default, gcc emits a COMPARISON CHAIN instead of a table and the
 *      function is 59 differing of 80; adding the redundant `case 4:` produces
 *      the table and takes it to 6. It also fixed an r5/r6 swap as a side
 *      effect, which is worth knowing on its own -- a register rename is not
 *      always an allocation problem, it can be downstream of a control-flow
 *      shape that is wrong.
 *   2. The `bl _call_via_r3` veneer is ordinary C: a typed function-pointer
 *      local, assigned then called, per the sibling src/rom_c9000/rom_cc5d8_a_b.c.
 *
 * A NEW LEVER CAME OUT OF THIS ONE, and it fixed two of the three instances --
 * see "Naming a shifted constant in the SAME basic block" in docs/elevation.md.
 * Writing `p = 0xc8 << 4; StartTask(fn, p);` instead of the literal at the call
 * site makes gcc build the constant CONTIGUOUSLY before loading the other
 * argument, which is what the two StartTask calls needed. That is the inverse
 * of the documented dominating-block lever, which exists to make gcc SPLIT a
 * constant. Both StartTask sites went from wrong to exact; 6 differing to 2.
 *
 * WHY THE REMAINING ONE RESISTS. It needs the opposite: the ROM interleaves
 * there, so the constant has to be split, which is the dominating-block lever's
 * job. The function HAS a boundary to cross -- the switch -- so unlike
 * src/non_matching/ovl_7cb2c0/200dca4.c the lever is available. It simply does
 * not take here.
 *
 * MEASURED:
 *   named locals for both StartTask constants                    2   (best)
 *   ... plus the third constant named in the same block          2
 *   ... plus the third constant assigned BEFORE the switch       2
 *   assigning the function pointer before the switch            37
 *   literals at every call site                                  6
 *   StartTask declared with no prototype                         6
 *   the function pointer typed with no parameter list            6
 *   -fno-schedule-insns                                          6
 *   -fno-schedule-insns2                                        19
 */
#include "gba/types.h"
#include "gba/io.h"

extern int _FILE_b4;
extern int _FILE_be;
extern int _FILE_c8;
extern int _FILE_cb;
extern int _FILE_cf;

extern void *galloc_iwram(int tag, int size);
extern void AnimStart(int a);
extern u8 *GetFile(s32 id);
extern void Func_8001af8(volatile u16 *dst, void *src, s32 len);
extern void StartTask(void *task, int prio);
extern void Func_80cc960(void);
extern void Task_BlitAnim(void);

void Anim_UnleashIntro(int mode)
{
    u8 *base;
    u8 *data;
    int p;
    int q;
    void (*fp)(volatile u16 *, void *, s32);

    base = galloc_iwram(0x27, 0x782c);
    galloc_iwram(0x28, 0x80 << 7);
    AnimStart(0);
    *(int *)(base + 0x77b4) = 0x18;
    REG_BG2PA = 0x100;
    *(vu16 *)(REG_ADDR_BG2PA + 0x32) = 0x1010;
    switch (mode) {
    case 0:
        data = GetFile((s32)&_FILE_c8);
        break;
    case 1:
        data = GetFile((s32)&_FILE_cf);
        break;
    case 2:
        data = GetFile((s32)&_FILE_b4);
        break;
    case 3:
        data = GetFile((s32)&_FILE_cb);
        break;
    case 4:
    default:
        data = GetFile((s32)&_FILE_be);
        break;
    }
    fp = Func_8001af8;
    fp((volatile u16 *)(0xa0 << 19), data, 0x80);
    *(int *)(base + 0x778c) = 0;
    *(int *)(base + (0xef << 7)) = 3;
    *(int *)(base + 0x7784) = 0x6060606;
    p = 0xc8 << 4;
    StartTask(Func_80cc960, p);
    q = 0x90 << 3;
    StartTask(Task_BlitAnim, q);
}

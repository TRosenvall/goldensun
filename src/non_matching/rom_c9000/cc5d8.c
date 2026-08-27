/* Anim_UnleashIntro -- NOT MATCHING
 *
 * RE-SCREENED IN BATCH 105 UNDER THE BASIC-BLOCK LEVER: still 2 of 80.
 *
 * The lever retired the r0-against-a-shift class on three other functions this
 * round, so it was the first thing tried here. Three placements were compiled
 * -- the shifted palette address assigned before the switch, at the top of the
 * function, and with the length levered alongside it -- and all three give the
 * same two instructions.
 *
 * The reason is visible once the shapes are lined up. gcc ALREADY splits the
 * pair here; what differs is only how much lands in the gap:
 *
 *     rom    mov r0, #0xa0 / ldr r3, =Func_8001af8 / mov r2, #0x80 / lsl r0, #19
 *     ours   mov r0, #0xa0 / ldr r3, =Func_8001af8 / lsl r0, #19 / mov r2, #0x80
 *
 * The lever's job is to make gcc rematerialise a value at the call rather than
 * hold it in a register, and it is already doing that. Which of the remaining
 * arguments gcc schedules into the gap is a separate question the lever does
 * not answer. Worth recording as the lever's OTHER boundary, alongside the
 * straight-line-function one.
 *
 * The original park text follows.
 *
 * Source asm: goldensun/asm/rom_c9000/rom_cc5d8_a_a.s
 * Best screen: 80 instructions against the ROM's 80, 2 differing.
 *
 * BLOCKER CLASS: one transposition in the indirect call's argument setup.
 *
 *     rom    mov r0, #0xa0 / ldr r3, =Func_8001af8 / mov r2, #0x80 / lsl r0, #19
 *     ours   mov r0, #0xa0 / ldr r3, =Func_8001af8 / lsl r0, #19 / mov r2, #0x80
 *
 * The ROM materialises the third argument before the shift that finishes the
 * first. Seven spellings were compiled and all give the same two: the length
 * as a local assigned before `pal <<= 19`, as a local assigned before
 * `pal = 0xa0`, as a local assigned before the GetFile call, the shift folded
 * into the argument, the address through a separate u8 *dst, and an
 * unprototyped `typedef void (*CopyFn)();`. 0x80 is a constant in every one of
 * them and gcc always emits it last.
 *
 * FOUR OTHER TRANSPOSITIONS OF THE SAME FAMILY DID FALL, which is the useful
 * part. The two StartTask calls started out as
 *
 *     ldr r0, =Func_80cc960 / lsl r1, #4        (ROM has these the other way)
 *
 * and writing the argument as its own statements -- arg = 0xc8; ... ;
 * arg <<= 4; StartTask(Func_80cc960, arg); -- puts the shift ahead of the
 * function address, exactly as src/rom_c9000/rom_e0524.c needed for its own
 * `pal <<= 19`. Six differing to two. So the statement-form lever reaches a
 * shift against a POOL LOAD but not a shift against a mov.
 *
 * The switch is solved: `case 4:` must be written explicitly ALONGSIDE
 * `default:`. With default alone gcc drops the table for a comparison tree
 * (59 differing); with both, the five-slot table appears and slot 4 doubles as
 * the out-of-range target, which is what the ROM has.
 *
 * The indirect call follows the CopyFn idiom established in
 * src/rom_c9000/rom_e0524.c -- assigning the callee to a local of
 * function-pointer type is what produces `bl _call_via_r3`.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*CopyFn)(volatile u16 *dst, void *src, s32 len);

extern void Func_8001af8(volatile u16 *dst, void *src, s32 len);
extern void *galloc_iwram(s32 tag, s32 size);
extern void AnimStart(s32 n);
extern void StartTask(void *fn, s32 arg);
extern void Func_80cc960(void);
extern void Task_BlitAnim(void);

void Anim_UnleashIntro(s32 kind)
{
    u8 *buf;
    u8 *data;
    u32 pal;
    CopyFn copy;
    s32 id;
    s32 len;
    s32 arg;

    pal = 0xa0 << 19;
    len = 0x80;
    buf = galloc_iwram(0x27, 0x782c);
    galloc_iwram(0x28, 0x80 << 7);
    AnimStart(0);
    *(s32 *)(buf + 0x77b4) = 0x18;
    REG_BG2PA = 0x100;
    REG_BLDALPHA = 0x1010;
    switch (kind) {
    case 0:
        id = FILE_c8;
        break;
    case 1:
        id = FILE_cf;
        break;
    case 2:
        id = FILE_b4;
        break;
    case 3:
        id = FILE_cb;
        break;
    case 4:
    default:
        id = FILE_be;
        break;
    }
    data = GetFile(id);
    copy = Func_8001af8;
    copy((volatile u16 *)pal, data, len);
    *(s32 *)(buf + 0x778c) = 0;
    *(s32 *)(buf + (0xef << 7)) = 3;
    arg = 0xc8;
    *(s32 *)(buf + 0x7784) = 0x6060606;
    arg <<= 4;
    StartTask(Func_80cc960, arg);
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
}

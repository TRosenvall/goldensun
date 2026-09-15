/* Sprite_SetAnim -- 0x0800ba30, asm/rom_9000/rom_b798_c_a_c.s (single-function
 * file; tools/datacheck.py confirms no data section, so it would convert WHOLE).
 *
 * BLOCKER CLASS: strength reduction of the part-array index, plus two
 * instructions that outlive it. 83 instructions against the ROM's 77, 66
 * differing. The PROLOGUE IS EXACT -- all fifteen instructions of it, including
 * the register roles -- and the residue is entirely inside the loop.
 *
 * WHAT IS SETTLED, so nobody re-derives it:
 *
 *   * THE PARAMETER IS REASSIGNED. The ROM computes `flag = a & 0x80` into r4
 *     and then `a &= 0x7f` IN PLACE (`and r6, r3`, destroying the incoming
 *     value). Two separate locals put the flag in a HIGH callee-saved register,
 *     which Thumb cannot `mov` an immediate into -- costing `mov r2,#128 /
 *     mov r8,r2 / mov r3,r8` -- and the function comes out 85 instructions.
 *     Reassigning the parameter is worth 85 -> 83 and makes the prologue exact.
 *   * The structs are the tree's own (rom_b798_c_a_a_a.c), extended only where
 *     this function names previously-padded bytes: SpriteHost +0x24 (the
 *     current anim), SpritePart +0x02 and +0x15, SpriteInfo +0x05.
 *   * `strb r4, [r5, #0x14]` and `strh r4, [r5, #2]` store r4 inside a branch
 *     guarded by `flag == 0`, so they are SUBSTITUTED CONSTANTS -- plain zeroes,
 *     not copies of the flag. Written as `= 0` they are byte-identical.
 *   * The loop bound is re-read every iteration through a held `&h->count`
 *     (r8), which is what `i < h->count` gives when a call may clobber memory.
 *
 * THE RESIDUE. gcc strength-reduces `h->parts[i]` into a walking pointer
 * (`ldmia r2!, {r5}`) and needs a THIRD spill slot for it, so the frame is 12
 * bytes against the ROM's 8. The ROM recomputes the address every iteration:
 *
 *     rom    lsl r3, r1, #2 / add r3, #0x28 / ldr r5, [r7, r3]
 *     ours   ldmia r2!, {r5}          with r2 spilled at [sp, #8]
 *
 * MEASURED AND INERT, all 66: the access spelled as an explicit byte offset
 * (`*(struct SpritePart **)((u8 *)h + 0x28 + i * 4)`), and the three guards
 * written as nested `if`s instead of `continue` -- the CFG is not the issue.
 *
 * -fno-strength-reduce IS NOT THE ANSWER EITHER, and the number is why: it gets
 * 83 -> 79 and 66 -> 60, so it removes the walking pointer and its spill slot
 * but TWO INSTRUCTIONS STILL REMAIN. Whatever else is wrong is independent of
 * the reduction, so a flag rule would not close this even if one were wanted.
 *
 * NEXT: find the two instructions that survive -fno-strength-reduce first --
 * they are a smaller and better-posed problem than the reduction, and they may
 * be what is really driving the register pressure that makes gcc reduce at all.
 * The batch-268 `i != N` lever does not apply: the bound here is `h->count`,
 * re-read each iteration, not a constant.
 */
#include "gba/types.h"

struct SpritePart {
    /* 0x00 */ s16 id;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 pad_05[2];
    /* 0x07 */ u8 unk_07;
    /* 0x08 */ void *pixels;
    /* 0x0C */ void *unk_0c;
    /* 0x10 */ u32 unk_10;
    /* 0x14 */ u8 unk_14;
    /* 0x15 */ u8 unk_15;
    /* 0x16 */ u8 unk_16;
    /* 0x17 */ u8 pad_17;
};

struct SpriteInfo {
    /* 0x00 */ u8 kind;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ u8 unk_04;
    /* 0x05 */ u8 unk_05;
    /* 0x06 */ u8 unk_06;
    /* 0x07 */ u8 unk_07;
    /* 0x08 */ u8 pad_08[2];
    /* 0x0A */ u8 unk_0a;
    /* 0x0B */ u8 pad_0b;
    /* 0x0C */ void *pixels;
    /* 0x10 */ void *unk_10;
};

struct SpriteHost {
    /* 0x00 */ u8 pad_00[0x18];
    /* 0x18 */ s32 depth;
    /* 0x1c */ u8 pad_1c[4];
    /* 0x20 */ u8 width;
    /* 0x21 */ u8 height;
    /* 0x22 */ u8 corrX;
    /* 0x23 */ u8 corrY;
    /* 0x24 */ u8 anim;
    /* 0x25 */ u8 pad_25[2];
    /* 0x27 */ u8 count;
    /* 0x28 */ struct SpritePart *parts[4];
};

extern struct SpriteInfo *_GetSpriteInfo(s32 id);

int Sprite_SetAnim(struct SpriteHost *h, int a)
{
    struct SpritePart *p;
    struct SpriteInfo *info;
    int flag;
    int i;

    flag = a & 0x80;
    a &= 0x7f;
    if (h->anim != a) {
        for (i = 0; i < h->count; i++) {
            p = h->parts[i];
            if (p == NULL)
                continue;
            if (p->unk_0c == NULL)
                continue;
            info = _GetSpriteInfo(p->id);
            if (a >= info->unk_05)
                continue;
            p->unk_04 = info->unk_04;
            p->unk_10 = ((u32 *)p->unk_0c)[a];
            p->unk_15 = 0x10;
            if (flag == 0) {
                p->unk_14 = 0;
                p->unk_02 = 0;
            }
            if (i == 0) {
                h->corrY = info->unk_07;
                h->corrX = info->unk_06;
            }
        }
        h->anim = a;
    }
    return 0;
}

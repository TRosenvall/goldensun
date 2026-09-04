// fakematch
/* Anim_UnleashIntro  --  0x080ccaec
 *
 * Cut out of goldensun/asm/rom_c9000/rom_cc5d8_a_a.s.  NOTE THE ADDRESS:
 * the park was filed as cc5d8.c and the .s is named rom_cc5d8, but cc5d8 is
 * where that FILE's region starts, not where this function does.  The linked
 * ELF puts Anim_UnleashIntro at 0x080ccaec.  A park's filename is not an
 * address; check nm before writing one down.
 *
 * Sets up the unleash intro animation: allocate two buffers, pick a file by
 * kind, DMA a palette through an indirect call, then start two tasks.
 *
 * PARKED TWICE, at 2 of 80 both times -- once on the original screen and again
 * in batch 105 under the basic-block lever. The residue was one transposition
 * in the indirect call's argument setup:
 *
 *     rom   mov r0, #0xa0 / ldr r3, =Func_8001af8 / mov r2, #0x80 / lsl r0, #19
 *     ours  mov r0, #0xa0 / ldr r3, =Func_8001af8 / lsl r0, #19 / mov r2, #0x80
 *
 * The ROM materialises the third argument BEFORE the shift that finishes the
 * first. Pinning r0 and r2 and assigning them in the ROM order reaches it:
 *
 *     p0 = 0xa0;  copy = Func_8001af8;  p2 = 0x80;  p0 <<= 19;
 *     copy((volatile u16 *)p0, data, p2);
 *
 * The `pal` and `len` locals the park assigned at the top of the function are
 * gone -- the pins replace them, and they were what let gcc choose when to
 * finish the shift.
 *
 * THE SECOND PARK DIAGNOSED THIS EXACTLY RIGHT AND STILL COULD NOT ACT ON IT.
 * Its words: the basic-block lever "is already doing that" -- gcc splits the
 * pair here without help -- and "which of the remaining arguments gcc schedules
 * into the gap is a separate question the lever does not answer." That is
 * precisely correct, and it is the reason a pin was needed rather than another
 * placement of an ordinary local. Seven spellings had already been compiled at
 * that point, all landing on the same two instructions, because every one of
 * them left the choice to gcc. A pin does not leave it to gcc.
 *
 * The pool sits INSIDE this function in the ROM (tools/tryc.py warns about it),
 * so the match was confirmed by the build rather than by the instruction
 * stream alone.
 *
 * KEPT FROM THE PARK, all still load-bearing:
 *   - `case 4:` written explicitly ALONGSIDE `default:`. With default alone
 *     gcc drops the jump table for a comparison tree, 59 differing; with both,
 *     the five-slot table appears and slot 4 doubles as the out-of-range
 *     target, which is what the ROM has.
 *   - The CopyFn function-pointer local, which is what produces
 *     `bl _call_via_r3` rather than a direct call.
 *   - Both StartTask arguments built as their own statements, `arg = 0xc8;
 *     ... arg <<= 4;`, which puts each shift ahead of the pool load. That
 *     lever reaches a shift against a POOL LOAD; the one above reaches a shift
 *     against a mov, and they are not the same lever.
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
    CopyFn copy;
    s32 id;
    s32 arg;

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
    {
        register u32 p0 __asm__("r0");
        register s32 p2 __asm__("r2");
        p0 = 0xa0;
        copy = Func_8001af8;
        p2 = 0x80;
        p0 <<= 19;
        copy((volatile u16 *)p0, data, p2);
    }
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

/* Task_BlitAnim -- MATCHING (0 of 104), pin-free.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_c9000/rom_cd260_a_a_b.c \
 *     asm/rom_c9000/rom_cd260_a_a_b.s --whole
 *
 * SPLIT REQUIRED. asm/rom_c9000/rom_cd260_a_a.s carries TWO functions,
 * Task_BlitAnim and Task_BlitAnim_BG1Wide (the latter still parked at
 * src/non_matching/rom_c9000/80cd358.c). `tools/datacheck.py` on the .s is
 * clean (no data section), and
 *
 *   python3 tools/split_s.py asm/rom_c9000/rom_cd260_a_a.s Task_BlitAnim
 *
 * reports
 *
 *   would write asm/rom_c9000/rom_cd260_a_a_b.s  (1 function(s), 113 lines)
 *   would write asm/rom_c9000/rom_cd260_a_a_c.s  (1 function(s), 90 lines)
 *   would REMOVE asm/rom_c9000/rom_cd260_a_a.s
 *   would rewrite stage1.ld
 *
 * so this file installs as src/rom_c9000/rom_cd260_a_a_b.c against
 * asm/rom_c9000/rom_cd260_a_a_b.s, and _a_a_c.s keeps Task_BlitAnim_BG1Wide.
 *
 * PINS: 0.  FAKEMATCH: no.  DEVICES: none.
 *
 * WHAT THE PARK CLAIMED, AND WHAT SURVIVED.
 *
 * The park (3 of 104, batch-319-verified) called the residue "the
 * POOL-LOADS-FIRST class ... where the only construct known to reach it is
 * register pinning with inline asm", citing
 * src/non_matching/overlays/pool_load_first.c.
 *
 *   - Its OBSERVATION survived exactly: three encodings, indices 32-34, in
 *     case 1's first call, `ldr r0,=0x6004000` third for us and fifth in ROM.
 *   - Its VERDICT was wrong, and its own evidence refuted it: case 0
 *     (.Lcd298) is byte-exact and has the pool load THIRD. The ROM's two
 *     sibling blocks disagree with each other, so no "pool loads first/last"
 *     rule can be the mechanism.
 *   - Its batch-105 BASIC-BLOCK LEVER finding (three separate 0x4000 locals,
 *     one per use site, to stop the constant being held in a callee-saved
 *     register) is load-bearing and kept: inlining len1 measures 28 of 104.
 *
 * THE MECHANISM: the int-RETURN LEVER, through the function-POINTER type.
 *
 * `-fno-schedule-insns2` prints the pre-sched order of case 1's first call as
 *
 *     ldr r3,=Func / ldr r0,=0x6004000 / mov r1,r5 / mov r2,#128 / lsl r2,#7
 *
 * sched2 hoists `mov r2,#128` (priority 2: it feeds the lsl which feeds the
 * call) and then emits the rest in dependent-count-then-LUID order. Because
 * CopyFn was declared VOID, the call is `*call_insn`, which only (use)s r0 and
 * never SETS it, so the output dependence from the later `mov r0, r5` (the
 * second call's first argument) attaches to the r0 ARGUMENT FILL rather than
 * to the call -- giving that fill the same dependent count as `ldr r3` and
 * letting its low LUID put it third. Declaring CopyFn int-returning makes the
 * call `*call_value_insn`, which SETS r0; the output dependence moves onto the
 * call, the r0 fill loses its extra dependent, and it sorts LAST -- exactly
 * where the ROM has it.
 *
 * This is docs/elevation.md's "THE int-RETURN LEVER, FINALLY EXPLAINED -- IT
 * IS A DEPENDENT COUNT", reaching through a pointer type the way
 * src/rom_b5000/rom_b5a0c_c_c_a_a_a_b.c records it. Note it fires on CopyFn
 * ONLY: FillFn -> int alone is still 3, and the four sibling files in this
 * directory that declare `extern void Func_8001af8(volatile u16 *, void *,
 * s32)` all match with the VOID spelling, so the lever's direction is per call
 * site, not per callee.
 *
 * Everything else the park got right is unchanged: the `_call_via_r3` idiom
 * for both indirect calls, the two-word read of iwram_3001eec through
 * `pp[0]`/`pp[1]` with `pp[1]` read inside the guard, 0x7780 spelled so gcc
 * derives it from the 0x7824 already in a register (`sub r2, #0xa4`), and the
 * tail where the two writes to +0x7820 cross-jump into one `str`.
 */
#include "gba/types.h"

typedef int (*CopyFn)(volatile u16 *dst, void *src, u32 len);
typedef void (*FillFn)(void *dst, u32 len, u32 value);

extern int Func_8001af8(volatile u16 *dst, void *src, u32 len);
extern void Func_80008d8(void *dst, u32 len, u32 value);
extern void BlitFade_Div2(void *src, void *dst, u32 len);
extern void BlitFade_Div4(void *src, void *dst, u32 len);
extern void BlitFade_Sub(void *src, u32 amt, void *dst, u32 len);
extern void BlitFade_Add(void *src, u32 amt, void *dst, u32 len);
extern unsigned char *iwram_3001eec;

void Task_BlitAnim(void)
{
    unsigned char **pp;
    unsigned char *b;
    void *s;
    CopyFn copy;
    FillFn fill;
    u32 val;
    u32 len1;
    u32 len2;
    u32 len0;

    len1 = 0x4000;
    len2 = 0x4000;
    len0 = 0x4000;
    pp = &iwram_3001eec;
    b = pp[0];
    if (*(int *)(b + 0x7824) == 1) {
        s = pp[1];
        switch (*(int *)(b + 0x7780)) {
        case 0:
            copy = Func_8001af8;
            copy((volatile u16 *)0x6004000, s, len0);
            break;
        case 1:
            copy = Func_8001af8;
            copy((volatile u16 *)0x6004000, s, len1);
            val = *(u32 *)(b + 0x7784);
            fill = Func_80008d8;
            fill(s, len2, val);
            break;
        case 2:
            if (*(int *)(b + 0x7784) == 0x32)
                BlitFade_Div2(s, (void *)0x6004000, 0x4000);
            else
                BlitFade_Div4(s, (void *)0x6004000, 0x4000);
            break;
        case 3:
            BlitFade_Sub(s, *(u32 *)(b + 0x7784), (void *)0x6004000, 0x4000);
            break;
        case 4:
            BlitFade_Add(s, *(u32 *)(b + 0x7784), (void *)0x6004000, 0x4000);
            break;
        }
        *(int *)(b + 0x7824) = 0;
        *(int *)(b + 0x7820) = 1;
    } else {
        *(int *)(b + 0x7820) = *(int *)(b + 0x7820) + 1;
    }
}

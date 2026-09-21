/* Func_80aad10 (PaintDjinnBackground, 0x080aad10) -- NON-MATCHING, 94 lines against 82.
 * Blocker class: NEW -- CSE-SHARED EXPENSIVE CONSTANTS ACROSS SIBLING CALLS.
 * Never attempted before batch 276.
 *
 * asm/rom_a1000/rom_aa538_c_c_a_c_a.s (2 functions, so landing needs a split).
 *
 * EVERYTHING IN THIS FUNCTION IS RIGHT EXCEPT ONE THING, and that one thing is a
 * blocker class this corpus has not recorded before. gcc CSEs the three constants
 * shared between the `Func_8001af8` and `Func_80008d8` call sites -- 0x6004000,
 * 0x2000 and 0x5000080 -- into pseudos that then live ACROSS the calls and take
 * r9/r10/r11. That costs +4 prologue, +4 epilogue and +4 setup `mov`s, and it
 * pushes the `Func_8001af8` pointer out of r6 into r8, so all three indirect calls
 * become `bl _call_via_r8`.
 *
 * THE MECHANISM, and why cheap constants do not have the problem.
 * `precompute_register_parameters` makes a pseudo for each EXPENSIVE constant
 * argument -- `.00.rtl` has six separate constant sets (lines 722, 726, 764, 796,
 * 800, 832) -- and then local CSE shares the earlier pseudo at the later site:
 * `.03.cse` substitutes at insns 86, 88 and 98, e.g.
 * `(set (reg:SI 0 r0) (reg:SI 44))` carrying `REG_EQUAL (const_int 100679680)`.
 * `.18.greg` then puts those pseudos in r9/r10/r11. The cheap 0x80 goes STRAIGHT to
 * the hard argument register instead of via a pseudo, so `invalidate_for_call`
 * kills it and it is correctly re-materialised at both sites.
 *
 * GROUND TRUTH CHECKED AGAINST baserom.gba (0x080aad10-0x080aae14), because "the ROM
 * did not share a constant" is exactly the kind of claim worth verifying in bytes:
 * both 0x6004000 loads point at the SAME pool word 0x080aadd4 and both 0x5000080
 * loads at 0x080aaddc, while 0x2000 is BUILT with `movs #128 / lsls #6` TWICE. So
 * the ROM re-materialises where we share.
 *
 * AND THE DECISIVE DIAGNOSTIC, which is the reason this park is worth keeping.
 * With the fill-site constants deliberately PERTURBED so nothing can be shared
 * (`t1_diag.c`), the whole body lines up with the ROM instruction-for-instruction,
 * and the only residue is the 2 insns from the perturbed 0x2004 pooling instead of
 * `mov`/`lsl`. NOTHING ELSE IN THE FUNCTION IS WRONG. objcmp agrees: all 15
 * relocations are the ROM's, same symbols in the same order, differing only
 * `_call_via_r6` -> `_call_via_r8` at three sites.
 *
 * MEASURED, all 94 lines / 93 differing -- no spelling of the argument reaches it:
 *   `int`-typed pointer parameters                        94 / 93
 *   `volatile u16 *` destinations                         94 / 93
 *   unprototyped `void (*copy)()`                         94 / 93
 *   `int`-returning pointer types                         94 / 93
 *   pinning the shared constants into caller-saved hard
 *     registers with `register ... __asm__("r1")`          85 / 83  (WORSE)
 *
 * The pin result matters: the scaffolding route does not pay here either, so this is
 * not a fakematch candidate.
 *
 * NEXT, AND THIS IS A TARGETING NOTE AS MUCH AS A PARK. Before attempting any
 * function that makes repeated indirect calls with the same VRAM address or DMA
 * length, grep for `_call_via_rN` together with a repeated pool-sized literal -- if
 * the ROM re-materialises the literal and we would share it, this class is why, and
 * no argument spelling will move it. Two candidate handles for anyone who wants to
 * break the class: whether a constant can be made cheap enough to skip
 * precompute_register_parameters, and whether a flag in the CSE_CFLAGS family
 * (-fno-cse-follow-jumps / -fno-cse-skip-blocks) suppresses the sharing without
 * collateral -- neither was tested here.
 */
#include "dma.h"


extern u8 *iwram_3001f2c;
extern u8 Data_af26c[];

extern void Func_80a10d0(void *win, int a, int b, int c, int d, int e);
extern void WaitFrames(int n);
extern void Func_8001af8(void *a, void *b, int n);
extern void Func_80008d8(void *a, int n, int v);
extern void _Func_8021a18(void *a);
extern void *GetSpritePalette(void);
extern void Func_80aac84(int add);
extern int Func_80aafb8(void *state);

int Func_80aad10(void)
{
    u8 *base;
    u8 *state;
    void (*copy)(void *, void *, int);
    void (*fill)(void *, int, int);

    base = iwram_3001f2c;
    state = *(u8 **)(base + 0x184);
    Func_80a10d0(base + 0x30, 0, 5, 0x1e, 0xf, 2);
    WaitFrames(1);
    copy = Func_8001af8;
    copy(state + 0xa8, (void *)0x6004000, 0x2000);
    copy(state + 0x20a8, (void *)0x5000080, 0x80);
    fill = Func_80008d8;
    fill((void *)0x6004000, 0x2000, 0x33333333);
    fill((void *)0x5000080, 0x80, 0x55555555);
    _Func_8021a18((void *)0x6005000);
    copy((void *)0x60052c0, Data_af26c, 0x20);
    DMA3_SET(GetSpritePalette(), (void *)0x50000a0, 0x80000010);
    *(u16 *)0x50000bc = *(u16 *)0x50001e8;
    DMA3_SET((void *)0x50001e0, (void *)0x50000e0, 0x80000010);
    Func_80aac84(8);
    *(u16 *)0x50000e8 = *(u16 *)0x50001e8;
    *(u16 *)0x50000c8 = *(u16 *)0x50001e8;
    Func_80aafb8(state);
}

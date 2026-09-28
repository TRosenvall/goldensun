/* CamelotLogo (0x080f2d54) -- NON-MATCHING.
 * NON-MATCHING: 138 encodings of 143 differ (objcmp), ours 140.
 * THE COUNTS DISAGREE AT THE DEFAULT FLAGS, so 138 is not a distance. Under
 * -ffixed-r7 the same source is 144 encodings against 143 with 109 differing
 * (measured with scratch_elev/b291/H/cnt.py, which is objcmp's comparison with
 * an extra cflag; objcmp itself takes no cflags).
 *
 * TWO FINDINGS, one of them large.
 *
 *  1. THIS TU LOOKS LIKE A -ffixed-r7 FILE, and that is testable from the
 *     PROLOGUE ALONE. The ROM opens
 *         push {r5, r6, lr} / mov r6, r8 / push {r6}
 *     -- three callee-saved values, and it SKIPS r7 to reach r8, which
 *     REG_ALLOC_ORDER {3,2,1,0,12,14,4,5,6,7,8,10,...} says cannot happen while
 *     r7 is available. At the default flags this source emits
 *         push {r5, r6, r7, lr}
 *     and is 4 instructions short. Adding -ffixed-r7 (the Makefile already has a
 *     FIXEDR7_CFLAGS group) reproduces the ROM's prologue and epilogue exactly
 *     and closes the 4-instruction gap. NOTHING SOURCE-LEVEL WAS FOUND THAT DOES
 *     THIS: r7 is skipped, not merely unused, so no spelling can reach it.
 *     This is a flag-row candidate for asm/rom_f2000/rom_f2028_c_a.o and it
 *     needs the user's decision -- and the same test should be run on the
 *     sibling function at 0x080f2b70 in the same .s before either is landed.
 *
 *  2. TWO .sym CANDIDATES, both with in-function control. `ldr r6, =0x19` here
 *     and `ldr r5, =0x18` in the sibling are pooled GetFile ids where
 *     `movs r0, #0x19` is a legal Thumb immediate -- the named-constant
 *     signature. file_table.sym already carries _FILE_13..17, _FILE_1a and
 *     _FILE_1c, so _FILE_18 = 0x18 and _FILE_19 = 0x19 fill a hole in a run.
 *     This draft uses `GetFile((int)&_FILE_19)`, the tree's established idiom
 *     (src/rom_9000/rom_11568_a_c_c_a.c:95). Each COMPLETES its function's
 *     pooled-constant account: no other constant in either function pools.
 *     The R_ARM_ABS32 against _FILE_19 is why objcmp reports RELOCATIONS differ
 *     against the reference's plain literal; that is expected and goes away once
 *     the symbol is in a .sym the link resolves.
 *
 * LOOP SHAPES, both measured:
 *  - THE ANIMATION LOOP IS A `while` WITH ITS BODY'S TAIL PEELED. The ROM
 *    computes the frame index in the pre-header, jumps forward into the middle of
 *    the loop, and computes the frame index again at the end of the back-edge
 *    block; the DMA and the key test are shared. Writing
 *        frame = ...; DMA(...);  while ((gKeyPress & 9) == 0) { WaitFrames(1);
 *        i++; if (i > 0x77) break; frame = ...; DMA(...); }
 *    -- the frame/DMA pair deliberately duplicated in the SOURCE -- lets
 *    find_cross_jump merge the two DMA tails and reproduces it: 131 instructions
 *    -> 139. A do-while with a `break` on the key test (131) and a `for(;;)` with
 *    two breaks (131) both leave the loop unrolled and 12 instructions short.
 *  - THE iwram_3001ad0 CLEAR LOOP COUNTS UP, so its counter is UNSIGNED. With
 *    `int` gcc reverses it (`movs r5,#3 / subs r5,#1 / bge`); the ROM has
 *    `movs r5,#0 / adds r5,#1 / cmp r5,#3 / bls` -- an unsigned bound, which
 *    check_dbra_loop will not reverse.
 *  - A NAMED `short *h = iwram_3001ad0;` for the clear loop, the [5] store and
 *    the HDMA source: 120 differing -> 109. Do NOT inline the array at each site.
 *
 * REMAINING RESIDUE under -ffixed-r7, 144 against 143: one allocno rotation.
 * The ROM puts the GetFile id in r6 (straight from the pool, no copy) and
 * &iwram_3001ad0 in r8; this source puts the id in r8 and pays a `mov r8, r3`
 * for it, then uses r5 where the ROM uses r6 for the walking gBuffer pointer.
 * Same class as src/non_matching/rom_c0/800615c.c.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_f2000/f2d54.c \
 *     asm/rom_f2000/rom_f2028_c_a.s --func CamelotLogo
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern u8 iwram_3001d18;
extern short iwram_3001ad0[];
extern volatile unsigned int iwram_3001e40;
extern volatile unsigned int gKeyPress;
extern unsigned char gBuffer[];
extern int _FILE_19;
extern void ClearTasks(void);
extern void Func_8003b70(int a);
extern void ClearVRAM(void);
extern void WaitFrames(int n);
extern void *GetFile(int id);
extern void DecompressLZ(void *src, void *dst);
extern void Func_800479c(void);
extern void Func_8003c3c(int a);
extern void Func_8003ce0(void);

int CamelotLogo(void)
{
    unsigned char *p;
    short *h;
    int i;
    unsigned int u;
    int frame;
    int id;

    iwram_3001d18 = 1;
    id = (int)&_FILE_19;
    ClearTasks();
    Func_8003b70(1);
    ClearVRAM();
    WaitFrames(1);
    REG_BG2CNT = 0x685;
    REG_DISPCNT = 0x1440;
    h = iwram_3001ad0;
    h[5] = 0;
    p = gBuffer;
    DecompressLZ(GetFile(id), p);
    DMA3_SET(p, (void *)0x5000000, 0x84000070);
    p += 0x1c0;
    DMA3_SET(p, (void *)0x6003000, 0x84000200);
    p += 0x800;
    DMA3_SET(p, (void *)0x6004000, 0x84001000);
    p += 0x4000;
    for (u = 0; u < 4; u++) {
        h[u * 2 + 1] = 0;
        h[u * 2] = 0;
    }
    DMA3_SET(h, (void *)REG_ADDR_BG0HOFS, 0x84000004);
    Func_800479c();
    ClearVRAM();
    Func_8003c3c(1);
    Func_8003ce0();
    *(vu16 *)0x4000000 = 0x1540;
    i = 0;
    frame = (iwram_3001e40 >> 3) & 3;
    DMA3_SET(p + frame * 0x400, (void *)0x6004100, 0x840000d0);
    while ((gKeyPress & 9) == 0) {
        WaitFrames(1);
        i++;
        if (i > 0x77)
            break;
        frame = (iwram_3001e40 >> 3) & 3;
        DMA3_SET(p + frame * 0x400, (void *)0x6004100, 0x840000d0);
    }
    return 0;
}

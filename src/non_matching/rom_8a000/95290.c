/* StartThunder2 -- NOT MATCHING, a SIZE mismatch: ref 184 bytes against ours 192.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/nthunc.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/95290.c \
 *     asm/rom_8a000/rom_944ec_a_a_c_c_a.s --func StartThunder2
 *
 * The original prose park is preserved below.
 */
#include "dma.h"

extern void *galloc_ewram(int kind, int size);
extern unsigned char *iwram_3001ed0;
extern void Func_8090a5c(int a, void *b, void *c, int d);
extern void Func_809088c(void *a, void *b, void *c, int d);
extern void StartTask(void *f, int n);
extern void Task_Thunder(void);

void StartThunder2(int a, int b)
{
    unsigned char *buf;
    unsigned char *y;
    unsigned char *p;

    y = iwram_3001ed0;
    buf = galloc_ewram(0x1e, 0x1f88);
    DMA3_CLEAR(buf, 0x1f88);
    Func_8090a5c(a, y, buf, 1);
    p = buf + (0xa8 << 4);
    Func_8090a5c(b, y, p, 1);
    Func_809088c(p, buf, buf + (0xa8 << 5), 0xc);
    Func_8090a5c((int)buf, 0, y + (0xe0 << 4), 1);
    *(unsigned short *)(buf + (0xfc << 5)) = 0x78;
    *(unsigned short *)(buf + 0x1f82) = 0;
    StartTask(Task_Thunder, 0xc8 << 4);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* StartThunder2 -- asm/rom_8a000/rom_944ec_a_a_c_c_a.s
 *
 * BLOCKER: REGISTER ALLOCATION at the PARAMETER COPY
 *
 * 32 of 74 differing (ours 76 lines).  The first difference is instruction 6
 * and everything after it is that one decision propagating:
 *
 *     rom  mov r6, r0   (param a -> r6)   ... mov r8, r3  (y -> r8)
 *     ours mov r8, r0   (param a -> r8)   ... mov r6, r3  (y -> r6)
 *
 * `y` outlives `a` -- it is used at three call sites, `a` at one -- and the ROM
 * gives the LONGER-lived value the high register.  gcc does the opposite.  The
 * two extra instructions we emit are downstream of the same thing: a spare pool
 * entry for 0x78 and the `b` over it that gcc then needs.
 *
 * WHAT IS ALREADY RIGHT: the whole DMA block is exact.  `DMA3_CLEAR(buf,
 * 0x1f88)` from include/dma.h reproduces `mov r0,sp / mov r3,#0 / str r3,[r0] /
 * mov r1,r5 / ldr r3,=REG_DMA3SAD / ldr r2,=0x850007e2 / stmia r3!, {r0,r1,r2}
 * / sub r3,#0xc` byte for byte -- the control word 0x850007e2 decodes as
 * 0x85000000 | (0x1f88 / 4), i.e. the size passed to galloc_ewram.  So is the
 * entire tail from Func_809088c onward apart from the pool entry.
 *
 * MEASURED (all 76 lines, first diff at 6):
 *   buf assigned before y                                       39
 *   y assigned before buf                                       32  <- best
 *   ... + first parameter copied into a local (agent4's lever)   39
 *   ... + unsigned short instead of short for both stores        32
 *   ... + named local for the 0xe0 << 4 offset                   32
 *   ... + `y += 0xe0 << 4;` as its own statement, mirroring the
 *         ROM's in-place `add r8, r3`                            32
 *   declaration order y,buf,p / p,y,buf / y,p,buf                32 each
 *
 * Best C is scratch/nthunc.c.  Both of the levers that have moved this exact
 * signature elsewhere -- copying the first parameter (batch 115) and swapping
 * declarations (batch 115) -- are measured here and are inert, which is worth
 * recording: they are real but they are not general.
 *)

*/
#endif

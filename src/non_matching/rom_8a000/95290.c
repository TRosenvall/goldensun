/* StartThunder2 -- asm/rom_8a000/rom_944ec_a_a_c_c_a.s
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
 */

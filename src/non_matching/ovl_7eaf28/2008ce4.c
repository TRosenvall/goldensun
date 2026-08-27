/* OvlFunc_960_2008ce4 -- asm/overlays/rom_7eaf28/ovl_314_c_c_a.s
 *
 * BLOCKER: HImode TRUNCATION SIGNEDNESS (asr where the ROM has lsr)
 *
 * 1 of 27 differing.  26 lines are exact, including the pooled 0x40, both
 * shifts of the colour build, the orr grouping, and the pool-jump tail:
 *
 *     rom  orr r3, r2 / lsl r3, #0x10 / ldr r2, =0x500019e / lsr r3, #0x10
 *     ours orr r3, r2 / lsl r3, #0x10 / ldr r2, =0x500019e / asr r3, #0x10
 *
 * Both encode the same 16-bit store through the following strh, so the choice
 * is free to gcc -- which is exactly why it is hard to steer.  The truncation
 * itself only appears at all when the destination is VOLATILE; without that
 * gcc drops the pair entirely (strh truncates) and the screen is 8 of 27.
 *
 * The best C so far, at 1 of 27, is scratch/m8ce4b.c:
 *
 *     unsigned short v, n;
 *     v = iwram_3001e40 & 0x3f;
 *     if (v > 0x1f) v = 0x40 - v;
 *     n = (v >> 1) + 7;
 *     n = n | ((n << 10) | (n << 5));
 *     *(volatile unsigned short *)0x500019e = n;
 *
 * MEASURED:
 *   unsigned short n, volatile store                      1   <- best
 *   ... with an explicit (unsigned short) cast on the or   1
 *   ... with a separate unsigned short w for the result    1
 *   ... storing through a named volatile u16 * local       1
 *   short n (signed), volatile store                      14  (and 29 lines)
 *   unsigned short n, NON-volatile store                   8  (no lsl/lsr at all)
 *   int n, volatile store                                  8  (no lsl/lsr at all)
 *   int n + & 0xffff                                      13  (r1/r2 cascade from line 5)
 *   unsigned int n + & 0xffff                             13  (same cascade)
 *   assigning the result back into v instead of n          5
 *
 * Note the shape of the evidence: every spelling that PRODUCES the truncation
 * produces `asr`, and every spelling that would produce `lsr` (the & 0xffff
 * forms) re-allocates the whole function from line 5.  Declaring n unsigned is
 * not enough -- gcc-2.96 picks the sign-extending pattern here regardless, so
 * the lever, if there is one, is not the type of n.
 */

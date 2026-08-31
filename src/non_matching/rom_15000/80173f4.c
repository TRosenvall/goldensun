/* Func_80173f4 -- asm/rom_15000/rom_15e8c_c_a_c_a_a_b.s
 *
 * BLOCKER: gcc HOISTS the pooled address offsets. 18 of 37, LENGTH EXACT.
 *
 * A task setup: upload sprite graphics, write six halfwords into the module
 * block, then register a per-frame task. The call, its arguments, all six
 * stores and the StartTask priority reproduce.
 *
 * ONE LEVER LANDED AND IS KEPT -- the stored constants through int locals.
 * Written as bare literals, 9, 0xa and 0xf into HALFWORD stores are HImode and
 * gcc pools them (`ldr r3, =0x9` against the ROM's `mov r3, #0x9`), which is
 * the operand-mode rule from batch 155 that also closed Func_8011b00. Routing
 * each through an `int` local makes all three `mov` and takes the function to
 * the exact length.
 *
 * WHAT REMAINS is the address offsets. The ROM loads each immediately before
 * its use and REUSES THE SAME REGISTER for the value:
 *
 *     rom    ldr r3, =0x12b0 / add r2, r5, r3 / mov r3, #0x9 / strh r3, [r2]
 *     ours   ldr r0, =0x12b0 / ldr r1, =0xea8 / add r3, r5, r0 / mov r2, #0x9
 *
 * gcc loads two or three of the pooled offsets up front and keeps them live,
 * so it needs more registers and cannot reuse one for the value.
 *
 * MEASURED:
 *   bare literals for the stored values        39 lines, 10 differ
 *   one `int` local reused for all of them     37 lines, 25 differ
 *   a separate int local per stored value      37 lines, 18 differ  <- best
 *   + a pointer local per store, to shorten
 *     each address's live range                37 lines, 32 differ
 *
 * The trade is worth stating: the literal form is CLOSER BY COUNT and two lines
 * long; the int-local form is the exact length and further by count. On a
 * function whose only structural defect is length, the length is the one that
 * matters, so the int-local form is kept.
 *
 * The last line is the informative negative. Giving each address its own
 * pointer local should shorten its live range and stop the hoist; it does the
 * opposite, because each pointer is then a named value gcc must place and the
 * pressure goes UP. Shortening a live range by naming it is self-defeating --
 * the same shape as Func_80ae9f0, where writing the ROM's in-place base advance
 * made that function six worse.
 */
extern int iwram_3001e8c;
extern int UploadSpriteGFX(int a, int b, int c);
extern void StartTask(void *fn, int prio);
extern void Func_801789c(void);

void Func_80173f4(void)
{
    char *p;
    int v1;
    int v2;
    int v3;
    int zero;

    p = (char *)iwram_3001e8c;
    *(unsigned short *)(p + 0x12b8) = UploadSpriteGFX(0x5f, 0x80 << 6, 0);
    v1 = 9;
    *(unsigned short *)(p + 0x12b0) = v1;
    v2 = 0xa;
    *(unsigned short *)(p + 0xea8) = v2;
    zero = 0;
    *(unsigned short *)(p + 0xeac) = zero;
    v3 = 0xf;
    *(unsigned short *)(p + 0xeae) = v3;
    *(unsigned short *)(p + 0x12b2) = zero;
    StartTask(Func_801789c, 0xc8 << 4);
}

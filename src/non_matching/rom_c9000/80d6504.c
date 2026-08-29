/* Task_SpinCamera -- NON-MATCHING.
 * Blocker class: REGISTER CHOICE, plus one extra callee-saved register.
 * 39 lines against the ROM's 41, 28 differing, and the opening -- including
 * the part that looked hardest -- is exact.
 *
 * SOLVED, AND THE LEVER IS NEW: ONE SYMBOL'S ADDRESS DERIVED FROM ANOTHER.
 * The ROM reaches two globals 0x6c apart by loading one address and
 * subtracting:
 *
 *     ldr r3, =iwram_3001eec / ldr r2, [r3] / sub r3, #0x6c / ldr r1, [r3]
 *
 * Two separate `extern`s give two pool loads and never this. Writing the
 * second as an offset FROM THE FIRST SYMBOL'S ADDRESS reproduces it exactly:
 *
 *     st   = iwram_3001eec;
 *     view = *(char **)((char *)&iwram_3001eec - 0x6c);
 *
 * This is the offset-clobber lever applied to a SYMBOL address rather than a
 * struct offset -- the address register is reused for the second load, so it
 * has to be modified in place. Worth trying wherever two globals are reached
 * a fixed distance apart.
 *
 * WHAT REMAINS. gcc reads the mode word into r4 where the ROM uses r3, and
 * saves r5 where the ROM saves nothing -- one value too many live. The ROM
 * overwrites the state pointer with the amount it loads from it, so state is
 * dead from that point; in C the two have different types and cannot share a
 * variable, so the source cannot express the reuse.
 *
 * Tried:
 *   - the two loads through a named `char **p`, clobbering p between them:
 *     identical, 28 differing
 *   - the mode assignment written without a temporary, `if (...) *mode = 2;
 *     else *mode = 0;`: THREE lines short instead of two, so the temporary is
 *     load-bearing and is kept
 *
 * The signed halving is right and needed no work: `/ 2` on an int gives the
 * ROM's `lsr #31 / add / asr #1`.
 */
extern char *iwram_3001eec;

void Task_SpinCamera(void)
{
    char *st;
    char *view;
    int *mode;
    int amt;
    int v;

    st = iwram_3001eec;
    view = *(char **)((char *)&iwram_3001eec - 0x6c);
    mode = (int *)(st + 0x77b0);
    if (*mode == 1) {
        amt = *(int *)(st + 0x77ac);
        *(unsigned short *)(view + 0x36) = *(unsigned short *)(view + 0x36) + amt;
        *mode = 0;
    } else {
        amt = *(int *)(st + 0x77ac) / 2;
        *(unsigned short *)(view + 0x36) = *(unsigned short *)(view + 0x36) + amt;
        if (*mode != 2)
            v = 2;
        else
            v = 0;
        *mode = v;
    }
}

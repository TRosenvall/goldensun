/* Func_80b6148 (RunBattleTransition)  --  0x080b6148, the ONLY function in
 * asm/rom_b5000/rom_b5a0c_c_c_a_a_a_a_c_c_a.s.  datacheck: no data; the .word
 * runs in the reference are gcc's own mid-function literal pools, and
 * `grep -ci func_start` = 1.  Whole-file conversion, no split.
 *
 * EXACT: 560 bytes, 254 encodings and 13 relocations identical
 *   (objcmp --whole: "OK whole file -- 560 bytes, 254 encodings and 13
 *    relocations identical").
 * NO SHIMS: no PIN macro, no "+r" barrier, no volatile, no DMA3_SET, no .equ,
 * no new .sym entry, no per-file flag.
 *
 * TEMPLATE.  src/rom_b5000/rom_b5a0c_c_c_a_a_a_a_c_b.c (Func_80b60a0) is the
 * same link handshake one step long and already landed.  It supplies the extern
 * set verbatim: the `*(unsigned char **)&iwram_3001e74` load, the
 * `(1 ^ p[0x50]) * 24` peer stride off &ewram_2002024, the &ewram_2002224
 * mailbox, and the `(iwram_3001f64 & 3) != 3` link poll.  This function runs the
 * same poll five times, writing a fresh pair of halfword tokens each time.
 *
 * THREE LOAD-BEARING CONSTRUCTS, each measured by dropping it alone.
 *
 * 1. `int v[5];` -- AN UNUSED LOCAL ARRAY FOR THE BARE `sub sp, #0x14`.
 *    Nothing in the body touches sp.  Dropped: 37 of 254 differ.  This is
 *    docs/elevation.md's "An unused local array reproduces a bare sub sp, #N"
 *    at 20 bytes; five words, and the body needs none of them.
 *
 * 2. THE LAST `return -1;` IN THE FUNCTION MUST BE STEP E'S, which forces step
 *    E alone to be written with gotos so its success block comes AFTER that
 *    return.  gcc-2.96 folds every `return -1;` into one block and the SURVIVING
 *    copy is the textually last one, so its position decides whether the four
 *    `bne` exits in steps C and D reach the shared epilogue with a plain
 *    conditional branch (the ROM) or need gcc's `bcc near / b far` long-jump
 *    pair.  Written like steps A-D -- success block first, a trailing
 *    `return -1;` after the block -- it is 104 of 254 differ and 568 bytes, i.e.
 *    four extra instructions, all of them long-jump halves.  This is the
 *    concrete pay-off of "Two textually separate `return -1;` statements ALWAYS
 *    merge": the merge point is a CODE-LAYOUT lever, not just a size one.
 *
 * 3. THE PRE-LOOP `WaitFrames(1);` OF STEP A.  Only step A waits once before
 *    its poll loop; steps B-E enter their loop at the test.  Dropped: 226 of 254
 *    differ, 556 bytes (one instruction short).
 *
 * INERT, measured: the poll loops of steps A-D as `goto`/label chains instead of
 * `for (;;)` (both EXACT, 254 encodings).  So only step E's goto is real; the
 * `while (C) B` block order I first read out of the reference's `b <test>` is
 * what gcc gives a `for (;;)` here anyway.  An all-`for (;;)` version, step E
 * included, is 55 of 254 differ and 572 bytes -- the regression is construct 2,
 * not the loop form.
 *
 * THE FIVE TOKEN PAIRS NEED NO HELP.  The reference loads every one of
 * 0x65 0x78 0x54 0x55 0x72 0x6e 0x45 0x58 0x43 0x74 0x75 0x52 0x4e from a
 * literal pool even though each fits `mov rD, #imm8`; that is not a symbol tell
 * here.  gcc-2.96 has no immediate alternative for an HImode store source, so
 * the plain `q[0] = 0x65;` spelling pools them, in source order, one pool word
 * each, with 0x45 commoned across q[0] and q[2] exactly as the ROM does.
 *
 * ALSO WORTH NOTING (a contradiction to docs/elevation.md section 1b, measured
 * here): gcc emits these as `ldrh r3, .L48` while the reference writes
 * `ldr r3, .Lb6198`.  Section 1b calls that "the wrong instruction reading it"
 * and treats the ldrh/ldr split as a real blocker.  It is not, at least for a
 * PC-relative pool load in thumb: there is no PC-relative `ldrh` encoding, gas
 * assembles `ldrh rD, <label>` in thumb to the same `ldr rD, [pc, #N]` word, and
 * this file is byte-identical with 13 of 13 relocations while carrying nine such
 * loads.  The landed twin Func_80b60a0 has the same pair and also matched.  So
 * before parking on a "pool load width" residue, check it at the ENCODING level
 * with objcmp rather than in the text.
 */
extern unsigned int iwram_3001e74;
extern unsigned int ewram_2002024;
extern unsigned int ewram_2002224;
extern unsigned short iwram_3001f64;
extern int WaitFrames(int frames);

int Func_80b6148(void)
{
    int v[5];
    unsigned char *p;
    unsigned short *q;
    unsigned short *r;
    int n;

    p = *(unsigned char **)&iwram_3001e74;
    n = 0;
    if (p[0x44] != 0) {
        r = (unsigned short *)((unsigned char *)&ewram_2002024 + (1 ^ p[0x50]) * 24);
        q = (unsigned short *)&ewram_2002224;
        if (p[0x52] != 0)
            return -1;
        q[0] = 0x65;
        q[1] = 0x78;
        q[4] = 0x54;
        q[5] = 0x55;
        WaitFrames(1);
        for (;;) {
            if ((iwram_3001f64 & 3) != 3) {
                n++;
                if (n > 0x18)
                    return -1;
            } else {
                n = 0;
                if (q[2] != r[2])
                    return -1;
                if (q[3] != r[3])
                    return -1;
                if (q[0] == r[0] && q[1] == r[1] && q[4] == r[4] && q[5] == r[5])
                    break;
            }
            WaitFrames(1);
        }
        q[6] = 0x72;
        q[7] = 0x6e;
        for (;;) {
            if ((iwram_3001f64 & 3) != 3) {
                n++;
                if (n > 0x18)
                    return -1;
            } else {
                n = 0;
                if (q[4] != r[4])
                    return -1;
                if (q[5] != r[5])
                    return -1;
                if (q[6] == r[6] && q[7] == r[7])
                    break;
            }
            WaitFrames(1);
        }
        q[0] = 0x45;
        q[1] = 0x58;
        q[2] = 0x45;
        q[3] = 0x43;
        for (;;) {
            if ((iwram_3001f64 & 3) != 3) {
                n++;
                if (n > 0x18)
                    return -1;
            } else {
                n = 0;
                if (q[6] != r[6])
                    return -1;
                if (q[7] != r[7])
                    return -1;
                if (q[0] == r[0] && q[1] == r[1] && q[2] == r[2] && q[3] == r[3])
                    break;
            }
            WaitFrames(1);
        }
        q[4] = 0x74;
        q[5] = 0x75;
        for (;;) {
            if ((iwram_3001f64 & 3) != 3) {
                n++;
                if (n > 0x18)
                    return -1;
            } else {
                n = 0;
                if (q[0] != r[0])
                    return -1;
                if (q[1] != r[1])
                    return -1;
                if (q[2] != r[2])
                    return -1;
                if (q[3] != r[3])
                    return -1;
                if (q[4] == r[4] && q[5] == r[5])
                    break;
            }
            WaitFrames(1);
        }
        q[6] = 0x52;
        q[7] = 0x4e;
        goto tE;
    lE:
        WaitFrames(1);
    tE:
        if ((iwram_3001f64 & 3) == 3)
            goto okE;
        n++;
        if (n <= 0x18)
            goto lE;
        return -1;
    okE:
        n = 0;
        if (r[6] == 0x72 && r[7] == 0x6e)
            goto lE;
    }
    return 0;
}

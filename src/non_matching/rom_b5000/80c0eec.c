/* Func_80c0eec -- asm/rom_b5000/rom_bffb8_c_c_a.s
 *
 * BLOCKER: LOOP ROTATION / basic-block placement. 73 of 81, one line short.
 *
 * A key-repeat edit loop: four guarded adjustments to the int at base+0x828
 * (-1, +1, -0x64, +0x64 on four gKeyRepeat bits), confirm on gKeyPress & 1,
 * otherwise WaitFrames(1) and go round again. A shared tail returns 0x18f if
 * gKeyHeld & 4.
 *
 * The body is right -- all four adjustments, their masks, their constants, the
 * reload of iwram_3001e74 INSIDE the loop, and the shared tail. What differs
 * is where gcc puts the blocks:
 *
 *     rom    ... beq L0 / ldr r5,=gKeyRepeat / L1: <body> ... WaitFrames / b L1
 *     ours   ... beq L0 / ldr r5,=gKeyRepeat / b L1 / L2: WaitFrames / L1: <body>
 *
 * gcc emits an extra `b` into the loop and lays the WaitFrames continuation
 * block AHEAD of the body; the ROM falls straight into the body and puts the
 * continuation at the bottom. Every later difference is that one placement
 * shifting the streams.
 *
 * MEASURED, three loop shapes:
 *   for(;;) { body; if (press) { r = ...; break; } WaitFrames(1); }
 *                                                   80 lines, 73 differ
 *   the same with `r = ...` moved AFTER the loop     80 lines, 73 differ
 *                                                   (byte-identical to above)
 *   while ((gKeyPress & 1) == 0) { WaitFrames(1); body; }
 *                                                   87 lines, 79 differ
 *
 * The first two being byte-identical is the useful part: moving the result
 * read out of the `break` changes nothing, so the placement is not driven by
 * what the exit block contains. The third is a clean negative -- restructuring
 * the loop so the test is the loop condition costs SEVEN lines, because the
 * body then has to be duplicated or the pointer reloaded before the test.
 *
 * NOT TRIED: nothing further. Block placement is chosen after the source
 * structure is fixed, and the two shapes that preserve the ROM's semantics
 * both give the same layout. This is the same family as the loop-rotation
 * note in docs/elevation.md (subscript vs walking pointer), but the lever
 * there does not apply -- there is no induction variable to reshape here.
 */
extern int gKeyHeld;
extern int gKeyRepeat;
extern int gKeyPress;
extern int iwram_3001e74;
extern void WaitFrames(int n);

int Func_80c0eec(void)
{
    char *base;
    int r;

    if (gKeyHeld & 8) {
        for (;;) {
            base = (char *)iwram_3001e74;
            if (gKeyRepeat & 0x20)
                *(int *)(base + 0x828) -= 1;
            if (gKeyRepeat & 0x10)
                *(int *)(base + 0x828) += 1;
            if (gKeyRepeat & 0x40)
                *(int *)(base + 0x828) -= 0x64;
            if (gKeyRepeat & 0x80)
                *(int *)(base + 0x828) += 0x64;
            if (gKeyPress & 1) {
                r = *(int *)(base + 0x828);
                break;
            }
            WaitFrames(1);
        }
    }
    if (gKeyHeld & 4)
        r = 0x18f;
    return r;
}

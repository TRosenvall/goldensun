/* Func_80c9048 -- 0x080c9048 -- asm/rom_c9000/rom_c9048_a_a.s
 *
 * BLOCKER: the placement of ONE pooled address load. 9 of 54, LENGTH EXACT.
 *
 * Sets eight blend and window registers, then appends a three-word entry to
 * the DMA task queue under IME-off, then waits a frame. The register writes,
 * the IME save/disable/restore pair, the queue-full guard, the count bump and
 * all three queued words reproduce.
 *
 * Two things read off the disassembly before writing, both now routine:
 *   - the window registers are written PAIRED BY WINDOW -- WIN0H, WIN0V,
 *     WIN1H, WIN1V -- visible in the destination hops `add r3, #4 / sub r3, #2
 *     / add r3, #4`, the same ordering as Func_80cd418.
 *   - every stored constant is POOLED here (`ldr r3, .LcXXXX`), so bare
 *     literals are correct and the int-local operand-mode fix would be wrong.
 *     This is the mirror of Func_8011b00, where the ROM had `mov` and the
 *     literal had to be routed through an int.
 *
 * WHAT REMAINS is where `ldr r1, =gDMATaskCount` sits. The ROM issues it in the
 * MIDDLE of the window writes, between the REG_WININ address/value setup and
 * the store; ours issues it after the IME address load. Every later line is
 * that one instruction's offset.
 *
 * MEASURED -- source order does not decide it. Three placements of
 * `q = &gDMATaskCount;`, all WORSE than leaving it where its use is:
 *
 *   immediately before the REG_WININ write     54 lines, 14 differ
 *   immediately after the last window write    54 lines, 14 differ
 *   first statement in the function            54 lines, 14 differ
 *   (left at its point of use)                 54 lines,  9 differ  <- best
 *
 * All three moved forms are identical to each other, which is the tell that
 * gcc is scheduling the load on its own and ignoring the statement position --
 * moving it earlier only makes the value live longer and costs five more.
 *
 * The same shape as Func_8096d84: a load whose consumer is further down, where
 * the ROM issues it early and gcc sinks it toward the use. There the
 * diagnostics showed post-reload scheduling owns the decision.
 */
#include "gba/io.h"

extern void WaitFrames(int n);
extern unsigned short gDMATaskCount;

void Func_80c9048(void)
{
    unsigned short *q;
    int savedIme;
    int count;
    int *task;

    REG_BLDCNT = 0;
    REG_BLDALPHA = 0x100e;
    REG_WIN0H = 0xf0;
    REG_WIN0V = 0x1088;
    REG_WIN1H = 0xf0;
    REG_WIN1V = 0x1088;
    REG_WININ = 0x3537;
    REG_WINOUT = 0x3f21;
    q = &gDMATaskCount;
    savedIme = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    count = *q;
    if (count <= 0x1f) {
        task = (int *)(count * 12 + (int)q + 4);
        *q = count + 1;
        *task++ = 0x7741;
        *task++ = 0x80 << 19;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
    WaitFrames(1);
}

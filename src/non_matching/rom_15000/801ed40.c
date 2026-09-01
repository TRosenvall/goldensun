/* Func_801ed40 (0x0801ed40) -- NON-MATCHING.
 * Blocker class: gcc DERIVES a pooled constant the ROM loads fresh.
 * ONE differing line of 63, at the ROM's exact length.
 *
 *     rom    ldr r2, =0x12ee ... ldr r2, =0x12ec
 *     ours   ldr r2, =0x12ee ... sub r2, #0x2
 *
 * Two field offsets four bytes apart, tested one after the other. The ROM
 * pools both; gcc pools the first and subtracts 2 for the second.
 *
 * THIS IS THE MIRROR OF THE USUAL CASE and it is worth naming. Every recorded
 * derivation lever so far is "the ROM derives and gcc does not" -- write the
 * derivation, `n -= 0xff` rather than `n - 0xff` (rom_a1000/rom_a1814_a_a_a_b.c),
 * `t--` rather than `t = -1` (rom_15000/8026e80.c). Here gcc derives and the ROM
 * does not, and there is no lever in that direction: you cannot ask for a
 * second pool entry.
 *
 * MEASURED (rom 63 lines, all at exact length, all 1 differing):
 *   `p + 0x12ec` written plainly
 *   `p + (0x12f0 - 4)` so the constant is spelled from the other end
 *   each address named into its own pointer before its test
 *   -fno-gcse / -fno-cse-follow-jumps / -fno-expensive-optimizations
 *
 * WHAT IS RIGHT: the r8 holding the third argument across two calls, with no
 * lever -- the reject that hid this whole class is gone, see the docs; the two
 * out-parameter locals at sp+8 and sp+0xc passed by address; the two stack
 * arguments `a + 0xe` and `1`, which gcc builds through ONE register exactly as
 * the ROM does because the first is a variable; and the speculative `a = 1;`
 * before the first comparison.
 *
 * NEXT: nothing. One instruction, and the direction has no lever.
 */
extern unsigned char *iwram_3001e8c;
extern int _GetFlag(int);
extern int GetPortrait(int b);
extern void LoadPortrait(int id, int c, int *v, int *t, int e, int f);

void Func_801ed40(unsigned int a, int b, int c)
{
    unsigned char *p;
    int id;
    int t;
    int v;
    int k;

    p = iwram_3001e8c;
    if (_GetFlag(0x20) != 0) {
        if (b == 0)
            b = 0x12;
        if (b == 1)
            b = 0x13;
    }
    id = GetPortrait(b);
    if (id == -1)
        return;
    if (a > 1) {
        a = 1;
        if (*(unsigned short *)(p + 0x12ee) != id) {
            if (*(unsigned short *)(p + 0x12ec) != id)
                return;
            a = 0;
        }
    }
    k = a * 2 + 0x12f0;
    v = *(unsigned short *)(p + k);
    LoadPortrait(id, c, &v, &t, a + 0xe, 1);
}

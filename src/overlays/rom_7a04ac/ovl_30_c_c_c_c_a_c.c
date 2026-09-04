/* OvlFunc_913_200a974 -- 0x0200a974
 *
 * Advances the storm state (three cases over two counters), then every eighth
 * frame spawns a raindrop actor at a randomised offset from the camera origin
 * and hands it its script.
 *
 * FOUR THINGS, and three of them are register-level rather than structural.
 *
 *  - THE SWITCH VALUE IS UNSIGNED. The dispatch uses `bhi`, not `bgt`. With the
 *    counter declared `int` every comparison is signed and the whole function
 *    shifts; `unsigned int` is the fix and it is visible in one instruction.
 *
 *  - THE ZERO STORED IN CASE 3 IS THE SPAWNED ACTOR'S OWN VARIABLE. The ROM
 *    emits `mov r5, #0` BEFORE the switch and stores r5 in case 3, and r5 later
 *    holds the actor returned by __CreateActor. A separate zero local does not
 *    reproduce it -- gcc sinks it into the arm -- and a barrier on that local
 *    puts it in a scratch register instead. Initialising the ACTOR to 0 up
 *    front and storing it in case 3 is what gives the callee-saved register a
 *    live range long enough to reach the arm. One instruction, and it moved the
 *    first divergence from 3 to 24.
 *
 *  - THE SHARED TAIL ONLY SHARES THE COMPARISON. Both arms load the counter
 *    themselves (`ldr r2, =L3384 / ldr r3, [r2]`) and only the compare-and-
 *    decrement is cross-jumped. Writing the load once in the shared block
 *    leaves the function two instructions short; a per-arm load plus a shared
 *    `if (v > lim) L3384 = v - 0x4000;` is the shape.
 *
 *  - TWO CROSSED SITES, both cured without a barrier and one with. The limit is
 *    written `lim = K; v = L3384; lim <<= S;` so the counter load lands BETWEEN
 *    the constant's mov and its shift, which is the barrier-free reordering
 *    cure. The flag byte at q[9] needed the barrier: reading it into a local
 *    first is not enough, because gcc still issues `mov r3, #0xd` ahead of the
 *    `ldrb`, and only a barrier on the loaded value holds the order.
 *
 * `lim` and the counter value are pinned to r1 and r3; unpinned, gcc picks
 * r3/r2 and the two arms disagree with the ROM by four lines.
 */
extern unsigned int L338c __asm__(".L338c");
extern int L3388 __asm__(".L3388");
extern int L3384 __asm__(".L3384");
extern unsigned int iwram_3001e40;
extern unsigned char **iwram_3001e70;
extern unsigned char *__CreateActor(int id, int a, int b, int c);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern void __Actor_SetAnim(void *a, int n);
extern void __Actor_SetScript(void *a, void *s);
extern unsigned char gScript_913__0200b308[];

void OvlFunc_913_200a974(void)
{
    unsigned char *n;
    unsigned char *b;
    register int lim __asm__("r1");
    register int v __asm__("r3");
    int px;
    int py;
    unsigned char *q;
    int t;
    int u;

    n = 0;
    switch (L338c) {
    case 1:
        if (L3388 <= 0x3a97)
            L3388 += 0x32;
        lim = 0xf0;
        v = L3384;
        lim <<= 14;
        goto step;
    case 2:
        if (L3388 <= 0x752f)
            L3388 += 0x32;
        lim = 0xc0;
        v = L3384;
        lim <<= 13;
    step:
        if (v > lim)
            L3384 = v - 0x4000;
        break;
    case 3:
        if (L3384 < (int)0xff800000) {
            L338c = (unsigned int)n;
        } else {
            L3388 += 0x32;
            L3384 -= 0x4000;
        }
        break;
    }
    if ((iwram_3001e40 & 7) != 0)
        return;
    n = __CreateActor(0x11d, 0, 0, 0);
    if (n == 0)
        return;
    b = iwram_3001e70[0];
    if ((iwram_3001e40 & 0x3f) == 0)
        __PlaySound(0xf6);
    if (L338c != 0) {
        t = L3388 * __Random();
        px = *(int *)b + (((unsigned int)t >> 16) << 8) + L3384;
    } else {
        px = *(int *)b + (__Random() << 8) + 0xff800000;
    }
    py = *(int *)(b + 8) + (__Random() << 8) + 0xff800000;
    n[0x55] = 0;
    *(int *)(n + 0xc) = 0xa0 << 16;
    q = *(unsigned char **)(n + 0x50);
    *(int *)(n + 0x18) = 0xe666;
    *(int *)(n + 0x1c) = 0xe666;
    *(int *)(n + 8) = px;
    *(int *)(n + 0x10) = py;
    q[0x26] = 0;
    v = 0xfe;
    v &= n[0x23];
    n[0x23] = v;
    u = q[9];
    __asm__ volatile ("" : : "r" (u));
    v = -13;
    v &= u;
    v |= 4;
    q[9] = v;
    __Actor_SetAnim(n, 1);
    __Actor_SetScript(n, gScript_913__0200b308);
}

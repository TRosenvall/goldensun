// fakematch
/* OvlFunc_941_2009394  --  0x02009394
 *
 * WAS "THE CANONICAL SMALL SPECIMEN" OF THE DUPLICATE-CONSTANT CSE WALL, and
 * the wall is not there. Byte-exact: 136 bytes, 54 encodings and 13
 * relocations identical.
 *
 * The park closed its search after THIRTEEN FLAGS, concluding "no spelling
 * separates two uses of one literal... This is the class boundary, not a
 * function-specific problem", and its DUP-CONST column became a HARD SKIP in
 * four selection scripts gating about 158 candidates.
 *
 * THE ASSUMPTION EVERY ONE OF THOSE THIRTEEN MEASUREMENTS SHARED IS THAT THE
 * LEVER HAD TO BE A FLAG. None of them tried writing the argument into a hard
 * register. Two PIN3 blocks on the __MapActor_Emote calls, one
 * `do { } while (0)` before the message base, and one PIN2 on
 * __Func_8092c40 make it exact.
 *
 * The park's own diagnosis was right in every particular -- `0x81 << 1` is
 * passed to two __MapActor_Emote calls eleven calls apart, the ROM rebuilds it
 * both times, gcc hoists it into r6 and pays a wider push and pop, and all 39
 * differing lines follow from that one decision. It was the CURE it ruled out,
 * by looking only where it had been looking.
 *
 * A NOTE ON THAT FLAG TABLE, because it should not simply be trusted again.
 * tools/tryc.py silently discarded bare `-f` flags passed on the command line
 * until it was fixed in batch 222, and an "inert" row is exactly what a
 * dropped flag produces. Two rows of the table -- -fno-expensive-optimizations
 * and -fno-omit-frame-pointer -- DID move the numbers, so flags were reaching
 * the compiler somehow in at least those runs. The ten "inert" rows are not
 * thereby refuted, but neither are they evidence: an inert flag and a dropped
 * flag are indistinguishable without a positive control.
 *
 * It does not matter here, because the cure was never a flag.
 *
 * THE LESSON IS THE ONE ALREADY IN THE DOC, paid for at scale: a park that
 * measured N spellings has only ruled out what those N SHARED. This one
 * measured thirteen and they shared everything that mattered, and the
 * conclusion was then promoted into four tools as a hard skip over 158
 * candidates.
 *
 * Landing this means the DUP-CONST skip must be revisited -- see HANDOFF.
 */
extern void __Func_809280c(int a, int b, int c);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __Func_8092adc(int slot, int a, int b);
extern void __CutsceneWait(int n);
extern void __MapActor_DoAnim(int slot, int a);
extern void __Func_8092c40(int slot, int a);
extern int __Func_8091c7c(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

int OvlFunc_941_2009394(void)
{
    int m;

    __Func_809280c(2, 0, 0);
    { PIN3; q1 = 0x81; q2 = 0x3c; q1 <<= 1; q0 = 2; __MapActor_Emote(q0, q1, q2); }
    do { } while (0);
    m = 0x255e;
    __MessageID(m);
    __ActorMessage(2, 0);
    __Func_8092adc(0xc, 0xc0 << 6, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0xc, 4);
    __MessageID(m + 1);
    __ActorMessage(0xc, 0);
    { PIN3; q1 = 0x81; q2 = 0x3c; q1 <<= 1; q0 = 3; __MapActor_Emote(q0, q1, q2); }
    m += 2;
    __MessageID(m);
    { PIN2; q1 = 0; q0 = 3; __Func_8092c40(q0, q1); }
    return __Func_8091c7c(0, 0) == 0;
}

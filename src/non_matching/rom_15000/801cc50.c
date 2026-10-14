/* Func_801cc50 -- NON-MATCHING, 3 differing STREAM encodings of 55.
 *
 * ref 55 / ours 55 stream encodings; 54/54 16-bit instructions -- IN PHASE, so a
 * true DISTANCE.  THIS PARK HAD NO FIGURE AT ALL before batch 338 brief E: it was
 * a prose park, invisible to parkcheck, to the figure bands and to
 * reports/frontier.md.  A body now exists and this is its measurement.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/801cc50.c \
 *     asm/rom_15000/rom_1ca1c_a_a_c_c_a_a.s --func Func_801cc50
 *
 * ***THE "BYTE-IDENTICAL CANDIDATE" CLAIM BELOW IS WRONG -- THE PATH NAMES A
 * DIFFERENT FUNCTION.***  Checked by the coordinator after batch 350 when the
 * owner approved installing the two parks sitting at a verified 0:
 * scratch_elev/b338/E/p3_candidate.c is `Func_80b8824` (its line 1 says so, and
 * its reference is asm/rom_b5000/rom_b8228_c_a_c_a_c_a.s).  Measured against THIS
 * park's reference it reads SIZE ref 112 / ours 100 and INSTRUCTION COUNT ref 54
 * / ours 35 -- a different function, not a candidate for this one.
 *
 * So no byte-identical body for Func_801cc50 exists at that path, and the ladder
 * below describing "with the barrier 0 of 55" is UNVERIFIED: nothing on disk
 * reproduces it.  Treat the 3 of 55 as this park's only measured figure until
 * something re-derives the 0.
 *
 * This is exactly the failure mode tools/unrealised.py is documented to guard
 * against -- it ranks a park's smaller self-reported figures as candidates FOR
 * READING, not as results, and this row is why that wording matters.  A stale
 * POSITIVE can also be a stale POINTER.
 *
 * The superseded claim, kept for the record:
 * A BYTE-IDENTICAL CANDIDATE EXISTS AND IS NOT INSTALLED.  scratch_elev/b338/E/
 * p3_candidate.c reads 0 of 55.  The difference between it and this body is ONE
 * BARE `__asm__ volatile ("")` -- a fakematch-class device -- so the ladder is:
 *
 *     with the barrier       0 of 55
 *     without it (this body) 3 of 55
 *
 * The three register pins are NOT the question: they are the `.call_via` class,
 * structural to an indirect call through a register, and BOTH bodies carry them
 * identically.  install_batch.py's "LANDS WITH 3 PIN(S)" warning is about those,
 * and the owner's policy -- "if no pin-free landing is readily available, park it
 * at the pin-free figure" -- presumes a pin-free figure exists.  Here none does,
 * because the pins come from the call ABI rather than from matching.  So the
 * decision is about the BARRIER, exactly as Func_80c0130's was in batch 337.
 *
 * FIRST THING TO TRY, AND IT DISSOLVED THE EQUIVALENT QUESTION LAST BATCH:
 * substitute a `do { } while (0)` ANCHOR for the barrier.  On Func_80c0130 that
 * substitution matched in all four barrier/anchor combinations and the owner
 * decision went away entirely -- the shared header ended up carrying no device.
 * Note that an anchor is NOT a stronger barrier: it works through loop notes
 * (haifa-sched.c:3714, :3744-3757, reached only when schedule_barrier_found is
 * set by NOTE_INSN_LOOP_BEG/_END), and `:3735-3736` hangs those notes on the
 * FOLLOWING insn, so it fences immediately before the next insn.  That makes the
 * placement predictable rather than a sweep.  NOT TRIED HERE -- brief E finished
 * before batch 338 brief A established the mechanism.
 *
 * THE LEVER THAT GOT IT HERE, worth keeping either way: `"r2"` ALONE in the
 * clobber list.  The prose park had tested `"r2","r3"` together, found it worse,
 * and correctly reasoned that r3 survives the call -- then discarded r2 along
 * with it.  A REJECTED PAIR IS NOT A REJECTED MEMBER.  Crossed with a u16 return
 * and a split load temp.
 */
extern int Func_8000888(int a, int b);

#define CALL_VIA_R4(res, aa, bb) do {                                   \
    register int _b __asm__("r1") = (bb);                               \
    register int _a __asm__("r0") = (aa);                               \
    __asm__ volatile ("\t.align\t2, 0\n\tmov\tr12, pc\n\tbx\tr4"        \
        : "=r" (_a) : "r" (f), "0" (_a), "r" (_b)                       \
        : "memory", "lr", "r12", "r2");                                 \
    (res) = _a;                                                         \
} while (0)

unsigned short Func_801cc50(short *p, int a, int b, int c)
{
    register int (*f)(int, int) __asm__("r4");
    int r, g, bl;
    int v;
    int t;
    unsigned int off;

    off = 0;
    t = *(short *)((unsigned int)p + off);
    f = Func_8000888;
    CALL_VIA_R4(v, t << 16, a);
    r = v >> 16;
    off = 2;
    t = *(short *)((unsigned int)p + off);
    CALL_VIA_R4(v, t << 16, b);
    g = v >> 16;
    off = 4;
    t = *(short *)((unsigned int)p + off);
    CALL_VIA_R4(v, t << 16, c);
    bl = v >> 16;
    if (r < 0)
        r = 0;
    if (r > 0x1f)
        r = 0x1f;
    if (g < 0)
        g = 0;
    if (g > 0x1f)
        g = 0x1f;
    if (bl < 0)
        bl = 0;
    if (bl > 0x1f)
        bl = 0x1f;
    return r + ((bl << 10) + (g << 5));
}

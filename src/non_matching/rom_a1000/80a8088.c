/* Func_80a8088 -- NON-MATCHING, 3 differing STREAM encodings of 63.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/Na088_best.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 63 / ours 63 stream encodings; 55/55 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a8088.c \
 *     asm/rom_a1000/rom_a7380_c_a_a_a.s --func Func_80a8088
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 3 of 63, instruction counts EQUAL, byte size and pool
 *   count equal too (this park is in phase on all three length dimensions, so
 *   its figure IS a real distance).
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: 16 placements swept, but 9 WERE BIT-IDENTICAL to
 *   the base, so only 7 DISTINCT PROGRAMS were actually tested.
 *   Those 7 are inert; the other 9 measured nothing.  No variant was capped and none failed to
 *   compile.
 *
 *   SO DO NOT RE-SCREEN THESE.  Every brief so far has spent part of a round on
 *   exactly that.  The residue here is NOT a placement or flag question; it is a
 *   register choice, a tie, or a pass-level effect, and it needs a hypothesis.
 *
 *   Read as "inert against THIS body".  A recorded negative expires silently
 *   when the body changes -- six batches running, the decisive lever has turned
 *   up inside a park's own negative list.
 */
extern unsigned char *iwram_3001f2c;

extern int Func_80a10d0(int *q, int b, int c, int d, int e, int f);
extern unsigned char *_Func_801ec6c(int a, int b, int c, int d, int e, int f);
extern void Func_80a33d4(unsigned char *p, int e);
extern void Func_80a9cf8(int e);
extern void Func_80a8604(int e, int a, int c);

void Func_80a8088(int a)
{
    unsigned char *p;
    unsigned char *r;
    int *q;
    int e;
    int ok;
    unsigned char **w;
    unsigned short *h;

    p = iwram_3001f2c;
    e = *(int *)(p + 0x24);
    ok = 0;
    if (e == 0) {
        q = (int *)(p + 0x24);
        ok = Func_80a10d0(q, 0, 5, 0x1e, 0xf, 2);
        e = *q;
    }
    if (ok != 0) {
        r = _Func_801ec6c(a, 0, 0, e, 0, 0);
        w = (unsigned char **)(p + (0xbe << 1));
        *w = r;
        r[0xf] = 0xf0;
        h = (unsigned short *)(p + (0xbe << 1) + 0xa4);
        if (*h == 3)
            Func_80a33d4(p, e);
        Func_80a9cf8(e);
        Func_80a8604(e, a, 0x80 << 1);
    } else {
        Func_80a8604(e, a, 0);
    }
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* Func_80a8088 -- 0x080a8088, asm/rom_a1000/rom_a7380.s
 *
 * 65 of 65 lines, THREE differing, and they are one instruction moved:
 *
 *      rom   add r2, #0xa4 / mov r3, #0xf0 / strb r3, [r0, #0xf]
 *      ours  mov r3, #0xf0 / strb r3, [r0, #0xf] / add r2, #0xa4
 *
 * The ROM derives the second field offset (0x17c + 0xa4 = 0x220) BEFORE the
 * byte store; we derive it after.  Everything else, including the derivation
 * itself, is identical.  Candidate at scratch/Na088_best.c.
 *
 * SOLVED, and it sharpens the batch-152 pointer-local rule rather than just
 * applying it.  The store `*(void **)(p + o) = r` with a NAMED OFFSET gives the
 * register-offset form `str r0, [r6, r3]` where the ROM computes the address --
 * 33 differing.  On CutsceneStart a pointer temporary fixed exactly this.  Here
 * it does NOT: gcc folds `w = (void **)(p + o); *w = r;` straight back into the
 * register-offset store, because nothing forces the address to exist as a value.
 *
 * What worked instead was DROPPING THE SHARED OFFSET VARIABLE and writing both
 * offsets as plain constants.  gcc then computes the first address with an
 * `add`, and still DERIVES the second from it (`add r2, #0xa4`) of its own
 * accord.  33 differing -> 3, line count exact.
 *
 * So the rule is narrower than CutsceneStart suggested: a pointer local forces
 * the `add` only when something else keeps the offset alive -- there, the offset
 * was mutated between the address and the store.  With nothing to keep it, drop
 * the offset variable entirely and let the constants speak.
 *
 * SCREENED, none better than 3: the pointer temporaries with the shared offset
 * (33); the offset mutated between the address and the store (64 lines, 29);
 * the second address computed BEFORE the byte store, which is where the ROM
 * derives it (66 lines, 30 -- it hoists more than the derivation); and the
 * second address written as `(unsigned char *)w + 0xa4`, expressing the ROM's
 * derivation directly (3, byte-identical to the constants form).
 *
 * The last of those is the useful negative: gcc reaches the same code from
 * "derive from the first pointer" and "write the second constant", so that
 * choice is not the handle on the ordering.
 *)

*/
#endif

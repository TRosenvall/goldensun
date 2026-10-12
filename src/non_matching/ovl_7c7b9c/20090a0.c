/* OvlFunc_943_20090a0 -- NON-MATCHING, 2 differing STREAM encodings of 112.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/W90a0_D.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 112 / ours 112 stream encodings; 75/75 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7c7b9c/20090a0.c \
 *     asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_a_a_c.s --func OvlFunc_943_20090a0
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_808e118(void);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_943_20092f0(void);
extern void OvlFunc_943_200b9ec(int slot);

void OvlFunc_943_20090a0(void)
{
    unsigned char *p;
    int m;
    int b;
    int s1, s2, t1, u1, v1, w1, w2, e1;

    s1 = 0x33333;
    s2 = 0x19999;
    t1 = 0x30a;
    u1 = 0xc9 << 2;
    v1 = 0xaf << 2;
    w1 = 0xf6 << 16;
    w2 = 0x80 << 18;
    e1 = 0x101;
    if (__GetFlag(0x911)) {
        if (__GetFlag(0x922) == 0) {
            __CutsceneStart();
            __Func_808e118();
            OvlFunc_943_20092f0();
            __MapActor_SetSpeed(0x14, 0x6666, 0x3333);
            p = __MapActor_GetActor(0x14) + 0x5a;
            m = 0xfe;
            *p = m & *p;
            __Func_80921c4(0x14, 0xe8, 0xcc << 2);
            __CutsceneWait(1);
            p = __MapActor_GetActor(0x14) + 0x5a;
            b = 1;
            *p = *p | b;
            __CutsceneWait(0x14);
            __Func_809259c(0x14, 2);
            OvlFunc_943_200b9ec(0x14);
            __MapActor_SetSpeed(0x14, 0x13333, 0x9999);
            p = __MapActor_GetActor(0x14) + 0x5a;
            *p = m & *p;
            __Func_80921c4(0x14, 0xf4, u1);
            __CutsceneWait(1);
            p = __MapActor_GetActor(0x14) + 0x5a;
            *p = *p | b;
            __CutsceneWait(0x14);
            __MapActor_SetSpeed(0x14, s1, s2);
            __Func_80921c4(0x14, 0xf8, t1);
            __Func_80921c4(0x14, 0xf8, v1);
            __MapActor_SetPos(0x14, w1, w2);
            __Func_8092adc(0x14, 0, 0);
            __MapActor_Emote(0, e1, 0x3c);
            __CutsceneEnd();
        }
    }
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_943_20090a0 -- 0x020090a0, asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_a.s
 * Twin of OvlFunc_943_20091c8 -- 0x020091c8, SAME .s file.
 *
 * A clean twin family: the two differ in FOUR immediates (0xe8/0xca, 0xf4/0xc0,
 * and 0xf8/0xb4 twice), so one solution elevates both.
 *
 * 102 of 102 lines, TWO differing.  Candidate: scratch/W90a0_D.c.
 *
 * THE INTERLEAVE LEVER DID ALMOST ALL OF IT -- 18 differing to 2 -- and this is
 * the largest number of sites it has fixed at once: SIX.  Every call whose
 * argument setup the ROM interleaves has its non-r0 arguments named in the block
 * dominating the guarded region (before the outer `if`), and r0 left a literal:
 * two __MapActor_SetSpeed pooled pairs, three __Func_80921c4 third arguments,
 * the __MapActor_SetPos split builds, and the __MapActor_Emote pooled argument.
 *
 * Naming them next to their calls instead does NOTHING (18, unchanged); naming
 * them before the outer `if` is what works, even though the values then cross
 * two __GetFlag calls.  Both were screened, and it is the same dominance
 * precondition confirmed on OvlFunc_948_200a188 and OvlFunc_932_200a9dc.
 *
 * BLOCKER: the commutative register-role swap, at ONE of the two `*p |= 1` sites.
 *      rom   orr r6, r3 / strb r6      (mutates the constant register)
 *      ours  orr r3, r6 / strb r3      (preserves it)
 * The FIRST of the two sites matches exactly; only the second differs.
 *
 * THE INTERESTING PART, and why this is not just another instance: the two MASK
 * sites in the same function have the identical shape -- `and` with a value held
 * across calls -- and gcc gets BOTH right on its own, preserving r5 at the first
 * and destroying it at the second exactly as the ROM does.  So gcc is capable of
 * the preserve-then-mutate pattern; it simply does not choose it for the `orr`.
 * That kills the obvious theory that the class is about liveness, because the
 * liveness structure of the mask and the bit is the same.
 *
 * TRIED, all 2 except where noted: constant on the left at both OR sites and at
 * the second only; spelling the second as a mutation (`b |= *p; *p = b;`) which
 * is what the ROM literally does -- WORSE, 103 lines and 42 differing, because
 * the mutation costs an instruction; CSE, GCSE, ALIAS, STRENGTH.  SCHED2 is
 * worse (24).
 *
 * Same class as 200a5c0.c, where seven spellings also failed.  Do not spend a
 * round on it; the note above about the mask is the new information.
 *)

*/
#endif

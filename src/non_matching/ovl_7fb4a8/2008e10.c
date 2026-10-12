/* OvlFunc_971_2008e10 -- NOT MATCHING, a SIZE mismatch: ref 264 bytes against ours 268.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/N8e10_best.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7fb4a8/2008e10.c \
 *     asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_c.s --func OvlFunc_971_2008e10
 *
 * The original prose park is preserved below.
 */
extern unsigned char gState[];
extern volatile unsigned int gKeyRepeat;
extern volatile unsigned int gKeyPress;

extern void __CutsceneStart(void);
extern int __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __Func_809280c(int a, int b, int c);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern int __CreateUIBox(int a, int b, int c, int d, int e);
extern void __Func_8016478(int box);
extern void __Func_801e9a0(int a, int b, int c, int d, int e);
extern void __CloseUIBox(int box, int n);
extern int __Debug_LoadPresetParty(int n);

int OvlFunc_971_2008e10(int a)
{
    unsigned char *g;
    int box;
    int sel;
    int last;

    __CutsceneStart();
    g = gState;
    __Func_809280c(a, *(int *)(g + (0xfa << 1)), 0);
    __MessageID(0x989);
    __ActorMessage(a, 0);
    box = __CreateUIBox(0, 0, 6, 4, 2);
    sel = 0;
    last = -1;
    for (;;) {
        if (sel != last) {
            __Func_8016478(box);
            __Func_801e9a0(sel, 3, box, 0, 0);
            last = sel;
        }
        if (gKeyRepeat & 0x20)
            sel -= 1;
        if (gKeyRepeat & 0x10)
            sel += 1;
        if (sel < 0)
            sel = 0;
        if (gKeyPress & 1)
            break;
        if (gKeyPress & 2) {
            sel = -1;
            break;
        }
        __WaitFrames(1);
    }
    __CloseUIBox(box, 1);
    if (sel < 0)
        goto msg_a;
    if (__Debug_LoadPresetParty(sel))
        goto msg_b;
    goto msg_c;
msg_a:
    __MessageID(0x98a);
    goto shared;
msg_b:
    __MessageID(0x98b);
shared:
    __ActorMessage(9, 0);
    goto tail;
msg_c:
    __MessageID(0x98c);
    __ActorMessage(9, 0);
tail:
    __WaitFrames(0xa);
    return __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_971_2008e10 -- 0x02008e10,
 * asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c.s
 *
 * 114 lines against the ROM's 115, 29 differing, and the first 79 are exact --
 * the whole selection loop, the key handling and the early exit.  Candidate at
 * scratch/N8e10_best.c.
 *
 * THE CORRECTION THIS FUNCTION CARRIES, and it is aimed at batch 148's own
 * advice: "explicit gotos are how you control block order" HAS A BOUNDARY, and
 * I walked past it here.  Five goto arrangements were screened before the plain
 * `for (;;) { ... if (c) break; ... }` was tried at all, and the plain form beat
 * every one of them:
 *
 *      goto form, break block adjacent to the loop        41 differing
 *      goto form + out-of-line message blocks            40
 *      goto form, break block at the ROM's position      79  (gcc DUPLICATES it)
 *      goto form, message id in a shared local           35
 *      plain for(;;) with two breaks                     29   <- best
 *
 * The reason the goto forms lose is worth keeping: a two-instruction block
 * reached by `goto` is DUPLICATED INLINE by gcc unless it happens to sit
 * adjacent to the branch.  The ROM has that block out of line at the end and
 * branches to it, which is exactly what an ordinary `break` produces and what a
 * `goto` to a distant label does not.  Reach for gotos when the ROM's block
 * ORDER cannot be expressed with ordinary control flow -- not before trying it.
 *
 * ALSO SOLVED: `gKeyRepeat` must be volatile (the two masks are read with no
 * call between them and CSE into one load otherwise); the gState base needs a
 * named pointer or the +0x1f4 folds into the pool constant; `sel = 0` is
 * written BEFORE `last = -1` (the reverse order swaps two instructions); and
 * the epilogue `pop {r1} / bx r1` says the function returns a value, so it is
 * `int` and ends `return __CutsceneEnd();`.
 *
 * BLOCKER: which of the two message arms goes out of line.
 *      rom   blt L8 / mov r0, r5 / bl __Debug_LoadPresetParty / b L9
 *      ours  bge L8 / ldr r0, =0x98a / bl __MessageID / b L9
 * The ROM keeps the LoadPresetParty path on the FALLTHROUGH and puts the 0x98a
 * load out of line; we do the reverse, and because of that our two __MessageID
 * calls never cross-jump -- the ROM merges them one instruction further back,
 * sharing `bl __MessageID` with the id already in r0.  Writing the shared id as
 * an explicit local to force that merge makes it WORSE (35), because gcc then
 * spends an instruction moving it into r0.
 *
 * This is the same block-placement question as the `goto` rows above, one level
 * down, and I have no lever for it: `if (sel < 0) goto msg_a;` with the
 * LoadPresetParty code immediately after is already the arrangement that should
 * give the ROM's branch, and gcc lays the arms out the other way regardless.
 *)

*/
#endif

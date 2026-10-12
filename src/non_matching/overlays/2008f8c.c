/* OvlFunc_971_2008f8c -- NOT MATCHING, a SIZE mismatch: ref 196 bytes against ours 192.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/L8f8c.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/2008f8c.c \
 *     asm/overlays/rom_7fb4a8/ovl_30_c_a_a_a_a.s --func OvlFunc_971_2008f8c
 *
 * The original prose park is preserved below.
 */
extern unsigned int gState;
extern int _MSG_294e;
extern int OvlFunc_971_2008f30(int a);
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_809280c(int a, int b, int c);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);

void OvlFunc_971_2008f8c(int slot)
{
    unsigned int g;
    unsigned int off;
    int a, b, f;
    int m;
    int k1, k2;

    k1 = 0xbc << 2;
    k2 = 0xbc << 2;
    m = (int)(&_MSG_294e);
    a = OvlFunc_971_2008f30(0);
    b = OvlFunc_971_2008f30(slot);
    __CutsceneStart();
    g = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    g += off;
    __Func_809280c(slot, *(int *)g, 0);
    if (__GetFlag(0xc1 << 2) != 0) {
        __GetFlag(k1);
        f = __GetFlag(slot + k2);
        if (__GetFlag(0x305) != 0) {
            if (f != 0)
                m = 0x2967;
            else
                m = 0x296c;
        } else {
            if (f != 0)
                m = 0x2971;
            else
                m = 0x2976;
        }
    } else {
        if (a == 0)
            m = 0x2958;
        else if (b == 0)
            m = 0x2953;
    }
    __MessageID(m + slot - 1);
    __ActorMessage(slot, 0);
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_971_2008f8c -- 0x02008f8c,
 * asm/overlays/rom_7fb4a8/ovl_30_c_a_a_a_a.s
 *
 * 71 vs 75 lines, 45 differing.  Candidate at scratch/L8f8c.c.
 *
 * SOLVED, and this is a THIRD instance of the symbol-base lever: the ROM holds
 * the default message id 0x294e in r5 across three calls and every branch,
 * reaching the final `__MessageID(m + slot - 1)` with it.  Written as
 * `int m = 0x294e;` gcc sinks the load into the branches that need it and never
 * spends a callee-saved register -- 63 differing.  `extern int _MSG_294e;` with
 * `m = (int)(&_MSG_294e);` puts `ldr r5, =...` in the ROM's position: 63 -> 50.
 * (The symbol is NOT yet in message.sym; the screen ran with it unresolved.)
 *
 * Naming the two `0xbc << 2` builds separately took 50 -> 45; ours had CSE'd
 * them into one register where the ROM builds the flag id twice.
 *
 * BLOCKER: ours is FOUR instructions short and the parameter/result registers
 * are permuted -- the ROM puts the slot parameter in r6 and the second
 * OvlFunc_971_2008f30 result in r7, ours the reverse, and that propagates.
 * Being short by four suggests something structural is still missing rather
 * than a pure allocation difference, so this is not a one-lever fix.
 *
 * Note the ROM calls `__GetFlag(0xbc << 2)` and DISCARDS the result (r0 is
 * overwritten by the following `add` before the next call).  That is
 * transcribed as a bare call statement and is not the problem.
 *)

*/
#endif

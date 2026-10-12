/* Func_8020198 -- NON-MATCHING, 3 differing STREAM encodings of 71.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/N0198_best.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 71 / ours 71 stream encodings; 56/56 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/8020198.c \
 *     asm/rom_15000/rom_20198_a_a_a.s --func Func_8020198
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern unsigned char L371e0[] __asm__(".L371e0");
extern int _MSG_09;

extern void Func_8016478(int a);
extern void Func_801e41c(int a, int b, int c, int d, int e);
extern void Func_801e8b0(unsigned char *a, int b, int c, int d);
extern void UIDrawText(void *s, int b, int c, int d);
extern void Func_801e9d4(int a, int b, int c, int d, int e);
extern void Func_801e7c0(int a, int b, int c, int d);
extern void *Func_801f680(int a, void *b);
extern void Func_801ea08(int a, int b, int c, int d, int e);

void Func_8020198(int a, unsigned char *p)
{
    int buf[4];
    int n;

    if (a == 0)
        return;
    Func_8016478(a);
    Func_801e41c(a, 0, 4, 0xd, 4);
    Func_801e8b0(p + 0x10, a, 0, 0);
    n = 0;
    UIDrawText(L371e0, a, 0x48, 0);
    Func_801e9d4(p[0x1c], 2, a, 0x50, n);
    Func_801e7c0(p[0x1d] + 0x741, a, 0, 0x10);
    Func_801e7c0((int)&_MSG_09, a, 0, 0x20);
    UIDrawText(Func_801f680(*(int *)(p + 0x20), buf), a, 0x30, 0x28);
    n = 0x30;
    Func_801ea08(*(int *)(p + 0x24), 6, a, 0, n);
    Func_801e7c0(0xc88, a, 0x30, 0x30);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* Func_8020198 -- 0x08020198, asm/rom_15000/rom_20198_a_a.s
 *
 * 68 of 68 lines, TWO differing, and they are two independent loads in the
 * opposite order.  Candidate at scratch/N0198_best.c.
 *
 *      rom   ldr r3, =0x741 / ldrb r0, [r7, #0x1d]
 *      ours  ldrb r0, [r7, #0x1d] / ldr r3, =0x741
 *
 * SOLVED, two things, and both are worth the note:
 *
 *   THE POOLED 9 IS A MESSAGE SYMBOL.  `ldr r0, =0x9` for a value an eight-bit
 *   `mov` builds is the pooled-constant tell, and Func_801e7c0's first argument
 *   is a message id -- three elevated files pass one to it.  `_MSG_09 = 0x09;`
 *   added to message.sym alongside the existing _MSG_0a.._MSG_0d; it takes the
 *   count from 18 differing to 17 and emits no bytes.  It is kept even though
 *   this function is parked, so the next attempt does not re-derive it.
 *
 *   THE STACK ARGUMENT NEEDS A CALL BETWEEN ITS ASSIGNMENT AND ITS USE.  The
 *   ROM spends a THIRD callee-saved register (it pushes {r5, r6, r7}) with r6
 *   holding the fifth argument at two six-argument calls and r7 holding the
 *   parameter.  Written beside its call the value gets a scratch register, we
 *   push only {r5, r6}, and the parameter lands in r6 -- 17 differing, almost
 *   all of it that one rename cascading.  Moving `n = 0;` up so it spans the
 *   preceding UIDrawText makes gcc spend r7, and 17 drops to 2.  This is the
 *   batch-149 rule (statement position picks the register class) paying off on
 *   a function where the whole diff was downstream of it.
 *
 *   Moving the SECOND assignment up instead works equally well; moving BOTH is
 *   worse (11 differing).  One crossing is enough and two is too many.
 *
 * BLOCKER: SCHEDULER TIE between two independent loads.  sched2 is producing
 * the ROM's order everywhere else in the function -- --no-sched2 goes to 24
 * differing -- so this is not a reason to reach for a flag.
 *
 * SCREENED AND UNCHANGED AT 2: the addends written the other way round
 * (`0x741 + p->f1d`); the constant named in a local first; the parameter
 * declared as a typed struct so the byte read is a field, which is the
 * batch-145 alias-set lever and does nothing here; and the per-declaration
 * prototype sweep over every callee.
 *)

*/
#endif

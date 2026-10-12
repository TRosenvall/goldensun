/* OvlFunc_907_2008328 -- NOT MATCHING, a SIZE mismatch: ref 220 bytes against ours 224.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/N8328_D.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_79b154/2008328.c \
 *     asm/overlays/rom_79b154/ovl_30_c_a_a_c_c_c_c_c_c_a.s --func OvlFunc_907_2008328
 *
 * The original prose park is preserved below.
 */
extern char *iwram_3001ebc;
extern short L1d0c[] __asm__(".L1d0c");
extern void *L1cf0[] __asm__(".L1cf0");

extern void __PlaySound(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_8010560(void *s, int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8091e9c(int n);

void OvlFunc_907_2008328(void)
{
    char *w;
    unsigned char *p;
    unsigned int i;
    int k;
    int t;

    w = iwram_3001ebc;
    __CutsceneStart();
    for (i = 8; i <= 0x41; i++) {
        p = __MapActor_GetActor(i);
        if (p != 0)
            p[0x55] = 0;
    }
    t = *(unsigned short *)(w + (0xb6 << 1));
    t -= 3;
    k = (short)t;
    if (k == 6)
        __PlaySound(0xbc);
    else
        __PlaySound(0x9e);
    __Func_8010560(L1cf0[k], L1d0c[k * 2], L1d0c[k * 2 + 1]);
    __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
    p = __MapActor_GetActor(0);
    p[0x55] = 0;
    w = iwram_3001ebc;
    *(int *)(w + (0xe0 << 1)) = 0x100;
    if (k == 6) {
        __MapActor_SetAnim(0, 2);
        __Func_809228c(0, 0, -4);
    } else {
        __Func_8092208(0, 3, -0x10);
    }
    if (k == 4)
        __Func_8092b08(0, 3);
    else
        __Func_8092b08(0, 2);
    __CutsceneWait(0x10);
    __Func_8091e9c(k + 3);
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_907_2008328 -- 0x02008328, asm/overlays/rom_79b154/ovl_30_c_a_a_c_c_c_c_c_c.s
 *
 * 97 ROM lines against 98 of ours, 60 differing, down from 78.
 * Candidate: scratch/N8328_D.c.
 *
 * The clear-every-actor loop (8..0x41 inclusive, unsigned), the two PlaySound
 * arms, the derived store of 0x100 through iwram_3001ebc, and both trailing
 * two-way branches all reproduce.
 *
 * SOLVED ON THE WAY, and reusable: the index is computed as
 *      t = *(unsigned short *)(w + (0xb6 << 1));  t -= 3;  k = (short)t;
 * Written as one expression -- `(short)(*(unsigned short *)... - 3)` -- the
 * arithmetic stays in unsigned-short and gcc emits `ldr r4, =0xfffd / add r3, r4`
 * where the ROM has a plain `sub r3, #3`.  Routing it through an int local gives
 * the sub.  78 differing -> 60, and one line shorter.  Same shape as the
 * halfword-mask finding in batch 136: a narrow type makes gcc pool a constant
 * that an int keeps as an immediate.
 *
 * BLOCKER, two things and both known walls:
 *
 *   1. REGISTER ROLES.  The ROM holds the iwram pointer in r7 and the loop's
 *      stored zero in r6; ours has them swapped, and that decides four lines
 *      plus everything downstream that names either register.  Both assignment
 *      orders were screened (zero named before and after the pointer) and both
 *      give 79 -- worse than leaving the zero a literal at 78.
 *
 *   2. TABLE-ACCESS ORDER.  __Func_8010560 takes a word from one table and two
 *      shorts from another at the same k*4 index.  The ROM computes the two
 *      shorts FIRST and the word into r0 last; gcc does r0 first.  Dropping the
 *      callee's prototype -- the documented way to push r0 to the end -- changes
 *      nothing here (60 either way).
 *
 * So the remaining 60 are one register-role decision and one argument-evaluation
 * order, neither of which has a lever.  Worth revisiting if the register-role
 * class ever yields the way the memory-typed half of it did in batch 136.
 *)

*/
#endif

/* OvlFunc_901_200858c -- NOT MATCHING, a SIZE mismatch: ref 180 bytes against ours 188.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/L858c.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * objcmp stops at the size check, so no encoding figure exists yet; the
 * body must reach the reference's LENGTH before a distance can be read.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/200858c.c \
 *     asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_a_a_a.s --func OvlFunc_901_200858c
 *
 * The original prose park is preserved below.
 */
extern int _MSG_1cb1;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __WaitFrames(int n);

void OvlFunc_901_200858c(void)
{
    unsigned char *a;
    unsigned short *p;
    int saved;
    int m;
    int v;
    int w1;

    w1 = 0x80 << 1;
    a = __MapActor_GetActor(0xe);
    saved = *(short *)(a + 6);
    p = (unsigned short *)(a + 0x64);
    *p |= 2;
    __CutsceneStart();
    m = (int)(&_MSG_1cb1);
    __MessageID(m);
    __MapActor_SetAnim(0xe, 0);
    __Func_8092848(0xe, 0, 2);
    if (__GetFlag(0xc0 << 2) == 0) {
        __MapActor_Emote(0xe, w1, 0x3c);
        __Func_8093040(0xe, 0, 0xa);
        __Func_8093040(0xe, 0, 0xa);
        __SetFlag(0xc0 << 2);
    }
    __MessageID(m + 2);
    __Func_8093040(0xe, 0, 0xa);
    *(short *)(a + 6) = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    v = 1;
    *p = v;
    __SetFlag(0x307);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_901_200858c -- 0x0200858c,
 * asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_a_a_a.s
 *
 * 70 of 70 lines, TWO differing.  Candidate at scratch/L858c.c.  Needs
 * CSE_CFLAGS and `_MSG_1cb1 = 0x1cb1;` in message.sym (a one-line addition,
 * deliberately NOT made while the function stays parked).
 *
 * SOLVED, and three of these are worth reusing:
 *   - `unsigned short *p` for the +0x64 field: a signed `short *` gives
 *     `ldrsh rD,[rB,rI]` with a zero index register where the ROM has
 *     `ldrh rD,[rB,#0]`.
 *   - `*p |= 2;` as a COMPOUND assignment gives the ROM's pooled constant
 *     (`ldr r2, =0x2`) and destructive orr.  Written as three steps --
 *     `v = 2; v |= *p; *p = v;` -- gcc emits `mov r3,#0x2` instead and the
 *     block is 45 differing.  Neither `*p = 2 | *p;` nor an int `v` reaches it.
 *   - the final `*p = 1;` needs the OPPOSITE treatment: an int intermediate,
 *     or gcc pools it (`ldr r3, =0x1`) where the ROM has `mov r3, #0x1`.
 *
 * So the same halfword pointer wants a pooled constant at one operation and a
 * mov-built one at the next, and the spelling that gets each is different.
 * That is the sharpest instance yet of the HImode-literal rule not being one
 * rule.
 *
 * BLOCKER: the OR's register roles.
 *      rom   orr r2, r3 / strh r2, [r6, #0x0]
 *      ours  orr r3, r2 / strh r3, [r6, #0x0]
 * Two instructions, and the destination register decides both.  `*p = 2 | *p;`
 * and a named constant both leave it at 2.
 *)

*/
#endif

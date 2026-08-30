/* Cluster OvlFunc_927_20099b8..OvlFunc_927_20099b8 extracted from
 * goldensun/asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_c.s.
 *
 * 112 instructions, and it took four separate levers.  Recorded in the order
 * they were needed, because each one only became visible after the previous.
 *
 * 1. THE SIX-ARGUMENT CALL PASSES A STRUCT BY VALUE.  The ROM loads four words
 *    into r0-r3 and then block-copies the remaining two to the outgoing area
 *    with `ldmia r3!, {r0,r1} / stmia r2!, {r0,r1}`.  Written as six separate
 *    int arguments gcc uses two spare registers and two `str`s -- move-by-pieces
 *    -- which costs a THIRD callee-saved register and a `push {r7}` the ROM does
 *    not have.  `OvlFunc_927_2008608(t)` with the whole 24-byte struct produces
 *    the block move and frees the register.  A `ldmia`/`stmia` pair feeding an
 *    outgoing argument area is the tell.
 *
 * 2. THE CASE CHAIN IS WRITTEN WITH GOTOs.  The ROM tests 9, then 0xb, then 8,
 *    with all three bodies OUT OF LINE (`beq` and `b` forward to them), and the
 *    case-8 arm rejoins at the final __CutsceneEnd rather than at the flag test.
 *    An if/else-if chain puts each body inline behind a `bne` and scores 99
 *    differing; a `switch` sorts the cases and emits a balanced compare tree
 *    (`cmp #9 / bgt`), which the ROM does not have.  The explicit goto form is
 *    the only one that lays the blocks out the ROM's way: 99 -> 37.
 *
 * 3. NAME BOTH STACK ARGUMENTS, PER CALL SITE -- EXCEPT WHERE THAT COSTS A
 *    REGISTER.  __Func_8010704 takes six arguments, so two go on the stack.  At
 *    the three sites inside the flag-setting arms, naming BOTH gives them their
 *    own registers and the ROM's store order; leaving them as expressions makes
 *    gcc reuse one register and store in the wrong order.  At the FOURTH site --
 *    the case-8 arm -- naming the shifted value makes gcc compute it
 *    SPECULATIVELY above the `bne` and hold it in r6, which costs the callee-
 *    saved register back.  There, only the constant is named.  37 -> 19 -> exact
 *    once the fourth site is treated differently from the other three.
 *
 * 4. `t.f8 >> 20` IS WRITTEN OUT AT EVERY USE.  The ROM re-reads the field and
 *    re-shifts it each time, including immediately after comparing it, where it
 *    keeps the compare's own result in r2 and stores that.  A local for the
 *    shifted value is not what it does.
 *
 * A NOTE ON THE ROUND: an intermediate version of this file lost a pair of
 * braces in an edit, so `if (t.f4 == 8) s4b = 0x31; __Func_8010704(...);` left
 * the call unconditional -- and gcc then deleted the comparison entirely, which
 * read exactly like a compiler mystery until the source was looked at.  Two
 * missing lines in the diff were two missing lines in the C.
 */
struct T {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
};

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int OvlFunc_927_2008474(struct T *t);
extern void OvlFunc_927_2008608(struct T t);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092b08(int a, int b);
extern void __SetFlag(int id);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);

void OvlFunc_927_20099b8(void)
{
    struct T t;
    unsigned char *e;
    int f;
    int s1a, s1b, s2a, s2b, s3a, s3b, s4b;

    f = 0;
    __CutsceneStart();
    if (OvlFunc_927_2008474(&t)) {
        OvlFunc_927_2008608(t);
        if (t.f4 == 9)
            goto c9;
        if (t.f4 == 0xb)
            goto cb;
        goto c8;
    c9:
        s1a = t.f8 >> 20;
        s1b = 0x44;
        __Func_8010704(0x26, 0x44, 1, 4, s1a, s1b);
        if ((t.f8 >> 20) == 0x2a) {
            s2a = t.f8 >> 20;
            s2b = 0x17;
            __Func_8010704(0x1a, 0x14, 2, 4, s2a, s2b);
            __Func_8092b08(9, 1);
            f = 1;
            __SetFlag(0x312);
        }
        goto ftest;
    cb:
        if ((t.f8 >> 20) == 0x28) {
            s3a = t.f8 >> 20;
            s3b = 0x20;
            __Func_8010704(0x1a, 0x14, 2, 4, s3a, s3b);
            __Func_8092b08(0xb, 1);
            f = 1;
            __SetFlag(0x313);
        }
    ftest:
        if (f == 0) {
            __CutsceneEnd();
            return;
        }
        __MapActor_SetAnim(t.f4, 3);
        __Func_809228c(t.f4, 0x12, 6);
        __CutsceneWait(0x1e);
        __MapActor_SetAnim(t.f4, 8);
        __PlaySound(0xf0);
        e = __MapActor_GetActor(t.f4);
        e[0x23] = 2;
        goto done;
    c8:
        if (t.f4 == 8) {
            s4b = 0x31;
            __Func_8010704(0x2a, 0x31, 1, 4, t.f8 >> 20, s4b);
        }
    }
done:
    __CutsceneEnd();
}

/* OvlFunc_947_200a384  --  0x0200a384  [asm/overlays/rom_7d0e88/ovl_1528_c_c_c_c_a_c_c.s]
 *
 * NOT MATCHING. Best 44 of 100, ours 98 lines. The candidate below is that form.
 * The .s holds this function alone with no data tail, so no split is needed when
 * it is finished.
 *
 * TWO STACK-ARGUMENT SITES, and the first one is SOLVED -- the fix is recorded
 * here because it refines an existing entry.
 *
 * docs/elevation.md says a stack-argument pair needs both values live at once,
 * i.e. two named locals. True, but incomplete: WHICH REGISTERS THEY LIVE IN
 * matters as much. Written as ordinary named locals the pair is given
 * CALLEE-SAVED registers and the prologue widens from `push {r5, lr}` to
 * `push {r5, r6, r7, lr}` -- 53 of 96. Written as plain literals at the call
 * they are built and stored one at a time (`mov r4,#1 / str / mov r3,#0xa /
 * mov r4,#2`) where the ROM builds BOTH first and then stores both -- 52 of 98.
 *
 * The ROM uses r3 and r2, which are call-clobbered, and REUSES them immediately
 * afterwards for the fourth and third register arguments:
 *
 *     mov r3, #1 / mov r2, #2 / str r3, [sp] / str r2, [sp, #4] /
 *     mov r3, #0xa / mov r2, #0x3c / mov r1, #0xa / mov r0, #0x49
 *
 * Pinning the pair to r3 and r2 in a tight block gives exactly that and takes
 * the first site to exact. So: A STACK-ARGUMENT PAIR NEEDS TWO NAMED LOCALS
 * PINNED TO THE ROM'S SCRATCH REGISTERS, not merely two named locals.
 *
 * WHAT REMAINS is the second site, at __Func_8010704:
 *
 *     rom   mov r3,#0x12 / mov r2,#0xd / str r5,[r0,#0x6c] / mov r1,#0xd /
 *           str r3,[sp] / str r2,[sp,#4] / mov r3,#1 / mov r2,#1 / mov r0,#0x11
 *     ours  mov r1,#0xd / str r5,[r0,#0x6c] / str r3,[sp] / str r2,[sp,#4] / ...
 *
 * The ROM BUILDS 0xd TWICE -- once into r2 as the sixth argument and once into
 * r1 as the second -- and ours commons the two, which is where the missing two
 * instructions go. The same pin treatment that fixed the first site does not
 * fix this one, because here the duplicated value is shared between a STACK
 * argument and a REGISTER argument of the same call, and gcc commons them
 * before the pins are consulted.
 *
 * MEASURED, five forms:
 *
 *     pair as ordinary named locals, function scope     53 differ, 96 lines
 *     pair scoped into each call                        55 differ, 98 lines
 *     pair as plain literals at the call                52 differ, 98 lines
 *     pair pinned to r3/r2 at both sites                44 differ, 98 lines
 *     the stored zero written as the flag result        44 differ, identical
 *
 * The last of those is worth a line: the ROM stores r5, which holds
 * __GetFlag(0x203)'s result, into an actor field -- and the branch is taken only
 * when that result is zero, so gcc could be substituting a known zero. Writing
 * the store as `(void *)r` rather than `0` is byte-identical to writing `0`, so
 * that is NOT what is happening and the substitution is gcc's own.
 *
 * NEXT: force 0xd to be materialised twice. Nothing in this tree records a way
 * to defeat commoning between a stack and a register argument of one call; the
 * closest entry is the rematerialisation lever, which works because a
 * call-clobbered pin cannot survive a `bl`, and here there is no call between
 * the two uses. A different sixth-argument spelling that is not literally 0xd
 * -- an expression gcc cannot fold -- would be inventing code and is not the
 * answer.
 */
extern void OvlFunc_947_200a230(void);
extern void OvlFunc_947_200a2d8(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, int s);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __StartTask(void (*f)(void), int n);
extern void __StopTask(void (*f)(void));
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8092504(int a);
extern void __Func_8092950(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_947_200a384(void)
{
    register int p0 __asm__("r0");

    if (__GetFlag(0x203) == 0) {
        __SetFlag(0x202);
        __CutsceneStart();
        __Func_80933d4(0x9999, 0x1333);
        { PIN4; q0 = 0x9c; q1 = 1; q2 = 0xb8; q0 <<= 17; q1 = -q1; q2 <<= 16; q3 = 1;
          __Func_80933f8(q0, q1, q2, q3); }
        __Func_8093530();
        __CutsceneWait(0x14);
        {
            register int e0 __asm__("r3");
            register int e1 __asm__("r2");
            e0 = 1; e1 = 2;
            __CopyMapTiles(0x49, 0xa, 0x3c, 0xa, e0, e1);
        }
        __CutsceneWait(0x14);
        {
            register void (*q0)(void) __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 0xc8; q1 <<= 4; q0 = OvlFunc_947_200a230;
            __StartTask(q0, q1);
        }
        __CutsceneWait(0x28);
        p0 = 0x201;
        if (__GetFlag(p0) != 0) {
            {
                register int q1 __asm__("r1");
                *(void **)(__MapActor_GetActor(0xc) + 0x6c) = OvlFunc_947_200a2d8;
                q1 = 6;
                __MapActor_SetAnim(0xc, q1);
            }
            __Func_8092504(0xc);
            {
                register int q3 __asm__("r3");
                register int q2 __asm__("r2");
                q3 = 0x12; q2 = 0xd;
                *(void **)(__MapActor_GetActor(0xc) + 0x6c) = 0;
                __Func_8010704(0x11, 0xd, 1, 1, q3, q2);
            }
            p0 = 0x201;
            __ClearFlag(p0);
            __Func_8092950(0xc, 0);
            __MapActor_SetBehavior(0xc, 1);
        } else {
            __CutsceneWait(0x3c);
        }
        __StopTask(OvlFunc_947_200a230);
        __CutsceneWait(0x14);
        {
            register int e0 __asm__("r3");
            register int e1 __asm__("r2");
            e0 = 1; e1 = 2;
            __CopyMapTiles(0x48, 0xa, 0x3c, 0xa, e0, e1);
        }
        __CutsceneWait(0x14);
        __CutsceneEnd();
    }
}

/*
 * OvlFunc_950_20085a8 -- asm/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_c_a_c_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: gcc rematerialises a pooled message id per branch where the ROM
 * keeps it in a callee-saved register and derives. 76 lines against 77 -- ONE
 * SHORT, and the one is `push {r6}` with its pop.
 *
 *      rom   ldr r6, =0x2389 ... add r0, r6, #1 ... add r0, r6, #2
 *      ours  ldr r0, .L10+4  ... ldr r0, .L10+8 ... ldr r0, .L10+12
 *
 * PROBED, and the shape is easy to get in isolation. With
 * `b = 0x2389; f(b); f(b+1); f(b+2);` in one basic block gcc emits exactly
 * `ldr r5, .L3 / add r0, r5, #1` and pushes r5 -- and a SYMBOL base
 * (`(int)&_MSG_x`, the form message.sym users take) behaves identically, so
 * this is not a symbol question.
 *
 * What breaks it here is that the three uses are SPLIT ACROSS BRANCHES: the
 * base is used once before an if/else and the two derived ids once in each
 * arm. With only one use per arm, a fresh pool load costs gcc the same as
 * keeping the base live, and it picks the pool.
 *
 * TRIED AND REJECTED, both byte-identical at 76 lines: assigning the base at
 * the very top of the function to lengthen its live range across two more
 * calls; swapping the if/else arms so the derived-by-1 id is in the fallthrough.
 *
 * NOT TRIED: computing the id into a single variable in both arms and calling
 * __MessageID once after the join. The ROM does share one __MessageID call at
 * .L61e between the base+1 path and the 0x2219 path, so a join exists there
 * already -- but the base+2 call at .L60a is separate, so the source cannot be
 * a single merged call either. Working out which two of the three share is the
 * next step.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_80b0278(int a, int b);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);

void OvlFunc_950_20085a8(int slot)
{
    unsigned char *a;
    unsigned int t;
    int base;

    a = __MapActor_GetActor(0);
    t = (*(unsigned short *)(a + 6) + (0x80 << 6)) & 0xffffc000;
    if (t << 16 == (0xc0 << 24)) {
        __Func_80b0278(0x1a, slot);
        return;
    }
    if (__GetFlag(0x95 << 4)) {
        base = 0x2389;
        __MessageID(base);
        __Func_8092c40(slot, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0xa);
            __MessageID(base + 1);
        } else {
            __MessageID(base + 2);
        }
        __ActorMessage(slot, 0);
        return;
    }
    if (__GetFlag(0x962)) {
        __MessageID(0x2219);
        __ActorMessage(slot, 0);
        return;
    }
    __MessageID(0x1fd2);
    __ActorMessage(slot, 0);
    __MapActor_Emote(slot, 0x83 << 1, 0);
    __CutsceneWait(0x28);
    __ActorMessage(slot, 0);
}

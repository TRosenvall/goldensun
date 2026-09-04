/* OvlFunc_959_200c9a0  --  0x0200c9a0  [asm/overlays/rom_7e7574/ovl_9dc_c_c_a_c_a_a.s]
 *
 * NOT MATCHING. Best 165 lines against the ROM's 166. The .s holds this
 * function alone, so no split is needed when it is finished.
 *
 * A long conversation gated on three flags, with two actors walking a route
 * between five message lines.
 *
 * WHAT IS DIAGNOSED, precisely. gcc keeps TWO SHARED POOL CONSTANTS live
 * across the body and spills a high register to afford them:
 *
 *      1  push {r5, r6, r14}
 *      2  mov r6, r8
 *      3  push {r6}
 *     35  ldr r6, =0x1999          <- held, used at 43 and 61
 *     58  mov r8, r3               <- 0x3333 held, used at 60 and 100
 *
 * `0x1999` is the second argument of one __MapActor_SetSpeed and the third of
 * another; `0x3333` likewise. The ROM issues a fresh `ldr` at every one of the
 * six argument positions and its prologue is `push {r5, lr}` -- one register,
 * for the message-id base.
 *
 * THE PIN OVERSHOOTS HERE, and that is the finding worth keeping. Pinning
 * r0/r1/r2 at all three __MapActor_SetSpeed calls does remove the spill, and
 * takes the function to 158 lines -- EIGHT SHORT of the ROM rather than one
 * over. So the lever is not simply "too weak" at this site; applied here it
 * changes more than the hoist. This is the third measured case in two batches
 * of a pin costing rather than reaching (see
 * src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_a_c_b.c for the first).
 *
 * MEASURED:
 *
 *     plain                                        165 lines, 161 differing
 *     + r0 pinned at both `0xc5 << 2` flag uses    165 lines, 155  <- kept
 *     + r0/r1/r2 pinned at the three SetSpeed calls 158 lines, 157
 *     guard rewritten as nested ifs instead of ||   173 lines, 166
 *
 * The flag-id pin IS right and is kept: `0xc5 << 2` is tested and later set,
 * and without the pin gcc holds it in r7 and feeds both uses from there.
 *
 * THE GUARD SHAPE IS ALREADY CORRECT AS `||`. The ROM tests 0x94e and then
 * 0xc5 << 2, branching to the same arm from either, which is what a
 * short-circuit `||` produces. Rewriting it as nested `if`s with the main body
 * as the inner `then` is eight instructions WORSE, so the `||` is not an
 * accident of spelling.
 *
 * ONE INSTRUCTION OF THE GAP IS A BRANCH-DISTANCE ARTEFACT and should not be
 * chased directly. The ROM spells both early tests as `beq Lx / b Ly`, the
 * two-instruction form gcc emits when the conditional branch cannot reach its
 * target; ours are single `bne`. That form follows from the size of the code
 * in between, so it will appear on its own once the body is the right length,
 * and cannot be forced from the source.
 *
 * THAT NEXT STEP IS ALREADY ANSWERED, AND NEGATIVELY. Pinning ONLY the register
 * that holds the shared value at each site -- r1 alone at the first call, r1
 * and r2 at the second, r2 alone at the third -- is BYTE-IDENTICAL to pinning
 * the whole argument triple: 158 lines, the same eight short. So the overshoot
 * is not caused by pinning registers that did not need it. Forcing the reload
 * is itself what removes the instructions, and the ROM is longer than the
 * reloaded form by eight, which means something ELSE in the ROM accounts for
 * those eight and the hoist was never the whole story.
 *
 * NEXT: the question is how to make gcc reload a POOL constant at each use
 * without the pin's side effects. The batch-195 case that worked
 * (src/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a_b.c, also a pooled 0x3333
 * shared by two calls) pinned only r2, at two sites, and matched. Here three
 * calls and two distinct constants interleave across the argument positions,
 * which is the difference to attack first: try pinning ONLY the register that
 * actually holds the shared value at each site, rather than the whole argument
 * triple.
 */

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_80925cc(int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);

void OvlFunc_959_200c9a0(void)
{
    int msg;
    register int p0 __asm__("r0");

    if (__GetFlag(0x941) == 0) {
        __MessageID(0x244d);
        __ActorMessage(0x18, 0);
    } else if (__GetFlag(0x94e) != 0 || (p0 = 0xc5 << 2, __GetFlag(p0) != 0)) {
        __MessageID(0x2567);
        __ActorMessage(0x18, 0);
    } else {
        msg = 0x2561;
        __MessageID(msg);
        __ActorMessage(0x18, 0);
        __Func_80925cc(0x18, 1);
        __CutsceneWait(0x1e);
        __MapActor_SetSpeed(0x18, 0x1999, 0xccc);
        __Func_809228c(0x18, -4, 0);
        __MapActor_WaitMovement(0x18);
        __MapActor_SetAnim(0x18, 3);
        __CutsceneWait(0x3c);
        __MapActor_SetSpeed(0x18, 0x3333, 0x1999);
        __Func_809228c(0x18, -6, 0);
        __Func_809280c(0x18, 0, 0);
        __MapActor_WaitMovement(0x18);
        __MessageID(msg + 1);
        __ActorMessage(0x18, 0);
        __Func_80925cc(0x18, 1);
        __Func_809280c(0x19, 0x18, 0);
        __MessageID(msg + 2);
        __ActorMessage(0x18, 0);
        __CutsceneWait(0x46);
        __MapActor_SetAnim(0x19, 3);
        __CutsceneWait(0x3c);
        __MapActor_SetSpeed(0x19, 0x6666, 0x3333);
        __Func_809218c(0x19, 0xdc << 2, 0x70);
        __MapActor_WaitMovement(0x19);
        __Func_8092adc(0x19, 0xd0 << 8, 0);
        __MessageID(msg + 3);
        __ActorMessage(0x18, 0);
        __MapActor_SetAnim(0x18, 3);
        __CutsceneWait(0x46);
        __Func_809228c(0x18, 8, 0);
        __MapActor_WaitMovement(0x18);
        msg += 4;
        __MapActor_SetAnim(0x18, 5);
        __MessageID(msg);
        __ActorMessage(0x18, 0);
        __Func_809218c(0, 0xe0 << 2, 0x78);
        __MapActor_WaitMovement(0);
        __Func_8092848(0, 0x19, 0);
        __CutsceneWait(0x3c);
        __MapActor_SetAnim(0x19, 3);
        __CutsceneWait(0x1e);
        p0 = 0xc5 << 2;
        __SetFlag(p0);
    }
}

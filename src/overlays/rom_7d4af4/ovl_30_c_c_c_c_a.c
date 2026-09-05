// fakematch
/* OvlFunc_949_2008980  --  0x02008980
 *   [asm/overlays/rom_7d4af4/ovl_30_c_c_c_c_a.s, the whole file]
 *
 * 306 instructions of straight-line cutscene script gated on __Func_8091c7c:
 * two near-identical arms differing in one anim id, one counter bump, two
 * speed pairs and the closing walk.  Message base 0x1fb6, sets flag 0x8c0.
 *
 * BUILT AT THE TREE DEFAULT -O2.  No Makefile pattern rule matches
 * rom_7d4af4/ovl_30_c_c_c_c%; the only rom_7d4af4 line in the Makefile is the
 * unrelated ovl_30_c_c_a_c_c_c_c_c_c_b rule, so `asm/%.o: src/%.c` applies and
 * a scratch-path screen sees the same flags as the real object.
 *
 * THE PROLOGUE PICKS THE CURE: `push {r5, lr}`.  One callee-saved register,
 * and it is spent twice on values that are not constants -- first the message
 * id, then &iwram_3001ebc.  So the ROM keeps NO script constant anywhere and
 * every repeated one is rebuilt at every use.  Written as plain C the function
 * is 309 lines against 308 with `push {r5, r6, lr}`, r5/r6 holding 0x80<<9 and
 * 0x80<<8 across the two __MapActor_SetSpeed calls: 293 of 309 differing.  This
 * is a PIN function.
 *
 * FIFTEEN PINS, MINIMAL BY MEASUREMENT.  Thirty-six argument groups were
 * pinned, then stripped one at a time under objcmp: 21 were individually inert
 * and -- unusually for this class -- all 21 came out together.  Greedy passes
 * from both ends of the candidate list converge on the SAME fifteen, and a
 * second round found every one of the fifteen load-bearing in the reduced
 * shape.  What survives is __Func_80933f8; the 0x1d __MapActor_SetSpeed; the
 * three __MapActor_SetPos fills; __Func_80921c4(0, 0x40, 0x84<<1); both
 * __MapActor_Surprise; the first arm-A and both arm-B __Func_8092adc 0x80<<7
 * fills; and all four ldr-pair __MapActor_SetSpeed sites.  Every survivor is
 * the FIRST use of its value inside its own basic block.
 *
 * THE TWO ARMS NEED DIFFERENT PIN COUNTS AT THE SAME PAIR OF CALLS.  The two
 * adjacent __Func_8092adc(*, 0x80<<7, 0) calls are byte-identical in both arms,
 * yet arm A needs only the first pinned and arm B needs both.  Do not
 * regularise this by eye: pinning the arm-A site is inert, not wrong, and the
 * whole 36-pin set matches too -- "N pins is a size, not a set".
 *
 * THE MESSAGE ID IS A LIVE VARIABLE BUT NEEDS NO PIN, AND THAT IS THE
 * DIFFERENCE FROM src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_..._b.c.  The ROM
 * has `ldr r5, =0x1fb6 / mov r0, r5` and, 90 instructions later, `add r5, #3`
 * -- an in-place add, where the neighbour has the three-operand `add r0, r5,
 * #6`.  Two independent literals cannot produce either (cse.c relates two
 * CONSTs only through get_related_value, which needs a SYMBOL_REF): spelled as
 * 0x1fb6 and 0x1fb9 the function is 306 lines and 275 differing.  But because
 * m is REDEFINED rather than merely re-read, gcse's cprop has no constant to
 * substitute at the second site, so a plain `int m` survives -O2 here and the
 * neighbour's `register int m __asm__("r5")` is inert.  Prefer the plain int.
 *
 * 0x1fb6 IS A LITERAL, NOT A SYMBOL.  Spelled `(int)&_MSG_1fb6` the function is
 * 308 lines and ONE instruction differing by tryc -- a better score -- but
 * objcmp shows the candidate carrying an R_ARM_ABS32 for it that the reference
 * does not have.  The reference object's ONLY R_ARM_ABS32 in this function is
 * iwram_3001ebc; 0x1fb6, 0x1cccc, 0xe666, 0x19999 and 0xcccc are bare literals.
 *
 * THE do/while(0) IS LOAD-BEARING, 7 ENCODINGS.  Without it sched2 hoists
 * `ldr r5, =0x1fb6` seven slots up, above the second __MapActor_SetSpeed, and
 * takes the second SetSpeed's mov/lsl order with it.  `while (0) ;` is
 * byte-identical; `if (0) ;` is INERT -- so it is the loop note jump.c leaves
 * behind that ends the scheduling region, not a label.  Ending the region has
 * the documented side effect too: in the fully-pinned shape it flipped which
 * mov/lsl order the second SetSpeed fill emitted, so that site's pin had to be
 * re-measured afterwards -- and in the minimised shape it needs no pin at all.
 *
 * THE TWO iwram_3001ebc SITES HAVE OPPOSITE BIRTH ORDER, AND ONLY THE FIRST
 * CARES.  The ROM builds the offset before loading the pointer at the store
 * (`mov r2, #0xfa / ldr r3, [r5] / lsl r2, #1 / add r3, r2`) and after it at
 * the bump (`ldr r2, [r5] / mov r3, #0xec / lsl r3, #1 / add r2, r3`).  Naming
 * the pointer into a `char *p` first at the STORE site gives r2/r3 swapped
 * across all six instructions; dereferencing the extern inline in the same
 * expression puts the offset pseudo first and is exact.  (In the 15-pin shape
 * the named form happens to be inert too, but the inline form is what was
 * measured in every shape, so both sites are written the same way.)
 *
 * r5 SERVES TWO PSEUDOS AND THAT IS ORDINARY ALLOCATION, not one variable: the
 * message id dies at __Func_8017658 and &iwram_3001ebc is CSE'd out of the two
 * dereferences straddling the branch.  The pointer itself is reloaded on the
 * far side because the calls kill it.
 *
 * LANDING NEEDS NO SPLIT AND NO MAKEFILE WORK.  The .s holds this function
 * alone and overlays/rom_7d4af4/overlay.ld:43 already names the single .o.
 */
extern int iwram_3001ebc;

extern void __Func_8092c40(int slot, int a);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __ActorMessage(int actor, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_WaitMovement(int slot);
extern int __Func_8017658(int id, int a, int b, int c);
extern int __Func_8019da8(int a, int b, int c, int d);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809218c(int slot, int x, int y);
extern void __Func_80921c4(int slot, int x, int y);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092950(int slot, int a);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);

void OvlFunc_949_2008980(void)
{
    int m;
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");
    register int p3 __asm__("r3");

    __CutsceneStart();
    p0 = 1; p1 = 1; p2 = 1; p3 = 0; p0 = -p0; p1 = -p1; p2 = -p2;
    __Func_80933f8(p0, p1, p2, p3);
    p1 = 0x80; p2 = 0x80; p0 = 0x1d; p1 <<= 9; p2 <<= 8;
    __MapActor_SetSpeed(p0, p1, p2);
    __MapActor_SetSpeed(0x1e, 0x80 << 9, 0x80 << 8);
    do { } while (0);
    m = 0x1fb6;
    __MessageID(m);
    p1 = 0x90; p2 = 0xd0; p0 = 0x1d; p1 <<= 15; p2 <<= 16;
    __MapActor_SetPos(p0, p1, p2);
    p1 = 0xe0; p2 = 0xd0; p0 = 0x1e; p1 <<= 14; p2 <<= 16;
    __MapActor_SetPos(p0, p1, p2);
    __Func_8092950(0x20, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x20), 0);
    p1 = 0xbe; p2 = 0xa0; p0 = 0x20; p1 <<= 15; p2 <<= 14;
    __MapActor_SetPos(p0, p1, p2);
    __Func_809218c(0x1d, 0x48, 0xf8);
    __Func_809218c(0x1e, 0x38, 0xf8);
    p2 = 0x84; p0 = 0; p1 = 0x40; p2 <<= 1;
    __Func_80921c4(p0, p1, p2);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __MapActor_WaitMovement(0x1d);
    __MapActor_SetAnim(0x1d, 1);
    __MapActor_SetAnim(0x1e, 1);
    __MapActor_SetAnim(0, 1);
    __Func_809280c(0x1d, 0, 0);
    __Func_809280c(0x1e, 0, 0);
    __CutsceneWait(0x14);
    p1 = 0x81; p0 = 0x1d; p1 <<= 1;
    __MapActor_Surprise(p0, p1);
    p1 = 0x81; p0 = 0x1e; p1 <<= 1;
    __MapActor_Surprise(p0, p1);
    __Func_809259c(0x1d, 2);
    __Func_80925cc(0x1e, 2);
    __CutsceneWait(0x14);
    __Func_8092c40(0x1d, 0);
    __CutsceneWait(0x19);
    m += 3;
    __Func_8019da8(0x34, 0, 0xc, 7);
    __Func_8017658(m, 0xb, 0xc, 2);
    *(int *)((char *)iwram_3001ebc + (0xfa << 1)) = 0x20;
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        __Func_80925cc(0x1e, 2);
        __CutsceneWait(0x1e);
        __Func_8092adc(0x1e, 0, 0);
        __CutsceneWait(0x1e);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0x1d, 3);
        __CutsceneWait(0x14);
        __Func_8092adc(0x1d, 0, 0);
        __CutsceneWait(0x1e);
        __ActorMessage(0x1d, 0);
        __CutsceneWait(0x14);
        p1 = 0x80; p0 = 0x1d; p1 <<= 7; p2 = 0;
        __Func_8092adc(p0, p1, p2);
        __Func_8092adc(0x1e, 0x80 << 7, 0);
        __CutsceneWait(0x1e);
        __MapActor_SetAnim(0x1d, 3);
        __MapActor_DoAnim(0x1e, 3);
        __CutsceneWait(0x14);
        p0 = 0x1d; p1 = 0x1cccc; p2 = 0xe666;
        __MapActor_SetSpeed(p0, p1, p2);
        p0 = 0x1e; p1 = 0x1cccc; p2 = 0xe666;
        __MapActor_SetSpeed(p0, p1, p2);
        __Func_809218c(0x1d, 0xe8, 0xf8);
        __CutsceneWait(2);
        __Func_809218c(0x1e, 0xe8, 0xf8);
        __MapActor_WaitMovement(0x1d);
        __Func_809218c(0x1d, 0xf8, 0xf8);
        __Func_80921c4(0x1e, 0xf8, 0xf8);
    } else {
        __CutsceneWait(0x14);
        __Func_80925cc(0x1e, 2);
        __CutsceneWait(0x1e);
        __Func_8092adc(0x1e, 0, 0);
        __CutsceneWait(0x1e);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0x1d, 4);
        __CutsceneWait(0x14);
        __Func_8092adc(0x1d, 0, 0);
        __CutsceneWait(0x1e);
        *(unsigned short *)((char *)iwram_3001ebc + (0xec << 1)) += 1;
        __ActorMessage(0x1d, 0);
        __CutsceneWait(0x14);
        p1 = 0x80; p0 = 0x1d; p1 <<= 7; p2 = 0;
        __Func_8092adc(p0, p1, p2);
        p1 = 0x80; p2 = 0; p1 <<= 7; p0 = 0x1e;
        __Func_8092adc(p0, p1, p2);
        __CutsceneWait(0x1e);
        __MapActor_SetAnim(0x1d, 3);
        __MapActor_DoAnim(0x1e, 3);
        __CutsceneWait(0x14);
        p0 = 0x1d; p1 = 0x19999; p2 = 0xcccc;
        __MapActor_SetSpeed(p0, p1, p2);
        p0 = 0x1e; p1 = 0x19999; p2 = 0xcccc;
        __MapActor_SetSpeed(p0, p1, p2);
        __Func_809218c(0x1d, 0x48, 0xb8);
        __Func_80921c4(0x1e, 0x38, 0xb8);
    }
    __MapActor_SetPos(0x1d, 0, 0);
    __MapActor_SetPos(0x1e, 0, 0);
    __MapActor_SetPos(0x20, 0, 0);
    __SetFlag(0x8c << 4);
    __CutsceneEnd();
}

/*
 * OvlFunc_927_2009818 -- asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: argument emission interleave, same as OvlFunc_944_2008468.
 * 36 lines against 36, THREE differing:
 *      rom   mov r0,#0x11 / lsl r1,#0x11 / lsl r2,#0x11
 *      ours  lsl r1,#0x11 / lsl r2,#0x11 / mov r0,#0x11
 *
 * TRIED AND REJECTED: naming the slot (3 differing, unchanged); naming both
 * shifted arguments (3); naming only the second (4, worse).
  *
 * whodoesthis.py RESULT, and it changes this park's status from "scheduler,
 * probably unreachable" to "reachable, but not from this function's shape".
 *
 * The interleave IS emitted by ordinary C: 27 matching functions produce
 * `mov rA,#i / mov rB,#i / mov rC,#i / lsl rA / lsl rB / bl`, and SIXTEEN of
 * those push only lr, exactly like this function. So a callee-saved frame is
 * not what buys it.
 *
 * Reading them, the spelling is a NAMED LOCAL for each shifted value, assigned
 * near the TOP of the function and used later -- e.g.
 * src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_a_b.c has `a = 0x80 << 9;
 * b = 0x80 << 8;` at the top and `__MapActor_SetSpeed(0x16, a, b)` twelve lines
 * down, with a call in between, and gcc REMATERIALISES both at the use.
 *
 * APPLIED HERE IT DOES NOT REMATERIALISE. Measured: the shifted value named at
 * the top gives `push {r5}` and 25-26 lines against 23; naming two or three
 * locals instead of one gives 26; naming it immediately before the call is
 * byte-identical to the literal. The matching functions carry six or more named
 * locals competing for registers, which is apparently what tips gcc from
 * allocating to rematerialising. This function has almost no other live values,
 * so a single named local always wins a register.
 *
 * That is a real condition rather than a spelling, and it is why this stays
 * parked. Re-attack it if a way is found to make gcc rematerialise without
 * inventing locals the source did not have.
*/
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_927_2008ea8(int a, int b);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008e18(int a);
extern void __Func_8092950(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __SetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_927_2009818(void)
{
    __CutsceneStart();
    OvlFunc_927_2008ea8(0xe, 1);
    OvlFunc_927_2008d90(0xe, 0xd4 << 1, 0xf0 << 1, 0x79999);
    __CutsceneWait(2);
    OvlFunc_927_2008e18(0xe);
    __Func_8092950(0xe, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
    __CutsceneWait(0x1e);
    __SetFlag(0x305);
    __MapActor_SetPos(0x11, 0xd4 << 17, 0xf0 << 17);
    __CutsceneEnd();
}

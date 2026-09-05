// fakematch
/* OvlFunc_926_200a7ec  --  0x0200a7ec
 * [asm/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_a_a_c.s, first of three]
 *
 * 284 instructions of cutscene with a re-ask loop. Byte-exact: 740 bytes, 285
 * encodings and 80 relocations identical.
 *
 * THE TOP-TEST LOOP NEEDS A `goto`, AND THIS QUALIFIES THE RECORDED RULE.
 * The ROM is `L: call / cmp #1 / bne END ; body ; b L` -- test at the TOP, an
 * unconditional branch back at the bottom. Every natural-loop spelling is
 * rewritten by stmt.c's expand_end_loop into the rotated `b test / body /
 * test: / beq body` shape, even though the test contains a CALL that cannot be
 * duplicated:
 *
 *     while (f() == 1) { ... }                115 differing
 *     for (;;) { if (f() != 1) break; ... }   115 differing (identical output)
 *     do { ... } while (f() == 1);            111 differing
 *     label: if (f() == 1) { ...; goto label; }   EXACT
 *
 * Only the goto survives, because it is not a natural loop and expand_end_loop
 * never sees it.
 *
 * THE DOC'S do/while(1) CURE IS NOT UNIVERSAL. It was exact on
 * OvlFunc_943_200bc88, whose ROM has this same top-test shape, and it is 111
 * differing here. So the top-test shape admits TWO cures and which one applies
 * is function-specific -- measure both before concluding. The sibling
 * OvlFunc_910_20085dc landed alongside this one covers the third case: a ROM
 * loop with the test at the BOTTOM is expand_end_loop's own output, and there a
 * plain `while` is right and do/while(1) costs 22.
 *
 * Cross-jumping was left to gcc as the doc advises: the guard's false edge and
 * the loop's exit edge both fall to the same block and merge unaided.
 *
 * `push {r5, lr}` where r5 holds &iwram_3001ebc -- a compiler-generated CSE
 * temp, not a named local. Written with the bare global, the r5/r3 split falls
 * out unaided, including the ROM's derivation of 0x100 from the 0x1c0 offset
 * already in a register.
 *
 * _MSG_17e0 IS A SYMBOL AND THE TELL IS A LENGTH DIFFERENCE. 0x17e0 is
 * 0xbf << 5, so gcc synthesises it as mov+lsl -- two instructions where the ROM
 * has one pooled load. The three other message ids in this function are NOT
 * shiftable, so they pool as plain literals and need nothing. message.sym
 * already carries the entry.
 *
 * Seven register pins over six sites, minimised to a fixpoint in three passes;
 * the two __Func_8092c40 calls INSIDE the guard are ascending in the ROM and
 * correctly unpinned, while the three reached from a different predecessor are
 * descending. Four fills the ROM emits out of order come out right unaided and
 * were left alone.
 */
extern unsigned char *iwram_3001ebc;
extern int _MSG_17e0;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_DoAnim(int slot, int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __SetFlag(int f);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_926_200a7ec(void)
{
    __CutsceneStart();
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        q1 = 0x80; q0 = 0; q1 <<= 8;
        __MapActor_SetSpeed(q0, q1, 0x80 << 7);
    }
    __Func_809218c(0, 0xa8, 0xfc << 1);
    *(int *)(iwram_3001ebc + 0x1c0) = 0x100;
    __MapTransitionIn();
    __WaitMapTransition();
    __MapActor_WaitMovement(0);
    __CutsceneWait(0x14);
    __Func_809259c(8, 2);
    __MapActor_Surprise(8, 0x81 << 1);
    __CutsceneWait(0x3c);
    *(unsigned char *)(__MapActor_GetActor(8) + 0x5b) = 0;
    __PlaySound(0x98);
    *(int *)(__MapActor_GetActor(8) + 0x28) = 0x80 << 12;
    __MapActor_SetAnim(8, 1);
    __MessageID(0x17be);
    __Func_8093040(8, 0, 0x14);
    __Func_8093040(8, 0, 0x14);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    {
        register int q0 __asm__("r0");
        q0 = 0;
        __MapActor_Emote(q0, 0x101, 0x3c);
    }
    {
        register int q1 __asm__("r1");
        q1 = 0;
        __Func_8092c40(8, q1);
    }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0xa);
        __MapActor_DoAnim(8, 3);
        __CutsceneWait(0x14);
        __Func_8093040(8, 0, 0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 2;
    } else {
        __CutsceneWait(0xa);
        __Func_80925cc(8, 2);
        __CutsceneWait(0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __Func_8093040(8, 0, 0x14);
        __MapActor_DoAnim(8, 3);
        __CutsceneWait(0x14);
        __Func_8093040(8, 0, 0x14);
    }
    __Func_8093040(8, 0, 0x14);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    {
        register int q0 __asm__("r0");
        q0 = 0;
        __MapActor_Emote(q0, 0x101, 0x3c);
    }
    {
        register int q1 __asm__("r1");
        q1 = 0;
        __Func_8092c40(8, q1);
    }
    if (__Func_8091c7c(0, 0) == 1) {
        __CutsceneWait(0xa);
        __MapActor_Emote(8, 0x81 << 1, 0x3c);
        __MessageID(0x17c8);
        __Func_8092c40(8, 0);
    loop:
        if (__Func_8091c7c(0, 0) == 1) {
            __CutsceneWait(0xa);
            __MapActor_Emote(8, 0x81 << 1, 0x3c);
            __MessageID((int)&_MSG_17e0);
            __Func_8092c40(8, 0);
            goto loop;
        }
    }
    __MessageID(0x17c9);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    {
        register int q1 __asm__("r1");
        q1 = 0;
        __Func_8092c40(8, q1);
    }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 3);
        __CutsceneWait(0x14);
        __Func_8093040(8, 0, 0x14);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        __CutsceneWait(0xa);
        __Func_80925cc(8, 2);
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __Func_8093040(8, 0, 0x14);
    }
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __MapActor_SetAnim(8, 5);
    __SetFlag(0x893);
    __CutsceneEnd();
}

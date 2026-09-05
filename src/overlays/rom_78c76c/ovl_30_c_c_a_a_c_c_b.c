// fakematch
/* OvlFunc_891_2008614  --  0x02008614
 * [asm/overlays/rom_78c76c/ovl_30_c_c_a_a_c_c.s, FIRST of three functions;
 *  the other two are OvlFunc_891_2008c8c and OvlFunc_891_2008eb0]
 *
 * 610 instructions. Byte-exact: 1656 bytes, 636 encodings and 169 relocations
 * identical under tools/objcmp.py, against both the scratch reference and
 * asm/overlays/rom_78c76c/ovl_30_c_c_a_a_c_c.s itself.
 *
 * `push {lr}` -- the ROM keeps NOTHING across the body, so only pins can work
 * and no named local for a script constant can be right. Plain C is 618 lines
 * against 615 with 589 differing: the three extra lines are gcc spilling
 * r8-r11 through r5-r7 at entry to hold CSE'd script constants, and the whole
 * residue is that hoisting plus argument ordering.
 *
 * THE UNIFORM ASCENDING FILL AT ALL 111 MULTI-ARGUMENT SITES LEAVES EXACTLY
 * TWO DIFFERING LINES, and they are the recorded `__Func_8092c40` tell:
 * `__Func_8092c40(8, 0)` wants the DESCENDING fill (`q1 = 0; q0 = 8;`). That is
 * now the fifth function with this callee as the lone descending site. Nothing
 * else in the function needs a hand-ordered fill.
 *
 * THIRTY-THREE OF THE 111 PINS SURVIVE. Greedy removal re-tested under objcmp
 * after every drop, then a two-direction fixpoint pass over the survivors; each
 * of the 33 fails individually when dropped from the final set.
 *
 * The splits repeat the sibling's evidence and extend it. Three back-to-back
 * `__MapActor_Emote(slot, 0x101, 0)` calls split first-two-load-bearing /
 * third-inert, exactly as in OvlFunc_891_2008150. But BOTH triples of
 * `__Func_8092adc(slot, 0x80 << 7, 0)` -- six sites, same value, same shape --
 * are load-bearing at every one, while a SEVENTH site with the same 0x80 << 7
 * on slot 8 is inert. Position does not predict this; what each pin buys does.
 *
 * THE CLOSING STORE BLOCK INVERTS THE SIBLING'S CURE. Same two words at the
 * same two offsets (0x204 at 0xe0 << 1, 0x10 at 0xe4 << 1), and here PLAIN
 * STORES are exact. The sibling's `register int *d __asm__("r3")` costs 14
 * differing here and the offset-clobber form costs 12, because the ROM's
 * register roles are the other way round in this function: r3 carries the
 * offset and then the value, r2 carries the address. Read the block, do not
 * copy the neighbour's fix.
 *
 * `p` is the only named local and the only one possible: the
 * `__MapActor_GetActor(0)` result is live across a `cmp`/branch and two
 * `ldrsh`, so the "result dies immediately" rule does not apply.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __MessageID(int id);
extern void __ClearFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_891_200a3a4(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_891_2008614(void)
{
    unsigned char *p;

    __CutsceneStart();
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x20;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    __MapActor_SetPos(8, 0x90 << 18, 0x94 << 17);
    __CutsceneWait(1);
    __MessageID(0x1004);
    OvlFunc_891_200a3a4(8, 6);
    { PIN2; q0 = 0xcccc; q1 = 0x1999;
      __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0x23e << 16; q1 = -1; q2 = 0xb4 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 8; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x90 << 2; q2 = 0xd8;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x14);
    __MapActor_Jump(5, 2, 0);
    __CutsceneWait(0x1e);
    OvlFunc_891_200a3a4(5, 6);
    __Func_80925cc(8, 2);
    __CutsceneWait(6);
    __Func_8092adc(8, 0x90 << 8, 0);
    __CutsceneWait(0xa);
    __Func_80933d4(0x59999, 0xb333);
    { PIN4; q0 = 0x11f << 16; q1 = -1; q2 = 0xb0 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x3c);
    { PIN4; q0 = 0x23e << 16; q1 = -1; q2 = 0xb4 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0xa);
    { PIN3; q0 = 8; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    __MapActor_Jump(8, 6, 0);
    { PIN3; q0 = 8; q1 = 0xc0 << 10; q2 = 0x80 << 10;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x90 << 2; q2 = 0xb8;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x28);
    __MapActor_DoAnim(8, 3);
    OvlFunc_891_200a3a4(8, 6);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(5, 3);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x28);
    __Func_80925cc(8, 3);
    OvlFunc_891_200a3a4(8, 6);
    __MapActor_Jump(0, 2, 0);
    __MapActor_Jump(1, 2, 0);
    __MapActor_Jump(5, 2, 0);
    __CutsceneWait(0x1e);
    __Func_80925cc(1, 2);
    OvlFunc_891_200a3a4(1, 6);
    __Func_80925cc(8, 1);
    __MapActor_DoAnim(8, 4);
    OvlFunc_891_200a3a4(8, 6);
    __Func_8092848(0, 5, 0);
    __CutsceneWait(0x28);
    __Func_809280c(8, 0, 0);
    __Func_809280c(8, 5, 0);
    __CutsceneWait(0x28);
    __MapActor_Jump(8, 6, 0);
    { PIN3; q0 = 8; q1 = 0x90 << 2; q2 = 0xd8;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 8; q1 = 0x13333; q2 = 0x9999;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_809218c(8, 0xd8 << 1, 0xc8);
    __CutsceneWait(0x14);
    __Func_80933d4(0x26666, 0x4ccc);
    { PIN4; q0 = 0x90 << 17; q1 = -1; q2 = 0xab << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x50);
    __MapActor_SetAnim(8, 1);
    { PIN4; q0 = 0x23e << 16; q1 = -1; q2 = 0xb4 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __CutsceneWait(0x14);
    __Func_8092adc(8, 0, 0);
    __CutsceneWait(0x1e);
    { PIN3; q0 = 8; q1 = 0x90 << 2; q2 = 0xd8;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __Func_80933d4(0xcccc, 0x1999);
    __Func_80933f8(0x23e << 16, -1, 0xab << 16, 1);
    __MapActor_Jump(8, 6, 0);
    { PIN3; q0 = 8; q1 = 0xc0 << 10; q2 = 0x80 << 10;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(8, 0x90 << 2, 0xb8);
    __CutsceneWait(0x50);
    OvlFunc_891_200a3a4(8, 6);
    __MapActor_DoAnim(8, 4);
    OvlFunc_891_200a3a4(8, 0x14);
    __MapActor_Emote(5, 0x81 << 1, 0);
    __CutsceneWait(0x28);
    __Func_80925cc(5, 2);
    OvlFunc_891_200a3a4(5, 6);
    __MapActor_DoAnim(8, 3);
    __Func_8092adc(8, 0x80 << 8, 0);
    __CutsceneWait(0x28);
    __Func_8092adc(8, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    OvlFunc_891_200a3a4(8, 6);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(5, 0x101, 0);
    __CutsceneWait(0x3c);
    __MapActor_DoAnim(8, 4);
    OvlFunc_891_200a3a4(8, 6);
    __Func_8092848(1, 0, 0);
    __CutsceneWait(0x28);
    __Func_8092848(5, 0, 0);
    __CutsceneWait(0x28);
    { PIN2; q1 = 0; q0 = 8;
      __Func_8092c40(q0, q1); }
    { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    if (__Func_8091c7c(0, 0) == 0)
        __MessageID(0x1010);
    else
        __MessageID(0x1011);
    OvlFunc_891_200a3a4(8, 6);
    __MessageID(0x1012);
    __Func_80925cc(8, 2);
    OvlFunc_891_200a3a4(8, 6);
    __MapActor_Emote(1, 0x81 << 1, 0);
    __CutsceneWait(0x3c);
    OvlFunc_891_200a3a4(1, 6);
    __MapActor_DoAnim(8, 3);
    OvlFunc_891_200a3a4(8, 6);
    __Func_8092adc(8, 0x80 << 7, 0);
    __CutsceneWait(0x14);
    __MapActor_Jump(8, 6, 0);
    { PIN3; q0 = 8; q1 = 0x90 << 2; q2 = 0xd8;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x28);
    __Func_80933d4(0xcccc, 0x1999);
    { PIN4; q0 = 0x23e << 16; q1 = -1; q2 = 0xbf << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 8; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(8, 0x90 << 2, 0xe8);
    __CutsceneWait(0x28);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x28);
    OvlFunc_891_200a3a4(8, 6);
    __Func_8092adc(8, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    OvlFunc_891_200a3a4(8, 6);
    __MapActor_DoAnim(8, 3);
    { PIN4; q0 = 0x90 << 18; q1 = -1; q2 = 0xd7 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3; q0 = 8; q1 = 0x23e; q2 = 0x143;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(8, 0, 0);
    __Func_80933d4(0x39999, 0x7333);
    __Func_80933f8(0x90 << 18, -1, 0x88 << 16, 1);
    __Func_8093530();
    __CutsceneWait(0x14);
    __Func_8092adc(5, 0, 0);
    __CutsceneWait(0xa);
    OvlFunc_891_200a3a4(5, 6);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(5, 3);
    { PIN3; q0 = 1; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(5, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(5, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(5);
    __MapActor_SetPos(5, 0, 0);
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __ClearFlag(0x12f);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x204;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    __CutsceneEnd();
}

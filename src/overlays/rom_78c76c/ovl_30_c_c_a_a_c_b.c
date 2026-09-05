// fakematch
/* OvlFunc_891_2008150  --  0x02008150
 * [asm/overlays/rom_78c76c/ovl_30_c_c_a_a_c.s, second of five functions]
 *
 * 449 instructions. Byte-exact: 1220 bytes, 465 encodings and 128 relocations
 * identical.
 *
 * `push {r5, r6, r7, lr}` AND STILL A PIN FUNCTION -- read the content. r7 is
 * &iwram_3001ebc, r6 is the offset 0xe0<<1 and later the stored word, r5 is a
 * second offset. A pointer and two structure offsets, no script constant
 * anywhere. Plain C is 459 lines against 449 with r8-r11 shuffled into r5-r7
 * at entry, 447 differing.
 *
 * THE LAST TWELVE LINES CAME FROM A SCRATCH-REGISTER CHOICE, and this is the
 * useful part. With a plain `int *d` the tail is 449 lines and 12 differing:
 * REG_ALLOC_ORDER is {3, 2, 1, 0, ...}, so gcc takes r3 for the offset and r2
 * for the address, and the ROM has them the other way round.
 * `register int *d __asm__("r3")` binds the ADDRESS to r3 and closes it.
 *
 * That pin also fixed the two __MapActor_TravelTo index registers FORTY
 * INSTRUCTIONS EARLIER -- `mov r2, #0xa` where we had r3 -- as a side effect of
 * the allocation it forces. Worth remembering when a residue looks like two
 * unrelated clusters: one scratch-register pin can settle both, and chasing the
 * earlier cluster on its own was measured inert at six different spellings.
 *
 * TWENTY-SIX PINS, AND THE SPLITS ARE THE EVIDENCE FOR "WHAT A PIN IS FOR".
 * This function has NO BRANCH before the guarded fetches, so block position
 * explains nothing. Three back-to-back __Func_8092adc(slot, 0x80<<7, 0) calls
 * ALL THREE need pins -- each buys its own argument ordering -- while a fourth
 * site with the same value on the same straight line is inert. Three
 * back-to-back __MapActor_Emote(slot, 0x101, 0) sites split the OTHER way: the
 * first two are load-bearing and the third is inert, because the ROM emits it
 * in the order gcc reaches unaided.
 *
 * The uniform ascending fill covered all 71 multi-argument sites with no
 * hand-ordered fill anywhere; the whole residue was the closing store block.
 *
 * Naming the destination pointer is what restores the ROM's `sub r3, #0x3c`
 * derivation of the second offset from the first; bare stores rebuild it as
 * mov+lsl. The doc's offset-clobber form (`int off` reused through `*d = off`)
 * is much worse here at 29 differing, because gcse hoists the offset into a
 * callee-saved register across all three store sites.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
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
extern void __Func_800fe9c(void);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_891_200a3a4(int a, int b);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_891_2008150(void)
{
    unsigned char *p;

    __CutsceneStart();
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x100;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x20;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    { PIN3; q0 = 8; q1 = 0x90 << 18; q2 = 0xe8 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    __CutsceneWait(1);
    __MessageID(0x101a);
    OvlFunc_891_200a3a4(8, 6);
    { PIN3; q0 = 8; q1 = 0x90 << 18; q2 = 0x8c << 17;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_80933d4(0xcccc, 0x1999);
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
    OvlFunc_891_200a3a4(8, 6);
    { PIN3; q0 = 8; q1 = 0x80 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x10;
    __MapTransitionOut();
    __WaitMapTransition();
    { PIN4; q0 = 0x11f << 16; q1 = -1; q2 = 0xb0 << 16; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    __MapActor_SetPos(8, 0xd4 << 17, 0xc8 << 16);
    __WaitFrames(1);
    OvlFunc_891_200a3a4(8, 0x14);
    __MapTransitionOut();
    __WaitMapTransition();
    { PIN3; q0 = 8; q1 = 0x90 << 18; q2 = 0xb8 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN4; q0 = 0x23e << 16; q1 = -1; q2 = 0xb4 << 16; q3 = 0;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    __Func_80933d4(0x13333, 0x2666);
    { PIN4; q0 = 0x23e << 16; q1 = -1; q2 = 0x9d << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    __CutsceneWait(0x14);
    __Func_80925cc(1, 2);
    OvlFunc_891_200a3a4(1, 0x14);
    __MapActor_DoAnim(5, 3);
    OvlFunc_891_200a3a4(5, 6);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(8, 4);
    OvlFunc_891_200a3a4(8, 0x50);
    __MapActor_Emote(8, 0x81 << 1, 0);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 8; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x1e);
    OvlFunc_891_200a3a4(8, 0x14);
    { PIN3; q0 = 0; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x101; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    __MapActor_Emote(5, 0x101, 0);
    __CutsceneWait(0x3c);
    __Func_8092adc(8, 0xc0 << 8, 0);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(8, 4);
    OvlFunc_891_200a3a4(8, 6);
    __Func_8092848(0, 1, 0);
    __CutsceneWait(0x28);
    __Func_8092848(0, 5, 0);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(8, 3);
    OvlFunc_891_200a3a4(8, 6);
    { PIN3; q0 = 0; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x80 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x28);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(5, 3);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 3);
    OvlFunc_891_200a3a4(8, 6);
    __Func_80933d4(0x19999, 0x3333);
    { PIN4; q0 = 0x90 << 18; q1 = -1; q2 = 0xd7 << 16; q3 = 1;
      __Func_80933f8(q0, q1, q2, q3); }
    __Func_8092adc(8, 0x80 << 7, 0);
    __CutsceneWait(0xa);
    __MapActor_Jump(8, 6, 0);
    { PIN3; q0 = 8; q1 = 0x90 << 2; q2 = 0xd9;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 8; q1 = 0x90 << 2; q2 = 0x141;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetPos(8, 0, 0);
    __Func_80933d4(0x39999, 0x7333);
    __Func_80933f8(0x90 << 18, -1, 0x88 << 16, 1);
    __Func_8093530();
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 5; q1 = 0x80 << 9; q2 = 0x80 << 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(1, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(1, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_SetAnim(5, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(5, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(5);
    __MapActor_SetPos(5, 0, 0);
    __ClearFlag(0x12f);
    {
        register int *d __asm__("r3");

        d = (int *)(iwram_3001ebc + (0xe0 << 1));
        *d = 0x204;
        d = (int *)(iwram_3001ebc + (0xe4 << 1));
        *d = 0x10;
    }
    __CutsceneEnd();
}

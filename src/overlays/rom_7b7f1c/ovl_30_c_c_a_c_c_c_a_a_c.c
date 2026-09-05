// fakematch
/* OvlFunc_930_20081ec  --  0x020081ec
 * [asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_a_c.s, the only function]
 *
 * 631 instructions. Byte-exact: 1632 bytes, 636 encodings and 176 relocations
 * identical.
 *
 * FORTY-SIX PINS AND NOT ONE IS REMOVABLE -- the first function in the corpus
 * where the minimisation sweep found NOTHING to drop. Every one of the 46 was
 * tried individually under objcmp and every one fails. Worth recording as the
 * far end of the range: the usual result is a third to a half inert, and
 * "N pins is a size, not a set" has been about which subset survives, so a
 * function where the answer is "all of them" is a real data point rather than
 * a failure to look.
 *
 * The prologue picked that: `push {r5, lr}` with r5 holding &iwram_3001ebc, a
 * base pointer. So pins only, and the final file declares ZERO named locals.
 * Plain C is 628 lines against 631 with SIX registers spent hoisting constants
 * -- `push {r5,r6,r7,lr}` plus r8, r9 and r10 shuffled in -- and 605 differing.
 *
 * The uniform ascending fill took it to 631 lines and 7 differing in one step,
 * and reproduced the ROM's interleave without splitting the shift into its own
 * statement. The residue was three sites of pure argument ordering, closed by
 * transcribing those three.
 *
 * WRITE THE SHARED STATEMENT TWICE. The halfword increment appears in BOTH arms
 * of the guard -- at the END of the if arm and the START of the else arm -- and
 * is written literally twice, with no local carried across the join.
 *
 * THE `.L` EXTERNS WERE CHECKED FOR CAPTURE, NOT ASSUMED SAFE. Two file-local
 * data labels are reached as `extern unsigned char L1788[] __asm__(".L1788");`
 * and its neighbour. gcc's own labels in the generated .s are .L3-.L8 and
 * .Lfe1, so there is no collision, and `objdump -r` confirms both survive as
 * real R_ARM_ABS32 relocations rather than resolving locally. That is the
 * label-capture hazard in the doc, verified rather than hoped.
 *
 * The derived constant needs no help: the ROM builds 0x1c0 and then derives
 * 0x201 from it with `add r2, #0x41`, and writing the store plainly reproduces
 * both.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char L1788[] __asm__(".L1788");
extern unsigned char L179e[] __asm__(".L179e");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *p, int a, int b);
extern int  __Func_8091c7c(int a, int b);
extern void __Func_809202c(void);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093054(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_930_20081ec(void)
{
    __CutsceneStart();
    { PIN3; q0 = 8; q1 = 0x88 << 16; q2 = 0xa8 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0xa0 << 7; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x90 << 16; q2 = 0xc8 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xa0 << 16; q2 = 0xc0 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0x80 << 16; q2 = 0xc8 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xe0 << 15; q2 = 0xc0 << 16;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    *(unsigned int *)(iwram_3001ebc + 0x1c0) = 0x201;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x3c);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __MessageID(0x19e9);
    __Func_8093040(8, 0, 0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 4);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    __MapActor_DoAnim(3, 4);
    __CutsceneWait(0x14);
    __Func_8093040(3, 0, 0x14);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    __Func_8093040(2, 0, 0x14);
    __Func_80925cc(8, 2);
    { PIN3; q0 = 8; q1 = 0xa0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(8, 0, 0x14);
    { PIN3; q0 = 2; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x78);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0xc0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 1;
      __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0)
    {
        __CutsceneWait(0x14);
        __Func_8092adc(3, 0, 0x14);
        { PIN3; q0 = 3; q1 = 0x101; q2 = 0x3c;
          __MapActor_Emote(q0, q1, q2); }
        __Func_8093040(3, 0, 0x14);
        { PIN3; q0 = 0; q1 = 0xe0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 2; q1 = 0xe0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __Func_809259c(1, 2);
        __MapActor_Surprise(1, 0x81 << 1);
        __CutsceneWait(0x3c);
        { PIN3; q0 = 1; q1 = 0x80 << 8; q2 = 0x14;
          __Func_8092adc(q0, q1, q2); }
        __Func_8093040(1, 0, 0x14);
        *(unsigned short *)(iwram_3001ebc + 0x1d8) += 2;
    }
    else
    {
        *(unsigned short *)(iwram_3001ebc + 0x1d8) += 2;
        __CutsceneWait(0x14);
        __Func_8092adc(3, 0, 0x14);
        __MapActor_DoAnim(3, 3);
        __CutsceneWait(0x14);
        __Func_8093040(3, 0, 0x14);
        { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0x3c;
          __MapActor_Emote(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0xe0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 2; q1 = 0xe0 << 8; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        __Func_8093040(1, 0, 0x14);
    }
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xe0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q0 = 8; q1 = 0xc0 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(8, 0, 0x14);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    __Func_8093040(8, 0, 0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(2, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 8; q1 = 0xc0 << 8; q2 = 0x1e;
      __Func_8092adc(q0, q1, q2); }
    __PlaySound(0xbc);
    __Func_8010560(L1788, 0x43, 6);
    { PIN3; q0 = 8; q1 = 0xcccc; q2 = 0x6666;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(8, 0x88, 0x88);
    __MapActor_SetPos(8, 0, 0);
    __PlaySound(0xbc);
    __Func_8010560(L179e, 0x43, 6);
    __CutsceneWait(0x3c);
    __Func_809202c();
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0xc0 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(1, 0, 0x14);
    { PIN3; q0 = 0; q1 = 0x80 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 3; q1 = 0x80 << 6; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(3, 0, 0x14);
    __Func_80925cc(1, 1);
    { PIN3; q0 = 0; q1 = 0xe0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093054(1, 0);
    __CutsceneWait(0x14);
    __Func_80925cc(2, 2);
    __CutsceneWait(0x14);
    { PIN3; q0 = 0; q1 = 0x80 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __Func_8093040(2, 0, 0x14);
    __Func_809259c(0, 1);
    __Func_809259c(1, 1);
    __Func_80925cc(3, 1);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(2, 4);
    __CutsceneWait(0x14);
    __Func_8093040(2, 0, 0x14);
    __Func_80925cc(3, 2);
    __CutsceneWait(0x14);
    __Func_8093040(3, 0, 0x14);
    { PIN3; q0 = 2; q1 = 0xb0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x14);
    __Func_8093040(2, 0, 0x14);
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(1, 0, 0x14);
    { PIN3; q0 = 2; q1 = 0xe0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(2, 4);
    __CutsceneWait(0x14);
    __Func_8093040(2, 0, 0x14);
    { PIN3; q0 = 0; q1 = 0x81 << 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x81 << 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    __Func_8093040(3, 0, 0x14);
    { PIN3; q0 = 2; q1 = 0xa0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x14);
    __Func_8093040(2, 0, 0x14);
    __Func_809259c(0, 1);
    __Func_809259c(1, 1);
    __Func_80925cc(3, 1);
    __CutsceneWait(0x14);
    { PIN3; q0 = 2; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_80921c4(2, 0x80, 0xb8);
    { PIN3; q0 = 2; q1 = 0x80 << 7; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xc0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xa0 << 8; q2 = 0;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xe0 << 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x14);
    __Func_8093040(2, 0, 0x14);
    __MapActor_SetAnim(0, 3);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(3, 3);
    __CutsceneWait(0x14);
    { PIN3; q0 = 1; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0x80 << 8; q2 = 0x80 << 7;
      __MapActor_SetSpeed(q0, q1, q2); }
    __Func_809218c(1, 0x90, 0xc8);
    __Func_809218c(2, 0x90, 0xc8);
    __Func_809218c(3, 0x90, 0xc8);
    __MapActor_WaitMovement(1);
    __MapActor_SetPos(1, 0, 0);
    __MapActor_WaitMovement(2);
    __MapActor_SetPos(2, 0, 0);
    __MapActor_WaitMovement(3);
    __MapActor_SetPos(3, 0, 0);
    __CutsceneEnd();
}

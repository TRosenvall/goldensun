// fakematch
/* OvlFunc_896_2009450  --  0x02009450
 * [asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c.s, second of six functions]
 *
 * 313 instructions of cutscene. Byte-exact: 828 bytes, 317 encodings and 94
 * relocations identical.
 *
 * `push {lr}` AND NOTHING ELSE -- the ROM keeps ZERO callee-saved registers, so
 * every constant is rebuilt at every use and no named local can be right. None
 * ships. Plain C reads the class off the length immediately: 325 lines against
 * 313, with the r8-r11 spill that is the signature of a constant used twice
 * across calls, and 313 of 313 differing.
 *
 * THE UNIFORM FILL DID THE BULK AND THE RESIDUE WAS ALL ONE SHAPE. Pinning the
 * twenty-one repeated-constant sites with the uniform ascending fill reached
 * 313 lines and 13 differing -- the length exact -- and sched2 reproduced every
 * transposed emitted order from that one spelling. All thirteen residual lines
 * sat at the six sites left unpinned, and every one was the same fault: `mov r0`
 * needing to sit BETWEEN the `mov r1` and its `lsl`. Transcribing just those
 * six per-instruction, with the shift in the ROM's position, closed it.
 *
 * That is the recorded procedure behaving exactly as written -- uniform first,
 * then measure the residue and transcribe only the survivors -- and it is worth
 * noting the residue was homogeneous. When thirteen differing lines all share
 * one shape, they are one lever, not thirteen problems.
 *
 * ONE PIN OF TWENTY-SEVEN IS INERT, AND IT IS THE FIRST-USE RULE. The second
 * __Func_80933f8's `-1` is already written straight into r1 at the first such
 * call, leaving no pseudo for CSE to hand the later site. Dropping the FIRST
 * one instead costs 79 differing. Every other repeated value's later sites are
 * load-bearing because they buy ARGUMENT ORDERING, which no earlier pin can
 * supply -- the distinction recorded as "what a pin is for, not where the
 * blocks are".
 *
 * `void` ON THE INTRA-OVERLAY CALLEE IS LOAD-BEARING. Declaring
 * OvlFunc_896_200c248 as returning `int`, as a sibling file does, costs 16
 * differing: whether the call writes r0 truncates the dependent list of the
 * `mov r0` feeding it and flips an argument-setup scheduling tie. The sibling's
 * declaration is not automatically right for this file.
 *
 * Nothing else was needed -- no scheduling barrier, no named locals, no symbol
 * spellings. 0x109b is not shifted-byte-representable, so gcc pools it unaided
 * and no _MSG_ entry is warranted.
 */
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int id);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_800fe9c(void);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093054(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void OvlFunc_896_200c248(int a, int b);


#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_896_2009450(void)
{
    { PIN3; q1 = 0xc0; q0 = 5; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xa0 << 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x80 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xa0 << 7; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xd0 << 8; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0xb, 2);
    OvlFunc_896_200c248(0xb, 0x14);
    __Func_80925cc(0xc, 2);
    { PIN2; q0 = 0xc; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    OvlFunc_896_200c248(0xc, 0xa);
    __Func_809280c(0xa, 0xc, 0);
    __Func_809280c(5, 0xc, 0);
    __Func_809280c(9, 0xc, 0);
    __CutsceneWait(0x28);
    __Func_80925cc(0xa, 1);
    { PIN3; q0 = 0xa; q1 = 0x80 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __MapActor_SetAnim(0xa, 3);
    OvlFunc_896_200c248(0xa, 0xa);
    __Func_8092adc(0xb, 0, 0xa);
    __MapActor_DoAnim(0xb, 3);
    __CutsceneWait(0xa);
    __Func_8092adc(5, 0, 0);
    { PIN3; q0 = 9; q1 = 0x80 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __Func_809259c(5, 2);
    __Func_80925cc(9, 2);
    __CutsceneWait(0xa);
    { PIN3; q0 = 5; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xb0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xa, 3);
    OvlFunc_896_200c248(0xa, 0xa);
    { PIN3; q1 = 0x80; q2 = 0x14; q0 = 5; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q0 = 0xc; q1 = 0x81 << 1; __MapActor_Surprise(q0, q1); }
    __Func_80925cc(0xc, 3);
    __CutsceneWait(0x28);
    __MapActor_SetPos(1, 0xad << 17, 0xdc << 17);
    __WaitFrames(1);
    __ActorMessage(1, 0);
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 1; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapTransitionOut();
    __WaitMapTransition();
    { PIN3; q1 = 0x8c; q2 = 0xeb; q0 = 1; q1 <<= 17; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN4; q0 = 0x1050000; q1 = -1; q2 = 0xe9 << 17; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
    __Func_800fe9c();
    __WaitFrames(1);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    { PIN3; q1 = 0xc0; q0 = 0; q1 <<= 6; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0x81 << 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    __Func_80925cc(1, 3);
    __Func_8093054(1, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(1, 4);
    __CutsceneWait(0xa);
    __Func_8093054(1, 0);
    __CutsceneWait(0xa);
    __MessageID(0x109b);
    __ActorMessage(0xb, 0);
    { PIN3; q0 = 0xb; q1 = 0xd0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_809259c(0, 2);
    __Func_80925cc(0, 2);
    __CutsceneWait(0xa);
    { PIN3; q0 = 0; q1 = 0xe0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xe0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_80933f8(0x1dd0000, -1, 0xa7 << 17, 0);
    __Func_800fe9c();
    __WaitFrames(1);
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    __Func_80925cc(9, 2);
    __CutsceneWait(0xa);
    { PIN3; q0 = 9; q1 = 0x80 << 8; q2 = 0x1e; __Func_8092adc(q0, q1, q2); }
    OvlFunc_896_200c248(9, 0x14);
    __Func_8092adc(5, 0, 0x28);
    __MapActor_DoAnim(5, 4);
    __CutsceneWait(0x14);
    OvlFunc_896_200c248(5, 0xa);
    __Func_80925cc(0xc, 2);
    __CutsceneWait(0x50);
    OvlFunc_896_200c248(0xc, 0x14);
    __Func_80925cc(5, 2);
    { PIN3; q0 = 5; q1 = 0xc0 << 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0xb0 << 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    __MapActor_DoAnim(0xc, 3);
    OvlFunc_896_200c248(0xc, 0x14);
    __Func_809259c(5, 2);
    __Func_80925cc(9, 2);
    { PIN3; q0 = 0xc; q1 = 0xb0 << 8; q2 = 0x28; __Func_8092adc(q0, q1, q2); }
}

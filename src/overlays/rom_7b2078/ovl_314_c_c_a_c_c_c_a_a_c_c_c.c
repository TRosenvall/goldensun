/* OvlFunc_926_2009494  --  0x02009494
 *
 * 853 instructions of straight-line cutscene script: the ONLY function in
 * asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_c_c_c.s, so the file lands
 * WHOLE -- no split, no linker-script edit.
 *
 * `hi=0 hiv=0` WAS THE WHOLE STORY. Written plainly this function is 152 of 858
 * and its prologue is
 *
 *     push {r5, r6, r7, lr} / mov r7, r11 / mov r6, r10 / mov r5, r9 /
 *     push {r5, r6, r7} / mov r7, r8 / push {r7}
 *
 * against the ROM's `push {r5, lr}`. Twelve instructions of prologue/epilogue
 * the ROM does not have, because the script reuses about twenty shifted
 * constants -- 0x80 << 1, 0xcccc, 0x9a << 1, 0x98 << 17, 0xd8 << 16, 0x103,
 * 0x84 << 1, 0xd0 << 8, 0x81 << 1, 0x80 << 8 ... -- and cse_main commons every
 * one of them into a long-lived pseudo. With five high registers in play gcc
 * reaches r8/r9/r10/r11 and pays for them twice.
 *
 * ONE EVICTION PIN PER CALL SITE, TRANSCRIBED FROM THE ROM, FIXES ALL OF IT.
 * Every argument that reaches a hard call-clobbered register is invalidated by
 * the call, so it cannot be commoned; and writing the fills in the ROM's own
 * order settles the scheduling at the same time. 233 call sites, 120 of them
 * three-argument. The pins are not decoration here: removing them brings the
 * high registers straight back.
 *
 * The four read-modify-write sites are the exception and must NOT be pinned:
 * `__MapActor_GetActor(0x10)->f5a &= 0xfe` and the three `0x13` field stores
 * are single statements whose own call supplies r0.
 *
 * THE TWO `b .LNNNN` + `.pool_aligned` SITES ARE POOL JUMPS, NOT CONTROL FLOW.
 * draft_script.py flags .L18ac as a join and says the register state after it is
 * unreliable; it is not a join at all, `mov r0, #0x14 / b .L18ac / .L18ac: bl
 * __CutsceneWait` is one `__CutsceneWait(0x14)` with gcc's literal pool dumped
 * between the two halves. All three pools come out identical in content AND in
 * order, which is what makes the PC-relative offsets the screen cannot see
 * safe: 0x89a 0x183b 0xcccc 0x6666 .L477a 0x103 .L4790 / 0x101 0x103 0xcccc
 * 0x6666 iwram_3001ebc 0x105 0x898 / 0x899 gScript_926__0200c638.
 *
 * The real control flow is the two `__Func_8091c7c` tests near the end, which
 * share an exit: both arms fall into one `__Func_8093040(s, 0, 0x14)` whose
 * slot is 0x10 on one path and 0x12 on the other, so the slot is a variable.
 * `iwram_3001ebc` is referenced on two of those paths and its pool address is
 * what r5 holds -- the ROM's one pushed register.
 */
struct Sub {
    unsigned char pad00[0x1e];
    short f1e;
};

struct Actor {
    unsigned char pad00[0xc];
    int f0c;
    unsigned char pad10[8];
    int f18;
    unsigned char pad1c[0x3c - 0x1c];
    int f3c;
    unsigned char pad40[0x50 - 0x40];
    struct Sub *f50;
    unsigned char pad54[6];
    unsigned char f5a;
};

extern char *iwram_3001ebc;
extern unsigned char L477a[] __asm__(".L477a");
extern unsigned char L4790[] __asm__(".L4790");
extern int gScript_926__0200c638[];

#define PIN1 register int q0 __asm__("r0")
#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

extern void __CutsceneWait(int n);
extern void __DeleteFieldActor(int slot);
extern void __MessageID(int id);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __SetCameraTarget(int slot, int a);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, void *p);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8010560(void *p, int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809218c(int slot, int a, int b);
extern void __Func_80921c4(int slot, int a, int b);
extern void __Func_809259c(int slot, int a);
extern void __Func_80925cc(int slot, int a);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int slot, int a, int b);
extern void __Func_8092c40(int slot, int a);
extern void __Func_8093040(int slot, int a, int b);
extern void __Func_8093054(int slot, int a);

void OvlFunc_926_2009494(void)
{
    int s;

    { PIN1; q0 = 0x89a; __SetFlag(q0); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN3; q0 = 0xd; q1 = 0; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xf; q1 = 0; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q1 = 0; q2 = 0; q0 = 0x10; __Func_809280c(q0, q1, q2); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q1 = 0x80; q0 = 0xd; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0xf; q1 <<= 1; q2 = 0; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q1 <<= 1; q2 = 0; q0 = 0x10; __MapActor_Emote(q0, q1, q2); }
    { PIN1; q0 = 0x3c; __CutsceneWait(q0); }
    { PIN1; q0 = 0x183b; __MessageID(q0); }
    { PIN3; q0 = 0xd; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q2 = 0; q0 = 0; q1 = 0xd; __Func_809280c(q0, q1, q2); }
    { PIN2; q1 = 1; q0 = 0xf; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0xf; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q2 = 0; q0 = 0; q1 = 0xf; __Func_809280c(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0x10; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q2 = 0; q0 = 0; q1 = 0x10; __Func_809280c(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0x10; __Func_8093054(q0, q1); }
    { PIN1; q0 = 0x32; __CutsceneWait(q0); }
    { PIN2; q0 = 0x10; q1 = 1; __SetCameraTarget(q0, q1); }
    { PIN3; q0 = 0x10; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0xb0; q2 = 0xf8; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x9a; q0 = 0x10; q1 <<= 1; q2 = 0xf8; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q1 <<= 8; q2 = 0x14; q0 = 0x10; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x9e; __PlaySound(q0); }
    { PIN3; q2 = 0xd; q0 = (int)L477a; q1 = 0x4e; __Func_8010560((void *)q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0x10; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q1 = 0xc0; q2 = 0xc0; q1 <<= 9; q2 <<= 8; q0 = 0x10; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_GetActor(0x10)->f5a &= 0xfe;
    { PIN3; q1 = 0x9a; q2 = 0x88; q1 <<= 1; q2 <<= 1; q0 = 0x10; __Func_80921c4(q0, q1, q2); }
    { PIN1; q0 = 1; __CutsceneWait(q0); }
    __MapActor_GetActor(0x10)->f5a |= 1;
    { PIN3; q1 = 0; q0 = 0x10; q2 = 0x32; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0x98; q2 = 0xd8; q0 = 0x11; q1 <<= 17; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0x98; q0 = 0x11; q1 <<= 1; q2 = 0xf8; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xf; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0x11; q0 = 0; __Func_809280c(q0, q1, q2); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN2; q0 = 9; q1 = 2; __Func_809259c(q0, q1); }
    { PIN2; q0 = 0xa; q1 = 2; __Func_809259c(q0, q1); }
    { PIN2; q0 = 0xb; q1 = 2; __Func_809259c(q0, q1); }
    { PIN2; q0 = 0xc; q1 = 2; __Func_809259c(q0, q1); }
    { PIN2; q0 = 0xd; q1 = 2; __Func_809259c(q0, q1); }
    { PIN2; q0 = 0xe; q1 = 2; __Func_809259c(q0, q1); }
    { PIN2; q0 = 0xf; q1 = 2; __Func_809259c(q0, q1); }
    { PIN2; q0 = 0x10; q1 = 2; __Func_80925cc(q0, q1); }
    { PIN3; q0 = 0x11; q1 = 0x103; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x98; q2 = 0xd8; q0 = 0x12; q1 <<= 17; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0x98; q0 = 0x12; q1 <<= 1; q2 = 0xf8; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0x8c; q2 = 0x84; q1 <<= 1; q2 <<= 1; q0 = 0x11; __Func_809218c(q0, q1, q2); }
    { PIN1; q0 = 0x12; __MapActor_WaitMovement(q0); }
    { PIN3; q1 = 0xa0; q1 <<= 7; q2 = 0; q0 = 0x12; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x11; __MapActor_WaitMovement(q0); }
    { PIN1; q0 = 0x9f; __PlaySound(q0); }
    { PIN3; q0 = (int)L4790; q1 = 0x4e; q2 = 0xd; __Func_8010560((void *)q0, q1, q2); }
    { PIN3; q0 = 0x12; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xf; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0x10; q1 = 0x11; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0x11; q0 = 0; __Func_809280c(q0, q1, q2); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN2; q1 = 2; q0 = 0x11; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 4; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q2 = 0x14; q0 = 0x12; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 3; q0 = 0x11; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 1; q0 = 0x12; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0x12; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q0 = 0x11; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x11; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0x81; q0 = 0x12; q1 <<= 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0x12; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 3; q0 = 0x11; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q2 = 0x14; q0 = 0x11; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 3; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q2 = 0x14; q0 = 0x12; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 3; q0 = 0x11; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 4; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q2 = 0x14; q0 = 0x12; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0x11; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0x11; q1 = 0; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x11; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x14; q0 = 0x10; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 3; q0 = 0x10; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 3; q0 = 0x11; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0x14; q0 = 0x11; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 3; q0 = 0x10; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0x10; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x11; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0x11; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 3; q0 = 9; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 9; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0xd0; q2 = 0x14; q0 = 0x11; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 1; q0 = 0x11; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0x11; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0x81; q0 = 0x12; q1 <<= 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x12; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0x11; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0x11; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 3; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0x12; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x11; q1 <<= 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0x11; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 4; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0x11; q1 = 0x103; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 0x11; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x12; q1 <<= 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0x12; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 4; q0 = 0x11; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q2 = 0x14; q0 = 0x11; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0x12; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q2 = 0x14; q0 = 0x12; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0x11; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN3; q1 = 0x80; q2 = 0x8c; q0 = 0x11; q1 <<= 1; q2 <<= 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x11; q1 <<= 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0; q0 = 0x11; __MapActor_SetPos(q0, q1, q2); }
    { PIN1; q0 = 0x11; __DeleteFieldActor(q0); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN2; q1 = 2; q0 = 9; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q2 = 0x14; q0 = 9; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0xf; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0xf; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q2 = 0; q1 = 0x12; q0 = 0x10; __Func_809280c(q0, q1, q2); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 2; q0 = 0x10; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0x10; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0x10; q2 = 0; q0 = 0x12; __Func_809280c(q0, q1, q2); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q2 = 0x14; q0 = 0x12; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 4; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q2 = 0x14; q0 = 0x12; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 3; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0x12; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 0x12; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x12; q1 <<= 1; q2 = 0xf8; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0x14; q0 = 0x12; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0x12; q1 = 1; __Func_809259c(q0, q1); }
    { PIN3; q1 = 0x80; q0 = 0x12; q1 <<= 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q2 = 0xb8; q0 = 0x12; q1 = 0xf0; __Func_80921c4(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0x12; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q1 = 0xe8; q2 = 0xa8; q0 = 0x13; q1 <<= 16; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xe8; q2 = 0xa8; q1 <<= 16; q2 <<= 16; q0 = 0x14; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_GetActor(0x13)->f0c = 0xc0 << 12;
    __MapActor_GetActor(0x13)->f3c = 0x80 << 24;
    __MapActor_GetActor(0x13)->f18 = 0xcccc;
    __MapActor_GetActor(0x13)->f50->f1e = 0x80 << 8;
    { PIN1; q0 = 0x7c; __PlaySound(q0); }
    { PIN3; q0 = 0x12; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 0; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x10; q1 <<= 1; q2 = 0xf0; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xb0; q2 = 0x14; q0 = 0x10; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0x10; q1 = 1; __Func_80925cc(q0, q1); }
    { PIN3; q0 = 0x10; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN3; q0 = 9; q1 = 0; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xf; q1 = 0; q2 = 0; __Func_809280c(q0, q1, q2); }
    { PIN3; q2 = 0; q0 = 0x10; q1 = 0; __Func_809280c(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0x12; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q1 = 0xa0; q0 = 0x12; q1 <<= 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x12; q1 = 0xf8; q2 = 0xd0; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q0 = 0x12; q1 <<= 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0x12; __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) != 0)
        goto join1_skip;
    { PIN2; q1 = 1; q0 = 0x10; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    s = 0x10;
    goto join1;
join1_skip:
    *(unsigned short *)(iwram_3001ebc + 0x1d8) += 1;
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0x12; q1 = 0x105; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x14; q0 = 0x12; q1 <<= 7; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0x10; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0; q0 = 0x10; __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) != 0)
        goto join2;
    { PIN2; q1 = 3; q0 = 0x10; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q1 = 0xb0; q0 = 0x12; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    s = 0x12;
join1:
    { PIN3; q1 = 0; q2 = 0x14; q0 = s; __Func_8093040(q0, q1, q2); }
    { PIN1; q0 = 0x898; __SetFlag(q0); }
    goto join3;
join2:
    *(unsigned short *)(iwram_3001ebc + 0x1d8) += 1;
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 4; q0 = 0x12; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0x12; q1 = 0; q2 = 0x14; __Func_8093040(q0, q1, q2); }
    { PIN1; q0 = 0x899; __SetFlag(q0); }
join3:
    { PIN3; q1 = 0x80; q0 = 0xa; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x14; q0 = 0xb; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    { PIN2; q0 = 0xa; q1 = 5; __MapActor_SetAnim(q0, q1); }
    { PIN2; q0 = 0xb; q1 = 5; __MapActor_SetAnim(q0, q1); }
    { PIN2; q1 = (int)gScript_926__0200c638; q0 = 0xc; __MapActor_SetBehavior(q0, (void *)q1); }
}

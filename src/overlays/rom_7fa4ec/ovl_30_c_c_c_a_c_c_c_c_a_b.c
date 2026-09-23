/* OvlFunc_970_2008430 -- 678 instructions, 1796 bytes, 694 encodings and 195
 * relocations identical.  Split out of
 * asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_c_a.s (3 functions); text-only, no
 * data sections.
 *
 * NEEDS `_CONST_0 = 0x0;`, WHICH THIS COMMIT ADDS to const.sym -- the file's first
 * zero entry.  It COMPLETES the function: byte-identical with it, and without it 2
 * differing encodings at size 1792, exactly the 4-byte missing pool word.  The full
 * evidence is in the const.sym entry, including that THE HALFWORD EXCEPTION WAS
 * CHECKED AND DOES NOT APPLY (this is an SImode call argument, not a byte or halfword
 * store, so there is no HImode path to account for the pooling) and that this same
 * function passes a literal 0 in four other argument slots which all reproduce as
 * `mov rN, #0`.
 *
 * ================================================================
 * BOTH SPELLINGS OF ONE GLOBAL IN ONE FUNCTION
 * ================================================================
 *
 * `iwram_3001ebc` is reached as `iwram_3001e70[0x13]` at the ONE site where r6 is
 * already live, and as `iwram_3001ebc` directly in the three branch blocks where the
 * ROM makes a FRESH pool entry.  The two globals are 0x4c apart and the ROM derives
 * one from the other's pool word only where it already has the base in hand -- so
 * WHICH SYMBOL IS IN THE POOL AT EACH SITE TELLS YOU WHICH SPELLING THE SOURCE USED
 * THERE.  This is the sibling park src/non_matching/ovl_7fa4ec/2008da4.c's finding,
 * confirmed and extended: it is per-site, not per-function.
 *
 * THE 4-ARGUMENT `neg` FILL FORM, from a 30-permutation sweep: PAIR EACH REGISTER'S
 * DEFINE WITH ITS OWN OP, IN ASCENDING REGISTER ORDER --
 * `q0 = ...; q0 <<= k; q1 = ...; q1 = -q1; q2 = ...; q2 <<= k; q3 = ...;` -- which
 * fixed all three __Func_80933f8 sites at once (10 -> 3).  Note this is NOT the same
 * as grouping all the defines then all the ops, and not the same as copying the ROM's
 * emission order; see src/overlays/rom_794ac0/ovl_30_c_a_a_c_b.c for why the source is
 * ascending regardless of the ROM's direction.
 *
 * THE BARE `__asm__ volatile ("");` IS THE RIGHT DIRECTIONAL TOOL FOR A TWO-`mov`
 * sched2 TRANSPOSITION.  A stored zero and its address materialised in the wrong
 * order; four spellings of the VALUE (block-scoped, `unsigned short`, r2-pinned, bare
 * literal) were all inert or worse, and the empty barrier alone took it 3 -> 1.  This
 * is the lighter counterpart to `do { } while (0);`, which plants two TOTAL barriers.
 *
 * No per-file Makefile flag override applies to this stem.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char *iwram_3001e70[];
extern unsigned char gScript_970__02009820[];
extern unsigned char gScript_970__020098e0[];
extern unsigned char gScript_970__0200998c[];
extern unsigned char gScript_970__02009a4c[];

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __PlaySound(int id);
extern void __SetMusicTempo(int n);
extern void __SetFlag(int id);
extern int _CONST_0;
extern void __SetDestMap(int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_WaitScript(int slot);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809233c(int a, int b, int c, int d);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void OvlFunc_970_20092ac(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_970_2008430(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *bb;
    int n;
    int *w;
    int s1;
    int s2;

    __CutsceneStart();
    { PIN4; q0 = 0x1; q1 = 0x1; q2 = 0x1; q0 = -q0; q1 = -q1; q2 = -q2; q3 = 0x0; __Func_80933f8(q0, q1, q2, q3); }
    s1 = 0x12;
    s2 = 0xc;
    __Func_8010704(0x12, 0, 3, 1, s1, s2);
    { PIN1; q0 = 0x28fe; __MessageID(q0); }
    { PIN4; q1 = 0xa; q3 = 0xc0; q0 = 0x1; q1 = -q1; q2 = 0x10; q3 <<= 8; __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q3 = 0xc0; q0 = 0x3; q1 = 0x0; q2 = 0x18; q3 <<= 8; __Func_809233c(q0, q1, q2, q3); }
    { PIN4; q3 = 0xc0; q3 <<= 8; q2 = 0x10; q1 = 0xa; q0 = 0x2; __Func_809233c(q0, q1, q2, q3); }
    { PIN1; q0 = 0x1; __MapActor_WaitMovement(q0); }
    { PIN1; q0 = 0x32; __CutsceneWait(q0); }
    { PIN2; q1 = 0x2; q0 = 0x1; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x0; q0 = 0x1; __ActorMessage(q0, q1); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN2; q1 = 0x3; q0 = 0x2; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x0; q0 = 0x2; __ActorMessage(q0, q1); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN3; q1 = 0x80; q2 = 0x0; q1 <<= 7; q0 = 0x3; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x28; __CutsceneWait(q0); }
    { PIN2; q1 = 0x0; q0 = 0x3; __ActorMessage(q0, q1); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN3; q1 = 0x80; q0 = 0x0; q1 <<= 7; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x1; q1 <<= 7; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x0; q1 <<= 7; q0 = 0x2; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x3c; __CutsceneWait(q0); }
    { PIN2; q0 = 0x0; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN2; q0 = 0x1; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN2; q1 = 0x3; q0 = 0x2; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x3c; __CutsceneWait(q0); }
    { PIN2; q1 = 0x2; q0 = 0x1; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN3; q1 = 0xe0; q1 <<= 8; q2 = 0x0; q0 = 0x1; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x0; q0 = 0x1; __Func_8092c40(q0, q1); }
    if (!__Func_8091c7c(0, 0)) {
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN3; q1 = 0x81; q2 = 0x32; q0 = 0x1; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q0 = 0x1; q1 = 0x0; __ActorMessage(q0, q1); }
    *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN3; q1 = 0x81; q0 = 0x1; q1 <<= 1; q2 = 0x32; __MapActor_Emote(q0, q1, q2); }
    *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    { PIN2; q0 = 0x1; q1 = 0x0; __ActorMessage(q0, q1); }
    }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN3; q1 = 0x80; q1 <<= 8; q2 = 0x0; q0 = 0x2; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN3; q2 = 0x32; q0 = 0x2; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0x0; q0 = 0x2; __ActorMessage(q0, q1); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN3; q1 = 0x0; q2 = 0x0; q0 = 0x1; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN3; q2 = 0x3c; q0 = 0x1; q1 = 0x101; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0x2; q0 = 0x3; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN3; q1 = 0xa0; q2 = 0x0; q1 <<= 8; q0 = 0x3; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN2; q1 = 0x0; q0 = 0x3; __ActorMessage(q0, q1); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN3; q1 = 0x80; q2 = 0x32; q0 = 0x1; q1 <<= 1; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 0x0; q0 = 0x1; __ActorMessage(q0, q1); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN2; q1 = 0x2; q0 = 0x2; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x0; q0 = 0x2; __ActorMessage(q0, q1); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN3; q1 = 0xc0; q2 = 0x0; q1 <<= 8; q0 = 0x3; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x4; q0 = 0x3; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x0; q0 = 0x3; __Func_8092c40(q0, q1); }
    if (!__Func_8091c7c(0, 0)) {
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x2; q0 = 0x1; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q1 = 0xe0; q2 = 0x0; q1 <<= 8; q0 = 0x1; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q0 = 0x1; q1 = 0x0; __ActorMessage(q0, q1); }
    *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x2; q0 = 0x1; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q1 = 0xe0; q1 <<= 8; q2 = 0x0; q0 = 0x1; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    { PIN2; q0 = 0x1; q1 = 0x0; __ActorMessage(q0, q1); }
    }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN3; q1 = 0x81; q1 <<= 1; q2 = 0x32; q0 = 0x0; __MapActor_Emote(q0, q1, q2); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN3; q1 = 0xa0; q2 = 0x0; q1 <<= 8; q0 = 0x2; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x3; q0 = 0x2; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x0; q0 = 0x2; __ActorMessage(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x0; q0 = 0x1; __ActorMessage(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x3; q0 = 0x0; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN2; q1 = 0x3; q0 = 0x3; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x0; q0 = 0x3; __Func_8092c40(q0, q1); }
    { PIN3; q1 = 0xe0; q0 = 0x1; q1 <<= 8; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 0x3; q1 <<= 8; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xa0; q1 <<= 8; q0 = 0x2; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    if (!__Func_8091c7c(0, 0)) {
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN2; q1 = 0x3; q0 = 0x3; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q0 = 0x3; q1 = 0x0; __ActorMessage(q0, q1); }
    *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN2; q1 = 0x3; q0 = 0x3; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    { PIN2; q0 = 0x3; q1 = 0x0; __ActorMessage(q0, q1); }
    }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN3; q1 = 0xc0; q2 = 0x0; q1 <<= 8; q0 = 0x1; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN2; q1 = 0x0; q0 = 0x1; __ActorMessage(q0, q1); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN3; q1 = 0xc0; q1 <<= 8; q2 = 0x0; q0 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN3; q1 = 0xc0; q2 = 0x0; q1 <<= 8; q0 = 0x2; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN2; q1 = 0x3; q0 = 0x2; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x0; q0 = 0x2; __ActorMessage(q0, q1); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN2; q1 = 0x3; q0 = 0x3; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x0; q0 = 0x3; __ActorMessage(q0, q1); }
    { PIN1; q0 = 0x11; __PlaySound(q0); }
    { PIN1; q0 = 0xa; __CutsceneWait(q0); }
    { PIN2; q0 = 0x0; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN2; q0 = 0x1; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN2; q1 = 0x3; q0 = 0x2; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x28; __CutsceneWait(q0); }
    { PIN3; q0 = 0x0; q1 = 0x13333; q2 = 0x9999; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x13333; q2 = 0x9999; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x13333; q2 = 0x9999; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x9999; q0 = 0x3; q1 = 0x13333; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(0, gScript_970__02009820);
    { PIN1; q0 = 0x32; __CutsceneWait(q0); }
    { PIN4; q0 = 0x84; q0 <<= 17; q1 = 0x1; q1 = -q1; q2 = 0xc8; q2 <<= 16; q3 = 0x1; __Func_80933f8(q0, q1, q2, q3); }
    __MapActor_SetBehavior(1, gScript_970__020098e0);
    { PIN1; q0 = 0x32; __CutsceneWait(q0); }
    __MapActor_SetBehavior(2, gScript_970__0200998c);
    { PIN1; q0 = 0x2; __MapActor_WaitScript(q0); }
    { PIN3; q1 = 0x80; q0 = 0x0; q1 <<= 7; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x1; q1 <<= 7; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x2; q1 <<= 7; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q2 = 0x20; q1 = 0x0; q2 = -q2; q0 = 0x3; __Func_8092304(q0, q1, q2); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN3; q1 = 0x80; q1 <<= 7; q2 = 0x0; q0 = 0x3; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x3c; __CutsceneWait(q0); }
    { PIN3; q1 = 0x80; q2 = 0x0; q1 <<= 8; q0 = 0x3; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    __MapActor_SetBehavior(3, gScript_970__02009a4c);
    { PIN1; q0 = 0x3; __MapActor_WaitScript(q0); }
    { PIN3; q1 = 0xc0; q1 <<= 8; q2 = 0x0; q0 = 0x3; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN4; q0 = 0xd8; q0 <<= 16; q1 = 0x1; q1 = -q1; q2 = 0xa8; q2 <<= 16; q3 = 0x1; __Func_80933f8(q0, q1, q2, q3); }
    __Func_8093530();
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q1 = 0x80; q0 = 0x0; q1 <<= 8; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x1; q1 <<= 8; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x2; q1 <<= 8; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x0; q1 <<= 8; q0 = 0x3; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x28; __CutsceneWait(q0); }
    { PIN2; q0 = 0x0; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN2; q0 = 0x1; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN2; q0 = 0x3; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN2; q1 = 0x3; q0 = 0x2; __MapActor_DoAnim(q0, q1); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN1; q0 = 0x43; __PlaySound(q0); }
    { PIN1; q0 = 0xf0; __SetMusicTempo(q0); }
    OvlFunc_970_20092ac();
    { PIN1; q0 = 0x50; __CutsceneWait(q0); }
    bb = iwram_3001e70[0];
    a = __MapActor_GetActor(8);
    *(int *)(a + 0x34) = 0x83;
    *(int *)(a + 0x30) = 0x80 << 10;
    w = (int *)(bb + (0x8e << 1));
    *w = 0xffffd000;
    { PIN3; q0 = 0x8; q1 = 0x3c; q2 = 0x0; __Func_8092304(q0, q1, q2); }
    *w = 0xffffa000;
    { PIN3; q2 = 0x0; q1 = 0x3c; q0 = 0x8; __Func_8092304(q0, q1, q2); }
    { PIN1; q0 = 0x50; __CutsceneWait(q0); }
    { PIN1; q0 = 0x64; __CutsceneWait(q0); }
    { PIN2; q0 = 0x23333; q1 = 0x28f; __Func_80933d4(q0, q1); }
    { PIN4; q0 = 0xca; q0 <<= 18; q1 = 0x1; q1 = -q1; q2 = 0xa8; q2 <<= 16; q3 = 0x1; __Func_80933f8(q0, q1, q2, q3); }
    { PIN1; q0 = 0x96; q0 <<= 1; __CutsceneWait(q0); }
    b = iwram_3001e70[0x13];
    *(int *)(b + (0xe0 << 1)) = 0x100;
    n = 0;
    __asm__ volatile ("");
    *(unsigned short *)(0xa0 << 19) = n;
    *(int *)(b + (0xe4 << 1)) = 0x60;
    __MapTransitionOut();
    __WaitMapTransition();
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN1; q0 = 0x8d; q0 <<= 1; __SetFlag(q0); }
    { PIN2; q0 = (int)&_CONST_0; q1 = 0x9; __SetDestMap(q0, q1); }
}

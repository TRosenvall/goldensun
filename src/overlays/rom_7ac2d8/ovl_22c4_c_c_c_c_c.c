/* OvlFunc_924_200a8b0 -- 499 instructions, 1308 bytes, 513 encodings and 132
 * relocations identical.  Its .s was the file's only function, but the file ALSO
 * carried a `.section .data` holding .L60b8 (52 bytes of .incbin from orig.bin), so
 * this was NOT a plain whole-file conversion.
 *
 * THE DATA WAS REHOMED, NOT EMITTED FROM C, per docs/elevation.md's own rule:
 * rehome the blob when it is large, opaque, or an .incbin of real data; emit it
 * from C only when it is a handful of words you can read.  52 bytes of .incbin is
 * the first case.  It now lives in
 * asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_c_c_dat.s with a `.global .L60b8` (a
 * zero-byte export declaration, needed because the reference is now cross-object),
 * and overlay.ld's .data line was repointed to it.  The rehome was gated
 * byte-neutral on its own BEFORE this .c was written.
 *
 * 106 -> 0 IN FOUR STEPS:
 *
 * 1. Plain transcription screened at 106 of 508.
 * 2. ARGUMENT PINS AT EVERY REPEATED-CONSTANT SITE, WRITTEN IN THE ROM'S OWN FILL
 *    ORDER -> 11.  The fill order was extracted MECHANICALLY (awk over the
 *    reference, grouping by `bl`) rather than by eye, which is what made 58 pins
 *    tractable at all.
 * 3. NAMING THE STACK ARGUMENTS of the two 6-argument calls -> 2.  Exactly batch
 *    280's lever: the ROM emits `mov r3 / mov r2 / str / str` while literals emit
 *    `mov r3 / str / mov r3 / str` alternating through r3.
 * 4. A SINGLE `__asm__ volatile ("" : : "r" (q0));` AFTER `q0 = 0xd2;` -> EXACT.
 *
 * STEP 4 IS THE NEW RULE: WHEN NO PERMUTATION MOVES A FILL, TRY A BARRIER AFTER THE
 * FIRST PINNED ASSIGNMENT.  All 25 mov-permutations and all 6 shift-order
 * permutations were inert at 2; only the barrier moved it.  It was also the only
 * thing that fixed the analogous site in this function's sibling
 * (src/non_matching/ovl_77dd1c/2009b18.c), where a 3-position x 7-spelling sweep
 * found nothing.  This confirms batch 280's reading that these fills land BY
 * RELATION, NOT POSITION, and adds the handle for when relation-by-permutation runs
 * out.
 *
 * TWO MECHANISMS WORTH CARRYING, both read out of the compiler rather than guessed:
 *
 * `define_peephole` IN arm.md (line 523) EXPLAINS `mov rX,#N / add rX,sp` AGAINST
 * `add rX,sp,#N`.  It is a PEEPHOLE, so it fires only when the two insns end up
 * adjacent in the output -- the ROM's two-instruction form is NOT A SOURCE FACT AT
 * ALL, it is sched2 having separated them.  That retired what looked like a
 * structural blocker on a sibling function.
 *
 * `update_equiv_regs` (local-alloc.c) DOES `REG_LIVE_LENGTH (regno) *= 2;` FOR ANY
 * PSEUDO WITH A CONSTANT REG_EQUIV.  Since global-alloc priority is
 * floor_log2(n_refs) * freq / live_length, LENGTHENING A POOLED SYMBOL'S LIVE RANGE
 * BY HOISTING ITS ASSIGNMENT TO A NAMED LOCAL DEMOTES IT BELOW ITS COMPETITORS.  On
 * a sibling that took 336 -> 90 differing.
 *
 * And the converse: PINNING A POOLED SYMBOL TO A *CALL-CLOBBERED* REGISTER
 * REPRODUCES THE ROM'S REMATERIALISATION where CSE otherwise commons it, because a
 * hard register cannot survive the intervening `bl` so cse1 has nothing to common
 * into.  The ORDER matters -- the assignment must come AFTER the index computation,
 * or the load lands in an earlier basic block.
 *
 * No .sym entry is implied: 0x207e9f and the other pooled constants here all
 * reproduce as plain literals, and none has the in-function control the bar
 * requires.  No per-file Makefile flag override applies to this stem.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char L60b8[] __asm__(".L60b8");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __StopTask(void (*fn)(void));
extern void __Func_8091200(int a, int b);
extern void __Func_8091220(int a, int b);
extern void __Func_8091254(int a);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void OvlFunc_common0_0(unsigned char *a, int b);
extern void OvlFunc_common0_18(int a, int b, int c, int d);
extern void OvlFunc_924_200a648(void);
extern void OvlFunc_924_200a844(void);
extern void OvlFunc_924_200cf44(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_924_200a8b0(void)
{
    int z;
    unsigned char *a;

    z = *(int *)(__MapActor_GetActor(0xa) + 8) / 0x100000;
    __CutsceneStart();
    switch (z) {
    case 0x33:
        { PIN3; q2 = 0x6666; q1 = 0xcccc; q0 = 3; __MapActor_SetSpeed(q0, q1, q2); }
        __CutsceneWait(0x14);
        { PIN2; q1 = 2; q0 = 3; __Func_80925cc(q0, q1); }
        __CutsceneWait(0x14);
        { PIN3; q1 = 0xd0; q0 = 3; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xa0; q1 <<= 7; q2 = 0xa; q0 = 0; __Func_8092adc(q0, q1, q2); }
        __MessageID(0x157f);
        { PIN2; q1 = 0; q0 = 3; __Func_8092c40(q0, q1); }
        if (__Func_8091c7c(0, 0) == 0) {
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
            __CutsceneWait(0x14);
            __MapActor_DoAnim(3, 3);
            __Func_8093040(3, 0, 0x14);
        } else {
            __CutsceneWait(0x14);
            __MapActor_DoAnim(3, 4);
            { PIN3; q2 = 0x14; q0 = 3; q1 = 0; __Func_8093040(q0, q1, q2); }
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        }
        { PIN3; q1 = 0x80; q2 = 0x3c; q1 <<= 1; q0 = 0; __MapActor_Emote(q0, q1, q2); }
        OvlFunc_common0_0(__MapActor_GetActor(1), 1);
        OvlFunc_common0_0(__MapActor_GetActor(2), 1);
        { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q1 = 0xda; q2 = 0x96; q0 = 1; q1 <<= 18; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q1 = 0xda; q2 = 0x96; q0 = 2; q1 <<= 18; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
        { PIN3; q1 = 0xde; q2 = 0x9e; q0 = 2; q1 <<= 2; q2 <<= 2; __Func_809218c(q0, q1, q2); }
        { PIN3; q1 = 0xdc; q2 = 0x9a; q0 = 1; q1 <<= 2; q2 <<= 2; __Func_80921c4(q0, q1, q2); }
        { PIN3; q1 = 0xa0; q1 <<= 7; q2 = 0; q0 = 1; __Func_8092adc(q0, q1, q2); }
        __MapActor_WaitMovement(2);
        { PIN3; q1 = 0x80; q2 = 0; q0 = 2; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
        { PIN2; q1 = 1; q0 = 1; __Func_80925cc(q0, q1); }
        __CutsceneWait(0x14);
        __Func_8093040(1, 0, 0x14);
        { PIN3; q0 = 3; q1 = 0x101; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
        __Func_8093040(3, 0, 0x14);
        { PIN3; q1 = 0xc0; q0 = 1; q1 <<= 6; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xb0; q2 = 0x14; q0 = 2; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
        __MapActor_SetAnim(1, 3);
        { PIN2; q1 = 3; q0 = 2; __MapActor_DoAnim(q0, q1); }
        __CutsceneWait(0x1e);
        { PIN3; q1 = 0xa0; q0 = 1; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 2; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        { PIN3; q2 = 0x14; q0 = 2; q1 = 0; __Func_8093040(q0, q1, q2); }
        { PIN2; q1 = 3; q0 = 3; __MapActor_DoAnim(q0, q1); }
        __CutsceneWait(0x14);
        __Func_8093040(3, 0, 0x14);
        { PIN3; q1 = 0xd2; q2 = 0xa4; q1 <<= 2; q2 <<= 2; q0 = 3; __Func_809218c(q0, q1, q2); }
        __CutsceneWait(5);
        { PIN3; q1 = 0xa0; q1 <<= 7; q2 = 0; q0 = 2; __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0xa);
        { PIN3; q1 = 0x80; q1 <<= 7; q2 = 0; q0 = 0; __Func_8092adc(q0, q1, q2); }
        __MapActor_WaitMovement(3);
        __CutsceneWait(0xa);
        __Func_8093040(3, 0, 0x14);
        { PIN3; q1 = 0xd0; q0 = 3; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        { PIN3; q2 = 0x14; q0 = 3; q1 = 0; __Func_8093040(q0, q1, q2); }
        { PIN2; q1 = 1; q0 = 1; __Func_80925cc(q0, q1); }
        __CutsceneWait(0x14);
        { PIN3; q1 = 0xc0; q0 = 1; q1 <<= 8; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
        { PIN2; q1 = 0; q0 = 1; __Func_8092c40(q0, q1); }
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0x14);
            __MapActor_DoAnim(1, 3);
            { PIN3; q2 = 0x14; q0 = 1; q1 = 0; __Func_8093040(q0, q1, q2); }
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        } else {
            *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
            __CutsceneWait(0x14);
            __MapActor_DoAnim(1, 4);
            __Func_8093040(1, 0, 0x14);
        }
        { PIN2; q1 = 0x10; q0 = 3; __MapActor_SetAnim(q0, q1); }
        __CutsceneWait(0x1e);
        { PIN2; q1 = 1; q0 = 3; __Func_80925cc(q0, q1); }
        __CutsceneWait(0x14);
        __Func_8093040(3, 0, 0x14);
        { PIN3; q1 = 0xd2; q2 = 0x9e; q0 = 3; q1 <<= 2; q2 <<= 2; __Func_80921c4(q0, q1, q2); }
        { PIN3; q1 = 0xa0; q0 = 0; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xa0; q0 = 1; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x80; q2 = 0; q1 <<= 8; q0 = 2; __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0x1e);
        OvlFunc_924_200cf44();
        __CutsceneWait(0x32);
        __PlaySound(0x83);
        { PIN2; q0 = 0x80; q0 <<= 9; q1 = 0; __Func_8091220(q0, q1); }
        { PIN2; q1 = 0; q0 = 0x207e9f; __Func_8091200(q0, q1); }
        __Func_8091254(0xa);
        __WaitFrames(1);
        __PlaySound(0xdc);
        __WaitFrames(0x28);
        { PIN2; q0 = 0x80; q1 = 0; q0 <<= 9; __Func_8091200(q0, q1); }
        __Func_8091254(0x3c);
        __WaitFrames(0x3c);
        __PlaySound(0xd1);
        OvlFunc_924_200a844();
        { int s4 = 1; int s5 = 2; __CopyMapTiles(0x7e, 0x23, 0x74, 0x23, s4, s5); }
        { PIN3; q2 = 0x23; q1 = 0x74; q0 = (int)L60b8; __Func_8010560((unsigned char *)q0, q1, q2); }
        __StopTask(OvlFunc_924_200a648);
        __CutsceneWait(0x14);
        { PIN2; q1 = 3; q0 = 3; __MapActor_DoAnim(q0, q1); }
        __CutsceneWait(0x14);
        { PIN3; q1 = 0xc0; q2 = 0xc0; q0 = 3; q1 <<= 10; q2 <<= 9; __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_Jump(3, 4, 0);
        { PIN3; q1 = 0xd2; q2 = 0x96; q1 <<= 2; q2 <<= 2; q0 = 3; __Func_809218c(q0, q1, q2); }
        __MapActor_WaitMovement(3);
        { int t4 = 0x34; int t5 = 0x24; __Func_8010704(0x74, 0x24, 3, 4, t4, t5); }
        { PIN4; q0 = 0xd2; __asm__ volatile ("" : : "r" (q0)); q1 = 0xe0; q2 = 0x98; q3 = 0xdf; q1 <<= 14; q2 <<= 18; q0 <<= 18;
          OvlFunc_common0_18(q0, q1, q2, q3); }
        { PIN3; q0 = 3; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q1 = 0xd2; q2 = 0x8c; q0 = 3; q1 <<= 2; q2 <<= 2; __Func_80921c4(q0, q1, q2); }
        { PIN3; q1 = 0; q2 = 0; q0 = 3; __MapActor_SetPos(q0, q1, q2); }
        __CutsceneWait(0x14);
        { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0x80; q0 = 1; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
        { PIN3; q1 = 0xb0; q2 = 0; q1 <<= 8; q0 = 2; __Func_8092adc(q0, q1, q2); }
        __CutsceneWait(0xa);
        __MapActor_SetAnim(0, 3);
        __MapActor_SetAnim(1, 3);
        { PIN2; q1 = 3; q0 = 2; __MapActor_DoAnim(q0, q1); }
        __CutsceneWait(0x14);
        __MapActor_SetAnim(1, 2);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_TravelTo(1, *(short *)(a + 0xa), *(short *)(a + 0x12));
        __MapActor_SetAnim(2, 2);
        a = __MapActor_GetActor(0);
        if (a != 0)
            __MapActor_TravelTo(2, *(short *)(a + 0xa), *(short *)(a + 0x12));
        __MapActor_WaitMovement(1);
        { PIN3; q1 = 0; q2 = 0; q0 = 1; __MapActor_SetPos(q0, q1, q2); }
        __MapActor_WaitMovement(2);
        __MapActor_SetPos(2, 0, 0);
        __SetFlag(0x871);
        break;
    }
    __CutsceneEnd();
}

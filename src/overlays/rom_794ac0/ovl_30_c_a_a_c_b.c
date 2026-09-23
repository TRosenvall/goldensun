/* OvlFunc_899_200afd4 -- 702 instructions, 1828 bytes, 714 encodings and 192
 * relocations identical.  Split out of asm/overlays/rom_794ac0/ovl_30_c_a_a_c.s;
 * OvlFunc_899_200b6f8 stays in asm.  datacheck.py reports no data sections, so the
 * split is text-only.  167 pin blocks; `void` return, read off `pop {r0} / bx r0`
 * against the donor's `pop {r1}` for its `int`.
 *
 * ================================================================
 * THIS TARGET WAS ASSIGNED THREE TIMES AND OPENED ONCE.  IT WAS NEVER DIFFICULT.
 * ================================================================
 *
 * Batches 281 and 282 both assigned it and both agents ran out of budget on earlier
 * targets, so it was carried forward twice with the corpus's highest unattempted
 * call-family score (34/34) untested.  Path once opened: naive C gave 708 of 708
 * differing with r8-r11 saved in the prologue -- the textbook repeated-pool-constant
 * CSE.  TRANSCRIBING EVERY CALL SITE MECHANICALLY FROM THE ROM'S OWN EMISSION ORDER
 * AS A PINNED BLOCK TOOK IT TO 4 OF 704 IN ONE STEP.  The last 4 were two sites.
 *
 * That is the second time a twice-carried target has landed cheaply on the round it
 * was finally opened -- OvlFunc_945_200aff0 landed in batch 282 with ZERO pins after
 * being carried from 281.  A CARRIED-FORWARD TARGET IS NOT EVIDENCE OF DIFFICULTY,
 * and the selector should promote one that has been skipped rather than re-ranking it
 * by score alone.
 *
 * ================================================================
 * THE LEVER THAT CLOSED IT: A PINNED FILL'S OP ORDER *INVERTS* THROUGH sched2, AND
 * `mov r0`'s POSITION SELECTS THE DIRECTION
 * ================================================================
 *
 * Copying the ROM's instruction order literally into the source is WRONG for the ops.
 * For `{ PIN3; q1 = A; q2 = B; ... }`:
 *
 *   ROM shifts ASCENDING  (`lsl r1` then `lsl r2`)  -> write them ascending
 *   ROM shifts DESCENDING (`lsl r2` then `lsl r1`)  -> WRITE THEM ASCENDING TOO
 *
 * Writing them descending transposes the two `mov`s instead.  That was this
 * function's entire residue, 4 -> 0, and two sites in
 * src/overlays/rom_7d0e88/ovl_1528_a_a_c_a_b.c.
 *
 * This refines batch 281's "split the pinned mov/lsl pair" and batch 282's "the shift
 * placement decides the mov order, and both directions occur": the DIRECTIONS occur in
 * the ROM's OUTPUT, but the SOURCE is ascending either way.  Read the ROM to know
 * which case you are in, then write ascending.
 *
 * No .sym entry is implied.  No per-file Makefile flag override applies to this stem.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gScript_899__0200d17c[];

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int id);
extern void __MapActor_WaitMovement(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __SetCameraTarget(int a, int b);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_800fe9c(void);
extern void __Func_8093530(void);
extern void __Func_8097adc(void);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void OvlFunc_899_200c5cc(void);
extern void OvlFunc_899_200c684(void);
extern void OvlFunc_899_200c5f4(int a, int b);
extern void OvlFunc_899_200c658(int a, int b);
extern void OvlFunc_899_200c60c(int a, int b, int c);
extern void OvlFunc_899_200c624(int a, int b, int c);
extern void OvlFunc_899_200c63c(int a, int b, int c);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_899_200afd4(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *s;

    p = *(unsigned char **)(__MapActor_GetActor(0xc) + 0x50);
    __CutsceneStart();
    { PIN3; q1 = 0xc6; q2 = 0xd0; q0 = 0xa; q1 <<= 18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xc8; q2 = 0xc8; q0 = 0xb; q1 <<= 18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xc2; q2 = 0xc4; q1 <<= 18; q2 <<= 17; q0 = 0xc; __MapActor_SetPos(q0, q1, q2); }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
    { PIN2; q0 = 0xa; q1 = 0x9; __MapActor_SetAnim(q0, q1); }
    { PIN2; q0 = 0xb; q1 = 0x9; __MapActor_SetAnim(q0, q1); }
    { PIN2; q1 = 0x9; q0 = 0xc; __MapActor_SetAnim(q0, q1); }
    q = __MapActor_GetActor(0xc) + 0x23;
    *q &= 0xfe;
    p[9] |= 0xc;
    s = gScript_899__0200d17c;
    __MapActor_SetBehavior(0xa, s);
    { PIN3; q1 = 0xc6; q2 = 0xdc; q0 = 0x0; q1 <<= 18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xca; q2 = 0xd8; q0 = 0x1; q1 <<= 18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xc2; q2 = 0xdc; q0 = 0x2; q1 <<= 18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 0x0; q1 <<= 8; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xb0; q0 = 0x1; q1 <<= 8; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xb0; q0 = 0x2; q1 <<= 8; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q0 = 0x8; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xb0; q0 = 0x8; q1 <<= 8; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x209;
    { PIN2; q0 = 0x0; q1 = 0x0; __SetCameraTarget(q0, q1); }
    __Func_8093530();
    __Func_800fe9c();
    OvlFunc_899_200c5cc();
    __MapActor_SetBehavior(0xb, s);
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    __MapActor_SetBehavior(0xc, s);
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN1; q0 = 0x12e4; __MessageID(q0); }
    { PIN2; q0 = 0xa; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN3; q1 = 0x81; q2 = 0x0; q1 <<= 1; q0 = 0x8; __MapActor_Emote(q0, q1, q2); }
    { PIN1; q0 = 0x3c; __CutsceneWait(q0); }
    { PIN2; q0 = 0x8; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN3; q1 = 0xca; q2 = 0xe4; q0 = 0x8; q1 <<= 2; q2 <<= 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xc6; q2 = 0xd8; q0 = 0x1; q1 <<= 2; q2 <<= 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x0; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xca; q2 = 0xcc; q0 = 0x8; q1 <<= 2; q2 <<= 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x8; q1 <<= 8; q2 = 0x0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xca; q2 = 0xd8; q0 = 0x1; q1 <<= 2; q2 <<= 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q1 <<= 8; q2 = 0x0; q0 = 0x1; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q2 = 0x14; q0 = 0x8; q1 = 0x3; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN2; q0 = 0x8; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN3; q1 = 0xc0; q2 = 0xcc; q1 <<= 2; q2 <<= 1; q0 = 0x8; __Func_80921c4(q0, q1, q2); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0x8; q1 = 0x0; q2 = 0x0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0x8; q2 = 0x0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x8; q2 = 0x0; __Func_809280c(q0, q1, q2); }
    { PIN3; q2 = 0x28; q0 = 0x2; q1 = 0x8; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN2; q0 = 0x8; q1 = 0x1e; OvlFunc_899_200c5f4(q0, q1); }
    { PIN2; q0 = 0x0; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN2; q0 = 0x1; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN3; q0 = 0x2; q1 = 0x3; q2 = 0x14; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN3; q1 = 0xba; q2 = 0xcc; q1 <<= 2; q2 <<= 1; q0 = 0x8; __Func_80921c4(q0, q1, q2); }
    { PIN1; q0 = 0x32; __CutsceneWait(q0); }
    { PIN2; q0 = 0xb; q1 = 0x2; __Func_80925cc(q0, q1); }
    { PIN2; q0 = 0xb; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    __MapActor_SetBehavior(0xb, s);
    { PIN2; q1 = 0x1; q0 = 0x1; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0x0; q1 = 0xb; q2 = 0x0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0xb; q2 = 0x0; __Func_809280c(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0x2; q1 = 0xb; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN2; q0 = 0x1; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN2; q0 = 0xc; q1 = 0x2; __Func_80925cc(q0, q1); }
    { PIN2; q0 = 0xc; q1 = 0x1e; OvlFunc_899_200c5f4(q0, q1); }
    __MapActor_SetBehavior(0xc, s);
    { PIN3; q2 = 0x0; q1 = 0x103; q0 = 0x1; __MapActor_Emote(q0, q1, q2); }
    { PIN1; q0 = 0x3c; __CutsceneWait(q0); }
    { PIN2; q0 = 0x1; q1 = 0x1e; OvlFunc_899_200c5f4(q0, q1); }
    { PIN3; q0 = 0x2; q1 = 0x0; q2 = 0x1e; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0x2; q2 = 0x1e; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0x2; q1 = 0x3; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN2; q0 = 0x2; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN3; q0 = 0x1; q1 = 0x2; q2 = 0x1e; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN3; q2 = 0x1e; q0 = 0x0; q1 = 0x3; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN2; q0 = 0x2; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN3; q1 = 0x80; q2 = 0x0; q1 <<= 1; q0 = 0x1; __MapActor_Emote(q0, q1, q2); }
    { PIN1; q0 = 0x3c; __CutsceneWait(q0); }
    { PIN2; q0 = 0x1; q1 = 0x1e; OvlFunc_899_200c5f4(q0, q1); }
    { PIN3; q1 = 0x101; q2 = 0x0; q0 = 0x2; __MapActor_Emote(q0, q1, q2); }
    { PIN1; q0 = 0x3c; __CutsceneWait(q0); }
    { PIN3; q2 = 0x14; q0 = 0x0; q1 = 0x1; OvlFunc_899_200c624(q0, q1, q2); }
    { PIN2; q0 = 0x0; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN3; q2 = 0x14; q0 = 0x1; q1 = 0x3; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN2; q0 = 0x2; q1 = 0x0; OvlFunc_899_200c658(q0, q1); }
    __Func_8097adc();
    { PIN3; q0 = 0x0; q1 = 0x2; q2 = 0x0; __Func_809280c(q0, q1, q2); }
    { PIN3; q2 = 0xa; q0 = 0x1; q1 = 0x2; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN2; q0 = 0x0; q1 = 0x1; __Func_809259c(q0, q1); }
    { PIN2; q1 = 0x1; q0 = 0x1; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q1 = 0x81; q1 <<= 1; q0 = 0x1; __MapActor_Surprise(q0, q1); }
    { PIN1; q0 = 0x3c; __CutsceneWait(q0); }
    { PIN3; q0 = 0x0; q1 = 0x1; q2 = 0xa; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN2; q1 = 0x0; q0 = 0x1; __Func_8092c40(q0, q1); }
    if (!__Func_8091c7c(0, 0)) {
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN3; q0 = 0x0; q1 = 0x2; q2 = 0x0; __Func_809280c(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0x1; q1 = 0x2; OvlFunc_899_200c60c(q0, q1, q2); }
    OvlFunc_899_200c684();
    { PIN2; q0 = 0x0; q1 = 0x1; __Func_809259c(q0, q1); }
    { PIN2; q1 = 0x1; q0 = 0x1; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q0 = 0x1; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    } else {
    *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    { PIN3; q1 = 0x81; q1 <<= 1; q2 = 0x0; q0 = 0x1; __MapActor_Emote(q0, q1, q2); }
    { PIN1; q0 = 0x3c; __CutsceneWait(q0); }
    { PIN2; q0 = 0x1; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN3; q0 = 0x0; q1 = 0x2; q2 = 0x0; __Func_809280c(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0x1; q1 = 0x2; OvlFunc_899_200c60c(q0, q1, q2); }
    OvlFunc_899_200c684();
    { PIN2; q0 = 0x0; q1 = 0x1; __Func_809259c(q0, q1); }
    { PIN2; q0 = 0x1; q1 = 0x1; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    }
    { PIN3; q1 = 0x81; q1 <<= 1; q2 = 0x0; q0 = 0x2; __MapActor_Emote(q0, q1, q2); }
    { PIN1; q0 = 0x3c; __CutsceneWait(q0); }
    { PIN3; q2 = 0x14; q1 = 0x4; q0 = 0x2; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN1; q0 = 0x12f2; __MessageID(q0); }
    { PIN2; q0 = 0x2; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN2; q0 = 0x0; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN3; q0 = 0x1; q1 = 0x3; q2 = 0x28; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN3; q1 = 0xca; q2 = 0xcc; q0 = 0x8; q1 <<= 2; q2 <<= 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q1 <<= 8; q2 = 0x0; q0 = 0x8; __Func_8092adc(q0, q1, q2); }
    { PIN1; q0 = 0x1e; __CutsceneWait(q0); }
    { PIN3; q0 = 0x0; q1 = 0x8; q2 = 0x0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x8; q2 = 0x0; __Func_809280c(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0x2; q1 = 0x8; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN2; q1 = 0x1; q0 = 0x8; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q0 = 0x8; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN3; q0 = 0x9; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xba; q2 = 0xcc; q0 = 0x9; q1 <<= 18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0xcc; q0 = 0x9; q1 <<= 2; q2 <<= 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0xa; q2 = 0x1e; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN3; q1 = 0xba; q2 = 0xcc; q0 = 0xd; q1 <<= 18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0xcc; q0 = 0xd; q1 <<= 2; q2 <<= 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xba; q2 = 0xcc; q0 = 0xe; q1 <<= 18; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xc4; q2 = 0xc8; q0 = 0xe; q1 <<= 2; q2 <<= 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0xc2; q2 = 0xd4; q1 <<= 2; q2 <<= 1; q0 = 0xd; __Func_80921c4(q0, q1, q2); }
    { PIN1; q0 = 0xe; __MapActor_WaitMovement(q0); }
    { PIN3; q0 = 0xd; q1 = 0xa; q2 = 0x0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xa; q2 = 0x14; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN3; q0 = 0x0; q1 = 0x9; q2 = 0x0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0x1; q1 = 0x9; q2 = 0x0; __Func_809280c(q0, q1, q2); }
    { PIN3; q0 = 0x2; q1 = 0x9; q2 = 0x14; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0x9; q1 = 0x4; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN2; q0 = 0x9; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN2; q0 = 0xb; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN2; q0 = 0xc; q1 = 0x1e; OvlFunc_899_200c5f4(q0, q1); }
    { PIN3; q2 = 0x14; q0 = 0x9; q1 = 0xd; OvlFunc_899_200c624(q0, q1, q2); }
    { PIN2; q1 = 0x1; q0 = 0xd; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q0 = 0xd; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN3; q0 = 0x9; q1 = 0x3; q2 = 0x1e; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0xe; q2 = 0x14; OvlFunc_899_200c624(q0, q1, q2); }
    { PIN3; q0 = 0x9; q1 = 0x3; q2 = 0x1e; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0x9; q1 = 0xa; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN2; q0 = 0x9; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN2; q0 = 0xd; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN3; q0 = 0xe; q1 = 0x3; q2 = 0x14; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN3; q2 = 0x14; q0 = 0xd; q1 = 0xe; OvlFunc_899_200c624(q0, q1, q2); }
    { PIN2; q0 = 0xd; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN3; q0 = 0xe; q1 = 0x3; q2 = 0x14; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN3; q1 = 0xc6; q2 = 0xc4; q0 = 0xe; q1 <<= 2; q2 <<= 1; __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0xc4; q2 = 0xc8; q0 = 0xd; q1 <<= 2; q2 <<= 1; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xc; q2 = 0x0; q0 = 0xd; __Func_809280c(q0, q1, q2); }
    { PIN1; q0 = 0xe; __MapActor_WaitMovement(q0); }
    { PIN3; q2 = 0x14; q0 = 0xe; q1 = 0xb; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN2; q1 = 0x1; q0 = 0xd; __Func_80925cc(q0, q1); }
    { PIN1; q0 = 0x14; __CutsceneWait(q0); }
    { PIN2; q0 = 0xd; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN3; q2 = 0x14; q0 = 0xe; q1 = 0x4; OvlFunc_899_200c63c(q0, q1, q2); }
    { PIN2; q0 = 0xe; q1 = 0x1e; OvlFunc_899_200c5f4(q0, q1); }
    { PIN3; q2 = 0x14; q0 = 0xd; q1 = 0x0; OvlFunc_899_200c624(q0, q1, q2); }
    { PIN2; q0 = 0xd; q1 = 0x14; OvlFunc_899_200c5f4(q0, q1); }
    { PIN2; q0 = 0x0; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN2; q0 = 0x1; q1 = 0x3; __MapActor_SetAnim(q0, q1); }
    { PIN3; q0 = 0x2; q1 = 0x3; q2 = 0x32; OvlFunc_899_200c63c(q0, q1, q2); }
    *(int *)(iwram_3001ebc + (0xe4 << 1)) = 0x1e;
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x201;
    __MapTransitionOut();
    __WaitMapTransition();
    { PIN1; q0 = 0x3c; __CutsceneWait(q0); }
    __CutsceneEnd();
}

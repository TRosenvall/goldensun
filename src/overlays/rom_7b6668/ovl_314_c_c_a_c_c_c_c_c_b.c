// fakematch
/* OvlFunc_928_2009060  --  0x02009060
 *
 * Cut out of goldensun/asm/overlays/rom_7b6668/ovl_314_c_c_a_c_c_c_c_c.s.
 *
 * Seven consecutive __MapActor_GetActor(0x12) calls, each feeding one field
 * store, then an ordinary cutscene tail. 83 instructions.
 *
 * TWO NAMED CONSTANTS AND THE ALLOCATOR PUT THEM THE WRONG WAY ROUND. The ROM
 * holds `0` in r6 and `0x80 << 24` in r5 across all seven stores; gcc gives the
 * first-assigned r5 and the second r6, which is exactly backwards and costs 10
 * of 83. `docs/elevation.md` records that "the second-assigned wins the
 * lower-numbered register" for a pair like this -- HERE THAT PREDICTS THE ROM
 * AND GCC DOES THE OPPOSITE, so the rule is not general.
 *
 * DECLARATION ORDER IS INERT: swapping the two declarations, and merging them
 * into one `int z, v;`, both give the identical 10. What settles it is pinning
 * both -- `register int z __asm__("r6")` and `register int v __asm__("r5")`.
 * Pinning is cheap here because these are LOW registers; the batch-207
 * pressure boundary is about r8-r11 and does not apply.
 *
 * Torn down: with the pins removed and everything else unchanged the score is
 * 10, so both are load-bearing and neither alone was tried in isolation because
 * the pair is what the ROM fixes.
 *
 * __MapActor_SetBehavior is called once with a small integer and once with a
 * script address, so it is declared taking an `int` and the script is passed as
 * `(int)gScript_...` -- the same cast idiom the tree already uses for `_MSG_`
 * symbols, chosen over two incompatible declarations of one callee.
 */
extern unsigned char gScript_928__020095b0[];
extern void OvlFunc_928_2008500(void);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_928_2009060(void)
{
    register int z __asm__("r6");
    register int v __asm__("r5");

    __CutsceneStart();
    { PIN2; q1 = 1; q0 = 0x12; __MapActor_SetBehavior(q0, q1); }
    z = 0;
    *(int *)(__MapActor_GetActor(0x12) + 0x6c) = z;
    v = 0x80;
    v <<= 24;
    *(int *)(__MapActor_GetActor(0x12) + 0x38) = v;
    *(int *)(__MapActor_GetActor(0x12) + 0x40) = v;
    *(int *)(__MapActor_GetActor(0x12) + 0x24) = z;
    *(int *)(__MapActor_GetActor(0x12) + 0x2c) = z;
    *(int *)(__MapActor_GetActor(0x12) + 0x30) = z;
    *(int *)(__MapActor_GetActor(0x12) + 0x34) = z;
    { PIN3; q2 = 0; q1 = 0x103; q0 = 0x12; __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0x12; __Func_809259c(q0, q1); }
    __CutsceneWait(0x3c);
    { PIN3; q1 = 0xc0; q2 = 0xc0; q0 = 0x12; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0xc0; q0 = 0; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x8c; q0 = 0x12; q1 <<= 1; q2 = 0xe8;
      __Func_809218c(q0, q1, q2); }
    { PIN3; q1 = 0x94; q1 <<= 1; q2 = 0xe8; q0 = 0;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_WaitMovement(0x12);
    { PIN3; q1 = 0x80; q0 = 0; q1 <<= 8; q2 = 0x14;
      __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0x81; q2 = 0x3c; q0 = 0; q1 <<= 1;
      __MapActor_Emote(q0, q1, q2); }
    { PIN2; q1 = (int)gScript_928__020095b0; q0 = 0x12;
      __MapActor_SetBehavior(q0, q1); }
    *(void **)(__MapActor_GetActor(0x12) + 0x6c) = OvlFunc_928_2008500;
    __CutsceneEnd();
}

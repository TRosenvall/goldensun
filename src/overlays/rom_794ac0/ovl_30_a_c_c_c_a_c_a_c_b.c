// fakematch
/* OvlFunc_899_2009a4c  --  0x02009a4c
 *
 * Cut out of goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_c_c_a_c_a_c.s.
 *
 * 123 instructions. Two actors fetched up front and written at the very end,
 * twenty pinned call sites in between, one crossed __MapActor_SetSpeed taking a
 * single barrier.
 *
 * THE TAIL STORES POOL THEIR VALUES UNLESS NAMED, and this is the third
 * function this batch to want it. `*(short *)a = 1` emits `ldr r3, =0x1` --
 * a pooled word for a value that fits an 8-bit immediate three times over --
 * and also leaves a stray branch, so the function comes out 125 against 123.
 * Naming the value as its own statement AFTER the address advance gives the
 * ROM's `add r5, #0x64 / mov r3, #0x1 / strh r3, [r5]`.
 *
 * Order matters as much as naming: the ROM advances the pointer first and
 * builds the value second, and writing the two statements the other way round
 * puts the `mov` first. This is the same address-then-value ordering recorded
 * for the store pair in
 * src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c_c_b.c.
 *
 * `__StartTask` takes a function pointer, so its r0 pin is declared
 * `register void (*q0)(void)` rather than `register int` -- assigning a
 * function address to an int-typed pin compiles but loses the relocation.
 */
extern unsigned char gScript_899__0200d830[];
extern unsigned char gScript_899__0200d560[];
extern void OvlFunc_899_200aba0(void);
extern void OvlFunc_899_200c5f4(int a, int b);
extern void OvlFunc_899_200c60c(int a, int b, int c);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int n);
extern void __MapActor_SetBehavior(int slot, int s);
extern void __MapActor_WaitScript(int slot);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __StartTask(void (*f)(void), int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_899_2009a4c(void)
{
    unsigned char *a;
    unsigned char *b;
    int e0, e1, v;

    a = __MapActor_GetActor(0x18);
    b = __MapActor_GetActor(0x19);
    __CutsceneStart();
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0xae; q0 = 0; q1 = 0xe8; q2 <<= 2; __Func_80921c4(q0, q1, q2); }
    { PIN3; q2 = 0xae; q1 = 0xc8; q2 <<= 2; q0 = 0; __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0xa);
    { PIN3; q1 = 0x80; q0 = 0x19; q1 <<= 1; q2 = 0;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q1 <<= 1; q2 = 0; q0 = 0x18;
      __MapActor_Emote(q0, q1, q2); }
    __CutsceneWait(0x3c);
    { PIN3; q2 = 0xa; q0 = 0x19; q1 = 0; OvlFunc_899_200c60c(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0x18; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x14);
    __MessageID(0x1296);
    OvlFunc_899_200c5f4(0x18, 0x14);
    { PIN2; q1 = 0x81; q1 <<= 1; q0 = 0x19; __MapActor_Surprise(q0, q1); }
    __CutsceneWait(0x3c);
    OvlFunc_899_200c5f4(0x19, 0x14);
    __Func_80925cc(0x18, 1);
    OvlFunc_899_200c5f4(0x18, 0x1e);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x18; q1 <<= 11; q2 <<= 10;
      __MapActor_SetSpeed(q0, q1, q2); }
    {
        PIN3;
        q1 = 0xe0; __asm__ volatile ("" : : "r" (q1));
        q2 = 0xe0; q2 <<= 9; q0 = 0x19; q1 <<= 10;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    { PIN2; q1 = (int)gScript_899__0200d830; q0 = 0x19;
      __MapActor_SetBehavior(q0, q1); }
    { PIN2; q1 = (int)gScript_899__0200d560; q0 = 0x18;
      __MapActor_SetBehavior(q0, q1); }
    __MapActor_WaitScript(0x18);
    e0 = 0xe;
    e1 = 0x2c;
    __Func_8010704(0xe, 0x2d, 3, 1, e0, e1);
    __SetFlag(0x852);
    __SetFlag(0xc0 << 2);
    {
        register void (*q0)(void) __asm__("r0");
        register int q1 __asm__("r1");
        q1 = 0xc8; q1 <<= 4; q0 = OvlFunc_899_200aba0;
        __StartTask(q0, q1);
    }
    a += 0x64;
    v = 1;
    *(short *)a = v;
    b += 0x64;
    v = 3;
    *(short *)b = v;
    __CutsceneEnd();
}

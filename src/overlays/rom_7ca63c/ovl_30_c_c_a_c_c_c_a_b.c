/* Cluster OvlFunc_944_2008e78..OvlFunc_944_2008e78 extracted from
 * goldensun/asm/overlays/rom_7ca63c/ovl_30_c_c_a_c_c_c_a.s.
 *
 * Total .text for this TU = 205 encodings. Never attempted before batch 279.
 * TEN PIN3 SITES, ALL TEN REQUIRED -- one fakematch row. No flags.
 *
 * A `GlobalState` STRUCT WITH `short f1c4; short f1c6;` FIELDS IS WHAT KEEPS TWO ADJACENT HALFWORD
 * INDEX BUILDS INDEPENDENT, and this is the lever worth carrying. The function writes gState+0x1c4
 * then gState+0x1c6. With `extern unsigned char gState[]` and ANY spelling of the offsets -- plain
 * literals, named index locals, two base pointers, a `do{}while(0)`, a volatile memory barrier --
 * gcc derives the second index as `adds r0, #2` (18-20 differing). The struct-field spelling
 * reproduces the ROM's two INDEPENDENT builds, `mov #0xe2 / lsl #1 / add` and
 * `mov #0xe3 / lsl #1 / add`, and took this function from 18 to 2.
 *
 * Two more things fell out of that one block:
 *   * `(int)&_AREA_6f` must be a SYMBOL, because the ROM loads it with a WORD `ldr`. A literal
 *     produces an `ldrh` pool load -- the const.sym halfword exception -- and is 61 differing. So
 *     the halfword exception is also a DISCRIMINATOR: if the ROM's pool load is a word `ldr`, the
 *     exception does not apply and the value really is a symbol.
 *   * a struct-field assignment also gets `gState.f1c6 = 2;` to `mov r3, #2`, where
 *     `*(short *)(g + ...) = 2` pools the 2 as a HImode entry.
 *
 * The ten PIN3 sites are seven `__MapActor_SetSpeed` calls carrying 0xc0 << 10 / 0xc0 << 9, plus
 * `__MapActor_Emote`, one further `__MapActor_SetSpeed`, and `__Func_80921c4`. A greedy drop found
 * none of them removable. In all of them `q0` is assigned FIRST -- see the sibling
 * ovl_30_c_c_a_c_a.c for why that ordering is the lever rather than the pinning alone.
 *
 * objcmp reports one phantom encoding and one extra relocation for `_AREA_6f`, an absolute symbol
 * the linker fills; all 205 encodings otherwise identical, and `make compare` is the gate.
 *
 * The idiom source was found by grepping GENERATED `asm/` for `mov rN, #227` and reading back to
 * the `.c` -- src/overlays/rom_7c5974/ovl_30_c_c_c_c_a_c.c, which already carried the GlobalState
 * struct and the `_AREA_` usage. SEARCHING GENERATED ASSEMBLY FOR A ROM INSTRUCTION PATTERN, THEN
 * READING BACK TO ITS SOURCE, found in one step what a name-based search would not.
 */
extern unsigned char *iwram_3001ebc;
typedef struct {
    unsigned char pad00[0x1c2];
    short f1c2;
    short f1c4;
    short f1c6;
    unsigned char pad1c8[0x2c0 - 0x1c8];
} GlobalState;

extern GlobalState gState;
extern int _AREA_6f;
extern unsigned char gOvl_0200976c[];
extern unsigned char gScript_944__0200939c[];
extern unsigned char gScript_944__02009450[];
extern unsigned char gScript_944__02009480[];
extern unsigned char gScript_944__020094b0[];
extern unsigned char gScript_944__020094e0[];
extern unsigned char gScript_944__02009510[];
extern unsigned char gScript_944__02009540[];
extern unsigned char gScript_944__02009570[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __LoadFieldActors(unsigned char *p);
extern void __MapActor_SetBehavior(int slot, unsigned char *p);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8092950(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8091e9c(int a);
extern void OvlFunc_944_2008a84(int slot);
extern int OvlFunc_944_2009130(void);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_944_2008e78(void)
{
    __CutsceneStart();
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __LoadFieldActors(gOvl_0200976c);
    __WaitFrames(1);
    OvlFunc_944_2008a84(9);
    OvlFunc_944_2008a84(0xa);
    OvlFunc_944_2008a84(0xb);
    OvlFunc_944_2008a84(0xc);
    OvlFunc_944_2008a84(0xd);
    OvlFunc_944_2008a84(0xe);
    OvlFunc_944_2008a84(0xf);
    __MapActor_SetBehavior(8, gScript_944__0200939c);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x203;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0xc8 << 1);
    __MapActor_SetIdle(9);
    __MapActor_SetIdle(0xa);
    __MapActor_SetIdle(0xb);
    __MapActor_SetIdle(0xc);
    __MapActor_SetIdle(0xd);
    __MapActor_SetIdle(0xe);
    __MapActor_SetIdle(0xf);
    { PIN3; q0 = 9; q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xa; q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xb; q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xc; q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xd; q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xe; q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0xf; q1 = 0xc0 << 10; q2 = 0xc0 << 9; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetBehavior(9, gScript_944__02009450);
    __MapActor_SetBehavior(0xa, gScript_944__02009480);
    __MapActor_SetBehavior(0xb, gScript_944__020094b0);
    __MapActor_SetBehavior(0xc, gScript_944__020094e0);
    __MapActor_SetBehavior(0xd, gScript_944__02009510);
    __MapActor_SetBehavior(0xe, gScript_944__02009540);
    __MapActor_SetBehavior(0xf, gScript_944__02009570);
    __CutsceneWait(0x28);
    __Func_809259c(8, 3);
    __MapActor_Surprise(8, 0x81 << 1);
    __CutsceneWait(0x78);
    __Func_809259c(8, 1);
    { PIN3; q0 = 8; q1 = 0x80 << 1; q2 = 0x3c; __MapActor_Emote(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0x80 << 9; q2 = 0x80 << 8; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 8; q1 = 0xa4; q2 = 0xac << 1; __Func_80921c4(q0, q1, q2); }
    __MapActor_Jump(8, 4, 0xa);
    __MapActor_Jump(8, 6, 0x14);
    __MessageID(0x1ee4);
    __Func_8093040(8, 0, 0x14);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
    __MapTransitionOut();
    __WaitMapTransition();
    gState.f1c4 = (int)(&_AREA_6f);
    gState.f1c6 = 2;
    if (OvlFunc_944_2009130() == 0xb) {
        __Func_8091e9c(0xf);
    } else {
        __Func_8091e9c(0xe);
    }
    __CutsceneEnd();
}

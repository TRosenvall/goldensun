// fakematch
/* OvlFunc_924_200a684  --  0x0200a684
 *
 * Cut out of goldensun/asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_a.s.
 *
 * 152 instructions, exact on the first screen. Twenty pinned call sites, one
 * crossed __Func_80921c4 fill taking a single barrier after `q1 = 0xd2`, and
 * two __MapActor_SetSpeed calls sharing the same pair of pooled constants which
 * the ROM reloads at each -- the pins force that, since a call-clobbered
 * register cannot carry a value across a `bl`.
 *
 * TWO SHAPES WORTH A LINE, both already on file and both easy to "tidy" wrongly:
 *
 *   THE INCREMENT IS DUPLICATED IN BOTH ARMS. `*(unsigned short *)(p + 0x1d8)
 *   += 1` appears at the END of the true arm and at the START of the false one.
 *   Hoisting it to a single statement after the `if` would be the obvious
 *   simplification and would not match; the ROM emits both copies. Same class as
 *   the duplicated call in src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_a_b.c.
 *
 *   THE ACTOR FETCHES ARE INLINE. Both `*(int *)(GetActor(3) + 0x18) = ...`
 *   stores subscript the call result directly rather than through a named
 *   local, so the address dies inside the statement and stays in r0 -- the lever
 *   from src/overlays/rom_799abc/ovl_30_c_c_c_c_b.c. The second store's value is
 *   `0x80 << 9` written inline and it comes out as `mov r3 / lsl r3` unaided,
 *   because r3 is call-clobbered and the build has to follow the call anyway.
 *   That is the one place where the usual "split a shifted constant into
 *   statements" cure is unnecessary: the call already forces the ordering.
 */
extern unsigned char *iwram_3001ebc;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_924_200a684(void)
{
    __CutsceneStart();
    { PIN3; q0 = 3; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xcccc; q2 = 0x6666; q0 = 0; __MapActor_SetSpeed(q0, q1, q2); }
    __MessageID(0x1577);
    __Func_8093040(3, 0, 0x14);
    { PIN3; q1 = 0xd2; q2 = 0xa2; q0 = 3; q1 <<= 2; q2 <<= 2;
      __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 3; q1 <<= 1; q2 = 0x3c;
      __MapActor_Emote(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x14; q0 = 3; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0x10; q0 = 3; __MapActor_SetAnim(q0, q1); }
    *(int *)(__MapActor_GetActor(3) + 0x18) = 0xffff0000;
    __CutsceneWait(0x14);
    { PIN3; q2 = 0x14; q0 = 3; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 1; q0 = 3; __MapActor_SetAnim(q0, q1); }
    *(int *)(__MapActor_GetActor(3) + 0x18) = 0x80 << 9;
    __CutsceneWait(0x14);
    { PIN3; q1 = 0x80; q0 = 3; q1 <<= 7; q2 = 0x14; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 3; __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        __MapActor_DoAnim(3, 3);
        { PIN3; q2 = 0x14; q0 = 3; q1 = 0; __Func_8093040(q0, q1, q2); }
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
    } else {
        *(unsigned short *)(iwram_3001ebc + (0xec << 1)) += 1;
        __CutsceneWait(0x14);
        __MapActor_DoAnim(3, 4);
        __Func_8093040(3, 0, 0x14);
    }
    __CutsceneWait(0x14);
    { PIN3; q1 = 0xc0; q2 = 0x14; q0 = 3; q1 <<= 8; __Func_8092adc(q0, q1, q2); }
    __Func_80933d4(0xcccc, 0x1999);
    { PIN4; q0 = 0xd2; q1 = 1; q2 = 0x9e; q3 = 1; q0 <<= 18; q1 = -q1; q2 <<= 18;
      __Func_80933f8(q0, q1, q2, q3); }
    { PIN3;
      q1 = 0xd2; __asm__ volatile ("" : : "r" (q1));
      q2 = 0x9e; q2 <<= 2; q1 <<= 2; q0 = 3;
      __Func_80921c4(q0, q1, q2); }
    __Func_8093530();
    __CutsceneWait(0x14);
    { PIN2; q1 = 2; q0 = 3; __Func_80925cc(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 4; q0 = 3; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { PIN3; q1 = 0; q2 = 0x14; q0 = 3; __Func_8093040(q0, q1, q2); }
    __SetFlag(0x87 << 4);
    __CutsceneEnd();
}

// fakematch
/* OvlFunc_948_2008ccc  --  0x02008ccc
 *
 * Cut out of goldensun/asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_a_c.s.
 *
 * A one-shot cutscene behind two flags: walks actor 0xf along a zig-zag of ten
 * waypoints, then hands it a follow-up behaviour and ends.
 *
 * TWO LEVERS, and the second is a NEGATIVE that matters more than the first.
 *
 * 1. THE FLAG ID IS REMATERIALISED. The ROM issues `ldr r0, =0x9ca` fresh at
 *    __GetFlag and again at __SetFlag. gcc caches it in r6 -- `ldr r6, =0x9ca /
 *    mov r0, r6` and then `mov r0, r6` -- which also widens the prologue to
 *    `push {r5, r6, lr}`. Assigning it to the r0 pin before each call forces
 *    the rebuild, because r0 is call-clobbered and the value cannot survive the
 *    `bl`. That single change took the candidate from 144 differing to TWO.
 *
 * 2. A PIN CAN BE ACTIVELY HARMFUL, and this file is the demonstration. After
 *    the flag fix, the only remaining disagreement was one transposed pair:
 *
 *        rom   mov r1, #0xdc / mov r2, #0xaa
 *        ours  mov r2, #0xaa / mov r1, #0xdc
 *
 *    Every attempt to STEER that pair failed, all byte-identical at 2 of 150:
 *    writing the two assignments in the opposite source order; a
 *    `do { } while (0)` scheduling barrier between them; moving the
 *    interleaved store between them; a data dependence (`p2 = p1 - 0x32`),
 *    which gcc folds straight back to the constant and loses the ordering with
 *    it -- the outcome src/non_matching/ovl_793768/2008e0c.c predicted for
 *    exactly this trick; and two plain `int` locals instead of the pins.
 *
 *    What matched was REMOVING THE PINS AT THAT ONE CALL and writing it as an
 *    ordinary `__Func_80921c4(0xf, 0xdc << 2, 0xaa << 2)`. gcc's natural order
 *    there is already the ROM's, and pinning the two argument registers is
 *    what broke it.
 *
 * SO PINS ARE NOT FREE. The batch-193 and 194 entries establish where a pin
 * REACHES something; this is the first measured case of one COSTING something.
 * The rule the notebook already has -- that a pin does not order two
 * independent movs -- understates it: at a site where the scheduler was
 * already going to produce the ROM's order, introducing the pins changes the
 * order to the wrong one. Try the site unpinned before assuming a residue
 * there needs a lever.
 *
 * TEARDOWN, and its limit stated honestly. Stripping the pins from EVERY call
 * site is much worse -- 153 lines against 150, 146 differing -- so they are
 * load-bearing at the other sites and only this one wanted them gone. That is
 * a coarse teardown, all-on against all-off plus the one site isolated by the
 * residue; the remaining sites were not each removed individually, which would
 * have cost a compile apiece.
 */

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetAnimSpeed(unsigned char *a, int n);
extern void __PlaySound(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int n);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void OvlFunc_948_2008aa8(void);

void OvlFunc_948_2008ccc(void)
{
    unsigned char *a;
    unsigned char *s;
    int f;
    int k;
    register int p0 __asm__("r0");
    register int p1 __asm__("r1");
    register int p2 __asm__("r2");

    if (__GetFlag(0x9c9) == 0)
        return;
    p0 = 0x9ca;
    f = __GetFlag(p0);
    if (f != 0)
        return;
    p0 = 0x9ca;
    __SetFlag(p0);
    __CutsceneStart();
    a = __MapActor_GetActor(0xf);
    s = *(unsigned char **)(a + 0x50);
    p1 = 0x10;
    *(short *)(s + 0x1e) = f;
    __Actor_SetAnimSpeed(a, p1);
    __PlaySound(0x98);
    a = __MapActor_GetActor(0xf);
    k = 0x80;
    k <<= 12;
    p1 = 0x80;
    *(int *)(a + 0x28) = k;
    p2 = 0x1e;
    p0 = 0xf;
    p1 <<= 8;
    __Func_8092adc(p0, p1, p2);
    p1 = 0x81; p0 = 0xf; p1 <<= 1;
    __MapActor_Surprise(p0, p1);
    p1 = 2; p0 = 0xf;
    __Func_80925cc(p0, p1);
    __CutsceneWait(0x14);
    p1 = 0x80; p2 = 0x80; p1 <<= 9; p2 <<= 8; p0 = 0xf;
    __MapActor_SetSpeed(p0, p1, p2);
    __PlaySound(0x98);
    a = __MapActor_GetActor(0xf);
    k = 0x80;
    k <<= 11;
    *(int *)(a + 0x28) = k;
    __Func_80921c4(0xf, 0xdc << 2, 0xaa << 2);
    __CutsceneWait(0xa);
    p0 = 0xf; p1 = 0x101;
    __MapActor_Surprise(p0, p1);
    p1 = 0x80; p2 = 0x80; p0 = 0xf; p1 <<= 10; p2 <<= 9;
    __MapActor_SetSpeed(p0, p1, p2);
    p1 = 0xdc; p2 = 0xae; p0 = 0xf; p1 <<= 2; p2 <<= 2;
    __Func_80921c4(p0, p1, p2);
    p2 = 0xb0; p0 = 0xf; p1 = 0x372; p2 <<= 2;
    __Func_80921c4(p0, p1, p2);
    p1 = 0xdc; p2 = 0xb2; p0 = 0xf; p1 <<= 2; p2 <<= 2;
    __Func_80921c4(p0, p1, p2);
    p2 = 0xb4; p0 = 0xf; p1 = 0x36e; p2 <<= 2;
    __Func_80921c4(p0, p1, p2);
    p1 = 0xdc; p2 = 0xb6; p0 = 0xf; p1 <<= 2; p2 <<= 2;
    __Func_80921c4(p0, p1, p2);
    p2 = 0xb8; p0 = 0xf; p1 = 0x372; p2 <<= 2;
    __Func_80921c4(p0, p1, p2);
    p1 = 0xdc; p2 = 0xba; p0 = 0xf; p1 <<= 2; p2 <<= 2;
    __Func_80921c4(p0, p1, p2);
    p2 = 0xbc; p0 = 0xf; p1 = 0x36e; p2 <<= 2;
    __Func_80921c4(p0, p1, p2);
    p1 = 0xdc; p2 = 0xbe; p0 = 0xf; p1 <<= 2; p2 <<= 2;
    __Func_80921c4(p0, p1, p2);
    p1 = 0xd6; p2 = 0xce; p1 <<= 18; p2 <<= 18; p0 = 0xf;
    __MapActor_SetPos(p0, p1, p2);
    __CutsceneWait(0xa);
    p1 = 0xc0; p2 = 0x14; p0 = 0xf; p1 <<= 8;
    __Func_8092adc(p0, p1, p2);
    p1 = 0x80; p0 = 0xf; p1 <<= 1;
    __MapActor_Surprise(p0, p1);
    a = __MapActor_GetActor(0xf);
    *(void **)(a + 0x6c) = (void *)OvlFunc_948_2008aa8;
    __CutsceneEnd();
}

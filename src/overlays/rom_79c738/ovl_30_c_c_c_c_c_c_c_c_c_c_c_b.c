// fakematch
/* OvlFunc_909_200a1bc  --  0x0200a1bc
 * [asm/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_c_c.s, second of two]
 *
 * 218 instructions. Byte-exact: 588 bytes, 228 encodings and 54 relocations
 * identical.
 *
 * THE POOLED ZERO IS A BARE LITERAL, AND THE POOL ORDER IS THE PROOF. This is
 * the counterweight to _CONST_a1, added to const.sym for OvlFunc_959_2009e94
 * one batch ago on the strength of the same surface tell, so the two should be
 * read together.
 *
 * The ROM's four `p[0x55] = 0` stores read `ldr r5, [pc, #60]` off a
 * MID-FUNCTION pool whose words are [0, 0x2410000, 0x2960000, 0xfffc0000].
 * The zero sorts FIRST -- ahead of 0x2410000, whose own `ldr` is forty-five
 * instructions EARLIER. add_minipool_forward_ref keeps the pool sorted by
 * max_address, so an entry can only sort ahead of an earlier reference when its
 * OWN reference is NARROW: gcc routes the QImode zero through an HImode temp
 * with a 64-byte pool range, and the ROM's displacement is 60. That same
 * narrowness is what dumps the pool mid-body with the ROM's `b` over it and
 * splits the remaining seven words into a second pool after the epilogue.
 *
 * `(int)&_CONST_0` reproduces the REGISTER and the PLACEMENT and still fails:
 * it sorts the zero THIRD and puts all eleven words at the end -- 216 lines
 * against 218, four bytes short. So the pool's ORDER, not just its contents,
 * discriminates literal from symbol here, and it does so where the register
 * allocation alone would have said "symbol". No const.sym entry belongs to this
 * function.
 *
 * THE 0x9000 HALFWORD MUST BE BORN AFTER ITS __MapActor_GetActor, and that was
 * worth more than every pin in the file. Written
 * `*(short *)(__MapActor_GetActor(0x11) + 6) = w;`, gcc evaluates w first and
 * hands it r5; the pooled zero -- whose definition sits at the FIRST strh,
 * because expand leaves a dead `(set (reg:HI) (const_int 0))` there that CSE
 * resurrects for the byte stores -- is then displaced into r8, giving a
 * `mov r6, r8 / push {r6}` prologue, a `mov r3, r8` at all four stores, and the
 * pool dragged 168 bytes forward. Naming the actor pointer and assigning w
 * after the call leaves r5 free: 227 lines and 211 differing become 218 and
 * exact.
 *
 * `int z = 0;` for the first byte store and the two halfword stores is the
 * ROM's `mov r6, #0`, held callee-saved across sixteen calls; as bare literals
 * it collapses into the pooled zero, 97 differing.
 *
 * Twelve pins from twenty-one candidates at a greedy fixpoint, two full passes.
 * Notably removable: a __MapActor_SetPos whose `lsl r2 / lsl r1` is CROSSED and
 * which gcc gets right unaided, and both __Func_8010704 calls.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern unsigned char *__Func_8093554(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern unsigned int iwram_3001ebc;

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_909_200a1bc(void)
{
    int z;

    __CutsceneStart();
    { PIN4; q0 = -1; q1 = -1; q2 = -1; q3 = 0; __Func_80933f8(q0, q1, q2, q3); }
    z = 0;
    __Func_8093554()[0x55] = z;
    __Func_80933f8(0x9d << 18, -1, 0xbb << 18, 0);
    __Func_8010704(0x26, 0x37, 4, 1, 0x26, 0x2d);
    __Func_8010704(0x2a, 0x37, 4, 1, 0x26, 0x2e);
    *(short *)(__MapActor_GetActor(0) + 6) = z;
    { PIN3; q0 = 0; q1 = 0x2410000; q2 = 0xbe << 18;
      __MapActor_SetPos(q0, q1, q2); }
    *(short *)(__MapActor_GetActor(0x13) + 6) = z;
    __MapActor_SetPos(0x13, 0x94 << 18, 0xbe << 18);
    {
        unsigned char *a;
        int w;

        a = __MapActor_GetActor(0x11);
        w = 0x90 << 8;
        *(short *)(a + 6) = w;
    }
    { PIN3; q0 = 0x11; q1 = 0x2960000; q2 = 0xbf << 18;
      __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x15; q1 = 0x9a << 18; q2 = 0xb6 << 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x16; q1 = 0x9e << 18; q2 = 0xb6 << 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x17; q1 = 0xa2 << 18; q2 = 0xb6 << 18; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetPos(0x18, 0xa6 << 18, 0xb6 << 18);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x15), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x16), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x17), 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 0);
    __MapActor_GetActor(0x15)[0x55] = 0;
    __MapActor_GetActor(0x16)[0x55] = 0;
    __MapActor_GetActor(0x17)[0x55] = 0;
    __MapActor_GetActor(0x18)[0x55] = 0;
    *(int *)(__MapActor_GetActor(0x15) + 0xc) = 0xfffc0000;
    *(int *)(__MapActor_GetActor(0x16) + 0xc) = 0xfffc0000;
    *(int *)(__MapActor_GetActor(0x17) + 0xc) = 0xfffc0000;
    *(int *)(__MapActor_GetActor(0x18) + 0xc) = 0xfffc0000;
    __Func_800fe9c();
    __WaitFrames(1);
    {
        unsigned char *base = (unsigned char *)iwram_3001ebc;

        *(int *)(base + 0x1c0) = 0x201;
        *(int *)(base + 0x1c8) = 0x10;
    }
    __MapTransitionIn();
    __WaitMapTransition();
    { PIN3; q0 = 0x13; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x9999; q2 = 0x4ccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0x9d << 2; q2 = 0xbf << 2; __Func_809218c(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0x99 << 2; q2 = 0xbf << 2; __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(0x13, 1);
    __CutsceneWait(0x14);
    __Func_80925cc(0x13, 1);
    __MessageID(0x1746);
    __Func_8093040(0x13, 0, 0xa);
    { PIN3; q0 = 0x13; q1 = 0x26e; q2 = 0xc3 << 2; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0x13; q1 = 0xc0 << 8; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    __Func_80925cc(0x11, 2);
    __Func_8093040(0x11, 0, 0xa);
    __MapActor_DoAnim(0, 3);
    __ClearFlag(0x12f);
    __SetFlag(0x202);
    __CutsceneEnd();
}

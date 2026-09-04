// fakematch
/* OvlFunc_882_2009348  --  0x02009348
 *
 * Cut out of goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_c_a_a.s.
 *
 * A one-shot cutscene behind a save flag, with a second guarded block that
 * branches on the player's world position.
 *
 * THE WHOLE FUNCTION IS ONE LEVER APPLIED SIX TIMES: THREE FLAG IDS ARE EACH
 * USED TWICE AND THE ROM REBUILDS EVERY ONE. 0x313 is tested then set, 0x833 is
 * tested then set, and 0xc3 << 2 is tested then set. Left as plain literals gcc
 * caches all three in callee-saved registers and the prologue goes from
 * `push {r5, lr}` to `push {r5, r6, r7, lr}` -- 119 of 119 differing, which is
 * to say the entire body shifted. Assigning each to an r0 pin immediately
 * before its call forces the rebuild, because r0 is call-clobbered and the
 * value cannot survive the `bl`. 119 to 3, with the length becoming exact.
 *
 * THE PROLOGUE WIDTH IS THE TELL AND IT IS GETTING RELIABLE. Two extra pushed
 * registers against the ROM's zero said, before any instruction was compared,
 * that two values were being kept which the ROM rebuilds. This is the third
 * function in two batches where the push list alone identified the lever; see
 * also src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_c_c_b.c (one extra) and
 * src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_c_b.c (one TOO FEW, which
 * meant the opposite -- a value that needed naming).
 *
 * The `&&` chain had to become nested `if`s, because each test needs its own
 * pin assignment as a statement ahead of it and there is nowhere to put one
 * inside a `&&`. gcc emits the same code either way; this is a spelling forced
 * by the lever, not by the ROM.
 *
 * THE LAST RESIDUE was __Func_80921c4(0, 0x1bf, 0x4cb) in the else arm, left
 * plain because the ROM fills r0, r1, r2 in ascending order and that usually
 * needs no help. It does when two of the three arguments are POOL LOADS: gcc
 * issues those first and the cheap `mov r0, #0` last. Anchor any call with a
 * pool load in it regardless of the register order -- the same thing was true
 * of the final call in ovl_30_c_c_c_c_a_a_c_c_b.c this same round. Note the
 * sibling call OvlFunc_882_2009a64(0x1bd, 0x494), whose arguments are BOTH pool
 * loads, needs nothing: with no cheap immediate in the list there is nothing to
 * get out of order.
 *
 * The actor is fetched once into a named local because it is written at five
 * offsets across the block; the two shifted constants stored to it are built
 * with `v = 0x80; v <<= 9;` rather than inline, which is the documented cure for
 * a shifted constant reaching the pool.
 */
extern void OvlFunc_882_2009498(void);
extern void OvlFunc_882_2009a64(int a, int b);

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_809202c(void);
extern void __Func_80921c4(int a, int b, int c);

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_882_2009348(void)
{
    unsigned char *a;
    register int p0 __asm__("r0");
    int v, w, t, u;

    p0 = 0x313;
    if (__GetFlag(p0) == 0) {
        __CutsceneStart();
        p0 = 0x833;
        if (__GetFlag(p0) == 0) {
            a = __MapActor_GetActor(0xe);
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x80; q1 <<= 11; q2 <<= 9; q0 <<= 11;
              __Func_8012330(q0, q1, q2); }
            __PlaySound(0x8d);
            __WaitFrames(0x28);
            __PlaySound(0x91);
            { PIN3; q1 = 0xed; q0 = 0xe; q1 <<= 17; q2 = 0x47b0000;
              __MapActor_SetPos(q0, q1, q2); }
            v = 0x80;
            v <<= 9;
            *(int *)(a + 0x30) = v;
            *(int *)(a + 0x34) = v;
            w = 0x90;
            t = *(int *)(a + 0xc);
            w <<= 15;
            t += w;
            *(int *)(a + 0xc) = t;
            *(int *)(a + 0x3c) = t;
            u = 0x80;
            u <<= 8;
            *(int *)(a + 0x44) = u;
            { PIN3; q1 = 0xd8; q1 <<= 1; q2 = 0x47b; q0 = 0xe;
              __Func_80921c4(q0, q1, q2); }
            __CutsceneWait(0x28);
            __PlaySound(0x121);
            { PIN3; q0 = 1; q1 = 1; q0 = -q0; q1 = -q1; q2 = 0xe666;
              __Func_8012330(q0, q1, q2); }
            __Func_8012350();
            __Func_809202c();
            p0 = 0x833;
            __SetFlag(p0);
        }
        OvlFunc_882_2009498();
        p0 = 0x313;
        __SetFlag(p0);
        p0 = 0x837;
        if (__GetFlag(p0) != 0) {
            p0 = 0x841;
            if (__GetFlag(p0) == 0) {
                p0 = 0xc3; p0 <<= 2;
                if (__GetFlag(p0) == 0) {
            if (*(int *)(__MapActor_GetActor(0) + 0x10) <= 0x479ffff) {
                {
                    register int q0 __asm__("r0");
                    register int q1 __asm__("r1");
                    q0 = 0xce; q1 = 0x8c; q0 <<= 1; q1 <<= 3;
                    OvlFunc_882_2009a64(q0, q1);
                }
                { PIN3; q1 = 0xcf; q0 = 0; q1 <<= 1; q2 = 0x42c;
                  __Func_80921c4(q0, q1, q2); }
            } else {
                OvlFunc_882_2009a64(0x1bd, 0x494);
                { PIN3; q0 = 0; q1 = 0x1bf; q2 = 0x4cb; __Func_80921c4(q0, q1, q2); }
            }
                p0 = 0xc3; p0 <<= 2;
                __SetFlag(p0);
                }
            }
        }
        __CutsceneEnd();
    }
}

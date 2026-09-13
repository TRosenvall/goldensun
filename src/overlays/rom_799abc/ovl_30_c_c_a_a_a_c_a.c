/* asm/overlays/rom_799abc/ovl_30_c_c_a_a_a_c_a.s -- BOTH functions.
 *
 * OvlFunc_905_2008bd0  -- the sparkle-burst task StartTask'd by the cutscene
 *                         below; fires a particle on every 8th and every 16th
 *                         global tick.
 * OvlFunc_905_2008ce0  -- the cutscene itself.
 *
 * WHOLE-FILE conversion: the .s holds exactly these two .thumb_func_start
 * blocks and no .data/.rodata tail, so nothing is split and the overlay .ld
 * line keeps its asm/ prefix verbatim.
 */
#include "actor.h"

extern int iwram_3001e40;

extern struct Actor *__MapActor_GetActor(int slot);
extern unsigned int __Random(void);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int frames);
extern void __CutsceneEnd(void);
extern void __Actor_SetSpriteFlags(struct Actor *a, int flags);
extern void __Func_8092950(int slot, int a);
extern void __Func_800c548(struct Actor *a, int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __StartTask(void (*fn)(void), int prio);
extern void __StopTask(void (*fn)(void));
extern void __PlaySound(int id);
extern void __Func_809228c(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_905_2008a68(int a, int b, int c, int d, int e, int f, int g);

/* n * 6553 written out as the shift-and-add ladder the ROM uses: this gcc
 * emits `ldr r3, =6553 / mul` for the multiply, so the ladder has to be in
 * the source.  Every step is a COMPOUND assignment on ONE variable, which is
 * what produces the ROM's destructive two-operand `add rD, rS` forms; writing
 * a step as `t = t * 4 + n` makes gcc allocate a fresh pseudo per step and the
 * accumulator ping-pongs between two registers. */
#define SCALE_6553(n, t, d) \
    ((n) >>= 16, (t) = (n) << 1, (t) += (n), (t) <<= 2, (t) += (n), \
     (d) = (t) << 6, (d) -= (t), (d) <<= 3, (d) += (n))

void OvlFunc_905_2008bd0(void)
{
    int z;
    unsigned int n1, t1, d1;
    unsigned int n2, t2, d2;

    z = iwram_3001e40 & 7;
    if (z == 0) {
        OvlFunc_905_2008a68(
            __MapActor_GetActor(9)->pos.x + ((__Random() * 12 >> 16) << 16),
            __MapActor_GetActor(9)->pos.y,
            __MapActor_GetActor(9)->pos.z + (0xc0 << 11),
            0,
            -(int)((n1 = __Random() * 5, SCALE_6553(n1, t1, d1))),
            __Random() * 2 >> 16,
            z);
        z = iwram_3001e40 & 0xf;
        if (z == 0) {
            OvlFunc_905_2008a68(
                __MapActor_GetActor(9)->pos.x + ((__Random() * 12 >> 16) << 16),
                __MapActor_GetActor(9)->pos.y,
                __MapActor_GetActor(9)->pos.z + (0xc0 << 11),
                0,
                -(int)((n2 = __Random() * 5, SCALE_6553(n2, t2, d2))),
                __Random() * 2 >> 16,
                z);
        }
    }
}

void OvlFunc_905_2008ce0(void)
{
    int x;
    int c1, c2, c3;
    int m1, m2, m3;
    int e, f;
    unsigned int n, t, d;

    x = __MapActor_GetActor(9)->pos.x;
    if (x < 0)
        x += 0xfffff;
    x >>= 20;
    /* Each repeated constant needs its OWN local assigned in a block that
     * DOMINATES the use, or cse commons the pair and gcc copies one register
     * into the other where the ROM builds both. */
    c1 = 0xa0 << 11;
    c2 = 0xa0 << 11;
    m1 = -1;
    m2 = -1;
    c3 = 0x80 << 9;
    m3 = 0xe666;
    __CutsceneStart();
    if (x == 0x19) {
        __MapActor_GetActor(0xb)->layer = 1;
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 0);
        __Func_8092950(0xb, 0xe);
        __Func_800c548(__MapActor_GetActor(0xb), 1);
        __MapActor_SetPos(0xb, 0xcf << 17, 0xf0 << 16);
        __CutsceneWait(0xa);
        __StartTask(OvlFunc_905_2008bd0, 0xc8 << 4);
        __PlaySound(0x8d);
        __Func_809228c(9, 1, 0);
        __MapActor_WaitMovement(9);
        __CutsceneWait(0xa);
        __Func_809228c(9, 2, 0);
        __MapActor_WaitMovement(9);
        __MapActor_GetActor(9)->velX = 0;
        __MapActor_GetActor(9)->velY = 0x9999;
        __CutsceneWait(3);
        __MapActor_SetSpeed(9, 0xa0 << 10, 0x80 << 7);
        __PlaySound(0x90 << 1);
        __MapActor_TravelTo(9, 0xd0 << 1, 0xc8);
        __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
        __StopTask(OvlFunc_905_2008bd0);
        __CutsceneWait(0xc);
        __PlaySound(0xbd);
        OvlFunc_905_2008a68(
            __MapActor_GetActor(9)->pos.x + ((__Random() * 12 >> 16) << 16),
            __MapActor_GetActor(9)->pos.y,
            __MapActor_GetActor(9)->pos.z + (0xc0 << 11),
            0,
            -(int)((n = __Random() * 5, SCALE_6553(n, t, d))),
            __Random() * 2 >> 16,
            0);
        __CutsceneWait(0x14);
        __PlaySound(0x9a);
        __Func_8012330(c1, c2, c3);
        __Func_8012330(m1, m2, m3);
        __Func_8012350();
        __MapActor_SetPos(9, 0, 0);
        __MapActor_SetPos(0xb, 0, 0);
        __SetFlag(0xc0 << 2);
        /* Two stack arguments: name BOTH adjacent to the call, in the order
         * the ROM stores them, and reuse the shared one for the register
         * argument -- otherwise gcc builds them into one register in turn. */
        e = 0x15;
        f = 0xb;
        __Func_8010704(e, 0x2d, 4, 2, e, f);
    }
    __CutsceneEnd();
}

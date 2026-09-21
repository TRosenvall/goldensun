/* Cluster OvlFunc_909_20086e0..OvlFunc_909_20086e0 extracted from
 * goldensun/asm/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_a_a.s.
 *
 * Total .text for this TU = 480 bytes (= 0x1e0). Never attempted before batch 278.
 * THREE PINS (q0/q1/q2 on the four __MapActor_SetPos sites) -- one fakematch row.
 * No flags. `int` returning `return 0;`, which the `pop {r1} / bx r1` epilogue requires.
 *
 * THE PINS ARE UNAVOIDABLE HERE, AND THE MECHANISM IS EXACT. The repeated `0xb6 << 18` goes
 * through precompute_register_parameters and then cse1: `.00.rtl` has FOUR independent
 * `(set (reg N) (const_int 47710208))`, `.03.cse` has rewritten regs 76/79/82 to reg 73, and the
 * copies are `(set (reg:SI 2 r2) (reg:SI 73))`. `.17.lreg` then reads
 * `Register 73 used 5 times across 30 insns; crosses 3 calls` -- priority 2*5/30 = 0.33,
 * beating the branch-proven zero at 2*6/95 = 0.126, so it wins r6 and adds a push.
 *
 * NO CSE FLAG DISABLES cse1, and separate named locals do not survive it. Only a call-clobbered
 * pin does. This is the recorded CSE-shared-expensive-constants class, and this function is the
 * case where it IS beatable -- by pinning rather than by spelling.
 *
 * AND THE ASSIGNMENT ORDER IS THE SECOND HALF OF THE LEVER: q0, q1, q2 in ASCENDING order is
 * exact, where (q1,q2) is 6 and (q1,q2,q0) is 6. The ascending order is also what buys the ROM's
 * `mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2` interleave -- the same interleave the
 * three-named-locals lever produces when a dominating branch lets it.
 *
 * THE BATCH-182 "BOTH SPLIT BUILDS NAMED" LEVER BACKFIRES HERE: 59 differing. Four sites is not
 * six, and naming all eight locals blew everything into r8-r11. Worth recording as a measured
 * negative against that entry.
 *
 * Measured: all bare with the arms in ROM order 24; arms swapped with the four `y` builds grouped
 * 21; `y` assigned immediately before each call 17; r0 pin only 11; r0 pin with the shift as a
 * variable 11; r0+r2 with q2 first 17; three r2-bearing permutations 25; (q1,q2) 6; (q1,q2,q0) 6;
 * ASCENDING q0,q1,q2 EXACT. Eight named locals 59; `x` named with `y` inline 53.
 * Flags: -fno-gcse 23; -fno-cse-follow-jumps, -fno-cse-skip-blocks, -fno-expensive-optimizations
 * and -fno-caller-saves all inert at 11; -O1 49.
 *
 * AND ONE COMPILER BUG WORTH KNOWING ABOUT: `-fno-rerun-cse-after-loop` on this function is an
 * INTERNAL COMPILER ERROR -- "in decode_rtx_const, at varasm.c:3421". That flag is a member of
 * the CSE_CFLAGS group several files use, so it can ICE rather than merely measure badly.
 */
extern unsigned char gState[];
extern unsigned char iwram_3001ebc[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_80118c0(int a);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetPos(int slot, int x, int y);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *p, int f);
extern void OvlFunc_909_20088c0(void);
extern void OvlFunc_909_200979c(void);
extern void OvlFunc_909_20099b0(void);
extern void OvlFunc_909_200a1bc(void);

int OvlFunc_909_20086e0(void)
{
    unsigned char *base;
    unsigned char *p;
    int off;
    int s1, s2;
    int v;

    base = *(unsigned char **)iwram_3001ebc;
    *(int *)(base + (0xe0 << 1)) = 0x209;
    __Func_80118c0(1);
    __Func_80118c0(2);
    __SetFlag(0x84b);
    if (__GetFlag(0x109))
        __ClearFlag(0x80 << 2);
    off = 0xe1 << 1;
    if (__GetFlag(0x84f) == 0 && __GetFlag(0x845) == 0) {
        v = *(short *)(gState + off);
        if (v == 0x1d) {
            OvlFunc_909_20088c0();
        } else if (v == 9) {
            if (__GetFlag(0x321))
                OvlFunc_909_200979c();
        }
    } else if (__GetFlag(0x84e) == 0) {
        v = *(short *)(gState + off);
        if (v == 0x1d) {
            if (__GetFlag(0x85e) == 0 && __GetFlag(0x845))
                OvlFunc_909_20099b0();
        } else if (v == 0x1c) {
            if (__GetFlag(0x322)) {
                if (__GetFlag(0x109)) {
                    s1 = 0x26;
                    s2 = 0x2d;
                    __Func_8010704(0x26, 0x37, 4, 1, s1, s2);
                    s2 = 0x2e;
                    __Func_8010704(0x2a, 0x37, 4, 1, s1, s2);
                    { register int q0 __asm__("r0"); register int q1 __asm__("r1"); register int q2 __asm__("r2"); q0 = 0x15; q1 = 0x9a << 18; q2 = 0xb6 << 18; __MapActor_SetPos(q0, q1, q2); }
                    { register int q0 __asm__("r0"); register int q1 __asm__("r1"); register int q2 __asm__("r2"); q0 = 0x16; q1 = 0x9e << 18; q2 = 0xb6 << 18; __MapActor_SetPos(q0, q1, q2); }
                    { register int q0 __asm__("r0"); register int q1 __asm__("r1"); register int q2 __asm__("r2"); q0 = 0x17; q1 = 0xa2 << 18; q2 = 0xb6 << 18; __MapActor_SetPos(q0, q1, q2); }
                    { register int q0 __asm__("r0"); register int q1 __asm__("r1"); register int q2 __asm__("r2"); q0 = 0x18; q1 = 0xa6 << 18; q2 = 0xb6 << 18; __MapActor_SetPos(q0, q1, q2); }
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
                } else {
                    OvlFunc_909_200a1bc();
                }
            }
        }
    }
    return 0;
}

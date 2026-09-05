// fakematch
/* OvlFunc_948_200a334  --  0x0200a334
 *
 * Cut out of goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c.s.
 *
 * A map-entry cutscene: clear the busy byte on actors 0xe, 0xb and 0xc, start
 * two background tasks, run the area's setup routines, then -- unless save bit
 * 0x109 is already set -- place actor 0xf at one of four positions chosen by
 * bits 0x9ca / 0x9c9 / 0x9c8.
 *
 * FAKEMATCH.  This is the duplicate-constant CSE class, and the ROM's PROLOGUE
 * picks the cure: `push {r5, lr}` is one callee-saved register, and the ROM
 * spends it on the 0x9ca flag result, not on a constant.  So every repeated
 * constant is rebuilt at every use and only PINS work.
 *
 *   plain C, named `p` at the three byte stores    117 lines, 114 differing
 *   store through the call's return value directly 114 lines,  27 differing
 *   + pin r1 at both __StartTask sites             114 lines,  11 differing
 *   + pinned r0/r1 fills at all four SetPos sites  114 lines,   0
 *
 * THE BYTE STORE MUST NOT GO THROUGH A NAMED LOCAL.  `p = GetActor(n);
 * p[0x55] = 0;` gives the address its own pseudo, and thumb's `add reg, imm8`
 * is destructive, so each of the three sites costs an extra `mov r3, r0` before
 * the `add` -- the whole three-line length difference.  Writing
 * `__MapActor_GetActor(n)[0x55] = 0;` lets the add happen in r0 in place.
 *
 * 0xc80 IS THE COMMONED CONSTANT.  `mov r1, #0xc8 / lsl r1, #4` is two
 * instructions used at both __StartTask calls, so cse_main hoists it into a
 * callee-saved register; that widens the push to {r5, r6, lr} and displaces the
 * zero out of r5, shifting the entire function.  BOTH sites need the pin here,
 * which is unusual for this class: pinning only the first leaves 2 differing
 * and only the second leaves 15.  The two calls are adjacent in one basic
 * block, so with the first site pinned the second still has a live pseudo to
 * copy from and emits `mov r1, r5`.
 *
 * THE SetPos FILLS NEED THE MULTI-STATEMENT FORM, NOT THE ONE-STATEMENT ONE.
 * The ROM puts `mov r0, #0xf` BETWEEN the two seed movs and the two shifts at
 * three sites, and between `lsl r1` and `lsl r2` at the 0x9c9 site.  Written as
 * one statement (`q1 = 0xd6 << 18;`) the seed movs sit at depth 2 and the bare
 * `mov r0` at depth 1, so sched issues r0 LAST and the result is the same 11
 * differing as no pins at all.  One statement per machine instruction puts each
 * shift where the source puts it -- which is why the 0x9c9 arm shifts r1 before
 * assigning r0 and the other three do not.
 *
 * MINIMISED BY MEASUREMENT: nine pins.  Stripping any one costs 2-4 differing
 * (r1 at StartTask, 13; the four blocks, 3/2/3/3).  A tenth register, r2, was
 * pinned in each fill and is INERT -- individually in all four blocks and,
 * checked as a set, jointly -- so the third argument is left a bare expression
 * and gcc schedules its `mov r2` correctly on its own.
 *
 * No Makefile rule is needed: nothing in the tree wildcards rom_7d30e0, and the
 * CSE_CFLAGS rule in that directory is an exact non-pattern rule for
 * ovl_30_c_a_c_c_a_a_c_c_c_c_c_c_c_c_b.o.  This object builds at plain -O2.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __StartTask(void (*f)(void), int n);
extern int __GetFlag(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Actor_SetAnimSpeed(unsigned char *a, int n);
extern void __Func_808edac(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void OvlFunc_948_2009e94(void);
extern void OvlFunc_948_2009edc(void);
extern void OvlFunc_948_2009ac8(void);
extern void OvlFunc_948_2009c28(void);
extern void OvlFunc_948_2009cf8(void);
extern void OvlFunc_948_2009e54(void);
extern void OvlFunc_948_2009e74(void);
extern void OvlFunc_948_2009df8(void);
extern void OvlFunc_948_2009984(void);
extern void OvlFunc_948_2008aa8(void);

void OvlFunc_948_200a334(void)
{
    unsigned char *p;
    int v;

    __MapActor_GetActor(0xe)[0x55] = 0;
    {
        register int q1 __asm__("r1");
        q1 = 0xc8 << 4;
        __StartTask(OvlFunc_948_2009e94, q1);
        q1 = 0xc8 << 4;
        __StartTask(OvlFunc_948_2009edc, q1);
    }
    __Func_808edac(0x6b, 0, 0);
    if (__GetFlag(0xed9))
        __MapActor_SetAnim(0xe, 2);
    OvlFunc_948_2009ac8();
    OvlFunc_948_2009c28();
    OvlFunc_948_2009cf8();
    OvlFunc_948_2009e54();
    OvlFunc_948_2009e74();
    __Func_8092b08(8, 3);
    __MapActor_GetActor(0xb)[0x55] = 0;
    __MapActor_GetActor(0xc)[0x55] = 0;
    OvlFunc_948_2009df8();
    if (__GetFlag(0x80 << 2)) {
        OvlFunc_948_2009984();
        __MapActor_SetAnim(0xd, 5);
    }
    if (!__GetFlag(0x109)) {
        v = __GetFlag(0x9ca);
        if (v) {
            {
                register int q0 __asm__("r0");
                register int q1 __asm__("r1");
                q1 = 0xd6;
                q0 = 0xf;
                q1 <<= 18;
                __MapActor_SetPos(q0, q1, 0xce << 18);
            }
            p = __MapActor_GetActor(0xf);
            *(void **)(p + 0x6c) = (void *)OvlFunc_948_2008aa8;
        } else if (__GetFlag(0x9c9)) {
            {
                register int q0 __asm__("r0");
                register int q1 __asm__("r1");
                q1 = 0xde;
                q1 <<= 18;
                q0 = 0xf;
                __MapActor_SetPos(q0, q1, 0xa6 << 18);
            }
            p = __MapActor_GetActor(0xf);
            *(short *)(*(int *)(p + 0x50) + 0x1e) = v;
            __Actor_SetAnimSpeed(p, 0x10);
        } else if (__GetFlag(0x9c8)) {
            {
                register int q0 __asm__("r0");
                register int q1 __asm__("r1");
                q1 = 0x92;
                q0 = 0xf;
                q1 <<= 18;
                __MapActor_SetPos(q0, q1, 0xaa << 18);
            }
        } else {
            {
                register int q0 __asm__("r0");
                register int q1 __asm__("r1");
                q1 = 0x92;
                q0 = 0xf;
                q1 <<= 18;
                __MapActor_SetPos(q0, q1, 0xa6 << 18);
            }
        }
    }
}

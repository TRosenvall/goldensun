// fakematch
/* OvlFunc_895_2008420  --  0x02008420
 *
 * Was the whole of goldensun/asm/overlays/rom_78dee8/ovl_30_c_c_a_c_a.s;
 * split_s.py confirmed one function and no data tail.
 *
 * 127 instructions behind two save flags. THREE CROSSED FILLS, all closed with
 * batch 212's barrier-free cure -- the shifts and negations written in the ROM's
 * MOV order rather than its own order. Two are three-register `0x80 << n`
 * triples and the third is the `-1, -1, 0xe666` pair; none needed a volatile
 * asm. That is now four functions closed this way and no case yet where the
 * reordering was tried and the barrier was still required.
 *
 * ONE WALL FOR A HOISTED PINNED POOL LOAD. The message base is
 * `register int m __asm__("r6")` -- a plain integer with `m += 1` later, so the
 * pin is needed, the contrast case being a linker symbol which derives unaided.
 * Its `ldr r6, =0x1032` was scheduled THREE statements early, and a single
 * `do { } while (0)` behind it is enough: 7 of 127 to 3. Batch 207's rule says
 * to add the second wall only if the load is still moving, and here it is not,
 * even though the load had crossed three statements rather than one.
 *
 * The last three instructions were the first __CopyMapTiles fill, whose ROM
 * order is r2, r3, r1, r0 -- all four anchored, which is what the tree's
 * anchor-every-argument rule asks for once any one of them is out of place.
 *
 * Both stack-argument pairs are named locals: the first call passes the same
 * local twice, and the ROM stores it to both slots from one register.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_800fe9c(void);
extern void __Func_801776c(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_808e118(void);
extern void __Func_8092adc(int a, int b, int c);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_895_2008420(void)
{
    register int e __asm__("r5");
    register int m __asm__("r6");
    register int p0 __asm__("r0");
    int s0, s1;

    if (__GetFlag(0xf02) != 0) {
        p0 = 0x821;
        if (__GetFlag(p0) == 0) {
            __CutsceneStart();
            __Func_808e118();
            __PlaySound(0xb6);
            e = 1;
            {
                register int q0 __asm__("r0");
                register int q1 __asm__("r1");
                register int q2 __asm__("r2");
                register int q3 __asm__("r3");
                q2 = 0x64; q3 = 0x47; q1 = 0x47; q0 = 0;
                __CopyMapTiles(q0, q1, q2, q3, e, e);
            }
            __Func_800fe9c();
            __CutsceneWait(0x28);
            do { } while (0);
            m = 0x1032;
            { PIN2; q1 = 1; q0 = m; __Func_801776c(q0, q1); }
            __CutsceneWait(0x14);
            __PlaySound(0xb7);
            __CopyMapTiles(0x7a, 0x14, 0x78, 0x1e, e, 2);
            s0 = 0x78;
            s1 = 0x1e;
            __Func_8010704(0x7a, 0x14, 1, 2, s0, s1);
            __Func_800fe9c();
            { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q0 <<= 9; q1 <<= 9; q2 <<= 9;
              __Func_8012330(q0, q1, q2); }
            __CutsceneWait(0x14);
            { PIN3; q1 = 0x80; q0 = 0; q1 <<= 1; q2 = 0;
              __MapActor_Emote(q0, q1, q2); }
            { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q0 <<= 10; q1 <<= 10; q2 <<= 9;
              __Func_8012330(q0, q1, q2); }
            __CutsceneWait(0x14);
            { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0x28;
              __Func_8092adc(q0, q1, q2); }
            { PIN3; q1 = 0x80; q0 = 0; q1 <<= 8; q2 = 0x14;
              __Func_8092adc(q0, q1, q2); }
            __Func_8092adc(0, 0, 0x14);
            { PIN3; q1 = 0x80; q0 = 0; q1 <<= 7; q2 = 0xa;
              __Func_8092adc(q0, q1, q2); }
            __MapActor_Jump(0, 4, 0x14);
            __MapActor_Jump(0, 6, 0x28);
            { PIN3; q0 = 1; q1 = 1; q0 = -q0; q1 = -q1; q2 = 0xe666;
              __Func_8012330(q0, q1, q2); }
            m += 1;
            __CutsceneWait(0x28);
            { PIN2; q1 = 1; q0 = m; __Func_801776c(q0, q1); }
            __SetFlag(0x143);
            p0 = 0x821;
            __SetFlag(p0);
            __CutsceneEnd();
        }
    }
}

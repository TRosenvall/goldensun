// fakematch
/* OvlFunc_882_2008f38  --  0x02008f38
 *
 * Cut out of goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_a.s.
 *
 * The third member of a family already worked twice --
 * src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_c_a_a_b.c (batch 207) and the
 * parked src/non_matching/ovl_77dd1c/2008d5c.c (batch 208). Same overlay, same
 * guarded tail on 0x837 / 0x841 / 0xc3 << 2, same OvlFunc_882_2009a64 pair in
 * the two arms. Exact on the first screen, 133 instructions.
 *
 * EVERY LEVER CAME FROM THE TWO SIBLINGS: six r0 pins for the three flag ids
 * each used twice, the shifted-constant stores written as `v = 0xc0; v <<= 9;`
 * statements rather than inline, the actor fetched once into a named local and
 * written at five offsets, and both arms of the position test anchored because
 * their third argument is pooled.
 *
 * The one thing read fresh off the listing is the crossed __Func_80921c4 in the
 * middle -- `mov r1,#0x81 / mov r2,#0xd5 / lsl r2,#2 / mov r0,#0xc / lsl r1,#1`,
 * movs r1 then r2 against shifts r2 then r1 -- which takes one barrier after
 * `q1 = 0x81`. Placing it before the first compile rather than iterating to it
 * is the whole difference between this function and its parked sibling.
 */
extern void OvlFunc_882_20090a4(void);
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
extern void __Func_8092b08(int a, int b);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_882_2008f38(void)
{
    unsigned char *a;
    unsigned char *p;
    register int p0 __asm__("r0");
    int v, w, u, x;

    p0 = 0x311;
    if (__GetFlag(p0) == 0) {
        __CutsceneStart();
        p0 = 0x831;
        if (__GetFlag(p0) == 0) {
            a = __MapActor_GetActor(0xc);
            { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x80; q1 <<= 11; q2 <<= 9; q0 <<= 11;
              __Func_8012330(q0, q1, q2); }
            __PlaySound(0x8d);
            __WaitFrames(0x28);
            __PlaySound(0x91);
            { PIN3; q2 = 0xca; q0 = 0xc; q1 = 0x17d0000; q2 <<= 18;
              __MapActor_SetPos(q0, q1, q2); }
            v = 0xc0;
            v <<= 9;
            *(int *)(a + 0x30) = v;
            *(int *)(a + 0x34) = v;
            w = 0x80;
            u = *(int *)(a + 0xc);
            w <<= 17;
            u += w;
            *(int *)(a + 0xc) = u;
            *(int *)(a + 0x3c) = u;
            x = 0x80;
            x <<= 8;
            *(int *)(a + 0x44) = x;
            { PIN3; q1 = 0x91; q2 = 0x341; q0 = 0xc; q1 <<= 1;
              __Func_80921c4(q0, q1, q2); }
            __Func_8092b08(0xc, 1);
            { PIN3;
              q1 = 0x81; __asm__ volatile ("" : : "r" (q1));
              q2 = 0xd5; q2 <<= 2; q0 = 0xc; q1 <<= 1;
              __Func_80921c4(q0, q1, q2); }
            __Func_8092b08(0xc, 2);
            { PIN3; q2 = 0xda; q1 = 0xe0; q2 <<= 2; q0 = 0xc;
              __Func_80921c4(q0, q1, q2); }
            __CutsceneWait(0x28);
            __PlaySound(0x121);
            { PIN3; q0 = 1; q1 = 1; q0 = -q0; q1 = -q1; q2 = 0xe666;
              __Func_8012330(q0, q1, q2); }
            __Func_8012350();
            __Func_809202c();
            p0 = 0x831;
            __SetFlag(p0);
        }
        OvlFunc_882_20090a4();
        p0 = 0x311;
        __SetFlag(p0);
        p0 = 0x837;
        if (__GetFlag(p0) != 0) {
            p0 = 0x841;
            if (__GetFlag(p0) == 0) {
                p0 = 0xc3; p0 <<= 2;
                if (__GetFlag(p0) == 0) {
                    p = __MapActor_GetActor(0);
                    if (*(int *)(p + 0xc) > (0x80 << 16)) {
                        { PIN2; q1 = 0x34b; q0 = 0xdb;
                          OvlFunc_882_2009a64(q0, q1); }
                        { PIN3; q0 = 0; q1 = 0xb3; q2 = 0x33d;
                          __Func_80921c4(q0, q1, q2); }
                    } else {
                        { PIN2; q1 = 0xe3; q1 <<= 2; q0 = 0xd6;
                          OvlFunc_882_2009a64(q0, q1); }
                        { PIN3; q0 = 0; q1 = 0xdb; q2 = 0x38f;
                          __Func_80921c4(q0, q1, q2); }
                    }
                    p0 = 0xc3; p0 <<= 2;
                    __SetFlag(p0);
                }
            }
        }
        __CutsceneEnd();
    }
}

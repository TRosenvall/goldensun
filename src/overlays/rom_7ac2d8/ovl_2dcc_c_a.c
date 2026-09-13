/* asm/overlays/rom_7ac2d8/ovl_2dcc_c.s -- BOTH functions, exact.
 *
 *   OK OvlFunc_924_200ae6c -- 252 bytes, 111 encodings and 12 relocations identical
 *   OK OvlFunc_924_200af68 -- 1616 bytes, 652 encodings and 138 relocations identical
 *   merged TU (wholecmp): encodings equal, 763 == 763, size 1868 == 1868
 *     the only object-level residue is the .L5e70 relocation: the reference
 *     resolves it against .data because the blob is in the same TU; ours emits
 *     a symbol reloc.  The blob is an .incbin and cannot move into C, so the
 *     defining .s must carry `.global .L5e70` (precedent:
 *     asm/overlays/rom_7ac2d8/ovl_e20_c_c_c_c_c_c_c_c.s:13 for .L6700).
 *
 * LEVERS USED
 *  - 200ae6c: one BARE r0 pin at the __Func_8092950 site.  The park called this
 *    an "argument interleave at an UNGUARDED site" and unreachable; it is the
 *    ceiling batch 259 disproved.  A bare hard-register declaration is enough --
 *    no dominating block, no barrier.  2 differing -> 0 on one line.
 *  - 200af68 register allocation: PER-LOOP LOCALS.  One shared x/y/z/w set gave
 *    the loop counter r10 and &p r9 where the ROM has r8 and r10; giving each of
 *    the three loops its own locals put every one of r5..r11 on the ROM's value.
 *    583 differing -> 489 on that change alone.
 *  - 200af68 tail: EVERY call site pinned in the ROM's own assignment order,
 *    declarations hoisted (C89) and assignments in listing order.  This kills
 *    the repeated-constant CSE (0xc6 << 2 etc.) and fixes the r0-in-the-middle
 *    interleave in the same edit.  369 -> 11.
 *  - one crossed mov/shift site (__Func_8012330(0xa0<<11, 0xa0<<11, 0x80<<9)):
 *    three bare pins plus the documented volatile barrier after the FIRST mov.
 *  - `neg`+`add` rather than `sub` is spelled `-x - K`, not `K - x`.
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    short f18;
    unsigned char pad1a[2];
    void *f1c;
    unsigned char pad20[8];
};

extern unsigned int __Random(void);
extern void __PlaySound(int id);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __CutsceneEnd(void);
extern void __StopTask(void (*fn)(void));
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_809202c(void);
extern int __GetFlag(int id);
extern void __MapActor_SetSpeed(int a, int b, int c);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __MapActor_Jump(int a, int b, int c);
extern void __MapActor_SetAnim(int a, int b);
extern void __MapActor_DoAnim(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);
extern void __MapActor_WaitMovement(int a);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8091200(int a, int b);
extern void __Func_8091220(int a, int b);
extern void __Func_8091254(int a);
extern void __Func_8091e9c(int a);
extern void OvlFunc_common0_10c(int a, int b, int c, int d,
                                int e, int f, int g, struct P *p);
extern void OvlFunc_924_200ae08(void);
extern void OvlFunc_924_200adcc(void);
extern int *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(int *a, int f);
extern void __Func_8092950(int a, int b);
extern void OvlFunc_924_200bbd4(int x, int y, int z);
extern unsigned char L5e70[] __asm__(".L5e70");

void OvlFunc_924_200ae6c(int slot)
{
    struct P p;
    int *a;
    unsigned int i;
    int x;
    int y;
    int n;

    a = __MapActor_GetActor(slot);
    *((unsigned char *)a + 0x55) = 0;
    __Actor_SetSpriteFlags(a, 0);
    {
        register int q0 __asm__("r0") = slot;
        __Func_8092950(q0, 0x80 << 1);
    }
    __PlaySound(0xdd);
    p.f0 = 1;
    p.f4 = 5;
    p.f18 = 0x8f << 1;
    p.f1c = L5e70;
    i = 0;
    do {
        if (i <= 0x1f)
            OvlFunc_924_200bbd4(a[2], a[3], a[4]);
        if ((1 & i) != 0) {
            __PlaySound(0xf6);
            x = a[2] + ((__Random() * 24) >> 16 << 16);
            x += 0xfff40000;
            y = a[3] + ((__Random() << 5) >> 16 << 16);
            y += 0xfff00000;
            n = ((__Random() * 4) >> 16 << 15) + (0x80 << 8);
            OvlFunc_common0_10c(x, y, a[4], 0, n, 0, 0xcc << 14, &p);
        }
        a[3] = a[3] + i * 0x1999;
        a[0xf] = a[3];
        __WaitFrames(2);
        i++;
    } while (i <= 0x2f);
}

void OvlFunc_924_200af68(void)
{
    struct P p;
    unsigned int i;
    int a0, a1;
    int b0, b1, b2;
    int c0, c1, c2, c3;

    __CutsceneStart();
    __CutsceneWait(0x14);
    OvlFunc_924_200ae08();
    __StopTask(OvlFunc_924_200adcc);
    {
        register int s0 __asm__("r3");
        register int s1 __asm__("r2");
        s0 = 9;
        s1 = 4;
        __CopyMapTiles(0x2d, 0x4d, 0x2d, 0x49, s0, s1);
    }
    __CutsceneWait(0x1e);
    p.f0 = 1;
    p.f4 = 5;
    p.f18 = 0x8f << 1;
    p.f1c = L5e70;
    i = 0;
    do {
        if ((1 & i) != 0)
            __PlaySound(0xf6);
        a0 = ((__Random() * 48) >> 16 << 16) + (0xc0 << 18);
        a1 = ((__Random() * 56) >> 16 << 16) + (0x88 << 16);
        OvlFunc_common0_10c(a0, 0, a1, 0, 0, 0, 0x330001, &p);
        __CutsceneWait(2);
        i++;
    } while (i <= 0xf);
    __CutsceneWait(0x28);
    i = 0;
    do {
        if ((1 & i) != 0)
            __PlaySound(0xf6);
        b0 = ((__Random() * 48) >> 16 << 16) + (0xc0 << 18);
        b1 = ((__Random() * 56) >> 16 << 16) + (0x98 << 16);
        b2 = -(int)(((__Random() * 10) >> 16) * 0x3333) - 0x3333;
        OvlFunc_common0_10c(b0, 0, b1, 0, 0, b2, 0x330001, &p);
        __CutsceneWait(2);
        i++;
    } while (i <= 0xf);
    __CutsceneWait(0x3c);
    __PlaySound(0x8d);
    {
        register int t0 __asm__("r0") = 0xa0;
        register int t1 __asm__("r1") = 0xa0;
        register int t2 __asm__("r2") = 0x80;
        __asm__ volatile ("" : : "r" (t0));
        t2 <<= 9;
        t0 <<= 11;
        t1 <<= 11;
        __Func_8012330(t0, t1, t2);
    }
    __CutsceneWait(0x3c);
    p.f4 = 7;
    p.f8 = 0xb333;
    p.fc = 0xb333;
    p.f10 = 0x13333;
    p.f14 = 0x13333;
    i = 0;
    do {
        {
            register int s1 __asm__("r1");
            s1 = 1;
            __CopyMapTiles(0x3b, 12 - i, 0x30, 12 - i, 3, s1);
        }
        c3 = 0;
        do {
            c0 = ((__Random() * 48) >> 16 << 16) + (0xc0 << 18);
            c1 = ((__Random() << 3) >> 16) * 0x1999 - 0x6664;
            c2 = ((__Random() << 3) >> 16) * 0x1999;
            OvlFunc_common0_10c(c0, 0,
                                ((0 - ((unsigned int)c3 >> 1) - i * 16) << 16) + (0xc0 << 16),
                                c1, 0, c2, 0xd0001, &p);
            c3++;
            __CutsceneWait(2);
        } while ((unsigned int)c3 <= 0x1f);
        i++;
    } while (i <= 3);
    __PlaySound(0x121);
    {
        register int m0 __asm__("r0");
        register int m1 __asm__("r1");
        m0 = 1;
        m1 = 1;
        m0 = -m0;
        m1 = -m1;
        __Func_8012330(m0, m1, 0xe666);
    }
    __Func_8012350();
    __Func_809202c();
    __CutsceneWait(0x1e);
    if (__GetFlag(0x881)) {
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q0 = 0;
            q1 = 0xcccc;
            q2 = 0x6666;
            __MapActor_SetSpeed(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xce;
            q0 = 0;
            q1 <<= 2;
            q2 = 0xe8;
            __Func_80921c4(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q0 = 0;
            q1 <<= 2;
            q2 = 0xe8;
            __Func_80921c4(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q0 = 0;
            q1 <<= 2;
            q2 = 0xd0;
            __Func_80921c4(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x80;
            q2 = 0x80;
            q0 = 0;
            q1 <<= 10;
            q2 <<= 9;
            __MapActor_SetSpeed(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q0 = 0;
            q1 = 4;
            q2 = 0;
            __MapActor_Jump(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q2 = 0xc8;
            q1 <<= 2;
            q0 = 0;
            __Func_809218c(q0,q1,q2);
        }
        __CutsceneWait(0xa);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 0x12;
            q0 = 0;
            __MapActor_SetAnim(q0,q1);
        }
        OvlFunc_924_200ae6c(0);
        __CutsceneWait(0x3c);
        __StopTask(OvlFunc_924_200adcc);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q0 = 0x80;
            q0 <<= 9;
            q1 = 0;
            __Func_8091220(q0,q1);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 0;
            q0 = 0x10005;
            __Func_8091200(q0,q1);
        }
        __Func_8091254(0x78);
        __CutsceneWait(0x78);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 0;
            q0 = 0x7fff;
            __Func_8091200(q0,q1);
        }
        __Func_8091254(0x3c);
        __CutsceneWait(0x3c);
        __Func_8091e9c(9);
        __CutsceneEnd();
    } else {
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q0 = 0;
            q1 = 0xcccc;
            q2 = 0x6666;
            __MapActor_SetSpeed(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q0 = 1;
            q1 = 0xcccc;
            q2 = 0x6666;
            __MapActor_SetSpeed(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q0 = 2;
            q1 = 0xcccc;
            q2 = 0x6666;
            __MapActor_SetSpeed(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q0 = 3;
            q1 = 0xcccc;
            q2 = 0x6666;
            __MapActor_SetSpeed(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xce;
            q0 = 0;
            q1 <<= 2;
            q2 = 0xf0;
            __Func_80921c4(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xa0;
            q0 = 0;
            q1 <<= 8;
            q2 = 0x14;
            __Func_8092adc(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xce;
            q2 = 0xf0;
            q0 = 3;
            q1 <<= 18;
            q2 <<= 16;
            __MapActor_SetPos(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q0 = 3;
            q1 <<= 2;
            q2 = 0xe8;
            __Func_80921c4(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc0;
            q1 <<= 8;
            q2 = 0;
            q0 = 3;
            __Func_8092adc(q0,q1,q2);
        }
        __CutsceneWait(0x3c);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x80;
            q2 = 0x14;
            q0 = 3;
            q1 <<= 6;
            __Func_8092adc(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 3;
            q0 = 3;
            __MapActor_DoAnim(q0,q1);
        }
        __CutsceneWait(0x28);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc0;
            q0 = 3;
            q1 <<= 8;
            q2 = 0x14;
            __Func_8092adc(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q2 = 0xc8;
            q1 <<= 2;
            q0 = 3;
            __Func_80921c4(q0,q1,q2);
        }
        OvlFunc_924_200ae6c(3);
        __CutsceneWait(0x14);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 2;
            q0 = 0;
            __Func_80925cc(q0,q1);
        }
        __CutsceneWait(0x1e);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q0 = 0;
            q1 <<= 2;
            q2 = 0xe8;
            __Func_80921c4(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc0;
            q0 = 0;
            q1 <<= 8;
            q2 = 0;
            __Func_8092adc(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q2 = 0xe8;
            q0 = 1;
            q1 <<= 18;
            q2 <<= 16;
            __MapActor_SetPos(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q2 = 0xe8;
            q0 = 2;
            q1 <<= 18;
            q2 <<= 16;
            __MapActor_SetPos(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xcc;
            q0 = 1;
            q1 <<= 2;
            q2 = 0xe0;
            __Func_809218c(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc0;
            q1 <<= 2;
            q2 = 0xe0;
            q0 = 2;
            __Func_80921c4(q0,q1,q2);
        }
        __MapActor_WaitMovement(1);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xa0;
            q0 = 1;
            q1 <<= 8;
            q2 = 0;
            __Func_8092adc(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xe0;
            q0 = 2;
            q1 <<= 8;
            q2 = 0x14;
            __Func_8092adc(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x81;
            q0 = 0;
            q1 <<= 1;
            q2 = 0;
            __MapActor_Emote(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x81;
            q0 = 1;
            q1 <<= 1;
            q2 = 0;
            __MapActor_Emote(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x81;
            q0 = 2;
            q1 <<= 1;
            q2 = 0x50;
            __MapActor_Emote(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc0;
            q0 = 1;
            q1 <<= 7;
            q2 = 0;
            __Func_8092adc(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x80;
            q2 = 0x14;
            q0 = 2;
            q1 <<= 6;
            __Func_8092adc(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 1;
            q0 = 0;
            __Func_80925cc(q0,q1);
        }
        __CutsceneWait(0x3c);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x80;
            q2 = 0x80;
            q0 = 0;
            q1 <<= 8;
            q2 <<= 7;
            __MapActor_SetSpeed(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q0 = 0;
            q1 <<= 2;
            q2 = 0xe0;
            __Func_80921c4(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x80;
            q0 = 1;
            q1 <<= 8;
            q2 = 0;
            __Func_8092adc(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q0 = 2;
            q1 = 0;
            q2 = 0;
            __Func_8092adc(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q0 = 0;
            q1 <<= 2;
            q2 = 0xd0;
            __Func_80921c4(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xa0;
            q0 = 1;
            q1 <<= 8;
            q2 = 0;
            __Func_8092adc(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xe0;
            q2 = 0x14;
            q0 = 2;
            q1 <<= 8;
            __Func_8092adc(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 3;
            q0 = 0;
            __MapActor_DoAnim(q0,q1);
        }
        __CutsceneWait(0x14);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x80;
            q2 = 0x80;
            q0 = 0;
            q1 <<= 10;
            q2 <<= 9;
            __MapActor_SetSpeed(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q0 = 0;
            q1 = 4;
            q2 = 0;
            __MapActor_Jump(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q2 = 0xc8;
            q1 <<= 2;
            q0 = 0;
            __Func_809218c(q0,q1,q2);
        }
        __CutsceneWait(0xa);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q0 = 0;
            q1 = 0x12;
            __MapActor_SetAnim(q0,q1);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x80;
            q0 = 1;
            q1 <<= 1;
            q2 = 0;
            __MapActor_Emote(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x80;
            q2 = 0;
            q0 = 2;
            q1 <<= 1;
            __MapActor_Emote(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q0 = 1;
            q1 = 2;
            __Func_809259c(q0,q1);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 2;
            q0 = 2;
            __Func_809259c(q0,q1);
        }
        OvlFunc_924_200ae6c(0);
        __CutsceneWait(0x3c);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x81;
            q0 = 1;
            q1 <<= 1;
            q2 = 0;
            __MapActor_Emote(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0x81;
            q0 = 2;
            q1 <<= 1;
            q2 = 0x50;
            __MapActor_Emote(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q2 = 0x14;
            q0 = 1;
            q1 = 2;
            __Func_8092848(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q0 = 1;
            q1 = 3;
            __MapActor_SetAnim(q0,q1);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 3;
            q0 = 2;
            __MapActor_DoAnim(q0,q1);
        }
        __CutsceneWait(0x28);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q0 = 1;
            q1 <<= 2;
            q2 = 0xd8;
            __Func_80921c4(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xe0;
            q0 = 2;
            q1 <<= 8;
            q2 = 0;
            __Func_8092adc(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q1 <<= 2;
            q2 = 0xc8;
            q0 = 1;
            __Func_80921c4(q0,q1,q2);
        }
        __CutsceneWait(0x1e);
        OvlFunc_924_200ae6c(1);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q0 = 2;
            q1 <<= 2;
            q2 = 0xd8;
            __Func_80921c4(q0,q1,q2);
        }
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            register int q2 __asm__("r2");
            q1 = 0xc6;
            q2 = 0xc8;
            q1 <<= 2;
            q0 = 2;
            __Func_80921c4(q0,q1,q2);
        }
        __CutsceneWait(0x1e);
        OvlFunc_924_200ae6c(2);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q0 = 0x80;
            q1 = 0;
            q0 <<= 9;
            __Func_8091220(q0,q1);
        }
        __StopTask(OvlFunc_924_200adcc);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 0;
            q0 = 0x10005;
            __Func_8091200(q0,q1);
        }
        __Func_8091254(0x78);
        __CutsceneWait(0x78);
        {
            register int q0 __asm__("r0");
            register int q1 __asm__("r1");
            q1 = 0;
            q0 = 0x7fff;
            __Func_8091200(q0,q1);
        }
        __Func_8091254(0x3c);
        __CutsceneWait(0x3c);
        __CutsceneEnd();
        __Func_8091e9c(8);
    }
}

// fakematch
/* Cluster OvlFunc_964_2009abc..OvlFunc_964_2009d04 extracted from
 * goldensun/asm/overlays/rom_7ed0a0/ovl_30_c_a_a.s.
 *
 * The .s holds these THREE functions and no .data/.rodata, so this C replaces
 * the whole translation unit: NO SPLIT and NO LINKER EDIT.
 * overlays/rom_7ed0a0/overlay.ld names the object on exactly one line,
 *     asm/overlays/rom_7ed0a0/ovl_30_c_a_a.o(.text)
 * in the .text output section only, and the cross-dir `asm/%.o: src/%.c` rule
 * already builds that object from this file.
 *
 * FLAG GROUP: NONE. tryc.makefile_flags for this path is the empty set, so the
 * object is built by the default rule at plain GCC296_CFLAGS (-O2).
 *
 * All three functions are byte-identical in this single translation unit:
 *   OK OvlFunc_964_2009abc -- 368 bytes, 155 encodings and 27 relocations identical
 *   OK OvlFunc_964_2009c2c -- 216 bytes,  91 encodings and 16 relocations identical
 *   OK OvlFunc_964_2009d04 -- 600 bytes, 252 encodings and 48 relocations identical
 *
 * `// fakematch` is inherited from OvlFunc_964_2009abc, which needs three
 * `register ... __asm__("rN")` pins on its first __MapActor_SetSpeed call and
 * two on its second. Neither of the other two functions carries scaffolding of
 * any kind.
 *
 * THE MERGE COSTS ONE SPELLING. The three candidates were written against
 * three different declarations of __MapActor_GetActor (`struct A *`,
 * `unsigned char *`, `struct Actor *`); one TU can only have one, and one
 * `struct Actor` serves all three. The single member that does NOT unify is
 * +0x64: OvlFunc_964_2009abc reads it with `ldrsh` (signed) and
 * OvlFunc_964_2009d04 with `ldrh` (unsigned). A `union { short; unsigned short; }`
 * is NOT the fix -- gcc-2.96 aligns the union to 4 and the struct grows from
 * 0x70 to 0x74, moving `sub sp` and the +0x6c store (3 differing). What works
 * is `short f64` plus an `unsigned short *` cast at OvlFunc_964_2009d04's two
 * read-modify-write sites, which are its only reads of the field.
 *
 * Per-function write-ups, with every lever and its measured-worse table, are in
 *   scratch_elev/b237/f2009abc/final.c
 *   scratch_elev/b237/f2009c2c/final.c
 *   scratch_elev/b238/f2009d04/final.c
 */
struct Sub {
    unsigned char pad0[9];
    unsigned char f9;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x23 - 0x18];
    unsigned char f23;
    unsigned char pad24[0x28 - 0x24];
    int f28;
    unsigned char pad2c[0x3c - 0x2c];
    int f3c;
    unsigned char pad40[0x50 - 0x40];
    struct Sub *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    short f64;
    unsigned char pad66[0x6c - 0x66];
    void *f6c;
};

extern int L3350[] __asm__(".L3350");
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __PlaySound(int id);
extern void __CutsceneWait(int frames);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern unsigned char *__Func_8093554(void);
extern void OvlFunc_964_2009038(void *actor);
extern void OvlFunc_964_20091e0(int who);
extern void OvlFunc_964_2009a98(void);

void OvlFunc_964_2009abc(int arg0)
{
    unsigned int i;
    int x;
    int z;

    {
        register int p0 __asm__("r0") = 10;
        register int p1 __asm__("r1") = 0x8000;
        register int p2 __asm__("r2") = 0x4000;
        __MapActor_SetSpeed(p0, p1, p2);
    }
    {
        register int q0 __asm__("r0") = 11;
        register int q1 __asm__("r1") = 0x8000;
        __MapActor_SetSpeed(q0, q1, 0x4000);
    }
    if (arg0)
        __PlaySound(0xb4);
    __Actor_TravelTo(__MapActor_GetActor(10), __MapActor_GetActor(10)->f8,
                     L3350[__MapActor_GetActor(10)->f64],
                     __MapActor_GetActor(10)->f10);
    __Actor_TravelTo(__MapActor_GetActor(11), __MapActor_GetActor(11)->f8,
                     L3350[__MapActor_GetActor(11)->f64],
                     __MapActor_GetActor(11)->f10);
    __MapActor_WaitMovement(10);
    __MapActor_WaitMovement(11);
    __MapActor_GetActor(10)->fc = L3350[__MapActor_GetActor(10)->f64];
    __MapActor_GetActor(11)->fc = L3350[__MapActor_GetActor(11)->f64];
    if (arg0)
        __PlaySound(0x121);
    for (i = 0; i <= 4; i++) {
        if (__MapActor_GetActor(i + 10)->fc / 0x10000 < 0 &&
            __MapActor_GetActor(i + 10)->fc / 0x10000 > -30) {
            x = __MapActor_GetActor(i + 10)->f8 >> 20;
            z = __MapActor_GetActor(i + 10)->f10 >> 20;
            __Func_8010704(4, 9, 1, 1, x, z);
        }
    }
    __CutsceneWait(arg0);
}

void OvlFunc_964_2009c2c(unsigned int who)
{
    int best;
    unsigned int i;
    unsigned int id;

    best = -0x500000;
    for (i = 0; i <= 4; i++) {
        id = i + 10;
        if (id == who) continue;
        if (__MapActor_GetActor(id)->f8 >> 20
            != __MapActor_GetActor(who)->f8 >> 20) continue;
        if (__MapActor_GetActor(id)->f10 >> 20
            != __MapActor_GetActor(who)->f10 >> 20) continue;
        if (best > __MapActor_GetActor(id)->fc + 0x100000) continue;
        best = __MapActor_GetActor(id)->fc + 0x100000;
        __MapActor_GetActor(who)->f64 = id;
    }
    __MapActor_SetSpeed(who, 0x40000, 0x20000);
    __Actor_TravelTo(__MapActor_GetActor(who),
                     __MapActor_GetActor(who)->f8,
                     best,
                     __MapActor_GetActor(who)->f10);
    __MapActor_WaitMovement(who);
    __PlaySound(0xbc);
    OvlFunc_964_20091e0(who);
    __CutsceneWait(0x1e);
}

void OvlFunc_964_2009d04(void)
{
    struct Actor tmp;
    int i;
    int j;
    int k;
    int who;
    int other;
    int tgt;
    int n;
    int zero;
    unsigned short *p;
    struct Actor *tp;
    unsigned char *bp;

    __CutsceneStart();
    i = 0;
    tp = &tmp;
    zero = 0;
    who = 0xc;
loop:
    if ((__MapActor_GetActor(who)->f50->f9 & 0xc) == 0xc
        && __GetFlag(i + (0x80 << 2)) == 0) {
        OvlFunc_964_2009038(__MapActor_GetActor(who));
        __MapActor_SetPos(who, 0, 0);
        __SetFlag(i + (0x80 << 2));
        goto done;
    }
    if (__MapActor_GetActor(who)->f10 >> 20 != 9)
        goto next;
    if (__GetFlag(i + (0x80 << 2)) != 0)
        goto next;
    __MapActor_GetActor(who)->f14 = 0;
    __MapActor_GetActor(who)->f28 = 0;
    __MapActor_GetActor(who)->f3c = 0x80 << 24;
    __MapActor_GetActor(who)->f55 = 0;
    __MapActor_GetActor(who)->f64 = 0;
    k = i;
    for (j = 0; j < i; j++) {
        if (__GetFlag((0x80 << 2) + j) == 0) {
            tp->f8 = __MapActor_GetActor(who)->f8;
            tp->fc = __MapActor_GetActor(who)->fc;
            tp->f10 = __MapActor_GetActor(who)->f10;
            other = j + 0xc;
            __MapActor_GetActor(who)->f8 = __MapActor_GetActor(other)->f8;
            __MapActor_GetActor(who)->fc = __MapActor_GetActor(other)->fc;
            __MapActor_GetActor(who)->f10 = __MapActor_GetActor(other)->f10;
            __MapActor_GetActor(other)->f8 = tp->f8;
            __MapActor_GetActor(other)->fc = tp->fc;
            __MapActor_GetActor(other)->f10 = tp->f10;
            k = j;
            break;
        }
    }
    tgt = k + 0xc;
    __MapActor_GetActor(tgt)->f14 = zero;
    __MapActor_GetActor(tgt)->f28 = zero;
    __MapActor_GetActor(tgt)->f3c = 0x80 << 24;
    __MapActor_GetActor(tgt)->f55 = zero;
    __MapActor_GetActor(tgt)->f64 = zero;
    __Func_80933d4(0xc0 << 10, 0xc0 << 7);
    __Func_8093554()[0x55] = zero;
    __Func_80933f8(0xa8 << 16, 0x80 << 12, 0xb8 << 16, 1);
    __Func_8093530();
    OvlFunc_964_2009c2c(tgt);
    if (__MapActor_GetActor(tgt)->f8 >> 20 == 8) {
        p = (unsigned short *)&__MapActor_GetActor(0xa)->f64;
        n = *p;
        *p = n + 1;
        p = (unsigned short *)&__MapActor_GetActor(0xb)->f64;
        n = *p;
        *p = n - 1;
    } else {
        p = (unsigned short *)&__MapActor_GetActor(0xa)->f64;
        n = *p;
        *p = n - 1;
        p = (unsigned short *)&__MapActor_GetActor(0xb)->f64;
        n = *p;
        *p = n + 1;
    }
    __MapActor_GetActor(tgt)->f6c = (void *)OvlFunc_964_2009a98;
    OvlFunc_964_2009abc(0x28);
    bp = &__MapActor_GetActor(tgt)->f23;
    *bp |= 2;
    __SetFlag((0x80 << 2) + k);
    goto done;
next:
    i++;
    who++;
    if (i <= 2)
        goto loop;
done:
    __CutsceneEnd();
}

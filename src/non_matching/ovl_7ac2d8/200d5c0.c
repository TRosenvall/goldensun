/* OvlFunc_924_200d5c0 -- NON-MATCHING, 16 ENCODINGS OF 371.  Size equal, count
 * equal.
 *
 * Blocker class: a scratch-register rotation in two signed-division blocks.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7ac2d8/200d5c0.c \
 *     asm/overlays/rom_7ac2d8/ovl_35b8_a_c_c_a.s
 * ONE function in the reference -- it CONVERTS WHOLE, no split.
 *
 * THE RESIDUE:
 *
 *     rom   mov r2,r11 / ldr r3,[r2,#8] ... ldr r0,=0xfffff ... mov r1,r11 ...
 *     ours  mov r1,r11 / ldr r3,[r1,#8] ... ldr r2,=0xfffff ... mov r0,r11 ...
 *
 * plus `ldrh r6,[r1,r3]` against `ldrh r6,[r3,r1]` -- same two registers, the `plus`
 * operands canonicalised the other way.
 *
 * MEASURED: five table-indexing spellings (the byte-offset form was worth 18 -> 16;
 * `i[tp]`, `*(tp+i)`, `*(i+tp)` all inert), three cell-destination spellings
 * (`g += t*4` inert at 16; applying it to BOTH sites WORSE at 33).  Flag probes all
 * inert or harmful: -fno-gcse 89 against 90, -fno-cse-skip-blocks 310,
 * -fno-cse-follow-jumps 90, -fno-rerun-cse-after-loop 91, -fno-schedule-insns2 148,
 * -fno-strict-aliasing inert.  NO PER-FILE FLAG ROW IS IMPLIED.
 *
 * ================================================================
 * TWO MECHANISMS THIS FUNCTION ESTABLISHED, both read out of the compiler
 * ================================================================
 *
 * `define_peephole` IN arm.md:523 EXPLAINS `mov rX,#N / add rX,sp` AGAINST
 * `add rX,sp,#N`.  IT IS A PEEPHOLE, so it fires only when the two insns end up
 * ADJACENT IN THE OUTPUT -- so the ROM's two-instruction form IS NOT A SOURCE FACT
 * AT ALL, it is sched2 having separated them.  This retired what had looked like a
 * structural blocker here.
 *
 * `update_equiv_regs` (local-alloc.c) DOES `REG_LIVE_LENGTH (regno) *= 2;` FOR ANY
 * PSEUDO WITH A CONSTANT REG_EQUIV.  Global-alloc priority is
 * floor_log2(n_refs) * freq / live_length, so LENGTHENING A POOLED SYMBOL'S LIVE
 * RANGE BY HOISTING ITS ASSIGNMENT TO A NAMED LOCAL DEMOTES IT BELOW ITS
 * COMPETITORS.  Hoisting `gb = gBuffer;` to the top of the function took 336 -> 90
 * differing and handed `ang` the r6 the ROM gives it.
 *
 * And the converse: PINNING A POOLED SYMBOL TO A *CALL-CLOBBERED* REGISTER
 * REPRODUCES THE ROM'S REMATERIALISATION where CSE otherwise commons it --
 * `{ register int g __asm__("r1"); ... g = (int)gBuffer; ... }` at both sites gives
 * the ROM's two `ldr r1, =gBuffer` loads, because a hard register cannot survive the
 * intervening `bl` so cse1 has nothing to common into.  THE ORDER MATTERS: the
 * assignment must come AFTER the index computation, or the load lands in an earlier
 * basic block -- 15 instructions early, not a scheduling slip.
 *
 * THE call_via CLOBBER LIST IS CONFIRMED AS A LEVER A SECOND TIME: adding "r2"
 * moved the Func_8000888 pointer from r2 to the ROM's r4, worth 2 encodings.  Same
 * finding as src/non_matching/ovl_7aa430/2009cb4.c, and see
 * src/non_matching/ovl_7a5214/2009004.c from this same batch for the fuller
 * correction to that clobber list.
 *
 * STACK-SLOT ORDER FOLLOWS C DECLARATION ORDER EXACTLY.  Reversing four
 * declarations moved the first divergence from instruction 14 to 33 IN ONE EDIT --
 * worth checking before reading any register.
 *
 * A MEASUREMENT NOTE FROM THIS FUNCTION: it read as "3 disagreeing regions" while
 * measuring 23 of 371, and 67 regions while measuring 303.  Both correct; neither a
 * distance.
 */
extern unsigned int gState;
extern unsigned int gKeyHeld;
extern unsigned int gKeyPress;
extern unsigned char *iwram_3001edc;
extern short L5e44[] __asm__(".L5e44");
extern unsigned char gBuffer[];
extern int gScript_924__0200de20;
extern int gScript_924__0200de2c;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __ClearFlag(int f);
extern void __vec3_translate(int dist, int angle, int *v);
extern int __TestCollision(unsigned char *a, int *v);
extern void __Actor_SetScript(unsigned char *a, void *s);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __DeleteActor(unsigned char *a);
extern void __Sprite_SetAnim(unsigned char *s, int n);
extern void __Func_8092158(int a, int b, int c);
extern void OvlFunc_924_200d158(unsigned char *a);
extern int Func_8000888(int a, int b);

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12", "r2"
    );
    return _a;
}

struct E {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    unsigned char *f14;
    unsigned char *f18;
};

void OvlFunc_924_200d5c0(void)
{
    int v[3];
    int *p;
    unsigned char *base;
    unsigned char *cell2;
    int saved;
    unsigned char *savep;
    struct E *e;
    unsigned char *actor;
    unsigned char *cell1;
    unsigned char *a;
    unsigned char *spr;
    unsigned int r3;
    unsigned int r4;
    int idx;
    int off;
    int ang;
    int t;
    int n;
    int i;
    int j;
    short *tp;
    int mask;

    r3 = (unsigned int)&iwram_3001edc;
    e = *(struct E **)*(unsigned char **)r3;
    r3 -= 0x20;
    base = *(unsigned char **)r3;
    r4 = (unsigned int)&gState;
    r4 += 0xfa << 1;
    idx = *(int *)r4;
    off = idx * 4;
    off += 0x14;
    actor = *(unsigned char **)(base + off);
    savep = actor + 0x55;
    saved = *savep;
    tp = L5e44;
    i = (gKeyHeld >> 4) & 0xf;
    j = i * 2;
    ang = (unsigned short)*(short *)((char *)tp + j);
    if (*(short *)((char *)tp + j) == -1)
        return;

    p = v;
    p[0] = (*(int *)(actor + 8) & 0xfff00000) + (0x80 << 12);
    p[1] = *(int *)(actor + 0x14);
    p[2] = (*(int *)(actor + 0x10) & 0xfff00000) + (0x80 << 12);
    { register int g __asm__("r1"); int t;
      t = (p[2] / 0x100000) * 128 + p[0] / 0x100000; __asm__ volatile ("" : : "r" (t));
      g = (int)gBuffer;
      cell1 = (unsigned char *)(t * 4 + g); }
    __vec3_translate(0x80 << 14, ang, p);
    { register int g __asm__("r1"); int t;
      t = (p[2] / 0x100000) * 128 + p[0] / 0x100000; __asm__ volatile ("" : : "r" (t));
      g = (int)gBuffer;
      cell2 = (unsigned char *)(t * 4 + g); }
    if (cell1[2] != e->f4 && cell2[2] == e->f4 && e->f0 == 0)
        return;
    __CutsceneStart();
    t = __TestCollision(actor, v);
    if (t)
        return;
    a = e->f18;
    if (a != 0) {
        *(unsigned short *)(a + 0x64) = t;
        __Actor_SetScript(a, &gScript_924__0200de2c);
        __Actor_SetAnim(a, 7);
        e->f18 = (unsigned char *)t;
    }
    if (cell2[2] == e->f4 && e->f0 != 0) {
        unsigned char *m = e->f14;
        a = __CreateActor(0x1a, *(int *)(m + 8), *(int *)(m + 0xc),
                          *(int *)(m + 0x10));
        if (a != 0) {
            spr = *(unsigned char **)(a + 0x50);
            *(unsigned char **)(a + 0x14) = *(unsigned char **)(m + 0x14);
            __Actor_SetScript(a, &gScript_924__0200de20);
            a[0x55] = t;
            *(unsigned short *)(a + 0x64) = t;
            a[0x23] = 2;
            *(int *)(a + 0x30) = 0x80 << 11;
            *(int *)(a + 0x34) = 0x80 << 10;
            __Actor_TravelTo(a, p[0], p[1], p[2]);
            if (spr != 0) {
                __Sprite_SetAnim(spr, 6);
                spr[0x26] = 0;
            }
            e->f18 = a;
        }
        n = e->f0 - 1;
        e->f0 = n;
        if (n == 0) {
            __DeleteActor(e->f14);
            e->f14 = (unsigned char *)n;
            __ClearFlag(0x161);
        } else if (e->f14 != 0) {
            __Actor_SetAnim(e->f14, 6 - n);
        }
    }
    __Actor_SetAnim(actor, 6);
    __WaitFrames(3);
    __PlaySound(0x98);
    __Actor_SetAnim(actor, 7);
    *(int *)(actor + 0x30) = 0xc0 << 10;
    *(int *)(actor + 0x34) = 0x80 << 10;
    *(int *)(actor + 0x28) = 0x80 << 11;
    *savep = *savep & 0x7e;
    __Actor_SetSpriteFlags(actor, 0);
    __Func_8092158(0, ((short *)p)[1], ((short *)p)[5]);
    __Actor_SetAnim(actor, 6);
    __WaitFrames(2);
    if (cell2[2] != e->f4)
        __Actor_SetSpriteFlags(actor, 1);
    else
        __PlaySound(0xd7);
    __WaitFrames(1);
    *savep = saved;
    if (cell2[2] == e->f4 && e->f18 == 0) {
        __Actor_SetAnim(actor, 0x12);
        __PlaySound(0xf1);
        mask = 0xf;
        i = 0;
        for (;;) {
            if ((i & mask) == 0)
                OvlFunc_924_200d158(actor);
            if (i > 0x1f && gKeyPress != 0)
                break;
            __WaitFrames(1);
            i++;
        }
        __PlaySound(0x90 << 1);
        __WaitFrames(1);
        *(int *)(actor + 8) = e->fc;
        *(int *)(actor + 0x10) = e->f10;
        __Actor_SetSpriteFlags(actor, 1);
    }
    e->f8 = 0;
    __CutsceneEnd();
    *(int *)(base + (0xda << 1)) +=
        call_via(Func_8000888, *(int *)(base + (0xd8 << 1)), 0x80 << 14);
}

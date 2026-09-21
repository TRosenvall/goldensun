/* Cluster OvlFunc_924_200d458..OvlFunc_924_200d458 extracted from
 * goldensun/asm/overlays/rom_7ac2d8/ovl_35b8_a_c_a_c.s.
 *
 * Total .text for this TU = 288 bytes (= 0x120). Never attempted before batch 278.
 * No pins, no flags. This file needs no split -- the function is alone in its `.s`.
 *
 * TWIN OF OvlFunc_923_2009ec8 (src/overlays/rom_7aa430/ovl_1a3c_a_c_a_b.c) -- read that file for
 * the four levers. The two differ in exactly two lines, both per-overlay symbols (the script
 * pointer and the called sibling); no constant differs. Each source was verified by objcmp
 * against its OWN reference rather than inferred from the other. If you edit one, edit both.
 */
extern unsigned char *iwram_3001edc;
extern unsigned char gState[];
extern unsigned char gScript_924__0200de38[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __WaitFrames(int n);
extern void __SetFlag(int id);
extern void OvlFunc_924_200d388(void);

void OvlFunc_924_200d458(void)
{
    unsigned int r3; unsigned int r1; unsigned int r4;
    unsigned char *q; unsigned char *e; unsigned char *w; unsigned char *t;
    unsigned char *a; unsigned char *n; unsigned char *s;
    unsigned int off; int i;

    r3 = (unsigned int)&iwram_3001edc;
    q = *(unsigned char **)r3;
    r3 -= 0x20;
    e = *(unsigned char **)q;
    r1 = 0xfa;
    w = *(unsigned char **)r3;
    r4 = (unsigned int)&gState;
    r1 <<= 1;
    r4 += r1;
    off = *(unsigned int *)r4;
    off <<= 2;
    off += 0x14;
    t = *(unsigned char **)(w + off);
    if (*(unsigned int *)e > 2)
        return;
    __CutsceneStart();
    a = *(unsigned char **)(e + 0x14);
    if (a == 0) {
        n = __CreateActor(0x1a, *(int *)(t + 8),
                          *(int *)(t + 0xc) + (0xc0 << 13),
                          *(int *)(t + 0x10));
        if (n != 0) {
            s = *(unsigned char **)(n + 0x50);
            *(int *)(n + 0x14) = *(int *)(t + 0x14);
            __Actor_SetScript(n, gScript_924__0200de38);
            *(unsigned char **)(n + 0x68) = t;
            {
                int v = 4;
                n[0x55] = v;
            }
            *(int *)(n + 0xc) += 0xffff8000;
            if (s != 0) {
                int z = 0xd;
                int b;
                s[0x26] = 0;
                b = s[9];
                z = -z;
                z &= b;
                s[9] = z | 4;
            }
            n[0x54] = 0;
            *(unsigned char **)(e + 0x14) = n;
            a = n;
        } else {
            a = *(unsigned char **)(e + 0x14);
        }
    }
    for (i = *(int *)e; i <= 2; i++) {
        OvlFunc_924_200d388();
        __WaitFrames(0x1e);
        a[0x54] = 1;
        __Actor_SetAnim(a, 5 - i);
    }
    *(int *)e = 3;
    *(int *)(e + 0xc) = (*(int *)(a + 8) & 0xfff00000) + (0x80 << 12);
    *(int *)(e + 0x10) = (*(int *)(a + 0x10) & 0xfff00000) + (0x80 << 12);
    __SetFlag(0x161);
    __CutsceneEnd();
}

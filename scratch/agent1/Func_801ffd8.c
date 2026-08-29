struct Spr {
    unsigned char pad0[9];
    unsigned char f9_0 : 2;
    unsigned char f9_2 : 2;
    unsigned char f9_4 : 4;
    unsigned char pad_a[0x1c];
};

extern unsigned char *iwram_3001f2c;
extern int L73854[] __asm__(".L73854");
extern struct Spr *_CreateSprite(int id);
extern void _Sprite_SetAnim(struct Spr *s, int anim);
extern void StartTask(void *fn, int prio);
extern void Func_80200cc(void);

void Func_801ffd8(unsigned char *a, int x, int y)
{
    unsigned char *base;
    short *q;
    struct Spr **w;
    struct Spr *s;
    int i;
    int acc;
    int off;

    base = iwram_3001f2c;
    if (a != 0) {
        off = 0x8d << 2;
        q = (short *)(base + off);
        off -= 0x10;
        w = (struct Spr **)(base + off);
        i = 0;
        acc = 0;
        for (; i <= 3; i++) {
            s = _CreateSprite(L73854[i]);
            if (s != 0) {
                _Sprite_SetAnim(s, 2);
                *((unsigned char *)s + 0x26) = 0;
                s->f9_2 = 0;
            }
            *w++ = s;
            q[0] = ((*(unsigned short *)(a + 0xc) + x + acc) << 3) + 0x10;
            q[4] = ((*(unsigned short *)(a + 0xe) + y) << 3) + 0x10;
            acc += 3;
            q++;
        }
        StartTask(Func_80200cc, 0xc8 << 4);
    }
}

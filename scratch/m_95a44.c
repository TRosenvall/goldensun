extern unsigned char gState[];
extern unsigned char *iwram_3001f30;
extern unsigned char *MapActor_GetActor(int slot);
extern void Func_80958a8(void);
extern void _Func_80b0840(int a);
extern void _PlaySound(int id);
extern void Func_80925cc(int a, int b);
extern void WaitFrames(int n);
extern void Func_809592c(void);
extern void MapActor_Jump(int a, int b, int c);
extern void Func_80974d8(int *v);
extern void _DeleteActor(unsigned char *e);
extern void Func_809ba90(unsigned char *p, int a, int b, int c);
extern void Func_809ba7c(unsigned char *p, void (*f)(void));
extern void Func_8095938(void);
extern void Func_809ba70(unsigned char *p, int n);
extern unsigned int Random(void);
extern void _Sprite_SetColorswap(int a, int b);
extern unsigned int __udivsi3(unsigned int a, unsigned int b);
extern void Func_8092adc(int a, int b, int c);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void _Func_80b0894(void);
extern void Func_80958e4(void);

void GetJupiterDjinni(int slot)
{
    int v[3];
    int *bp;
    unsigned char *e;
    unsigned char *p;
    unsigned char *g;
    int i;

    e = MapActor_GetActor(slot);
    if (e == 0)
        return;
    Func_80958a8();
    p = iwram_3001f30;
    _Func_80b0840(0x20118c);
    _PlaySound(0xad);
    Func_80925cc(slot, 1);
    _PlaySound(0xae);
    Func_80925cc(slot, 1);
    _PlaySound(0xaf);
    Func_80925cc(slot, 1);
    WaitFrames(0x14);
    _PlaySound(0x8c);
    *(void **)(e + 0x6c) = Func_809592c;
    WaitFrames(0x28);
    _PlaySound(0x99);
    MapActor_Jump(slot, 0xc, 0x16);
    bp = v;
    bp[0] = *(int *)(e + 8);
    bp[1] = *(int *)(e + 0xc);
    bp[2] = *(int *)(e + 0x10);
    Func_80974d8(bp);
    _DeleteActor(e);
    _PlaySound(0xa4);
    p += 0x58;
    i = 0x17;
loop:
    Func_809ba90(p, 0x8e << 1, bp[0], bp[2]);
    Func_809ba7c(p, Func_8095938);
    Func_809ba70(p, 7);
    _Sprite_SetColorswap(*(int *)p, (Random() * 7) >> 16);
    *(int *)(p + 0x2c) = __udivsi3(Random(), 3) + (0x80 << 9);
    *(int *)(p + 0x28) = *(int *)(p + 0x2c);
    i--;
    WaitFrames(1);
    p += 0x48;
    if (i >= 0)
        goto loop;
    WaitFrames(0x3c);
    g = gState + (0xfa << 1);
    Func_8092adc(*(int *)g, 0x80 << 7, 0);
    WaitFrames(0x14);
    _Actor_SetAnim(MapActor_GetActor(*(int *)g), 0x1c);
    WaitFrames(0x28);
    _PlaySound(0xa4);
    WaitFrames(0x64);
    _Func_80b0894();
    Func_80958e4();
}

extern int L222c __asm__(".L222c");
extern int L2230 __asm__(".L2230");

extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned int __Random(void);
extern int __GetFlag(int id);
extern void __PlaySound(int id);

void OvlFunc_935_2008aa0(void)
{
    unsigned char *e;
    int i;
    int t;
    int c;
    unsigned int n;
    int *q;

    e = __MapActor_GetActor(0xa);
    if (e[0x5b] != 0)
        return;
    c = L222c + 1;
    L222c = c;
    if ((c & 0x3f) == 0) {
        q = &L2230;
        n = __Random() % 6;
        *q = n;
        e = __MapActor_GetActor(n + 0xa);
        *(int *)(e + 0x48) = 0xa3d;
    }
    i = 0;
    t = 0xff << 16;
    do {
        e = __MapActor_GetActor(i + 0xa);
        if (__GetFlag(i + (0x80 << 2))) {
            if (*(int *)(e + 0x28) > 0 || *(int *)(e + 0xc) <= 0x20ffff) {
                *(int *)(e + 0xc) = t;
                *(int *)(e + 0x48) = 0;
                *(int *)(e + 0x28) = 0;
                __PlaySound(0x6a);
            }
        } else {
            if (*(int *)(e + 0x28) > 0 || *(int *)(e + 0xc) <= 0xffff) {
                *(int *)(e + 0x48) = 0;
                *(int *)(e + 0x28) = 0;
                *(int *)(e + 0xc) = t;
                __PlaySound(0x6a);
            }
        }
        i += 1;
    } while (i <= 5);
}

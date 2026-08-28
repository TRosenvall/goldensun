extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);

void OvlFunc_964_20094ac(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *p;
    int v;
    int m;

    __SetFlag(0x201);
    if (__GetFlag(0x80 << 2) != 0) {
        a = __MapActor_GetActor(0xe);
        a += 0x62;
        *a = 0;
        b = __MapActor_GetActor(0xe);
        b += 0x59;
        p = b;
        m = 0xf7;
        v = *p & m;
    } else {
        a = __MapActor_GetActor(0xe);
        a += 0x62;
        *a = 1;
        b = __MapActor_GetActor(0xe);
        b += 0x59;
        p = b;
        m = 8;
        v = *p | m;
    }
    *p = v;
}

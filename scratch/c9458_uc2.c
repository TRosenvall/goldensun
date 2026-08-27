extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void *__MapActor_GetActor(int slot);

void OvlFunc_964_2009458(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char m;
    int v;

    __SetFlag(0x80 << 2);
    if (__GetFlag(0x201)) {
        p = (unsigned char *)__MapActor_GetActor(0xe);
        p += 0x62;
        *p = 0;
        q = (unsigned char *)__MapActor_GetActor(0xe);
        q += 0x59;
        m = 0xf7;
        v = m & *q;
    } else {
        p = (unsigned char *)__MapActor_GetActor(0xe);
        p += 0x62;
        *p = 1;
        q = (unsigned char *)__MapActor_GetActor(0xe);
        q += 0x59;
        m = 8;
        v = m | *q;
    }
    *q = v;
}

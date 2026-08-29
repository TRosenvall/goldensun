extern int __Func_80796c4(void *buf);
extern void OvlFunc_973_20080a0(int id, int levels);

void OvlFunc_973_20080c0(int levels)
{
    unsigned short buf[0x10];
    unsigned short *p;
    int n;

    n = __Func_80796c4(buf);
    if (n > 0) {
        p = buf;
        n = n;
        do {
            OvlFunc_973_20080a0(*p, levels);
            n--;
            p++;
        } while (n != 0);
    }
}

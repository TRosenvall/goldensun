extern int __GetFlag(int id);
extern unsigned char gScript_968__0200d508[];
extern unsigned char L4ef0[] __asm__(".L4ef0");
extern unsigned char L5028[] __asm__(".L5028");
extern unsigned char L4cf8[] __asm__(".L4cf8");
extern unsigned char L4ba8[] __asm__(".L4ba8");

unsigned char *OvlFunc_943_2008950(void)
{
    unsigned char *p;
    unsigned char *q;
    int v;
    int w;
    int o1, o2, o3, o4;

    o1 = 0xa7 << 1;
    o2 = 0xd7 << 1;
    o3 = 0xd7 << 1;
    o4 = 0xe3 << 1;
    if (__GetFlag(0x93e) != 0)
        return gScript_968__0200d508;
    if (__GetFlag(0x927) != 0)
        return L4ef0;
    v = __GetFlag(0x928);
    if (v != 0)
        return L5028;
    if (__GetFlag(0x911) != 0) {
        if (__GetFlag(0x925) != 0) {
            p = L4cf8;
            q = p + o1;
            *q = v;
            q = p + o2;
            w = 2;
        } else if (__GetFlag(0x922) != 0) {
            p = L4cf8;
            q = p + o3;
            w = 1;
        } else {
            return L4cf8;
        }
        *q = w;
        p += o4;
        *p = w;
        return L4cf8;
    }
    return L4ba8;
}

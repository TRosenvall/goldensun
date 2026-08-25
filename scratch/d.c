typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001f30;
extern void Func_8097608(void);

void Func_8096ab0(void)
{
    unsigned char *p;
    unsigned char *g;
    unsigned char *q;
    unsigned int o1;
    unsigned int o2;
    unsigned int o3;
    short a;
    short b;

    p = iwram_3001f30;
    o1 = 0x1e;
    if (*(short *)(p + o1) != 2)
        return;
    Func_8097608();
    o2 = 0x24a;
    g = (unsigned char *)&gState + o2;
    o2 = 0;
    a = *(short *)(g + o2);
    o3 = 0x1a;
    b = *(short *)(p + o3);
    if (a == b)
        return;
    q = *(unsigned char **)(p + 0x14);
    q += 0x5b;
    *q = 0;
}

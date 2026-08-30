extern unsigned char *__MapActor_GetActor(void);
extern int __cos(int a);
extern int __sin(int a);
extern int _divsi3_RAM(int a, int b);
extern void OvlFunc_946_2008da4(void);
extern void OvlFunc_946_2008ae8(int a, int b, int c, int d, int e, int f, int g, int *h);

void OvlFunc_946_2008e00(void)
{
    int v[3];
    int a[10];
    unsigned char *act;
    int *ap;
    int *vp;
    unsigned int i;
    int s;

    act = __MapActor_GetActor();
    ap = a;
    ap[9] = (int)OvlFunc_946_2008da4;
    vp = v;
    i = 0;
    do {
        vp[1] = 0;
        vp[0] = __cos(i << 12);
        s = __sin(i << 12);
        vp[2] = s;
        vp[0] = vp[0] + _divsi3_RAM(vp[0], 3);
        OvlFunc_946_2008ae8(*(int *)(act + 8), *(int *)(act + 0xc), *(int *)(act + 0x10),
                            vp[0], vp[1], s, 0x1000001, ap);
        i += 2;
    } while (i <= 0x10);
}

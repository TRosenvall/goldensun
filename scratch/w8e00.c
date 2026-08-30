extern unsigned char *__MapActor_GetActor(void);
extern int __cos(int a);
extern int __sin(int a);
extern int _divsi3_RAM(int a, int b);
extern void OvlFunc_946_2008da4(void);
extern void OvlFunc_946_2008ae8(int a, int b, int c, int d, int e, int f, int g, int *h);

void OvlFunc_946_2008e00(void)
{
    int a[10];
    int v[3];
    unsigned char *act;
    unsigned int i;
    int s;

    act = __MapActor_GetActor();
    a[9] = (int)OvlFunc_946_2008da4;
    i = 0;
    do {
        v[1] = 0;
        v[0] = __cos(i << 12);
        s = __sin(i << 12);
        v[2] = s;
        v[0] = v[0] + _divsi3_RAM(v[0], 3);
        OvlFunc_946_2008ae8(*(int *)(act + 8), *(int *)(act + 0xc), *(int *)(act + 0x10),
                            v[0], v[1], s, 0x1000001, a);
        i += 2;
    } while (i <= 0x10);
}

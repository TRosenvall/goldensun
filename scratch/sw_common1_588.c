typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern volatile int _AREA_8f;
extern volatile int _AREA_90;
extern void __Func_8019908(int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(void *a, int n);

void OvlFunc_common1_588(void *a, int b)
{
    unsigned char *g;
    void *p;
    unsigned int k;
    int v;
    int m;

    p = a;
    __Func_8019908(b, 5);
    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)(g + (unsigned int)0);
    if (v == (int)(&_AREA_8f))
        m = 0x2076;
    else if (v == (int)(&_AREA_90))
        m = 0x2078;
    else
        m = 0x207a;
    __MessageID(m + 1);
    __ActorMessage(p, 0);
}

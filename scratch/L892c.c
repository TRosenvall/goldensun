extern char *iwram_3001ebc;
extern unsigned char L269c[] __asm__(".L269c");

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __StartTask(void (*fn)(void), int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_895_2009ac8(void);

void OvlFunc_895_200892c(void)
{
    char *p;
    int s;

    __SetFlag(0xa2 << 1);
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x100;
    if (__GetFlag(0x814)) {
        *(int *)L269c = 0;
        __StartTask(OvlFunc_895_2009ac8, 0xc8 << 4);
    }
    if (__GetFlag(0x879)) {
        s = 6;
        __Func_8010704(5, 6, 1, 1, s, s);
        __Func_8010704(5, 6, 1, 1, 7, s);
        __Func_8010704(5, 6, 1, 1, 8, s);
        __Func_8010704(0, 1, 3, 1, s, 5);
    }
    if (__GetFlag(0x815)) {
        __MapActor_SetPos(8, 0xf0 << 15, 0xe8 << 16);
        s = 0xe;
        __Func_8010704(2, 0xa, 1, 1, 6, s);
        __Func_8010704(2, 0xa, 1, 1, 7, s);
        __Func_8010704(2, 0xa, 1, 1, 8, s);
    }
}

typedef unsigned char u8;

extern u8 gState[];
extern volatile unsigned int iwram_3001e40;
extern short *__MapActor_GetActor(int i);
extern unsigned int _umodsi3_RAM(unsigned int a, unsigned int b);
extern unsigned int __Random(void);
extern void __Func_80933f8(int a, int b, int c, int d);

void OvlFunc_881_200b95c(void)
{
    u8 *g;
    short *a;
    int x;
    int y;
    unsigned int v;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + 0x1f4));
    x = a[5];
    y = a[9];
    if (_umodsi3_RAM(iwram_3001e40, 3) == 0) {
        v = __Random() * 4 >> 16;
        switch (v) {
        case 0:
            __Func_80933f8((x << 16) - 0x10000, -1, (y << 16) + 0x10000, 1);
            break;
        case 1:
            __Func_80933f8((x << 16) + 0x10000, -1, (y << 16) - 0x10000, 1);
            break;
        case 2:
            __Func_80933f8((x << 16) + 0x10000, -1, (y << 16) + 0x10000, 1);
            break;
        case 3:
            __Func_80933f8((x << 16) - 0x10000, -1, (y << 16) - 0x10000, 1);
            break;
        }
    }
}

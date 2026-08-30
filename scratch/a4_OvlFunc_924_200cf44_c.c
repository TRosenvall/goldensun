typedef struct S {
    unsigned char pad[0x71c];
    unsigned char flags;
} S;

extern unsigned int iwram_3001f30;
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8096fb0(int a, int b);
extern void __Func_80970f8(int a, int b);
extern void __Func_809728c(void);
extern void __FieldMove(int a);
extern void __Func_8097174(void);

void OvlFunc_924_200cf44(void)
{
    S *p;
    int x, y;

    x = 0xd2 << 18;
    y = 0x96 << 18;
    p = (S *)iwram_3001f30;
    __MapActor_SetPos(0xb, x, y);
    __Func_8096fb0(0x5d, 1);
    __Func_80970f8(3, 0xb);
    p->flags |= 8;
    __Func_809728c();
    __FieldMove(1);
    __Func_8097174();
}

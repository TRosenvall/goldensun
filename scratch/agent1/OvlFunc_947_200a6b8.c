extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int a, int b);

void OvlFunc_947_200a6b8(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned int i;
    unsigned char m;

    p = __MapActor_GetActor(0);
    for (i = 8; i <= 0xb; i++) {
        q = __MapActor_GetActor(i);
        if (*(int *)(p + 0xc) / 0x10000 == *(int *)(q + 0xc) / 0x10000
            && *(int *)(p + 0x10) <= *(int *)(q + 0x10) - 0x80000
            && *(int *)(p + 0x10) > *(int *)(q + 0x10) - 0x180000) {
            if (*(int *)(p + 8) - 0x100000 <= *(int *)(q + 8)
                && *(int *)(q + 8) < *(int *)(p + 8) + 0x100000) {
                __Func_8092b08(0, ((unsigned int)*(unsigned char *)(*(unsigned char **)(q + 0x50) + 9) << 28) >> 30);
                return;
            }
        } else {
            q = __MapActor_GetActor(0) + 0x23;
            m = 1;
            m |= *q;
            *q = m;
        }
    }
}

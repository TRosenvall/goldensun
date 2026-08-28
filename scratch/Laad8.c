extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned int iwram_3001e40;

void OvlFunc_913_200aad8(void)
{
    unsigned char *a;
    int z;

    a = __MapActor_GetActor(0xd);
    if (a != 0) {
        *(a + 0x55) = 0;
        if ((iwram_3001e40 & 1) == 0)
            *(int *)(a + 0xc) = 0;
        else
            *(int *)(a + 0xc) = 0xfa << 17;
    }
    a = __MapActor_GetActor(0xe);
    if (a != 0) {
        z = 0;
        *(a + 0x55) = z;
        if ((iwram_3001e40 & 1) != 0)
            *(int *)(a + 0xc) = z;
        else
            *(int *)(a + 0xc) = 0xfa << 17;
    }
    a = __MapActor_GetActor(0xf);
    if (a != 0) {
        *(a + 0x55) = 0;
        if ((iwram_3001e40 & 1) == 0)
            *(int *)(a + 0xc) = 0;
        else
            *(int *)(a + 0xc) = 0xfa << 17;
    }
    a = __MapActor_GetActor(0x10);
    if (a != 0) {
        z = 0;
        *(a + 0x55) = z;
        if ((iwram_3001e40 & 1) != 0)
            *(int *)(a + 0xc) = z;
        else
            *(int *)(a + 0xc) = 0xfa << 17;
    }
}

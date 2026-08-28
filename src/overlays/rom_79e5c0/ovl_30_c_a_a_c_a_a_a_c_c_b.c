extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned int iwram_3001e40;

void OvlFunc_911_2008800(void)
{
    unsigned char *a;
    int z;

    a = __MapActor_GetActor(0x13);
    if (a != 0) {
        *(a + 0x55) = 0;
        if ((iwram_3001e40 & 1) == 0)
            *(int *)(a + 0xc) = 0;
        else
            *(int *)(a + 0xc) = 0xfa << 17;
    }
    a = __MapActor_GetActor(0x14);
    if (a != 0) {
        z = 0;
        *(a + 0x55) = z;
        if ((iwram_3001e40 & 1) != 0)
            *(int *)(a + 0xc) = z;
        else
            *(int *)(a + 0xc) = 0xfa << 17;
    }
    a = __MapActor_GetActor(0x15);
    if (a != 0) {
        *(a + 0x55) = 0;
        if ((iwram_3001e40 & 1) == 0)
            *(int *)(a + 0xc) = 0;
        else
            *(int *)(a + 0xc) = 0xfa << 17;
    }
    a = __MapActor_GetActor(0x16);
    if (a != 0) {
        z = 0;
        *(a + 0x55) = z;
        if ((iwram_3001e40 & 1) != 0)
            *(int *)(a + 0xc) = z;
        else
            *(int *)(a + 0xc) = 0xfa << 17;
    }
}

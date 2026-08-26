extern unsigned int iwram_3001e40;
extern unsigned int L57fc;
extern void *__MapActor_GetActor(int slot);
extern void OvlFunc_882_200a09c(void *a, int n);

void OvlFunc_882_200a0fc(void)
{
    if (((iwram_3001e40 >> L57fc) & 3) != 0) {
        OvlFunc_882_200a09c(__MapActor_GetActor(0x20), 1);
        OvlFunc_882_200a09c(__MapActor_GetActor(0x21), 1);
        OvlFunc_882_200a09c(__MapActor_GetActor(0x1e), 1);
        OvlFunc_882_200a09c(__MapActor_GetActor(0x1d), 1);
    } else {
        OvlFunc_882_200a09c(__MapActor_GetActor(0x20), 8);
        OvlFunc_882_200a09c(__MapActor_GetActor(0x21), 8);
        OvlFunc_882_200a09c(__MapActor_GetActor(0x1e), 8);
        OvlFunc_882_200a09c(__MapActor_GetActor(0x1d), 8);
    }
}

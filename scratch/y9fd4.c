extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int OvlFunc_947_2009268(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_947_20083a8(void);
extern void OvlFunc_947_2009d84(void);

void OvlFunc_947_2009fd4(void)
{
    unsigned char *p;
    int m;

    __CutsceneStart();
    if (OvlFunc_947_2009268() == 0) {
        m = 0xfe;
        p = __MapActor_GetActor(0) + 0x55;
        *p = m & *p;
        p = __MapActor_GetActor(0) + 0x23;
        *p = m & *p;
        OvlFunc_947_20083a8();
        OvlFunc_947_2009d84();
        m = 1;
        p = __MapActor_GetActor(0) + 0x55;
        *p = m | *p;
        p = __MapActor_GetActor(0) + 0x23;
        *p = *p | m;
    }
    __CutsceneEnd();
}

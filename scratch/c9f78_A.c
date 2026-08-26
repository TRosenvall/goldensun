extern char *iwram_3001ebc;
extern unsigned char gState[];
extern int _AREA_75;
extern int _AREA_76;
extern int _AREA_78;
extern void OvlFunc_948_200a188(void);
extern void OvlFunc_948_200a290(void);
extern void OvlFunc_948_200a334(void);

int OvlFunc_948_2009f78(void)
{
    short *a;
    int off;
    char *p;

    p = iwram_3001ebc;
    off = 0xe0 << 1;
    *(int *)(p + off) = 0x81 << 2;
    a = (short *)(gState + off);
    if (*a == (int)&_AREA_75)
        OvlFunc_948_200a188();
    if (*a == (int)&_AREA_76)
        OvlFunc_948_200a290();
    if (*a == (int)&_AREA_78)
        OvlFunc_948_200a334();
    return 0;
}

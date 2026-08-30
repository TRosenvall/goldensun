extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern int _AREA_4d;
extern int _AREA_4f;
extern int _AREA_50;
extern int _AREA_51;
extern int _AREA_52;
extern int _AREA_53;
extern int _AREA_55;
extern int _AREA_56;
extern int _AREA_57;

extern void OvlFunc_932_200a0d0(void);
extern void OvlFunc_932_200a310(void);
extern void OvlFunc_932_200a428(void);
extern void OvlFunc_932_200a490(void);
extern void OvlFunc_932_200a5c0(void);
extern void OvlFunc_932_200a6c0(void);
extern void OvlFunc_932_200a804(void);
extern void OvlFunc_932_200a934(void);
extern void OvlFunc_932_200a9dc(void);

int OvlFunc_932_200a020(void)
{
    unsigned char *p;
    unsigned char *g;
    int o;
    int area;

    p = iwram_3001ebc;
    o = 0xe0 << 1;
    *(int *)(p + o) = 0x81 << 2;
    g = gState;
    area = *(short *)(g + o);
    if (area == (int)&_AREA_4d)
        OvlFunc_932_200a0d0();
    else if (area == (int)&_AREA_4f)
        OvlFunc_932_200a310();
    else if (area == (int)&_AREA_50)
        OvlFunc_932_200a428();
    else if (area == (int)&_AREA_51)
        OvlFunc_932_200a490();
    else if (area == (int)&_AREA_52)
        OvlFunc_932_200a5c0();
    else if (area == (int)&_AREA_53)
        OvlFunc_932_200a6c0();
    else if (area == (int)&_AREA_55)
        OvlFunc_932_200a804();
    else if (area == (int)&_AREA_56)
        OvlFunc_932_200a934();
    else if (area == (int)&_AREA_57)
        OvlFunc_932_200a9dc();
    return 0;
}

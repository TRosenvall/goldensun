extern unsigned char iwram_3001e8c[];
extern void *__MapActor_GetActor(int slot);
extern int OvlFunc_898_2009674(void *a, void *b, int c, int d);

int OvlFunc_898_2008314(void *e)
{
    unsigned char *base;
    char *tbl;
    unsigned short *p;
    int kind;
    int flag;
    void *a;
    void *a1;

    base = *(unsigned char **)iwram_3001e8c;
    tbl = *(char **)(iwram_3001e8c + 0x30);
    p = (unsigned short *)((char *)e + 0x64);
    flag = 0;
    kind = 0x12;
    if (*p & 1)
        a1 = __MapActor_GetActor(0xf);
    else
        a1 = __MapActor_GetActor(0xe);
    if (OvlFunc_898_2009674(e, a1, 0x20, 0) == 0) {
        a = __MapActor_GetActor(0);
        if (*(short *)(tbl + (0xbc << 1)) != 0 || base[0xea4] != 0) {
            kind = 0x1a;
            if (*p & 2)
                flag = 1;
        }
        OvlFunc_898_2009674(e, a, kind, flag);
    }
    return 0;
}

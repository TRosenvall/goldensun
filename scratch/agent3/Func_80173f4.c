extern void *iwram_3001e8c;
extern int UploadSpriteGFX(int slot, unsigned int size, unsigned char *gfx);
extern void StartTask(void *task, int priority);
extern void Func_801789c(void);

void Func_80173f4(void)
{
    unsigned char *r5;
    unsigned short gfxRes;
    unsigned short *pA;
    unsigned short vA;
    unsigned short *pB;
    unsigned short vB;
    unsigned short *pC;
    unsigned short vC;
    unsigned short *pD;
    unsigned short vD;
    unsigned short *pE;
    int prio;

    r5 = (unsigned char *)iwram_3001e8c;
    gfxRes = (unsigned short)UploadSpriteGFX(0x5f, 0x80 << 6, 0);
    *(unsigned short *)(r5 + 0x12b8) = gfxRes;
    pA = (unsigned short *)(r5 + 0x12b0);
    vA = 9;
    *pA = vA;
    pB = (unsigned short *)(r5 + 0xea8);
    vB = 10;
    *pB = vB;
    pC = (unsigned short *)(r5 + 0xeac);
    vC = 0;
    *pC = vC;
    pD = (unsigned short *)(r5 + 0xeae);
    vD = 15;
    *pD = vD;
    pE = (unsigned short *)(r5 + 0x12b2);
    *pE = vC;
    prio = 0xc8 << 4;
    StartTask(Func_801789c, prio);
}

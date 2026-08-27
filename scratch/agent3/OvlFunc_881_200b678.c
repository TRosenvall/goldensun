extern unsigned char gState[];
extern unsigned char iwram_3001ebc[];
extern unsigned char iwram_3001e40[];

extern unsigned char *__MapActor_GetActor(int id);
extern int  __GetFlagByte(int id);
extern void __SetFlagByte(int id, int v);
extern int  __GetFlag(int id);

void OvlFunc_881_200b678(void)
{
    unsigned char *gs;
    unsigned char *actor;
    unsigned char *base;
    unsigned char *p;
    unsigned int off;
    int t;
    int v;

    gs = gState;
    gs += 0xfa << 1;
    actor = __MapActor_GetActor(*(int *)gs);
    base = *(unsigned char **)iwram_3001ebc;
    t = *(int *)iwram_3001e40;
    t <<= 12;
    *(short *)(actor + 6) = t;
    v = __GetFlagByte(0xbe << 2);
    if (v != 0) {
        if (v == 1) {
            off = 0xc1 << 1;
            p = base + off;
            off = 0x63;
            *(short *)p = off;
        } else if (__GetFlag(0x83 << 1) == 0) {
            v--;
        }
    }
    __SetFlagByte(0xbe << 2, v);
}

extern void *__MapActor_GetActor(int slot);
extern int __GetFlagByte(int id);
extern void __SetFlagByte(int id, int v);
extern int __GetFlag(int id);
extern unsigned char gState;
extern unsigned char iwram_3001ebc[];
extern unsigned char iwram_3001e40[];

void OvlFunc_960_2008400(void)
{
    unsigned char *gs;
    unsigned char *actor;
    unsigned char *base;
    int t;
    int v;
    short *p;
    int k;

    k = 0x63;
    gs = (unsigned char *)&gState;
    gs += (0xfa << 1);
    actor = (unsigned char *)__MapActor_GetActor(*(int *)gs);
    base = *(unsigned char **)iwram_3001ebc;
    t = *(int *)iwram_3001e40;
    t <<= 12;
    *(short *)(actor + 6) = t;
    v = __GetFlagByte(0x84 << 2);
    if (v != 0) {
        if (v == 1) {
            p = (short *)(base + (0xc1 << 1));
            *p = k;
        } else if (__GetFlag(0x83 << 1) == 0) {
            v--;
        }
    }
    __SetFlagByte(0x84 << 2, v);
}

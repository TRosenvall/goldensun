struct Actor {
    unsigned char pad00[6];
    unsigned short facing;
    int x;
    unsigned char pad0c[4];
    int z;
};

extern char *iwram_3001ebc;
extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void OvlFunc_959_2008e80(void);

void OvlFunc_959_2009a44(void)
{
    struct Actor *a;
    char *p;
    int x;
    int z;
    int v;

    v = 0x28;
    a = __MapActor_GetActor(0);
    if (__GetFlag(0xd6 << 2) == 0) {
        x = a->x / 0x100000;
        z = a->z / 0x100000;
        if (x == 0x10 && z > 0x37 && z <= 0x3a
            && (a->facing == (0xc0 << 8) || a->facing == (0x80 << 7))) {
            p = iwram_3001ebc;
            *(short *)(p + (0xb6 << 1)) = v;
            OvlFunc_959_2008e80();
        }
    }
}

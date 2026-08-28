extern int *GetFieldActor(int);
extern int atan2(int, int);
extern void vec3_translate(int, int, int *);
extern unsigned char *iwram_3001ebc;
typedef struct { unsigned char pad[0x1f4]; int id; } GlobalState;
extern GlobalState gState;

void Func_808ee0c(void)
{
    unsigned char *e;
    int *act;
    int i;
    int ax, ay;
    int u, v;
    int ang;

    act = GetFieldActor(gState.id);
    e = iwram_3001ebc + 0x11c;
    i = 0;
    if (e[4] != 0) {
        ax = act[8 / 4];
        ay = act[0x10 / 4];
        do {
            u = (ax - (e[6] << 20)) - 0x80000;
            v = (ay - (e[7] << 20)) - 0x80000;
            if ((unsigned int)(u + 0xfffff) <= 0x1ffffe && (unsigned int)(v + 0xfffff) <= 0x1ffffe) {
                act[8 / 4] = (e[6] << 20) + 0x80000;
                act[0x10 / 4] = (e[7] << 20) + 0x80000;
                ang = atan2(v, u);
                vec3_translate(0x140000, (unsigned short)ang, act + 2);
                act[0x38 / 4] = 0x80000000;
                act[0x3c / 4] = 0x80000000;
                act[0x40 / 4] = 0x80000000;
                break;
            }
            i++;
            e += 8;
            if (i > 9)
                break;
        } while (e[4] != 0);
    }
}

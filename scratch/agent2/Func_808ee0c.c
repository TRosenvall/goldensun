typedef unsigned char u8;

struct Ent {
    u8 pad0[4];
    u8 b4;
    u8 pad5;
    u8 b6;
    u8 b7;
};

extern u8 gState[];
extern u8 *iwram_3001ebc;
extern int *GetFieldActor(int id);
extern int atan2(int y, int x);
extern void vec3_translate(int a, int b, int *v);

void Func_808ee0c(void)
{
    u8 *g;
    int *p;
    struct Ent *e;
    int i;
    int X;
    int Y;
    int a;
    int b;
    int dx;
    int dy;

    g = gState;
    p = GetFieldActor(*(int *)(g + 0x1f4));
    e = (struct Ent *)(iwram_3001ebc + 0x11c);
    i = 0;
    if (e->b4 != 0) {
        X = p[2];
        Y = p[4];
        do {
            dx = X - (e->b6 << 20);
            a = dx - 0x80000;
            dy = Y - (e->b7 << 20);
            b = dy - 0x80000;
            if (a >= -0xfffff && a <= 0xfffff && b >= -0xfffff && b <= 0xfffff) {
                p[2] = (e->b6 << 20) + 0x80000;
                p[4] = (e->b7 << 20) + 0x80000;
                vec3_translate(0x140000, (unsigned short)atan2(b, a), p + 2);
                p[14] = 0x80000000;
                p[15] = 0x80000000;
                p[16] = 0x80000000;
                break;
            }
            i++;
            e++;
        } while (i <= 9 && e->b4 != 0);
    }
}

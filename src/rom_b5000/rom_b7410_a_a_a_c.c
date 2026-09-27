/* Func_80b7424  --  0x080b7424, was asm/rom_b5000/rom_b7410_a_a_a_c.s (this
 * function alone), so it converts whole. Matched from scratch.
 */
extern unsigned char *_GetUnit(int id);
extern int Func_80c23c0(int a);

void Func_80b7424(unsigned short *ids, int n, int *a, int *b)
{
    unsigned int w;
    int x;
    int i;
    unsigned char *u;

    w = 0x1e;
    if (n > 4)
        w = 0x1b;
    x = (int)((n - 1) * w) / 2;
    for (i = 0; i != n; i++) {
        a[i] = -0x50;
        w = 0;
        if (i != 0) {
            w = 0x19;
            if ((unsigned short)(ids[i] - 0xfe) > 1) {
                u = _GetUnit(ids[i]);
                if (Func_80c23c0(u[0x128]) != 0)
                    w = 0x1b;
                else
                    w = 0x26;
                if (u[0x128] == 0x94 || u[0x128] == 0x79)
                    a[i] = -0x32;
            }
        }
        x -= w >> 1;
        b[i] = x;
        w = 0x19;
        if ((unsigned short)(ids[i] - 0xfe) > 1) {
            if (Func_80c23c0(_GetUnit(ids[i])[0x128]) != 0)
                w = 0x1b;
            else
                w = 0x26;
        }
        x -= w >> 1;
    }
}

typedef unsigned short u16;

extern void *GetUnit(int id);
extern void CalcStats(int id);

int Func_80788c4(int id, int slot)
{
    u16 *u;
    u16 *arr;
    u16 *rd;
    u16 *wr;
    int i;
    int n;
    int v;
    u16 t;
    int ret;

    u = (u16 *)GetUnit(id);
    slot = slot * 2 + 0xd8;
    v = *(u16 *)((char *)u + slot);
    ret = -1;
    if (v != 0) {
        if ((v & 0xf800) != 0) {
            *(u16 *)((char *)u + slot) = v - 0x800;
            ret = 1;
        } else {
            arr = (u16 *)((char *)u + 0xd8);
            *(u16 *)((char *)u + slot) = 0;
            rd = arr;
            n = 0;
            wr = arr;
            for (i = 0xe; i >= 0; i--) {
                t = *rd;
                rd++;
                if (t != 0) {
                    *wr = t;
                    n++;
                    wr++;
                }
            }
            for (i = n; i < 0xf; i++)
                arr[i] = 0;
            ret = 2;
        }
    }
    CalcStats(id);
    return ret;
}

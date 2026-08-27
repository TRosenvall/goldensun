extern int GetMapActorIndex();
extern unsigned int iwram_3001ebc;
extern int _Func_8011f54(int a, int b, int c);

void Func_808edac(int unused, int v1, int v2)
{
    int m;
    int res;
    unsigned char *base;
    int *obj;
    int t;
    int r;

    res = GetMapActorIndex();
    m = -1;
    if (res != m) {
        base = (unsigned char *)(*(unsigned int *)&iwram_3001ebc + (res << 3) + 0x11c);
        obj = *(int **)base;
        if (obj != 0) {
            if (v1 == m) {
                t = base[6];
                t <<= 20;
                v1 = t + 0x80000;
            }
            if (v2 == m) {
                t = base[7];
                t <<= 20;
                v2 = t + 0x80000;
            }
            obj[2] = v1;
            obj[4] = v2;
            r = _Func_8011f54(0, v1, v2);
            obj[5] = r;
            obj[3] = r;
        }
    }
}

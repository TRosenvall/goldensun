extern int GetMapActorIndex();
extern unsigned int iwram_3001ebc;
extern int _Func_8011f54(int a, int b, int c);

void Func_808edac(int unused, int v1, int v2)
{
    int res;
    unsigned char *base;
    int *obj;
    int r;

    res = GetMapActorIndex();
    if (res != -1) {
        base = (unsigned char *)(*(unsigned int *)&iwram_3001ebc + (res << 3) + 0x11c);
        obj = *(int **)base;
        if (obj != 0) {
            if (v1 == -1)
                v1 = (base[6] << 20) + 0x80000;
            if (v2 == -1)
                v2 = (base[7] << 20) + 0x80000;
            obj[2] = v1;
            obj[4] = v2;
            r = _Func_8011f54(0, v1, v2);
            obj[5] = r;
            obj[3] = r;
        }
    }
}

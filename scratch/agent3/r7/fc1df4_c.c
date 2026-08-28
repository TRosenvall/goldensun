extern unsigned char *iwram_3001e74;

int Func_80c1df4(int key)
{
    unsigned char *g;
    unsigned char *cnt;
    int i, n, j, k, off;

    g = iwram_3001e74;
    cnt = g + 0x40;
    n = *cnt;
    i = 0;
    if (i < n && *(unsigned short *)(g + 0x10) != key) {
        unsigned short *q = (unsigned short *)(g + 0x10);
        do {
            i++;
            if (i >= n)
                break;
            q++;
        } while (*q != key);
    }
    if (i != n) {
        k = i + 0x34;
        j = 0;
        if (*(signed char *)(g + k) < 0) {
            int a = i * 4 + 0x1c;
            *(signed char *)(g + k) = 1;
            *(int *)(g + a) = 3;
            return 0x8001;
        }
        off = i * 4;
        while (j <= 0x1f) {
            int a = off + 0x1c;
            *(signed char *)(g + k) = (*(signed char *)(g + k) + 1) % 9;
            if ((*(int *)(g + a) & (1 << *(signed char *)(g + k))) == 0)
                break;
            j++;
        }
        {
            int a = off + 0x1c;
            *(int *)(g + a) = *(int *)(g + a) | (1 << *(signed char *)(g + k));
        }
        return *(signed char *)(g + k);
    }
    if (n > 4)
        return -1;
    {
        int m = -1;
        *(signed char *)(g + (n + 0x34)) = m;
    }
    *(unsigned short *)(g + (n * 2 + 0x10)) = key;
    *(int *)(g + (n * 4 + 0x1c)) = 0;
    *cnt = n + 1;
    return 9;
}

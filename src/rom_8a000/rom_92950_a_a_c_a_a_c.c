void Func_80929d8(char *a, int v)
{
    char *o;
    int cnt;
    int n;
    char **p;
    char *e;

    if ((*(unsigned char *)(a + 0x54) & 0xf) == 1) {
        o = *(char **)(a + 0x50);
        cnt = *(unsigned char *)(o + 0x27);
        if (cnt != 0) {
            p = (char **)(o + 0x28);
            n = cnt;
            do {
                e = *p++;
                if (e != 0 && *(int *)(e + 0x10) != 0)
                    *(unsigned char *)(e + 5) = v;
                n--;
            } while (n != 0);
        }
        *(unsigned char *)(o + 0x25) = 1;
    }
}

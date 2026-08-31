extern unsigned int iwram_3001e40;
extern unsigned char L9ed80[] __asm__(".L9ed80");

void Func_8092980(char *a)
{
    char *o;
    unsigned char v;
    int cnt;
    int n;
    char **p;
    char *e;

    if ((*(unsigned char *)(a + 0x54) & 0xf) == 1) {
        o = *(char **)(a + 0x50);
        v = L9ed80[(iwram_3001e40 >> 1) & 3];
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

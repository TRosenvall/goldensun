extern unsigned char *GetEnemyInfo(int id);
extern unsigned char *GetClassInfo(int id);

int Func_8079e9c(unsigned char *rec, int needle)
{
    unsigned char *p;
    int i;

    if (rec[0x129] == 0) {
        p = GetEnemyInfo(rec[0x128]) + 0x48;
        for (i = 0; i <= 2; i++) {
            if (*p == needle)
                return 1;
            p++;
        }
    } else {
        p = GetClassInfo(rec[0x129]) + 0x50;
        for (i = 0; i <= 2; i++) {
            if (*p++ == needle)
                return 1;
        }
    }
    return 0;
}

extern unsigned char *GetItemInfo(int id);

int CheckEquipmentCritBoost(int rec)
{
    unsigned char *p;
    unsigned char *q;
    int i;
    int j;
    int total;
    int rc;

    rc = rec;
    total = 0;
    p = (unsigned char *)0xd8;
    i = 0xe;
    do {
        if (*(unsigned short *)(p + rc) & (0x80 << 2)) {
            q = GetItemInfo(*(unsigned short *)(p + rc)) + 0x18;
            for (j = 3; j >= 0; j--) {
                if (*q == 0x17)
                    total += *(signed char *)(q + 1);
                q += 4;
            }
        }
        p += 2;
        i--;
    } while (i >= 0);
    if (total < 0)
        rc = rec;
    total = 0;
    return total;
}

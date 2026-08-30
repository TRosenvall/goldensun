extern unsigned int iwram_3001e40;

void Func_8096b88(unsigned char *e)
{
    unsigned char *o;
    unsigned char **list;
    unsigned char *q;
    int n;

    if (*(unsigned char *)(e + 0x54) == 1) {
    o = *(unsigned char **)(e + 0x50);
    if (o != 0 && (o[0x1d] & 1) == 0) {
    if (o[0x27] != 0) {
        n = o[0x27];
        list = (unsigned char **)(o + 0x28);
        do {
            q = *list++;
            q[5] = iwram_3001e40 % 6;
            n--;
        } while (n != 0);
    }
    o[0x25] = 1;
    }
    }
}

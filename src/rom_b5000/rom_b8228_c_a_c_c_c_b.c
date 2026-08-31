extern char *iwram_3001e74;

int Func_80b9a70(int key)
{
    char *b;
    int i;
    int off;
    int flag;
    short v;

    b = iwram_3001e74;
    if ((unsigned int)key <= 7) {
        flag = 0x80;
        i = 0;
        flag <<= 1;
        off = 0x58;
        for (;;) {
            v = *(short *)(off + (int)b);
            if (v == 0xff)
                return -1;
            if (v != 0xfe && v == key)
                return i | flag;
            off += 2;
            i++;
        }
    } else {
        flag = 0xc0;
        i = 0;
        b += 2;
        flag <<= 1;
        off = 0x64;
        for (;;) {
            v = *(short *)(off + (int)b);
            if (v == 0xff)
                return -1;
            if (v != 0xfe && v == key)
                return i | flag;
            off += 2;
            i++;
        }
    }
}

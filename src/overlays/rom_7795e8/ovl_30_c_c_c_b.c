unsigned int OvlFunc_880_20092c8(unsigned int n, unsigned char *p)
{
    unsigned int crc;
    unsigned int i;
    int j;

    crc = 0xffff;
    i = 0;
    if (n != 0) {
        do {
            crc ^= p[0] << 8;
            j = 0;
            do {
                if ((crc & 0x8000) != 0)
                    crc = (crc << 1) - 0x1021;
                else
                    crc = crc << 1;
                j++;
            } while (j != 8);
            i++;
            p++;
        } while (i != n);
    }
    return (unsigned short)~crc;
}

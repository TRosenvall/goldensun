void Func_80b09fc(unsigned char *rec, int rows, int cursor, int flag)
{
    unsigned char *list;
    int v;
    int z;

    list = *(unsigned char **)rec;
    v = *(unsigned short *)(list + 6);
    z = 0;
    *(short *)(rec + 4) = v;
    v = *(unsigned short *)(list + 8);
    *(short *)(rec + 8) = rows;
    *(short *)(rec + 6) = v;
    *(short *)(rec + 0xa) = cursor;
    rec[0xd] = flag;
    rec[0xc] = z;
}

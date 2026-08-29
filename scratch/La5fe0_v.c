extern unsigned char *iwram_3001f2c;
extern unsigned char *_GetMoveInfo(int id);
extern int _Func_808e96c(int a);

int Func_80a5fe0(void)
{
    unsigned char *rec;
    unsigned int p;
    int id;
    int v;

    p = (unsigned int)iwram_3001f2c;
    p += 0xbc << 1;
    id = 0x3fff & *(unsigned short *)p;
    rec = _GetMoveInfo(id);
    if (_Func_808e96c(rec[0xc]) != 0)
        return 0;
    if (rec[8] == 0xff)
        return 2;
    v = rec[0] ^ 2;
    return 1 - (v != 0);
}

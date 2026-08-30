extern unsigned char *iwram_3001e8c;
extern int _CONST_f;

void SetTextColor(int c)
{
    unsigned char *p;
    int off;

    p = iwram_3001e8c;
    c = c & (int)&_CONST_f;
    off = 0xeae;
    p = p + off;
    *(short *)p = c;
}

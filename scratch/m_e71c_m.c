extern unsigned char *iwram_3001e8c;
extern int _CONST_f;

void SetTextColor(int c)
{
    unsigned char *p;
    int off;
    int m;

    p = iwram_3001e8c;
    m = (int)&_CONST_f;
    c = c & m;
    off = 0xeae;
    p = p + off;
    *(short *)p = c;
}

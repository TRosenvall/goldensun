extern unsigned char *iwram_3001e8c;
extern int _CONST_f;

void SetTextColor(int c)
{
    unsigned char *p;
    int off;

    p = iwram_3001e8c;
    c = c & (int)&_CONST_f;
    p = p + 0xeae;
    *(short *)p = c;
}

struct Box {
    int f0;
    unsigned short f4;
    unsigned short f6;
    unsigned short f8;
    unsigned short fa;
};

extern unsigned char *iwram_3001e8c;
extern struct Box *galloc_ewram(int a, int b);
extern void Func_801eea0(int a);
extern void Func_801f200(int a);
extern int CreateUIBox(int a, int b, int c, int d, int e);

void Func_801ef08(int arg)
{
    struct Box *p;
    unsigned char *flag;
    int zero;

    p = galloc_ewram(0x10, 0x10);
    flag = iwram_3001e8c + 0xea6;
    zero = 0;
    *flag = 1;
    Func_801eea0(arg);
    p->f0 = CreateUIBox(p->f4, p->f6, p->f8, p->fa, 6);
    Func_801f200(arg);
    *flag = zero;
}

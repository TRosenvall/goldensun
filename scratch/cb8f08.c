struct X { unsigned char pad00[0xa]; short fa; };
struct U { unsigned char pad00[0x38]; short f38; };

extern struct U *_GetUnit(int id);
extern int Func_80b6b40(int kind, short *buf);
extern unsigned int Random(void);

int Func_80b8f08(struct X *x)
{
    short buf[14];
    int id;
    int n;

    id = x->fa;
    if (_GetUnit(id)->f38 != 0)
        return id;
    n = Func_80b6b40(id > 0x7f ? 2 : 1, buf);
    if (n == 0)
        return 0x80 << 1;
    return *(short *)((char *)buf + ((n * Random() >> 16) << 1));
}

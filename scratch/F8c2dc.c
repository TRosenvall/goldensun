extern unsigned char gState[];
extern int _GetPartySize(void);
extern void _ModifyPP(int member, int arg);

void Func_808c2dc(int arg)
{
    unsigned char *p;
    int n;

    n = _GetPartySize();
    if (n > 0) {
        p = gState + (0xfc << 1);
        do {
            _ModifyPP(*p++, arg);
            n--;
        } while (n != 0);
    }
}

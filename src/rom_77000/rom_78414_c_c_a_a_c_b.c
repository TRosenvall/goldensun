extern int Func_80796c4(short *buf);
extern int GiveItemTo(int item, int who);

int GiveItem(int who)
{
    short buf[10];
    short *p;
    int n;
    int i;
    int v;

    n = Func_80796c4(buf);
    p = buf;
    for (i = 0; i < n; i++) {
        v = *p++;
        if (GiveItemTo(v, who) >= 0)
            return v;
    }
    return -1;
}

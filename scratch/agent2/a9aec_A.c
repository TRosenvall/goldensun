extern int _MSG_182;

extern unsigned char *_GetItemInfo(int id);
extern void _Func_801e7c0(int msg, int win, int x, int y);

void Func_80a9aec(int win, unsigned short *list)
{
    int base;
    int i;
    int v;
    int id;
    int m;
    unsigned char *info;

    base = (int)&_MSG_182;
    i = 0xe;
loop:
    v = *list;
    m = 0x200;
    m &= v;
    list++;
    if (m != 0) {
        id = 0x1ff;
        id &= v;
        info = _GetItemInfo(id);
        switch (info[2]) {
        case 1:
            _Func_801e7c0(id + base, win, 8, 8);
            break;
        case 2:
            _Func_801e7c0(id + base, win, 8, 0x38);
            break;
        case 3:
            _Func_801e7c0(id + base, win, 8, 0x28);
            break;
        case 4:
            _Func_801e7c0(id + base, win, 8, 0x18);
            break;
        }
    }
    i--;
    if (i >= 0)
        goto loop;
}

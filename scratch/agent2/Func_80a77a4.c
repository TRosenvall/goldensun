typedef unsigned char u8;
typedef unsigned short u16;

extern unsigned int iwram_3001f2c;
extern void _Func_8016498(void *w);
extern int _GetFlag(int id);
extern void _Func_801e41c(void *w, int a, int b, int c, int d);
extern void Func_80a1ac0(int x, int y);
extern int Func_80a7d68(void);
extern int Func_80a7a34(void);
extern void Func_80a17c4(void *n);
extern void WaitFrames(int n);

int Func_80a77a4(int which)
{
    u8 *s;
    u8 *node;
    int off;
    int sel;
    int r;

    off = 0x1c + which;
    which <<= 2;
    s = (u8 *)iwram_3001f2c;
    node = *(u8 **)(s + (which + 0x14));
    node[5] = 1;
    *(u16 *)(node + 0xc) = 0;
    sel = *(signed char *)(s + off);
    _Func_8016498(*(void **)(s + 0x10));
    if (_GetFlag(0xb9 * 2) != 0)
        _Func_801e41c(*(void **)(s + 0x10), 9, 1, 9, 3);
    if (sel == -1)
        s[off] = 0;
    else
        Func_80a1ac0(((sel * 2 + sel) << 3) - 0xa, 0x10);
    if (*(u16 *)(s + 0x88 * 4) == 3)
        r = Func_80a7d68();
    else
        r = Func_80a7a34();
    Func_80a17c4(*(void **)(s + (which + 0x14)));
    WaitFrames(1);
    return r;
}

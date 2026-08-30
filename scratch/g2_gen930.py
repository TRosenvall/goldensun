import itertools, os
HDR = """extern unsigned char *iwram_3001ebc;
extern void *GetFieldActor(int slot);
extern int _Func_8017658(int msg, int x, int y, int flag);
extern int _Func_8017394(int handle);
extern void WaitFrames(int n);

void Func_80930bc(int packed)
{
%(decls)s
    s = iwram_3001ebc;
    slot = packed & 0xfff;
    GetFieldActor(slot);
    *(int *)(s + (0xfa << 1)) = slot;
    if (*(int *)(s + (0xe6 << 1)) == 0) {
%(body)s
        handle = _Func_8017658(*(short *)(s + (0xec << 1)), %(xa)s, %(ya)s, 1);
        *(int *)(s + (0xfc << 1)) = handle;
        while (_Func_8017394(handle) == 0)
            WaitFrames(1);
    }
%(tail)s}
"""
DECLS = {
 'plain': "    unsigned char *s;\n    int slot;\n    int x;\n    int y;\n    int handle;\n",
 'copy':  "    unsigned char *s;\n    int slot;\n    int x;\n    int y;\n    int handle;\n    int xx;\n    int yy;\n",
 'copyfirst': "    int x;\n    int y;\n    int xx;\n    int yy;\n    unsigned char *s;\n    int slot;\n    int handle;\n",
}
BODY_DIRECT = """        if (y > 0x77)
            y += 0x20;
        else
            y -= 0x20;
        x = x < 8 ? 8 : x;
        if (x > (0x9c << 1))
            x = 0x9c << 1;
        y = y < 0x14 ? 0x14 : y;
        if (y > 0xdc)
            y = 0xdc;
"""
BODY_COPY = """        yy = y;
        xx = x;
        if (yy > 0x77)
            yy += 0x20;
        else
            yy -= 0x20;
        xx = xx < 8 ? 8 : xx;
        if (xx > (0x9c << 1))
            xx = 0x9c << 1;
        yy = yy < 0x14 ? 0x14 : yy;
        if (yy > 0xdc)
            yy = 0xdc;
"""
BODY_COPY2 = """        yy = y > 0x77 ? y + 0x20 : y - 0x20;
        xx = x < 8 ? 8 : x;
        if (xx > (0x9c << 1))
            xx = 0x9c << 1;
        yy = yy < 0x14 ? 0x14 : yy;
        if (yy > 0xdc)
            yy = 0xdc;
"""
TAIL_INLINE = "    *(unsigned short *)(s + (0xec << 1)) += 1;\n"
TAIL_PTR = "    m = (unsigned short *)(s + (0xec << 1));\n    *m += 1;\n"

combos = {
 'v1': ('plain', BODY_DIRECT, 'x', 'y', TAIL_INLINE),
 'v2': ('copy', BODY_COPY, 'xx', 'yy', TAIL_INLINE),
 'v3': ('copy', BODY_COPY, 'xx', 'yy', TAIL_PTR),
 'v4': ('copy', BODY_COPY2, 'xx', 'yy', TAIL_INLINE),
 'v5': ('copyfirst', BODY_COPY, 'xx', 'yy', TAIL_INLINE),
 'v6': ('copyfirst', BODY_COPY2, 'xx', 'yy', TAIL_PTR),
}
for name,(d,b,xa,ya,t) in combos.items():
    decls = DECLS[d]
    if 'm = ' in t:
        decls += "    unsigned short *m;\n"
    open('scratch/g2_930_%s.c'%name,'w').write(HDR % dict(decls=decls, body=b, xa=xa, ya=ya, tail=t))
print("wrote")

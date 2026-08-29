typedef unsigned short u16;

extern void CopyMapTiles(int sx, int sy, int dx, int dy, int w, int h);
extern void WaitFrames(int n);

void Func_8010560(short *p, int dx, int dy)
{
    int sy, w, h, d;
    int sx;

    sx = *(u16 *)p;
    while (sx != 0xffff) {
        sy = p[1];
        w = p[2];
        h = p[3];
        d = p[4];
        CopyMapTiles(sx, sy & 0xffff, dx, dy, w & 0xffff, h & 0xffff);
        WaitFrames(d & 0xffff);
        p += 5;
        sx = *(u16 *)p;
    }
}

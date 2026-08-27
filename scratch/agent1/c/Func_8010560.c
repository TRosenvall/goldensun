typedef unsigned short u16;

extern void CopyMapTiles(u16 sx, u16 sy, int dx, int dy, u16 w, u16 h);
extern void WaitFrames(u16 n);

void Func_8010560(u16 *p, int dx, int dy)
{
    short sy, w, h, d;
    int sx;

    sx = p[0];
    while (sx != 0xffff) {
        sy = p[1];
        w = p[2];
        h = p[3];
        d = p[4];
        CopyMapTiles(sx, sy, dx, dy, w, h);
        WaitFrames(d);
        p += 5;
        sx = p[0];
    }
}

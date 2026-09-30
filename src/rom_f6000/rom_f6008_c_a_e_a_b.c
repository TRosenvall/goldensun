extern unsigned char *iwram_3001ef0;

void Func_80f62b8(int x0, int y0, int x1, int y1, int col)
{
    unsigned char *fb;
    int dx, dy, adx, ady, step, acc, t;
    unsigned int num;
    unsigned int dt;
    int o;

    dx = x1 - x0;
    dy = y1 - y0;
    acc = 0x80;
    fb = iwram_3001ef0;
    adx = dx;
    if (dx < 0)
        adx = -dx;
    ady = dy;
    if (dy < 0)
        ady = -dy;
    if (adx < ady) {
        if (dy < 0) {
            t = x0;
            x0 = x1;
            x1 = t;
            t = y0;
            y0 = y1;
            y1 = t;
            dx = x1 - x0;
            dy = y1 - y0;
        }
        num = dx << 8;
        if (dx < 0)
            num = (x0 - x1) << 8;
        if (dy >= 0)
            step = (int)num / dy;
        else
            step = (int)num / (y0 - y1);
        num = y0;
        dt = x0;
        if (num != y1) {
            do {
                o = (((num >> 3) * 32 + (dt >> 3)) * 8 + (num & 7)) * 8 + (dt & 7);
                if (fb[o] < col)
                    fb[o] = col;
                acc += step;
                if (acc & 0x100) {
                    if (dx > 0)
                        dt++;
                    else
                        dt--;
                    acc &= ~0x100;
                }
                num++;
            } while (num != y1);
        }
    } else {
        if (dx < 0) {
            t = x0;
            x0 = x1;
            x1 = t;
            t = y0;
            y0 = y1;
            y1 = t;
            dx = x1 - x0;
            dy = y1 - y0;
        }
        num = dy << 8;
        if (dy < 0)
            num = (y0 - y1) << 8;
        step = (int)num / (dx >= 0 ? x1 - x0 : x0 - x1);
        num = x0;
        dt = y0;
        if (num != x1) {
            do {
                o = (((dt >> 3) * 32 + (num >> 3)) * 8 + (dt & 7)) * 8 + (num & 7);
                if (fb[o] < col)
                    fb[o] = col;
                acc += step;
                if (acc & 0x100) {
                    if (dy > 0)
                        dt++;
                    else
                        dt--;
                    acc &= ~0x100;
                }
                num++;
            } while (num != x1);
        }
    }
}

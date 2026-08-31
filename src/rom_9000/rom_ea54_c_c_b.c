int Func_800eba0(int *a, int ra, int *b, int rb)
{
    int dx;
    int dy;
    int dz;
    int r;
    int lim;

    dx = (*a++ - *b++) >> 16;
    dy = (*a++ - *b++) >> 16;
    dz = (*a - *b) >> 16;
    r = ra + rb;
    lim = 0x80 << 15;
    if (dx > lim)
        return -1;
    if (dz > lim)
        return -1;
    if (dx * dx + dy * dy + dz * dz < r * r)
        return 0;
    return -1;
}

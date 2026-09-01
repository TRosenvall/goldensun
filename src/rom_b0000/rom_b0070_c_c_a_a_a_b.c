extern void *_GetUnit(int id);

int Func_80b27b0(int id, int kind)
{
    unsigned char *u;
    int r;

    u = (unsigned char *)_GetUnit(id);
    r = 0;
    if (kind == 0) {
        if (*(short *)((char *)u + 0x38 + (unsigned int)0) <= 0)
            goto set;
    }
    if (kind == 1) {
        if (*(signed char *)(u + 0x131) != 0)
            goto set;
    }
    if (kind == 2) {
        if (u[0xa0 << 1] != 0)
            goto set;
    }
    if (kind == 3) {
        if (*(signed char *)(u + (0x98 << 1)) == 0)
            goto out;
        goto set;
    }
    goto out;
set:
    r = 1;
out:
    return r;
}

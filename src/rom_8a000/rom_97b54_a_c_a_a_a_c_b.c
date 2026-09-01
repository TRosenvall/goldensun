extern void _Actor_WaitMovement(unsigned char *a);

void Func_8098184(unsigned char *a)
{
    int v;
    int w;

    if (a == 0)
        return;
    v = *(int *)(a + 0x18);
    if (v <= 0xffff) {
        do {
            w = v + (0x80 << 5);
            v = w;
        } while (w <= 0xffff);
        *(int *)(a + 0x18) = w;
        *(int *)(a + 0x1c) = w;
    }
    _Actor_WaitMovement(a);
}

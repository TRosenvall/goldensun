extern void __DeleteActor(unsigned char *a);

void OvlFunc_936_200b6f8(unsigned char *a)
{
    unsigned char *q;
    int s;
    int vx;
    int vy;
    int z;

    q = a + 0x64;
    s = *(short *)q;
    if (s == 0) {
        __DeleteActor(a);
    } else if (s == 1) {
        z = 0;
        *(int *)(a + 0x24) = z;
        *(int *)(a + 0x28) = z;
        *(int *)(a + 8) = z;
        *(int *)(a + 0xc) = z;
    } else {
        *(int *)(a + 0x18) += 0x80 << 4;
        *(int *)(a + 0x1c) += 0x80 << 4;
    }
    *(int *)(a + 8) += *(int *)(a + 0x24);
    vx = *(int *)(a + 0x24);
    vy = *(int *)(a + 0x28);
    *(int *)(a + 0xc) += vy;
    *(int *)(a + 0x24) = vx - vx / 0x100;
    *(int *)(a + 0x28) = vy - vy / 0x10;
    *(short *)q -= 1;
}

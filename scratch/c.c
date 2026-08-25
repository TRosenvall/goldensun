extern void __WaitFrames(int n);

void OvlFunc_964_2009038(void *actor)
{
    unsigned char *a;
    int i;
    int v;

    a = (unsigned char *)actor;
    i = 0x3c;
    while (i != 0) {
        __WaitFrames(1);
        v = *(int *)(a + 0x14);
        i--;
        if (*(int *)(a + 0xc) <= v)
            goto done;
    }
    v = *(int *)(a + 0x14);
done:
    *(int *)(a + 0x28) = 0;
    *(int *)(a + 0xc) = v;
    *(int *)(a + 0x3c) = 0x80 << 24;
}

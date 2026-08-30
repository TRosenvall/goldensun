struct E {
    unsigned char pad[2];
    unsigned char b0 : 1;
    unsigned char mid : 4;
    unsigned char hi : 3;
    unsigned char pad3[5];
};

extern struct E Lc7420[] __asm__(".Lc7420");

int Func_80c23e8(unsigned int id)
{
    int v;

    if (id > 0xab)
        return 1;
    v = Lc7420[id].mid;
    if (v != 0)
        return v;
    return 1;
}

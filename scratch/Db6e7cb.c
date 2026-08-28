extern unsigned short Lc593c[] __asm__(".Lc593c");

int GetWeaponType(int id)
{
    unsigned short *t;
    int i;
    int v;

    t = Lc593c;
    i = 0;
    do {
        v = t[i];
        if (id == (v & 0x1ff))
            return t[i] >> 9;
        i++;
    } while ((short)v != -1);
    return 6;
}

struct Model { unsigned char pad0[0x28]; short *f28; };
struct Ent { unsigned char pad0[0x50]; struct Model *f50; unsigned char f54; };

extern unsigned char iwram_3001ebc[];

int Func_8092be0(int id)
{
    char *base;
    struct Ent *e;
    int i;
    int off;
    int found;

    base = *(char **)iwram_3001ebc;
    found = -1;
    for (i = 8; i <= 0x41; i++) {
        off = i * 4 + 0x14;
        e = *(struct Ent **)(base + off);
        if (e == 0)
            continue;
        if (e->f54 != 1)
            continue;
        if (*e->f50->f28 != id)
            continue;
        found = i;
        break;
    }
    return found;
}

struct Slot {
    unsigned char pad00[0xa];
    short flag;
    unsigned char pad0c[0x34 - 0xc];
};

struct Menu {
    unsigned char pad000[0x400];
    struct Slot slot[7];
    unsigned char pad56c[0x574 - 0x56c];
    short tail[5];
};

void Func_801c9c8(struct Menu *m)
{
    struct Slot *s;

    s = m->slot;
    s->flag = 0; s++;
    s->flag = 0; s++;
    s->flag = 0; s++;
    s->flag = 0; s++;
    s->flag = 0; s++;
    s->flag = 0; s++;
    s->flag = 0;
    m->tail[0] = 0;
    m->tail[1] = 0;
    m->tail[2] = 0;
    m->tail[3] = 0;
    m->tail[4] = 0;
}

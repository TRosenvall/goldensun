extern unsigned char *iwram_3001e8c;
extern unsigned int gState;
extern unsigned char L9fc28[] __asm__(".L9fc28");
extern int Func_8092ba8(int slot);
extern int GetSpriteVoice(int a);

void Func_8093304(unsigned int slot)
{
    unsigned int base;
    short *q;
    unsigned int off;
    unsigned int r1;
    unsigned int r3;
    int value;
    int id;

    base = (unsigned int)iwram_3001e8c;
    if (slot == 0x80000000) {
        off = 0x12f6;
        q = (short *)(base + 0x12f4);
        value = 0;
        *q = value;
    } else {
        id = GetSpriteVoice(Func_8092ba8(slot));
        r3 = (unsigned int)&gState;
        r1 = 0x83;
        r1 <<= 2;
        r3 += r1;
        value = L9fc28[*(unsigned char *)r3];
        off = 0x12f4;
        q = (short *)(base + off);
        off += 2;
        *q = id;
    }
    r3 = (unsigned int)base;
    r3 += off;
    *(short *)r3 = value;
}

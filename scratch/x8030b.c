extern int __Random(void);
extern unsigned int _umodsi3_RAM(unsigned int a, unsigned int b);

void OvlFunc_921_2008030(unsigned char *e)
{
    short *c;
    int v;

    c = (short *)(e + 0x64);
    switch (*c) {
    case 2:
        *(int *)(e + 0x18) += 0x80 << 5;
        *(int *)(e + 0x1c) += 0xfffff800;
        break;
    case 4:
        *(int *)(e + 0x18) += 0x80 << 6;
        *(int *)(e + 0x1c) += 0xfffff000;
        break;
    case 6:
        *(int *)(e + 0x18) += 0xffffc000;
        *(int *)(e + 0x1c) += 0x80 << 6;
        break;
    case 0:
        *(int *)(e + 0x18) += 0x80 << 5;
        *(int *)(e + 0x1c) += 0xfffff800;
        if (*(short *)(e + 0x66) != 0)
            v = _umodsi3_RAM(__Random(), 0x28) + 0x28;
        else
            v = _umodsi3_RAM(__Random(), 0x14) + 0x14;
        *c = v;
        break;
    }
    (*c)--;
}

extern unsigned char gState[];

void Func_808b2b0(int n)
{
    unsigned short v;

    switch (n) {
    case 1:
        v = 0x38;
        break;
    case 2:
        v = 0x3a;
        break;
    case 3:
        v = 0x3c;
        break;
    case 4:
        v = 0x36;
        break;
    case 5:
        v = 0x37;
        break;
    case 6:
        v = 0x37;
        break;
    case 7:
        v = 0x36;
        break;
    default:
        v = 0x39;
        break;
    }
    *(unsigned short *)(gState + (0xeb << 1)) = v;
}

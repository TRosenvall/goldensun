extern unsigned char gState[];

void Func_808b2b0(int n)
{
    unsigned short *p;

    p = (unsigned short *)(gState + (0xeb << 1));
    switch (n) {
    case 1:
        *p = 0x38;
        break;
    case 2:
        *p = 0x3a;
        break;
    case 3:
        *p = 0x3c;
        break;
    case 4:
        *p = 0x36;
        break;
    case 5:
        *p = 0x37;
        break;
    case 6:
        *p = 0x37;
        break;
    case 7:
        *p = 0x36;
        break;
    default:
        *p = 0x39;
        break;
    }
}

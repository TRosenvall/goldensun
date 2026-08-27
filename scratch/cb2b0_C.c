extern unsigned char gState[];
extern int _AREA_36;
extern int _AREA_37;
extern int _AREA_38;
extern int _AREA_39;
extern int _AREA_3a;
extern int _AREA_3c;

void Func_808b2b0(int n)
{
    int v;

    switch (n) {
    case 1:
        v = (int)&_AREA_38;
        break;
    case 2:
        v = (int)&_AREA_3a;
        break;
    case 3:
        v = (int)&_AREA_3c;
        break;
    case 4:
        v = (int)&_AREA_36;
        break;
    case 5:
        v = (int)&_AREA_37;
        break;
    case 6:
        v = (int)&_AREA_37;
        break;
    case 7:
        v = (int)&_AREA_36;
        break;
    default:
        v = (int)&_AREA_39;
        break;
    }
    *(unsigned short *)(gState + (0xeb << 1)) = v;
}

extern char gState[];
extern short L9e1d8[] __asm__(".L9e1d8");

void UpdateRespawnMap(void)
{
    char *g;
    short *p;
    short a;
    short b;

    g = gState;
    a = *(short *)(g + 0xe0 * 2);
    b = *(short *)(g + 0xe1 * 2);
    p = L9e1d8;
    while (p[0] != -1) {
        if (p[0] == a && (p[1] == -1 || p[1] == b)) {
            *(short *)(g + 0xe2 * 2) = p[2];
            *(short *)(g + 0xe3 * 2) = p[3];
            break;
        }
        p += 4;
    }
}

extern unsigned char *iwram_3001f38;
extern int _MSG_c7b;
extern void Func_80164d4(void *w, int a, int b, int c, int d);
extern void Func_801e7c0(int id, void *w, int a, int y);

void Func_8028aa8(void)
{
    unsigned char *g;
    unsigned char *a;
    unsigned char *b;
    int m;

    g = iwram_3001f38;
    a = g + 0x8c;
    b = g + 0x96;
    if (*(short *)b != *(short *)a) {
        *(unsigned short *)b = *(unsigned short *)a;
        Func_80164d4(*(void **)(g + 0x7c), 8, 0x28, 0x90, 0x50);
        switch (*(short *)a) {
        case 0:
            m = (int)&_MSG_c7b;
            Func_801e7c0(m, *(void **)(g + 0x7c), 0x12, 0x28);
            Func_801e7c0(m + 1, *(void **)(g + 0x7c), 0x12, 0x30);
            Func_801e7c0(m + 2, *(void **)(g + 0x7c), 0x12, 0x38);
            Func_801e7c0(m + 3, *(void **)(g + 0x7c), 0x12, 0x40);
            Func_801e7c0(m + 4, *(void **)(g + 0x7c), 0x12, 0x48);
            break;
        case 1:
            m = (int)&_MSG_c7b;
            Func_801e7c0(m, *(void **)(g + 0x7c), 0x12, 0x28);
            Func_801e7c0(m + 1, *(void **)(g + 0x7c), 0x12, 0x30);
            Func_801e7c0(m + 2, *(void **)(g + 0x7c), 0x12, 0x38);
            break;
        default:
            m = (int)&_MSG_c7b;
            Func_801e7c0(m, *(void **)(g + 0x7c), 0x12, 0x28);
            Func_801e7c0(m + 1, *(void **)(g + 0x7c), 0x12, 0x30);
            break;
        }
    }
}

extern char *ewram_2004c00;

void Func_80f7df0(int idx)
{
    char *b;
    char *hp;
    char *np;
    int no;
    int n4;
    int to;
    int ho;

    b = ewram_2004c00;
    no = idx * 12;
    to = 0x3404 + idx * 4;
    ho = *(int *)(b + to) * 4;
    hp = b + ho + (0xc0 << 6);
    n4 = no + 4;
    *(char **)(b + n4) = hp;
    ho = ho + (0xc0 << 6);
    *(char **)(b + no) = *(char **)(b + ho);
    np = b + no;
    *(char **)(b + ho) = np;
    if (*(char **)np != 0)
        *(char **)(*(char **)np + 4) = np;
}

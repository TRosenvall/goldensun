extern unsigned char *iwram_3001eec;
extern void (*Data_80ee2b4[])(int *p);

extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);

void Anim_Func(int *p)
{
    int k;
    int **q;

    galloc_iwram(0x29, 0x302);
    galloc_iwram(0x27, 0x782c);
    galloc_iwram(0x28, 0x80 << 7);
    q = (int **)(iwram_3001eec + 0x7828);
    k = *p;
    *q = p;
    if (k == 0)
        p[6] = 0;
    else
        Data_80ee2b4[k - 1](p);
    gfree(0x28);
    gfree(0x27);
    gfree(0x29);
}

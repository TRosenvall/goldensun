extern int iwram_3001f30;
extern void *GetFieldActor(int id);
extern void vec3_translate(int a, int b, void *p);

void Func_80970f8(int i0, int i1)
{
    char *g;
    char *a;
    char *b;
    int d;
    int v;

    g = (char *)iwram_3001f30;
    *(unsigned short *)(g + 0x18) = i0;
    a = (char *)GetFieldActor((short)i0);
    *(unsigned short *)(g + 0x1a) = i1;
    *(char **)(g + 0x10) = a;
    b = (char *)GetFieldActor((short)i1);
    d = (*(unsigned short *)(a + 6) + (0x80 << 6)) & (0xc0 << 8);
    *(char **)(g + 0x14) = b;
    *(int *)g = d;
    if (b != 0) {
        *(int *)(g + 0x38) = *(int *)(b + 0x6c);
        *(int *)(g + 0x3c) = *(int *)b;
        v = (*(char **)(*(char **)(b + 0x50) + 0x28))[5];
        g[0x44] = v;
        *(int *)(g + 4) = *(int *)(b + 8);
        *(int *)(g + 0xc) = *(int *)(b + 0x10);
        *(int *)(g + 8) = *(int *)(b + 0xc);
    } else {
        *(int *)(g + 4) = *(int *)(a + 8);
        *(int *)(g + 0xc) = *(int *)(a + 0x10);
        *(int *)(g + 8) = *(int *)(a + 0xc);
        vec3_translate(0x80 << 13, d, g + 4);
    }
}

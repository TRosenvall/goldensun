extern unsigned char *iwram_3001e8c;
extern int BufferString(int id, int mode);
extern void *CreateUIBox(int a, int b, int c, int d, int e);
extern void Func_8017248(int a, int b, int c, int d, int e);
extern int Func_8016670(void *box, int n, int c);
extern void CloseUIBox(void *box, int b);

void PrintBattleText(int id)
{
    unsigned char *p;
    int *slot;
    unsigned char *mode;
    int n;
    int off;
    int v;
    int one;
    void *box;
    void *t;

    p = iwram_3001e8c;
    mode = p + 0xea5;
    slot = *(int **)((char *)&iwram_3001e8c + 0x58);
    *mode = 2;
    v = 0;
    n = BufferString(id, 1);
    one = 1;
    *mode = one;
    off = n * 2;
    off += 0xeb << 4;
    if (*(unsigned short *)(p + off) != 0) {
        t = *(void **)slot;
        if (t == 0) {
            box = CreateUIBox(0, 0xf, 0x1e, 6, 0xa);
            *(void **)slot = box;
            Func_8017248(0, 0xf, 0x1e, 6, one);
            slot[2] = v;
        } else {
            box = t;
        }
        if (box != 0) {
            v = Func_8016670(box, n, slot[2]);
            slot[1] = v;
            slot[2] = 0;
            if (v == 0)
                CloseUIBox(box, 1);
        }
    }
}

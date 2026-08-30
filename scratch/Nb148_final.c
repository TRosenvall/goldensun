struct Node {
    unsigned char pad00[4];
    struct Node *next;
    unsigned char pad08[2];
    unsigned short fa;
    unsigned short fc;
};

extern unsigned char *iwram_3001e98;

extern void Func_801a97c(void);
extern void CloseUIBox(int box, int n);
extern void WaitFrames(int n);
extern void Func_8003f3c(int id);
extern void Func_801c21c(void);
extern void gfree(int tag);

void Func_801b148(void)
{
    unsigned char *p;
    struct Node *q;
    int z;

    p = iwram_3001e98;
    Func_801a97c();
    CloseUIBox(*(int *)(p + (0xd4 << 2)), 2);
    WaitFrames(1);
    q = *(struct Node **)(p + (0xd2 << 2));
    if (q != 0) {
        z = 0;
        do {
            if (q->fa != 0) {
                Func_8003f3c(q->fc);
                q->fa = z;
            }
            q = q->next;
        } while (q != 0);
    }
    q = *(struct Node **)(p + (0xd3 << 2));
    if (q != 0) {
        z = 0;
        do {
            if (q->fa != 0) {
                Func_8003f3c(q->fc);
                q->fa = z;
            }
            q = q->next;
        } while (q != 0);
    }
    Func_801c21c();
    if (*(short *)(p + 0x12) != 0) {
        Func_8003f3c(*(unsigned short *)(p + 0xc));
        if (*(short *)(p + 0x12) != 0)
            Func_8003f3c(*(unsigned short *)(p + 0x40));
    }
    Func_8003f3c(*(unsigned short *)(p + (0xb9 << 2)));
    gfree(0x12);
}

extern unsigned char *iwram_3001e8c;
extern int BufferString(int id, int mode);
extern int TextBox(int id, int *a, int *b, int *c, int *d);
extern void *CreateUIBox(int a, int b, int c, int d, int e);
extern void CloseUIBox(void *box, int b);
extern int Func_80165d8(void *box, int n, int c, int d, int e, int f);
extern int Func_8017364(void);
extern int Func_8017394(void *box);
extern void WaitFrames(int n);

void Func_8019aa0(int id, int mode, int yoff)
{
    unsigned char *g;
    void *box;
    int n;
    int off;
    int w;
    int h;
    int vc;
    int v8;
    short z;

    g = iwram_3001e8c;
    box = 0;
    w = 8;
    h = 8;
    n = BufferString(id, 1);
    off = n * 2;
    off += 0xeb << 4;
    if (*(unsigned short *)(g + off) != 0) {
        TextBox(id, &w, &h, &vc, &v8);
        w = (0x1e - vc) >> 1;
        h = ((0xf - v8) >> 1) + yoff;
        if (mode != 0) {
            box = CreateUIBox(w, h, vc, v8, 0);
        } else {
            box = CreateUIBox(w, h, 0, 0, 2);
            *(short *)((char *)box + 8) = 0;
            *(short *)((char *)box + 0xa) = 0;
        }
        if (Func_80165d8(box, n, 0, 0, 0, 0) == 0) {
            CloseUIBox(box, 1);
            return;
        }
        while (Func_8017364() == 0)
            WaitFrames(1);
        if (mode != 0) {
            CloseUIBox(box, 0);
            while (Func_8017394(box) == 0)
                WaitFrames(1);
        } else {
            CloseUIBox(box, 1);
        }
        *(short *)(g + 0x12f4) = z = 0;
        *(short *)(g + 0x12f6) = z;
    }
}

typedef struct { unsigned char b[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _GetLocationName(int a, int b);
extern void TextBox(int id, int *a, int *b, int *c, int *d);
extern void *CreateUIBox(int a, int b, int c, int d, int e);
extern void DrawSmallText(int id, void *w, int x, int y);
extern int StartTask(void *f, int prio);
extern void Func_801c3e8(void);

void Func_801c34c(void)
{
    unsigned char *st;
    unsigned char *g;
    int w, h, tw, th;
    int id;
    void *box;

    st = iwram_3001ebc;
    w = 8;
    h = 8;
    g = (unsigned char *)&gState;
    id = _GetLocationName(*(short *)(g + (0xe0 << 1)), *(short *)(g + (0xe1 << 1)));
    id += 0x99b;
    TextBox(id, &w, &h, &tw, &th);
    w = (0x1e - tw) >> 1;
    h = (0xa - th) >> 1;
    box = CreateUIBox(w, h, tw, th, 2);
    *(void **)(st + (0x8c << 2)) = box;
    DrawSmallText(id, box, 0, 0);
    id = 0x5a;
    *(short *)(st + (0x8d << 2)) = id;
    StartTask(Func_801c3e8, 0xc8 << 4);
}

struct Ui {
    unsigned char pad0[0x78];
    void *box;
    unsigned char pad7c[0x12];
    short count;
};

extern unsigned char *iwram_3001f38;

extern void Func_8028194(void);
extern void StopTask(void *task);
extern void CloseUIBox(void *box, int n);
extern void Func_8003f3c(int id);
extern void gfree(int tag);
extern void WaitFrames(int n);

void Func_802851c(void)
{
    struct Ui *u;
    int i;

    u = (struct Ui *)iwram_3001f38;
    StopTask(Func_8028194);
    if (u->box != 0)
        CloseUIBox(u->box, 2);
    for (i = 0; i < u->count; i++)
        Func_8003f3c(*(unsigned short *)((unsigned char *)u + i * 0x14 + 0x12));
    gfree(0x3a);
    WaitFrames(1);
}

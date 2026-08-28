extern unsigned char gSleepMode;

extern int StartMenu_Main(void);
extern int Menu_Save(void);
extern int Menu_Settings(void);
extern void Func_801776c(int a, int b);

int StartMenu(void)
{
    int r;

loop:
    r = StartMenu_Main();
    if (r == -1)
        return -1;
    if (r == 0) {
        if (Menu_Save() == -1)
            goto loop;
    } else if (r == 1) {
        Func_801776c(0xc2a, 1);
        gSleepMode = r;
    } else if (r == 2) {
        if (Menu_Settings() == -1)
            goto loop;
    }
    return 0;
}

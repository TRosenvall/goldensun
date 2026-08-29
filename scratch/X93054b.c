extern unsigned char gState[];
extern char *iwram_3001ebc;
extern void Func_8092c40(void);
extern int Func_8091c7c(int a, int b);
extern void ActorMessage(int a, int b);

int Func_8093054(int slot, int flags)
{
    int choice;
    char *p;
    int off;
    unsigned short *m;

    Func_8092c40();
    choice = Func_8091c7c(*(int *)(gState + (0xfa << 1)), 0);
    if (choice != 0) {
        p = iwram_3001ebc;
        off = 0xec << 1;
        m = (unsigned short *)(p + off);
        *m = *m + 1;
        ActorMessage(slot, flags);
    } else {
        ActorMessage(slot, flags);
        p = iwram_3001ebc;
        off = 0xec << 1;
        m = (unsigned short *)(p + off);
        *m = *m + 1;
    }
    return choice;
}

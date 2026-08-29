extern unsigned char gState[];
extern char *iwram_3001ebc;
extern void Func_8092c40(void);
extern int Func_8091c7c(int a, int b);
extern void ActorMessage(int a, int b);

int Func_8093054(int slot, int flags)
{
    int choice;
    unsigned short *m;
    unsigned short v;

    Func_8092c40();
    choice = Func_8091c7c(*(int *)(gState + (0xfa << 1)), 0);
    if (choice != 0) {
        m = (unsigned short *)(iwram_3001ebc + (0xec << 1));
        v = *m;
        *m = v + 1;
        ActorMessage(slot, flags);
        return choice;
    }
    ActorMessage(slot, flags);
    m = (unsigned short *)(iwram_3001ebc + (0xec << 1));
    v = *m;
    *m = v + 1;
    return choice;
}

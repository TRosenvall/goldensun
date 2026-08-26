extern unsigned char iwram_3001ebc[];
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern int __Func_8091c7c(int a, int b);
/* __Func_8092c40 intentionally implicit */

void OvlFunc_958_2008fd0(void)
{
    int id;
    char *base;

    id = 0x23cc;
    __MessageID(id);
    __Func_8092c40(8, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        if (__GetFlag(0x95 << 4)) {
            if (__GetFlag(0x96f) == 0)
                __MessageID(id + 8);
        }
        __ActorMessage(8, 0);
    } else {
        base = *(char **)iwram_3001ebc;
        (*(unsigned short *)(base + (0xec << 1)))++;
        __ActorMessage(8, 0);
    }
}

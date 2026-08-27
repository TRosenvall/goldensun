/* OvlFunc_883_2008eb4  --  0x02008eb4
 *
 * A shop-keeper who has one conversation before a flag is set and a
 * different one after, with the "after" branch also bumping a scene counter.
 *
 * Matched on the FIRST SCREEN with the basic-block lever applied up front: the
 * two shifted arguments to __Func_809218c in the else arm are assigned to
 * locals above the outer `if`. Two callees are declared `int` and the rest
 * `void`, read off whether the ROM puts `mov r0` first or last.
 */
extern char *iwram_3001ebc;
extern unsigned char L755a[] __asm__(".L755a");
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_883_2008eb4(void)
{
    unsigned short *q;
    int a;
    int b;

    a = 0xaa << 1;
    b = 0xde << 2;
    if (__GetFlag(0x815) && __GetFlag(0x87a) == 0) {
        __CutsceneStart();
        __MessageID(0x11b6);
        __Func_8092c40(0x15, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __Func_8093040(0x15, 0, 0x3c);
            __ActorMessage(0x15, 0);
        } else {
            q = (unsigned short *)(iwram_3001ebc + (0xec << 1));
            *q += 2;
            __CutsceneWait(0x28);
            __ActorMessage(0x15, 0);
        }
        __CutsceneEnd();
    } else {
        __PlaySound(0x9e);
        __Func_8010560(L755a, 0x32, 0x2c);
        __Func_809218c(0, a, b);
        __Func_8091e9c(7);
    }
}

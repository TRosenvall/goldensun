extern unsigned char L603a[] __asm__(".L603a");
extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void *__MapActor_GetActor(int slot);
extern void __CutsceneWait(int n);
extern int __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);

void OvlFunc_924_2009568(void)
{
    unsigned char *a;
    unsigned char *b;

    if (__GetFlag(0x256) != 0) {
        __CutsceneStart();
        __ClearFlag(0x256);
        a = (unsigned char *)__MapActor_GetActor(0);
        *(int *)(a + 0xc) += 0x80 << 10;
        b = (unsigned char *)__MapActor_GetActor(0);
        *(int *)(b + 0x3c) = *(int *)((unsigned char *)__MapActor_GetActor(0) + 0xc);
        __CutsceneWait(5);
        __CopyMapTiles(7, 2, 5, 0xb, 1, 1);
        __PlaySound(0xd9);
        __Func_8010560(L603a, 9, 7);
        __CutsceneEnd();
    }
}

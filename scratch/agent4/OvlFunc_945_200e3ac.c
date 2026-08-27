extern int __GetFlag(int flag);
extern void __ClearFlag(int flag);
extern void __SetFlag(int flag);

void OvlFunc_945_200e3ac(int a, int b)
{
    unsigned int i;
    int n1;
    int n2;
    int f;

    n1 = 0;
    n2 = 0;
    for (i = 0; i <= 8; i++) {
        f = a + i;
        if (__GetFlag(f) != 0) {
            __ClearFlag(f);
            break;
        }
        n1++;
    }
    for (i = 0; i <= 8; i++) {
        f = b + i;
        if (__GetFlag(f) != 0) {
            __ClearFlag(f);
            break;
        }
        n2++;
    }
    __SetFlag(b + n1);
    __SetFlag(a + n2);
}

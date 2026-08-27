typedef unsigned short u16;
typedef volatile unsigned short vu16;

extern u16 Func_801cbd4(void *t, unsigned int a, unsigned int b, unsigned int c);

void Func_801cae0(void *t)
{
    *(vu16 *)0x50001e8 = Func_801cbd4(t, 0xeeee, 0xcccc, 0x11110);
    *(vu16 *)0x50001ea = Func_801cbd4(t, 0xd555, 0xbbbb, 0xeeee);
    *(vu16 *)0x50001ec = Func_801cbd4(t, 0xbbbb, 0xaaaa, 0xcccc);
    *(vu16 *)0x50001ee = Func_801cbd4(t, 0xa221, 0x9999, 0xaaaa);
    *(vu16 *)0x50001f0 = Func_801cbd4(t, 0x10888, 0xdddd, 0x13333);
    *(vu16 *)0x50001f2 = Func_801cbd4(t, 0x12221, 0xeeee, 0x15555);
    *(vu16 *)0x50001f4 = Func_801cbd4(t, 0x13bbb, 0x80 << 9, 0x17777);
}

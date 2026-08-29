extern void __WaitFrames(int n);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_80118c0(int n);
extern void __Func_80118a8(int n);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int n);

void OvlFunc_882_200bc48(void)
{
    __WaitFrames(0x14);
    __SetFlag(0xb3 << 1);
    __Func_80118c0(0);
    __Func_80118c0(1);
    __Func_80118c0(2);
    __Func_80118c0(3);
    __Func_80118c0(4);
    __Func_80118c0(5);
    __Func_8091200(0x10003, 1);
    __Func_8091200(0x80 << 9, 2);
    __Func_8091254(1);
    __WaitFrames(0x78);
    __Func_8091200(0, 0);
    __Func_8091254(0x3c);
    __WaitFrames(0x3c);
    __ClearFlag(0xb3 << 1);
    __Func_80118a8(0);
    __Func_80118a8(1);
    __Func_80118a8(2);
    __Func_80118a8(3);
    __Func_80118a8(4);
    __Func_80118a8(5);
}

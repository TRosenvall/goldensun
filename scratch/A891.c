extern void __Func_8012078(int a, int b, int c, int d);
extern int OvlFunc_891_2009be8(int a, int b, int c);
extern void OvlFunc_891_200a244(void);
extern void OvlFunc_891_200a2f4(void);

void OvlFunc_891_20095d4(void)
{
    __Func_8012078(2, 0xd0 << 16, 0xe0 << 15, 0);
    if (OvlFunc_891_2009be8(0xa, 0xe, 7) != 0)
        OvlFunc_891_200a244();
}

void OvlFunc_891_20095fc(void)
{
    __Func_8012078(2, 0xb0 << 17, 0xe0 << 15, 0);
    if (OvlFunc_891_2009be8(0xc, 0x15, 7) != 0)
        OvlFunc_891_200a2f4();
}

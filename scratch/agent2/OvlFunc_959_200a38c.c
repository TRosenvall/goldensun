extern char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_801776c(int id, int n);
extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_959_200a2a0(void);

void OvlFunc_959_200a38c(void)
{
	char *base;
	int a;
	int b;

	base = iwram_3001ebc;
	if (*(short *)(base + 0xcb8) == 0)
		return;
	if (__GetFlag(0x948) != 0)
		return;
	__Func_801776c(0x1528, 1);
	__PlaySound(0xbc);
	__CutsceneWait(1);
	a = 3;
	b = 0x37;
	__Func_80105d4(6, 0x4d, 1, 2, a, b);
	__CutsceneWait(5);
	__Func_80105d4(7, 0x4d, 1, 2, a, b);
	__CutsceneWait(1);
	OvlFunc_959_200a2a0();
	__SetFlag(0x948);
}

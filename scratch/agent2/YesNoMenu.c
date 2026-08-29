extern void Func_80284dc(void);
extern void AddMenuBarOption(int n);
extern void Func_8028808(int a, int b, int c);
extern int Func_8028574(int n);
extern void Func_802851c(void);

int YesNoMenu(int a, int b, int c, int d)
{
	int flags;

	flags = 0;
	Func_80284dc();
	if (c == 0)
		c = 3;
	if (a != 0)
		flags = 0x11;
	AddMenuBarOption(5);
	AddMenuBarOption(6);
	Func_8028808(flags, c, b);
	d = Func_8028574(d);
	Func_802851c();
	if (d == -1)
		d = 1;
	return d;
}

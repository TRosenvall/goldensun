extern void Func_8097868(void);
extern void StartTask(void (*fn)(void), int prio);
extern char *iwram_3001e8c;

void Func_8097a7c(void)
{
	short *p;

	*(iwram_3001e8c + 0xea4) = 1;
	p = (short *)0x50001e2;
	*p = 0x739c;
	p += 2;
	*p = 0x739c;
	p++;
	*p = 0x739c;
	p++;
	*p = 0x739c;
	p++;
	*p = 0x739c;
	p++;
	*p = 0x739c;
	p++;
	*p = 0x739c;
	p++;
	*p = 0x739c;
	p++;
	*p = 0x739c;
	p++;
	*p = 0x739c;
	p++;
	*p = 0x739c;
	StartTask(Func_8097868, 0x480);
}

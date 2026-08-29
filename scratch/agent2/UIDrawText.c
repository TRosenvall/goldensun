extern void *Func_8004970(int size);
extern void Func_8017c8c(void *buf, int a, unsigned int x, unsigned int y);
extern void free(void *p);

void UIDrawText(unsigned char *s, int a, unsigned int x, unsigned int y)
{
	unsigned short *buf;
	unsigned short *p;

	buf = (unsigned short *)Func_8004970(0x200);
	p = buf;
	while (*s != 0) {
		*p = *s;
		s++;
		p++;
	}
	*p = 0;
	x >>= 3;
	y >>= 3;
	Func_8017c8c(buf, a, x, y);
	free(buf);
}

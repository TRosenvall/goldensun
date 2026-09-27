/* Func_8095778  --  0x08095778, was asm/rom_8a000/rom_944ec_a_a_c_c_c_c.s (this
 * function alone), so it converts whole. Written from the landed Func_8095680.
 *
 * THE POOLED HALFWORD ZERO IS REG_EQUIV: local-alloc marks the HImode zero
 * pseudo REG_EQUIV const 0 and reload pools it. A block-local
 * `{ int z = 0; *(u16 *)g2 = z; }` with the address (a fresh `g2`) computed
 * first gives the ROM's `mov r3,#0` after the add; a function-scope z fixes the
 * pool but moves unrelated allocations.
 */
extern unsigned char gState[];

extern void _SetFlag(int id);
extern unsigned char *Func_808d394(int id);
extern void WaitFrames(int n);
extern void Func_80955b0(int a, int b, int c);

void Func_8095778(int arg)
{
	unsigned char *g;
	unsigned char *p;
	unsigned char *rec;
	int hi;
	int low;
	int v;
	int n;
	int a;
	int b;
	int i;
	unsigned char *g2;

	g = gState;
	p = g + (0x8d << 2);
	hi = *(short *)p & (0xf0 << 8);
	low = *(unsigned short *)p & 0xfff;
	if (arg == 0) {
		if (hi == 0) {
			low &= 0x7ff;
			if ((unsigned int)(low - 0x12c) > 0x50)
				return;
			v = *(short *)(g + 0x236);
			if (v > 0 && v != 0x3e7)
				return;
			_SetFlag(low - 0xac);
			*(unsigned short *)p = hi;
		} else if (hi == (0x80 << 5)) {
			if (*(short *)(g + 0x236) == 1)
				_SetFlag(low);
			*(unsigned short *)p = arg;
		}
		return;
	}
	if (hi == 0) {
		low &= 0x7ff;
		if ((unsigned int)(low - 0x12c) <= 0x50) {
			low &= 0x7ff;
			if (*(short *)(g + 0x236) > 0) {
				n = low - 0x12c;
				a = n / 20;
				b = n % 20;
				for (i = 8; i <= 0x41; i++) {
					rec = Func_808d394(i);
					if (rec == 0)
						continue;
					if (*(short *)(rec + 2) - 0x30 != low - 0x12c)
						continue;
					WaitFrames(0x28);
					Func_80955b0(i, a, b);
					break;
				}
			}
		}
	}
	g2 = gState;
	g2 += 0x8d << 2;
	{
		int z = 0;
		*(unsigned short *)g2 = z;
	}
}

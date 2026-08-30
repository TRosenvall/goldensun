extern int __Random(void);

int OvlFunc_944_20080c0(char *p)
{
	short *q;
	int v;
	int n;
	int t;
	int sv;

	q = (short *)(p + 0x64);
	v = *q;
	if (v == 9) {
		*(int *)(p + 0x4c) = 0;
	} else if (v != 0) {
		n = (unsigned int)(__Random() << 11) >> 16;
		t = *(int *)(p + 0x4c);
		t -= n;
		*(int *)(p + 0x4c) = t;
		if (t < -0xc000) {
			sv = 0;
			*q = sv;
		}
	} else {
		n = (unsigned int)(__Random() << 11) >> 16;
		t = *(int *)(p + 0x4c);
		t += n;
		*(int *)(p + 0x4c) = t;
		if (t > (0xc0 << 8)) {
			sv = 1;
			*q = sv;
		}
	}

	t = *(int *)(p + 8);
	if (t > 0x280000 && t < 0x1400000)
		*(int *)(p + 8) = t + *(int *)(p + 0x4c);

	q = (short *)(p + 0x66);
	v = *q;
	if (v == 9) {
		*(int *)(p + 0xc) = 0;
	} else if (v != 0) {
		n = (unsigned int)((__Random() * 3) << 14) >> 16;
		t = *(int *)(p + 0xc);
		t -= n;
		*(int *)(p + 0xc) = t;
		if (t < 0) {
			sv = 0;
			*q = sv;
		}
	} else {
		n = (unsigned int)((__Random() * 3) << 14) >> 16;
		t = *(int *)(p + 0xc);
		t += n;
		*(int *)(p + 0xc) = t;
		if (t > (0x80 << 13)) {
			sv = 1;
			*q = sv;
		}
	}

	return 1;
}

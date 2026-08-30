extern int __Random(void);

int OvlFunc_944_20080c0(char *p)
{
	short *q;
	int v;
	int n1, n2, n3, n4;
	int t1, t2, t3, t4, tm;
	int sv;

	q = (short *)(p + 0x64);
	v = *q;
	if (v == 9) {
		*(int *)(p + 0x4c) = 0;
	} else if (v != 0) {
		n1 = (unsigned int)(__Random() << 11) >> 16;
		t1 = *(int *)(p + 0x4c);
		t1 -= n1;
		*(int *)(p + 0x4c) = t1;
		if (t1 < -0xc000) {
			sv = 0;
			*q = sv;
		}
	} else {
		n2 = (unsigned int)(__Random() << 11) >> 16;
		t2 = *(int *)(p + 0x4c);
		t2 += n2;
		*(int *)(p + 0x4c) = t2;
		if (t2 > (0xc0 << 8)) {
			sv = 1;
			*q = sv;
		}
	}

	tm = *(int *)(p + 8);
	if (tm > 0x280000 && tm < 0x1400000)
		*(int *)(p + 8) = tm + *(int *)(p + 0x4c);

	q = (short *)(p + 0x66);
	v = *q;
	if (v == 9) {
		*(int *)(p + 0xc) = 0;
	} else if (v != 0) {
		n3 = (unsigned int)((__Random() * 3) << 14) >> 16;
		t3 = *(int *)(p + 0xc);
		t3 -= n3;
		*(int *)(p + 0xc) = t3;
		if (t3 < 0) {
			sv = 0;
			*q = sv;
		}
	} else {
		n4 = (unsigned int)((__Random() * 3) << 14) >> 16;
		t4 = *(int *)(p + 0xc);
		t4 += n4;
		*(int *)(p + 0xc) = t4;
		if (t4 > (0x80 << 13)) {
			sv = 1;
			*q = sv;
		}
	}

	return 1;
}

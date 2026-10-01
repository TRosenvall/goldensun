extern int A(void); extern int B(void); extern int C(void);
/* Table 2 shape: minval 8, maxval 18, cases 15 and 16 ABSENT (default-filled),
   cases 13 and 14 sharing one arm. Selector unsigned. */
int p5(unsigned int s, int x, int y)
{
	int r = 0;
	switch (s) {
	case 8:  r = A(); break;
	case 9:  r = x; break;
	case 10: r = -x; break;
	case 11: r = y; break;
	case 12: r = -y; break;
	case 13: r = B(); break;
	case 14: r = B(); break;
	case 17: r = C(); break;
	case 18: r = -C(); break;
	}
	return r;
}

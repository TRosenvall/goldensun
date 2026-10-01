extern void Arm(int);
extern void Dflt(void);
/* case 0 sharing the DEFAULT body: pins the table's minval to 0, so gcc
   emits no `sub` and the entry test is a bare `bhi` against the maximum. */
void p3(unsigned int s)
{
	switch (s) {
	case 0:  Dflt(); break;
	case 10: Arm(1); break;
	case 11: Arm(1); break;
	case 12: Arm(1); break;
	case 13: Arm(1); break;
	case 14: Arm(2); break;
	case 15: Arm(2); break;
	case 16: Arm(2); break;
	case 17: Arm(2); break;
	case 18: Arm(2); break;
	case 19: Arm(2); break;
	case 20: Arm(2); break;
	case 21: Arm(2); break;
	default: Dflt(); break;
	}
}

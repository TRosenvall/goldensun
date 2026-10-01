extern void Arm(int);
extern void Dflt(void);
/* `default:` and `case 0:` on ONE block: entry 0 and the out-of-range
   target become the same label, which is the ROM's shape. */
void p4(unsigned int s)
{
	switch (s) {
	default:
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
	}
}

extern void Arm(int);
extern void Dflt(void);
/* Each case gets its OWN body, so the labels differ at expand time and
   group_case_nodes cannot merge them into ranges. */
void p2(unsigned int s)
{
	switch (s) {
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

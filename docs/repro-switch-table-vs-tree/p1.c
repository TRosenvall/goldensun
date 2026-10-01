extern void ArmA(void);
extern void ArmB(void);
extern void Dflt(void);
void p1(unsigned int s)
{
	switch (s) {
	case 10: case 11: case 12: case 13:
		ArmA(); break;
	case 14: case 15: case 16: case 17:
	case 18: case 19: case 20: case 21:
		ArmB(); break;
	default:
		Dflt(); break;
	}
}

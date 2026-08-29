extern void f(int a, int b, int c);
extern int S1;
/* same symbol twice */
void p8(void) { f(0, (int)&S1, 1); f(1, (int)&S1, 2); }

extern void f(int a, int b, int c);
extern int cond(void);
/* branch between the two uses */
void p6(void) { f(0, 0x9999, 0x4ccc); if (cond()) f(1, 0x9999, 0x4ccc); }
/* two distinct extern symbols with the same value */
extern int S1;
extern int S2;
void p7(void) { f(0, (int)&S1, 1); f(1, (int)&S2, 2); }

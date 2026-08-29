extern void f(int a, int b, int c);
extern void g(int a, int b, int c);
/* 1: plain, two uses of the same pool constant */
void p1(void) { f(0, 0x9999, 0x4ccc); f(1, 0x9999, 0x4ccc); }
/* 2: two named locals */
void p2(void) { int a = 0x9999, b = 0x4ccc, c = 0x9999, d = 0x4ccc; f(0, a, b); f(1, c, d); }
/* 3: three uses */
void p3(void) { f(0, 0x9999, 1); f(1, 0x9999, 2); f(2, 0x9999, 3); }
/* 4: uses separated by other calls */
void p4(void) { f(0, 0x9999, 0x4ccc); g(9, 9, 9); g(8, 8, 8); f(1, 0x9999, 0x4ccc); }
/* 5: one use only */
void p5(void) { f(0, 0x9999, 0x4ccc); }

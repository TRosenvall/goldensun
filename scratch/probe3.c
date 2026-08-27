extern void a3(int a, int b, short c);
extern void b3(int a, int b, unsigned char c);
extern void c3(int a, int b, long long c);
extern void d3();
extern void e3(int a, int b, int c, int d);
extern void f3(int a, short b);
extern void g3(int a, int b, int c);
extern int cond(void);
void q1(void) { a3(0xc, 0, 0xa); }
void q2(void) { b3(0xc, 0, 0xa); }
void q3(void) { d3(0xc, 0, 0xa); }
void q4(void) { int d = 0xa; if (cond()) g3(0xc, 0, d); }
void q5(void) { f3(0xc, 2); }
void q6(void) { g3(0xc, 0, (int)(char)0xa); }

extern void f(int);
extern int _MSG_x;

void t_lit(void)  { int b = 0x2389;        f(b); f(b + 1); f(b + 2); }
void t_sym(void)  { int b = (int)&_MSG_x;  f(b); f(b + 1); f(b + 2); }

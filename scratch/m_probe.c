extern void f(int);
extern void g(void);
extern int _S1;
extern int _S2;

void t1(void)   /* same literal twice */
{
    f(0x16f); g(); f(0x16f);
}

void t2(void)   /* two distinct symbols */
{
    f((int)&_S1); g(); f((int)&_S2);
}

void t3(void)   /* same symbol twice */
{
    f((int)&_S1); g(); f((int)&_S1);
}

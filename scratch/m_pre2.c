extern void f(int, int, int);
extern int g(void);
extern void h(int);

/* dominating use, then MANY calls, then a branch use */
void t4(void)
{
    f(8, 0xa0 << 7, 0);
    h(1); h(2); h(3); h(4); h(5); h(6); h(7); h(8);
    if (g()) f(9, 0xa0 << 7, 0);
}

/* dominating use, then a branch with the second use deep inside */
void t5(void)
{
    f(8, 0xa0 << 7, 0);
    if (g()) { h(1); h(2); h(3); f(9, 0xa0 << 7, 0); h(4); }
    else { h(5); }
}

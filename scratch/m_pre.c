extern void f(int, int, int);
extern int g(void);

/* both uses inside mutually exclusive branches */
void t1(void)
{
    if (g()) f(8, 0xa0 << 7, 0);
    else     f(9, 0xa0 << 7, 0);
}

/* one use dominating, one in a branch */
void t2(void)
{
    f(8, 0xa0 << 7, 0);
    if (g()) f(9, 0xa0 << 7, 0);
}

/* three uses, all in branches */
void t3(void)
{
    if (g()) f(8, 0xa0 << 7, 0);
    else if (g()) f(9, 0xa0 << 7, 0);
    else f(10, 0xa0 << 7, 0);
}

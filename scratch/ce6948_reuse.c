extern char *iwram_3001eec;
extern void AnimStart(int a);
extern void AnimEnd(void);
extern void Anim_Djinni(int a, int b, int c, int d, int *e, int *f);

void Anim_Kite(int arg)
{
    int u;
    int v;
    int **p;
    int *q;

    p = (int **)(iwram_3001eec + 0x7828);
    q = (int *)arg;
    *p = q;
    AnimStart(0);
    Anim_Djinni(arg, 7, q[1] ^ 1, 0, &u, &v);
    AnimEnd();
}

extern char *iwram_3001eec;
extern void AnimStart(int a);
extern void AnimEnd(void);
extern void Anim_Djinni(int a, int b, int c, int d, int *e, int *f);

void Anim_Kite(int arg)
{
    int v;
    int u;
    int **p;

    p = (int **)(iwram_3001eec + 0x7828);
    *p = (int *)arg;
    AnimStart(0);
    Anim_Djinni(arg, 7, (*p)[1] ^ 1, 0, &u, &v);
    AnimEnd();
}

extern void *galloc_ewram(int tag, int size);
extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void BaseAnim_SpecialAttack(int *p);
extern void Anim_CriticalHit(int *p);
extern void BaseAnim_Attack(int *p);

void Anim_Attack(int *p)
{
    int k;

    galloc_ewram(0x29, 0x60e);
    galloc_iwram(0x27, 0x782c);
    galloc_iwram(0x28, 0x80 << 7);
    k = *p;
    if ((unsigned int)(k - 0x64) <= 0x23)
        BaseAnim_SpecialAttack(p);
    else if (k > 0xc7)
        Anim_CriticalHit(p);
    else
        BaseAnim_Attack(p);
    gfree(0x28);
    gfree(0x27);
    gfree(0x29);
}

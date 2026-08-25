extern void *__MapActor_GetActor(int slot);

void OvlFunc_930_2009060(void *actor)
{
    unsigned char *a;
    unsigned char *p;
    int t;
    int v;

    a = (unsigned char *)actor;
    if (*(int *)((unsigned char *)__MapActor_GetActor(0) + 0xc) > *(int *)(a + 0xc)) {
        p = a;
        p += 0x23;
        t = *p;
        v = 2 | t;
    } else {
        p = a;
        p += 0x23;
        t = *p;
        v = 0xfd & t;
    }
    *p = v;
}

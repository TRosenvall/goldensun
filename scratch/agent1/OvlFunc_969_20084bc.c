extern unsigned char *iwram_3001ebc;
extern unsigned char *__MapActor_GetActor(int slot);
extern int OvlFunc_969_2008480(void *a, void *b);

int OvlFunc_969_20084bc(void)
{
    unsigned char **p;
    unsigned char *actor;
    unsigned char *e;
    unsigned int i;
    int best;
    int besti;

    p = (unsigned char **)(iwram_3001ebc + 0x14);
    besti = 0;
    best = 0xa0 << 2;
    actor = __MapActor_GetActor(0);
    for (i = 8; i <= 0x41; i++) {
        e = p[i];
        if (e != 0 && **(short **)(*(int *)(e + 0x50) + 0x28) == 0xf2) {
            int r = OvlFunc_969_2008480(actor + 8, e + 8);
            if (r < best) {
                best = r;
                besti = i;
            }
        }
    }
    return besti;
}

extern unsigned char *iwram_3001e60;
extern unsigned char *_GetSpriteInfo(int id);

void Func_8012d70(int group, int anim)
{
    unsigned char *base;
    unsigned char *ent;
    unsigned char *info;
    unsigned char *ip;
    int off;
    int i;
    int z;
    int tbl;

    base = iwram_3001e60;
    off = ((group & 3) << 2) + 0x28;
    i = 0;
    z = 0;
    do {
        ent = *(unsigned char **)(base + off);
        if (*(int *)(ent + 0xc) != 0) {
            info = _GetSpriteInfo(*(short *)ent);
            if (anim < info[5]) {
                ent[4] = info[4];
                tbl = *(int *)(ent + 0xc);
                ip = (unsigned char *)(anim << 2);
                *(int *)(ent + 0x10) = *(int *)(ip + tbl);
                *(short *)(ent + 2) = i << 4;
                ent[0x15] = 0x10;
                ent[0x14] = z;
                ent[0x17] = z;
                ent[0x16] = 0xff;
            }
            base[0x23] = info[7];
            *(short *)(base + 0x1e) = z;
        }
        i++;
        base += 0x38;
    } while (i <= 9);
}

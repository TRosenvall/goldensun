extern int Sprite_SetAnim(unsigned int sprite, unsigned int anim);
extern void Sprite_SetAnimSpeed(unsigned int sprite, unsigned int speed);

void Actor_SetAnimAndSpeed(unsigned char *p, unsigned int anim, unsigned int speed) {
    unsigned int v;
    unsigned int *q;
    int i;

    if (p == (unsigned char *)0)
        return;

    switch (*(unsigned char *)(p + 0x54) & 0xf) {
    case 1:
        Sprite_SetAnim(*(unsigned int *)(p + 0x50), anim);
        Sprite_SetAnimSpeed(*(unsigned int *)(p + 0x50), speed);
        break;
    case 2:
        q = *(unsigned int **)(p + 0x50);
        for (i = 3; i >= 0; i--) {
            v = *q++;
            if (v != 0) {
                Sprite_SetAnim(v, anim);
                Sprite_SetAnimSpeed(v, speed);
            }
        }
        break;
    }
}

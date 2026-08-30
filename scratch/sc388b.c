extern void Sprite_SetAnim(void *s, int a);
extern void Sprite_SetAnimSpeed(void *s, int a);

void Actor_SetAnimAndSpeed(unsigned char *e, int anim, int speed)
{
    void **list;
    void *s;
    int i;
    int a;

    a = anim;
    if (e == 0)
        return;
    switch (*(unsigned char *)(e + 0x54) & 0xf) {
    case 1:
        Sprite_SetAnim(*(void **)(e + 0x50), a);
        Sprite_SetAnimSpeed(*(void **)(e + 0x50), speed);
        break;
    case 2:
        list = *(void ***)(e + 0x50);
        for (i = 3; i >= 0; i--) {
            s = *list++;
            if (s != 0) {
                Sprite_SetAnim(s, a);
                Sprite_SetAnimSpeed(s, speed);
            }
        }
        break;
    }
}

extern int iwram_3001f2c;
extern void _Sprite_SetAnim(void *sprite, int anim);
extern void StopTask(void *fn);
extern void Func_80a3c08(void);

void Func_80a3c98(void)
{
    char *p;
    signed char i;
    int off;

    p = (char *)iwram_3001f2c;
    if (*(unsigned char *)(p + 0x219) != 0) {
        i = 0;
        do {
            off = 0x8a * 2 + i * 4;
            _Sprite_SetAnim(*(void **)(p + off), 1);
            i++;
        } while (i < *(unsigned char *)(p + 0x219));
    }
    StopTask(Func_80a3c08);
}

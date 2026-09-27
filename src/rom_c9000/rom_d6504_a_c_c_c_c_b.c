/* Func_80d6888  --  0x080d6888, split out of asm/rom_c9000/rom_d6504_a_c_c_c_c.s;
 * Func_80d67dc stays in _a.s. Matched from scratch.
 *
 * Five parameters, the fifth an `int` read by `ldrb` from sp+0x34. The outer
 * loop is a backward `goto` to deny invariant motion; the store is spelled
 * `{ int k = slot + 0x7818; base[k] = val; }`; and the inner loop indexes
 * ((u8 **)(spr + 0x28))[j] rather than stepping a pointer.
 */
extern int *_GetBattleActor(int id);
extern unsigned char *_Func_80b7f70(int a, int b);
extern int _Func_80b6cd0(int id);
extern void _Sprite_SetAnim(void *sprite, int anim);
extern unsigned char *iwram_3001eec;

void Func_80d6888(int id, int pal, int anim, int slot, int val)
{
    int *actor;
    unsigned char *base;
    unsigned char *spr;
    int i;
    int j;

    actor = _GetBattleActor(id);
    base = iwram_3001eec;
    i = 0;
    goto test;
body:
    if (slot != -1)
        { int k = slot + 0x7818; base[k] = val; }
    if (((short *)actor)[0x15] != 0)
        goto next;
    if (pal != -1) {
        for (j = 0; j != spr[0x27]; j++) {
            unsigned char *p = ((unsigned char **)(spr + 0x28))[j];
            if (p == 0 || p == (unsigned char *)actor[9] || p == (unsigned char *)actor[8])
                continue;
            if (pal == 0)
                p[5] = _Func_80b6cd0(id);
            else
                p[5] = pal;
            p[0x16] = 0xff;
        }
    }
    if (anim != -1)
        _Sprite_SetAnim(spr, anim);
next:
    i++;
test:
    if ((spr = _Func_80b7f70(*actor, i)) != 0)
        goto body;
}

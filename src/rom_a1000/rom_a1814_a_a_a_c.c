/* Func_80a1870  --  0x080a1870, was asm/rom_a1000/rom_a1814_a_a_a_c.s (this
 * function alone), so it converts whole. Matched from scratch.
 *
 * - `((signed char *)s)[9] &= ~0xc` gives the ROM's `mov #0xd / neg` mask AND
 *   fixes the order of the incoming-argument stack spills (67 lines without).
 * - StartTask declared `int`, not `void`, fixes the `mov r1` / `ldr r0` order
 *   before the call: batch 286's callee-return-type lever.
 * (The old .s header said the 0x10000 store goes to the actor; it goes to
 * state+0x154+i*4.)
 */
extern int iwram_3001f2c;
extern int _Func_80796c4(void *out);
extern int _Func_808b398(int id);
extern unsigned char *_CreateSprite(int resource);
extern void _Sprite_SetAnim(void *sprite, int anim);
extern int StartTask(void *fn, int prio);
extern void Func_80a19a0(void);

void Func_80a1870(unsigned short *win, int x, int y, int spacing)
{
    char *base;
    unsigned char *s;
    int n, i;
    unsigned short buf[14];

    base = (char *)iwram_3001f2c;
    n = (unsigned short)_Func_80796c4(buf);
    base[0x1e] = n;
    for (i = 0; i < n; i++) {
        s = _CreateSprite(_Func_808b398(buf[i]));
        if (s != 0) {
            ((unsigned char **)(base + 0x114))[i] = s;
            ((short *)(base + 0x134))[i] = (x + win[6]) * 8 + (spacing + 0x10) * i;
            ((short *)(base + 0x144))[i] = (y + win[7]) * 8 + 0x10;
            ((int *)(base + 0x154))[i] = 0x10000;
            ((signed char *)s)[9] &= ~0xc;
            s[0x26] = 0;
            _Sprite_SetAnim(s, 1);
        }
    }
    for (; i <= 7; i++)
        ((unsigned char **)(base + 0x114))[i] = 0;
    StartTask(Func_80a19a0, 0xc80);
}

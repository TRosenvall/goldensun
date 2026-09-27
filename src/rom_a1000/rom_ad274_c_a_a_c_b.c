/* Func_80ad508  --  0x080ad508, split out of asm/rom_a1000/rom_ad274_c_a_a.s;
 * Func_80ad40c stays in _c_a.s and Func_80ad5b4 (parked) in _c_c.s. Matched
 * from scratch, from the ad274 template (slot[8] = 0x10000, y = 0xc8,
 * Func_80ad40c registered).
 *
 * The loop body order is load-bearing: `((union U *)slot)->i = sprite;
 * slot[8] = 0x10000; coord[0] = 0x10; coord[4] = 0xc8; slot++; coord++;
 * off += 4;` -- `*slot++ = sprite` gives `stmia` and swaps r6/r7 (24 off). The
 * union store matches under default flags; a union-free spelling needs
 * -fno-strict-aliasing and is not used.
 */
extern unsigned int iwram_3001f2c;
extern int  _DeleteSprite(int sprite);
extern int  _CreateSprite(int resource);
extern void _Sprite_SetAnim(int sprite, int anim);
extern void StartTask(void *task, int priority);
extern int  Func_80ad40c;
extern unsigned char Laf304[] __asm__(".Laf304");

union U { int i; short s; };
void Func_80ad508(int window, int unused)
{
    unsigned char *base;
    unsigned char *tbl;
    unsigned int *slot;
    short *coord;
    int i;
    int off;
    int sprite;
    int priority;

    base = (unsigned char *)iwram_3001f2c;
    for (i = 0x89; i <= 0x8c; i++) {
        if (*(unsigned int *)(base + (i << 2)) != 0) {
            _DeleteSprite(*(unsigned int *)(base + (i << 2)));
            *(unsigned int *)(base + (i << 2)) = 0;
        }
    }

    tbl = Laf304;
    coord = (short *)(base + (0x8d << 2));
    slot = (unsigned int *)(base + (0x89 << 2));
    off = 0;
    for (i = 0; i < 4; i++) {
        sprite = _CreateSprite(*(unsigned int *)(off + (int)tbl));
        if (sprite != 0) {
            _Sprite_SetAnim(sprite, 2);
        }
        ((union U *)slot)->i = sprite;
        slot[8] = 0x10000;
        coord[0] = 0x10;
        coord[4] = 0xc8;
        slot++;
        coord++;
        off += 4;
    }

    priority = 0xc8 << 4;
    StartTask((void *)&Func_80ad40c, priority);
}

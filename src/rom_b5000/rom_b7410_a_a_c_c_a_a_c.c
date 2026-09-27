/* Func_80b7994  --  0x080b7994, was asm/rom_b5000/rom_b7410_a_a_c_c_a_a_c.s (this
 * function alone), so it converts whole. Matched from scratch.
 */
extern unsigned char *Func_80b7f70(void *a, int i);
extern void _Sprite_DeleteLayer(unsigned char *s, int layer);
extern int _Sprite_AddLayer(unsigned char *s, int n);
extern void _SpriteLayer_SetAnim(unsigned char *p, int b);

struct Part {
    void *f0;
    int f4;
    short f8;
    unsigned char pad[0x12];
    short mask;
    unsigned char cur;
    signed char timer;
    int layer;
};

int Func_80b7994(struct Part *a)
{
    unsigned char *s;
    int layer;
    int changed;
    int idx;
    unsigned short m;
    int mm;
    int cur;
    void *f0;
    unsigned char *l;

    changed = 0;
    if (a->timer >= 0)
        a->timer--;
    layer = a->layer;
    if (layer == 0 ? a->mask == 0 : (a->mask >> a->cur) & 1) {
        if (a->timer != 0)
            return;
    }
    m = a->mask;
    idx = -1;
    f0 = a->f0;
    mm = (short)m;
    if (mm != 0) {
        cur = a->cur;
        idx = cur + 1;
        for (;;) {
            if (idx > 13)
                idx = 0;
            if (((short)m >> idx) & 1)
                break;
            idx++;
        }
        if (cur != idx || layer == 0) {
            a->cur = idx;
            changed = 1;
        }
        a->timer = 0x50;
    } else {
        changed = 1;
    }
    s = Func_80b7f70(f0, 0);
    if (s == 0)
        return;
    if (idx >= 0) {
        if (s[0x20] == 0x20)
            idx += 0x154;
        else
            idx += 0x163;
    }
    if (a->layer != 0 && changed) {
        _Sprite_DeleteLayer(s, a->layer);
        a->layer = 0;
    }
    if (idx >= 0 && changed) {
        a->layer = _Sprite_AddLayer(s, idx);
        if (a->layer == -1)
            a->layer = 0;
        l = (unsigned char *)a->layer;
        if (l != 0) {
            l[6] = 3;
            _SpriteLayer_SetAnim(l, 0);
        }
    }
    s[0x25] = 1;
    if (idx >= 0)
        a->f8 = idx;
    else
        a->f8 = 0;
}

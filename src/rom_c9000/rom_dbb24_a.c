/* CreateSummonSprite  --  0x080dbb24, was asm/rom_c9000/rom_dbb24_a.s (this function alone),
 * so it converts whole. Matched from scratch.
 *
 * `base->spr[i]` as a MEMBER ARRAY gives the ROM's separate base and offset
 * registers (`str r0,[r5,r7]`); raw pointer arithmetic merges them. A
 * signed-char f9 gives the `#-13` mask where unsigned char gives `#243`.
 */
struct Spr { char pad[9]; signed char f9; char pad2[0x1c]; unsigned char f26; };
struct Bat { char pad[0x77d8]; struct Spr *spr[1]; };
extern struct Bat *iwram_3001eec;
extern struct Spr *_CreateSprite(int res);
extern void _Sprite_SetAnim(struct Spr *s, int anim);

void CreateSummonSprite(int count, int res, int prio)
{
    struct Bat *base = iwram_3001eec;
    int i;

    for (i = 0; i != count; i++) {
        struct Spr *s = _CreateSprite(res);
        base->spr[i] = s;
        if (s != 0) {
            s->f26 = 0;
            _Sprite_SetAnim(s, i);
            base->spr[i]->f9 = (base->spr[i]->f9 & ~0xc) | ((prio & 3) << 2);
        }
    }
}

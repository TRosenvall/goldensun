/* CreateUIBox  --  0x080162d4, was asm/rom_15000/rom_15e8c_a_c_a_c.s (this
 * function alone), so it converts whole. Matched from scratch. The ROM's
 * `ldr r2,=8` pool loads are gcc's own halfword pooling (its `ldrh rX, .L`
 * assembles to the same encoding), so no constant symbols are needed.
 */
typedef struct UIBox {
    struct UIBox *unk0;
    struct UIBox *unk4;
    unsigned short w;
    unsigned short h;
    unsigned short x;
    unsigned short y;
    unsigned short unk10;
    unsigned short unk12;
    unsigned short unk14;
    unsigned short flags;
    short unk18;
    short anim;
    unsigned char pad[0x24 - 0x1c];
} UIBox;

extern unsigned char *iwram_3001e8c;
extern void Func_80173ac(void);
extern void Func_8016230(UIBox *win);
extern void UIBox_WaitAnim(UIBox *win);
extern void WaitFrames(unsigned int nframes);

UIBox *CreateUIBox(int x, int y, int w, int h, int flags)
{
    UIBox *p;
    UIBox *box;
    int i;

    p = (UIBox *)(iwram_3001e8c + 0x500);
    box = 0;
    for (i = 0; i != 8; i++, p++) {
        if ((p->flags & 1) == 0 && p->anim == 0) {
            box = p;
            break;
        }
    }
    if (box != 0) {
        box->x = x;
        box->y = y;
        box->w = w;
        box->h = h;
        box->unk0 = 0;
        box->unk14 = 0;
        box->unk4 = p;
        box->unk10 = 1;
        box->flags = 1;
        Func_80173ac();
        if (flags & 8)
            box->flags |= 8;
        if (flags & 0x20)
            box->flags |= 0x20;
        if (flags & 0x40)
            box->flags |= 0x40;
        if (flags & 0x80)
            box->flags |= 0x80;
        if (flags & 0x100)
            box->flags |= 0x100;
        if (flags & 2) {
            box->flags |= 2;
            box->unk18 = 0;
            box->anim = 1;
            Func_8016230(box);
        } else {
            box->anim = 8;
            box->unk18 = 7;
            UIBox_WaitAnim(box);
            WaitFrames(1);
        }
    }
    return box;
}

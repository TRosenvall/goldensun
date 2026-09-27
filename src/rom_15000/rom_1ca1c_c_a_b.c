/* Func_801cf48  --  0x0801cf48, split out of asm/rom_15000/rom_1ca1c_c_a.s;
 * Func_801d014 stays in _c.s. Matched from scratch.
 *
 * Both GetFile calls are ONE call with a ternary argument -- the ROM has one
 * `bl` fed from two paths. The struct over iwram_3001ea0 (fields 0x574..0x5d4,
 * tbl indexed past its declared bound to match the addressing) should also
 * answer the "indexed load" and schedule complaints in the park for its
 * sibling Func_801d94c, which reads the same base.
 */
struct DialObj { unsigned char pad[0xe]; unsigned char slot; };
struct DialState {
    unsigned char pad0[0x574];
    unsigned short mode;          /* 0x574 */
    unsigned char pad1[6];
    unsigned short frame;         /* 0x57c */
    unsigned char pad2[0x594 - 0x57e];
    signed char sel[0x10];        /* 0x594 */
    unsigned char a[0x10];        /* 0x5a4 */
    struct DialObj *b;            /* 0x5b4 */
    unsigned char bpad[0xc];
    struct DialObj *c;            /* 0x5c4 */
    unsigned char cpad[0xc];
    void *tbl[1][3];              /* 0x5d4 */
};
extern struct DialState *iwram_3001ea0;
extern int _FILE_e8;
extern void _Func_80b08b8(void *p);
extern void _Func_80b0958(void *p);
extern void *GetFile(int id);
extern int UploadSpriteGFX(int slot, int n, void *src);
extern void Func_80217a4(void *p);

void Func_801cf48(void)
{
    struct DialState *s = iwram_3001ea0;
    int mode = s->mode;
    void *f;

    _Func_80b08b8(&s->a);
    _Func_80b0958(&s->b);
    _Func_80b0958(&s->c);
    f = GetFile(mode == 0 ? (s->frame & 7) + (int)&_FILE_e8 : (int)&_FILE_e8);
    UploadSpriteGFX(s->b->slot, 0x100, f);
    f = GetFile(mode == 1 ? (s->frame & 7) + (int)&_FILE_e8 : (int)&_FILE_e8);
    UploadSpriteGFX(s->c->slot, 0x100, f);
    if (mode > 1)
        Func_80217a4(s->tbl[mode][s->sel[mode]]);
    s->frame++;
}

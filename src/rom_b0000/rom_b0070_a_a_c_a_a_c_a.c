/* Cluster Func_80b0278..Func_80b0278 extracted from goldensun/asm/rom_b0000/rom_b0070_a_a_c_a_a_c.s.
 *
 * Total .text for this TU = 460 bytes (= 0x1cc). Never attempted before batch 279.
 * No pins, no flags, NO SYMBOL -- and that last point is the reason to read this file.
 *
 * Ladder (objcmp encodings against the ROM's 191):
 *     `for(;;)` + break, inline halfword store, char-pointer base      122 differing, 456 bytes
 *     + `goto loop` / `goto done`                                      119, 456
 *     + a `v` temp so the actor load chain evaluates before the LHS     91, 456
 *     + THE STRUCT SPELLING for the base and the window                 EXACT
 *
 * THE LAST ROW IS THE FINDING. The ROM has `ldr r3, .Lb0374 @ 0 / strb r3, [r5, #4]` plus a
 * MID-FUNCTION pool with a real `b` over it -- the pooled-narrow-head shape.
 *
 * Writing that zero as `(int)&_AREA_00` reproduces the ROM's assembly TEXT and gets to one
 * instruction short. IT IS THE RECORDED FALSE LEAD: an SImode symbol has pool_range 1020, so all
 * thirteen words land in ONE pool after the epilogue and the `b` disappears.
 *
 * THE ACTUAL PRODUCER IS THE STRUCT-FIELD SPELLING. `base->f3a4 = v;` makes `store_bit_field`
 * create a HImode `(const_int 0)` insert mask, cse hands that to `w->f4 = 0` as a `subreg:QI`,
 * and the resulting `ldrh r3, .L22` -- pool_range 64 -- DRAGS THE POOL UP. The generated `.s`
 * splits it exactly as the ROM does: [0, iwram_3001f2c, 937, 3227, 938] mid-function and the
 * remaining eight words at the end.
 *
 * And the obvious shortcut does NOT work: `unsigned short z = 0; w[4] = z;` folds to `mov r3, #0`
 * (91 and 79 differing for two placements). The HImode entry has to arrive as a BITFIELD INSERT
 * MASK, not as a halfword store of a named zero.
 *
 * SO src/non_matching/rom_b0000/80b0a20.c's reading -- "a pooled zero can be a leftover halfword
 * mask from a neighbouring field store" -- IS THE LIVE ROUTE, and it is reachable from ordinary
 * C. That park also says the symbol reading is not the answer, and this confirms both halves:
 * the symbol gets the text and loses the pool position; the bitfield mask gets both. Worth
 * re-reading that park with this file beside it.
 */
struct Shop {
    unsigned char pad000[0x26c];
    int f26c;
    unsigned char pad270[0x380 - 0x270];
    void *f380;
    unsigned char pad384[0x390 - 0x384];
    unsigned short f390;
    unsigned char pad392[0x3a4 - 0x392];
    unsigned short f3a4;
    unsigned char f3a6;
    unsigned char pad3a7[2];
    unsigned char f3a9;
    unsigned char f3aa;
    unsigned char pad3ab[1];
    unsigned char f3ac;
};

struct W {
    unsigned char pad00[4];
    unsigned char f4;
    unsigned char f5;
};

extern unsigned char iwram_3001f2c[];

extern int Func_80b26c8(void);
extern void Func_80b26cc(int id);
extern void Func_80b010c(void);
extern int Func_80b2764(int id);
extern unsigned char *_MapActor_GetActor(int slot);
extern int _Func_8019da8(int a, int b, int c, int d);
extern int _CreateUIBox(int a, int b, int c, int d, int e);
extern struct W *_Func_801eadc(int a, int b, int c, int d, int e);
extern void Func_80b0a20(void *p, int a, int b);
extern void Func_80b04dc(int msg);
extern int _ShopMenu(int sel);
extern int Func_80b2720(int id, void *p);
extern void Func_80b0aac(void);
extern void Func_80b1a14(void);
extern int Func_80b0070(void);
extern void WaitFrames(int n);
extern void Func_80b2110(void);
extern void _CloseUIBox(int h, int n);
extern void Func_80b0204(void);

int Func_80b0278(int id, int actor)
{
    struct Shop *base;
    struct W *w;
    int box;
    int sel;
    int v;

    sel = 0;
    if (id >= Func_80b26c8() || id < 0)
        id = 0;
    Func_80b26cc(id);
    Func_80b010c();
    base = *(struct Shop **)iwram_3001f2c;
    base->f3a9 = Func_80b2764(id);
    if (id == 0x10)
        base->f3ac = 1;
    if (id == 0x11)
        base->f3ac = 1;
    if (id == 0x12)
        base->f3ac = 1;
    v = *(unsigned short *)(*(int *)(*(int *)(_MapActor_GetActor(actor) + 0x50) + 0x28));
    base->f3a4 = v;
    box = _Func_8019da8(base->f3a4, 0, 0, 0);
    if (box == 0)
        box = _CreateUIBox(-5, 0, 5, 5, 2);
    w = _Func_801eadc(base->f390, 0x80 << 23, box, 0, 0);
    w->f5 = 1;
    w->f4 = 0;
    Func_80b0a20(&base->f380, -0x20, 0x70);
    base->f380 = w;
    Func_80b04dc(0xc9b);
loop:
    sel = _ShopMenu(sel);
    base->f3aa = sel;
    if (sel == 0) {
        base->f3a6 = Func_80b2720(id, &base->f26c);
        Func_80b04dc(0xca7);
        Func_80b0aac();
    } else if (sel == 1) {
        Func_80b04dc(0xca9);
        Func_80b1a14();
    } else if (sel == 2) {
        if (Func_80b0070() != 0) {
            Func_80b04dc(0xcb8);
            Func_80b0aac();
        } else {
            Func_80b04dc(0xcb7);
            WaitFrames(1);
        }
    } else if (sel == 3) {
        Func_80b04dc(0xcb9);
        Func_80b2110();
    } else {
        goto done;
    }
    Func_80b0a20(&base->f380, -0x20, 0x70);
    Func_80b04dc(0xca4);
    goto loop;
done:
    Func_80b04dc(0xca5);
    _CloseUIBox(box, 2);
    Func_80b0204();
    return 0;
}

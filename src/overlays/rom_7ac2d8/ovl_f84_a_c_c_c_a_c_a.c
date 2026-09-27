/* OvlFunc_924_2009164  --  0x02009164, was
 * asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_a_c_a.s (this function alone, no
 * data), so it converts whole.
 *
 * Parked at 12 of 205 on a local-alloc tie in thumb_expand_movstrqi's pointer
 * pair: six struct-copy sites each wanting {r2,r3} in one order or the other.
 *
 * THE "THREE QUANTITIES GIVES dst = r3" RULE IS A gcc-2.96 BUG, traced under gdb
 * in find_free_reg. For exactly three quantities block_alloc (local-alloc.c)
 * hand-sorts instead of calling qsort, and calls qty_compare(0,1),
 * qty_compare(1,2), qty_compare(0,1) on QUANTITY NUMBERS rather than qty_order[]
 * slots -- so the 0/1 swap runs twice and cancels, quantity 0 (the block move's
 * dst) is allocated first and takes r3 although src has the higher priority.
 * With two, or four or more (real qsort), src goes first and dst gets r2.
 *
 * The lever is to take the block OFF three quantities: the out-of-range flag
 * constant was the third pseudo, so each flag case holds its id in one
 * block-scoped `int f` shared by both arms, which moves it out of the copy's
 * block. The shared `int g` in case 10's two arms is still needed (4 without).
 */
struct S { int a; unsigned b; int c, d, e, f; };

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern int __GetFlag(int id);
extern int OvlFunc_924_2008758(struct S *s);
extern void OvlFunc_924_20088ec(struct S s);
extern void OvlFunc_924_200bc48(int a, int b, int c, int d);
extern void OvlFunc_924_20090c0(void);

void OvlFunc_924_2009164(void)
{
    struct S s;
    int g;

    __CutsceneStart();
    if (OvlFunc_924_2008758(&s)) {
        switch (s.b) {
        case 9:
            if ((s.e >> 20) == 8) {
                OvlFunc_924_20088ec(s);
                __CutsceneWait(0x14);
                __CopyMapTiles(0x77, 9, 0x6d, 0xb, 1, 1);
                OvlFunc_924_200bc48(0x2d60000, 0, 0xb4 << 16, 0x80 << 8);
                __SetFlag(0xc4 << 2);
            } else {
                __CopyMapTiles(0x75, 9, 0x68, 7, 1, 1);
                __CopyMapTiles(0x77, 8, 0x6d, 0xb, 1, 1);
                __CopyMapTiles(0x76, 8, 0x68, 0xd, 1, 1);
                OvlFunc_924_20088ec(s);
                __ClearFlag(0xc4 << 2);
            }
            break;
        case 10:
            {int f = 0x311;
            if ((s.e >> 20) == 0xc) {
                OvlFunc_924_20088ec(s);
                __CutsceneWait(0xa);
                g = __GetFlag(0xc4 << 2);
                if (g) {
                    __CopyMapTiles(0x76, 9, 0x68, 0xd, 1, 1);
                    OvlFunc_924_200bc48(0xa1 << 18, 0, 0xd2 << 16, 0x80 << 7);
                }
                __SetFlag(f);
            } else {
                __CopyMapTiles(0x77, 8, 0x6d, 0xb, 1, 1);
                g = __GetFlag(0xc4 << 2);
                if (g) {
                    __CopyMapTiles(0x77, 9, 0x6d, 0xb, 1, 1);
                    __CopyMapTiles(0x76, 8, 0x68, 0xd, 1, 1);
                }
                OvlFunc_924_20088ec(s);
                __ClearFlag(f);
            }}
            break;
        case 11:
            {int f = 0x312;
            if ((s.c >> 20) == 0x28) {
                OvlFunc_924_20088ec(s);
                __SetFlag(f);
            } else {
                OvlFunc_924_20088ec(s);
                __ClearFlag(f);
            }}
            break;
        }
        OvlFunc_924_20090c0();
    }
    __CutsceneEnd();
}

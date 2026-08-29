/* OvlFunc_903_2008d04  --  0x02008d04, cut from
 * goldensun/asm/overlays/rom_798dc4/ovl_314_c_a_c.s.
 *
 * Opens a passage, but only from one tile row: read the player's z as a whole
 * tile, and if it is 0xb set a flag bit on slot 8 and repaint two map-attribute
 * rectangles before recording the save bit.
 *
 * TWO NAMED LOCALS, ONE FOR EACH DIRECTION OF THE LEVER.
 *
 *   `unsigned char two = 2; *p = two | *p;` for the flag bit. Batch 83's rule
 *   -- a local of the width the constant is combined with, written first --
 *   and here it is needed: the plain `*p |= 2` and `*p = 2 | *p` both put the
 *   loaded byte in the `orr` destination where the ROM has the constant, and an
 *   `int` local does the same. Batch 85 found the opposite on an `and` in
 *   OvlFunc_928_2008968, where the plain form was already right, so the two
 *   functions together are the evidence that this is a spelling to TRY rather
 *   than a rule: same lever, same shape, opposite answers.
 *
 *   `int c = 0xc;` for the value that is argument six of the first call and
 *   argument five of the second. The ROM keeps it in r5 across both; naming it
 *   is what stops gcc rebuilding it, and is the stack-argument half of the same
 *   lever.
 *
 * `y` is read once and used twice -- for the `== 0xb` test and as the last
 * argument -- which is what the ROM's r6 does.
 */
struct A { unsigned char pad00[8]; int f8; };

extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void OvlFunc_903_2008dd8(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_903_2008d04(void)
{
    int y;
    unsigned char *p;
    int c;
    unsigned char two;

    __CutsceneStart();
    y = ((struct A *)__MapActor_GetActor(8))->f8 >> 20;
    if (y == 0xb) {
        OvlFunc_903_2008dd8(8);
        p = (unsigned char *)__MapActor_GetActor(8) + 0x23;
        two = 2;
        *p = two | *p;
        c = 0xc;
        __Func_8010704(0x27, 0xc, 3, 1, 8, c);
        __Func_8010704(0x2b, 0xb, 3, 1, c, y);
        __SetFlag(0x86 << 4);
    }
    __CutsceneEnd();
}

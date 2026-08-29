/* OvlFunc_932_200a428  --  0x0200a428
 *
 * Cut out of goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a.s,
 * which holds eighteen functions; the rest stay as assembly around it.
 *
 * Cleans up after a cutscene: once save bit 0x8fe is set the door sprite's
 * flag halfword loses bit 9, otherwise the doorway tile is repainted; then, if
 * the party is in one of two adjacent areas, save bit 0x12f is cleared.
 *
 * THE SUBTRACTION MUST BE DONE AT `int` WIDTH. Written as
 * `(unsigned short)(*(unsigned short *)(g + 0x1c2) - 6) <= 1`, gcc does the
 * arithmetic at halfword width and pools the addend:
 *
 *      rom    sub r3, #6
 *      ours   ldr r1, =0xffff... / add r3, r1
 *
 * Reading the halfword into an `int` local and subtracting there gives the
 * ROM's `sub`. This is the same family as the `v = v + 0xff` note in
 * src/non_matching/rom_b5000/80bf37c.c, seen from the other side: there the ROM
 * wanted the ADD and `v--` gave a sub; here it wants the SUB and the narrow
 * expression gave an add. The rule underneath both is that the WIDTH the
 * arithmetic happens at decides which gcc picks, so put the value in an `int`
 * and write what the ROM shows.
 *
 * `(unsigned short)v <= 1` compiles to `lsl #16 / cmp` against a pre-shifted
 * 0x10000 -- the narrowing-cast tell, here on a range rather than an equality.
 * It means the two areas are 6 and 7.
 *
 * The two stack arguments to __Func_8010704 are 0x35 and 0x2a, two different
 * values in the two slots, so both are named locals; batch 95's rule.
 */
struct B { unsigned char pad00[0x14]; unsigned short f14; };

extern struct B *iwram_3001e70;
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_932_200a428(void)
{
    struct B *p;
    unsigned char *g;
    int v;
    int e;
    int f;

    if (__GetFlag(0x8fe)) {
        p = iwram_3001e70;
        p->f14 &= 0xfdff;
    } else {
        e = 0x35;
        f = 0x2a;
        __Func_8010704(0x34, 0x2a, 1, 1, e, f);
    }
    g = gState;
    v = *(unsigned short *)(g + (0xe1 << 1));
    v = v - 6;
    if ((unsigned short)v <= 1)
        __ClearFlag(0x12f);
}

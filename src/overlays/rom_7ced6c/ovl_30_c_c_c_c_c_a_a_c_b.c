/* OvlFunc_946_200985c  --  0x0200985c
 * OvlFunc_946_20098b0  --  0x020098b0
 *
 * The back two functions of goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_a_a_c.s;
 * OvlFunc_946_2009774 stays as assembly in ovl_30_c_c_c_c_c_a_a_c_a.s.
 *
 * Twins. Each stops an actor, marks it, and repaints the tile it is standing on
 * -- one biased a tile west, the other a tile north, which is the only real
 * difference between them.
 *
 * TWO LEVERS, and both are about which of r2 and r3 holds what. That pair has
 * four parked functions behind it (see docs/elevation.md), so a case where it
 * IS reachable is worth writing out.
 *
 *   STATEMENT ORDER DECIDES THE COORDINATE PAIR. The ROM loads f8 into r3 and
 *   f10 into r2 for 200985c, and the other way round for 20098b0 -- matching
 *   which of the two gets the `- 1`. Writing the two assignments in the order
 *   the ROM loads them took the pair from 9 differing of 39 to 2. Declaration
 *   order does nothing; the order of the STATEMENTS is what counts.
 *
 *   THE NAMED CONSTANT'S TYPE DECIDES THE `orr` PAIR. The ROM has
 *
 *       ldrb r2, [r1] / mov r3, #2 / orr r3, r2 / strb r3, [r1]
 *
 *   -- the CONSTANT in the destination, not the loaded byte. `*q = 2 | *q`,
 *   `*q |= 2`, `*q = *q | 2` and a named `int two` all put the loaded byte
 *   there instead. A named constant of the FIELD's own type --
 *   `unsigned char two = 2;` -- puts the constant there and closes both
 *   functions.
 *
 *   That is the sharp part: `int two` fails and `unsigned char two` works, on
 *   the same statement. The width of the named constant, not the naming, is the
 *   lever. It did NOT transfer to src/non_matching/ovl_7ed0a0/2009458.c, which
 *   has the identical instruction shape but computes into a variable that
 *   crosses a join rather than storing in the same statement -- see the note
 *   there.
 *
 * `x` gets `- 1` in 200985c and `y` in 20098b0; the third and fourth arguments
 * to __Func_8010704 swap with them, which is how the two rectangles differ.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x23 - 0x14];
    unsigned char f23;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_200985c(int slot, int a, int b)
{
    struct A *p;
    unsigned char *q;
    unsigned char two;
    int x;
    int y;

    p = __MapActor_GetActor(slot);
    if (p != 0) {
        __Func_8092b08(slot, 3);
        q = &p->f23;
        two = 2;
        *q = two | *q;
        y = p->f10 >> 20;
        x = (p->f8 >> 20) - 1;
        __Func_8010704(a, b, 3, 1, x, y);
    }
}

void OvlFunc_946_20098b0(int slot, int a, int b)
{
    struct A *p;
    unsigned char *q;
    unsigned char two;
    int x;
    int y;

    p = __MapActor_GetActor(slot);
    if (p != 0) {
        __Func_8092b08(slot, 3);
        q = &p->f23;
        two = 2;
        *q = two | *q;
        x = p->f8 >> 20;
        y = (p->f10 >> 20) - 1;
        __Func_8010704(a, b, 1, 3, x, y);
    }
}

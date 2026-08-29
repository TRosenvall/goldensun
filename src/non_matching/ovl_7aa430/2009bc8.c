/* OvlFunc_923_2009bc8 -- NOT MATCHING
 * OvlFunc_924_200d158 -- the same function in a different overlay, differing
 *                        only in the script symbol, so this park covers two.
 *
 * OvlFunc_907_2008f3c -- a THIRD member, batch 99, same blocker at 7 of 46.
 *
 * Source asm: goldensun/asm/overlays/rom_7aa430/ovl_1a3c_a_a_a.s
 *             goldensun/asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a.s
 *             goldensun/asm/overlays/rom_79b154/ovl_30_c_c.s
 * Best screen: 7 differing of 39, streams the same length.
 *
 * OvlFunc_907_2008f3c is the same routine with one extra masked byte at the
 * sprite's +5 (`mask & s->f5 | 4`, the named-int form). It reaches EXACTLY the
 * same 7 differing positions with exactly the same spelling, which is a useful
 * confirmation: the residue is the two pointer chains and nothing to do with
 * what the sprite branch does afterwards. Two further spellings were measured
 * on it and both are worse -- naming the 1 and 2 as `unsigned char` (11), and
 * storing +0x22 before +0x55 (10) -- matching what the other two members do.
 *
 * BLOCKER CLASS: register allocation, r2 against r3, on two pointer chains.
 *
 * TWO LEVERS GOT IT FROM 26 TO 7 and both are new, so they are the reason this
 * is worth reading:
 *
 *   TWO POINTERS MUST BOTH BE LIVE, OR GCC DERIVES ONE FROM THE OTHER. The
 *   function writes a byte at +0x55 and two more at +0x22 and +0x23. Writing
 *   the first as a struct field and the others through a walked pointer gives
 *
 *       mov r2, r5 / add r2, #0x55 / strb / sub r2, #0x33 / strb ...
 *
 *   -- ONE register walked backwards, because 0x55 - 0x33 is 0x22 and gcc will
 *   find that. The ROM keeps two independent chains (`mov r3, r5 / add r3,
 *   #0x55` and `mov r2, r5 / add r2, #0x22`). Naming both as separate pointer
 *   locals is not enough on its own; they have to be COMPUTED BEFORE the first
 *   store, so that both are live across it. 26 differing to 10.
 *
 *   THE STORE ORDER THEN DECIDES WHICH CHAIN IS WHICH. With both pointers
 *   computed up front, storing to +0x55 first gives the ROM's order; storing to
 *   +0x22 first swaps the two chains. 10 to 7.
 *
 * What remains is the r2/r3 exchange itself -- the ROM puts the +0x55 chain in
 * r3 and the +0x22 chain in r2, and we have them the other way round. That is
 * the same four-member class documented in docs/elevation.md.
 *
 * WORTH TRYING NEXT: batch 97 found that for
 * src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_a_a_c_b.c the exchange was reachable
 * through the TYPE of a named constant -- `unsigned char two = 2` matched where
 * `int two` did not. Nothing here is a named constant of that shape, but the
 * three stored values (0, 1, 2) are all candidates.
 *
 * The shared zero IS forced: it is written to +0x55 and again to the sprite's
 * +0x26 after a call, so it crosses a call and a bare literal would be rebuilt.
 * That is batch 95's discriminator and it is satisfied below.
 */
struct Spr {
    unsigned char pad00[9];
    unsigned char f9;
    unsigned char pad0a[0x26 - 0xa];
    unsigned char f26;
};

struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
    unsigned char pad23[0x50 - 0x23];
    struct Spr *f50;
    unsigned char pad54[1];
    unsigned char f55;
};

extern unsigned char gScript_923__0200a7b8[];
extern unsigned char gScript_924__0200de08[];
extern struct A *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetScript(struct A *a, unsigned char *s);
extern void __Sprite_SetAnim(struct Spr *s, int n);

void OvlFunc_923_2009bc8(struct A *src)
{
    struct A *n;
    struct Spr *s;
    unsigned char *q;
    unsigned char *r;
    unsigned char zero;

    n = __CreateActor(0x18, src->f8, src->fc, src->f10);
    if (n != 0) {
        s = n->f50;
        __Actor_SetScript(n, gScript_923__0200a7b8);
        r = &n->f55;
        q = &n->f22;
        zero = 0;
        *r = zero;
        *q = 1;
        q += 1;
        *q = 2;
        if (s != 0) {
            __Sprite_SetAnim(s, 2);
            s->f26 = zero;
            s->f9 |= 0xc;
        }
    }
}

void OvlFunc_924_200d158(struct A *src)
{
    struct A *n;
    struct Spr *s;
    unsigned char *q;
    unsigned char *r;
    unsigned char zero;

    n = __CreateActor(0x18, src->f8, src->fc, src->f10);
    if (n != 0) {
        s = n->f50;
        __Actor_SetScript(n, gScript_924__0200de08);
        r = &n->f55;
        q = &n->f22;
        zero = 0;
        *r = zero;
        *q = 1;
        q += 1;
        *q = 2;
        if (s != 0) {
            __Sprite_SetAnim(s, 2);
            s->f26 = zero;
            s->f9 |= 0xc;
        }
    }
}

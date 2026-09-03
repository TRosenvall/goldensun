/* OvlFunc_969_200b660  --  0x0200b660
 *
 * Cut from the middle of goldensun/asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_a_a.s.
 * Unlike its already-landed sibling this is not a tail cut -- the function
 * before it stays in _a_a.s and the one after in _a_a_c.s. Each function in this
 * file owns its own one-word pool, so no pool crosses a boundary and the split
 * is layout-clean. Verified byte-neutral before this landed.
 *
 * Places a satellite actor on a circle around the party leader and advances its
 * angle by a fixed step each frame.
 *
 * MULTIPLY OPERAND ORDER IN THE SOURCE SURVIVES TO THE mul, AND IT IS A ONE-LINE
 * LEVER. Thumb's multiply is destructive, so gcc emits a copy into the
 * destination and then multiplies into it -- and WHICH factor becomes the copied
 * destination is decided by the order the two factors are written in C, with no
 * canonicalisation. Writing the call on the left puts the sum in the copy;
 * writing the sum on the left puts the call result there. That was the entire
 * diff, both multiplies, 4 to 0.
 *
 * This is the multiplicative twin of the recorded one-expression-versus-compound
 * copy lever, and it is cheap enough to sweep on any function containing a
 * multiply whose operands differ in kind.
 *
 * ONE LOAD KEPT ACROSS TWO CALLS IS A NAMED LOCAL; TWO LOADS OF THE SAME FIELD
 * ARE DIRECT FIELD READS. Both appear side by side here. The angle is loaded
 * ONCE and reused for both trig calls, so it is a local; the offset byte is
 * loaded TWICE with only its address commoned, so it is written inline at both
 * uses; and the trailing decrement reloads, which confirms it operates on the
 * field rather than on the local. THAT IS THE READ-COUNT RULE APPLIED ACROSS A
 * CALL BARRIER -- gcc cannot carry a memory value over a call, so a SINGLE load
 * is proof of a local and REPEATED loads are proof there is none.
 *
 * The callee-set grep beat filename adjacency for the third round running. The
 * stem-sibling gave only the field offsets, and all three of its headline levers
 * were inapplicable because this function has no branches at all; the useful hit
 * was a cross-bank file whose discussion of the destructive-copy mechanism is
 * exactly what the remaining diff turned on.
 *
 * Family note: the function immediately above this one in the parent is the same
 * shape minus one term, and these two levers should carry it directly.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    int pad0c;
    int y;
    unsigned char pad14[0x1c];
    int f30;
    unsigned char pad34[4];
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0x1e];
    unsigned char f62;
    unsigned char pad63;
    unsigned short angle;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_969_200b660(struct Actor *e)
{
    struct Actor *a;
    int ang;

    a = __MapActor_GetActor(0x17);
    ang = e->angle;
    e->x = a->x + __cos(ang) * (e->f30 + e->f62 + 6);
    e->y = a->y + __sin(ang) * (e->f62 + 4);
    e->f38 = e->x;
    e->f40 = e->y;
    e->angle -= 0x800;
}

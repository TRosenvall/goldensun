/* OvlFunc_947_20099f0 -- 0x020099f0, asm/overlays/rom_7d0e88/ovl_1528_a_a_c_a_c_a.s
 *
 * objcmp: ENCODINGS differ in 37 place(s) (ref 89, ours 89) -- 37 encodings of 89.
 * Same size (184 bytes). Everything up to the layer compare is EXACT.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7d0e88/20099f0.c asm/overlays/rom_7d0e88/ovl_1528_a_a_c_a_c_a.s --func OvlFunc_947_20099f0
 *
 * Leaf. Returns 1 if b sits in a's column/cell band (|dx| < 0x10 tiles, same
 * y>>16, b below a by < 0x20 in z), after copying a's 2-bit sprite layer
 * (byte 9, bits 2-3) and bits 2-3 of byte 0x15 into b's sprite when a's layer is
 * higher and clearing bit 0 of b+0x23; 0 otherwise.
 *
 * SOLVED (measured):
 *  - `r = 0; ... goto out; ... r = 1; out: return r;` puts `mov r0, #0` before
 *    the first compare and keeps r0 live across the whole test chain, as the ROM
 *    does. Early `return 0`s give a separate `mov r0,#0; b` block (55 differing).
 *  - The sprite fields are BITFIELDS (the -13 mask pair `mov #0xd / neg`, the
 *    `lsr 30 / lsl 2` insert). Explicit u8 mask arithmetic gives `mov #0xf3`.
 *  - ALIASING: the ROM reloads BOTH a->spr and b->spr after the byte-9 store, so
 *    the sprite struct's stores must conflict with the actor's pointer loads. A
 *    separate `struct Spr` does not (gcc keeps both pointers). Putting both views
 *    in ONE union type does -- that is the union below.
 *
 * BLOCKER: the ROM keeps a's `byte9 << 28` in r1 across the `b->f23 &= ~1`
 * store and re-does `lsr #30` for both the compare and the insert. With the
 * union (needed for the reloads) the char store to +0x23 invalidates a's byte,
 * so either gcc reloads it (t947c shape, 48) or, with `la` named, CSEs the
 * whole `>> 30` into one register (this file, 37). A separate struct Spr gets
 * the r1 shape exactly (t947b) but loses the pointer reloads (55). The two
 * requirements look asymmetric -- Spr store must conflict with the actor load,
 * actor char store must NOT conflict with the Spr load -- which alias sets
 * cannot express, so the ROM is probably holding a whole-byte or <<28 value in
 * a variable through some spelling not found yet.
 *
 * INERT: la's type (u8/int/char/u16/short all 37); u8 array view of byte 9
 * with explicit `(la << 28) >> 30` (37, CSE'd identically); volatile cast on
 * the layer store (no reload); -fno-rerun-cse-after-loop (32 text lines,
 * no closer to exact). A nested struct for +0x23 is impossible: ARM gcc aligns
 * every struct to 4.
 */
typedef union U {
    struct {
        unsigned char pad0[8];
        int x;
        int y;
        int z;
        unsigned char pad14[0x23 - 0x14];
        unsigned char f23;
        unsigned char pad24[0x50 - 0x24];
        union U *spr;
    } o;
    struct {
        unsigned char pad0[9];
        unsigned char lo9 : 2;
        unsigned char layer : 2;
        unsigned char hi9 : 4;
        unsigned char pad1[0x15 - 0xa];
        unsigned char lo15 : 2;
        unsigned char prio : 2;
        unsigned char hi15 : 4;
    } s;
} U;

int OvlFunc_947_20099f0(U *a, U *b)
{
    int r;
    unsigned int la;

    r = 0;
    if (b->o.x == a->o.x && b->o.y == a->o.y && b->o.z == a->o.z)
        goto out;
    if (a->o.x - 0x100000 >= b->o.x)
        goto out;
    if (b->o.x >= a->o.x + 0x100000)
        goto out;
    if (b->o.y / 0x10000 != a->o.y / 0x10000)
        goto out;
    if (a->o.z <= b->o.z)
        goto out;
    if (a->o.z - 0x200000 >= b->o.z)
        goto out;
    la = a->o.spr->s.layer;
    if (la > b->o.spr->s.layer) {
        b->o.f23 &= ~1;
        b->o.spr->s.layer = la;
        b->o.spr->s.prio = a->o.spr->s.prio;
    }
    r = 1;
out:
    return r;
}

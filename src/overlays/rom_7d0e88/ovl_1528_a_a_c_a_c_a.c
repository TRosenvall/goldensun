/* ovl_1528_a_a_c_a_c_a -- OvlFunc_947_2009938 and OvlFunc_947_20099f0.
 *
 * LANDING, BOTH FUNCTIONS: whole file byte-identical -- 368 bytes, 178
 * encodings, 0 relocations, 89/89 each.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7d0e88/ovl_1528_a_a_c_a_c_a.c \
 *     asm/overlays/rom_7d0e88/ovl_1528_a_a_c_a_c_a.s --whole
 *
 * SPLIT SHAPE: none. The piece holds exactly these two functions and
 * tools/datacheck.py reports no data, so "split": null and "exports": [].
 * PINS: 0 (tools/shimcount.py). This retires TWO parks in one landing.
 *
 * WHICH .s HOLDS WHICH -- the question that had to be resolved first. The park
 * for 2009938 named `ovl_1528_a_a_c.s` in its title and
 * `ovl_1528_a_a_c_a_c_a.s` in its recipe. The first PATH NO LONGER EXISTS; the
 * piece was split further since that park was written. Both functions are in
 * this one file, 2009938 at the top and 20099f0 below it, so one TU covers
 * both and neither could have been landed alone.
 *
 * THEY ARE REAL TWINS, not two functions that happened to measure the same
 * length. 86 instructions each, the same instruction sequence throughout,
 * differing only in which argument plays which role (the r5/r6 swap) and in
 * the sense of the layer compare:
 *
 *     2009938(a,b): proceed when a's layer is LOWER, and A is updated from B
 *     20099f0(a,b): proceed when a's layer is HIGHER, and B is updated from A
 *
 * Both read a's sprite byte FIRST, and in both the `<< 28` of the value that
 * gets INSERTED is the one kept live across the `+0x23` store.
 *
 * ================= WHAT BOTH PARKS WERE MISSING: ONE `lsr` =================
 *
 * Both read 86 against 85 -- ONE instruction short -- and aligncmp puts the
 * slip in one place: the ROM computes the 2-bit layer TWICE from the kept
 * `<< 28` value, `lsr #30` for the compare and `lsr #30 / lsl #2` for the
 * insert, where a single named layer variable gives one `lsl #2`.
 *
 * THE LEVER: THE COMPARE AND THE INSERT MUST READ THE FIELD THROUGH DIFFERENT
 * TREES. The compare goes through the BITFIELD members; the inserted value
 * comes from a separately named `<< 28` of the PLAIN BYTE view of the same
 * byte. Those are two different expressions when cse runs, so the `>> 30` is
 * never commoned, and the extra `lsr` appears. Writing BOTH through the named
 * `<< 28` -- which is what both parks tried and recorded as inert -- is
 * genuinely inert: cse commons it and the count stays at 85. So does naming
 * the raw byte and shifting at both uses.
 *
 * The third union view, `r.b[]`, exists only to spell that plain byte. It is a
 * view of memory the other two views already describe, not a fiction.
 *
 * ORDER OF EVALUATION IS THE SECOND HALF, and it is the whole difference
 * between the two functions' bodies. The ROM loads a's sprite byte first in
 * BOTH. In 20099f0 the named value IS a's, so `la = ...` before the `if` loads
 * a first and the body matched at once. In 2009938 the named value is b's, so
 * the same shape loaded b first and read 9 of 89 -- the two loads in the wrong
 * order and every register after them following. Reading a's layer into its own
 * variable first, THEN the named `<< 28` of b, puts them back:
 *
 *     ka = a->o.spr->s.layer;
 *     lb = (unsigned int)(b->o.spr->r.b[9] << 28);
 *     if (ka < b->o.spr->s.layer) { ... }
 *
 * and the compare's right operand is then commoned onto `lb >> 30`, which is
 * exactly the ROM's `lsr r3, r1, #30`. Folding that into the condition instead
 * -- an assignment inside the `if` -- reads 34 of 89; comparing `ka` against
 * `lb >> 30` directly also reads 34.
 *
 * WHAT CARRIED OVER FROM THE PARKS, unchanged and right: the single `ret`
 * variable set to 0 before the first compare with every guard a `goto`, which
 * is what puts `mov r0, #0` ahead of the test chain; the signed-negative range
 * literals, which keep the two-sided tests signed (`bge`, not `bcs`); the
 * division by 0x10000 with its rounding correction; the ONE union holding both
 * views, which is what makes the `+0x23` char store invalidate the sprite
 * pointer loads so the ROM's three reloads appear; and the `& 0xc` form for the
 * `+0x15` field copy, which gcc produces for a same-position bitfield-to-
 * bitfield assignment.
 *
 * ALSO REFUTED: 20099f0's park said "the ROM is probably holding a whole-byte
 * or <<28 value in a variable through some spelling not found yet." It is
 * holding a `<< 28` in a variable -- but the spelling that works is the one
 * where only ONE of the two uses goes through it.
 */
typedef union U {
    struct {
        unsigned char pad0[8];
        int x; int y; int z;
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
    struct {
        unsigned char b[0x16];
    } r;
} U;

int OvlFunc_947_2009938(U *a, U *b)
{
    int r;
    unsigned int lb;
    unsigned int ka;

    r = 0;
    if (b->o.x == a->o.x && b->o.y == a->o.y && b->o.z == a->o.z)
        goto out;
    if (b->o.x - 0x100000 >= a->o.x)
        goto out;
    if (a->o.x >= b->o.x + 0x100000)
        goto out;
    if (b->o.y / 0x10000 != a->o.y / 0x10000)
        goto out;
    if (b->o.z <= a->o.z)
        goto out;
    if (b->o.z - 0x200000 >= a->o.z)
        goto out;
    ka = a->o.spr->s.layer;
    lb = (unsigned int)(b->o.spr->r.b[9] << 28);
    if (ka < b->o.spr->s.layer) {
        a->o.f23 &= ~1;
        a->o.spr->s.layer = lb >> 30;
        a->o.spr->s.prio = b->o.spr->s.prio;
    }
    r = 1;
out:
    return r;
}

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
    la = (unsigned int)(a->o.spr->r.b[9] << 28);
    if (a->o.spr->s.layer > b->o.spr->s.layer) {
        b->o.f23 &= ~1;
        b->o.spr->s.layer = la >> 30;
        b->o.spr->s.prio = a->o.spr->s.prio;
    }
    r = 1;
out:
    return r;
}

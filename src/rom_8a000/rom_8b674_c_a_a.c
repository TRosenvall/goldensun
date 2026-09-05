/* asm/rom_8a000/rom_8b674_c_a_a.s -- BOTH functions of the file.
 *
 * The .s held exactly two functions and no data, so this replaces it whole:
 * stage1.ld line 729 keeps naming asm/rom_8a000/rom_8b674_c_a_a.o and the
 * cross-dir rule `asm/%.o: src/%.c` builds that object from this file with the
 * default GCC296_CFLAGS.  No split, no linker edit, no flag group.
 *
 * ---------------------------------------------------------------------------
 * Func_808b868 -- MarkRecordsInBounds.  Walks an array of 0x18-byte records
 * until the halfword at +0 reads -1, and tags every record whose +0x02 flag is
 * still 0 with 0x164 (inside the map window at [iwram_3001e70]+0xEC..+0xF8) or
 * 0x165 (outside).
 *
 * THE WHOLE FUNCTION TURNED ON ONE NUMBER: HOW MANY LOOP-INVARIANT ADDRESSES
 * GET HOISTED.  The ROM builds `g + 0xec` once in the preheader and keeps it in
 * r4, but rebuilds `g + 0xf4`, `g + 0xf0` and `g + 0xf8` inside the loop with
 * `mov r3, r6 / add r3, #K` every iteration.  The obvious C -- all four written
 * the same way -- hoists TWO of them and is 17 encodings out at the same 60
 * instructions.  Every source-level attempt to make the other three
 * un-hoistable measured WORSE, and all of them were the wrong question:
 *
 *   a2  one shared `unsigned int` scratch for the three         58 insns, 38 diff
 *   a3  three separate scratches assigned up front              70 insns, 68 diff
 *   a4  `int *lo = (int *)(g + 0xec);` before the calls         66 insns, 65 diff
 *   a5  three scratches assigned at their use sites             72 insns, 71 diff
 *   a6  same with `unsigned char *` scratches                   72 insns, 71 diff
 *   a7  nested `if`s instead of `&&`                            identical to a1
 *
 * READ FROM loop.c IN THE BUILD IMAGE, NOT INFERRED.  `move_movables` takes a
 * movable when
 *
 *     threshold * savings * m->lifetime >= insn_count
 *
 * and -- this is the part that decides everything here -- it does
 * `threshold -= 3` after EVERY move it makes ("the more regs we move, the less
 * we like moving them", loop.c:1900 and :2104).  With no call in the loop,
 * `threshold = 2 * (1 + n_non_fixed_regs)` = 30, and each of the four address
 * computations has savings 1 and lifetime 1, so the product is just the current
 * threshold.  The moves therefore run 30, 27, 24, ... against a fixed
 * `insn_count`, and the number of addresses hoisted is decided entirely by the
 * SIZE OF THE LOOP IN RTL INSNS:
 *
 *     insn_count <= 27   ->  2 hoists      (what the obvious C gives)
 *     28 <= insn_count <= 30  ->  1 hoist  (what the ROM has)
 *     insn_count >= 31   ->  0 hoists
 *
 * `-dL` prints the whole decision: the .08.loop dump says "not desirable" for
 * all four on pass 1 (33 insns), and after -frerun-loop-opt shrinks the loop to
 * 27 it says "moved to 195", "moved to 196", then "not desirable" twice.  The
 * fix is therefore not a spelling for the three -- it is ONE MORE RTL INSN IN
 * THE LOOP, and it must be loop-VARYING or it will simply be hoisted itself.
 *
 * `px = &r->x; x = *px;` is that insn.  Naming the field address splits one
 * `(mem (plus (reg) 8))` into an address set plus a load; combine folds them
 * back into `ldr r2, [r5, #8]`, so the final instruction stream is unchanged,
 * but at loop time the body is 28 insns and exactly one address is hoisted.
 * Byte-identical.  Three spellings of the same +1 all reached the same object
 * (`&r->x`, `&r->z`, and an `unsigned int` address chain on the x load), which
 * is what confirms the mechanism is the count and not the spelling.
 *
 * NEGATIVE, worth recording: adding the insn as a loop-INVARIANT one does not
 * work.  Writing the 0x164 as `k = 0xb2; k <<= 1;` adds two insns but they form
 * a `consec` movable that loop.c hoists ("Insn 90 ... consec 1 ... moved"),
 * which costs the split build its position and lands 66 instructions.  And a
 * narrowing temporary on the 0x165 store does reach 28 insns, but it also
 * destroys the constant's CSE -- the ROM keeps 0x165 in r7 across both calls
 * and reuses it in the loop -- for 58 instructions and 59 differing.
 *
 * Smaller readings, all confirmed against the ROM:
 *   - bottom test with an entry guard is a plain `while`, not a `do`;
 *   - `ldrsh` has no immediate form, so `mov r2,#K / ldrsh r3,[r5,r2]` is just
 *     a signed `short` field read and says nothing about the source;
 *   - `cmp r3(min), r2(x) / bgt` is `min <= x`, not `x >= min`;
 *   - the in-bounds store is the fall-through arm and the out-of-bounds store
 *     is the branch target, so the `&&` chain is the `if`, not the `else`.
 *
 * ---------------------------------------------------------------------------
 * Func_808b8e8 -- DespawnDistantSceneEntities.  Culls scene slots 8..0x41
 * against a window around the player entity at +0x1E0.
 *
 * Its loop is the same loop as Func_808b98c's in rom_8b674_c_a_b.c, so the
 * body came straight off that solved twin: `actor[0x54] = 1` (indexed, so the
 * pointer is copied before the add and survives for _DeleteActor), and the
 * `int flags` / `unsigned char newflags` pair that keeps regmove's two-operand
 * tie on the byte instead of on the mask.
 *
 * TWO THINGS WERE NEW.  The preheader schedules thirteen instructions and its
 * order is INITIALISER ORDER: with `mask` assigned before `slot` the mask lands
 * in r2 and `mov r5, r1` sinks to the middle of the block (9 differing).
 * Assigning `slot` first, then `mask`, then `i` puts `mov r5, r1` immediately
 * after the pool load and the mask in r1, which is the ROM (2 differing).
 * Three other orderings measured 10, 27 and 5.
 *
 * The last two were `mov r2, #0 / str r2, [r5]`, where gcc picks r3.  After the
 * call every low register is dead, so the choice is pure allocation order and
 * REG_ALLOC_ORDER hands out r3 first; the ROM's r2 is the register the sprite
 * pointer had.  A named `zero`, a named null pointer, and moving the
 * declaration to either end of the list all left it at r3 -- naming the
 * constant does not choose the register.  REUSING THE VARIABLE DOES:
 * `sprite = 0; *slot = sprite;` gives the store the sprite's register and the
 * object is identical.  (Reusing `flags` instead lands 8 differing, so it is
 * the specific variable, not "reuse something".)
 */

extern unsigned char *iwram_3001e70;
extern unsigned int iwram_3001ebc;
extern void _ClearFlag(int id);
extern void _SetFlag(int id);
extern void _DeleteActor(void *actor);

struct Rec {
    short id;
    short flag;
    int unk4;
    int x;
    int unkc;
    int z;
    int unk14;
};

void Func_808b868(struct Rec *r)
{
    unsigned char *g = iwram_3001e70;
    int *px;
    int x, z;

    _ClearFlag(0xb2 << 1);
    _SetFlag(0x165);

    while (r->id != -1) {
        if (r->flag == 0) {
            px = &r->x;
            x = *px;
            z = r->z;
            if (*(int *)(g + 0xec) <= x && x <= *(int *)(g + 0xf4) &&
                *(int *)(g + 0xf0) <= z && z <= *(int *)(g + 0xf8))
                r->flag = 0xb2 << 1;
            else
                r->flag = 0x165;
        }
        r++;
    }
}

void Func_808b8e8(void)
{
    unsigned int base;
    int *player;
    int px, pz;
    int xlo, xhi, zlo, zhi;
    unsigned int mask;
    unsigned char **slot;
    int i;
    int x, z;
    unsigned char *actor;
    unsigned char *sprite;
    unsigned int flags;
    unsigned char newflags;

    base = iwram_3001ebc;
    player = *(int **)(base + (0xf0 << 1));
    px = player[2];
    xlo = px - 0xa00000;
    xhi = px + 0xa00000;
    pz = player[4];
    zlo = pz - 0xc80000;
    zhi = pz + 0x640000;
    slot = (unsigned char **)(base + 0x34);
    mask = -2;
    i = 0x39;

    do {
        actor = *slot;
        if (actor != 0) {
            x = *(int *)(actor + 8);
            z = *(int *)(actor + 0x10);
            if (x != 0 || z != 0) {
                if (x < xlo || x > xhi || z < zlo || z > zhi) {
                    actor[0x54] = 1;
                    sprite = *(unsigned char **)(actor + 0x50);
                    flags = sprite[0x1d];
                    newflags = flags & mask;
                    sprite[0x1d] = newflags;
                    _DeleteActor(actor);
                    sprite = 0;
                    *slot = sprite;
                }
            }
        }
        i--;
        slot++;
    } while (i >= 0);
}

/* Func_80a1a40 -- 0x080a1a40, asm/rom_a1000/rom_a1814_a_c.s
 *
 * Places a cursor: index two byte tables by (iwram_1e40 >> 1) & 7, add the
 * caller's x/y and the window origin scaled by 8, store the result to the
 * cursor's coordinate halfwords and to two bitfields in its attribute word.
 *
 * THE INSTRUCTION STREAM IS EXACT -- tryc reports OK at 49 of 49, and the
 * function assembles to 0x80 bytes, the ROM's exact size, WITH the mid-function
 * literal pool and the `b` that jumps over it reproduced.
 *
 * BLOCKER: POOL WORD ORDER, and nothing else. 0xffff sorts FIRST in the ROM and
 * FIFTH in ours. That moves three pool words and five `ldr` pc-offsets -- five
 * halfwords of code and twelve bytes of pool. Every other byte is identical.
 *
 *   rom   ... e00e 0000 ffff 0000 01ff 0000 1f2c 0300 ...
 *   ours  ... e00e 0000 01ff 0000 0000 0000 0000 0000 ... ffff ...
 *
 * A NEW AND PRECISELY CHARACTERISED PARK CLASS: "the pool entry needs
 * *thumb_zero_extendhisi2 mode". Minipool entries are ordered by the maximum
 * address at which they can still be reached, which is the referencing
 * instruction's address plus its pattern's pool_range. For 0xffff to sort
 * FIRST, its reference must have a NARROW range. Only two patterns are narrow
 * enough -- *thumb_movhi_insn at 64 and *thumb_zero_extendhisi2 at 60 -- and
 * 64 is provably unreachable here.
 *
 * So the ROM's 0xffff must be `(zero_extend:SI (mem:HI <pool>))`, a
 * zero-extending HALFWORD load of a pool constant, which prints as `ldr` via
 * the LABEL_REF branch at arm.md:3050. The max_address arithmetic was computed
 * against the real ROM addresses and reproduces the ROM's pool order exactly,
 * so the inference is tight rather than a guess.
 *
 * WHY NO C SPELLING REACHES IT -- READ from the compiler, not merely swept.
 * A HImode `& 0xffff` DOES NOT EXIST. `simplify_binary_operation` returns op0
 * unchanged when `INTVAL(op1) == GET_MODE_MASK(mode)`, and tree-level `fold`
 * kills `(u16)x & (u16)0xffff` before RTL is even built. Eleven spellings were
 * tried -- a named `t = vx & 0xffff`, casts at the store, u16 locals, u32
 * locals, a `:16` bitfield, `unsigned int` bitfield units, and field
 * read-backs -- and every one either folded the AND away entirely or left it at
 * SImode, where the pool entry sorts last. The mask has to survive to RTL AT
 * HALFWORD WIDTH, and C has no way to ask for that.
 *
 * WHAT WAS WON, and it is most of the function. Three levers took this from 47
 * to 0 on the instruction stream:
 *
 *   - THE BITFIELD IS WHAT PLACES THE POOL. Writing the attribute merge by hand
 *     as `(attr & 0xfffffe00) | (v & 0x1ff)` lets convert_to_integer shorten
 *     the mask to 0xfe00 -- ordinary blocker-1b behaviour. An `int` local for
 *     the merge defeats the shortening and restores `.word 0xfffffe00`, but
 *     leaves both masks SImode, so the pool goes to the END of the function.
 *     Declaring the field `unsigned short a16 : 9` makes store_bit_field emit
 *     the mask at HImode -- `ldrh rN, .LC`, pool_range 64 -- which drags the
 *     minipool UP and makes gcc manufacture the `b` over it. That is the ROM's
 *     shape. This is the missing half of the existing 1b / mid-body-pool note:
 *     THE BITFIELD IS NOT ONLY THE FIX FOR NARROW-CONSTANT MATERIALISATION, IT
 *     IS THE LEVER THAT PLACES THE POOL.
 *
 *   - AN EAGERLY-LOADED POINTER IS FIXED BY GIVING A LATER POINTER AN EARLIER
 *     LIVE RANGE. `st` landed in r4 and coalesced with `cur`; the ROM has st in
 *     r5 and cur in r4. Assigning `cur = st->cur;` immediately after `st` --
 *     even though the ROM loads it fourteen instructions later -- makes the two
 *     conflict, forces st to r5, which makes reload pick r6 rather than r5 as
 *     the low temp for the constant 7, which creates the anti-dependence that
 *     stops sched2 hoisting `ldr r6, [r5, #0x10]`. ONE STATEMENT MOVE: 12 to 2.
 *     Statement order fixes register BIRTH order and the scheduler then
 *     restores the ROM's emission order.
 *
 *   - Splitting `a |= vx & 0x1ff;` into `vx &= 0x1ff;` as its own statement was
 *     worth 20 to 12 -- the documented statement-splitting lever, here on a
 *     mask chain.
 *
 * iwram_3001e40 must be `volatile`: the ROM keeps its address in r14, reloads
 * the value, and recomputes the `>> 1 & 7`.
 *
 * The two byte tables need no asm change -- .Laf294 and .Laf29d are already
 * .global in asm/rom_a1000/rom_a1814_c_c_c_c.s, so the __asm__-named externs
 * link as they stand.
 *
 * TRIED AND LOST, so nobody repeats them: a field read-back with a hand-written
 * merge (47); unsigned short locals, where PROMOTE_MODE gives lsl/lsr rather
 * than an and (41); int locals with an explicit &= 0xffff (25); win and cur as
 * named locals in six placements (20-30); the constant-as-destination
 * spelling (20); *8 rather than << 3 (20); compound accumulation (20); u16
 * locals or casts on the bitfield store (29-31); and -fno-schedule-insns2,
 * which is WORSE at 31 -- sched2 is helping here, not hurting.
 *
 * A NOTE FOR neighbour.py: it returned an 8-way tie on the single global
 * iwram_3001e40, all in a different directory with different conventions.
 * Applying the documented N-way-tie rule to the OTHER named global,
 * iwram_3001f2c, gave ~40 same-directory siblings, one of which already
 * declared the exact struct this function needs. SUGGESTED REFINEMENT: rank a
 * shared global by how FEW files use it, and prefer same-directory hits -- a
 * global shared with a distant overlay family is noise.
 */

struct Win {
    unsigned char pad00[0xc];
    unsigned short fc;
    unsigned short fe;
};

struct Cur {
    unsigned char pad00[6];
    unsigned short x;
    unsigned short y;
    unsigned char pad0a[0xa];
    unsigned short b14 : 8;
    unsigned short : 8;
    unsigned short a16 : 9;
};

struct State {
    unsigned char pad00[0x10];
    struct Win *win;
    struct Cur *cur;
};

extern struct State *iwram_3001f2c;
extern volatile unsigned int iwram_3001e40;
extern unsigned char Laf294[] __asm__(".Laf294");
extern unsigned char Laf29d[] __asm__(".Laf29d");

void Func_80a1a40(int x, int y)
{
    struct State *st;
    struct Cur *cur;
    int vx;
    int vy;

    st = iwram_3001f2c;
    cur = st->cur;
    vx = Laf294[(iwram_3001e40 >> 1) & 7] + x + (st->win->fc << 3) + 8;
    cur->x = vx;
    vx &= 0xffff;
    cur->a16 = vx;
    vy = Laf29d[(iwram_3001e40 >> 1) & 7] + y + (st->win->fe << 3) + 8;
    cur->y = vy;
    vy &= 0xffff;
    cur->b14 = vy;
}

/* Func_801c154 (ReleaseScreenTiles) -- 0x0801c154.  *** LANDS.  BYTE-IDENTICAL. ***
 * Batch 328 brief D.  The park read 8 of 17 and had read 8 for six batches.
 *
 * 0 differing encodings of 17.  objcmp --func:
 *   OK Func_801c154 -- 40 bytes, 17 encodings and 1 relocations identical
 * objcmp --whole:
 *   OK whole file -- 40 bytes, 17 encodings and 1 relocations identical
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_15000/rom_1aeec_c_a_a_a_a_a_a_c.c asm/rom_15000/rom_1aeec_c_a_a_a_a_a_a_c.s --whole
 *
 * SPLIT SHAPE: NONE.  grep -c thumb_func_start on the reference is 1;
 * tools/datacheck.py prints nothing and exits 0; split_s.py has nothing to do.
 * INSTALL PATH: src/rom_15000/rom_1aeec_c_a_a_a_a_a_a_c.c.
 * PINS: 0.  No flag group, no device, no symbol, no shim.
 *
 * ======================================================================
 * IT CAME FROM THE LANDED MODULE-MATES.  THE FUNCTION IS A BITFIELD STORE.
 * ======================================================================
 *
 * tools/upstream_module.py puts this in rom_15000/rom_1aeec.s with 34 landed
 * siblings.  The sibling IMMEDIATELY BEFORE it in the same object,
 * src/rom_15000/rom_1aeec_c_a_a_a_a_a_a_b.c (Func_801c0dc), declares
 * `struct OamSprite` with `y:8 ... x:9 ...`, and the module's
 * DisplayMenuArrowCursor / DisplayMenuArrowCursor2 parks carry the same struct
 * and the same `Func_8003dec(o, N)` tail.  Written as that struct, this
 * function is three statements:
 *
 *     o->x = x;  o->y = y;  Func_8003dec(o, 0xfc);
 *
 * and every instruction the park was fighting falls out of ONE bitfield store:
 *   `x:9` at bit 0 of the halfword at offset 6 ->  ldr r3,=0x1ff / ldrh r4,[r0,#6]
 *                                                  and r1,r3 / ldr r3,=0xfffffe00
 *                                                  and r3,r4 / orr r3,r1 / strh
 *   `y:8` at byte 4                            ->  strb r2,[r0,#4]
 *
 * TWO STANDING CLAIMS IN THE OLD PARK HEADER ARE REFUTED BY THIS LANDING.
 *
 * (1) THE POOL-PLACEMENT HALF WAS NOT A BOUND.  The park had it as 4 of the 8
 *     encodings and bounded by a freshly re-measured corpus floor -- "there is
 *     no pool-before function in this tree below 23 instructions whose pool
 *     loads are all `ldr`", against this function's 14.  The corpus statistic
 *     is real and was correctly measured; it was never a constraint on THIS
 *     function.  The bitfield store emits the ROM's instruction count AND the
 *     ROM's `b .L1c178` over the pool.  The park's own self-refutation
 *     argument ("length causes placement is circular here") was right that the
 *     `b` is the only length difference and wrong to conclude the placement
 *     was therefore the cause: both were symptoms of a hand-rolled struct.
 *
 * (2) THE RENAME HALF WAS NOT A regmove PROBLEM EITHER -- but the regmove
 *     reading is CORRECT and is worth keeping, because it is a rung this
 *     project had not named on any target.  Recorded in full below so the
 *     mechanism is not lost with the park.
 *
 * ======================================================================
 * THE regmove RUNG, READ IN THE COMPILER (keep; new to the tree)
 * ======================================================================
 *
 * The park observed, correctly, that `.13.combine` produced the ROM's form
 *     (insn 21 (set (reg/v:SI 33) (and (reg/v:SI 33) (reg:SI 36))))  dest = v
 * and `.15.regmove` rewrote it to
 *     (insn 21 (set (reg:SI 36)   (and (reg/v:SI 33) (reg:SI 36))))  dest = mask
 * moving the REG_DEAD note with it.  `-dr` prints
 *     Could fix operand 2 of insn 21 matching operand 0.
 *     Fixed operand 2 of insn 21 matching operand 0.
 *
 * THE DECIDER IS `replacement_quality`, NOT A PRIORITY AND NOT AN ALLOCATION.
 * regmove's FORWARD pass (regmove.c:1200-1205) skips the rewrite of a
 * two-address commutative insn only when the commutative partner already
 * matches the destination AND
 *     replacement_quality (comm) >= replacement_quality (src)
 * `replacement_quality` (regmove.c:341-368) returns
 *     3  the pseudo is not copied from any register
 *     2  copied from a PSEUDO
 *     1  copied from a HARD register
 *     0  REG_LIVE_LENGTH < 0
 * and `regno_src_regno[]` is filled in the same forward scan
 * (regmove.c:1104-1123) only for a reg-reg copy whose SOURCE DIES there.
 *
 * So on the park's body a PARAMETER -- `(set (reg 33) (reg:SI 1 r1))` with r1
 * dead -- scores 1, and a pooled-constant mask scores 3.  1 >= 3 is false, so
 * regmove swaps the destination onto the mask.  On the SECOND and, both
 * operands scored 3 (a zero_extend load and a constant), 3 >= 3 held, and
 * regmove left it alone -- which is why the park's two ands were wrong in
 * OPPOSITE directions.  GENERAL FORM, for any park whose residue is a
 * two-address destination on the wrong operand:
 *
 *   >> A PARAMETER'S PSEUDO IS replacement_quality 1, THE WORST NON-ZERO
 *      SCORE, SO regmove WILL ALWAYS MOVE A TWO-ADDRESS DESTINATION OFF A BARE
 *      PARAMETER AND ONTO A CONSTANT OR A LOADED VALUE.  If the ROM keeps the
 *      destination on the parameter, the parameter is probably NOT what the
 *      source named there -- look for a construct that makes the value
 *      something other than a bare incoming copy.  Here that construct was a
 *      bitfield store, where `store_bit_field` builds the and itself. <<
 */
#include "gba/types.h"

struct OamSprite {
    unsigned char pad[4];
    /* attr0 */
    unsigned int y:8;
    unsigned int affineMode:2;
    unsigned int objMode:2;
    unsigned int mosaic:1;
    unsigned int bpp:1;
    unsigned int shape:2;
    /* attr1 */
    unsigned int x:9;
    unsigned int matrixNum:5;
    unsigned int size:2;
    /* attr2 */
    unsigned int tileNum:10;
    unsigned int priority:2;
    unsigned int paletteNum:4;
};

extern void Func_8003dec(struct OamSprite *o, int n);

void Func_801c154(struct OamSprite *o, int x, int y)
{
    o->x = x;
    o->y = y;
    Func_8003dec(o, 0xfc);
}

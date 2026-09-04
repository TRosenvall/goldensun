/* Func_80a3c08 -- 0x080a3c08  [asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_c_a_a.s]
 *
 * NOT MATCHING. Best 20 of 62, and OURS IS EXACTLY 62 LINES -- no length error,
 * no missing or extra instruction, only register choice and schedule. The .s
 * holds this and Func_80a38d0; no split was done, since the split would be
 * wasted until the body lands.
 *
 * Refresh the equip-menu sprites: for each of the party members counted at
 * +0x219, ask _CanEquipItem whether the item in the slot at +0x208 fits the
 * unit id masked out of the halfword at +0x178, and set that member's sprite to
 * anim 3 or anim 1.
 *
 * WHAT IS ALREADY RIGHT, and it is most of the function. The `signed char`
 * loop counter reproduces the ROM's packed form exactly -- gcc keeps it shifted
 * left 24 in r7, reads it with `asr r5, r7, #0x18` and steps it with
 * `add r3, r7, #0x1000000` -- which is the idiom the sibling
 * src/rom_a1000/rom_a1814_c_a_c_c_c_c_a_c_a_b.c already matched with. The
 * guard-then-do-while shape, both `and` destinations (the VALUE for the 0x1f
 * test, the CONSTANT for the 0x1ff mask), the pooled 0x1ff, and every call are
 * right. The first divergence is at instruction 17.
 *
 * BLOCKER CLASS: three scheduling residues, none of which moved.
 *
 *  1. GCC COMMONS THE TWO BASE OFFSETS. 0x208 and 0x178 differ by 0x90, and gcc
 *     builds one then derives the other (`sub r2, #0x90`, or `add r2, #0x90`
 *     depending on spelling) where the ROM builds both independently as
 *     `mov #0x82 / lsl #2` and `mov #0xbc / lsl #1`, interleaved with each
 *     other and with the index shift.
 *  2. THE TWO ARMS HAVE EACH OTHER'S SCHEDULE. The ROM's anim-3 arm is
 *     `mov r2,#0x8a / lsl r3,r5,#2 / lsl r2,#1 / add r3,r2` -- constant and
 *     index interleaved -- and its anim-1 arm is `mov r1,#0x8a / lsl r1,#1 /
 *     lsl r3,r5,#2 / add r3,r1`, constant built first. Ours emits the second
 *     shape in the first arm and the first shape in the second.
 *  3. The `+ 1<<24` increment picks r1 where the ROM picks r2, and issues the
 *     `ldr =0x219` on the other side of the add.
 *
 * MEASURED, and the result is the reason this is a park rather than a wall.
 * SIX spellings, of which four tie at EXACTLY 20 differing lines:
 *
 *     inline offsets (this file)                          20
 *     item read as ((unsigned short *)p)[0x104 + i]       20
 *     a separate `off` variable per arm                   20
 *     the second arm's constant added in its own statement 20
 *     named `off` locals for the item load            41  (and one line SHORT)
 *     named offset constants with a barrier between   69  (much worse)
 *
 * docs/elevation.md's own warning applies to the first four: a tie is evidence
 * of a wall only when the spellings differ STRUCTURALLY, and those four all
 * share the assumption that the body is straight-line C with the arithmetic
 * written inline. That assumption is the next thing to vary -- NOT another
 * permutation of it. The named-offset result is the interesting one, because it
 * is the house style the matched sibling uses and here it is strictly worse,
 * which says the two functions reach their offsets differently.
 *
 * The order that DID help is recorded in the body: reading the item into `it`
 * BEFORE computing the mask took 24 to 20 and moved the first divergence from
 * 16 to 17, because the ROM loads the item first and gcc otherwise evaluates
 * the mask first.
 */
extern int iwram_3001f2c;
extern int iwram_3001e40;
extern int _CanEquipItem(int unit, int item);
extern void _Sprite_SetAnim(void *sprite, int anim);

void Func_80a3c08(void)
{
    char *p;
    signed char i;
    int t;
    int m;
    int it;
    int off;

    p = (char *)iwram_3001f2c;
    t = iwram_3001e40;
    t &= 0x1f;
    if (t != 0)
        return;
    if (*(unsigned char *)(p + 0x219) == 0)
        return;
    i = 0;
    do {
        it = *(unsigned short *)(p + 0x82 * 4 + i * 2);
        m = 0x1ff;
        m &= *(unsigned short *)(p + 0xbc * 2);
        if (_CanEquipItem(it, m) != 0) {
            off = 0x8a * 2 + i * 4;
            _Sprite_SetAnim(*(void **)(p + off), 3);
        } else {
            off = 0x8a * 2 + i * 4;
            _Sprite_SetAnim(*(void **)(p + off), 1);
        }
        i++;
    } while (i < *(unsigned char *)(p + 0x219));
}

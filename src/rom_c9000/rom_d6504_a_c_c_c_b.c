/* Func_80d6750 -- 0x080d6750, split out of asm/rom_c9000/rom_d6504_a_c_c_c.s.
 *
 * Builds the battle overlay list: walks either the enemy ids (0x80..0x85) or
 * the party ids (0..7), keeps the ones whose unit is alive, terminates the list
 * with 0xff and hands it to _CreateBattleSpriteOverlays.
 *
 * `i != N` RATHER THAN `i < N` IS THE WHOLE RESIDUE, and it is worth recording
 * because it fixed TWO things that looked unrelated.
 *
 *     for (i = 0; i < 6; i++)     add r6,#1 / cmp r6,#5 / ble   and
 *                                 the id strength-reduced into its own IV
 *                                 (`add r5, r5, #1`)
 *     for (i = 0; i != 6; i++)    add r6,#1 / cmp r6,#6 / bne   and
 *                                 the id RECOMPUTED each iteration
 *                                 (`mov r5, r6 / add r5, #0x80`) -- the ROM
 *
 * The exit test is the obvious half. The second is not: with the `<` bound gcc
 * turns `i + 0x80` into a second induction variable, and with the `!=` bound it
 * leaves it as an ordinary expression recomputed in the body -- which costs an
 * instruction per iteration and is what the ROM has. ONE loop bound, four
 * encodings, in both loops at once.
 *
 * MEASURED AND INERT, all 4: `unsigned int i`, the id named in its own local,
 * both of those together. MEASURED AND WORSE: `do { } while (i != 6)` is four
 * bytes SHORT (the rotation drops the entry test), `(i | 0x80)` instead of
 * `i + 0x80` is eight bytes LONG (gcc rebuilds 0x80 in the body rather than
 * adding it), and -fno-strength-reduce is 54 instructions against 66 -- so
 * strength reduction is WANTED here, just not on this expression.
 *
 * The pooled 0xff is the documented halfword exception, not a symbol: it is
 * stored into a `u16` array, gcc narrows it to HImode, and a HImode constant
 * goes to the pool. gcc's text says `ldrh r3, .L17` which GAS folds to the
 * ROM's `ldr`. No const.sym entry is warranted.
 *
 * tools/tryc.py reports its pool-placement warning here -- the reference keeps
 * its pool inside the function body -- and `make compare` settles it.
 * tools/datacheck.py confirms the source .s carries no data section.
 *
 * EXACT: 140 bytes, 66 encodings, 3 relocations, measured three times.
 */
#include "gba/types.h"

struct AnimCtx {
    u8 pad00[0x24];
    s16 f24;
};

extern u8 *_GetUnit(s32 id);
extern void _CreateBattleSpriteOverlays(u16 *list, int b);

void Func_80d6750(struct AnimCtx *c)
{
    u16 list[14];
    int n;
    int i;

    n = 0;
    if (c->f24 > 0x7f) {
        for (i = 0; i != 6; i++) {
            if (*(s16 *)(_GetUnit(i + 0x80) + 0x38) > 0) {
                list[n] = i + 0x80;
                n++;
            }
        }
    } else {
        for (i = 0; i != 8; i++) {
            if (*(s16 *)(_GetUnit(i) + 0x38) > 0) {
                list[n] = i;
                n++;
            }
        }
    }
    list[n] = 0xff;
    _CreateBattleSpriteOverlays(list, 0);
}

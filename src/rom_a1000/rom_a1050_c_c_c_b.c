/* Func_80a153c -- 0x080a153c, split out of asm/rom_a1000/rom_a1050_c_c_c.s.
 *
 * Draws the two-line stat block: a label, a caption, and four numbers, with the
 * current-HP figure recoloured when it is low or zero.
 *
 * ONE LEVER, THREE TIMES: EVERY VALUE PASSED TO Func_80a14f0 IS A NAMED LOCAL.
 * The ROM loads each halfword into r5 and then moves it to r0 for the call:
 *
 *     rom    ldrsh r5, [r7, r3] / mov r1, r6 / mov r0, r5 / ...
 *     ours   ldrsh r0, [r7, r3] / mov r1, r6 / ...
 *
 * Passing `s->f34` straight as the argument loads it into r0 and saves the
 * `mov` -- one instruction short per site, three sites, six bytes. Naming the
 * value first reproduces the ROM exactly. The first candidate was 23 differing
 * and four bytes short; naming one of the three got two bytes back; naming all
 * three is exact.
 *
 * THE SPLIT NEEDED AN EXPORT FIRST, and tools/split_s.py said so rather than
 * producing a broken tree: the function references .Laf210, .Laf214 and .Laf218
 * in the .s's .rodata block, and a `.L` symbol does not survive into the
 * object's symbol table. The exports were added and `make compare` gated GREEN
 * BEFORE the split, so the two changes stay separable -- which is what the tool
 * asks for. `.global` emits no bytes.
 *
 * The data block itself rode with the trailing part (rom_a1050_c_c_c_c.s, which
 * still holds Func_80a15f0), so this half needs no rehoming;
 * tools/datacheck.py confirms it.
 *
 * ONE READING WORTH KEEPING: `ldrh r3, [r7, #0x34] / lsl r3, #16 / asr r3, #18`
 * is `s->f34 >> 2` on a SIGNED halfword -- gcc sign-extends by shifting left 16
 * and folds the >> 2 into the same `asr` as >> 18. It is not a separate mask
 * and not an unsigned shift, and the field is read a SECOND time here after
 * already being loaded into a local above, which is what two distinct source
 * expressions give.
 *
 * EXACT: 180 bytes, 76 encodings, 14 relocations, measured three times, clean on
 * tools/tryc.py.
 */
#include "gba/types.h"

struct Stats {
    u8 pad00[0x34];
    s16 f34;
    s16 f36;
    s16 f38;
    s16 f3a;
};

extern unsigned char L_af210[] __asm__(".Laf210");
extern unsigned char L_af214[] __asm__(".Laf214");
extern unsigned char L_af218[] __asm__(".Laf218");
extern void _Func_801e8b0(void *s, void *w, int x, int y);
extern void _UIDrawText(void *s, void *w, int x, int y);
extern void Func_80a14f0(int v, void *w, int x, int y);
extern void _SetTextColor(int c);

void Func_80a153c(struct Stats *s, void *w)
{
    int v;
    unsigned char *t;

    _Func_801e8b0(L_af210, w, 0, 0x28);
    t = L_af214;
    _UIDrawText(t, w, 0x30, 0x28);
    v = s->f34;
    Func_80a14f0(v, w, 0x58, 0x28);
    v = s->f38;
    if (v < (s->f34 >> 2))
        _SetTextColor(4);
    if (v == 0)
        _SetTextColor(2);
    Func_80a14f0(v, w, 0x30, 0x28);
    _SetTextColor(0xf);
    _Func_801e8b0(L_af218, w, 0, 0x30);
    _UIDrawText(t, w, 0x30, 0x30);
    v = s->f3a;
    Func_80a14f0(v, w, 0x30, 0x30);
    v = s->f36;
    Func_80a14f0(v, w, 0x58, 0x30);
}

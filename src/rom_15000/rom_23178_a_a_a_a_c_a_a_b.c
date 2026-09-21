/* Cluster Func_8026fa8..Func_8026fa8 extracted from goldensun/asm/rom_15000/rom_23178_a_a_a_a_c_a_a.s.
 *
 * Total .text for this TU = 196 bytes (= 0xc4). Never attempted before batch 277.
 * No pins, no flags.
 *
 * `fold_truthop` FOLDS A THREE-TERM COMPARISON CHAIN INTO A RANGE TEST, AND `unsigned char`
 * DOES NOT BLOCK IT.
 *
 * `st != 2 && st > 2 && st <= 4` becomes `sub r3, #3 / cmp r3, #1 / bhi` against the ROM's
 * three separate compares. The obvious defence fails: declaring `st` as `unsigned char` --
 * on the theory that an unstrippable QImode-to-SImode NOP_EXPR would fail
 * `simple_operand_p` -- does NOT work, because gcc simply folds in QImode instead
 * (`add r3, #0xfd`, 76 differing).
 *
 * Nested `if`s DO block the fold and give the ROM's three compares, but gcc then HOISTS the
 * shared `ok = 0` above the compares and points the failing branches at the join rather
 * than at the assignment -- 87 lines against 86. Measured: nested ifs with three `ok = 0`
 * blocks 87/76; `ok = 0;` first plus nested ifs 85/74; an `else if` chain 83/75.
 *
 * SO THE RULE IS: a ROM shape of "N forward branches into a SHARED TAIL ASSIGNMENT that the
 * success path jumps over" needs explicit `goto`s. Nothing structured reaches it, because
 * every structured form lets gcc choose where the shared assignment lives. The exact
 * spelling is the one below:
 *
 *     if (st == 2) goto fail;
 *     if (st <= 2) goto fail;
 *     ok = 1;
 *     if (st <= 4) goto have;
 *   fail:
 *     ok = 0;
 *   have:
 *
 * That sits alongside the recorded control-flow rules rather than replacing them: "put the
 * early-exit constant in the TEXTUALLY LAST block" is the same instinct for a single exit,
 * and `if (c) goto L;` cannot place L before its dominator. This is the multi-predecessor
 * case, and the `goto`s are what pin the shared assignment below all of its branches.
 *
 * The buffer is `u16 buf[0x40]` on the frame -- read the frame size, not a sibling's array
 * bound, for that.
 */
#include "gba/types.h"

extern unsigned char *iwram_3001e74;
extern unsigned char *iwram_3001f34;
extern unsigned char gState[];
extern unsigned int gKeyPress;

extern int CreateUIBox(int a, int b, int c, int d, int e);
extern void Func_8016738(void);
extern int Func_801965c(int a, u16 *out, u32 n);
extern void Func_8017aa4(void *buf, int b, int c, int d);
extern void WaitFrames(int n);
extern int CloseUIBox(int h, int n);

int Func_8026fa8(void)
{
    u16 buf[0x40];
    unsigned char *p;
    int box;
    int st;
    int ok;

    p = iwram_3001e74;
    st = gState[0x22b];
    if (st == 2)
        goto fail;
    if (st <= 2)
        goto fail;
    ok = 1;
    if (st <= 4)
        goto have;
fail:
    ok = 0;
have:
    if (ok == 0 && *(signed char *)(p + 0x43) != 0)
        ok = 1;
    if (ok != 0) {
        box = CreateUIBox(0, 7, 0x1e, 4, 0x2a);
        Func_8016738();
        Func_801965c(0x845, buf, 0x34);
        Func_8017aa4(buf, box, 0, 4);
        do {
            WaitFrames(1);
        } while ((gKeyPress & 3) == 0 && *(int *)(iwram_3001f34 + 0x4c) != 0);
        CloseUIBox(box, 1);
    }
    return ok;
}

/* Func_802106c (0x0802106c) -- NON-MATCHING, 23 differing of 194.
 * ===== BATCH 335 RE-MEASUREMENT.  141 differing encodings of 193 (STREAM; ref
 * 193, ours 195 -- ours TWO longer).  Aligned-equal one hundred and eighty-five
 * of one hundred and ninety-three, ELEVEN differing in eight hunks.  Instructions
 * one hundred and sixty-three against ours one hundred and sixty-four, so we emit
 * ONE extra.  Body unchanged; this block is the current reading. =====
 *
 * THE WHOLE RESIDUE IS ITEM (b) AND NOTHING ELSE.  Of the eight aligned hunks,
 * SIX are branch displacements or literal-pool offsets that shift because the
 * body is longer, one is the trailing `.short 0x0000` pad that the longer body
 * forces, and ONE is the actual difference:
 *
 *     rom   add r3, sp, #0xc / str r0, [r3] / ldrh / ldrh / mov r8, r3
 *     ours  mov r3, #12 / ldrh / ldrh / add r3, r3, sp / mov r8, r3 / ... str
 *
 * One extra instruction plus one pad is the two-encoding stream gap, exactly.
 * Items (a) and (c) of the header's itemised twenty-three are NOT separately
 * visible any more: (a) is inside the pool offsets and (c) is already correct.
 * So this park is ONE INSTRUCTION from the message.sym ruling, not twenty-three
 * from anywhere.
 *
 * *** THE HEADER'S MECHANISM FOR (b) IS REFUTED BY THE ENCODINGS, AND THE
 * REPLACEMENT IS BETTER NEWS. *** It says reload splits the add "BECAUSE THE
 * PSEUDO IS ALLOCATED TO r8, and `add rd, sp, #imm` needs a LOW destination".
 * Our destination IS low -- `add r3, r3, sp` writes r3 -- and BOTH sides carry
 * the same `mov r8, r3` afterwards, so the high register is not what is being
 * worked around.  What we fail to select is the one-instruction PATTERN.
 *
 * `*thumb_addsi3` (arm.md:496-519) has seven alternatives.  Operand triples,
 * read down the columns: alternative FIVE is `=l` / `!k` / `!M`, which is
 * `add Rd, SP, #imm` and is the reference's instruction; the fallback we get is
 * alternative THREE, `*r` / `*0` / `*h`, which is `add %0, %0, %2` with the stack
 * pointer as the high source operand -- and it needs the twelve built in a
 * register first, which is the extra instruction.  THE OPERATIVE DETAIL IS THE
 * `!`: alternative five disparages BOTH operands severely, so it survives only
 * while the insn needs NO RELOAD AT ALL.  The output reload that copies the
 * address into its r8 home is enough to abandon it.
 *
 * SO THE HEADER'S OBSERVATION WAS RIGHT AND ITS REASON WAS WRONG, WHICH MAKES
 * THE NEXT ROUTE DIFFERENT.  The observation that matters is the one the header
 * states and then does not use: THE REFERENCE'S ADDRESS IS TWO PSEUDOS -- a low
 * one that the store goes through and dies, and the r8 copy that lives.  Ours is
 * ONE pseudo with an output reload.  The target is therefore not a spelling of
 * the address but a LIVE-RANGE SPLIT, and it has to come from somewhere other
 * than writing `&obj` twice, because that is now measured (below).  Read it with
 * the batch-277 priority formula as the header already says.
 *
 * SEVEN MORE SPELLINGS MEASURED THIS BATCH, ALL CROSSED, NONE REACHING IT --
 * aligned-equal of one hundred and ninety-three, against the shipped body's one
 * hundred and eighty-five:
 *     `*(int *)q = t` in place of `q->f0 = t`            one hundred eighty-five  INERT
 *     `obj.f0 = t` with `&obj` passed, q named after     one hundred eighty-five  INERT
 *     q named ONCE before the if, removed from both arms one hundred fifty-six    WORSE
 *     that, crossed with the cast store                  one hundred fifty-six    WORSE
 *     a short-lived `w` for the store, q re-taken         one hundred fifty-three  WORSE
 *     the same with the call taking `w`                   one hundred fifty-one    WORSE
 *     `obj.f0 = t` sp-relative, address only for the call one hundred fifty-three  WORSE
 * Added to the header's eight, the pointer-naming dimension now has FIFTEEN
 * negatives.  It is exhausted; stop varying it.
 *
 * AND THE PIECE SHAPE IS NOT WHAT THIS HEADER SAYS.  It claims four functions
 * naming Func_8020b64 first.  `grep '^\.thumb_func_start'` returns THREE:
 * UI_NameEntry at 0x08020bd8, this function, then Func_8021228.  UI_NameEntry is
 * parked at four hundred and eighty-eight of five hundred and twenty-five, so the
 * piece cannot convert whole; the one workable cut is `split_s.py` on
 * UI_NameEntry, which leaves this function and Func_8021228 in a single part and
 * therefore a single translation unit.  Do not run it before the two message.sym
 * rulings land, or the reference is consumed for a landing that cannot happen.
 *
 * NON-MATCHING, 141 of 193 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 193, ours 195) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 444 bytes, ours 448.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/802106c.c \
 *     asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c_c.s --func Func_802106c
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: one build-input symbol, plus a reload split on an r8-allocated frame address.
 * Never attempted before batch 277.
 *
 * asm/rom_15000/rom_20198_c_c_c_a_a_a_a_c_c.s (4 functions: Func_8020b64, UI_NameEntry,
 * Func_802106c, Func_8021228 -- so landing needs a split). Func_8021228 is parked at
 * src/non_matching/rom_15000/8021228.c, and ITS `struct S` + `q = &s` note is what gave this
 * function's sprite-record shape. A park is a file-mate source, again.
 *
 * READ THE TWO CANDIDATES IN THE RIGHT ORDER -- THE LOWER COUNT IS NOT THE BETTER OBJECT.
 * The candidate below is 23 differing, and its THREE literal pools and all 23 relocations
 * match the ROM in kind and order, offset by 4 bytes. A sibling candidate reaches 12
 * differing but its pool collapses to one at the end and the Data_310a4 relocation moves from
 * 0x9c to 0x198. Ranking on the count alone would pick the wrong one, which is the
 * count-is-not-a-difference rule applied to candidate SELECTION rather than to a flag.
 *
 * ===== THIS FUNCTION WANTS `_MSG_2080 = 0x2080;` IN message.sym, AND IT IS NOT ADDED. =====
 *
 * The evidence is the STRONG structural kind, not the weaker register argument: 0x2080 is
 * shiftable (0x41 << 7, equivalently 0x82 << 6 -- verified at landing), so
 * thumb_shiftable_const means gcc BUILDS it, and a candidate written with a plain int was
 * measured doing exactly that (`mov r0,#0x82 / lsl r0,#6`). The ROM POOLS it and then reaches
 * the next two ids by arithmetic off the held register (`add r0, r5, #1` and `add r5, #2`).
 * A pool word holding a shiftable value cannot have come from a const_int at all. That is the
 * same signature message.sym documents for _MSG_242e / _MSG_2430.
 *
 * The gap is real: the nearest named neighbours are _MSG_1ff1 below and _MSG_217f above, so
 * 0x2080 is in unnamed space. 0x2081 and 0x2082 are reached by arithmetic and must NOT be
 * named.
 *
 * WHY IT IS NOT ADDED: the batch-272 rule is that an entry is worth adding when it COMPLETES
 * a function, and this one does not -- it is worth 73 -> 67, and 23 differ after every other
 * lever. That is the same decision taken in batch 275 for _TBL_7a828 and _SIZE_80f0024, both
 * of which also had verified evidence and also failed to complete their functions. Adding
 * this one and not those would be drift, so all three are flagged together for the user
 * rather than decided unilaterally. The measurement here used a local
 * `__asm__(".set _MSG_2080, 0x2080")` stand-in, which resolves the pool word identically; the
 * real entry replaces that line.
 *
 * OTHER LEVERS THAT LANDED:
 *   * `extern volatile int gKeyRepeat;` -- 67 -> 57. Without it the address is not
 *     LICM-hoisted into the preheader, so it is rematerialised per read and r9/r11 go to the
 *     wrong values. This is the established volatile-hardware-register declaration (six
 *     landed files declare gKeyPress the same way and none is a fakematch), NOT scaffolding.
 *   * `do { ... } while ((gKeyRepeat & 1) == 0);` with the `& 2` early exit as a `goto` rather
 *     than `for(;;)` plus two `break`s -- 48 -> 23. The `for(;;)` form rotates the `&1` test
 *     to the loop top and gcc then moves the WRONG one of the two sound blocks out of line.
 *   * Naming the call result before taking the record address: `t = Func_801eadc(...);
 *     q = &obj; q->f0 = t;` -- 57 -> 48.
 *
 * THE REMAINING 23, ITEMISED:
 * (a) the `=_MSG_2080` pool word, above.
 * (b) `&obj` in the taken branch costs three instructions (`mov r3,#0xc / add r3,r3,sp /
 *     mov r8,r3`) against the ROM's two (`add r3,sp,#0xc / mov r8,r3`). RELOAD SPLITS IT
 *     BECAUSE THE PSEUDO IS ALLOCATED TO r8, and `add rd, sp, #imm` needs a LOW destination;
 *     the ROM's low-register `add` plus copy means the address is two pseudos there. An
 *     isolated four-case probe emits the single `add rd, sp, #imm` every time, and eight
 *     in-function spellings (two pointers, `q = p`, `obj.f0 =`, `*(int*)q =`, `q` as the first
 *     statement, `q` after the alloc call, no `q` at all) did not separate them -- and THE
 *     ELSE BRANCH OF THE SAME FUNCTION GETS IT RIGHT, which is what proves it is allocation
 *     rather than spelling.
 * (c) `pal[4] = 0x6318` through a `u16 *` makes a HImode pool entry. A named `int` fixes the
 *     load width to the ROM's `ldr`, but being the minipool head with the tightest range it
 *     is ALSO what pins the pool site -- removing it defers all three pools to one at the
 *     end. That is the Func_801f77c park's "pool position is downstream" entry reappearing,
 *     now with the added sign that A HImode ENTRY CAN BE LOAD-BEARING FOR THE POOL LAYOUT, so
 *     the two halves have to be solved together rather than in either order.
 *
 * NEXT: (b), which is an r8 allocation, and it should be read with the batch-277 priority
 * formula (floor_log2(refs) * refs / live_length) rather than swept.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern volatile int gKeyRepeat;
extern int _MSG_2080;
__asm__(".set _MSG_2080, 0x2080");
extern unsigned char Data_310a4[];

struct Spr { int f0; int f4; int f8; int fc; };

extern unsigned short *CreateUIBox(int a, int b, int c, int d, int e);
extern void Func_801e7c0(int a, unsigned short *b, int c, int d);
extern int AllocSpriteSlot(void);
extern void UploadSpriteGFX(int slot, int size, unsigned char *gfx);
extern int Func_801eadc(int a, unsigned int b, unsigned short *c, int d, int e);
extern void _Func_80b0a20(struct Spr *o, int x, int y);
extern void _Func_80b09fc(struct Spr *o, int x, int y, int d);
extern void _Func_80b08b8(struct Spr *o);
extern void Func_8020a60(unsigned short *box, int b, int c, int d, int e, int f);
extern void _PlaySound(int n);
extern void WaitFrames(int n);
extern void CloseUIBox(unsigned short *box, int mode);
extern void Func_8003f3c(int slot);

int Func_802106c(void)
{
    unsigned short *box;
    unsigned short *pal;
    struct Spr obj;
    struct Spr *q;
    int slot;
    int sel;
    int redraw;
    int m;
    int t;

    redraw = 1;
    box = CreateUIBox(7, 0xd, 0x12, 7, 2);
    m = (int)&_MSG_2080;
    Func_801e7c0(m, box, 8, 0);
    Func_801e7c0(m + 1, box, 8, 0x10);
    Func_801e7c0(m + 2, box, 8, 0x20);
    slot = AllocSpriteSlot();
    sel = 0;
    if (slot <= 0x5f) {
        UploadSpriteGFX(slot, 0x80, Data_310a4);
        t = Func_801eadc(slot, 0x40000000, box, 0, 0);
        q = &obj;
        q->f0 = t;
        _Func_80b0a20(q, box[6] * 8 - 3, box[7] * 8 + 9);
    } else {
        q = &obj;
    }
    pal = (unsigned short *)0x50001c0;
    DMA3_COPY((void *)0x50001e0, pal, 0x20);
    pal[4] = 0x6318;
    do {
        Func_8020a60(box, 1, sel * 2, 0xe, 1, 0xe);
        WaitFrames(1);
        Func_8020a60(box, 1, sel * 2, 0xe, 1, 0xf);
        if (redraw != 0) {
            redraw = 0;
            _Func_80b09fc(q, box[6] * 8 - 3, (box[7] + sel * 2) * 8 + 9, 3);
        }
        _Func_80b08b8(q);
        if ((gKeyRepeat & 0x40) != 0) {
            _PlaySound(0x6f);
            sel--;
            redraw = 1;
            if (sel == -1)
                sel = 2;
        }
        if ((gKeyRepeat & 0x80) != 0) {
            _PlaySound(0x6f);
            sel++;
            redraw = 1;
            if (sel == 3)
                sel = 0;
        }
        if ((gKeyRepeat & 2) != 0) {
            _PlaySound(0x71);
            sel = -1;
            goto done;
        }
    } while ((gKeyRepeat & 1) == 0);
    _PlaySound(0x70);
done:
    CloseUIBox(box, 2);
    WaitFrames(1);
    Func_8003f3c(slot);
    return sel;
}

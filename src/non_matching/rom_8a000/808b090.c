/* Func_808b090 (0x0808b090) -- NON-MATCHING, 26 differing in disagreeing regions of 100.
 * Blocker class: global_alloc PRIORITY, reached through a genuine source dilemma.
 * Never attempted before batch 276.
 *
 * asm/rom_8a000/rom_8ace0_a_a_c_a_c_a.s (2 functions, so landing needs a split).
 *
 * THE TWIN PARK IN THE SAME .s TRANSFERRED WHOLESALE AND WAS RIGHT ABOUT
 * EVERYTHING IT SETTLED. src/non_matching/rom_8a000/808b158.c (GetLocationName)
 * supplied the `s16 id; s16 sub : 15; u16 flag : 1;` bitfield table entry, the
 * byte-load flag test, the asymmetric `e->sub == -1 || e->sub == b`, and the
 * branching `continue`s. The only re-derivation needed was the record's TAIL --
 * this function's is `s16 cond; s16 val;` where GetLocationName's is
 * `void *name` -- plus the extra `e->cond == -1 || _GetFlag(e->cond)` gate.
 * A park is a file-mate source, and this is the sixth function to land work off
 * one.
 *
 * Everything from .L8b0da to the epilogue reproduces, including all THREE
 * `ldr r4, =gState` materialisations, both `lsl`/`asr` sign-extension pairs, the
 * hoisted 0x7fff and -1, and the shared `add r5, #8` tail.
 *
 * THE BLOCKER IS A GENUINE DILEMMA BETWEEN TWO FORMS THAT CANNOT COEXIST, and
 * this is the part that generalises:
 *
 *   The ROM's `add r3, r4, rOff / mov rZ, #0 / ldrsh rD, [r3, rZ]` requires the
 *   address to be `(plus (reg base) (const_int))`, which is INVALID for Thumb
 *   HImode -- there is no immediate `ldrsh` -- so gcc is forced to put the whole
 *   thing in one register, which is what emits the `mov #0` and the `add`. That
 *   needs LITERAL offsets.
 *
 *   But the base must ALSO be in a register, or `gState + 448` folds to a single
 *   pool word `=gState+448` (measured: 93 differing). The only things that put
 *   the symbol in a register are an explicit `g = gState` variable, or a REGISTER
 *   offset somewhere in the same extended basic block -- and a register offset
 *   makes the address `(plus reg reg)`, which IS valid, so gcc folds it and the
 *   `mov #0` and `add` vanish again.
 *
 * So the two halves of the ROM's addressing are reachable one at a time and not
 * together, from any spelling measured.
 *
 * THE STRUCTURALLY CORRECT SOURCE IS t3b (an explicit `g` used for both the reads
 * and the store): all THREE address forms come out CORRECT, and its residue is
 * then exactly the twin park's -- gcc gives the `gState` pseudo (5 references,
 * floor_log2 3) a callee-saved register and SPILLS `res` (3 references), adding
 * `sub sp, #4 / str / ldrh`, where the ROM leaves `res` unallocated and reload
 * rematerialises it into call-clobbered r4 three times. Eight values compete for
 * seven callee-saved registers under -fcall-used-r4 (`e`, `a`, `b`, `c`, `res`,
 * 0x7fff, -1, base). Second global_alloc park in this one .s.
 *
 * MEASURED (rom 100 lines):
 *   t3a literal offsets, no base variable       93 differing /  88 ours
 *   t3d `off` variable with `off += 0xa`        26 in regions /  96 ours  <- best, below
 *   t3e t3d plus a `short *` pointer local      identical to t3d (folded away)
 *   t3b explicit `g` for reads AND store        49 / 103  (all 3 address forms right,
 *                                                          `g` in r6, `res` spilled)
 *   t3f `g` for reads + separate `h` for store  52 / 101  (forms right, base in r2,
 *                                                          one `ldr` instead of three)
 *   t3h `g` for reads, literal in the store     48 /  97
 *   t3g all-literal `off`                       52 / 101
 *   t3i / t3j / t3m three declaration orders    all inert at their base numbers
 *
 * ONE CORRECTION TO READ OFF THIS: the ROM's `add r1, #0xa` is
 * reload_cse_move2add, NOT a source `off += 0xa`. `mov r1, #0xe1 / lsl r1, #1` is
 * a SINGLE RTL insn (thumb_shiftable_const), so move2add sees `(set r1 0x1c2)` and
 * rewrites the next `(set r1 0x1cc)`. It needs both constants in the same hard
 * register, which is again downstream of the allocation -- so the move2add
 * SPACING lever cannot be applied here independently.
 *
 * NEXT: global_alloc, with the twin park. Do not spend a round on address
 * spellings -- the dilemma above is proved from the Thumb HImode constraint and
 * nine spellings are on file.
 */
#include "gba/types.h"

struct Ent {
    s16 id;
    s16 sub : 15;
    u16 flag : 1;
    s16 cond;
    s16 val;
};

extern struct Ent L9d9f0[] __asm__(".L9d9f0");
extern unsigned char gState[];
extern int _GetFlag(int id);

void Func_808b090(void)
{
    struct Ent *e;
    int off;
    int a;
    int b;
    int c;
    int res;

    off = 0xe0 * 2;
    res = 0x12;
    a = *(short *)(gState + off);
    off = 0xe1 * 2;
    b = *(short *)(gState + off);
    off += 0xa;
    c = *(short *)(gState + off);
    e = L9d9f0;
    while (e->id != -1) {
        if (e->flag) {
            if (e->id != a) {
                e++;
                continue;
            }
        } else if (e->id != c) {
            e++;
            continue;
        }
        if (e->sub == -1 || e->sub == b) {
            if (e->cond == -1 || _GetFlag(e->cond)) {
                res = e->val;
                break;
            }
        }
        e++;
    }
    off = 0xf8 * 2;
    *(short *)(gState + off) = res;
}

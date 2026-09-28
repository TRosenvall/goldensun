/* Func_80b1614 -- batch 293 brief D target 2.  EXACT.
 * ref: asm/rom_b0000/rom_b0070_a_a_c_c_c_a_a_a_d.s  (189 insns / 200 encodings / 464 bytes)
 *
 *   objcmp --whole: OK whole file -- 464 bytes, 200 encodings and 26 relocations identical
 *
 * Whole-file conversion: datacheck reports no data (the two `.word`s are literal
 * pool), grep -ci func_start = 1.  No flags, no symbols.
 * NO SHIMS -- zero `register ... __asm__` declarations in the code.  DMA3_COPY is
 * include/dma.h's established helper (which carries its own register pins inside
 * the header); many landed non-park files use it and none carries a fakematch row
 * for it, so it needs no row here either.
 *
 * WHAT IT DOES.  A quantity spinner.  galloc_ewram a 0x400 scratch buffer, open a
 * UI box, take a sprite slot, bump the second object's 10-bit tile field by 4, then
 * loop on the d-pad: gKeyRepeat 0x20/0x10 move the cursor and raise a dirty flag;
 * when dirty, the cursor is wrapped modulo the span, the scratch is repainted with
 * four Func_80b06c0 calls, DMA'd to VRAM and the count and count*step printed.
 * A returns cur + 1, B returns -1.  The landed caller in
 * src/rom_b0000/rom_b0070_a_a_c_c_c_c_c_b.c passes (0, cnt, unit_price).
 *
 * ================= THE LOAD-BEARING CONSTRUCTS, WITH SINGLE DROPS =================
 *
 * Baseline: 40 of 199 aligned, 198 encodings vs 200.
 *
 * 1. THE `hi` PARAMETER IS OVERWRITTEN WITH THE SPAN -- there is NO `span` local.
 *    26 -> 3, and it is the whole function.  THE TELL IS IN THE ROM'S OWN TEXT:
 *    `mov r9, r1` at entry and `mov r9, r3` after the subtraction are the SAME
 *    high register, i.e. ONE pseudo, not two allocnos that happen to share r9.
 *    With a separate `int span` local the contest for the last high callee-saved
 *    register is span (4 refs, 3 of them in the loop) against `slot` (6 refs, 1 in
 *    the loop), `slot` wins on reference count, span spills to the frame, and the
 *    residue is 14 rows of "right instructions, wrong registers".  Overwriting the
 *    parameter removes the contest by construction: the pseudo is live from entry,
 *    is callee-saved for that reason alone, and `slot` spills because there is now
 *    nothing left for it.
 *
 *    THIS IS ALSO WHY THE ROM PAYS FOUR INSTRUCTIONS FOR ONE SUBTRACTION.
 *    `mov r3, r9 / mov r2, r11 / sub r3, r2 / mov r9, r3` looks like bad codegen
 *    until you see that both operands are in HIGH registers -- thumb's 3-operand
 *    `sub rD, rN, rM` takes low registers only, so each operand needs a copy down
 *    and the result a copy back up.  A separate `span` local puts `hi` in r5 and
 *    gets `mov r3, r11 / sub r3, r5, r3`: two instructions instead of four, and one
 *    instruction SHORT of the ROM.  A candidate that is too short by one in the
 *    middle of a register shuffle is evidence about which pseudos are high, not
 *    about the arithmetic -- worth adding to the "too low a count can be a
 *    signature" note in elevation.md, which so far only records the r8-counter case.
 *
 * 2. `ret = -1; cur = 0;` BEFORE the `_CreateUIBox` call, not after.  3 -> 0.
 *    Pure sched2 ordering: the only residue at 3 was `mov r10, r0` (the box copy)
 *    landing two slots early, ahead of the ROM's `mov r5,#1 / neg r5,r5 / mov r7,#0`.
 *    Measured all four placements of the two initialisers around the call:
 *      both before  -> 0      (this candidate)
 *      cur before   -> 3
 *      ret before   -> 2
 *      both after, swapped -> 2
 *      both after   -> 3
 *    So it is the pair that matters, not either one -- another instance of the
 *    coupled-lever point: neither single move reaches 0.
 *
 * 3. THE MUL OPERAND ORDER: `step * (cur + 1)`.  40 -> 39.  The ROM's
 *    `mov r0, r5 / mul r0, r3` copies `cur + 1` (r5), so per the procedure the OTHER
 *    operand -- `step` -- goes on the right, which is `(cur + 1) * step`, and that
 *    is the spelling that LOST.  Second site in this brief where the "read the mov"
 *    procedure pointed at the wrong order; see target 1.  Measure both, always.
 *
 * ============================ MEASURED INERT ============================
 *
 * - `(cur + hi) % hi` versus an explicit `__modsi3(cur + hi, hi)` call: BYTE
 *   IDENTICAL.  Both emit the libcall with the same argument setup, so the natural
 *   `%` spelling costs nothing and an explicit libcall declaration is never needed
 *   for a variable divisor here.
 * - `DMA3_SET(Lb3f80, buf, 0x84000040)` versus `DMA3_COPY(Lb3f80, buf, 0x100)`:
 *   BYTE IDENTICAL, despite DMA3_SET's extra "r0" clobber.
 * - `(hi + cur) % hi` operand order: inert.
 * - Declaration order among function-scope locals is inert for the REGISTER
 *   contest but decides the SPILL-SLOT order: `dirty` must be declared before
 *   `slot` (ROM has slot at sp+4, dirty at sp+8; ascending pseudo -> descending sp).
 * - A `__asm__ ("" : "+r" (span))` barrier on the span local was worth 39 -> 30 and
 *   then 26 combined with the slot order -- it is genuinely the right class for a
 *   "right instructions, wrong registers" residue -- but the parameter-overwrite
 *   reading reaches 0 with NO shim, so the shim is discarded.  Recording it because
 *   it is a case where the shim WORKED and was still the wrong answer: it was
 *   papering over a misread of which pseudo the ROM had.
 * - `span` computed before galloc_ewram: 43, worse.
 * - `hi - lo` spelled at each use site instead of a local: 34, and loop-invariant
 *   hoisting then computes it in the loop preheader where the ROM has it at the top.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_b0000/rom_b0070_a_a_c_c_c_a_a_a_d.s --whole
 */
#include "dma.h"

typedef struct {
    unsigned char pad00[0x18];
    unsigned short tile : 10;
    unsigned short rest : 6;
} Obj;

extern unsigned char Lb3f80[] __asm__(".Lb3f80");
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;

extern void *galloc_ewram(int tag, int size);
extern void gfree(int tag);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);
extern void _CloseUIBox(void *box, int a);
extern int AllocSpriteSlot(void);
extern void UploadSpriteGFX(int slot, int a, void *b);
extern Obj *_Func_801eadc(int slot, unsigned int a, void *box, int y, int b);
extern void _Func_801ea08(int v, int n, void *box, int x, int y);
extern void _Func_801e7c0(int id, void *box, int x, int y);
extern void Func_80b06c0(int a, int b, void *buf);
extern void WaitFrames(int n);
extern void _PlaySound(int id);

int Func_80b1614(int lo, int hi, int step)
{
    void *buf;
    void *box;
    Obj *o;
    int dirty;
    int slot;
    int cur;
    int ret;

    buf = galloc_ewram(0xe, 0x400);
    dirty = 1;
    hi = hi - lo;
    ret = -1;
    cur = 0;
    box = _CreateUIBox(7, 4, 0x17, 3, 2);
    if (box != 0) {
        slot = AllocSpriteSlot();
        if (slot != 0x60) {
            UploadSpriteGFX(slot, 0x100, 0);
            _Func_801eadc(slot, 0x40004000, box, 0, 0);
            o = _Func_801eadc(slot, 0x40004000, box, 0x20, 0);
            o->tile = o->tile + 4;
            for (;;) {
                if ((gKeyPress & 1) != 0) {
                    _PlaySound(0x70);
                    ret = cur + 1;
                    break;
                }
                if ((gKeyPress & 2) != 0) {
                    _PlaySound(0x71);
                    ret = -1;
                    break;
                }
                if ((gKeyRepeat & 0x20) != 0) {
                    _PlaySound(0x6f);
                    dirty = 1;
                    cur--;
                }
                if ((gKeyRepeat & 0x10) != 0) {
                    _PlaySound(0x6f);
                    dirty = 1;
                    cur++;
                }
                if (dirty != 0) {
                    dirty = 0;
                    cur = (cur + hi) % hi;
                    DMA3_COPY(Lb3f80, buf, 0x100);
                    Func_80b06c0(0x1e, 0xe, buf);
                    Func_80b06c0(lo + hi, 0, buf);
                    Func_80b06c0(lo + cur + 1, 0xa, buf);
                    Func_80b06c0(lo, 2, buf);
                    UploadSpriteGFX(slot, 0x100, buf);
                    _Func_801ea08(cur + 1, 2, box, 0x48, 0);
                    _Func_801ea08(step * (cur + 1), 6, box, 0x58, 0);
                    _Func_801e7c0(0xc88, box, 0x88, 0);
                }
                WaitFrames(1);
            }
            WaitFrames(1);
            _CloseUIBox(box, 2);
        }
    }
    gfree(0xe);
    return ret;
}

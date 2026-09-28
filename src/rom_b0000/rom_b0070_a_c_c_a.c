/* Func_80b211c -- batch 293 brief D target 3.  EXACT.
 * ref: asm/rom_b0000/rom_b0070_a_c_c_a.s  (224 insns / 231 encodings / 524 bytes)
 *
 *   objcmp --whole: OK whole file -- 524 bytes, 231 encodings and 28 relocations identical
 *
 * Whole-file conversion: datacheck reports no data (the `.word 0x1ff` is literal
 * pool), grep -ci func_start = 1.  No flags.  NO SHIMS -- zero
 * `register ... __asm__` declarations in the code.  NO NEW SYMBOLS: `_MSG_75` is
 * already in message.sym (= 0x0075) and include/message.h already declares it.
 *
 * WHAT IT DOES.  A 5-wide grid item picker.  The outer loop reopens the lower box
 * each round; the inner loop repaints on a dirty flag and reads the d-pad.
 * Left/right wrap with `% nfree`; L and R step by five with explicit fixups
 * (`if (sel < 0) sel += 15;` then `while (sel >= nfree) sel -= 5;`, and the mirror).
 * A commits through Func_80b2328, prints 0xcc2 and loops while a free slot remains;
 * B returns -1.
 *
 * ================= THE LOAD-BEARING CONSTRUCTS, WITH SINGLE DROPS =================
 *
 * Baseline with the obvious spelling (`break` out of the inner loop): 228 of 243.
 *
 * 1. THE TWO INNER-LOOP EXITS ARE `goto done`, NOT `break`.  228 -> 8.  This is the
 *    single biggest lever in the brief and I have not found it in elevation.md, so
 *    writing the mechanism out in full:
 *
 *    stmt.c's `expand_end_loop` (gcc-2.96 stmt.c:2257) carries a rotation it
 *    documents as "if the loop starts with a loop exit, roll that to the end where
 *    it will optimize together with the jump back".  It scans forward from the loop
 *    top for the FIRST conditional or unconditional jump whose target is
 *    `loop_stack->data.loop.end_label` or `alt_end_label`, then moves everything
 *    from the loop top through that jump to the BOTTOM and emits an entry jump into
 *    the middle -- the transform its own comment draws as
 *
 *        start: if (test) goto end;  body;  goto start;
 *     => goto start;  newstart: body;  start: if (test) goto end;  goto newstart;
 *
 *    With `break`, the first such jump is the A-button exit, so gcc rolls the whole
 *    repaint block AND the A test to the bottom and enters with `b .L6`.  That is a
 *    different block order for 200-odd instructions, which is where 220 of the 228
 *    came from -- the loop BODY was already right.
 *
 *    A `goto` to a label the programmer wrote is NOT `end_label`, so the scan runs
 *    off the end of the loop, `last_test_insn` stays NULL_RTX and the roll never
 *    fires.  The ROM's layout -- repaint block physically first at .Lb217e, reached
 *    by an unconditional `b .Lb217e` from the bottom, entry falling straight
 *    through -- is the UNROLLED shape, so the ROM's source cannot have used `break`
 *    here.  That makes this an argument from construction, not a preference.
 *
 *    Worth generalising: `break` and `goto` out of a loop are NOT interchangeable in
 *    gcc-2.96, and the tell is in the block ORDER, not in any single instruction.
 *    Any park whose residue is "the loop body matches but the blocks are in the
 *    wrong order, with an entry jump into the middle" should try `goto` before
 *    anything else.  The related note already in elevation.md -- "`goto` into a
 *    `do/while` is a SCHEDULING lever" -- is about a different mechanism
 *    (NOTE_INSN_LOOP_BEG acting as a barrier); this one is about which label the
 *    exit jump names.
 *
 * 2. DECLARATION ORDER `box2, box1, u`.  8 -> 1.  Spill-slot order only: the ROM
 *    puts `u` at sp+8, box1 at sp+0xc, box2 at sp+0x10 and the `unit` parameter at
 *    sp+0x14, and with ascending pseudo -> descending sp that requires the pseudos
 *    to run unit < box2 < box1 < u.  Six `ldr`/`str` rows, nothing else.
 *
 * 3. `_MSG_75` RATHER THAN THE LITERAL 0x75, at `Func_80b11a4(box2, MSG + item)`.
 *    The ROM spends `ldr r3, =0x75 / add r6, r3` -- TWO instructions and a pool
 *    word -- where a plain literal gives the single `add r6, #0x75`, since thumb's
 *    `add rd, #imm8` covers 0..255.  Same argument as the shiftable ids: gcc will
 *    not pay a pool word for something it can encode in one instruction.  The row
 *    already exists in message.sym from earlier work, so nothing is proposed here.
 *
 * ============================ MEASURED INERT / CHECKED ============================
 *
 * - `(sel % 5) * 16` and `(sel / 5) * 16 + 8` with the LITERAL 5 reproduce the
 *   ROM's `__modsi3` / `__divsi3` pair exactly.  Consistent with batch 292's
 *   divisor table: 5 is not a power of two, so the literal takes the libcall and a
 *   libcall proves nothing about the divisor's spelling.  No `int five = 5;` needed.
 * - The four `while` fixup loops reproduce as plain `while (...)` -- guard plus
 *   rotated do-while -- with no restructuring.  The counters ARE used (they are the
 *   value being fixed up), so the batch-292 count-up/do-while lever does not apply.
 * - `nfree = 1;` before the outer loop is load-bearing only as a value; the ROM has
 *   it, and it is the natural defensive initialiser.
 *
 * this scratch file verifies standalone; DELETE IT on landing.  Verified: without
 * it the object differs from the reference in exactly one word -- the pool entry at
 * +0x1fc reads 0x00000000 with an R_ARM_ABS32 against `_MSG_75` instead of
 * 0x00000075 -- and stage1.ld's `INCLUDE "message.sym"` resolves it to 0x75.
 * Every other one of the 231 encodings is already identical without the .equ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_b0000/rom_b0070_a_c_c_a.s --whole
 */
typedef struct { unsigned char pad00[0xd8]; unsigned short items[1]; } Unit;
struct P { unsigned char pad00[5]; unsigned char f05; };
typedef struct {
    unsigned char pad000[0x20];
    void *f20;
    unsigned char pad024[0x380 - 0x24];
    struct P *f380;
    unsigned char pad384[0x3a8 - 0x384];
    unsigned char f3a8;
} State;

extern unsigned char iwram_3001f2c[];
extern int _MSG_75;
extern volatile unsigned int gKeyPress;
extern volatile unsigned int gKeyRepeat;

extern Unit *_GetUnit(int unit);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);
extern void _CloseUIBox(void *box, int a);
extern int _FindEmptyInventorySlot(int unit);
extern void Func_80b0a6c(void *box, int x, int y);
extern int Func_80b20e8(int item);
extern void Func_80b110c(void *box, int item, int val, int n);
extern void Func_80b11a4(void *box, int msg);
extern void Func_80b2328(int unit, int sel);
extern void Func_80b04dc(int msg);
extern void WaitFrames(int n);
extern void _PlaySound(int id);

int Func_80b211c(int unit)
{
    State *st;
    void *box2;
    void *box1;
    Unit *u;
    int nfree;
    int sel;
    int redraw;
    int item;
    int ret;

    st = *(State **)iwram_3001f2c;
    u = _GetUnit(unit);
    nfree = 1;
    box1 = _CreateUIBox(0xf, 8, 0xf, 4, 2);
    sel = 0;
    for (;;) {
        box2 = _CreateUIBox(0, 5, 0x1e, 3, 2);
        st->f380->f05 = 0x12;
        st->f3a8 = 0xc;
        redraw = 1;
        for (;;) {
            if (redraw != 0) {
                redraw = 0;
                nfree = _FindEmptyInventorySlot(unit);
                if (sel > nfree - 1)
                    sel = nfree - 1;
                item = u->items[sel] & 0x1ff;
                Func_80b0a6c(st->f20, (sel % 5) * 16, (sel / 5) * 16 + 8);
                st->f3a8 = 3;
                Func_80b110c(box1, item, Func_80b20e8(u->items[sel]), 2);
                Func_80b11a4(box2, (int)&_MSG_75 + item);
            }
            if ((gKeyPress & 1) != 0) {
                _PlaySound(0x70);
                ret = 0;
                goto done;
            }
            if ((gKeyPress & 2) != 0) {
                _PlaySound(0x71);
                ret = -1;
                goto done;
            }
            if ((gKeyRepeat & 0x20) != 0) {
                _PlaySound(0x6f);
                sel--;
                sel = (sel + nfree) % nfree;
                redraw = 1;
            }
            if ((gKeyRepeat & 0x10) != 0) {
                _PlaySound(0x6f);
                sel++;
                sel = (sel + nfree) % nfree;
                redraw = 1;
            }
            if ((gKeyRepeat & 0x40) != 0) {
                sel -= 5;
                if (sel < 0)
                    sel += 15;
                while (sel >= nfree)
                    sel -= 5;
                _PlaySound(0x6f);
                redraw = 1;
            }
            if ((gKeyRepeat & 0x80) != 0) {
                sel += 5;
                if (sel >= nfree)
                    sel -= 15;
                while (sel < 0)
                    sel += 5;
                _PlaySound(0x6f);
                redraw = 1;
            }
            WaitFrames(1);
        }
        done:
        _CloseUIBox(box2, 2);
        WaitFrames(1);
        if (ret != 0)
            break;
        Func_80b2328(unit, sel);
        Func_80b04dc(0xcc2);
        if (_FindEmptyInventorySlot(unit) == 0)
            break;
    }
    _CloseUIBox(box1, 2);
    return ret;
}

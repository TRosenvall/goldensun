/* StartMenu_Main (RunSubScreen) -- 0x0801db70, asm/rom_15000/rom_1ca1c_c_c_c_c.s
 *
 * NON-MATCHING: 49 encodings of 195 differ (objcmp).
 *
 * SIZE EXACT (440 bytes both), INSTRUCTION COUNT EXACT (187 = 187), FRAME EXACT
 * (0x14 both), and objcmp reports NO size and NO relocation difference -- all 18
 * `bl` offsets and all 5 ABS32 pool words land at the ROM's byte offsets.  Only
 * ENCODINGS differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801db70.c \
 *     asm/rom_15000/rom_1ca1c_c_c_c_c.s --func StartMenu_Main
 *
 * NO SHIMS, NO PINS, NO asm.  `extern signed char L367dc[] __asm__(".L367dc");`
 * is the tree's spelling for a `.L` local label.
 *
 * SAME TEXT/DATA SPLIT AS ITS SIBLING, AND THE SAME MISSING EXPORT.  This
 * function also does `ldr r3, =.L367dc` (line 274) and `.L367dc` is not in the
 * `.global` list datacheck prints.  See the Func_801d9d4 park for the full note:
 * the split needs SEVEN exports.  Both functions in this .s must move together
 * or the label has to be exported anyway.
 *
 * FOUR LEVERS LANDED.  Drop ladder from 49, each removed alone:
 *   `tbl` local for the table base        151  (102 worse, 4 instructions short)
 *   `y` reused for the sprite Y           117  (68 worse)
 *   `sl = o->slot` loaded before `dim`     52
 *   `rows = 3` before the _GetFlag call    54
 * Full ladder: 152 -> 127 (`tbl`) -> 57 (`y` reuse) -> 54 (`sl`) -> 49
 * (`rows = 3` early).
 *
 * 1. A NAMED LOCAL FOR THE TABLE BASE IS WHAT SPILLS `redraw`, AND THAT IS THE
 *    WHOLE FUNCTION.  This is the biggest single lever in the batch and it works
 *    by RAISING REGISTER PRESSURE rather than by changing any instruction.
 *    Written as `L367dc[sel]` inside the loop, gcc-2.96 rematerialises
 *    `ldr r3,=.L367dc` every iteration (cheap, so global-alloc never asks for a
 *    register), `redraw` wins r11, `rows` is pushed to r9, and the frame comes
 *    out 0x10 instead of 0x14.  Assigning `tbl = L367dc;` in the loop's
 *    pre-header creates an allocno with refs ONLY inside the loop -- high
 *    n_refs/live_length -- which takes r9, evicts `rows` up to r11 and leaves
 *    `redraw` (5 refs over the whole function) with no register at all.  The ROM
 *    spills exactly that: `str rN,[sp,#0xc]` for 1 and for 0, `ldr r0,[sp,#0xc]`
 *    for the test.  It is the inverse of the Func_801c49c symbol-remat lever --
 *    there the lever DENIED a register by stretching a live range; here it WINS
 *    one by shortening it, and the eviction it causes is the point.
 *    POSITION MATTERS THE SAME WAY: `tbl = L367dc;` at the top of the function
 *    instead of in the pre-header reads 152, i.e. fully inert.
 *
 * 2. THE SPRITE Y GOES INTO `y`, THE SAME LOCAL THAT HELD -0x18 FOR Func_8021620.
 *    Three spellings measured, everything else held: a fresh temporary 137,
 *    reusing the loop counter `i` 59, reusing `y` 57.  The ROM's
 *    `mov r4,r3 / add r4,#0x10 / ... / mov r2,r4` needs a variable that is
 *    already allocated; `y` is the one whose register (r4) the ROM reuses.  One
 *    y-coordinate local serving the panel offset and the cursor sprite is also
 *    the reading that makes sense of the source.
 *
 * 3. `sl = o->slot;` BEFORE THE `dim` TEST.  The ROM loads `ldrb r1,[r5,#0xe]`
 *    above `mov r2,#0 / cmp / mov r2,#1`; passing `o->slot` directly as the
 *    second argument sinks it below.  Worth 3.
 *
 * 4. `rows = 3;` BETWEEN `redraw = 1;` AND the _GetFlag call.  Worth 5, and it is
 *    pure statement order -- the ROM's `mov r2,#3 / mov r11,r2` sits between the
 *    `str` of redraw and `lsl r0,#1`.
 *
 * MEASURED AND WRONG: THE STRENGTH-REDUCTION READING OF THE OPTION LOOP.
 *    The ROM's `ldmia r7!,{r5}` plus a second register stepping by 1 looks
 *    exactly like loop.c givs off `s->items[i]` and `tbl[base + i]`.  Written
 *    that way it reads 152 -- 103 worse -- and is 2 instructions short.  The
 *    pointers are SOURCE-LEVEL: `struct Opt **q = &s->items[0];` walked with
 *    `*q++`, and a separate `sel` initialised from `base` and incremented.  A ROM
 *    post-increment load is not on its own evidence of strength reduction.
 *
 * MEASURED AND INERT:
 *   `*(sel + tbl)` instead of `tbl[sel]`                               0
 *   `o = *q; q++;` split from `o = *q++`                               0
 *   `q` / `o` hoisted to function scope, before or after the ints      0
 *   `h` declared first / last, or retyped `int` with casts             0
 *   swapping `i++` and `sel++`                                         0
 *
 * BLOCKER: TWO COUPLED ALLOCATION FACTS, NEITHER REACHED FROM SOURCE.
 * PASS .18.greg (allocation) and reload1.c's spill-slot assignment.
 *
 *   (a) THE SPILL SLOTS OF `redraw` AND `h` ARE SWAPPED.  ROM frame:
 *       [sp,#0] caller-save slot, [sp,#4] base, [sp,#8] h, [sp,#0xc] redraw,
 *       [sp,#0x10] box.  Ours: [sp,#8] redraw, [sp,#0xc] h, everything else
 *       identical.  Both variables are spilled in both, the frame is 0x14 in
 *       both, and only the two slot numbers differ.  Declaration order does not
 *       move it: `h` is already declared above `redraw`, and moving it first,
 *       last, or retyping it measured 52 / 49 / 49.  This one swap accounts for
 *       most of the residual r0-vs-r1 noise, because the two slots are reached
 *       through different scratch registers.
 *
 *   (b) `i` AND `q` EXCHANGE r4 AND r7.  The ROM allocates the loop counter r4
 *       -- call-clobbered under -fcall-used-r4, so caller-save.c wraps both calls
 *       in `str r4,[sp] / bl / ldr r4,[sp]` -- and the option pointer r7.  Ours
 *       does the reverse, at identical cost: same two save/restore pairs, same
 *       instruction count, r4 and r7 swapped throughout.  The two allocnos have
 *       the same conflict set and the same reference count, so this is the
 *       "global-alloc EXACT priority ties break on pseudo number" case; five
 *       declaration orders and both pointer spellings were measured and none of
 *       them moved it.
 *
 * NOT the cse-zero class.  The one place this function shares a zero is in the
 * ROM's FAVOUR and we reproduce it: `redraw = 0;` stores 0 to [sp,#0xc] and the
 * `i < rows` guard then reads that same slot back (`ldr r2,[sp,#0xc] / cmp
 * r2,r11`) instead of rematerialising.  Writing `redraw = 0;` immediately before
 * `i = 0;` is what produces it.
 */
struct Opt { unsigned char pad[0xe]; unsigned char slot; unsigned char pri; };
struct SubScr {
    unsigned char pad0[0x574];
    unsigned short cur;                 /* 0x574 */
    unsigned char pad1[0x5a4 - 0x576];
    void *obj;                          /* 0x5a4 */
    unsigned char pad2[0x610 - 0x5a8];
    struct Opt *items[1];               /* 0x610 */
};
struct Box { unsigned char pad[0xc]; unsigned short x; unsigned short y; };

extern struct SubScr *iwram_3001ea0;
extern unsigned char gDebugMode;
extern signed char L367dc[] __asm__(".L367dc");
extern volatile int gKeyPress;
extern volatile int gKeyRepeat;
extern int _GetFlag(int flag);
extern void Func_801d980(void);
extern unsigned char *Func_801d9d4(void);
extern void *Func_8021620(int a, void *box, int b, int c);
extern void WaitFrames(int n);
extern void Func_80216b4(void *h);
extern void _PlaySound(int id);
extern void CloseUIBox(void *box, int mode);
extern void Func_801d9bc(void);
extern void _Func_80a17c4(struct Opt *o);
extern int StartMenu_AddOption(int id, int slot, int dim);
extern void _Func_80b09fc(void *p, int x, int y, int n);

int StartMenu_Main(void)
{
    struct SubScr *s;
    struct Box *bx;
    unsigned char *box;
    void *h;
    int redraw, f, base, rows, cur, ret, y, i, sel;
    signed char *tbl;

    redraw = 1;
    rows = 3;
    f = _GetFlag(0x17e);
    base = 0;
    Func_801d980();
    s = iwram_3001ea0;
    box = Func_801d9d4();
    y = -0x18;
    if (gDebugMode != 0)
        y += 8;
    h = Func_8021620(6, box, 0x28, y);
    WaitFrames(1);
    cur = s->cur;
    if (f != 0) {
        base = 2;
        rows = 1;
    }
    if (gDebugMode != 0)
        rows += 3;
    for (;;) {
        if (redraw != 0) {
            redraw = 0;
            cur = (cur + rows) % rows;
            s->cur = cur;
            i = 0;
            if (i < rows) {
                struct Opt **q = &s->items[0];
                tbl = L367dc;
                sel = base;
                do {
                    struct Opt *o = *q++;
                    int dim, sl;
                    o->pri = 0xfb;
                    _Func_80a17c4(o);
                    sl = o->slot;
                    dim = 0;
                    if (i != s->cur)
                        dim = 1;
                    StartMenu_AddOption(tbl[sel], sl, dim);
                    i++;
                    sel++;
                } while (i < rows);
            }
            bx = (struct Box *)box;
            y = (cur * 3 + bx->y) * 8 + 0x10;
            _Func_80b09fc(&s->obj, bx->x * 8, y, 3);
        }
        Func_80216b4(h);
        WaitFrames(1);
        if ((gKeyPress & 1) != 0) {
            ret = cur;
            _PlaySound(0x70);
            break;
        }
        if ((gKeyPress & 0xa) != 0) {
            ret = -1;
            _PlaySound(0x71);
            break;
        }
        if ((gKeyRepeat & 0x40) != 0) {
            _PlaySound(0x6f);
            cur--;
            redraw = 1;
        } else if ((gKeyRepeat & 0x80) != 0) {
            _PlaySound(0x6f);
            cur++;
            redraw = 1;
        }
    }
    CloseUIBox(box, 2);
    Func_801d9bc();
    WaitFrames(1);
    if (ret >= 0)
        ret += base;
    return ret;
}

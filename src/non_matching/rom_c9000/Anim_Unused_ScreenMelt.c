/* Anim_Unused_ScreenMelt -- NON-MATCHING, 185 of 186 encodings differ.
 * Reference asm//rom_c9000/rom_cfef4.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching//Anim_Unused_ScreenMelt.c \
 *       asm//rom_c9000/rom_cfef4.s --func Anim_Unused_ScreenMelt
 *
 * NOT a distance: ours is 192 instructions against 186 and 416 bytes against 404 --
 * SIX LONG, one cause.  Shim-free.
 * BLOCKER, scan_loop's read-only pool-MEM hoist: the dump says
 * `Hoisted regno 166 r/o from (mem/u/f:SI (symbol_ref/u:SI ("*.LC2")) 8)`.
 * Sixteen levers and four flags inert.
 * ITS PROSE IS WRONG ABOUT ITSELF: "175 instructions" is the .s LINE count; the
 * object has 186 encodings.  Use objcmp.
 * What did pay: THREE SEPARATE function-pointer locals, one per call site.  One
 * shared local gives `bl _call_via_fp` and re-assigning the same variable does not
 * help because gcc CSEs it; three distinct locals give `bl _call_via_r3` at all
 * three sites and made the relocation set match the reference exactly.
 * Also confirms const.sym's _CONST_1f correction on a second bank (this function's
 * mid-function pool and b-over-pool from a (u16) cast).
 */
/* Anim_Unused_ScreenMelt (0x080d0468) -- NON-MATCHING.
 * NON-MATCHING: 185 encodings of 186 differ (objcmp), ours 192, size 416
 * against the ROM's 404 (+12).  THE COUNTS DISAGREE, so 185 is NOT a distance;
 * the alignment-tolerant view is what says where the work is, and it says the
 * function is SIX INSTRUCTIONS LONG and every one of the six comes from ONE
 * cause.
 *
 * ================================================================
 * THE BLOCKER: loop.c's READ-ONLY CONSTANT-POOL MEM HOIST, TWICE
 * ================================================================
 *
 * Thumb-1 has no immediate form for a SYMBOL_REF, so gcc forces every symbol
 * address to the literal pool AT EXPAND TIME -- the RTL is
 * `(set (reg) (mem/u/f:SI (symbol_ref/u ("*.LC2"))))` with a
 * `REG_EQUAL (symbol_ref "gBuffer")` note.  That MEM is RTX_UNCHANGING_P, so
 * `loop_invariant_p` returns 1 for it and `scan_loop` hoists it out of the loop
 * unconditionally -- it does NOT go through move_movables' savings threshold.
 * From `.08.loop`:
 *
 *     Hoisted regno 166 r/o from (mem/u/f:SI (symbol_ref/u:SI ("*.LC2")) 8)
 *
 * and `.09.cse2` shows the inserted insn landing immediately before the row
 * loop's note:
 *
 *     (insn 634 631 167 (set (reg/v:SI 166)
 *             (mem/u/f:SI (symbol_ref/u:SI ("*.LC2")) 8)) 173 {*thumb_movsi_insn}
 *         (expr_list:REG_EQUAL (symbol_ref:SI ("gBuffer")) (nil)))
 *     (note 167 634 172 NOTE_INSN_LOOP_BEG ...)
 *
 * THE ROM DOES NOT HAVE THAT HOIST, AND IT HAS TWO PLACES IT WOULD SHOW:
 * `ldr r2, =gBuffer` is rebuilt inside the COLUMN loop body, and
 * `ldr r3, =Func_8001af8` is rebuilt inside the FRAME loop body at all three
 * call sites.  gcc hoists whichever one it can reach -- gBuffer when the pixel
 * address is written as an offset, Func_8001af8 when it is not -- and the cost
 * is always the same: a SEVENTH value live across the frame loop where the ROM
 * has six, which forces a FOURTH callee-saved high register.  That is the whole
 * residue: +2 in the prologue (`mov r7, r8 / push {r7}`), +2 in the epilogue,
 * and +2 on `frame++` because `frame` is pushed out to r11 and a high register
 * cannot be incremented in place (`mov r2,#1 / add r11,r2 / mov r3,r11 / cmp`
 * against the ROM's `add r5,#1 / cmp r5,#0x1b`).
 *
 * The ROM's six: frame r5, row r6, the 0x1f mask r7, acc r8, the melt table r9,
 * the phase counter r10.  Ours: row r5, 0x1f r6, the HOISTED ADDRESS r7,
 * acc r8, table r9, phase r10, frame r11.
 *
 * NOT A LOOP-SHAPE PROBLEM.  Both loops around it ARE real loops in the ROM and
 * must stay so -- `ldr r7, =0x1f` sits before the row loop's top LABEL, which
 * is where `emit_insn_before (loop->start)` puts a hoist and nowhere else, so
 * the row loop has notes; and the 0x1f hoist is the one hoist the ROM DOES have.
 * The column loop reads the same either way (a `goto` loop and a `do/while`
 * compile identically here).  Measured: row loop as `while` / as
 * `if (c) do {} while (c)` and column loop as `goto` / as `do/while`, all four
 * combinations 192.
 *
 * SIXTEEN LEVERS MEASURED AGAINST THE HOIST, ALL INERT AT 192 (except where
 * noted, and none of the exceptions removes the hoist):
 *   `&gBuffer[a]`; `gBuffer + a`; `(unsigned char *)((int)gBuffer + a)`;
 *   `px = gBuffer; px = px + a;`; `gBuffer[a]` inline at both the load and the
 *   store with no pointer (187/192); `extern unsigned char gBuffer[0x7800]`;
 *   `extern volatile unsigned char gBuffer[]` (the recorded
 *   volatile-on-the-DECLARATION lever for load_mems -- it cannot reach this
 *   promotion, because the MEM being promoted is gcc's own pool entry and not
 *   the object); a `volatile` cast at the use site; `extern const unsigned char
 *   gBuffer[]` with the store cast back; ONE reused `buf` pointer assigned at
 *   all five gBuffer sites (194, worse); building the address by successive
 *   pointer increments (190, still hoisted -- the two saved instructions are
 *   elsewhere and the shape is not the ROM's, which accumulates an int offset);
 *   and the flags -fno-gcse, -fno-rerun-loop-opt, -fno-move-all-movables,
 *   -fno-strength-reduce (180 asm lines each, i.e. all four inert).
 *
 * THE UNTESTED READING, for whoever picks this up: reload rematerialises a
 * spilled REG_EQUIV-to-pool register by re-emitting the pool load, which is
 * exactly `ldr r2, =gBuffer` inside the loop.  If the ROM's build hoisted and
 * then SPILLED, the visible result is the ROM's.  That needs one more register
 * of pressure than gcc currently sees here, and nothing source-level found it.
 *
 * ================================================================
 * WHAT CLOSED THE REST -- five mechanisms, three of them new
 * ================================================================
 *
 * 1. NEW: THREE SEPARATE FUNCTION-POINTER LOCALS, ONE PER CALL SITE.  The ROM
 *    calls Func_8001af8 INDIRECTLY three times, re-loading `ldr r3,
 *    =Func_8001af8` each time.  One `CopyFn copy` for all three -- the spelling
 *    the landed sibling src/rom_c9000/rom_cc5d8_a_a_b.c uses for its single call
 *    -- makes gcc keep the pointer in a callee-saved register and emit
 *    `bl _call_via_fp`.  Re-ASSIGNING the same variable at each site does not
 *    help (gcc CSEs the three sets).  THREE DISTINCT LOCALS do: all three sites
 *    become `bl _call_via_r3` and THE RELOCATION LIST THEN MATCHES THE
 *    REFERENCE'S EXACTLY -- same symbols, same multiplicities, 3x
 *    _call_via_r3, 2x gBuffer, 2x Func_8001af8, 2x WaitFrames, 1x Random,
 *    1x iwram_3001ef4.  A `_call_via_<callee-saved>` in a candidate against
 *    `_call_via_r3` in the ROM is a "the pointer was rematerialised" report,
 *    and the cure is more variables, not fewer.
 *
 * 2. `_CONST_1f` IS NOT NEEDED HERE EITHER, AND THE `(u16)` CAST IS WHY.  The
 *    ROM POOLS 0x1f (`ldr r7, .Ld04fc @ 0x1f`) where `mov r7, #0x1f` would do,
 *    with a mid-function pool and a `b` over it -- const.sym's tell.  Writing
 *    `(u16)(u >> 21) & 0x1f`, the batch-276 correction already landed in
 *    src/rom_f6000/rom_f6008_c_a_d.c, reproduces the pool AND the `b`, at zero
 *    extra instructions.  This is a SECOND independent bank confirming that
 *    correction; const.sym's `_CONST_1f` entry can cite it.
 *
 * 3. THE COLOUR EXTRACTION IS DONE IN THE SHIFTED DOMAIN AND THE SHIFTED VALUE
 *    IS UNSIGNED.  `u = c << 16` with `u` an `int` gives `asr` at all three
 *    extractions; the ROM has `lsr`.  `unsigned int u` gives `lsr`.  The red
 *    channel is `(u & 0x1f0000) >> 16` -- combine's own reassociation of
 *    `(u >> 16) & 0x1f`, which is why the ROM builds the wide mask
 *    `mov r3,#0xf8 / lsl r3,#13` instead of masking with 0x1f.
 *
 * 4. THE `y` RANGE TEST IS TWO SIGNED COMPARES AND MUST BE TWO STATEMENTS.
 *    `if (y >= 0 && y <= 0x77)` is folded to one unsigned `cmp #0x77 / bhi`.
 *    `if (y < 0) goto next; if (y > 0x77) goto next;` gives the ROM's
 *    `cmp #0 / blt` then `cmp #0x77 / bgt`.
 *
 * 5. EVERY LOOP TEST IN THIS FUNCTION IS `!=`, AND THE Random LOOP IS A
 *    do-while WITH A NAMED END POINTER.  `for (q = t; q != t + 0x100; q++)`
 *    emits a guard `cmp/beq` the ROM does not have, because gcc will not prove
 *    `t != t + 0x100`; `end = t + 0x100; q = t; do {...} while (q != end);`
 *    removes it.  The `!=`-on-every-loop observation is the sibling park's
 *    (src/non_matching/rom_c9000/cfef4_ShiningStar.c) and holds here on all
 *    five loops.
 *
 * ================================================================
 * THE REFERENCE'S PROSE IS WRONG ABOUT ITS OWN FUNCTION'S SIZE
 * ================================================================
 *
 * `@ Battle animation routine, 175 instructions.` -- 175 is the number of
 * instruction LINES in the .s body.  The assembled object has 186 encodings,
 * because the disassembly's literal pools are dumped as `.word` and the
 * `b`-over-pool jumps are real instructions the line count does not reach.
 * Use objcmp's figure, never the prose's.  The prose also says
 * `State: ewram_10000` where the code names `gBuffer`; those are the same
 * address (0x02010000), so that line is right, just spelled differently.
 * The prose says the body was not traced; it is traced now -- the function
 * builds a 32-entry red/green ramp palette at 0x5000040, pulls the 0x100-byte
 * melt table at iwram_3001ef4 full of `Random() & 0x3f`, copies the screen out
 * of 0x6008000 into gBuffer, then for 0x1b frames advances a row budget by
 * `phase / 4` (phase starting at 0x10) and, for every row that has come into
 * budget, replaces each of 0x100 columns' pixels with `0x3f - max(r, g, b)` of
 * its palette colour, copying gBuffer back to 0x6008000 each frame; it stops
 * early once the budget passes 0xf8.  Then a second ramp at 0x50000c0, +0x40 on
 * every one of 0x7800 bytes, and one last copy.
 *
 * ================================================================
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/d0468_ScreenMelt.c asm/rom_c9000/rom_cfef4.s \
 *     --func Anim_Unused_ScreenMelt
 * SIX functions in asm/rom_c9000/rom_cfef4.s (Anim_Condemn,
 * Anim_Unused_ScreenMelt, Anim_Bind, Anim_PsyphonSeal, Anim_AstralBlast,
 * Anim_ShiningStar) and a .rodata section, so a TEXT/DATA split is required and
 * the data must keep its own object.  tools/datacheck.py:
 * "Anim_Unused_ScreenMelt reads no data label -> split needs NO new export".
 * Shim count: 0 (tools/shimcount.py) -- PIN-FREE, no fakematch.txt row.
 */
#include "gba/types.h"

typedef void (*CopyFn)(void *dst, void *src, s32 len);

extern unsigned char *iwram_3001ef4;
extern unsigned char gBuffer[];
extern void Func_8001af8(void *dst, void *src, s32 len);
extern int Random(void);
extern void WaitFrames(int n);

void Anim_Unused_ScreenMelt(void)
{
    short *pal;
    unsigned char *t;
    unsigned char *q;
    unsigned char *end;
    unsigned char *px;
    CopyFn copy, copy2, copy3;
    int i, h, k, y, a, c, r, g, b;
    unsigned int u;
    int n, acc, row, frame;

    pal = (short *)0x5000040;
    for (i = 0; i != 0x20; i++) {
        h = i / 2;
        pal[i] = h | (h << 5) | (i << 10);
    }
    t = iwram_3001ef4;
    n = 0x10;
    acc = 0;
    copy = Func_8001af8;
    copy(gBuffer, (void *)0x6008000, 0x7800);
    end = t + 0x100;
    q = t;
    do {
        *q = Random() & 0x3f;
        q++;
    } while (q != end);
    row = 0;
    frame = 0;
    do {
        acc += n / 4;
        n++;
        while (row != acc) {
            k = 0;
        col:
            y = row - t[k];
            if (y < 0)
                goto next;
            if (y > 0x77)
                goto next;
            a = (k & 7) + (k / 8) * 64;
            a = a + (y & 7) * 8;
            a = a + (y / 8) * 0x800;
            px = &gBuffer[a];
            c = ((short *)0x5000000)[*px];
            u = c << 16;
            r = (u & 0x1f0000) >> 16;
            g = (u16)(u >> 21) & 0x1f;
            b = (u16)(u >> 26) & 0x1f;
            if (r < g)
                r = g;
            if (r < b)
                r = b;
            *px = 0x3f - r;
        next:
            k++;
            if (k != 0x100)
                goto col;
            row++;
        }
        copy2 = Func_8001af8;
        copy2((void *)0x6008000, gBuffer, 0x7800);
        WaitFrames(1);
        if (acc > 0xf8)
            goto tail;
        frame++;
    } while (frame != 0x1b);
tail:
    pal = (short *)0x50000c0;
    for (i = 0; i != 0x20; i++) {
        h = i / 2;
        pal[i] = h | (h << 5) | (i << 10);
    }
    q = gBuffer;
    for (i = 0; i != 0x7800; i++) {
        *q = *q + 0x40;
        q++;
    }
    copy3 = Func_8001af8;
    copy3((void *)0x6008000, gBuffer, 0x7800);
    WaitFrames(1);
}

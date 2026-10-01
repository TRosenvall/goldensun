/* Func_80b0aac -- NON-MATCHING, 530 of 550 encodings differ.
 * Unattempted before batch 302.  Reference asm/rom_b0000/rom_b0070_a_a_c_c_a_c_a_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_b0000/80b0aac.c \
 *       asm/rom_b0000/rom_b0070_a_a_c_c_a_c_a_a.s --func Func_80b0aac
 *
 * NOT A DISTANCE.  Size 1236 against 1272 and count 533 against 550, so the 530
 * SATURATES and cannot rank anything.  The ranking figure is tools/aligncmp.py:
 * aligned-equal 287 of 550 (52.2%), 360 differing/ins/del in 104 hunks.
 * We are SEVENTEEN INSTRUCTIONS SHORT and ONE FRAME WORD SHORT (0x20 against 0x24).
 * tools/shimcount.py: 0 -- PIN-FREE.
 *
 * SPLIT SHAPE.  `python3 tools/datacheck.py asm/rom_b0000/rom_b0070_a_a_c_c_a_c_a_a.s`
 * prints NOTHING: no text/data split needed.  The .s holds TWO functions
 * (Func_80b0aac at 0x080b0aac, Func_80b0fa4 at 0x080b0fa4) and this one is the
 * FIRST, so split_s.py is a HEAD split: _a is empty and not written, the target
 * becomes rom_b0070_a_a_c_c_a_c_a_b.s -> src/rom_b0000/rom_b0070_a_a_c_c_a_c_a_b.c,
 * and Func_80b0fa4 becomes rom_b0070_a_a_c_c_a_c_a_c.s.
 * stage1.ld:1471 names asm/rom_b0000/rom_b0070_a_a_c_c_a_c_a.o(.text), one line.
 * NO NEW `.global` IS REQUIRED: checked mechanically, the function's block
 * references no `.L` label it does not itself define.
 * src/rom_b0000/rom_b0070_a_a_c_c_a_c_a_b.c (LANDED; was src/non_matching/rom_b0000/80b0fa4.c) is the park for the OTHER function in this
 * .s; it names no suffix of its own, so whichever lands first may take _b.
 *
 * WHAT THE BODY IS.  A two-level shop screen.  An outer loop opens the item list
 * (two _CreateUIBox), an inner loop moves the cursor with modular arithmetic
 * (__modsi3 on +-1, and a page jump using __divsi3((n+6)/7)*7) and redraws on a
 * `redraw` flag; A accepts into a second screen that picks a recipient, calls
 * _GiveItemTo, checks the price against gState.f10 and either completes the sale
 * (Func_80b153c / Func_80b17e4) or refunds it (_Func_8078ad0 in a countdown loop).
 * Every one of the 73 relocations is present and in a body-correct position: the
 * symbol SEQUENCE matches the ROM except where noted under the blocker.
 *
 * ============ LEVERS THAT PAID, IN THE ORDER THEY PAID ============
 *
 *  L1. THE FOUR EXIT HANDLERS LIVE AT THE BOTTOM OF THE FUNCTION AND ARE REACHED
 *      BY `goto`.  aligned 198 -> 282 (36.0% -> 51.3%), the single biggest lever
 *      here by a wide margin, and it is READABLE OFF THE RELOCATION SEQUENCE ALONE.
 *      Written with `break` out of the two loops (the natural C), the accept and
 *      cancel arms land INLINE in the loop body and the relocation order reads
 *      ... Func_80b110c, _PlaySound, _PlaySound, __modsi3 ...  The ROM reads
 *      ... Func_80b110c, __modsi3, _PlaySound, __modsi3, _PlaySound ... and then
 *      carries FIVE relocations (_PlaySound, _PlaySound, Func_80b0574, _PlaySound,
 *      _PlaySound) after the last `Func_80b04dc` -- four one-line handlers parked
 *      past the end of both loops.  Their order there is the REVERSE of the order
 *      of the tests that reach them (cancel-inner2, no-money, cancel-inner1,
 *      accept-inner1, then the exit block), which is source order for labels
 *      written at the bottom, not any reordering pass.
 *      The tell in the reference is the `beq .Lnear / b .Lfar` PAIR at each test:
 *      a Thumb conditional branch reaches +-256 bytes, so a two-instruction
 *      conditional is proof the handler is hundreds of bytes away and therefore
 *      NOT inline.  Four of those pairs, four bottom handlers.
 *  L2. `signed char` IS THE RIGHT DECLARATION EVEN THOUGH THE ROM COMPILES IT TWO
 *      DIFFERENT WAYS.  ctx->f3a6 and ctx->f3aa come out `ldrb / lsl #24 / asr #24`
 *      while ctx->f3a7 -- the same width, the same signedness, one byte away --
 *      comes out `ldrsb rN, [rB, rI]`.  Thumb `ldrsb` has no immediate-offset form,
 *      so gcc uses it only where it has a register to spare for the index and falls
 *      back to a zero-extending load plus a shift pair otherwise.  Both forms drop
 *      out of a plain `signed char` field: all four reads in this function are
 *      already encoding-exact, and NOTHING needs to be spelled to choose between
 *      them.  Do not chase this difference in a reference; it is not a source fact.
 *  L3. THE OFFSETS ABOVE 0x3E ARE POOLED AND ADDED, AND THAT IS ALREADY RIGHT.
 *      `ldr r3,=0x3a6 / add r3,r10 / ldrb` is what a struct member at 0x3a6 gives,
 *      because Thumb `ldrb` reaches +31 and `ldrh` +62.  The struct declaration
 *      (not a byte array, per src/rom_b0000/rom_b0070_a_a_c_c_a_c_b.c's note in
 *      this same original file) is what keeps base and offset apart.
 *
 *  L4. SPILL-SLOT DECLARATION ORDER IS THE ROM'S FRAME ORDER, READ BOTTOM-UP.
 *      aligned 282 -> 287.  The reference's eight memory locals sit at sp+0x20
 *      (boxB), 0x1c (boxList), 0x18 (boxInfo), 0x14 (boxQ), 0x10 (sel), 0xc (qty),
 *      8 (info2) and 4 (again), with sp+0 the outgoing fifth argument.  Declaring
 *      them in DESCENDING SLOT ORDER -- boxB first, again last -- is what scored;
 *      moving boxQ alone to the front or the back of the list is EXACTLY INERT
 *      (530 / 533 / 282 / 361, byte-identical both ways).  So it is the order of the
 *      whole group that matters, not any one variable's position, and these are
 *      reload SPILLS rather than address-taken locals -- the same downward-from-the-
 *      top-of-frame correspondence that governs expand_decl reaches them too.
 *
 * ============ THE BLOCKER, NAMED BY THE PASS ============
 *
 * LOCAL-ALLOC / GLOBAL-ALLOC: ONE MISSING SPILL.  `boxQ` KEEPS A REGISTER WHERE
 * THE ROM SPILLS IT, so the frame is 0x20 against 0x24 and every sp displacement in
 * the second half shifts.  Secondary: jump.c's single-use-label block motion in
 * `jump_optimize`.
 *
 * THE ACCOUNTING, because the first reading of it was wrong.  We are 17 encodings
 * short, and objdump -dz counts LITERAL POOL WORDS in that stream, so 17 is not 17
 * instructions.  Every one is named by an aligncmp `delete` hunk:
 *   - THREE ARE POOL WORDS, not instructions: `.word 0x75`, `.word 0x00000000` and
 *     `.word 0x00000c9c`.  The first is the tell -- the ROM spells `item + 0x75` as
 *     `ldr r1,=0x75 / adds r1,r5,r1` (pool a constant that FITS an 8-bit immediate,
 *     then a three-operand register add) where we spell it `mov / add rd,#imm8`.
 *     Both are two instructions; only the pool word differs.
 *   - FIVE ARE THE MISSING boxQ SPILL: `str r0,[sp,#20]` after its _CreateUIBox,
 *     `mov r2,r8 / str r2,[sp,#0] / ldr r0,[sp,#20]` at the _Func_80a1870 call, and
 *     `ldr r0,[sp,#20]` at its _CloseUIBox.
 *   - TWO are the boxQty/cur2 register choice (`movs r7,#0 / mov r9,r0`: the ROM
 *     puts boxQty in r9 and zeroes cur2 in r7; we put boxQty in r7).
 *   - The rest are constant materialisations and one `ldrsh` that appear in an
 *     insert/delete PAIR -- present in our object at a different index, so they are
 *     scheduling, not absence.  This is exactly why aligncmp ranks and the count
 *     does not.
 *
 * SO THE HOIST IS A CONTRIBUTOR, NOT THE CAUSE, and the first draft of this note
 * over-attributed to it.  MEASURED: writing the handlers INLINE (the `break` form)
 * gives 531 instructions and writing them at the bottom gives 533, a difference of
 * TWO -- while the aligned figure moves 198 -> 282.  The hoist therefore costs
 * almost no length; what L1 buys is BLOCK ORDER over three quarters of the function.
 * The length is the spill.
 *
 * The hoist is real and reproducible.  Verified in the generated Thumb, compiled
 * with objcmp's own flags for this file (-O2 -mthumb -mthumb-interwork
 * -mcpu=arm7tdmi -fno-builtin -nostdinc -ffreestanding -fcall-used-r4):
 *
 *     .L5:  ldr / ldr / tst r3,#1 / beq .L6
 *     .L7:  mov r0,#112 / bl _PlaySound / ... / b .L19      <- hoisted accept arm
 *     .L6:  tst r3,#2 / beq .L8
 *     .L9:  mov r0,#113 / bl _PlaySound / ... / b .L19      <- hoisted cancel arm
 *
 * where the ROM has `beq .Lb0bae / b .Lb0f48` and leaves .Lb0f48 at the bottom.
 * The block gcc moves is in every case (a) the target of exactly ONE forward branch
 * and (b) terminated by an unconditional jump, so moving it deletes the label and
 * saves the `b`.
 *
 * WHAT RULES OUT THE ALTERNATIVES.
 *   - NOT the `&ctx->f39e` pointer.  The ROM derives that address four separate
 *     times (r5 at .Lb0c78, .Lb0d72 and .Lb0e28, r6 at .Lb0ee4) and it looked
 *     exactly like a named-pointer allocno.  It is not: ONE function-scope pointer
 *     is WORSE (aligned 260), and FOUR block-scoped pointers, one per region the
 *     ROM re-derives it in, are BYTE-IDENTICAL to naming nothing -- 530 / 533 /
 *     282 / 361 in 103 hunks, the same numbers to the digit.  gcc already CSEs the
 *     address within each block, so the pointer is not a spellable quantity here.
 *     This is the "one variable per region" rule coming out INERT, which is itself
 *     worth knowing: the rule pays when the regions want DIFFERENT registers, not
 *     merely when the reference recomputes an address.
 *   - NOT the signed-char forms (L2): all four already match.
 *   - NOT declaration order FOR boxQ ITSELF: first and last are both inert (above).
 *     allocno_compare's other two inputs are n_refs (boxQ has five) and live_length,
 *     and nothing a declaration can say moves either.
 *   - NOT scheduling at the top.  Moving `cur = sel;` ahead of the boxList
 *     _CreateUIBox (which is where the ROM schedules its `ldr r7,[sp,#0x10]`) and
 *     giving `ctx->f020 = 0` its own block-scoped zero -- the ROM uses two
 *     different zero registers there -- together move 362 differing to 361 and one
 *     hunk.  Real but negligible; kept because they are free.
 *   - NOT a struct-layout error.  Every field offset, both short tables (0x26c and
 *     0x36e), the 0x380 indirection and all four sub-0x3ff bytes reproduce their
 *     reference encodings.
 *
 * NEXT PROBE, if this is picked up.  The boxQ spill is what stands between this
 * candidate and a countable distance; it needs one MORE long-lived value competing
 * for the callee-saved registers, and the four `&ctx->f39e` derivations are already
 * proved not to be it.  For the block order, the question is what makes a bottom
 * handler ineligible for it -- a SECOND reference to the label is the obvious
 * candidate (gcc can only move a block it can delete the label of), which in
 * source terms means two tests that share one handler.  Two of the four handlers
 * here ARE identical (`_PlaySound(0x71); res = -1;` before the two different
 * closes), so a shape where the inner-1 and inner-2 cancel arms reach ONE label is
 * worth writing out.  Failing that, this is a candidate for the `-fno-thread-jumps`
 * / per-file flag sweep (tools/rank_parks.py --flags) rather than for another
 * source spelling.
 */
struct Box {
    unsigned char pad0[5];
    unsigned char f5;
};

struct Ctx {
    unsigned char pad000[0x0c];
    int f00c;
    unsigned char pad010[0x10];
    int f020;
    unsigned char pad024[0x248];
    short items[0x81];
    short f36e[9];
    struct Box *f380;
    unsigned char pad384[0x1a];
    unsigned short f39e;
    unsigned char pad3a0[6];
    signed char f3a6;
    signed char f3a7;
    unsigned char f3a8;
    unsigned char pad3a9[1];
    signed char f3aa;
};

typedef struct { unsigned char pad00[0x10]; int f10; } GlobalState;

extern unsigned char iwram_3001f2c[];
extern GlobalState gState;
extern unsigned int gKeyPress;
extern unsigned int gKeyRepeat;

extern int _CreateUIBox(int a, int b, int c, int d, int e);
extern void _CloseUIBox(int box, int a);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern void Func_80b10cc(void);
extern short *_GetItemInfo(int item);
extern void Func_80b0a6c(int box, int a, int b);
extern void Func_80b0fa4(int box, int i);
extern void Func_80b11a4(int box, int a);
extern void _Func_8016498(int box);
extern void Func_80b110c(int box, int a, int b, int c);
extern void Func_80b04dc(int id);
extern void Func_80b0574(int id);
extern int Func_80b0634(int a);
extern int Func_80b0070(void);
extern void _Func_80a1870(int box, int a, int b, int c, int d);
extern void _Func_80a195c(void);
extern void Func_80b11c4(int box, int a, int b);
extern int _Func_8078480(int item);
extern void Func_80b1470(int box, int a, int b);
extern void Func_80b1260(int box, int a, int b);
extern int _GiveItemTo(int who, int item);
extern void _Func_8019908(int a, int b);
extern int _FindEmptyInventorySlot(int who);
extern void _Func_80788c4(int who);
extern int _Func_807845c(int who, int item);
extern int Func_80b153c(int who, int item);
extern void Func_80b17e4(int who, int item, int qty);
extern void Func_80b24e4(int box, int box2);
extern void _Func_8078ad0(int item, int n);
extern int __modsi3(int a, int b);
extern int __divsi3(int a, int b);

int Func_80b0aac(void)
{
    struct Ctx *ctx;
    short *info;
    int boxB;
    int boxList;
    int boxInfo;
    int boxQ;
    int sel;
    int qty;
    int again;
    int cur;
    int n;
    int redraw;
    int res;
    int item;
    short *info2;
    int cur2;
    int who;
    int old;
    int k;
    int t;
    int boxQty;

    ctx = *(struct Ctx **)iwram_3001f2c;
    sel = 0;
    boxB = 0;
    {
        int zero = 0;
        ctx->f020 = zero;
    }
    ctx->f00c = _CreateUIBox(0x12, 7, 0xc, 4, 2);
    Func_80b10cc();
    boxB = _CreateUIBox(0, 8, 0xf, 4, 2);
Louter:
    cur = sel;
    boxList = _CreateUIBox(0, 0xc, 0x1e, 4, 2);
    ctx->f380->f5 = 0x12;
    ctx->f3a8 = 0xc;
    boxInfo = _CreateUIBox(0, 0x11, 0x1e, 3, 2);
    redraw = 1;
Linner1:
    n = ctx->f3a6;
    if (redraw != 0) {
        item = ctx->items[cur];
        info = _GetItemInfo(item);
        redraw = 0;
        Func_80b0a6c(boxList, (__modsi3(cur, 7) << 5) - 8, 8);
        ctx->f3a8 = 4;
        Func_80b0fa4(boxList, cur);
        Func_80b11a4(boxInfo, item + 0x75);
        _Func_8016498(boxB);
        Func_80b110c(boxB, item, info[0], 0);
    }
    if ((gKeyPress & 1) != 0)
        goto Laccept1;
    if ((gKeyPress & 2) != 0)
        goto Lcancel1;
    if ((gKeyRepeat & 0x20) != 0) {
        old = cur;
        cur = __modsi3(cur - 1 + n, n);
        if (cur != old) {
            _PlaySound(0x6f);
            redraw = 1;
        }
    }
    if ((gKeyRepeat & 0x10) != 0) {
        old = cur;
        cur = __modsi3(cur + 1 + n, n);
        if (cur != old) {
            _PlaySound(0x6f);
            redraw = 1;
        }
    }
    if ((gKeyRepeat & 0x40) != 0) {
        if (cur - 7 >= 0) {
            cur = cur - 7;
            redraw = 1;
        }
    }
    if ((gKeyRepeat & 0x80) != 0) {
        if (cur + 7 < __divsi3(n + 6, 7) * 7) {
            cur = cur + 7;
            redraw = 1;
        }
        if (cur > n - 1)
            cur = n - 1;
    }
    WaitFrames(1);
    goto Linner1;
LcloseList:
    _CloseUIBox(boxInfo, 2);
    _CloseUIBox(boxList, 2);
    WaitFrames(1);
    if (res != 0)
        goto Lexit;
    {
        unsigned short *p = &ctx->f39e;
        *p = ctx->items[sel];
        Func_80b04dc(0xc9d);
        info2 = _GetItemInfo(*p);
    }
    qty = 1;
    again = 0;
    boxQ = _CreateUIBox(0, 0xe, 0xd, 3, 2);
    ctx->f380->f5 = 4;
    ctx->f3a8 = 0xc;
    _Func_80a1870(boxQ, 2, 0, 8, res);
    boxQty = _CreateUIBox(0x10, 0xb, 0xe, 9, 2);
    cur2 = 0;
    redraw = 1;
Linner2:
    if (again != 0) {
        again = 0;
        Func_80b04dc(0xc9d);
        redraw = 1;
    }
    if (redraw != 0) {
        unsigned short *p;
        t = ctx->f3a7;
        redraw = 0;
        cur2 = __modsi3(cur2 + t, t);
        who = ctx->f36e[cur2];
        Func_80b0a6c(boxQ, (cur2 * 3) * 8 - 0xc, 0);
        ctx->f3a8 = 3;
        p = &ctx->f39e;
        Func_80b11c4(boxQ, cur2, *p);
        if (_Func_8078480(*p) == 0)
            Func_80b1470(boxQty, who, *p);
        else
            Func_80b1260(boxQty, who, *p);
    }
    if ((gKeyPress & 1) != 0) {
        unsigned short *p = &ctx->f39e;
        if (_GiveItemTo(who, *p) < 0) {
            _PlaySound(0x71);
            _Func_8019908(who, 1);
            _Func_8019908(*p, 2);
            if (_FindEmptyInventorySlot(who) == 0xf)
                Func_80b04dc(0xc9e);
            else
                Func_80b04dc(0xca6);
            goto Linner2;
        }
        _Func_80788c4(who);
        if ((unsigned int)info2[0] > (unsigned int)gState.f10)
            goto Lnomoney;
        if (_Func_807845c(who, *p) == 0) {
            _Func_8019908(who, 1);
            Func_80b04dc(0xc9f);
            if (Func_80b0634(0) != 0) {
                again = 1;
                goto Linner2;
            }
            again = 1;
        }
        {
            unsigned short *q = &ctx->f39e;
            _PlaySound(0x70);
            WaitFrames(1);
            qty = Func_80b153c(who, *q);
            again = 1;
            if (qty == -1)
                goto Linner2;
            Func_80b17e4(who, *q, qty);
        }
        Func_80b24e4(boxQ, boxQty);
        res = 0;
        goto Lclose2;
    }
    if ((gKeyPress & 2) != 0)
        goto Lcancel2;
    if ((gKeyRepeat & 0x20) != 0) {
        _PlaySound(0x6f);
        cur2--;
        redraw = 1;
    }
    if ((gKeyRepeat & 0x10) != 0) {
        _PlaySound(0x6f);
        cur2++;
        redraw = 1;
    }
    WaitFrames(1);
    goto Linner2;
Lclose2:
    _Func_80a195c();
    _CloseUIBox(boxQty, 2);
    _CloseUIBox(boxQ, 2);
    WaitFrames(1);
    if (res == 0 && ctx->f3aa == 2) {
        if (res < qty) {
            unsigned short *p = &ctx->f39e;
            k = qty;
            do {
                _Func_8078ad0(*p, -1);
                k--;
            } while (k != 0);
        }
        if (Func_80b0070() == 0)
            goto Lexit;
        t = ctx->f3a6 - 1;
        if (sel > t)
            sel = t;
    }
    Func_80b04dc(0xca8);
    goto Louter;
Lcancel2:
    _PlaySound(0x71);
    res = -1;
    goto Lclose2;
Lnomoney:
    _PlaySound(0x71);
    Func_80b0574(0xc9c);
    res = -1;
    goto Lclose2;
Lcancel1:
    _PlaySound(0x71);
    res = -1;
    goto LcloseList;
Laccept1:
    sel = cur;
    _PlaySound(0x70);
    res = 0;
    goto LcloseList;
Lexit:
    _CloseUIBox(boxB, 2);
    _CloseUIBox(ctx->f00c, 2);
    WaitFrames(1);
    return 0;
}

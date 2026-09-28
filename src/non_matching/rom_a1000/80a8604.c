/* Func_80a8604 -- DrawStatusNumbers -- 330 instructions.
 * NON-MATCHING, 295 encodings of 341.  ours 335 encodings / 756 bytes against ref 341 / 768, so 295 is NOT a true distance.
 *
 * (This claim line is first on purpose: tools/parkcheck.py reads the FIRST
 *  `N encodings of M` in the header, and the drop ladder below is full of
 *  `N of M` strings whose earliest is the ladder's WORST rung, not the claim.)
 * Reference: asm/rom_a1000/rom_a8604_a_a_a.s -- ONE function, datacheck clean,
 * so this is a WHOLE-FILE conversion (objcmp --whole).
 *
 * Verify:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a8604.c asm/rom_a1000/rom_a8604_a_a_a.s --whole
 *
 * STRUCTURE READ (from the .s, before any compile):
 *  - state(r8) = iwram_3001f2c, loaded BEFORE the _GetNumDjinn call.
 *  - `neg r3,r0 / orr r3,r0 / lsr r3,#31` is the int-valued `!= 0` idiom, so
 *    hasDjinn = _GetNumDjinn(-1) != 0.
 *  - `cmp r3,#1 / beq` on hasDjinn (not cmp #0), so the tests are spelled `== 1`.
 *  - col = 7; if ((flags & 0xff) != 1) col = 10.  The ROM stores 7 first and
 *    conditionally overwrites, which is the plain if-form, not a ternary.
 *  - buf is a 5-used/8-byte stack array at sp+0x20 (frame 0x28).
 *  - the five icon blocks are HAND-UNROLLED in source (five distinct msg ids
 *    0xbd5..0xbd9 and five distinct buf bytes), each `y = n*16 + 0x28`.
 *    The first block's `mov r2,#1 / mov r10,r2` is cprop folding 0+1 from the
 *    just-initialised n, so all five are written uniformly as `n++`.
 *  - `bne .La873a / b .La88c6` is a long-branch pair for `if (... == 3) return;`
 *    jumping to the shared epilogue.
 *  - the tail loop is i = 0..3 with `cmp #3 / ble` (signed) and NO entry guard,
 *    so a plain `for` is correct here (constant bounds; contrast the
 *    guard+do/while lever from Func_80a8f40, which had a variable count).
 *  - three x induction variables 0x68/0x70/0x78 step 0x20; gcc keeps bivs for
 *    0x68 and 0x78 and derives 0x70 as r9-8.  Written as i*0x20 + K.
 *  - col*8 is spilled at sp+8 and rematerialised (`lsl r6,r3,#3`) on the
 *    else-path, so it is written inline as `col * 8`, not as a named local.
 *  - `ldrsh r0,[r3,r2]` with r2 = 0 / 2 is the only Thumb form for a signed
 *    halfword load, so `*(short *)(unit + 0x48 + i*4)` needs no trick.
 *
 * EXTERN CONVENTIONS taken from the same family's parked sibling
 * src/non_matching/rom_a1000/80a8f40.c (same drawing routine one page over):
 * `unsigned int win`, _Func_801ea08(v, n, win, x, y), and the
 * `extern unsigned char af22c[] __asm__(".Laf22c");` binding, which needs NO
 * label.sym entry -- .Laf22c and .Laf230 are already .global at
 * asm/rom_a1000/rom_a8604_c_c_c_c_c.s:4-5.
 *
 * DROP LADDER (objcmp --whole, ref 341 encodings / 768 bytes):
 *   first candidate ....................................... 322 of 341 (ours 334)
 *   + the `!= 0` idiom written LONGHAND ................... 299 of 341 (ours 334)
 *   + `col` declared BEFORE `hasDjinn` .................... 296 of 341 (ours 334)
 *   + 0xafe/0xaff as a variable (`msg = 0xafe; ...; msg++`)  295 of 341 (ours 335)
 *
 * 295 IS NOT A TRUE DISTANCE: ours 335 encodings / 756 bytes against ref 341 /
 * 768.  The 6-instruction shortfall IS the blocker's signature, see below.
 *
 * LEVER, REUSABLE: gcc-2.96 DOES NOT SYNTHESISE `neg/orr/lsr #31` FOR `x != 0`.
 * Written as `hasDjinn = _GetNumDjinn(-1) != 0;` gcc emits a BRANCH
 * (cmp/beq/mov/str, 5 insns).  The ROM's 3-insn branchless form only appears if
 * the bit-twiddle is spelled out: `t = f(); hasDjinn = (unsigned)(-t | t) >> 31;`
 * This is the tree's existing convention -- src/rom_8a000/rom_8ba38_b.c does
 * exactly this against the same `bl _GetFlag / neg / orr / lsr #31` stream.
 * Worth 23 differing here (322 -> 299).  NOT a shim: plain C.
 *
 * LEVER: SPILL-SLOT ORDER IS DECLARATION ORDER.  The ROM puts hasDjinn at
 * sp+0xc and col at sp+0x10; reload assigns stack slots in ascending pseudo
 * number and pseudo numbers follow DECLARATION order, and the frame here grows
 * downward, so the LATER-declared local gets the LOWER offset.  Declaring
 * `col` before `hasDjinn` swapped both slots into place (299 -> 296).
 *
 * WHAT IS RIGHT: everything except register names.  The 35 relocations are
 * IDENTICAL in symbol and in ORDER, and past index 10 the two streams pair up
 * line for line -- every call, every constant, every branch, every stack slot.
 * The only per-line differences are the five register renames below.
 *
 * BLOCKER -- global.c `allocno_compare` (global register allocation), PROVED
 * UNREACHABLE BY ARITHMETIC.  gcc has exactly THREE low callee-saved registers
 * here: the prologue is `push {r5,r6,r7,lr}` in BOTH streams, so r4 is never
 * used.  Four values contend for them:
 *
 *     value   n_refs  floor_log2*n_refs  live_length  priority   ROM   ours
 *     n         11           33              ~62        0.53     r10    r5
 *     win       23           92             ~330        0.28     r7     r6
 *     buf       10           30             ~130        0.23     r5     r7
 *     nc         3            3             ~100        0.03     r6     r8
 *     state      3            3             ~138        0.02     r8     r10
 *
 * `allocno_compare` sorts by floor_log2(n_refs)*n_refs / live_length descending
 * and `find_reg` then walks REG_ALLOC_ORDER, so gcc allocates n, win, buf into
 * r5, r6, r7 and pushes nc and state to high registers.  That is EXACTLY what
 * our compile does -- gcc is obeying its own formula.  The ROM's order is
 * buf > nc > win > state > n, with `n` LAST despite having by far the highest
 * refs-per-length ratio of the five.
 *
 * WHY NO SOURCE SHAPE CAN REACH IT.  Both n_refs and live_length are fixed by
 * the instruction stream, and our stream already MATCHES the ROM's, so the
 * table above is not ours to move:
 *   - for `nc` to outrank `n` needs 3/L_nc > 33/L_n, i.e. L_nc < 5.6 insns.  But
 *     the ROM itself tests nc again at .La873a, AFTER the whole 5-icon block, so
 *     L_nc >= ~100 is forced by the ROM's own layout.  Ratio 18x short.
 *   - for `nc` to outrank `win` needs L_nc < 10.7.  Same contradiction.
 * So `nc` is always the value pushed high and `n` always takes a low register.
 *
 * THE 6-INSTRUCTION SHORTFALL IS THAT SAME FACT, NOT A SECOND BLOCKER.  `n` in
 * r10 costs the ROM a `mov r2,r10` before each `lsl r3,r2,#4` and a
 * `mov r3,#1 / add r10,r3` instead of `add r5,#1` -- about +11 across the five
 * icon blocks -- while `nc` in r8 costs us only +2 (`mov r8,r3`, `mov r2,r8`).
 * A size match therefore CANNOT coexist with the blocker; the deficit is the
 * blocker's readout.
 *
 * A REGISTER-PINNING SHIM DOES NOT RESCUE IT EITHER.  Thumb gcc-2.96 has an
 * `h` constraint (HI_REGS), so `__asm__ ("" : "+h" (n));` could force n high.
 * But the remaining three would then allocate win, buf, nc into r5, r6, r7 by
 * the same priority order, and the ROM wants buf, nc, win.  Two renames survive
 * the shim, so it buys nothing and was not written.
 *
 * TRIED AND INERT: `unsigned int` / `unsigned char` for n (unsigned char adds
 * the lsl#24/lsr#24 pair the ROM does not have); a named local for
 * `flags & 0x100`; a named local for `col * 8`; `(flags & 0xff)` hoisted to a
 * local (the ROM rematerialises it inside the tail loop, so inline is right).
 *
 * NO SHIMS IN THIS FILE.  `af22c` / `af230` are __asm__ name bindings for
 * labels that are not valid C identifiers, not register pins, and both are
 * already .global at asm/rom_a1000/rom_a8604_c_c_c_c_c.s:4-5 -- no label.sym
 * entry needed (same finding as src/non_matching/rom_a1000/80a8f40.c).
 */

extern unsigned char *iwram_3001f2c;
extern unsigned char af22c[] __asm__(".Laf22c");
extern unsigned char af230[] __asm__(".Laf230");

extern int _GetNumDjinn(int id);
extern unsigned char *_GetUnit(int id);
extern void _SetTextColor(int c);
extern void WaitFrames(int n);
extern void _Func_80164d4(unsigned int win, int x, int y, int w, int h);
extern void _Func_8019000(unsigned int win, int a, int b, int c, int d);
extern void _Func_801e7c0(int id, unsigned int win, int x, int y);
extern void _Func_801e8b0(void *unit, unsigned int win, int x, int y);
extern void _Func_801ea08(int v, int n, unsigned int win, int x, int y);
extern void _UIDrawText(void *s, unsigned int win, int x, int y);
extern int _Func_807987c(int id, int i);
extern void Func_80a8914(unsigned int win, int id, int flags);
extern void Func_80a8b10(void *buf, int a, int id);
extern void Func_80a9dc4(void *buf);
extern void Func_80a9d3c(void *buf);

int Func_80a8604(unsigned int win, int id, int flags)
{
    unsigned char *state;
    unsigned char *unit;
    unsigned char buf[8];
    int col;
    int hasDjinn;
    int n;
    int i;
    int t;
    int msg;

    state = iwram_3001f2c;
    t = _GetNumDjinn(-1);
    hasDjinn = (unsigned int)(-t | t) >> 31;
    unit = _GetUnit(id);
    col = 7;
    if ((flags & 0xff) != 1)
        col = 0xa;
    *(char *)(*(int *)(state + 0x17c) + 5) = 1;
    Func_80a8914(win, id, flags);
    Func_80a8b10(buf, 1, id);
    Func_80a9dc4(buf);
    if ((flags & 0x100) == 0)
        _Func_80164d4(win, 0, 0x28, 0x60, 0x60);
    n = 0;
    if (buf[0] != 0) {
        _Func_801e7c0(0xbd5, win, 0x10, n * 16 + 0x28);
        n++;
    }
    if (buf[1] != 0) {
        _Func_801e7c0(0xbd6, win, 0x10, n * 16 + 0x28);
        n++;
    }
    if (buf[2] != 0) {
        _Func_801e7c0(0xbd7, win, 0x10, n * 16 + 0x28);
        n++;
    }
    if (buf[3] != 0) {
        _Func_801e7c0(0xbd8, win, 0x10, n * 16 + 0x28);
        n++;
    }
    if (buf[4] != 0) {
        _Func_801e7c0(0xbd9, win, 0x10, n * 16 + 0x28);
        n++;
    }
    if (n == 0)
        _Func_801e7c0(0xbd4, win, 0, 0x28);
    Func_80a9dc4(buf);
    Func_80a9d3c(buf);
    if (*(unsigned short *)(state + 0x220) == 3)
        return 0;
    if ((flags & 0x100) == 0) {
        WaitFrames(1);
        _Func_80164d4(win, 0x40, 0x38, 0xe0, 0x60);
    }
    _SetTextColor(0xf);
    if (flags == 1 || hasDjinn == 1) {
        _Func_8019000(win, 1, 0xf, col, 4);
        _Func_8019000(win, 2, 0x13, col, 4);
        _Func_8019000(win, 3, 0x17, col, 4);
        _Func_8019000(win, 4, 0x1b, col, 4);
    }
    if (hasDjinn != 0)
        _Func_801e7c0(0xafd, win, 0x40, col * 8 + 8);
    if (flags == 1) {
        if (hasDjinn == 0)
            col--;
        _Func_801e8b0(af22c, win, 0x40, col * 8 + 0x10);
        msg = 0xafe;
        _Func_801e7c0(msg, win, 0x40, col * 8 + 0x18);
        msg++;
        _Func_801e7c0(msg, win, 0x40, col * 8 + 0x20);
    }
    for (i = 0; i <= 3; i++) {
        if (hasDjinn != 0)
            _Func_801ea08(unit[0x118 + i], 1, win, i * 0x20 + 0x78, col * 8 + 8);
        if ((flags & 0xff) == 1) {
            if (hasDjinn != 0) {
                _Func_801ea08(unit[0x11c + i], 1, win, i * 0x20 + 0x68, col * 8 + 8);
                _UIDrawText(af230, win, i * 0x20 + 0x70, col * 8 + 8);
            }
            _Func_801ea08(_Func_807987c(id, i), 2, win, i * 0x20 + 0x70, col * 8 + 0x10);
            _Func_801ea08(*(short *)(unit + 0x48 + i * 4), 3, win, i * 0x20 + 0x68, col * 8 + 0x18);
            _Func_801ea08(*(short *)(unit + 0x4a + i * 4), 3, win, i * 0x20 + 0x68, col * 8 + 0x20);
        }
    }
    return 0;
}

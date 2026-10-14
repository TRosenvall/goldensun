/* Func_80a8f40 -- DrawEquipPage -- PARK AT 6 of 167 stream encodings.
 * UNCHANGED FIGURE.  Body below is the park's body, unedited, including its one
 * `__asm__(".equ _MSG_333, 0x333")' MEASUREMENT SHIM (_MSG_333 is already
 * admitted in message.sym from batch 285; a LANDED file must not carry the shim).
 *
 * MEASURED BATCH 350, BRIEF D (not inherited): 6 differing encodings of 167
 * (ref 167 / ours 167), 143 INSTRUCTIONS against 143 -- IN PHASE, so the figure
 * IS a distance.  dsize 0, relocations clean, first differing index 139
 * (ref 9b01 = `ldr r3,[sp,#4]'; ours 9901 = `ldr r1,[sp,#4]').  The park's 6
 * survives for the SIXTH consecutive batch.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" \
 *   -w /work goldensun-build python3 tools/objcmp.py \
 *   src/non_matching/rom_a1000/80a8f40.c \
 *   asm/rom_a1000/rom_a8604_a_a_c_c_c.s --func Func_80a8f40
 *
 * SPLIT SHAPE (unchanged, b322): a pure three-way text split.  datacheck.py
 * silent; `split_s.py ... Func_80a8f40 --dry-run' would write _a.s (1 func, 254
 * lines), _b.s (1 func, 171 lines, THIS function), _c.s (1 func, 325 lines),
 * remove the combined .s and rewrite stage1.ld.  Install path on a landing:
 * src/rom_a1000/rom_a8604_a_a_c_c_c_b.c.  Exports: none.  NOTE the piece's worst
 * member is Func_80a90bc at 216 of 303, so a SPLIT is the practical route; this
 * function must not wait on the whole TU.
 *
 * ================================================================
 * ===== BATCH 350: THE ENTIRE RECORDED NEGATIVE LIST IS ONE PROGRAM =====
 * ================================================================
 *
 * Rebuilt all THIRTEEN recorded "EXACTLY INERT AT 6" spellings and swept them
 * with tools/sweep_variants.py's object fingerprinting.  *** ALL THIRTEEN ARE
 * BIT-IDENTICAL TO THE BASE OBJECT. ***  13 recorded spellings, ONE distinct
 * program, 100% measured nothing.  The park's wording is true and understates
 * it: not one of these reached the back end.
 *     b329's seven device-free 0x741 routes:
 *       `int base = 0x741;' ... `const int base = 0x741;'
 *       `int msgid = unit[0x129] + 0x741;' ... `0x741 + unit[0x129]'
 *       file-scope `static const int kMsgBase = 0x741;'
 *       `*(unit + 0x129) + 0x741' ... `unit[0x129] + 0x740 + 1'
 *     b336's one-member `union MsgBase { int v; }' carrier
 *     b328's five ADDRESS spellings of unit[0xf]:
 *       `p = unit + 0xf; *p'   `p = &unit[0xf]; *p'   `*(unit + 0xf)'
 *       `q = (int)unit + 0xf; *(unsigned char *)q'    `p = unit; p[0xf]'
 *
 * *** A CAVEAT ABOUT THE FINGERPRINT, FOR tools/sweep_variants.py's DOCSTRING:
 * *** IT HASHES ENCODINGS + RELOCATIONS, NOT SIZE.  The `static const int
 * *** kMsgBase' row is bit-identical in the code stream and STILL reports
 * *** dsize=4, because the unused static is emitted.  So a "BIT-IDENTICAL" row
 * *** can change the TU size.  That cannot matter for a figure; it can matter
 * *** for a landing.
 *
 * ===== THE REFERENCE TAIL CONFIRMS THE MECHANISM =====
 *     add  r3, r1, r2      r1 = unit reload, r2 = the 0x129 reload
 *     ldrb r0, [r3]
 *     ldr  r3, =0x741      A POOL LOAD, reusing r3
 *     mov  r1, r8 / add r0, r3 / mov r2, #0 / mov r3, #0x20 / bl _Func_801e7c0
 *     ...
 *     ldr  r3, [sp, #4]    <- INDEX 139.  Ours is `ldr r1,[sp,#4]'.
 *     ldrb r0, [r3, #0xf]
 * The ROM's 0x741 is a POOL LOAD into r3 -- the same encoding a reload emits,
 * which is exactly why the pool-word evidence is inconclusive.  What differs is
 * WHICH PASS creates it, hence whether it advances `last_spill_reg'
 * (`set_reload_reg', reload1.c:4937, which assigns only on acceptance).
 *
 * ===== THE ARITHMETIC RE-DERIVED, AND IT IS CHEAPER THAN THE PARK STATES =====
 * Window allocations in our build: r1 (unit), r2 (0x129), r3 (0x741).  Insn 322
 * then scans from last_spill_reg = r3 with a PRE-INCREMENT (reload1.c:5003):
 * r0 is the insn's own output (pseudo 108 in r0) so it is rejected, and r1 is
 * accepted.
 *     last = r0 -> r1     last = r1 -> r2     last = r2 -> r3     last = r3 -> r1
 * Only last_spill_reg = r2's index reaches the ROM's r3.
 *
 * *** SO ONE FEWER ALLOCATION IN THE WINDOW REACHES r3, which is CHEAPER than
 * *** the park's recorded "add THREE more, each emitting an instruction".  The
 * *** park only ever chased removing the 0x741 allocation.  The symmetric
 * *** candidate is the 0x129 allocation: drop it and the order becomes
 * *** r1 (unit), r2 (0x741), r3 (unit[0xf]) -- the ROM's register.
 * *** BUT THE ROM HAS THE 0x129 RELOAD: `ldr r2, =0x129' feeding
 * *** `add r3, r1, r2' is in the reference stream above, so removing it is not
 * *** available, and b336's `int k = 0x129;' row measured 65 of 167 at 142
 * *** instructions of 143 -- a LENGTH defect, not a figure.
 * Recorded with its evidence so the next reader does not re-derive it.
 *
 * ===== VERDICT: TERMINAL DEVICE-FREE AT 6 =====
 * Every route from a compile-time integer constant is now closed AND shown to be
 * a single program.  The 0 of 167 at `unit[0x129] + (int)&_MSG_741' with
 * `extern int _MSG_741;' (b329, byte-identical, relocation table identical) is
 * an OWNER DECISION under owner standard 1 and is not pre-empted here.
 * Blocker class unchanged: `allocate_reload_reg' round-robin position
 * (reload1.c:5003) at insn 322, set by the COUNT of reload-register allocations
 * in the window.  PARK HELD AT 6 DEVICE-FREE.
 * DO NOT RE-SWEEP: the flag axis (b340, all groups), the anchor axis (b340,
 * 30 of 30 including the one placement autosweep could not compile), the thirteen
 * rows above, or any further spelling of 0x741.
 */
__asm__(".equ _MSG_333, 0x333");

struct MoveInfo { unsigned char pad_00[8]; unsigned char f8; unsigned char f9; };

extern unsigned char *iwram_3001f2c;
extern unsigned char *_GetUnit(int id);
extern struct MoveInfo *_GetMoveInfo(int id);
extern void Func_80a2324(int count, int first, unsigned int win, int x, int y);
extern void Func_80a21b0(unsigned int win, int total, int perPage, int page, int col);
extern void Func_80a8cc0(unsigned int win, int a, int b, int c, int d);
extern void _Func_8016498(unsigned int win);
extern void _Func_801e7c0(int id, unsigned int win, int x, int y);
extern void _Func_801e8b0(void *unit, unsigned int win, int x, int y);
extern void _Func_801e9d4(int v, int n, unsigned int win, int x, int y);
extern void _Func_801ea08(int v, int n, unsigned int win, int x, int y);
extern void _UIDrawText(void *s, unsigned int win, int x, int y);
extern unsigned char af22c[] __asm__(".Laf22c");
extern int _MSG_333;

int Func_80a8f40(unsigned int win, int a1, int *d)
{
    unsigned char *state;
    unsigned char *unit;
    unsigned char n;
    unsigned char i;
    int first;
    int ofs;
    int y;
    int id;
    int t;
    struct MoveInfo *info;

    state = iwram_3001f2c;
    unit = _GetUnit(state[0x21a]);
    _Func_8016498(win);
    first = d[2] * 5;
    n = d[5] - first;
    if (n > 5)
        n = 5;
    Func_80a2324(5, first, win, 0x50, 0x3a);
    Func_80a21b0(win, d[5], 5, d[2], 0x1c);
    _Func_801e7c0(0xaed, win, 0xb0, 0);
    i = 0;
    if (n > i) {
        ofs = first * 2 + 0xe4 * 2;
        do {
            info = _GetMoveInfo(*(unsigned short *)(ofs + (int)state) & 0x3fff);
            id = (*(unsigned short *)(ofs + (int)state) & 0x3fff) + (int)&_MSG_333;
            y = i * 16 + 0x10;
            _Func_801e7c0(id, win, 0x58, y);
            _Func_801e9d4(info->f9, 2, win, 0xb0, y);
            t = info->f8;
            if (t == 0xff)
                t = 0xb;
            else
                t = t - 1;
            Func_80a8cc0(win, 0x19, i * 2 + 2, t, 0);
            i++;
            ofs += 2;
        } while (n > i);
    }
    if (state[0x218] == 0)
        _Func_801e7c0(0xaef, win, 0x60, 0x11);
    _Func_801e8b0(unit, win, 0x28, 0);
    _Func_801e7c0(unit[0x129] + 0x741, win, 0, 0x20);
    _UIDrawText(af22c, win, 0, 0x30);
    _Func_801ea08(unit[0xf], 2, win, 0x18, 0x30);
    return 1;
}

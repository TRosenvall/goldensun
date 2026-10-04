/* Func_80a8f40 -- DrawEquipPage -- 0x080a8f40, asm/rom_a1000/rom_a8604_a_a_c_c_c.s
 * NON-MATCHING, 6 of 167 encodings (measured batch 322).
 *
 * PARK, 6 of 167 encodings  (MEASURED batch 322, brief H).  PINS: 0.
 * Instruction count matches, 167 against 167; RELOCATIONS ARE IDENTICAL.
 * The figure IS a distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a8f40.c \
 *     asm/rom_a1000/rom_a8604_a_a_c_c_c.s --func Func_80a8f40
 *
 * SPLIT SHAPE: a pure text split, three ways.  `tools/datacheck.py
 * asm/rom_a1000/rom_a8604_a_a_c_c_c.s` is silent (exit 0 -- no data in the .s).
 * `tools/split_s.py asm/rom_a1000/rom_a8604_a_a_c_c_c.s Func_80a8f40 --dry-run`:
 *     would write ..._a.s  (1 function, 254 lines)
 *     would write ..._b.s  (1 function, 171 lines)   <- this function
 *     would write ..._c.s  (1 function, 325 lines)
 *     would REMOVE ..._a_a_c_c_c.s, would rewrite stage1.ld
 * Install path on a landing: src/rom_a1000/rom_a8604_a_a_c_c_c_b.c.  Exports: none.
 *
 * VERIFICATION SHIM, scratch only: the `__asm__(".equ _MSG_333, 0x333")` line
 * below.  _MSG_333 is ALREADY ADMITTED in message.sym (batch 285); a landed file
 * must NOT carry the shim.  It is an instrument, not a result, and it is the only
 * device in this file.
 *
 * ============ THE DIAGNOSIS, REPRODUCED IN FULL IN BATCH 322 ============
 *
 * The park's blocker claim is one of the few that survives intact, and I
 * reproduced it rather than inheriting it.  Compiled with
 * `-fno-schedule-insns2`, THE TAIL BECOMES THE ROM'S INSTRUCTION ORDER EXACTLY
 * and the only remaining difference is one register field:
 *
 *   ROM        ldr r3,[sp,#4] / ldrb r0,[r3,#0xf] / mov r3,#0x30 / str r3,[sp]
 *              / mov r1,#2 / mov r2,r8 / mov r3,#0x18 / bl _Func_801ea08
 *   -fno-s2    ldr r1,[sp,#4] / ldrb r0,[r1,#15]  / mov r3,#48   / str r3,[sp]
 *              / mov r1,#2 / mov r2,r8 / mov r3,#24 / bl _Func_801ea08
 *   with s2    ldr r1,[sp,#4] / mov r3,#48 / ldrb r0,[r1,#15] / mov r2,r8
 *              / str r3,[sp] / mov r1,#2 / mov r3,#24
 *
 * So the WHOLE residue is `allocate_reload_reg` (reload1.c:5003) taking r1 for
 * the third reload of the spilled `unit` pointer where the ROM took r3, and the
 * schedule difference is a CONSEQUENCE: with the pointer in r3 the ROM's order
 * is forced, because `mov r3,#0x30` cannot then be hoisted above the `ldrb`.
 * 6 = 2 register fields x their sched2 fan-out.  This is the `last_spill_reg`
 * rotation, and every insn before that block is byte-identical, so the rotation
 * state entering it is identical too -- there is nothing earlier to perturb
 * without changing emitted code.
 *
 * ============ CROSSED SWEEP, BATCH 322 -- FLAT, AND THAT IS THE FINDING ======
 *
 * `tools/crossfire.py --depth 2` over six declaration edits, 7 edits total,
 * every pair.  Reference memory profile ldr=18 ldrb=6 ldrh=2 str=6; BASE carried
 * NO flags (no COUNT, no MEM, no RELOC).  Sixteen rows read EXACTLY 6:
 *
 *   `_GetUnit` -> `void *`                          6  (exactly inert)
 *   `_Func_801ea08` -> `int` return                 6  (exactly inert)
 *   `_UIDrawText` first arg -> `unsigned char *`    6  (exactly inert)
 *   `_Func_801e8b0` first arg -> `unsigned char *`  6  (exactly inert)
 *   `iwram_3001f2c` -> `unsigned char *const`       6  (exactly inert)
 *   ... and every pair of the above                 6  (exactly inert)
 *
 * TRIED AND WORSE in the same sweep, with figures:
 *   `t` as `unsigned char`                          87 of 167 at 169 insns -- COUNT,
 *                                                   so that figure is misalignment
 *   `af22c` as `unsigned char *` not `[]`           23, RELOC + MEM -- a WRONG
 *                                                   PROGRAM (one extra indirection)
 *
 * THE DECLARATION LEVER IS A DIVIDEND HERE, NOT A FIX.  Five independent type
 * corrections are provably free.  I have NOT folded them into the body, because
 * `_Func_801ea08` returning `int` is a guess with no evidence behind it, and the
 * `void` declarations below agree with three other files in this bank
 * (80a8604.c, 80a4924.c, 80a112c.c).  Changing them would trade a measured
 * nothing for an unevidenced claim.
 *
 * RETURN TYPES CHECKED AGAINST THE TREE, not against park extern lines, because
 * a wrong return type has been found twice in this bank: none of _Func_801e7c0,
 * _Func_801e8b0, _Func_801e9d4, _Func_801ea08, _UIDrawText has a DEFINITION in
 * src/ -- they are all still asm -- and every park that declares them declares
 * them `void` (_GetUnit / _GetMoveInfo return pointers, as here).  So there is
 * no definition-level evidence to correct, and the `int`-return lever is inert
 * here by measurement rather than by argument.
 *
 * ============ WHAT IS RIGHT, kept from the park ============
 * The four levers below were transferred from Func_80a6b64 in
 * rom_a5534_c_c_c_a_c_c.s, which went EXACT in batch 290 and is the same drawing
 * routine one screen over.  The first candidate written with all four already in
 * place scored 6, and nothing since has moved it.
 *
 *  1. THE LOOP IS `i = 0; if (n > i) { ofs = ...; do { ... } while (n > i); }`,
 *     NOT a `for`.  The ROM emits the entry guard BEFORE the walking offset's
 *     init, so the init sits in the LOOP PREHEADER, after the copied exit test.
 *     Measured on the twin: 19 differing as a `for`, 6 as guard + do/while.
 *  2. THE LOOP CONDITION IS SPELLED COUNT-FIRST, `n > i`.  `i < n` is 83
 *     differing on the twin: the ROM's `cmp r9, r10 / bhi` puts the count first
 *     and gcc does not commute it.
 *  3. THE ADDRESS IS `*(unsigned short *)(ofs + (int)state)` -- OFFSET FIRST.
 *     `state + ofs` gives `ldrh rD,[state,ofs]`; the ROM has `ldrh rD,[ofs,state]`.
 *  4. `i` AND `n` ARE `unsigned char`.  The lsl #24 / lsr #24 pairs are the QImode
 *     zero-extension, and they are also what makes the two loop compares unsigned
 *     (`bhi` / `bls`).  `int` counters cost 4 instructions and 89 differing.
 *
 * TRIED AND INERT (park, all still 6): a named local for `unit[0xf]`; a second
 * pointer local `u2 = unit`; `*(unsigned char *)(unit + 0x129)`; `&af22c[0]`; a
 * `struct Unit *` with real f0f/f129 fields; an `int`-typed `unit` with casts;
 * declaring _Func_801ea08 / _Func_801e8b0 / _Func_801e7c0 / _Func_801e9d4 `int`.
 * TRIED AND WORSE (park): a shared `y = 0x30` local across the last two calls
 * (90); a local for the 0x741 message id (21); `_UIDrawText` declared `int` (9);
 * hoisting `unit[0xf]` above the _UIDrawText call (21).
 *
 * THE `.Laf22c` REFERENCE NEEDS NO label.sym ENTRY.  `extern unsigned char
 * af22c[] __asm__(".Laf22c");` makes gcc emit the relocation against the label
 * verbatim and objcmp reports the relocation table IDENTICAL.  The label is
 * already `.global` at asm/rom_a1000/rom_a8604_c_c_c_c_c.s:4.
 *
 * NEXT STEP FOR PASS 3, and it is not a C question: shift the `last_spill_reg`
 * rotation.  That needs either one more or one fewer reload-register ALLOCATION
 * earlier in the function (an inherited reload does not advance the rotation),
 * which cannot be arranged without changing emitted code -- or a register pin,
 * which this file deliberately does not carry.
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

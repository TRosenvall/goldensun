/* Func_80a8f40 -- DrawEquipPage -- NON-MATCHING: 6 encodings of 167 differ (objcmp).
 * Size and instruction count MATCH (ref 167 encodings / 17 relocations, ours 167).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a8f40.c \
 *     asm/rom_a1000/rom_a8604_a_a_c_c_c.s --func Func_80a8f40
 *
 * FRESH TARGET (batch 290). The .s holds three functions (Func_80a8d34,
 * Func_80a8f40, Func_80a90bc -- the last is parked); datacheck.py clean, so a
 * landing is a pure text split.
 *
 * VERIFICATION SHIM, scratch only: the `__asm__(".equ _MSG_333, 0x333")` line
 * below. _MSG_333 is ALREADY ADMITTED in message.sym (batch 285); a landed file
 * must NOT carry the shim.
 *
 * WHAT IS RIGHT: everything except one reload register. The four levers below
 * were transferred wholesale from Func_80a6b64 in rom_a5534_c_c_c_a_c_c.s, which
 * went EXACT in this same batch and is the same drawing routine one screen over.
 * First candidate written with all four already in place scored 6.
 *
 *  1. THE LOOP IS `i = 0; if (n > i) { ofs = ...; do { ... } while (n > i); }`,
 *     NOT a `for`. The ROM emits the entry guard BEFORE the walking offset's
 *     init, which means the init sits in the LOOP PREHEADER -- after the copied
 *     exit test. A `for` puts the init ahead of the guard. Measured on the twin:
 *     19 differing as a `for`, 6 as guard + do/while.
 *  2. THE LOOP CONDITION IS SPELLED COUNT-FIRST, `n > i`. `i < n` is 83
 *     differing on the twin: the ROM's `cmp r9, r10 / bhi` puts the count in the
 *     first operand and gcc does not commute it.
 *  3. THE ADDRESS IS `*(unsigned short *)(ofs + (int)state)` -- OFFSET FIRST.
 *     `state + ofs` gives `ldrh rD,[state,ofs]`; the ROM has `ldrh rD,[ofs,state]`,
 *     which is a different encoding. 3 differing on the twin.
 *  4. `i` AND `n` ARE `unsigned char`. The lsl #24 / lsr #24 pair on the counter
 *     increment and on `d[5] - first` is the QImode zero-extension, and it is
 *     also what makes the two loop compares UNSIGNED (`bhi` / `bls`). `int`
 *     counters cost 4 instructions and 89 differing on the twin.
 *
 * BLOCKER -- ONE RELOAD REGISTER, and its cost is multiplied by sched2.
 * The `unit` pointer returned by _GetUnit lives in the sp+4 stack slot and is
 * reloaded three times in the tail. The ROM takes r0, r1, r3; we take r0, r1, r1.
 * With the pointer in r3 the ROM's order is FORCED, because `mov r3,#0x30`
 * cannot then be hoisted above the `ldrb`:
 *
 *     ROM   ldr r3,[sp,#4] / ldrb r0,[r3,#0xf] / mov r3,#0x30 / str r3,[sp]
 *           / mov r1,#2 / mov r2,r8 / mov r3,#0x18
 *     ours  ldr r1,[sp,#4] / mov r3,#0x30 / ldrb r0,[r1,#0xf] / mov r2,r8
 *           / str r3,[sp] / mov r1,#2 / mov r3,#0x18
 *
 * PROVED that the schedule is a CONSEQUENCE, not a second blocker: at
 * -fno-schedule-insns2 our order becomes the ROM's exactly and only the two
 * register fields differ. So the whole residue is `allocate_reload_reg`
 * (reload1.c:5003) picking r1 where the ROM picked r3 -- the `last_spill_reg`
 * rotation, with no low-register pseudo live at that point to push it along.
 * Every insn before that block is byte-identical, so the rotation state entering
 * it is identical too, which is what makes this hard: there is nothing earlier
 * to perturb.
 *
 * TRIED AND INERT (all still 6): a named local for `unit[0xf]`; a second pointer
 * local `u2 = unit`; `*(unsigned char *)(unit + 0x129)` instead of `unit[0x129]`;
 * `&af22c[0]` instead of `af22c`; a `struct Unit *` with real f0f/f129 fields;
 * an `int`-typed `unit` with casts; declaring _Func_801ea08 / _Func_801e8b0 /
 * _Func_801e7c0 / _Func_801e9d4 `int` instead of `void`.
 * TRIED AND WORSE: a shared `y = 0x30` local across the last two calls (90);
 * a local for the 0x741 message id (21); `_UIDrawText` declared `int` (9);
 * hoisting `unit[0xf]` above the _UIDrawText call (21).
 *
 * THE `.Laf22c` REFERENCE NEEDS NO label.sym ENTRY. `extern unsigned char
 * af22c[] __asm__(".Laf22c");` makes gcc emit the relocation against the label
 * verbatim and objcmp reports the relocation table IDENTICAL. The label is
 * already `.global` at asm/rom_a1000/rom_a8604_c_c_c_c_c.s:4.
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

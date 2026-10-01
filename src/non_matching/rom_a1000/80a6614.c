/* Func_80a6614 -- DrawShortcutRow -- NON-MATCHING: 134 encodings of 167 differ (objcmp).
 * Size does NOT match: ref 384 bytes / 167 encodings, ours 376 / 163. The
 * difference count is therefore NOT a distance to exact -- ours is FOUR
 * INSTRUCTIONS SHORT and every encoding after the first shift is counted.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a6614.c \
 *     asm/rom_a1000/rom_a5534_c_c_a_c.s --func Func_80a6614
 *
 * FRESH TARGET (batch 290). The .s holds four functions -- Func_80a63e4,
 * Func_80a65e4, Func_80a6614, Func_80a6794 -- two of them already parked.
 *
 * VERIFICATION SHIMS, scratch only: `__asm__(".equ _MSG_333, 0x333")` and
 * `__asm__(".equ _MSG_ae0, 0xae0")`. _MSG_333 is already admitted; _MSG_ae0 is
 * PROPOSED -- see the symbol note at the bottom.
 *
 * WHAT IS RIGHT: the whole control flow, both halves, every call and every
 * relocation in order (the two _Func_801e7c0 calls land at ref offsets 0x34 and
 * 0x44 exactly). Three levers are established:
 *
 *  1. gState MUST BE A STRUCT, NOT AN ARRAY INDEXED BY A CONSTANT. The ROM's
 *     pool holds the BARE symbol and materialises 0x220 as `mov r3,#0x88 /
 *     lsl r3,#2 / add`, and derives 0x222 from it with `add r1,#2` (move2add) in
 *     one place and a pooled `.word 546` in the other. `*(u16 *)&gState[0x220]`
 *     folds to a single `gState+544` pool word and gets NONE of that. Probed in
 *     isolation: an array subscript, an `(int)gState + K` cast and a u16 recast
 *     all fold; `extern struct State gState;` with a far member, a struct
 *     POINTER, and `unsigned char *g = gState;` all three give the ROM's shape.
 *  2. THE _TextBox OUT-PARAMS ARE FOUR SEPARATE `unsigned int` LOCALS, passed
 *     `(id, &v3, &v2, &v1, &v0)` with `v0` declared first -- the idiom from the
 *     landed src/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_c_b.c. The one read back is
 *     `v1`, the second-lowest slot, which matches the ROM's `ldr r3,[sp,#8]`.
 *     They must be UNSIGNED: the ROM's width test is `cmp r3,#0xa / bhi`, and
 *     `int` locals give the signed `bgt`.
 *  3. 0xae0 NEEDS A SYMBOL (see below); 0xae4..0xae8 do not.
 *
 * BLOCKER -- ONE REGISTER, and it is quantified. Across the first _TextBox call
 * eight pseudos want the seven callee-saved registers r5-r11 (r4 is call-used):
 * win, the width flag, the gState+0x220 address, the FOUR sp addresses, and the
 * 0x3ff mask. The ROM spends its seven on the first seven and REMATERIALISES the
 * mask at all four uses (`ldr r0,=0x3ff / and r0,rX`, free because the pseudo
 * carries a REG_EQUIV). We give the mask r8 and SPILL one sp address to sp+4,
 * which is why our frame is 0x18 and the ROM's is 0x14, and why we are four
 * instructions short overall (we lose the ROM's four rematerialisation loads and
 * gain a str/ldr pair plus two `mov r0,r8` copies).
 *
 * READ OUT OF THE COMPILER, not guessed. `.18.greg` prints
 *     ;; 13 regs to allocate: 81 129 38 32 37 111 63 115 67 75 72 73 74
 * and `.17.lreg` identifies 67 and 115 as the two `(set (reg) (const_int 1023))`
 * pseudos (one per half, each with a REG_EQUIV note) and 72-75 as the four sp
 * addresses. The constants sort ABOVE the addresses because
 * floor_log2(n_refs)*n_refs/live_length rewards their short range: 3 refs over
 * ~25 insns beats 3 refs over the whole function. Flipping that needs the
 * addresses at EIGHT references each, or the constants' range stretched.
 *
 * WHY THE MASK IS ONE PSEUDO PER HALF AND NOT FOUR. cse (pass .03) already
 * shows `*thumb_movhi_insn` setting a pseudo to 1023 with a REG_EQUAL note, so
 * plain CSE merges the two uses inside each half across the _TextBox call; gcse
 * does NOT merge the halves, because gcse.c's want_to_gcse_p returns 0 for
 * CONST_INT. Four short-lived pseudos would each take a caller-saved register
 * and never compete, which is the ROM's picture -- so a source route that stops
 * cse reusing the constant across the call would close this.
 *
 * TRIED AND INERT: `(int)` casts on all four mask operands; the reversed operand
 * order `0x3ff & gState.f220`; `do { } while (0)` between the _TextBox call and
 * the width test (both halves); an `unsigned int v[4]` array with `&v[3]..&v[0]`.
 * MEASURED AS DIAGNOSTICS ONLY: -fno-gcse gives the ROM's 0x14 frame but then
 * hoists NONE of the four sp addresses into r8-r11, so it is the wrong knob;
 * -fno-expensive-optimizations also reaches 0x14; -fno-rerun-cse-after-loop and
 * -fno-cse-follow-jumps change nothing.
 *
 * SYMBOL PROPOSAL -- _MSG_ae0 = 0xae0, with an unusually strong in-function
 * control. 0xae0 == 0xae << 4 is thumb_shiftable_const, so
 * CONST_OK_FOR_THUMB_LETTER is consulted on the VALUE ALONE and gcc builds it
 * with `mov r0,#174 / lsl r0,#4`; the ROM has a single `ldr r0,=0xae0`. No
 * source spelling reaches a pool load -- the exact _MSG_b20 / _MSG_820 argument.
 * The control is the FIVE neighbouring ids in the SAME function -- 0xae4, 0xae5,
 * 0xae6, 0xae7, 0xae8 -- none of which is shiftable, all of which reproduce as
 * plain literals, and all of which sit in the same pool. It does NOT complete
 * the function on its own (the register blocker above is independent), so by the
 * bar in message.sym it should be WITHHELD until the blocker falls.
 */
/* VERIFICATION SHIMS (scratch only): _MSG_ae0 and _MSG_333.
 * _MSG_333 is already admitted in message.sym; _MSG_ae0 is proposed. */
__asm__(".equ _MSG_ae0, 0xae0");
__asm__(".equ _MSG_333, 0x333");

struct State {
    unsigned char pad_000[0x220];
    unsigned short f220;
    unsigned short f222;
};
extern struct State gState;

extern void _TextBox(int id, int *a, int *b, int *c, int *d);
extern void _Func_801e7c0(int id, unsigned int win, int x, int y);
extern void _Func_8019908(int id, int n);
extern void _Func_801e8b0(void *unit, unsigned int win, int x, int y);
extern void *_GetUnit(int id);
extern void _SetTextColor(int c);
extern int _MSG_333;
extern int _MSG_ae0;

int Func_80a6614(unsigned int win)
{
    unsigned int v0, v1, v2, v3;
    int wide;

    if (gState.f220 != 0 && gState.f222 != 0)
        _Func_801e7c0(0xae4, win, 0, -8);
    else
        _Func_801e7c0((int)&_MSG_ae0, win, 0, -8);

    _TextBox((gState.f220 & 0x3ff) + (int)&_MSG_333, &v3, &v2, &v1, &v0);
    wide = 1;
    if (v1 <= 0xa)
        wide = 0;
    if (gState.f220 != 0) {
        _Func_8019908(gState.f220 & 0x3ff, 4);
        _Func_801e7c0(0xae7, win, 0, 0);
        if (wide == 0)
            _Func_801e8b0(_GetUnit(gState.f220 >> 10), win, 0x50, 0);
    } else {
        _Func_801e7c0(0xae5, win, 0, 0);
    }

    _TextBox((gState.f222 & 0x3ff) + (int)&_MSG_333, &v3, &v2, &v1, &v0);
    wide = 1;
    if (v1 <= 0xa)
        wide = 0;
    if (gState.f222 != 0) {
        _Func_8019908(gState.f222 & 0x3ff, 4);
        _Func_801e7c0(0xae8, win, 0, 8);
        if (wide == 0)
            _Func_801e8b0(_GetUnit(gState.f222 >> 10), win, 0x50, 8);
        _SetTextColor(0xf);
    } else {
        _Func_801e7c0(0xae6, win, 0, 8);
    }
    return 1;
}

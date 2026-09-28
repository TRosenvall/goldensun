/* Func_801b010 (OpenPartyPanel) -- 0x0801b010, SECOND of the TWO functions in
 * asm/rom_15000/rom_1aeec_a_a_a_a.s.
 *
 * THE SPLIT.  grep -ci func_start says 2: DisplayMenuArrowCursor at 0x0801aeec
 * (lines 8-153) and Func_801b010 at 0x0801b010 (lines 154-end).  The first is
 * ALREADY PARKED at src/non_matching/rom_15000/DisplayMenuArrowCursor.c (16 of
 * 133), so it stays in assembly and this is the `_a` / `_b` two-way text split.
 * datacheck.py exits 0 with no data section and no EXPORTS line, so NOTHING has
 * to be exported in either direction: no label crosses the boundary either way.
 * Func_801b010 reads only iwram_3001e98, Func_801b36c, CreateUIBox, CloseUIBox,
 * Func_8016478 and Func_801e7c0, all already global.
 *
 * ================= EXACT, PENDING TWO message.sym ENTRIES =================
 *      _MSG_50 = 0x50;
 *      _MSG_51 = 0x51;
 * With them the instruction stream is IDENTICAL: 144 encodings of 144, 312
 * bytes against 312, and all ten pre-existing relocations identical in type,
 * symbol AND offset.  objcmp reports "ENCODINGS differ in 2 place(s)" and a
 * RELOCATIONS difference, and BOTH are the documented const.sym/message.sym
 * PHANTOM-RELOCATION class: the two differing "encodings" are the POOL WORDS
 * themselves (ref 00000051 / 00000050, ours 00000000 twice, because a symbol
 * reference leaves a placeholder plus an R_ARM_ABS32) and the two extra
 * relocations are those same two words.  objcmp CANNOT return OK for this
 * class; `make compare` is the only authority.
 *
 * THE MEASUREMENT BOTH WAYS, which is the bar for proposing a symbol:
 *   literal 0x50 / 0x51  -> `mov r0, #0x50` / `mov r0, #0x51`
 *                           142 encodings against 144, 304 bytes against 312,
 *                           tryc --align 2 of 151 -- i.e. the ONLY residue in
 *                           the whole function, and it is two MISSING POOL
 *                           WORDS, not a code difference.
 *   (int)&_MSG_50 / _51  -> `ldr r0, =_MSG_50` / `=_MSG_51`
 *                           144 of 144, 312 of 312, stream exact.
 * The symbols therefore COMPLETE the function, and there is full in-function
 * control: nothing else differs, in either spelling.  Both values are the first
 * argument of Func_801e7c0, which every other caller in the tree feeds a message
 * id ((int)&_MSG_182, (int)&_MSG_75, (int)&_MSG_333 in the rom_7fc720 overlay),
 * so message.sym is the right file and "named by value" is its own convention.
 * Neither value meets a halfword, so const.sym's halfword exception does not
 * apply and a literal genuinely cannot pool: gcc-2.96 builds 0x50 and 0x51 with
 * an eight-bit `mov` in every spelling tried.
 *
 * ======================== THE THREE LEVERS ========================
 * First candidate: 27 of 151 aligned, 146 instructions against 144.
 *
 * 1. THE TWO TAIL ARMS ARE A `switch`, AND `case 4` COMES BEFORE `case 2`
 *    IN THE SOURCE.  27 -> 4 aligned, and it fixes the instruction count.
 *    The ROM dispatches `cmp #2 / beq <far> ; cmp #4 / bne <end>` and then falls
 *    straight into the 0x51 body, with the 0x50 body laid out LAST.  An
 *    `else if (a == 2) ... else if (a == 4) ...` chain cannot produce that: the
 *    if-chain tests and lays out in the same order, so the a==2 body lands first
 *    and cross-jumping then merges the WRONG arm with the f394 tail.  A `switch`
 *    separates the two orders -- expand_case sorts the TESTS by value (so 2 is
 *    still tested first) while emit_case_nodes lays the BODIES out in SOURCE
 *    order.  Writing `case 4:` first is what puts the 0x51 body next to the
 *    dispatch and lets jump2 cross-jump its tail with the `f394 != 0` arm at
 *    `.L1b10e`, which is the shared `mov r2,#0 / mov r3,#0 / bl / b` the ROM has.
 *
 * 2. EACH HALFWORD CONSTANT STORE NEEDS ITS OWN `unsigned` CARRIER *AND* ITS
 *    OWN ADDRESS LOCAL, IN A BLOCK OF ITS OWN, ADDRESS FIRST.  4 -> 2 aligned.
 *    This is the brief's SET_IO lever, and the reason it is needed is in the
 *    machine description rather than in optimisation: `*thumb_movhi_insn`
 *    (arm.md:4318) lists alternative 1 as "=l" / "mn" BEFORE alternative 5's
 *    "=l" / "I", so recog matches `mn` first and EVERY HImode CONST_INT goes to
 *    the constant pool -- including plain 0, which is why the first candidate
 *    emits `ldrh r3, .L19` over a `.word 0`.  Alternative 5 is unreachable for a
 *    constant.  An SImode carrier moves the value into movsi, where 0 is
 *    `mov r3,#0` and 0x3e7 is CSE'd against the live offset constant 0x3b8 as
 *    the ROM's `add r3, #0x2f`.
 *
 * 3. `u32 b`, NOT `int b` -- `(9 - b) >> 1` must be a LOGICAL shift.  The ROM has
 *    `mov r0,#9 / sub r0,r5 / lsr r0,#1`; a signed `b` gives `asr`.
 *
 * SET_IO ITSELF IS NOT A DROP-IN HERE, and this qualifies the brief.  SET_IO
 * supplies the carrier but no address local, and measured on this function it is
 * WORSE than no carrier at all: 148 instructions / 55 aligned against the plain
 * store's 144 / 4.  Six carrier spellings were measured and only the one below
 * works:
 *   SET_IO on both stores                          148 insns, 55 aligned
 *   SET_IO on the first store only                 148 insns, 55 aligned
 *   one `unsigned v` reused, function scope         146 insns, 59 aligned
 *   two `unsigned` carriers, function scope         146 insns, 59 aligned
 *   carrier in a block, NO address local            146 insns, 59 aligned
 *   both carriers in ONE block with both addresses  141 insns, 49 aligned
 *   address local + carrier, ONE BLOCK PER STORE    142 insns,  2 aligned  <= this
 * The discriminator is exactly docs/elevation.md's "a halfword int carrier must
 * be declared AFTER the address": with no address local there is no ordering to
 * get right, and gcc keeps the previous offset alive in a register and derives
 * the next offset from it (`add r1,r1,#24`) instead of rebuilding it and
 * deriving the VALUE from it, which is the ROM's shape.
 *
 * NO SHIMS: no `register ... __asm__`, no `__asm__ ("")`, no per-file flags, no
 * fakematch row.  Two .sym entries, nothing else.
 */
#include "gba/types.h"

struct Box {
    u8  pad_00[8];
    u16 kind;     /* 0x08 */
};

struct Entry {
    u8  pad_00[0x20];
    u16 f20;      /* 0x20 */
};

extern u8 *iwram_3001e98;

extern struct Entry *Func_801b36c(u8 *base);
extern void *CreateUIBox(int a, int b, int c, int d, int e);
extern void CloseUIBox(void *box, int b);
extern void Func_8016478(void *box);
extern void Func_801e7c0(int id, void *box, int x, int y);

extern int _MSG_50;
extern int _MSG_51;

void Func_801b010(int a, u32 b)
{
    u8 *base;
    struct Entry *ent;
    void **box;

    base = iwram_3001e98;
    ent = Func_801b36c(base);
    box = (void **)(base + (0xd4 << 2));
    if (*box == 0) {
        if (a == 6) {
            if (*(u16 *)(base + (0xee << 2)) != 0)
                *box = CreateUIBox(0x11, 0x11, 5, 3, a);
            else
                *box = CreateUIBox(0x11, 0, 5, 3, a);
            {
                u16 *q = (u16 *)(base + (0xe8 << 2));
                unsigned v = 0;
                *q = v;
            }
            {
                u16 *q = (u16 *)(base + (0xee << 2));
                unsigned v = 0x3e7;
                *q = v;
            }
        } else {
            *box = CreateUIBox(((9 - b) >> 1) + 0x13, 0x11, b + 2, 3, 6);
        }
        Func_8016478(*(void **)(base + (0xd4 << 2)));
    } else {
        if (b != 0 && ((struct Box *)*box)->kind != b + 2) {
            CloseUIBox(*box, 2);
            *box = CreateUIBox(((9 - b) >> 1) + 0x13, 0x11, b + 2, 3, 6);
        }
        Func_8016478(*(void **)(base + (0xd4 << 2)));
    }
    if (*(u16 *)(base + (0xe5 << 2)) != 0) {
        Func_801e7c0(ent->f20, *(void **)(base + (0xd4 << 2)), 0, 0);
    } else {
        switch (a) {
        case 4:
            Func_801e7c0((int)&_MSG_51, *(void **)(base + (0xd4 << 2)), 0, 0);
            break;
        case 2:
            Func_801e7c0((int)&_MSG_50, *(void **)(base + (0xd4 << 2)), 0, 0);
            break;
        }
    }
}

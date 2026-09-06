/* OvlFunc_960_2008594  --  0x02008594
 *   [asm/overlays/rom_7eaf28/ovl_314_c_a_c_c_c_c_c_c_a.s, 1st of 1 -- NO SPLIT NEEDED]
 *
 * A 249-instruction map cutscene: a 16-frame fade loop that queues one DMA
 * blend-alpha write per frame, then a two-flag-guarded script of turns, walks
 * and dialogue.  Built at the tree default -O2 -- no Makefile rule, explicit or
 * pattern, names this stem (the only rom_7eaf28 explicit rule is for
 * ovl_314_c_a_c_a_c_c), so `asm/%.o: src/%.c` applies and objcmp prints no
 * `(built with: ...)` line from either path.  NO FLAG GROUP IS INVOLVED.
 *
 * NOT A PIN FUNCTION IN ITS FIRST HALF, AND THE PROLOGUE SAYS SO.  The template
 * this was started from (src/overlays/rom_7ec19c/ovl_30_c_c_a.c) reads its
 * `push {lr}` as "nothing is kept across calls, so only pins work".  This ROM's
 * prologue is the opposite reading: `push {r5, r6, r7, lr}` plus r8-r11 through
 * the low-register shuffle, EIGHT saved registers, so the ROM does keep seven
 * live values across calls -- the two actor pointers, the 0x340 flag, the two
 * ldrsh coordinates, the parameter, and the loop counter's `i + 1`.  Those are
 * ordinary C locals and they cost nothing but writing them down.  r4 is absent
 * from the push because of `-fcall-used-r4`, not because of anything in the
 * source; that is prologue-by-CONTENT working as recorded.
 *
 * Everything up to `__GetFlag(0x9a0)` -- 92 of 249 instructions, the whole loop
 * and the whole DMA push -- is EXACT from plain C with no pin at all.  The pin
 * work is confined to the guarded tail, where the ROM rebuilds every constant.
 * Read as one function it is neither a pin function nor a plain one; the two
 * halves were screened separately and treated differently.
 *
 * THE DMA PUSH IS A CORPUS IDIOM, NOT A PUZZLE.  solved_twins reports 0 twins,
 * but grepping asm/ for `gDMATaskCount` gives 25 .s files of which three have a
 * solved .c beside them, and src/overlays/rom_7795e8/ovl_30_c_c_a_a_a_b.c is the
 * same inline: the DmaTransfer/DmaQueue layout, `count * 12 + queue + 4`, the
 * count store BEFORE the source store, `SET_IO(REG_IME, REG_ADDR_IME)` for the
 * `strh r6, [r6]` that disables interrupts with the port's own address, and
 * `LOCK_IME` wrapped in `do { } while (0)`.  Copied over, it was exact first try.
 *
 * AND TWO OF THAT SIBLING'S THREE LEVERS INVERT HERE.  This is the point worth
 * carrying forward, because the shared inline invites copying the levers too:
 *
 *   - `do { } while (0)` on the IME save-and-disable: STILL LOAD-BEARING.
 *     Flattening the macro is 3 encodings with SIZE and RELOCATIONS silent --
 *     the gDMATaskCount pool load moves behind the REG_ADDR_IME one, the same
 *     residue the sibling recorded, on a different member.
 *
 *   - THE ARGUMENT HOIST IS WANTED HERE.  The corpus rule is stated as a
 *     prohibition ("DO NOT PASS A COMPUTED VALUE AS AN INLINE'S ARGUMENT",
 *     docs/elevation.md), because on the sibling it lifted the alpha build above
 *     the interrupt guard.  This ROM PUTS `add r5, r2, #1` ABOVE THE GUARD, so
 *     the computed value must be passed as the argument.  Building it inside the
 *     inline instead is 212 encodings, +4 bytes and every relocation moved.
 *     NEW, as a second specimen either side of the recorded rule: the hoist is a
 *     PLACEMENT LEVER WITH TWO DIRECTIONS, not a trap -- read where the ROM puts
 *     the computation and choose the side accordingly.
 *
 *   - THE `(u16)` WIDENING LEVER DOES NOT APPLY.  The sibling needed `int` plus
 *     explicit casts to keep an un-folded `lsl #0x10 / lsr #0x10`.  This ROM has
 *     no zero-extension there because the counter is an ordinary int induction
 *     variable; adding the casts is 213 encodings and +4 bytes.  A LIVE GIV IN
 *     THE ROM PROVES THE LOOP IS ORDINARY -- `for (i = 0; i <= 0xf; i++)` with
 *     `i + 1` passed to the inline reproduces the ROM's shared
 *     `add r5, r2, #1` in both arms with no help.
 *
 * TEN PINS IN THE TAIL, UNIFORM ASCENDING, AND FIVE OF THEM ARE SINGLETONS.
 * One statement per argument, ascending q0..q3, whole value per statement.
 * sched2 reproduces every transposed order the ROM emits from that one spelling:
 * `mov r1 / mov r2 / mov r0 / lsl r1 / lsl r2`, `mov r1 / mov r0 / lsl r1 /
 * mov r2`, `mov r2 / mov r0 / ldr r1`.  Transcribing the ROM's own emitted order
 * into four of the fills instead is 10 encodings; flipping all ten to descending
 * is 24.  The ascending form is CORRECT, not merely cheaper.
 *
 * THE FAMILY HEURISTIC EARNED FIVE OF THE TEN.  A repeated-constant rule
 * nominates only the four sites whose value occurs twice.  The other five --
 * __MapActor_Emote(0xd, 0x83 << 1, 0x3c), the 0x81 << 1 twin,
 * __Func_8092adc(0xd, 0x80 << 6, 0), __MapActor_Emote(0xd, 0x101, 0x3c) and
 * __Func_8092adc(0xd, 0xc0 << 8, 0x1e) -- all use a value that appears ONCE in
 * the function.  Every one of them is needed, and every one costs exactly 2
 * encodings when dropped: they are pure ORDERING pins, nominated only by
 * CALL-SITE FAMILY (same callee, all-constant argument shape).
 *
 * A TWO-SITE CSE CLASS BREAKS FROM EITHER END, AND BOTH OF THIS FUNCTION'S DO.
 * Twelve pins were written; the greedy removal sweep run to a fixpoint from BOTH
 * ends of the list agrees on the same two drops, so the set is 10:
 *   - 0x9b6 at __GetFlag and __SetFlag: pinning the GetFlag alone is enough
 *     (dropping it is 169 encodings, +4 bytes, relocations moved); the SetFlag
 *     pin is then inert.
 *   - 0x80 << 7 at __Func_809280c and __Func_8092adc: pinning __Func_809280c
 *     alone is enough (dropping it is 44 encodings, relocations moved); the
 *     __Func_8092adc pin is then inert.
 * Both times the FIRST use is the one that must be pinned and the LAST falls,
 * consistent with the recorded reading -- but the sweep was still run from both
 * ends, because that is the only way to know.
 *
 * THE ORR BITFIELD SITE, AND A REFINEMENT OF THE RECORDED RULE.  `p[0x5a] |= 1`
 * gives the loaded byte the destination; the ROM ties it to the CONSTANT
 * (`ldrb r2, [r0] / mov r3, #1 / orr r3, r2`).  The fix is
 * src/overlays/rom_7f148c/ovl_30_c_c_c_a_a_c_b.c's, reproduced exactly --
 * including its 6-encoding cost for the wrong declaration order.  Eight
 * spellings measured here separate the levers cleanly:
 *
 *   ptr declared first, uchar bit, `bit |= *r; *r = bit;`   EXACT   <- ships
 *   ptr declared first, uchar bit, `*r = bit | *r;`         EXACT
 *   ptr declared first, uchar bit, `*r = *r | bit;`         EXACT
 *   ptr declared first, INT   bit, `bit |= *r; *r = bit;`   EXACT
 *   ptr declared first, INT   bit, `*r = bit | *r;`         2
 *   bit declared first, uchar bit, `bit |= *r; *r = bit;`   6
 *   bit declared first, INT   bit, `bit |= *r; *r = bit;`   6
 *   no local at all, `*(... + 0x5a) |= 1;`                  2
 *
 * So there are TWO levers, not one: the pointer must be born before the byte
 * local (6 either way if not), and the constant must survive as a named local
 * (2 if not).  NEW: the recorded note that "an `int m` is folded and is
 * byte-identical to the plain literal" is true only of the EXPRESSION form.  In
 * the ACCUMULATOR form `bit |= *r; *r = bit;` the width is INERT -- `int` and
 * `unsigned char` are byte-identical -- because the accumulator statement makes
 * the local the destination whatever its mode.  The narrow local is a way of
 * reaching that, not the only one.  Grepped for QImode, declaration order and
 * "tying the destination" before writing this down.
 *
 * THE POINTER-RETURNING CALL MUST NOT BE NAMED at the two `&= 0xfe` sites:
 * `p = __MapActor_GetActor(0xd); p[0x5a] &= 0xfe;` costs a preserved base per
 * site, 131 encodings and +4 bytes.  The folded lvalue gives the ROM's
 * `add r0, #0x5a` in place.  `p` is named only at the two GetActor(0) sites
 * whose result genuinely crosses a `cmp` and a branch.  0xfe itself needs
 * nothing: gcc commons it into r5 across two calls exactly as the ROM does.
 *
 * THE gState BASE IS A LOCAL.  Written `*(int *)(gState + (0xfa << 1))` gcc
 * folds the whole address into one pooled `=gState+500` and the ROM's
 * `ldr r3, =gState / mov r1, #0xfa / lsl r1, #1 / add r3, r1` is lost: 231
 * encodings and FOUR BYTES SHORT.  The named-base idiom is the corpus's.
 *
 * READ THE RELOCATION LINE, NOT THE COUNT.  Every single-change measurement
 * above partitions with no middle: 2-3 encodings with SIZE and RELOCATIONS both
 * silent for an ordering loss, 44-231 with RELOCATIONS ALWAYS differing for a
 * CSE loss.  Nothing landed between 3 and 44.  But the two AGGREGATE
 * measurements -- all ten fills transposed (24) and all ten flipped to
 * descending (10) -- sit numerically inside the CSE band with RELOCATIONS
 * SILENT.  The partition is per-CHANGE; the relocation line, not the count, is
 * what survives aggregation.
 *
 * ALL FOUR POOLED VALUES ARE BARE LITERALS OR EXISTING SYMBOLS -- 0x9b6, 0x101,
 * 0x262e, and gState / gDMATaskCount / REG_ADDR_IME / REG_ADDR_BLDALPHA from
 * include/gba/io.h.  objcmp reports 47 relocations identical and the only
 * non-`bl` ones are R_ARM_ABS32 gState and R_ARM_ABS32 gDMATaskCount.  Nothing
 * belongs in const.sym or message.sym for this function.
 *
 * The `bne / b` and `beq / b` pairs are gcc's own long-branch expansion of the
 * two-flag guard; the guarded body is 354 bytes, past Thumb's conditional
 * range.  Writing the guard as `A && B` or as two nested `if`s is byte-identical.
 *
 * LANDING.  The .s holds exactly ONE function (one .thumb_func_start).  Exactly
 * ONE linker line names the .o, matched on full path and cited by content:
 *
 *   overlays/rom_7eaf28/overlay.ld:36
 *       asm/overlays/rom_7eaf28/ovl_314_c_a_c_c_c_c_c_c_a.o(.text)
 *
 * inside the `.text` output section, between ovl_314_c_a_c_c_c_c_c_b.o(.text)
 * and ovl_314_c_a_c_c_c_c_c_c_b.o(.text).  There is NO .data, .rodata, .data1 or
 * .bss line anywhere in that script naming this .o -- the `.data` section at
 * line 56 lists only the ovl_314_c_c_* objects -- and the reference .s carries
 * no section directive of its own.  The object keeps its existing asm/ path, so
 * the linker script needs NO edit at all.  Its five tiny callers
 * (ovl_314_c_a_c_c_c_c_c_c{_b,_c_b,_c_c_b,_c_c_c_b,_c_c_c_c_b}.c, passing 8..12)
 * are already elevated and are unaffected.
 *
 * Verified with tools/objcmp.py against the ORIGINAL asm/ path:
 *   OK OvlFunc_960_2008594 -- 616 bytes, 256 encodings and 47 relocations identical
 */
#include "gba/types.h"
#include "gba/io.h"

struct DmaTransfer {
    const void *src;
    void *dest;
    u32 control;
};

struct DmaQueue {
    u16 count;
    struct DmaTransfer tasks[32];
};

extern struct DmaQueue gDMATaskCount;
extern unsigned char gState[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);

#define LOCK_IME(saved)             \
do {                                \
    saved = REG_IME;                \
    SET_IO(REG_IME, REG_ADDR_IME);  \
} while (0)

static inline void SetBldAlpha(struct DmaQueue *queue, int t)
{
    u32 savedIme;
    s32 count;
    u32 *task;

    LOCK_IME(savedIme);
    count = queue->count;
    if (count < 32) {
        task = (u32 *)(count * 12 + (u32)queue + 4);
        *(u16 *)queue = count + 1;
        *task++ = ((0x10 - t) << 8) | t;
        *task++ = REG_ADDR_BLDALPHA;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, savedIme);
}

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_960_2008594(int slot)
{
    unsigned char *g;
    unsigned char *a;
    unsigned char *act;
    unsigned char *p;
    int flag;
    int x;
    int y;
    int i;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    act = __MapActor_GetActor(slot);
    flag = __GetFlag(0xd0 << 2);
    x = *(short *)(a + 0xa);
    y = *(short *)(a + 0x12);
    __CutsceneStart();
    __PlaySound(0xf4);
    for (i = 0; i <= 0xf; i++) {
        *(int *)(act + 0x18) = (i << 11) + (0x80 << 4);
        *(int *)(act + 0x1c) = (i << 12) + (0x80 << 5);
        if (flag == 0)
            SetBldAlpha(&gDMATaskCount, i + 1);
        __WaitFrames(1);
    }
    __SetFlag(slot + (0xff << 1));
    __SetFlag(0xd0 << 2);
    if (__GetFlag(0x9a << 4) != 0 && ({ PIN1; q0 = 0x9b6; __GetFlag(q0); }) == 0) {
        __SetFlag(0x9b6);
        { PIN3; q0 = 0xd; q1 = 0x80 << 9; q2 = 0x80 << 8;
          __MapActor_SetSpeed(q0, q1, q2); }
        { PIN3; q0 = 0; q1 = 0xc0 << 9; q2 = 0xc0 << 8;
          __MapActor_SetSpeed(q0, q1, q2); }
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_SetPos(0xd, *(int *)(p + 8), *(int *)(p + 0x10));
        { PIN3; q0 = 0xd; q1 = 0x80 << 7; q2 = 0;
          __Func_809280c(q0, q1, q2); }
        *(__MapActor_GetActor(0xd) + 0x5a) &= 0xfe;
        __Func_809218c(0xd, x, y - 0x10);
        *(__MapActor_GetActor(0) + 0x5a) &= 0xfe;
        __Func_80921c4(0, x + 8, y - 0x28);
        __CutsceneWait(1);
        { unsigned char *r = __MapActor_GetActor(0) + 0x5a;
          unsigned char bit = 1;
          bit |= *r; *r = bit; }
        __MapActor_SetAnim(0xd, 1);
        __Func_8092adc(0, 0x80 << 7, 0);
        __MessageID(0x262e);
        { PIN3; q0 = 0xd; q1 = 0x83 << 1; q2 = 0x3c;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0xd, 0);
        { PIN3; q0 = 0xd; q1 = 0x81 << 1; q2 = 0x3c;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0xd, 0);
        { PIN3; q0 = 0xd; q1 = 0x80 << 6; q2 = 0;
          __Func_8092adc(q0, q1, q2); }
        { PIN3; q0 = 0xd; q1 = 0x101; q2 = 0x3c;
          __MapActor_Emote(q0, q1, q2); }
        __ActorMessage(0xd, 0);
        __CutsceneWait(0xa);
        { PIN3; q0 = 0xd; q1 = 0xc0 << 8; q2 = 0x1e;
          __Func_8092adc(q0, q1, q2); }
        __Func_80925cc(0xd, 2);
        __ActorMessage(0xd, 0);
        __MapActor_DoAnim(0xd, 3);
        __ActorMessage(0xd, 0);
        { PIN3; q0 = 0xd; q1 = 0x80 << 9; q2 = 0x80 << 8;
          __MapActor_SetSpeed(q0, q1, q2); }
        __MapActor_SetAnim(0xd, 2);
        p = __MapActor_GetActor(0);
        if (p != 0)
            __MapActor_TravelTo(0xd, *(short *)(p + 0xa), *(short *)(p + 0x12));
        __MapActor_WaitMovement(0xd);
        __MapActor_SetPos(0xd, 0, 0);
    }
    __CutsceneEnd();
}

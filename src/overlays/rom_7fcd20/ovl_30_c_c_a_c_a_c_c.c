/* OvlFunc_974_2008cf4 -- asm/overlays/rom_7fcd20/ovl_30_c_c_a_c_a_c_c.s.
 *
 * The debug item-give box: two UI boxes, a palette fixup, then a loop that
 * redraws the item panel whenever a key moved the index, gives the item on A
 * and closes on B.
 *
 * FOUR LEVERS, and the first two are new.
 *
 * 1. DMA3_SET NEEDS "r2" IN ITS CLOBBER LIST HERE, which is why this file
 *    carries its own copy instead of including dma.h.  The two transfers are
 *    adjacent and their control words are 0x80000010 and 0x80000001, fifteen
 *    apart, so cse reaches the second with `sub r2, #15` off the first; the ROM
 *    loads both from the pool.  dma.h's DMA3_SET already clobbers "r0" and that
 *    is exactly why the ROM's second SOURCE is a fresh `ldr r0` rather than the
 *    `sub r0, #24` cse would otherwise find -- the clobber is what makes gcc
 *    forget the value.  Adding "r2" does the same for the control word, and r1
 *    is deliberately left OUT so the ROM's `add r1, #0x1c` survives.  So the
 *    clobber list of an inline-asm DMA macro is a PER-REGISTER cse switch, and
 *    the ROM tells you which way each one is set.
 *
 * 2. THE EXIT TAIL IS WRITTEN TWICE SO GCC CROSS-JUMPS IT, and that is what
 *    puts the loop's blocks in the ROM's order.  Written once behind a shared
 *    `cancel:` label, gcc makes the after-loop block the FALL-THROUGH of the
 *    `goto` and pushes the whole key-repeat chain past the epilogue -- 179
 *    differing, and five source arrangements (for/break, do/while, goto+label,
 *    if/else, inverted test) all do it.  Writing `__PlaySound(0x71); goto done;`
 *    in BOTH the GiveItem-failed path and the B-button path drops it to 178 and
 *    gives the ROM's layout exactly: gcc merges the two copies into one block,
 *    the merged block keeps a real `b` to the after-loop code, and the after-loop
 *    code stays last.  The ROM's `.Le0a` label with two predecessors is the
 *    tell that the original had the tail twice.
 *
 *    The `goto top` loop is load-bearing on top of that: the same file with
 *    `for (;;)` and `break` is 187 differing.
 *
 * 3. `_MSG_182` and `_MSG_75` have to be symbols, the usual reason: as plain
 *    literals 0x182 is built with `mov #193 / lsl #1` and 0x75 with
 *    `add r5, #117`, where the ROM loads both from the pool.  Both are ALREADY
 *    in message.sym -- no new entry is needed.
 *
 * 4. TWO LEVERS THAT LOOKED NECESSARY AND ARE NOT, both measured as single drops
 *    against this exact file: the initialiser-versus-assignment spelling of
 *    `redraw` (the rom_7fc720 lever) is INERT here, and so is passing `redraw`
 *    rather than a literal `0` as __Func_801e9d4's fifth argument.  Both were in
 *    the candidate that first reached the ROM's r8/r10 assignment, and neither is
 *    what fixed it -- the `_MSG_*` symbols and the DMA clobber did, between them.
 *    Worth recording because "it went right when I added X" is not evidence that
 *    X did it; only the drop is.
 *
 * The two register pins are the repeated-constant cure and a scheduler nudge:
 * `%` wants 0x10e materialised TWICE (the ROM does not common the addend with
 * the divisor), and the r1/r2/r3 pin on the second __UIDrawText pushes
 * `and r5, r6` past the argument fill where the ROM has it.
 *
 * VERIFIES 221 instructions against 221, 532 bytes against 532, and the
 * INSTRUCTION STREAM IS IDENTICAL.  objcmp reports "2 place(s)" and
 * "RELOCATIONS differ", and both are its own documented false-negative shapes:
 *
 *   * the two differing encodings are the POOL WORDS at 0x200 and 0x204 -- ref
 *     0x00000182 and 0x00000075, ours 0x00000000 carrying R_ARM_ABS32 to
 *     `_MSG_182` and `_MSG_75`.  Same offsets, and message.sym:220-221 already
 *     define both to exactly those values, so the linked bytes are equal.  NO NEW
 *     .sym ENTRY IS NEEDED.  The extra relocations are also the only reason the
 *     relocation LISTS differ in length.
 *   * `_modsi3_RAM` against `__modsi3` is this overlay's own alias
 *     (overlays/rom_7fcd20/overlay.ld:63).  Checked directly rather than trusted:
 *     `arm-none-eabi-nm overlays/rom_7fcd20/overlay.elf` puts both names at
 *     02009188.
 *
 * `make compare` is the authority for this shape and is the gate.
 */
#include "gba/types.h"
#include "gba/io.h"

static inline void DMA3_SET_R2CLOB(const void *src, void *dst, unsigned cnt)
{
    register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
    register const void *_src __asm__("r0") = src;
    register void *_dst __asm__("r1") = dst;
    register unsigned _cnt __asm__("r2") = cnt;
    __asm__ volatile (
        "stmia\tr3!, {r0, r1, r2}\n\t"
        "sub\tr3, #0xc"
        :
        : "r" (_base), "r" (_src), "r" (_dst), "r" (_cnt)
        : "memory", "r0", "r2"
    );
}

extern int _MSG_182;
extern int _MSG_75;
extern unsigned char gOvl_02009390[];
extern unsigned char gScript_944__0200939c[];
extern unsigned char gScript_960__020093b4[];
extern volatile int gKeyPress;
extern volatile int gKeyRepeat;

extern void __PlaySound(int id);
extern void *__CreateUIBox(int a, int b, int c, int d, int e);
extern void __WaitFrames(int n);
extern void __Func_8016498(void *box);
extern void __Func_80164ac(void *box);
extern void __UIDrawText(unsigned char *text, void *box, int x, int y);
extern void __Func_801e9d4(int a, int b, void *box, int d, int e);
extern int __Func_8078500(void);
extern void __GetItemInfo(int id);
extern void __Func_801e7c0(int a, void *box, int c, int d);
extern void __DrawSmallText(int id, void *box, int x, int y);
extern void __Func_80a4924(void *box, int b);
extern int __GiveItem(int id);
extern int __CloseUIBox(void *box, int b);

void OvlFunc_974_2008cf4(void)
{
    void *box;
    void *box2;
    int item = 1;
    int redraw = 1;

    __PlaySound(0x70);
    box = __CreateUIBox(0, 0, 0x1e, 7, 2);
    box2 = __CreateUIBox(0, 8, 0xd, 0xa, 2);
    DMA3_SET_R2CLOB((void *)0x5000200, (void *)0x50001c0, 0x80000010);
    DMA3_SET_R2CLOB((void *)0x50001e8, (void *)0x50001dc, 0x80000001);
    __WaitFrames(1);
top:
    {
        if (redraw) {
            {
                register int q0 __asm__("r0");
                register int q1 __asm__("r1");
                redraw = 0;
                q1 = 0x87;
                q0 = item + (0x87 << 1);
                q1 <<= 1;
                item = q0 % q1;
            }
            __Func_8016498(box);
            __Func_80164ac(box);
            __UIDrawText(gOvl_02009390, box, 0, 0);
            __Func_801e9d4(item, 0, box, 0x50, 0);
            if (__Func_8078500()) {
                int id;
                {
                    register int q1 __asm__("r1");
                    register int q2 __asm__("r2");
                    register int q3 __asm__("r3");
                    q1 = (int)box;
                    q2 = 0;
                    q3 = 0x20;
                    id = item & 0x1ff;
                    __UIDrawText(gScript_944__0200939c, (void *)q1, q2, q3);
                }
                __GetItemInfo(id);
                __Func_801e7c0(id + (int)&_MSG_182, box, 0x78, 0);
                __DrawSmallText(id + (int)&_MSG_75, box, 0, 0x10);
                __Func_8016498(box2);
                __Func_80a4924(box2, item);
            } else {
                __UIDrawText(gScript_960__020093b4, box, 0, 0x20);
            }
        }
        if (gKeyPress & 1) {
            if (__GiveItem(item) == -1) {
                __PlaySound(0x71);
                goto done;
            }
            __PlaySound(0xaf);
        }
        if (gKeyPress & 2) {
            __PlaySound(0x71);
            goto done;
        }
        if (gKeyRepeat & 0x40) { item--; redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x80) { item++; redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x10) { item += 0xa; redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x20) { item -= 0xa; redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x100) { item += 0x1e; redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x200) { item -= 0x1e; redraw = 1; __PlaySound(0x6f); }
        __WaitFrames(1);
    }
    goto top;
done:
    __Func_8016498(box);
    __WaitFrames(1);
    __CloseUIBox(box, 1);
    __CloseUIBox(box2, 1);
}

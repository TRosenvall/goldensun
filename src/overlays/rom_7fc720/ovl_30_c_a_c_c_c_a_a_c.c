/* Cluster OvlFunc_973_2008214..OvlFunc_973_20084b0 extracted from
 * goldensun/asm/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_a_c.s.
 *
 * THIS .s CONVERTS WHOLE: it holds exactly these two functions and no data,
 * so ovl_30_c_a_c_c_c_a_a_c.c is the whole TU and no split is needed.
 *
 * SanctumMenu / SanctumService.  Both are near-twins of the ALREADY LANDED
 * src/overlays/rom_7fcd20/ovl_30_c_c_a_c_a_c_c.c (OvlFunc_974_2008cf4), and that
 * file is where four of the five levers below come from.  What is new here is the
 * `signed char dir` scan variable, the two wrap-search loops it drives, and the
 * _MSG_333/_MSG_53a PAIR.
 *
 * FAKEMATCH -- the two `%` sites and one __UIDrawText argument fill are reached by
 * pinning registers with inline asm, exactly as the rom_7fcd20 twin does.
 *
 * LOAD-BEARING, each measured as a SINGLE DROP against the final file
 * (2008214 / 20084b0 differing encodings; 0 = the two pool placeholders only):
 *
 *   the `q0`/`q1` pin on the redraw `%`          238 / 214
 *   the `q0`/`q1` pins on the four wrap-loop `%` 207 / 201
 *   `"r0"` AND `"r2"` in the DMA3 clobber list    24 /  23
 *     (dma.h's own DMA3_SET, which clobbers only "r0", is 21 / 20 -- so BOTH
 *      clobbers are needed and neither alone is enough)
 *   `signed char dir` rather than `int dir`      172 / 151
 *   the exit tail written TWICE (2008214 only)   155
 *   _MSG_182 / _MSG_75 symbols (2008214 only)     45
 *   `void *box2;` LEFT UNINITIALISED (20084b0)   245
 *   _MSG_333 and _MSG_53a TOGETHER (20084b0)       4 with both as literals,
 *     but 26 with only _MSG_53a and 24 with only _MSG_333 -- two changes that are
 *     each WORSE than the pair of literals and decisive together.
 *
 * MEASURED INERT, so not claimed: `for (;;)` in place of `goto top` (2 / 2, i.e.
 * no change) -- the twin's note says the `goto top` loop is load-bearing there,
 * and on BOTH of these it is not.  Also inert on 20084b0: writing the second
 * message base as `id + (int)&_MSG_53a` twice rather than `id += ...` once.
 *
 * THE `dir` SCAN IS THE NEW SHAPE.  `mov r2,#0xff / mov r8,r2` is a QImode -1, and
 * the two compares are the tell that the variable is a signed char and not an int:
 * `== -1` needs the full `lsl #24 / asr #24 / cmp` because 0xff000000 is not an
 * immediate, while `== 1` reuses the SAME shifted value and compares it against
 * `0x80 << 17` = 1 << 24.  An `int dir` gives neither and costs 172 / 151.
 *
 * AND THE WRAP LOOPS ARE A CROSS-JUMP.  `.L3f2` / `.L66c` have two predecessors
 * and sit above the shared `lsl r1,#1 / bl mod / mov r6,r0 / and / GetInfo / cmp`
 * tail; the source is an ordinary `while` after a normalising `%`, and gcc merges
 * the two `%` tails itself.  WITHOUT the pins gcc commons 0x10e into one register
 * and uses it as BOTH addend and divisor, which breaks the merge and emits five
 * `bl __modsi3` where the ROM has three (+4 bytes) -- the pins are what make the
 * constant appear twice, which is what leaves an identical suffix to merge.
 *
 * VERIFIES, both functions, INSTRUCTION STREAM IDENTICAL:
 *   OvlFunc_973_2008214  283 encodings against 283, 668 bytes against 668
 *   OvlFunc_973_20084b0  249 encodings against 249, 584 bytes against 584
 * objcmp reports "2 place(s)" on each, and both are its documented
 * false-negative shapes:
 *   * the differing encodings are POOL WORDS -- ref 0x00000182 / 0x00000075 at
 *     0x280 / 0x284 in 2008214 and 0x00000333 / 0x0000053a at 0x230 / 0x234 in
 *     20084b0, ours 0x00000000 carrying R_ARM_ABS32 at the SAME offsets.  Every
 *     other relocation offset and symbol is identical and the sizes are equal.
 *     message.sym:220-221 already define _MSG_75 = 0x0075 and _MSG_182 = 0x0182
 *     and :256 defines _MSG_53a = 0x053a.  **_MSG_333 = 0x0333 IS A NEW ENTRY AND
 *     MUST BE ADDED** -- see the joint measurement above for its justification,
 *     which is the same register argument _MSG_53a itself rests on.
 *   * `_modsi3_RAM` against `__modsi3` is this overlay's own alias, declared at
 *     overlays/rom_7fc720/overlay.ld:52; objcmp prints its own `~~` note for it.
 *
 * `make compare` is the gate for this shape.
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
extern unsigned char L8d4[] __asm__(".L8d4");
extern unsigned char L8e0[] __asm__(".L8e0");
extern unsigned char L8f8[] __asm__(".L8f8");
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
extern unsigned short *__GetItemInfo(int id);
extern void __Func_801e7c0(int a, void *box, int c, int d);
extern void __Func_80a4924(void *box, int b);
extern int __GiveItem(int id);
extern int __CloseUIBox(void *box, int b);

extern int _MSG_333;
extern int _MSG_53a;
extern unsigned char L90c[] __asm__(".L90c");
extern unsigned char L914[] __asm__(".L914");
extern unsigned char *__GetMoveInfo(int id);
extern void __DrawSmallText(int a, void *box, int c, int d);

void OvlFunc_973_2008214(void)
{
    void *box;
    void *box2;
    int item = 1;
    int redraw = 1;
    signed char dir = 0;

    __PlaySound(0x70);
    box = __CreateUIBox(0, 0, 0x1e, 7, 2);
    box2 = __CreateUIBox(0, 8, 0x1c, 0xa, 2);
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
            __UIDrawText(L8d4, box, 0, 0);
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
                    __UIDrawText(L8e0, (void *)q1, q2, q3);
                }
                __GetItemInfo(id);
                __Func_801e7c0(id + (int)&_MSG_182, box, 0x78, 0);
                __Func_801e7c0(id + (int)&_MSG_75, box, 0, 0x10);
                __Func_8016498(box2);
                __Func_80a4924(box2, item);
            } else {
                __UIDrawText(L8f8, box, 0, 0x20);
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
        if (gKeyRepeat & 0x40)  { dir = -1; item -= 1;    redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x80)  { dir =  1; item += 1;    redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x10)  { dir =  1; item += 0xa;  redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x20)  { dir = -1; item -= 0xa;  redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x100) { dir =  1; item += 0x1e; redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x200) { dir = -1; item -= 0x1e; redraw = 1; __PlaySound(0x6f); }
        if (dir == -1) {
            { register int q0 __asm__("r0"); register int q1 __asm__("r1");
                  q1 = 0x87; q0 = item + (0x87 << 1); q1 <<= 1; item = q0 % q1; }
            while (__GetItemInfo(item & 0x1ff)[3] == 0)
                { register int q0 __asm__("r0"); register int q1 __asm__("r1");
                  q1 = 0x87; q0 = item + 0x10d; q1 <<= 1; item = q0 % q1; }
        }
        if (dir == 1) {
            { register int q0 __asm__("r0"); register int q1 __asm__("r1");
                  q1 = 0x87; q0 = item + (0x87 << 1); q1 <<= 1; item = q0 % q1; }
            while (__GetItemInfo(item & 0x1ff)[3] == 0)
                { register int q0 __asm__("r0"); register int q1 __asm__("r1");
                  q1 = 0x87; q0 = item + 0x10f; q1 <<= 1; item = q0 % q1; }
        }
        dir = 0;
        __WaitFrames(1);
    }
    goto top;
done:
    __Func_8016498(box);
    __WaitFrames(1);
    __CloseUIBox(box, 1);
    __CloseUIBox(box2, 1);
}

void OvlFunc_973_20084b0(void)
{
    void *box;
    void *box2;
    int item = 1;
    int redraw = 1;
    signed char dir = 0;

    __PlaySound(0x70);
    box = __CreateUIBox(0, 0, 0x1e, 0xc, 2);
    DMA3_SET_R2CLOB((void *)0x5000200, (void *)0x50001c0, 0x80000010);
    DMA3_SET_R2CLOB((void *)0x50001e8, (void *)0x50001dc, 0x80000001);
    __WaitFrames(1);
top:
    {
        if (redraw) {
            int id;
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
            __UIDrawText(L90c, box, 0, 0);
            __Func_801e9d4(item, 0, box, 0x50, 0);
            __UIDrawText(L914, box, 0, 0x48);
            id = item & 0x3fff;
            __Func_801e7c0(id + (int)&_MSG_333, box, 0x78, 0);
            id += (int)&_MSG_53a;
            __Func_801e7c0(id, box, 0, 0x18);
            __DrawSmallText(id, box, 0, 0x30);
        }
        if (gKeyPress & 2) {
            __PlaySound(0x71);
            __Func_8016498(box);
            __WaitFrames(1);
            __CloseUIBox(box, 1);
            __CloseUIBox(box2, 1);
            return;
        }
        if (gKeyRepeat & 0x40)  { dir = -1; item -= 1;    redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x80)  { dir =  1; item += 1;    redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x10)  { dir =  1; item += 0xa;  redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x20)  { dir = -1; item -= 0xa;  redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x100) { dir =  1; item += 0x1e; redraw = 1; __PlaySound(0x6f); }
        if (gKeyRepeat & 0x200) { dir = -1; item -= 0x1e; redraw = 1; __PlaySound(0x6f); }
        if (dir == -1) {
            { register int q0 __asm__("r0"); register int q1 __asm__("r1");
              q1 = 0x87; q0 = item + (0x87 << 1); q1 <<= 1; item = q0 % q1; }
            while (__GetMoveInfo(item & 0x3fff)[4] == 0)
                { register int q0 __asm__("r0"); register int q1 __asm__("r1");
                  q1 = 0x87; q0 = item + 0x10d; q1 <<= 1; item = q0 % q1; }
        }
        if (dir == 1) {
            { register int q0 __asm__("r0"); register int q1 __asm__("r1");
              q1 = 0x87; q0 = item + (0x87 << 1); q1 <<= 1; item = q0 % q1; }
            while (__GetMoveInfo(item & 0x3fff)[4] == 0)
                { register int q0 __asm__("r0"); register int q1 __asm__("r1");
                  q1 = 0x87; q0 = item + 0x10f; q1 <<= 1; item = q0 % q1; }
        }
        dir = 0;
        __WaitFrames(1);
    }
    goto top;
}

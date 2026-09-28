/* StartGS1Credits -- 0x080f03f0 -- asm/rom_f0000/rom_f0254_c_a.s (1 function; CONVERTS WHOLE).
 *
 * EXACT.  objcmp --whole: "OK whole file -- 328 bytes, 135 encodings and 19 relocations
 * identical".  objcmp --func StartGS1Credits: same numbers.
 *
 * THREE LOAD-BEARING CONSTRUCTS, each measured by removing it:
 *
 * 1. SET_IO ON EVERY HALFWORD REGISTER STORE, including the two inside the loop.  Without it
 *    gcc pools each store value as a HImode constant (`ldrh r3, .L17` against a `.word 64`)
 *    where the ROM builds it -- `mov r3, #0x40`, `mov r3, #0xe2 / lsl #5` -- and 0x10 gets
 *    hoisted into fp so `(0x10 - 1) << 8` no longer folds to the ROM's `mov r5, #0xf0 / lsl #4`.
 *    Drop ladder: no SET_IO anywhere 121 of 135 at 364 bytes against 328; SET_IO only on
 *    BLDALPHA 117 at 352; SET_IO everywhere BUT BLDALPHA 100 at 352; all nine 0 at 328.
 *    This is the `int` carrier rule from docs/elevation.md applied to a whole function:
 *    SET_IO's `unsigned __value` is exactly the int route.
 *
 * 2. `i & 1` WRITTEN INLINE AT BOTH SITES -- NO `page` LOCAL.  The local is the natural
 *    spelling and it is WRONG here: it gives one pseudo, and the ROM has two, so the ROM
 *    carries `mov r7, r5` after the call that we did not.  With the local: 60 of 135, SIZE
 *    IDENTICAL, one instruction short.  Without it: 0.  A `u32 page` local, a block-scoped
 *    one and an extra `next = page ^ 1` local are all 60, 60 and 69.
 *    THIS IS THE CONVERSE OF "A DECLARED LOCAL CAN COST THE REGISTER THE ROM GIVES IT":
 *    naming the common subexpression COALESCED a copy the ROM keeps.
 *
 * 3. `u32 i` / `int j`.  `cmp r2, #0x20 / bls` is unsigned and `cmp r6, #0x10 / ble` is
 *    signed; one differing encoding each if either is flipped.
 *
 * Also load-bearing: the `int` return with `return 0;` (`pop {r1}`, 9 differing as void) and
 * the arm order in the crossfade (20 differing with the arms swapped).
 *
 * Both loops are plain `for`s with no pre-test in the ROM: i starts 0 against <= 0x20 and
 * j starts 1 against <= 0x10, both provably true, so jump.c deletes the entry test itself.
 */
#include "gba/types.h"
#include "gba/io.h"

extern u8 iwram_3001d18;
extern u8 iwram_3001f58;
extern u8 iwram_3001ac4;
extern u8 iwram_3001d08;
extern int Lf0a5c[] __asm__(".Lf0a5c");

extern void ClearTasks(void);
extern int StartTask(void *fn, u32 pri);
extern void Func_80f03c0(void);
extern void Func_80f037c(void *dst);
extern void Func_80f0254(int page);
extern void Func_80f0678(void);
extern void WaitFrames(int n);
extern void LoadGS1CreditsBG(int id, int page);
extern void Func_800479c(void);
extern void ClearVRAM(void);

int StartGS1Credits(void)
{
    u32 i;
    int j;

    iwram_3001d18 = 0;
    iwram_3001f58 = 0;
    iwram_3001ac4 = 0;
    iwram_3001d08 = 0;
    ClearTasks();
    StartTask(Func_80f03c0, 0x480);
    SET_IO(REG_DISPCNT, 0x40);
    Func_80f037c((void *)0x6007800);
    Func_80f037c((void *)0x600f800);
    Func_80f0254(0);
    Func_80f0254(1);
    SET_IO(REG_BG2CNT, 0x1f8a);
    SET_IO(REG_BG3CNT, 0xf83);
    SET_IO(REG_DISPCNT, 0x1c40);
    SET_IO(REG_BLDCNT, 0x2844);
    Func_80f0678();
    WaitFrames(300);
    for (i = 0; i <= 0x20; i++) {
        LoadGS1CreditsBG(Lf0a5c[i], (i & 1) ^ 1);
        for (j = 1; j <= 0x10; j++) {
            if (i & 1)
                SET_IO(REG_BLDALPHA, (j << 8) | (0x10 - j));
            else
                SET_IO(REG_BLDALPHA, ((0x10 - j) << 8) | j);
            WaitFrames(4);
        }
        WaitFrames(0x10b);
    }
    SET_IO(REG_BLDCNT, 0);
    SET_IO(REG_DISPCNT, 0x1040);
    Func_800479c();
    ClearVRAM();
    iwram_3001d18 = 1;
    return 0;
}

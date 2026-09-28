/* Func_80f731c (DrawPrizeText) -- 0x080f731c -- asm/rom_f6000/rom_f6008_c_a_e.s
 * (4 functions in that .s -- Func_80f62b8, Func_80f6440, Func_80f731c, LuckyWheelsMain --
 * so this is NOT a whole-file shot; Func_80f6440 is 1892 lines and LuckyWheelsMain 1073.)
 *
 * EXACT.  objcmp --func Func_80f731c: "OK Func_80f731c -- 324 bytes, 145 encodings and 8
 * relocations identical".
 *
 * FOUR LOAD-BEARING CONSTRUCTS, each measured by removing it:
 *
 * 1. THE MEMBER-ARRAY SPELLING `h->tab[0x6e - i]` ON A STRUCT WHOSE BASE IS `base`, NOT A
 *    CAST POINTER AT base+0x4d8.  This is the single biggest lever in the function: 67 of 145
 *    at 320 bytes against 324 with `((u16 *)(base + 0x4d8))[0x6e - i]`, 10 of 145 at 324 with
 *    the struct.  The member form keeps BASE AND OFFSET IN SEPARATE REGISTERS and gives the
 *    ROM's `mov r1, r8 / strh r3, [r1, r2]`; the cast form folds base into the index
 *    (`add r2, r8 / add r2, r3 / strh r3, [r2]`) and, one instruction further out, loses the
 *    separate `mov r1, #0` the ROM needs for _CreateUIBox's second argument because the shared
 *    zero then lands in r1 instead of r2.  ONE SPELLING FIXED BOTH.
 *    (docs/elevation.md's "A member array keeps base and offset in separate registers".)
 *
 * 2. `q[i].fN = ...` WITH NO `q++`.  Walking the pointer leaves two scheduling ties wrong --
 *    `add r2, sl` before the loop's two zero constants instead of after, and the f4 giv's
 *    increment after `add r2, #0x1c` instead of before -- 5 of 145.  Indexing is 0.
 *    `q = (struct Slot *)(vram + 0x7080) + i` inside the loop is also 0.  Two spellings that
 *    keep `q++` but move the stores (`q[-1].f4` after the bump) are 118 and 124 at the wrong
 *    length, so this is not a free choice.
 *
 * 3. `x += Lf8736[i];` BETWEEN THE f0 AND f4 STORES.  With it after the f4 store (the reading
 *    the ROM's schedule suggests, since the `ldrb` lands between the two stores either way)
 *    the two loop-setup pool words come out in the other order -- .Lf8736 before 0xffe00000 --
 *    and the table pointer and the giv swap r0 for r1: 6 of 145.  loop.c's record_giv
 *    PREPENDS, so the setup is emitted in REVERSE body order and the later statement's
 *    constant is pooled first.  READ THE POOL ORDER TO RECOVER THE STATEMENT ORDER.
 *
 * 4. `g = &iwram_3001f04; base = g[0]; vram = g[-6];`  The reference has NO iwram_3001eec
 *    relocation: 0x3001f04 - 0x18 is reached with `sub r3, #0x18` off the one pool word.  Two
 *    plain externs give a second pool word and 83 of 146 at 328 bytes against 324.  The
 *    negative index is what the ROM's relocation list says the source did.
 *
 * Also load-bearing: `int msg = 0x905` with `msg - 1` for the second string id (the ROM keeps
 * it in callee-saved r5 across the call and does `sub r5, #1`; two literals are 29 differing
 * at 144 encodings against 145), `i != N` loop bounds (`i < N` is 134), the UNSIGNED shift
 * `(u32)(cos(...) * 3) >> 15` (`lsr`, not `asr` -- 2 differing), and the 0x94/0x8c/0x90 store
 * order (2 differing if written ascending).
 *
 * INERT, measured: SET_IO on the BLDCNT store.  `REG_BLDCNT = 0;` and `SET_IO(REG_BLDCNT, 0)`
 * are both exact, because the shared int zero already reaches the strh either way.
 */
#include "gba/types.h"
#include "gba/io.h"

extern char *iwram_3001f04;
extern u8 Lf8736[] __asm__(".Lf8736");
extern int cos(int a);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);
extern void _Func_801e7c0(int id, void *win, int x, int y);

struct St {
    u8 pad[0x4d8];
    u16 tab[0xa0];
};

struct Slot {
    u32 f0;
    u32 f4;
    u32 f8;
    u32 fc;
    u32 f10;
    u32 f14;
    u32 f18;
};

extern struct Slot ewram_2010018[];

void Func_80f731c(void)
{
    char **g;
    char *base;
    char *vram;
    struct Slot *q;
    struct St *h;
    int i;
    int x;
    void *box;
    int msg;

    g = &iwram_3001f04;
    base = g[0];
    vram = g[-6];
    x = 0;
    for (i = 0; i != 0x800; i++)
        ewram_2010018[i].f0 = 0;
    q = (struct Slot *)(vram + 0x7080);
    for (i = 0; i != 8; i++) {
        q[i].f0 = (x + 0x18) << 16;
        x += Lf8736[i];
        q[i].f4 = 0xffe00000 - i * 0x80000;
        q[i].f10 = 0;
        q[i].f18 = 0;
    }
    h = (struct St *)base;
    for (i = 0; i != 0xa0; i++)
        h->tab[i] = 0;
    for (i = 0; i != 0x28; i++) {
        h->tab[0x17 + i] = (u32)(cos(i * 0x199) * 3) >> 15;
        h->tab[0x6e - i] = (u32)(cos(i * 0x199) * 3) >> 15;
    }
    *(u32 *)(base + 0x94) = 0;
    *(u32 *)(base + 0x8c) = 0;
    *(u32 *)(base + 0x90) = 0;
    *(u32 *)(vram + 0x7780) = 1;
    *(u32 *)(vram + 0x7784) = 0;
    REG_BLDCNT = 0;
    box = _CreateUIBox(0x12, 0, 0xc, 4, 6);
    msg = 0x905;
    *(void **)(base + 0x4cc) = box;
    _Func_801e7c0(msg, box, 0, 8);
    _Func_801e7c0(msg - 1, *(void **)(base + 0x4cc), 0, 0);
}

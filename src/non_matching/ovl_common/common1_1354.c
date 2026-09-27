/* OvlFunc_common1_1354 -- asm/overlays/common/common1_a_a_c_a.s (this function
 * alone, no data). Fresh in batch 288, written from scratch.
 *
 * 16 encodings of 140 differ (objcmp --func; sizes equal, 140/140).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_common/common1_1354.c \
 *       asm/overlays/common/common1_a_a_c_a.s --func OvlFunc_common1_1354
 *
 * A sprite task: if a move is armed (.L32 = frame count) it lerps .L45/.L29
 * from .L38/.L42 toward .L21/.L48 by .L17/.L32 frames, then blinks one OAM
 * entry at .L41 for 14 of every 20 frames. `__divsi3` vs `_divsi3_RAM` is the
 * linker alias, already in all three overlay.ld files that link common.
 *
 * THE RESIDUE, two islands:
 *  1. The FIRST lerp only (the second is exact). ROM: `ldr r0,=L45 / ldr r1,=L38
 *     / ldr r3,=L21`, L21 read signed into r2, L38 signed into r3 with r0 as the
 *     ldrsh zero both times, and the plain `ldrh r6,[r1]` of L38 LAST. Ours
 *     reads L38 via r2 and does the ldrh first -- UNVERIFIED guess: the L38 address pseudo is
 *     shorter-lived in ours, so local-alloc gives it r2, not r1. Most of the 16.
 *  2. The OAM word: the ROM builds 0x40000000 in r4 (`mov r4,#0x80 / lsl r4,#23`)
 *     BEFORE the L19 ldrsh and ORs it in before L19<<28; ours ORs L19 first and
 *     builds the constant in r3 after. The rest.
 *
 * WHAT MOVED IT (56 -> 16):
 *  - q = .L41 as a local, assigned FIRST (the ROM keeps it in r11 and uses it
 *    both as the stmia base and as the call argument).
 *  - x and y (the raw .L45/.L29 reads) as locals read before `*p++ = 0`; the
 *    ROM loads both before the first stmia.
 *  - `(y - 8) | ((x - 8) << 16) | ...` -- y FIRST (51 -> 25 with the rest).
 *  - `(L21 - L38) * t` operand order (t in r0 for the mul).
 *
 * INERT: t as `short` / `++*(short *)L17` / `(*(short *)L17)++; t = ...` /
 * `+= 1`; x,y as short; every order of the four OR terms other than y-first;
 * an explicit int local for the difference (29, worse); the lerp as a static
 * inline function (56, worse); `(L21-L38)*t/n + L38` addend-last (19/21);
 * `-L38 + L21` (16, same); a `do {} while (0)` after lerp 1 (24, worse);
 * u16 vs short on the lerp store (same); q assigned after n or before the
 * if (21/25). A plain `int *p` gives the same stmia as `volatile int *`.
 */
struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

extern unsigned char L14[] __asm__(".L14");
extern unsigned char L17[] __asm__(".L17");
extern unsigned char L19[] __asm__(".L19");
extern unsigned char L21[] __asm__(".L21");
extern unsigned char L26[] __asm__(".L26");
extern unsigned char L29[] __asm__(".L29");
extern unsigned char L32[] __asm__(".L32");
extern unsigned char L38[] __asm__(".L38");
extern unsigned char L41[] __asm__(".L41");
extern unsigned char L42[] __asm__(".L42");
extern unsigned char L45[] __asm__(".L45");
extern unsigned char L48[] __asm__(".L48");
extern struct SpriteSlot gSpriteSlots[];
extern void __Func_8003dec(void *p, int n);

void OvlFunc_common1_1354(void)
{
    int tile;
    int n;
    int t;
    int c;
    volatile int *p;
    int x;
    int y;
    unsigned char *q;

    q = L41;
    tile = gSpriteSlots[*(short *)L14].vramOffset >> 5;
    n = *(short *)L32;
    if (n != 0) {
        t = (short)++*(unsigned short *)L17;
        *(short *)L45 = *(short *)L38
            + (*(short *)L21 - *(short *)L38) * t / n;
        *(short *)L29 = *(short *)L42
            + (*(short *)L48 - *(short *)L42) * t / n;
        if (t >= n)
            *(unsigned short *)L32 = 0;
        *(unsigned short *)L26 = 0;
    }
    c = (short)++*(unsigned short *)L26;
    if (c <= 13) {
        x = *(short *)L45;
        y = *(short *)L29;
        p = (volatile int *)q;
        *p++ = 0;
        *p++ = (y - 8) | ((x - 8) << 16) | 0x40000000 | (*(short *)L19 << 28);
        *p = tile | 0x400;
        __Func_8003dec(q, 0xff);
    } else if (c > 19) {
        *(unsigned short *)L26 = 0;
    }
}

/* Func_8010230 / UpdateBgScrollRegisters (0x08010230) -- NON-MATCHING:
 * 198 encodings of 239 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/8010230.c asm/rom_9000/rom_f9cc_c.s \
 *       --func Func_8010230
 *
 * 198 IS NOT A DISTANCE: size 500 against 488, instruction count 239 against 233.
 * The register-blind aligned measure (scratch_elev/b291/D/dis5.sh, a hand-rolled
 * shim) is 166 of the ROM's 236 disassembled instructions matching.
 *
 * SAME BLOCKER AS ITS FILE-MATE UpdateFieldScreen, one level worse: the ROM keeps
 * the configuration-block pointer in lr and the inner counter in ip, spills the
 * map-state pointer, the layer index and the attempt limit, and takes a 0x20 frame
 * where ours takes 0x1c.  OURS IS SIX INSTRUCTIONS SHORTER, which is the signature
 * of the REG_ALLOC_ORDER class rather than of a missing source shape: the ROM is
 * paying for its own register choices.
 *
 * THE READING IS BELIEVED RIGHT AND IT IS WORTH MORE THAN THE NUMBER.  Its two
 * inner loops are UpdateScreenEdge_V's blit INLINED and doubled -- the same
 * `(gBuffer[t] << 20) >> 17` tile scale, the same pair of words out of
 * ewram_2020000 / ewram_2020004 stored 0x40 apart in the 0x6002800 map page, the
 * same `& 0x7f` / `& 0x1e` wraps held in named locals.  So
 * src/non_matching/rom_9000/800fec8.c's two landed levers (one named destination
 * pointer; the two masks as named locals) apply here unchanged, and the outer
 * walk's parallax block is character for character UpdateFieldScreen's.
 *
 * WHAT IS RIGHT:
 *   - The layer gate is `st[i + 0x100]`, and the index must be a NAMED LOCAL:
 *     `o = i + 0x100; if (st[o])` gets the ROM's register+register `ldrb r3,[r5,r3]`
 *     where `st[i + 0x100]` written inline folds to `(st + i) + 0x100` and a
 *     plain `ldrb rd,[rn,#0]`.  This is the recorded `p->a[i + 0x32]` lever, for a
 *     bare pointer rather than a member array.
 *   - `n = 0x16` must be set BEFORE the two `fx32_multiply` calls, not after.  It
 *     is worth 2 instructions and it is why the ROM can keep the limit in r4:
 *     fx32_multiply is INLINE ASM, not a call, and its clobber list is only
 *     r0/r1/r2/r12, so r4 survives it even though -fcall-used-r4 makes r4
 *     caller-saved.  A value born after the asm cannot inherit that.
 *   - `X / 2` is hoisted out of the outer loop and masked inside it (the ROM's
 *     `str r3,[sp,#8]` then `and r4, r9` per iteration).
 *   - The outer loop's counter and limit are `unsigned` -- the ROM's `bcs` / `bcc`.
 *   - `void`, from `pop {r0} / bx r0`.
 *
 * NEXT: this is the wrong function to push on.  Fix UpdateFieldScreen's r8 question
 * first -- it is the same question with six fewer instructions of noise around it.
 */
#include "gba/types.h"
#include "math.h"

extern unsigned char *iwram_3001e70;
extern unsigned int gBuffer[];
extern unsigned char ewram_2020000[];
extern unsigned char ewram_2020004[];

void Func_8010230(int x, int y)
{
    unsigned char *st;
    unsigned char *cfg;
    int *px;
    int *pz;
    int a;
    int b;
    int t;
    int X;
    int Y;
    int hx;
    int i;
    unsigned int j;
    unsigned int k;
    unsigned int n;
    int tbase;
    int cbase;
    int tx;
    int cx;
    int m1;
    int m2;
    int o;
    u8 *dst;
    u8 *d;

    st = iwram_3001e70;
    cfg = st + 0x104;
    y += 0xffa00000;
    x += 0xff880000;
    if (x < *(int *)(st + 0xec))
        x = *(int *)(st + 0xec);
    t = *(int *)(st + 0xf4) + 0xff100000;
    if (x > t)
        x = t;
    if (y < *(int *)(st + 0xf0))
        y = *(int *)(st + 0xf0);
    t = *(int *)(st + 0xf8) + 0xff600000;
    if (y > t)
        y = t;
    px = (int *)(st + 0xe4);
    *px = x;
    pz = (int *)(st + 0xe8);
    *pz = y;
    i = 0;
    do {
        o = i + 0x100;
        if (st[o] != 0) {
            n = 0x16;
            a = fx32_multiply(*px, *(int *)(cfg + 0x10));
            b = fx32_multiply(*pz, *(int *)(cfg + 0x14));
            if (*(int *)(cfg + 0x18) != 0) {
                t = *(int *)(cfg + 0x20) + *(int *)(cfg + 0x18);
                a += t;
                *(int *)(cfg + 0x20) = t;
                a &= (*(unsigned short *)(cfg + 0x28) << 19) | 0x7ffff;
            }
            if (*(int *)(cfg + 0x1c) != 0) {
                t = *(int *)(cfg + 0x24) + *(int *)(cfg + 0x1c);
                b += t;
                *(int *)(cfg + 0x24) = t;
                b &= (*(unsigned short *)(cfg + 0x2a) << 19) | 0x7ffff;
                n = 0x20;
            }
            a += *(int *)(cfg + 8);
            b += *(int *)(cfg + 0xc);
            cfg += 0x30;
            X = a / 0x80000;
            Y = b / 0x80000;
            dst = (u8 *)(0x6002800 + (i << 11));
            m1 = 0x7f;
            m2 = 0x1e;
            tbase = ((Y / 2) & m1) << 7;
            cbase = (Y & m2) << 5;
            n >>= 1;
            hx = X / 2;
            for (j = 0; j < n; j++) {
                tx = hx & m1;
                cx = X & m2;
                for (k = 0; k <= 0xf; k++) {
                    o = (gBuffer[tbase + tx] << 20) >> 17;
                    d = dst + ((cbase + cx) << 1);
                    *(u32 *)d = *(u32 *)(ewram_2020000 + o);
                    *(u32 *)(d + 0x40) = *(u32 *)(ewram_2020004 + o);
                    tx = (tx + 1) & m1;
                    cx = (cx + 2) & m2;
                }
                tbase = (tbase + 0x80) & 0x3f80;
                cbase = (cbase + 0x40) & 0x3c0;
            }
        }
        i++;
    } while (i <= 2);
}

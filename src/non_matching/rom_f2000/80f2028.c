/* Func_80f2028 (0x080f2028) -- NON-MATCHING, 463 of 532 encodings differ.
 * 465 ROM instructions.  Size 1112 against the ROM's 1144 (-32); 517
 * instructions against 532 (-15).  Frontier target: UNATTEMPTED before this
 * batch.  The `@ AnimateTitleScreen` prose above the reference is broadly
 * right (frame counter at +0x0c, phase at +0x14, k table at .Lf39ab) but its
 * "540 lines" is a .s LINE count, not an instruction count -- 465 is the
 * instruction count.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_f2000/Func_80f2028.c \
 *       asm/rom_f2000/rom_f2028_a.s --func Func_80f2028
 *
 * 463 IS NOT A DISTANCE -- the counts disagree, so objcmp's figure saturates.
 * MEASURE THIS ONE BY RELOCATION OFFSETS.  All twelve relocations appear in the
 * ROM's ORDER with the ROM's SYMBOLS, and relocations 0, 3, 4, 5, 6 and 7 are
 * BYTE-EXACT: the prologue and first __divsi3 (0x6a), the whole pool block at
 * 0x1ac-0x1b8 (iwram_3001efc, iwram_3001d20, iwram_3001ad0, .Lf39ab) and the
 * __divsi3 at 0x1f4 all land on the ROM's addresses.  The first differing
 * encoding is index 28, and it is only `mov r10, r3` against `mov r9, r3`.
 *
 * ================================================================
 * WHAT THE FUNCTION IS -- decoded, because the prose does not say it
 * ================================================================
 *
 * The buffer at [iwram_3001efc]+0x18 is an OAM SHADOW of 8-byte slots, DMA'd to
 * 0x7000000 at the end.  Slot word0 is attr0|attr1<<16, word1 is attr2.  So
 * `0x40002400 | y | (x+4)<<16` is attr0 = 0x2400|y (256-colour, semi-transparent)
 * and attr1 = 0x4000|(x+4) (size 01); the 0x5.../0x6.../0x7... tags add the
 * H/V flip bits, which makes each group a MIRRORED 2x2 QUAD.
 *
 * The per-sprite scale is `dy * (k - 0x68) / 0x50 + horizon` with dy = y - horizon,
 * horizon = 0x30 - iwram_3001ad0[3], y = 0x90 - phase, and k a byte from .Lf39ab
 * (which is exactly 6 bytes: .incrom 0xf39ab, 0xf39b1 -- six k values, and the
 * six uses are indices 0..5).  `__divsi3` with r1 = 0x50 proves a signed `/ 0x50`,
 * not a shift.
 *
 * The clamp is a HAND-WRITTEN WRAP, not a mask:
 *     while (v > 0xff) v -= 0x100;  while (v < 0) v += 0x100;
 * Both loops get a hoisted preheader constant (a pooled -0x100, then
 * `mov #0x80 / lsl #1`), which is what proves they are real loops with loop
 * notes rather than an `& 0xff`.
 *
 * Even frame writes 4+1+4 = 9 slots from k[0], k[2], k[4]; odd writes 4+4+4 = 12
 * from k[1], k[3], k[5].  That is why the counter ends at 9 and 12 and why it
 * must live in a register at the join.  The fill loop then writes 0x400020a0
 * into every remaining slot through 0x77, so the counter is 0x78 at the DMA.
 *
 * .Lf2396 IS A CROSS-JUMPED COMMON TAIL shared by the even k[4] quad and the odd
 * k[5] quad -- the two differ only in `mov r2, #0xc0` against `mov r2, #0xa0`.
 * Do not try to write it as one call; it is jump.c merging two inline copies.
 *
 * ================================================================
 * THE BLOCKER, isolated and PROVED REACHABLE -- addressing-mode selection
 * ================================================================
 *
 * The ROM stores EVERY constant-index OAM slot as
 *     mov r1, #0x18 / str r3, [r6, r1]
 * where gcc-2.96 emits `str r3, [r6, #24]`.  Thumb's `str rD,[rB,#imm]` scales
 * imm5 by 4, so every offset used here (0x18..0x5c) is in range and gcc always
 * folds it.  That is 26 extra `mov`s across the two branches and it is the
 * DOMINANT residue.
 *
 * MEASURED, not assumed.  Eight spellings were compiled (scratch probe): a plain
 * struct store, a `volatile` struct store, a pointer built by cast, a local index
 * set to 0 in the same block, a `volatile u32 *` cast, `((u32 *)st)[6]`, a named
 * offset variable used once, a named offset variable MUTATED by 4 and by 8, and
 * four adjacent slot stores.  ALL TEN fold to an immediate offset.  Only an index
 * gcc genuinely cannot know (a parameter) gives `str r1, [r0, r2]`.
 *
 * A corpus scan agrees: across the 3,914 generated `.s` files only 12 sites emit
 * `mov rN,#imm` + `str/ldr [rB,rN]`, and every one is either #224/#228 (OUT of
 * the imm5*4 range, so forced) or a degenerate #0 into a 2-D `.L` array.  There
 * is no in-range counterexample.  So this is the ORIGINAL COMPILER materialising
 * a constant displacement into a register where gcc-2.96 folds it -- an
 * addressing-mode/pass-order difference, not a source shape.
 *
 * IT IS REACHABLE, AND THE COST IS THE POINT.  An `__asm__("" : "+r"(o))` on the
 * OFFSET -- not on the index -- produces the ROM's form exactly and for free:
 *     mov r3, #24 / str r1, [r0, r3]
 * Laundering the INDEX instead is wrong and was measured: it emits `lsl`/`add`
 * scaffolding the ROM does not have.
 *
 * With that launder on all 26 constant-offset stores (scratch f2028_v2.c) the
 * function goes from -15 instructions to +7 (539 against 532) and 1156 against
 * 1144 bytes.  SO EVERYTHING ELSE IS WITHIN 7 INSTRUCTIONS.  That is the whole
 * value of the experiment; it is NOT a landing, because 26 `"+r"` barriers is a
 * fakematch-class shim (shimcount.py flags the macro outright) and the brief's
 * standard is pin-free.  The body below is therefore the PIN-FREE candidate.
 *
 * SHIMS: 0.  Pin-free.  No fakematch.txt row needed.
 *
 * TWO SMALLER RESIDUES, both named:
 *   1. A HIGH-REGISTER ROTATION at the first differing encoding: the ROM puts
 *      `horizon` in r10 and `dy` in r9; we get the reverse.  The ROM's
 *      later-defined quantity takes the HIGHER register, so this is an
 *      allocation-input difference (dy has two short defs, one per branch;
 *      horizon one long range), not REG_ALLOC_ORDER -- that is settled.
 *   2. gcc DERIVES `x + 0x14` from `x + 4` (`add r3,r3,#16`) where the ROM
 *      rebuilds it from x (`mov r1,r7 / add r1,#0x14`).  One instruction, the
 *      named-base lever pointing the other way.
 *
 * SPLIT: anchored grep gives THREE functions in asm/rom_f2000/rom_f2028_a.s --
 * Func_80f2028, LoadGS1TitleGFX, StartTitleScreen.  datacheck.py is SILENT: no
 * data section, no exports.  `.Lf39ab` is ALREADY `.global` in
 * asm/rom_f2000/rom_f2028_c_c_c_c.s, so it needs no export work -- reach it with
 * `extern unsigned char Lf39ab[] __asm__(".Lf39ab")`.
 *
 * CORRECTION to the LoadGS1TitleGFX park's split note: it says "the other two are
 * ALREADY PARKED (StartTitleScreen.c and the Func_80f2028 park)".  There was NO
 * Func_80f2028 park before this one -- src/non_matching/rom_f2000/ held only
 * 80f24a0_LoadGS1TitleGFX.c, 80f2d54.c, 80f3858.c, NintendoLogo.c and
 * StartTitleScreen.c.  With this file all three now have C, but all three are
 * NON-MATCHING, so the three-way cut is still premature -- see the report.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char iwram_3001efc[];
extern u8 iwram_3001d20;
extern unsigned short iwram_3001ad0[];
extern unsigned char Lf39ab[] __asm__(".Lf39ab");

struct Slot {
    u32 a01;
    u32 a2;
};

struct St {
    /* 0x00 */ s32 f0;
    /* 0x04 */ s32 f4;
    /* 0x08 */ s32 timer;
    /* 0x0c */ s32 frame;
    /* 0x10 */ s32 f10;
    /* 0x14 */ s32 phase;
    /* 0x18 */ struct Slot slot[121];
};

static inline s32 Wrap256(s32 v)
{
    while (v > 0xff) v -= 0x100;
    while (v < 0) v += 0x100;
    return v;
}

void Func_80f2028(void)
{
    struct St *st;
    u32 n;
    s32 horizon, y, dy, v, x, k;
    u32 xa, xb;
    u8 y2;
    u32 bytes;
    void *src;

    st = *(struct St **)iwram_3001efc;
    n = 0;
    if (iwram_3001d20 == 0) {
        s32 f = st->frame + 1;
        st->frame = f;
        if ((f & 3) == 0)
            st->phase++;
    }
    horizon = 0x30 - iwram_3001ad0[3];
    y = 0x90 - st->phase;
    if (st->timer < 0x118) {
        if ((st->frame & 1) == 0) {
            k = Lf39ab[0];
            dy = y - horizon;
            v = dy * (k - 0x68) / 0x50 + horizon - 0x10;
            x = k - 0x10;
            v = Wrap256(v);
            xa = (u32)(x + 4) << 16;
            xb = (u32)(x + 0x14) << 16;
            y2 = v + 0x10;
            st->slot[n].a01     = xa | (u32)v | 0x40002400;
            st->slot[n + 1].a01 = xb | (u32)v | 0x50002400;
            st->slot[n + 2].a01 = xa | y2 | 0x60002400;
            st->slot[n + 3].a01 = xb | y2 | 0x70002400;
            st->slot[n].a2     = 0xe8;
            st->slot[n + 1].a2 = 0xe8;
            st->slot[n + 2].a2 = 0xe8;
            st->slot[n + 3].a2 = 0xe8;
            n += 4;

            k = Lf39ab[2];
            v = dy * (k - 0x68) / 0x50 + horizon - 0x10;
            x = k - 0x10;
            v = Wrap256(v);
            st->slot[n].a01 = ((u32)(x + 4) << 16) | (u32)v | 0x80002400;
            st->slot[n].a2 = 0x80;
            n += 1;

            k = Lf39ab[4];
            v = dy * (k - 0x68) / 0x50 + horizon - 0x20;
            x = k - 0x20;
            v = Wrap256(v);
            xa = (u32)(x + 4) << 16;
            xb = (u32)(x + 0x24) << 16;
            y2 = v + 0x20;
            st->slot[n].a01     = xa | (u32)v | 0x80002400;
            st->slot[n + 1].a01 = xb | (u32)v | 0x90002400;
            st->slot[n + 2].a01 = xa | y2 | 0xa0002400;
            st->slot[n + 3].a01 = xb | y2 | 0xb0002400;
            st->slot[n].a2     = 0xc0;
            st->slot[n + 1].a2 = 0xc0;
            st->slot[n + 2].a2 = 0xc0;
            st->slot[n + 3].a2 = 0xc0;
            n += 4;
        } else {
            k = Lf39ab[1];
            dy = y - horizon;
            v = dy * (k - 0x68) / 0x50 + horizon - 0x10;
            x = k - 0x10;
            v = Wrap256(v);
            xa = (u32)(x + 4) << 16;
            xb = (u32)(x + 0x14) << 16;
            y2 = v + 0x10;
            st->slot[n].a01     = xa | (u32)v | 0x40002400;
            st->slot[n + 1].a01 = xb | (u32)v | 0x50002400;
            st->slot[n + 2].a01 = xa | y2 | 0x60002400;
            st->slot[n + 3].a01 = xb | y2 | 0x70002400;
            st->slot[n].a2     = 0xe8;
            st->slot[n + 1].a2 = 0xe8;
            st->slot[n + 2].a2 = 0xe8;
            st->slot[n + 3].a2 = 0xe8;
            n += 4;

            k = Lf39ab[3];
            v = dy * (k - 0x68) / 0x50 + horizon - 0x10;
            x = k - 0x10;
            v = Wrap256(v);
            xa = (u32)(x + 4) << 16;
            xb = (u32)(x + 0x14) << 16;
            y2 = v + 0x10;
            st->slot[n].a01     = xa | (u32)v | 0x40002400;
            st->slot[n + 1].a01 = xb | (u32)v | 0x50002400;
            st->slot[n + 2].a01 = xa | y2 | 0x60002400;
            st->slot[n + 3].a01 = xb | y2 | 0x70002400;
            st->slot[n].a2     = 0xe0;
            st->slot[n + 1].a2 = 0xe0;
            st->slot[n + 2].a2 = 0xe0;
            st->slot[n + 3].a2 = 0xe0;
            n += 4;

            k = Lf39ab[5];
            v = dy * (k - 0x68) / 0x50 + horizon - 0x20;
            x = k - 0x20;
            v = Wrap256(v);
            xa = (u32)(x + 4) << 16;
            xb = (u32)(x + 0x24) << 16;
            y2 = v + 0x20;
            st->slot[n].a01     = xa | (u32)v | 0x80002400;
            st->slot[n + 1].a01 = xb | (u32)v | 0x90002400;
            st->slot[n + 2].a01 = xa | y2 | 0xa0002400;
            st->slot[n + 3].a01 = xb | y2 | 0xb0002400;
            st->slot[n].a2     = 0xa0;
            st->slot[n + 1].a2 = 0xa0;
            st->slot[n + 2].a2 = 0xa0;
            st->slot[n + 3].a2 = 0xa0;
            n += 4;
        }
    }
    if (n <= 0x77) {
        do {
            st->slot[n].a01 = 0x400020a0;
            n++;
        } while (n <= 0x77);
    }
    REG_BLDCNT = 0x3f50;
    REG_BLDALPHA = 0xe0e;
    bytes = n * 8;
    src = &st->slot[0];
    DMA3_SET(src, (void *)(0xe0 << 19), 0x84000000 | (bytes >> 2));
    v = 0x20 - iwram_3001ad0[3];
    v = Wrap256(v);
    st->slot[12].a01 = (u32)v | 0xc05c2000;
    st->slot[12].a2 = 0x80 << 4;
    DMA3_SET(&st->slot[12], (void *)((0xe0 << 19) + bytes), 0x84000002);
    DMA3_SET(src, (void *)(0xe0 << 19), 0x84000008);
}

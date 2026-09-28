/* Func_8094544 (BuildDistortionTable) -- 0x08094544.  PARKED at 167 instructions
 * NON-MATCHING, 210 encodings of 238.  NOT a distance (ref 492 bytes / 238 encodings against ours 488 / 235).  `--align`: 167 of 238.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M` in the header.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/8094544.c \
 *     asm/rom_8a000/rom_944ec_a_a_a_a_c_a_a.s --func Func_8094544
 * in disagreeing regions of 238 (tryc --align).  NOT a true distance: 235
 * encodings against 238 and 488 bytes against 492, so objcmp's positional
 * 210 of 238 means nothing.
 * ref: asm/rom_8a000/rom_944ec_a_a_a_a_c_a_a.s  (ONE function, no data section;
 *      grep -ci func_start = 1; would convert the WHOLE FILE)
 * batch 293, brief A, target 4.  Shims in the code: ONE, and it is not a pin --
 * `extern short L9ed84[] __asm__(".L9ed84")` is a name binding (elevation.md
 * says so explicitly) and needs no linker alias because
 * asm/rom_8a000/rom_944ec_c_c_b.s already declares `.global .L9ed84`.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_8a000/8094544.c \
 *     --ref asm/rom_8a000/rom_944ec_a_a_a_a_c_a_a.s --align
 *
 * WHAT THE FUNCTION IS.  Two identical halves, each rebuilding one of the two
 * interleaved halfword columns of a 0xa0-row, 0xc-byte-per-row HBlank scroll
 * table.  Half 1 writes the table at +0/+4/+8 of each row, half 2 at +2/+6/+0xa;
 * each half picks its double buffer with `sel = 1 ^ buf[0xf00]` and `p = buf +
 * sel * 0x780`, and each has a degenerate arm (step == 0: fill every row with the
 * same three values) and a wave arm.  Half 1 reads the source fields at +0xc,
 * +8, +4 of iwram_3001ad0 and half 2 the ones at +0xe, +0xa, +6 -- i.e. the two
 * halves of three (x, y) pairs.
 *
 * ESTABLISHED, and these are the parts worth keeping:
 *
 * 1. `.call_via r10` IS include/math.h's `fx32_multiply`, USED UNCHANGED.  The
 *    ROM's per-half `ldr r0, =Func_8000888 / mov r10, r0` in the loop preheader
 *    plus `.call_via r10` in the body is exactly what the shipped inline gives
 *    when loop-invariant motion hoists its `fxmul` pointer: the inline leaves the
 *    pointer UNPINNED (`"r" (fxmul)`) and gcc chooses a high register on its own.
 *    No hand-rolled call_via helper, no register pin, one call site per half.
 *    This is the "one site" sub-shape of elevation.md's `.call_via` section and it
 *    reproduces with no help at all -- worth recording, because the section
 *    prescribes writing a bespoke helper per (callee, register) pair.
 *
 * 2. A POINTER LOCAL FOR THE SOURCE STRUCT, `g = iwram_3001ad0;`.  All six reads
 *    are `ldrsh`, which in thumb-1 is register-offset only, so the ROM has
 *    `ldr r3,=iwram_3001ad0 / mov r4,#0xe / ldrsh r2,[r3,r4]`.  Written
 *    `*(short *)(iwram_3001ad0 + 0xe)` the sum is a link-time constant, gcc pools
 *    `iwram_3001ad0+14` as ONE word and then derives the other five with
 *    `sub r2, r3, #2` -- eight extra instructions and a wrong relocation.
 *    The pointer local fixes all six at once: 241 lines -> 236, and the
 *    per-field `mov rN, #imm / ldrsh` pairs then match the ROM one for one.
 *    Note this is the SAME lever as the named gState offsets in Func_808d5dc,
 *    reached from the other end -- there the base stayed a symbol and the offset
 *    was named; here the offset stays a literal and the base is named.  Which
 *    end you name is decided by what the ROM's pool holds.
 *
 * 3. The 0xc-byte row stride is spelled DIFFERENTLY in the two arms, and the ROM
 *    says so: the fill arm writes `[r3], [r3,#4], [r3,#8]` then `add r3,#0xc`,
 *    the wave arm writes `[r4]` three times with `add r4,#4` between.  Both are
 *    in the draft as written; do not unify them.
 *
 * THE BLOCKER: HOW MANY VALUES ARE SPILLED, AND INTO WHICH SLOTS.
 *      ROM   sp+0x14=se  sp+0x10=sa  sp+0xc=s6  sp+8=(se<<16)
 *            sp+4=(u16)sc in half 1   sp+0=(u16)sa in half 2
 *            r12=sc  r0=s8  r1=s4   -- frame 0x18, SIX slots
 *      ours  sp+0x10=s6  sp+0xc=sa   sp+8=sc    sp+4=se
 *            r11=s8  r9=s4          -- frame 0x14, FIVE slots
 * We spill `sc` where the ROM keeps it in r12, and our slot order is the reverse
 * of the ROM's.  r12 (ip) is call-clobbered, so the ROM is holding `sc` in a
 * scratch register across the straight-line prologue only and spilling the three
 * fields half 2 needs; we spill four of the six fields instead.  That decides the
 * register names for the rest of the function, which is most of the 167.
 *
 * This is the third function in this brief to land on the same class -- see
 * Func_8095c08's park for the one case where the mechanism was pinned down
 * exactly (local-alloc claiming a register before global-alloc ranks the
 * competitor, proved from the .lreg dump).  The same `-dl` / `-dg` dump technique
 * applies here and has NOT yet been run on this function; that is the cheapest
 * next step and it will say whether `sc`'s r12 comes from local-alloc or from
 * reload.
 *
 * SECOND RESIDUE, independent of the spills and probably cheap: the ROM computes
 * `(u16)se` ONCE as `se << 16` stored at sp+8 and both halves do
 * `ldr r1,[sp,#8] / lsr r3, r1, #16`.  The draft has `t = se << 16;` at the top
 * and `(unsigned)t / 0x10000` at both sites, which is the right shape, but check
 * the emitted form: gcc may be recomputing the shift rather than reloading the
 * slot.
 *
 * NOT YET TRIED AT ALL: writing the two halves as two calls to one
 * `static inline`, or as two separate `static inline` bodies with the offsets
 * written inside each (batch 292's AnimStart lever).  The halves differ only in
 * three offsets (0xf10/0xf14, 0xf18/0xf1c, +0/+2) and three field choices, so
 * they are the textbook case for it, and the current draft writes them out twice.
 */
#include "math.h"

extern unsigned char iwram_3001ed8[];
extern unsigned char iwram_3001ad0[];
extern short L9ed84[] __asm__(".L9ed84");

void Func_8094544(void)
{
    unsigned char *b;
    unsigned char *q;
    unsigned char *p;
    unsigned char *g;
    int s4;
    int s6;
    int s8;
    int sa;
    int sc;
    int se;
    int sel;
    int step;
    int amp;
    int acc;
    int t;
    int i;
    int d;
    int v;

    b = *(unsigned char **)iwram_3001ed8;
    g = iwram_3001ad0;
    se = *(short *)(g + 0xe);
    sc = *(short *)(g + 0xc);
    sa = *(short *)(g + 0xa);
    s8 = *(short *)(g + 8);
    s6 = *(short *)(g + 6);
    s4 = *(short *)(g + 4);
    sel = 1 ^ b[0xf00];
    p = b + sel * 0x780;
    step = *(int *)(b + 0xf10);
    t = se << 16;
    acc = *(int *)(b + 0xf08) * (*(unsigned short *)(b + 0xf02) + (unsigned)t / 0x10000);
    if (step == 0) {
        q = p;
        for (i = 0; i < 0xa0; i++) {
            *(short *)q = sc;
            *(short *)(q + 4) = s8;
            *(short *)(q + 8) = s4;
            q += 0xc;
        }
    } else {
        amp = *(int *)(b + 0xf18);
        q = p;
        for (i = 0; i < 0xa0; i++) {
            v = fx32_multiply(L9ed84[(acc >> 16) & 0xff], amp);
            if (v < 0)
                v += 0xff;
            d = (unsigned)(v << 8) / 0x10000;
            *(short *)q = (unsigned short)sc + d;
            q += 4;
            *(short *)q = (unsigned short)s8 + d;
            q += 4;
            *(short *)q = d + (unsigned short)s4;
            q += 4;
            acc += step;
        }
    }
    sel = 1 ^ b[0xf00];
    p = b + sel * 0x780 + 2;
    step = *(int *)(b + 0xf14);
    acc = *(int *)(b + 0xf08) * (*(unsigned short *)(b + 0xf02) + (unsigned)t / 0x10000);
    if (step == 0) {
        q = p;
        for (i = 0; i < 0xa0; i++) {
            *(short *)q = se;
            *(short *)(q + 4) = sa;
            *(short *)(q + 8) = s6;
            q += 0xc;
        }
    } else {
        amp = *(int *)(b + 0xf1c);
        q = p;
        for (i = 0; i < 0xa0; i++) {
            v = fx32_multiply(L9ed84[(acc >> 16) & 0xff], amp);
            if (v < 0)
                v += 0xff;
            d = (unsigned)(v << 8) / 0x10000;
            *(short *)q = (unsigned short)sa + d;
            q += 4;
            *(short *)q = (unsigned short)s6 + d;
            q += 4;
            *(short *)q = d + (unsigned short)se;
            q += 4;
            acc += step;
        }
    }
    *(unsigned short *)(b + 0xf02) = *(unsigned short *)(b + 0xf02) + 1;
    b[0xf00] = b[0xf00] ^ 1;
}

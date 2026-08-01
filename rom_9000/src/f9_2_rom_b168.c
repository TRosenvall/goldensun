/* Func_b168 -- SubmitSpritePairWithLabel
 *
 * Renders the object's text label via Func_aa0c, then builds and submits both
 * OAM entries.  See f9_2_rom_b168.s for the ROM's disassembly and annotation.
 *
 * STATUS: NOT MATCHING.  Stack frame size is now correct (0x44) and the affine
 * block matches instruction for instruction, but aff lands at sp+0x00 where the
 * ROM puts it at sp+0x24.  f9_2_rom_b168.s is present, so the .s rule wins and
 * THIS FILE IS NOT BUILT.
 *
 *     tools/asmdiff.py Func_b168 rom_9000/src/f9_2_rom_b168.c \
 *         --rom-offset 0xb168 --rom-size 544
 *
 * SOLVED on the way here, worth keeping:
 *
 *   - agbcc compiles `x & 0xffff0000` as a shift pair (lsr #16; lsl #16) when
 *     the constant is used once, but as `ldr` from the literal pool plus `and`
 *     when it is held in a VARIABLE and reused.  The ROM loads 0xffff0000 into
 *     r4 once and ANDs with it three times, so the masks must come from
 *     `unsigned int hi = 0xffff0000;` rather than appearing as literals.  Same
 *     for 0xffff.
 *   - aff[0] is one load, two mask-or pairs, one store -- not three separate
 *     read-modify-writes.  Hence the `t` temporary.
 *   - Taking `int *ap = aff` once makes agbcc keep the address in a register
 *     the way the ROM does (`add r0, sp, #36` then `[r0]` / `[r0+4]`).
 *   - The upper half of aff[1] is never initialised; the ROM reads whatever is
 *     on the stack. The masked assignment reproduces that.
 *
 * REMAINING BLOCKER: local layout, and it is not a tuning problem.
 *
 * The ROM has nine scalar slots at sp+0x00..0x20 and the 32-byte affine block
 * at sp+0x24.  agbcc does the opposite.  Confirmed with a minimal test rather
 * than inferred -- nine ints plus one int[8], compiled with the project flags:
 *
 *     add sp, sp, #-0x28      ; 0x28 = 32 (array) + 8 (two spilled scalars)
 *     str r0, [sp, #0x20]     ; scalars land ABOVE
 *     str r5, [sp]            ; array lands at 0x00
 *
 * So agbcc places aggregates low and scalar spills high, always.  Declaration
 * order does not move it; switching int[8] to a struct does not move it; both
 * were tried.  Therefore the ROM's block at sp+0x24 was NOT a local aggregate
 * in this function.  Candidates worth investigating, in order:
 *
 *   - it is two words, not eight, and a separate 24-byte local I have not
 *     modelled occupies sp+0x2C..0x43 (the ROM only ever touches [r0] and
 *     [r0+4] of it, so its real size is unverified)
 *   - the nine low slots are outgoing argument space for a call I have
 *     mis-modelled, which would place them low by the ABI and push a genuine
 *     local aggregate above
 *
 * decomp-permuter was run against this base (10 min, 6 threads): score 14945 ->
 * 12445, and the best candidate diffs no better than the hand version.  That is
 * consistent with the blocker being structural rather than allocational -- the
 * permuter searches register allocation, it cannot reshape the stack frame.
 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;

int Func_aa0c(void *obj, int style);
int Func_3d28(int *aff);
void Func_3dec(void *entry, int prio);

void Func_b168(void *obj, int *pos, int *scale, int style)
{
    int prio;
    int py;
    int mode;
    int offA, offB;
    u8 *hp;
    int halfH, halfW;
    int mtx;
    int aff[8];
    int sx, sy;
    register int px asm("r10");
    register int pz asm("r11");
    int pw;
    int lbl, t, d, h;
    int x1, y1;
    u8 *o2;
    int prioArg;

    halfW = *(u8 *)((u8 *)obj + 0x20) >> 1;
    hp    = (u8 *)obj + 0x21;
    halfH = *hp >> 1;
    offB  = 8;
    offA  = 4;

    sx = *scale++;
    px = *pos++;
    sy = *scale;
    py = *pos++;
    style = (u16)style;
    pz = *pos++;
    pw = *pos;

    lbl = Func_aa0c(obj, style);

    if (lbl == 0 && sx == 0x10000 && sy == sx
        && *(u16 *)((u8 *)obj + 0x1e) == 0) {
        mode = 0;
        mtx = 0;
    } else {
        unsigned int hi = 0xffff0000;
        unsigned int lo = 0xffff;
        unsigned int t;
        int *ap = aff;

        mode = 1;
        ap[1] = (ap[1] & hi) | *(u16 *)((u8 *)obj + 0x1e);
        t = ap[0];
        t = (t & hi) | (u16)(sx << 8 >> 16);
        t = (t & lo) | ((u16)(sy << 8 >> 16) << 16);
        ap[0] = t;
        if (lbl != 0)
            ap[0] = (hi & t) | (u16)(-(int)*(u16 *)ap);
        mtx = Func_3d28(ap);
    }

    if (sx > 0x10000 || sy > 0x10000) {
        halfW <<= 1;
        halfH <<= 1;
        mode = 3;
        offB = 0x10;
        offA = 8;
    }

    if (py <= (int)0xff9c0000) {
        prio = 1;
        prioArg = 0;
    } else {
        prio = (pz >> 17) + 0xa;
        prioArg = 2;
    }

    y1 = ((pz - pw) >> 16) - offA;
    t = px >> 16;

    if (*(u8 *)((u8 *)obj + 0x26) & 1) {
        if (y1 <= 0x9f) {
            o2 = (u8 *)obj + 0xc;
            o2[5] = (o2[5] & ~3) | mode;
            o2[7] = (o2[7] & ~0x3f) | ((mtx & 0x1f) << 1);
            *(unsigned short *)(o2 + 6) =
                (*(unsigned short *)(o2 + 6) & ~0x1ff) | ((t - offB) & 0x1ff);
            o2[4] = y1;
            Func_3dec(o2, prioArg);
        }
    }

    d = (s8)*(u8 *)((u8 *)obj + 0x22);
    x1 = (t - halfW) + ((sx * d + 0xffff) >> 16);

    h = *hp;
    h = (h >> 1) - (s8)*(u8 *)((u8 *)obj + 0x23);
    y1 = (((pz - py) >> 16) - halfH) - ((sy * h + 0xffff) >> 16);

    if (x1 <= 0xef && y1 <= 0x9f) {
        *(unsigned short *)((u8 *)obj + 6) =
            (*(unsigned short *)((u8 *)obj + 6) & ~0x1ff) | (x1 & 0x1ff);
        *(u8 *)((u8 *)obj + 4) = y1;
        *(u8 *)((u8 *)obj + 5) = (*(u8 *)((u8 *)obj + 5) & ~3) | mode;
        mtx &= 0x1f;
        *(u8 *)((u8 *)obj + 7) = (*(u8 *)((u8 *)obj + 7) & ~0x3f) | (mtx << 1);
        Func_3dec(obj, prio);
    }
}

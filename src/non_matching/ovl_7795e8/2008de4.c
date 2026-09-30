/* OvlFunc_880_2008de4  --  0x02008de4   [PARK DRAFT]
 *
 * NON-MATCHING, 464 of 496 encodings differ.  Reference
 * asm/overlays/rom_7795e8/ovl_30_c_c_c_a_a.s (ONE function by the anchored
 * .thumb_func_start pattern; tools/datacheck.py prints nothing, so text-only
 * and NO SPLIT).  ONE overlay.ld row: overlays/rom_7795e8/overlay.ld.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7795e8/2008de4.c \
 *       asm/overlays/rom_7795e8/ovl_30_c_c_c_a_a.s --func OvlFunc_880_2008de4
 *
 * NOT A DISTANCE: size 996 against 1024 AND count 483 against 496, so objcmp's
 * 464 is saturated.  tools/aligncmp.py (no masking) reads aligned-equal 197 of
 * 496 (39.7%), 355 differing/ins/del in 88 hunks.  That is a FIRST-DRAFT figure
 * from four passes, up from 33.9% -- it is not a stalled reconstruction, it is
 * an unfinished one, and the next rung is named below.
 * SHIMS: 0 pins, 0 barriers.  Four asm-label externs (.L16c0, .L16dc, .L16ec)
 * plus gOvl_020096d0, all ALREADY `.global` in the sibling data file
 * asm/overlays/rom_7795e8/ovl_30_c_c_c_c.s -- no export work, and all are FOUR
 * DIGITS so none sits in the two-digit capture hazard.
 *
 * ============================================================
 * THE BLOCKER, NAMED: `out` IS SPILLED AND THE ROM KEEPS IT IN r11.
 *
 * The frame is the tell.  The ROM's is 0x40 with the 8-word `buf` at sp+0x20 and
 * EIGHT word slots below it (0 = a caller-save temp, 4 = bp, 8 = v8, 0xc = vc,
 * 0x10 = v10, 0x14 = v14, 0x18 = size, 0x1c = mode); ours is 0x44 with buf at
 * sp+0x24 and NINE positions below, because we spill `out` as well (slot 0x1c
 * mode, 0x1c-4 out, ...).  The ROM's prologue reads `mov r11, r2`; ours reads
 * `str r2, [sp, #28]`, and that ONE decision moves every stack offset in the
 * function, which is why the raw figure looks so much worse than the
 * reconstruction is.
 *
 * Both `out` and `bp` are whole-function allocnos and only one can have a
 * callee-saved register; -fcall-used-r4 leaves only r5-r7 and r8-r11 to hold a
 * value across a call, and the four outer loops need r8 (unit), r9 (the loop
 * counter), r10 (the running output offset) and r11.  The ROM gives r11 to `out`
 * and spills `bp`; we spill both.
 *
 * MEASURED, and this is the useful part: a hard-register pin
 * (`register unsigned char *out __asm__("r11") = arg;`) DOES keep it in r11 and
 * moves size 996 -> 1016 and count 483 -> 493 of 496 -- but aligned-equal FALLS
 * to 35.5%, the frame stays 0x44, and something else spills in its place.  So
 * the pin is not the answer: the right lever is whatever REDUCES the competing
 * pressure by one, and until that is found this is a global-alloc park, the same
 * class as the 78 REG_ALLOC_ORDER parks but with a concrete, checkable target
 * (`mov r11, r2` in the prologue and a 0x40 frame).
 *
 * The three remaining register roles all follow from the same cause: the ROM's
 * 0x17-iteration ability loop puts its counter in r14 and its table pointer in
 * r12 (gcc-2.96 DOES allocate ip and lr -- REG_ALLOC_ORDER is {3,2,1,0,12,14,4,
 * ...}), and the unit pointer in r12 in the stats loop.  We reach for r5-r7
 * there because we have a register to spare.
 *
 * ============================================================
 * WHAT IS ESTABLISHED.  All of this is read out of the ROM and reproduced.
 *
 * 1. THE SIGNATURE IS (int, int, unsigned char *) AND THE FIRST ARGUMENT IS
 *    DEAD: `mov r0, #0xb` is the first instruction after the frame, so r0 is not
 *    a live parameter.  It returns `int` -- the size, reloaded from its stack
 *    slot at the epilogue (`ldr r0, [sp, #0x18]`).
 *
 * 2. THE SIZE IS A SWITCH ON THE MODE with a default: 0xb by default, 0xad for
 *    0, 0x27 for 1, 9 for 2.  `cmp r1,#1 / beq / cmp r1,#1 / bgt / cmp r1,#0 /
 *    beq / b` is gcc's own comparison tree for a three-case switch, not source.
 *
 * 3. `size` IS RELOADED FROM ITS SLOT EVERY ITERATION of the clearing loop, and
 *    that is CORRECT, not a spill artefact: the loop body is `out[i] = 0`
 *    through an `unsigned char *`, whose alias set is 0, so the store may alias
 *    the int local and gcc must reload.  The same mechanism explains the two
 *    separate `ldr r3, [r2, #0x10]` loads of the gState word in the 0xa5 block
 *    and the two `ldr [sp, #0x10]` loads of v10 in the tail -- each has a `strb`
 *    through `out` between it and the previous load.  DO NOT "fix" these.
 *
 * 4. THE ITEM LOOP INDEXES BY A BYTE OFFSET, NOT A POINTER.  `mov r0,#0xd8 /
 *    mov r8,r0 / ... / mov r1,r8 / ldrh r0,[r1,r4]` is a register-offset load
 *    with the OFFSET in the loop-carried register and the unit pointer in r4;
 *    writing it as a walking `unsigned short *` cost 6 encodings and lost the
 *    `str r4,[sp] / bl / ldr r4,[sp]` caller-save of the unit around
 *    __GetItemInfo.  An `int o = 0xd8; ... *(unsigned short *)(unit + o); o += 2;`
 *    is the shape, and it took the figure from 33.9% to 39.1%.
 *    THE OTHER THREE inner scans over the same +0xd8 table ARE pointer walks
 *    (`ldrh r3,[r0] / add r0,#2`).  Same table, two different constructs.
 *
 * 5. `id >> (bit + 1)` IS AN ARITHMETIC SHIFT (`asrs`), so the masked item id is
 *    a SIGNED int even though it cannot be negative.  `unsigned` gives `lsrs`.
 *
 * 6. THE LEVEL FIELD'S SHIFT IS DISTRIBUTED BY HAND: `lsl r0, r6, #3 / sub r2,
 *    r0, r4` is `(u << 3) - u`, not `u * 7` -- batch 298's rule.
 *
 * 7. `out[3]` COMBINES TWO ACCUMULATORS: `((v14 >> 20) & 0xf0) | (v10 & 0xf)`,
 *    and the byte reads of the spilled accumulators go through
 *    `add rN, sp, #K / ldrb rN, [rN]` because Thumb has no `ldrb [sp, #imm]` --
 *    that is gcc narrowing the load for a truncating store, NOT a union or a
 *    cast in the source.
 *
 * 8. THE TWO OVERWRITTEN BYTES IN THE PACKING LOOP ARE `|=`, AND GCC FORWARDS
 *    THE STORE.  `strb r1,[r0,#7] ... orr r1,r3 ... strb r1,[r0,#7]` is
 *    `p[7] = w1;` then `p[7] |= w2 >> 28;` with no reload, because every store
 *    between them is to a different offset of the SAME pointer.  Same for
 *    `p[0xb]`.  The nibble-straddling layout means each 16-byte record packs
 *    into 15 bytes.
 *
 * 9. THE 8-OR-9 OFFSET IS gcc's `!= 0`: `neg r3,r3 / orr r3,r4 / lsr r3,#31 /
 *    add r3,#8` is `8 + (mode != 0)`.
 *
 * 10. `st` (unit + 0x10) IS A NAMED SUB-POINTER and the two clamped 16.16
 *     halfwords are read BOTH signed and unsigned at the same address:
 *     `(short)st[0] > 0x7cf` gives the `ldrsh` (a register offset is mandatory --
 *     Thumb `ldrsh` has no immediate form, hence the `mov r0,#0` beside it) and
 *     `(short)st[0] < 0` gives an `ldrh` plus `lsl #16 / cmp #0`, with CSE
 *     satisfying the second read from the first load or from the clamp's store.
 *     The three 0x3e7 clamps below them are UNSIGNED (`bls`).
 *
 * 11. gState MUST COME FROM A NAMED BASE POINTER in the 0xa5 block or gcc folds
 *     the addend into the pool word and loses `ldrh r3,[r2,#0x12]` /
 *     `ldr r3,[r2,#0x10]`.  Worth 2 encodings and it is the same lever as the
 *     one that made OvlFunc_891_200905c exact.
 *
 * ============================================================
 * WHAT IT DOES.  Serialises the four party members into a byte buffer for the
 * link/trade protocol.  Returns the length: 0xad for mode 0 (everything), 0x27
 * for mode 1, 9 for mode 2, 0xb otherwise.  It clears that many bytes of `out`,
 * zeroes an 8-word scratch record, then folds six save bits into a bitmask,
 * clamps and packs each unit's HP/PP/ATK/DEF/AGI/LCK/level into two words of the
 * scratch record, ORs the levels into v14 and four elemental words into v10, and
 * marks which of eight key items each unit carries.  For mode 0 only it then
 * packs each unit's 15 item ids 9 bits at a time from out+0x27 and each unit's
 * 0x17 ability levels 5 bits at a time from out+0x6b, and copies the play
 * time/money from gState to out+0xa5.  Finally it byte-swaps the scratch record
 * into out+8 (or +9) and writes the bitmasks into out[0..8].
 */
extern unsigned char gState[];
extern unsigned short gOvl_020096d0[];
extern int L16c0[] __asm__(".L16c0");
extern unsigned short L16dc[] __asm__(".L16dc");
extern unsigned short L16ec[] __asm__(".L16ec");

extern int __GetFlag(int id);
extern unsigned char *__GetUnit(int id);
extern int __GetItemInfo(int item);

int OvlFunc_880_2008de4(int a0, int mode, unsigned char *out)
{
    unsigned int buf[8];
    int size;
    unsigned int v14;
    unsigned int v10;
    unsigned char vc;
    unsigned char v8;
    unsigned int *bp;

    unsigned int *w;
    unsigned int *wp;
    unsigned char *p;
    unsigned char *unit;
    unsigned short *ip;
    unsigned short *tp;
    unsigned short *fp;
    unsigned short *st;
    unsigned char *gs;
    int i, j, k, u;
    int sh, off, bit, shift, o;
    unsigned short t;
    unsigned short found;
    unsigned short want;
    unsigned int wv;
    int lvl;

    size = 0xb;
    switch (mode) {
    case 0:
        size = 0xad;
        break;
    case 1:
        size = 0x27;
        break;
    case 2:
        size = 9;
        break;
    }
    for (i = 0; i != size; i++)
        out[i] = 0;
    bp = buf;
    v14 = 0;
    v10 = 0;
    vc = 0;
    v8 = 0;
    w = bp;
    for (i = 0; i != 8; i++)
        *w++ = 0;
    fp = gOvl_020096d0;
    for (i = 0; i != 6; i++) {
        if (__GetFlag(*fp++) != 0)
            v8 |= 1 << i;
    }
    w = bp;
    for (u = 0; u != 4; u++) {
        unit = __GetUnit(L16c0[u]);
        st = (unsigned short *)(unit + 0x10);
        t = st[0];
        if ((short)st[0] > 0x7cf) {
            st[0] = 0x7cf;
            t = 0x7cf;
        }
        if ((short)t < 0)
            st[0] = 0;
        t = st[1];
        if ((short)st[1] > 0x7cf) {
            st[1] = 0x7cf;
            t = 0x7cf;
        }
        if ((short)t < 0)
            st[1] = 0;
        if (st[4] > 0x3e7)
            st[4] = 0x3e7;
        if (st[5] > 0x3e7)
            st[5] = 0x3e7;
        if (st[6] > 0x3e7)
            st[6] = 0x3e7;
        if (*(unsigned char *)(unit + 0x1e) > 0x63)
            *(unsigned char *)(unit + 0x1e) = 0x63;
        w[0] = ((short)st[0] << 21) | ((short)st[1] << 10) | st[4];
        w[1] = (st[5] << 22) | (st[6] << 12)
               | (*(unsigned char *)(unit + 0x1e) << 4);
        lvl = *(unsigned char *)(unit + 0xf);
        if (lvl > 0x63) {
            *(unsigned char *)(unit + 0xf) = 0x63;
            lvl = 0x63;
        }
        if (lvl == 0)
            *(unsigned char *)(unit + 0xf) = 1;
        v14 |= *(unsigned char *)(unit + 0xf) << ((u << 3) - u);
        wp = (unsigned int *)(unit + 0xf8);
        sh = 0;
        for (j = 0; j != 4; j++) {
            v10 += *wp++ << sh;
            sh += 7;
        }
        ip = (unsigned short *)(unit + 0xd8);
        for (k = 0; k != 15; k++) {
            want = ip[k] & 0x1ff;
            tp = L16dc;
            for (j = 0; j != 8; j++) {
                if (want == *tp++)
                    vc |= 1 << j;
            }
        }
        w += 2;
    }
    if (mode == 0) {
        off = 0x27;
        bit = 0;
        for (u = 0; u != 4; u++) {
            unit = __GetUnit(L16c0[u]);
            p = off + out;
            o = 0xd8;
            for (k = 0; k != 15; k++) {
                int id;
                __GetItemInfo(*(unsigned short *)(unit + o));
                id = *(unsigned short *)(unit + o) & 0x1ff;
                p[0] += id >> (bit + 1);
                p[1] += id << (7 - bit);
                bit++;
                p++;
                off++;
                if (bit == 7) {
                    bit = 0;
                    p++;
                    off++;
                }
                o += 2;
            }
        }
        shift = -1;
        off = 0x6b;
        for (u = 0; u != 4; u++) {
            unit = __GetUnit(L16c0[u]);
            p = off + out;
            tp = L16ec;
            for (j = 0; j != 0x17; j++) {
                want = *tp;
                found = 0;
                ip = (unsigned short *)(unit + 0xd8);
                for (k = 0; k != 15; k++) {
                    wv = ip[k];
                    if ((wv & 0x1ff) == want)
                        found = (wv & 0xf800) >> 11;
                }
                if (shift < 0) {
                    *p += found >> -shift;
                    off++;
                    p++;
                    shift += 8;
                }
                *p += found << shift;
                shift -= 5;
                if (shift == -5) {
                    p++;
                    off++;
                    shift = 3;
                }
                tp++;
            }
        }
        gs = gState;
        p = out;
        p += 0xa5;
        *p = *(unsigned short *)(gs + 0x12);
        p++;
        *p = *(unsigned int *)(gs + 0x10) >> 8;
        p++;
        *p = *(unsigned int *)(gs + 0x10);
    }
    if (mode != 2) {
        w = bp;
        p = out + (8 + (mode != 0));
        for (i = 0; i != 2; i++) {
            unsigned int w0, w1, w2, w3;
            w0 = w[0];
            p[0] = w0 >> 24;
            p[1] = w0 >> 16;
            p[2] = w0 >> 8;
            p[3] = w0;
            w1 = w[1];
            p[4] = w1 >> 24;
            p[5] = w1 >> 16;
            p[6] = w1 >> 8;
            p[7] = w1;
            w2 = w[2];
            p[7] |= w2 >> 28;
            p[8] = w2 >> 20;
            p[9] = w2 >> 12;
            p[0xa] = w2 >> 4;
            p[0xb] = w2 << 4;
            w3 = w[3];
            p[0xb] |= w3 >> 28;
            p[0xc] = w3 >> 20;
            p[0xd] = w3 >> 12;
            p[0xe] = w3 >> 4;
            w += 4;
            p += 0xf;
        }
    }
    out[0] = v14;
    out[1] = v14 >> 8;
    out[2] = v14 >> 16;
    out[3] = ((v14 >> 20) & 0xf0) | (v10 & 0xf);
    out[4] = v10 >> 4;
    out[5] = v10 >> 12;
    out[6] = v10 >> 20;
    out[7] = v8;
    if (mode != 0)
        out[8] = vc;
    return size;
}

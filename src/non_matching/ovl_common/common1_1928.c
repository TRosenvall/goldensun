/* OvlFunc_common1_1928  --  the first of three functions in
 * goldensun/asm/overlays/common/common1_c_a_c_c_a_c_c_a.s, named by exactly three
 * `overlay.ld` rows (rom_7db0c8, rom_7ddb88, rom_7e0928).
 *
 * NON-MATCHING, 193 of 222 encodings differ (ref 480 bytes / 222 encodings,
 * ours 472 / 219).  The instruction COUNT is exact at 211; objcmp's encoding
 * count saturates because three encodings are missing, so read the count as
 * "not done", not as a distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/common1_1928.c \
 *     asm/overlays/common/common1_c_a_c_c_a_c_c_a.s --func OvlFunc_common1_1928
 *
 * No data section in the file, so NO TEXT/DATA split: `datacheck` prints
 * nothing.  One external resolved by the asm-label extension:
 * `extern unsigned char L15[] __asm__(".L15");` -- `.L15` is NOT defined here,
 * and the only `.global .L15` in the tree is
 * asm/overlays/common/common1_c_c_b_c.s:37.  **That is a two-digit label and
 * therefore sits in the documented capture hazard**: gcc names its own branch
 * targets `.L15` too, and this function's generated assembly must be grepped
 * (`grep -oE '^\.L[0-9]+:' out.s`) before the file is landed.  It did not
 * collide in any of the runs measured here, but the notebook's advice for a low
 * number is to rename and export instead, and that decision is the owner's.
 * No shims, no pins.
 *
 * THE RESIDUE is two register-role swaps and the cursor block's schedule:
 *
 *   - `a` and `b` (the two script words) want r7 and r4; we get r4 and r7, so
 *     the caller-save pair the ROM spends on `b` around `__atan2` is spent on
 *     `a` around `__GetFlag` instead.  Both cost two instructions, which is why
 *     the count is right and the encodings are not.
 *   - the reload temporaries for the `ewram_2001000` base differ (the ROM uses
 *     r1/r2, we use r0/r2), which costs r0 the `e` value it was carrying and
 *     adds a `mov r0, r5` before `__Actor_SetAnim`.
 *   - in the cursor block the ROM interleaves `strh` of the advanced cursor
 *     between the two table loads; we emit it before both.
 *
 * Seven unrelated spellings measured IDENTICALLY (63.5% aligned, 112
 * differing): declaring `b` before `a`, merging the travel target into `b` as
 * well as `a`, and four callee return-type changes (`__Actor_SetAnim`,
 * `__SetFlag`, `__Actor_TravelTo`, `__Actor_SetScript` each as `int`).  Per the
 * notebook that says the variable does not exist and the return types are not
 * it, so the next rung is the flags.
 *
 * WHAT CLOSED THE REST, from 217 differing and 12 instructions short:
 *
 * 1. TWO OFFSETS 0xa AND 0x12 OVERLAP THE INTS AT 8 AND 0x10.  They are the
 *    HIGH HALVES of the two 16.16 coordinates, not separate fields; declaring
 *    them as struct members silently shifts every later offset (the first
 *    candidate wrote `[r5, #0x6c]` where the ROM has `#0x64`).  They are read
 *    as `*(short *)((char *)e + 0xa)`.
 *
 * 2. THE SCRIPT CURSOR IS `(short)k * 2 + 0xf0` WITH THE OFFSET FINISHED FIRST.
 *    Written as one expression gcc folds the base in early and loses the ROM's
 *    register-offset `ldrsh [r6, r3]`; `off = (short)k * 2; off += 0xf0;` then
 *    `*(short *)(base + off)` is the documented fix.  And the four cursors must
 *    come from FOUR SEPARATE pseudos (`n`, `k = n + 1`, `m = k + 1`, `m + 1`):
 *    writing `(short)(n + 1) * 2` lets fold distribute the cast and gcc chains
 *    the second offset off the first with `add r1, #0xf2`.
 *
 * 3. THE FIELD MUST BE READ ONCE.  `s->f6` is `unsigned short` and the cursor
 *    needs `(short)` of it; with the store `s->f6 = n + 2` written AFTER the
 *    reads, gcc satisfies `(short)n` with a SECOND load (`ldrsh`) beside the
 *    `ldrh`, three instructions where the ROM has one load and a shift pair.
 *    Putting the store before the reads kills the memory and forces the shift.
 *
 * 4. `if (e->f8 != a || e->f10 != b) { ...atan2... } else { s->f8++; }` -- the
 *    equality-first spelling puts the increment block ahead of the atan2 block
 *    and inverts `beq`/`b` into `bne`.
 *
 * 5. The `unsigned short` local for the `__atan2` result (see
 *    `OvlFunc_899_200c8c8`, lever 1) -- `extern unsigned short __atan2(...)` is
 *    NOT enough, gcc-2.96 does not re-extend a narrow return value.
 *
 * 6. The travel target reuses `a` (the first script word): the ROM's r7 holds
 *    the script word in one arm and the travel x in the other, and merging them
 *    is what moves `base` off r7 and onto the ROM's r6.
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x1c];
    int f30;
    int f34;
    unsigned char pad38[0x2c];
    short f64;
};

struct Ctl {
    short f0;
    short f2;
    short f4;
    unsigned short f6;
    short f8;
};

extern unsigned char ewram_2001000[];
extern unsigned char *iwram_3001f3c;
extern unsigned char L15[] __asm__(".L15");

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern int __atan2(int dz, int dx);

void OvlFunc_common1_1928(void)
{
    struct Ctl *s;
    struct Actor *e;
    unsigned char *base;
    int n, k, m, mm;
    int off;
    int a, b;
    int z;
    int t, d;
    unsigned short u;
    unsigned short *p;

    s = (struct Ctl *)ewram_2001000;
    base = iwram_3001f3c;
    e = __MapActor_GetActor(s->f4);
    if (e == 0)
        return;
    if (s->f0 == 1) {
        n = s->f6;
        k = n + 1;
        off = (short)n * 2;
        off += 0xf0;
        a = *(short *)(base + off);
        off = (short)k * 2;
        off += 0xf0;
        s->f6 = k + 1;
        b = *(short *)(base + off);
        if (a == 0 && b == 0) {
            s->f0 = 9;
            __Actor_SetAnim(e, 1);
            t = *(int *)(base + 0xe8);
            if (t < e->f8)
                a = t + (0xc0 << 12);
            else
                a = t - (0xc0 << 12);
            if (__GetFlag(0x211)) {
                z = *(int *)(base + 0xec) + (0x80 << 13);
                p = (unsigned short *)(base + 0xe4);
            } else {
                z = *(int *)(base + 0xec) - (0x80 << 13);
                p = (unsigned short *)(base + 0xe2);
            }
            e->f64 = *p;
            e->f34 = 0x80 << 7;
            e->f30 = 0x80 << 9;
            __Actor_TravelTo(e, a, 0, z);
            __SetFlag(0x211);
            __Actor_SetScript(e, L15);
            return;
        }
        a <<= 16;
        b <<= 16;
        if (s->f2 != 0)
            a = (*(int *)(base + 0xe8) << 1) - a;
        if (e->f8 != a || e->f10 != b) {
            u = __atan2(b - e->f10, a - e->f8);
            d = (short)(u - e->f6);
            if (d > (0x80 << 5))
                d = 0x80 << 5;
            if (d < -0x1000)
                d = -0x1000;
            e->f6 = e->f6 + d;
            e->f8 = a;
            e->f10 = b;
            s->f8 = 0;
        } else {
            s->f8++;
        }
        if (s->f8 > 2)
            __Actor_SetAnim(e, 1);
        else
            __Actor_SetAnim(e, 5);
    } else if (s->f0 == 2) {
        a = *(short *)((char *)e + 0xa);
        n = s->f6;
        b = *(short *)((char *)e + 0x12);
        k = n + 1;
        m = k + 1;
        off = (short)n * 2;
        off += 0xf0;
        *(short *)(base + off) = a;
        off = (short)k * 2;
        off += 0xf0;
        *(short *)(base + off) = b;
        s->f6 = m;
        mm = (short)m;
        if (mm != 0x383e)
            return;
        off = mm * 2;
        off += 0xf0;
        *(short *)(base + off) = 0;
        off = (short)(m + 1) * 2;
        off += 0xf0;
        *(short *)(base + off) = 0;
        s->f4 = *(unsigned short *)(base + 0xe0);
        s->f6 = 0;
        s->f0 = 1;
    }
}

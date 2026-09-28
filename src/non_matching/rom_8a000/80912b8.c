/* Func_80912b8 (ScreenShakeTask) -- 0x080912b8.  PARKED at 115 instructions in
 * NON-MATCHING, 184 encodings of 217.  A TRUE DISTANCE -- 217 = 217 encodings and no SIZE line.  `--align`: 115 of 207.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M` in the header.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/80912b8.c \
 *     asm/rom_8a000/rom_8d9a4_c_c_c_a_c_c_a.s --func Func_80912b8
 * disagreeing regions of 207 (tryc --align).  This IS A TRUE DISTANCE: objcmp
 * reports 217 encodings against 217 and prints no SIZE line, so instruction
 * count and byte size both agree exactly; only the register assignment and the
 * literal-pool ORDER are wrong (objcmp: 184 of 217 positional, RELOCATIONS
 * differ).
 * ref: asm/rom_8a000/rom_8d9a4_c_c_c_a_c_c_a.s  (ONE function, no data section;
 *      grep -ci func_start = 1; would convert the WHOLE FILE)
 * batch 293, brief A, target 1.  No shims in the draft.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_8a000/80912b8.c \
 *     --ref asm/rom_8a000/rom_8d9a4_c_c_c_a_c_c_a.s --align
 *
 * WHAT THE FUNCTION IS.  Two structurally identical halves, each one HBlank
 * scroll descriptor: half 1 works on the object at [iwram_3001ee0] and half 2 on
 * that object + 0xc, and the only differences between the two bodies are
 * `*(int *)(d+8) = 0x400` against `= 0`, and `x` having 0x100000 added before
 * half 2 runs.  Written out twice, which is what the ROM has.
 *
 * LEVERS THAT LANDED (each is in the draft; the structure, every offset, both
 * calls to _Func_8011f54, all the masked halfword writes and both guards are
 * already instruction-for-instruction right):
 *
 * 1. THE SECOND GLOBAL IS DERIVED, NOT NAMED.  The ROM reads two pointers:
 *        ldr r3, =iwram_3001ee0 / ldr r1, [r3] / sub r3, #0x70 / ldr r3, [r3]
 *    That `sub r3, #0x70` is cse reusing the pool-loaded address with an offset.
 *    Writing `*(unsigned char **)(iwram_3001ee0 - 0x70)` reproduces it exactly
 *    and needs ONE pool word.  Declaring a second `extern iwram_3001e70` would
 *    give two pool words and a second relocation; taking the OTHER symbol as the
 *    base (iwram_3001e70 + 0x70) would relocate against the wrong name.
 *
 * 2. `j = 0xbd << 1;` AS A NAMED OFFSET for gSpriteSlots.  The ROM has
 *    `ldr r3,=gSpriteSlots / mov r1,#0xbd / lsl r1,#1 / add r3,r1`; written
 *    `gSpriteSlots[0xbd]` the sum folds to one pooled `gSpriteSlots+0x17a`.
 *    Same lever as Func_808d5dc's gState offsets -- three functions in this bank
 *    this batch, so it is the bank's habit and not a one-off.
 *
 * 3. `d = base;` IN THE ENTRY BLOCK, NOT INSIDE THE FIRST GUARD.  The ROM's
 *    `mov r5, r11 / ldr r3, [r5, #0x18]` is one pseudo doing double duty: the
 *    low-register copy of `base` needed to read base[0x18], commoned by cse with
 *    the descriptor pointer used throughout half 1.  With `d = base;` inside the
 *    `if` the two land in different blocks, cse cannot common them, and `d` loses
 *    r5.  155 -> 115 on this one change, and it is what puts `d` in r5 and `hi`
 *    in r6 exactly as the ROM has them.
 *
 * 4. `int t = *(unsigned short *)(d + N); *(unsigned short *)(d + N) =
 *    (t & ~MASK) | a;` with `a` masked in its own earlier statement, which is the
 *    recipe already recorded in src/non_matching/rom_15000/801c154.c.  It gets
 *    the ROM's operand order (source mask computed first, destination mask as the
 *    orr's destination register) at all four sites.
 *
 * THE BLOCKER: WHICH VALUE GETS SPILLED, and it is one swap.
 * Ten values must survive calls -- base, d, z, w, b, x, hi and the three saved
 * halfwords -- and there are seven callee-saved registers, so three spill.
 *      ROM   r5=d r6=hi r7=x r8=b r9=w r10=z r11=base ; [sp+0]=s0 [sp+4]=s4 [sp+8]=s8
 *      ours  r5=d r6=hi r7=x r8=z r9=?  r10=w r11=s0  ; [sp+8]=base [sp+4]=s8 [sp+0]=s4
 * We agree on d, hi and x and then spill `base` where the ROM spills `s0`.
 * `base` and `s0` have the SAME reference count (3 each: one set and two uses)
 * and both are live from near the entry to half 2, so global.c's allocno_compare
 * separates them only by live_length -- and `base` is set first, so its range is
 * the longer one and it loses.  Its last use is half 2's `d = base + 0xc`, which
 * the ROM also has (`mov r5, r11 / add r5, #0xc`), so the range cannot be
 * shortened without changing the code.
 *
 * SECOND, SMALLER RESIDUE: the frame is `sub sp, #0x10` in the ROM and
 * `sub sp, #0xc` here, i.e. the ROM allocates FOUR words for three spilled
 * values and never touches sp+0xc.  gcc-2.96 does not pad a thumb frame (ours
 * is 12, unaligned), so the ROM really has a fourth slot.  An `int t[4]` for the
 * three saved halfwords was tried and gives `sub sp, #0x14` plus a spill, i.e.
 * five words, and costs 14 extra instructions (132 of 207) because array
 * elements are reloaded at every use -- so the fourth word is NOT a fourth array
 * element.  Unexplained; it is two encodings (`sub sp` and `add sp`).
 *
 * THIRD RESIDUE: POOL ORDER, and it is the same mode-of-the-constant mechanism
 * that Func_808d5dc needed.  `~0x3ff` and `~0x1ff` are used in HImode stores, so
 * gcc narrows them to HImode pool entries and emits `ldrh r3, .Lpool` -- the same
 * encoding as the ROM's `ldr r3, =0xfffffc00` (batch 292 settled that), but
 * placed FIRST in the pool, ahead of every SImode word, which shifts every
 * relocation.  Unlike Func_808d5dc, an int carrier does NOT fix it here:
 *   - named `int m10 = ~0x3ff; int m9 = ~0x1ff;`           125 (worse)
 *   - `(int)0xfffffc00` / `(int)0xfffffe00` casts           115 (byte-identical
 *                                                          to the draft: inert)
 *   - a named `int mf = 0xf0` for the +4 byte store         118 (worse)
 * The named masks cost two more live values and the register allocation gets
 * worse faster than the pool gets better.  They will probably be needed once the
 * spill choice above is fixed, so try them AGAIN then -- this is a candidate
 * coupled pair in exactly the sense batch 292 recorded: each alone is a
 * regression.
 *
 * ALSO NOTE: `((z >> 16) & 0xf0)` reaches the byte store at +4 and gcc narrows
 * the mask to QImode, where 0xf0 is -16, and emits `mov r1,#16 / neg r1,r1`
 * against the ROM's `mov r2,#0xf0`.  That is one extra instruction and it is a
 * REAL difference, not a register rename -- but the instruction count already
 * matches, so something else is one short.  Reconcile those two before trusting
 * the "true distance" label too far.
 */
extern unsigned char iwram_3001ee0[];
extern unsigned char gSpriteSlots[];
extern int _Func_8011f54(int a, int b, int c);
extern void Func_8003dec(unsigned char *p, int n);

void Func_80912b8(void)
{
    unsigned char *base;
    unsigned char *o;
    unsigned char *d;
    unsigned char *h;
    int s8;
    int s4;
    int s0;
    int z;
    int w;
    int b;
    int x;
    int hi;
    int t;
    int a;
    int m;
    int j;

    base = *(unsigned char **)iwram_3001ee0;
    h = *(unsigned char **)(iwram_3001ee0 - 0x70);
    h += 0xe4;
    s8 = *(short *)(h + 2);
    s4 = *(short *)(h + 6);
    d = base;
    o = *(unsigned char **)(d + 0x18);
    if (o == 0)
        return;
    z = *(int *)(o + 0x10);
    s0 = *(short *)(o + 0x16);
    x = *(int *)(o + 8) - 0x80000;
    b = o[0x22];
    j = 0xbd << 1;
    w = *(unsigned short *)(gSpriteSlots + j) >> 5;
    hi = _Func_8011f54(b, x, z + (0x80 << 13)) >> 16;
    t = (_Func_8011f54(b, x, z + (0x80 << 14)) >> 16) - 0x10;
    if (t > hi)
        hi = t;
    if (hi > 0 && hi > s0) {
        *(int *)(d + 4) = 0x40000800;
        *(int *)(d + 8) = 0x80 << 3;
        m = ~0xc;
        d[9] &= m;
        a = w & 0x3ff;
        t = *(unsigned short *)(d + 8);
        *(unsigned short *)(d + 8) = (t & ~0x3ff) | a;
        d[5] = (d[5] & m) | 4;
        a = ((x >> 16) & 0xfff0) - s8;
        a &= 0x1ff;
        t = *(unsigned short *)(d + 6);
        *(unsigned short *)(d + 6) = (t & ~0x1ff) | a;
        d[4] = ((z >> 16) & 0xf0) - s4 - hi + 0x10;
        Func_8003dec(d, 0);
    }
    x += 0x80 << 13;
    hi = _Func_8011f54(b, x, z + (0x80 << 13)) >> 16;
    t = (_Func_8011f54(b, x, z + (0x80 << 14)) >> 16) - 0x10;
    d = base + 0xc;
    if (t > hi)
        hi = t;
    if (hi > 0 && hi > s0) {
        *(int *)(d + 4) = 0x40000800;
        *(int *)(d + 8) = 0;
        m = ~0xc;
        d[9] &= m;
        w &= 0x3ff;
        t = *(unsigned short *)(d + 8);
        *(unsigned short *)(d + 8) = (t & ~0x3ff) | w;
        d[5] = (d[5] & m) | 4;
        a = ((x >> 16) & 0xfff0) - s8;
        a &= 0x1ff;
        t = *(unsigned short *)(d + 6);
        *(unsigned short *)(d + 6) = (t & ~0x1ff) | a;
        d[4] = ((z >> 16) & 0xf0) - s4 - hi + 0x10;
        Func_8003dec(d, 0);
    }
}

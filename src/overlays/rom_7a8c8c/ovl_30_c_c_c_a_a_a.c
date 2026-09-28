/* OvlFunc_922_2009c18  --  163 instructions
 *
 * Cut out of goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_c_c_a_a_a.s (single
 * function, no data; confirmed by `grep -ci func_start`).
 *
 * EXACT: 352 bytes, 166 encodings and 9 relocations identical (objcmp --func).
 * The three `__divsi3` vs `_divsi3_RAM` relocations are the linker alias and
 * `__divsi3 = _divsi3_RAM;` is ALREADY at overlays/rom_7a8c8c/overlay.ld:121,
 * so nothing needs adding there. objcmp says so itself, three times.
 *
 * Eight parameters, four of them on the stack. `.L2418` is 12 bytes -- exactly
 * three words -- and is already `.global` in
 * asm/overlays/rom_7a8c8c/ovl_30_c_c_c_c_c_c_c_c.s, so the reference needs no
 * new export. The `ldmia r3!, {r0,r1,r7} / stmia r2!, {r0,r1,r7}` pair is a
 * 12-byte aggregate copy of it into a local, which is why the local has to be a
 * struct rather than an `int[3]`: an array cannot be assigned.
 *
 * TWO THINGS DECIDE THIS FUNCTION, and both are register allocation reached
 * through statement placement rather than through any pin.
 *
 * 1. WHERE `mk` IS ASSIGNED DECIDES WHETHER ANYTHING SPILLS -- 65 -> 42 of 170,
 *    and it removes the spill slot (`sub sp,#0x10` back to the ROM's `#0xc`).
 *
 *    The function needs seven call-crossing quantities and has exactly seven
 *    registers (r5-r11; r4 is call-used under -fcall-used-r4). It only fits
 *    because two pairs share: a3 dies at the `[0x44]` store and `mk` is born
 *    after it, so both live in r11; and the 0xf mask, the scaled subscript
 *    `(flags & 0xf) << 2` and the `s` pointer chain through r5 in turn --
 *    the ROM's `ldr r5, [r1, r5]` is the last of those three handovers.
 *
 *    Written at its point of use, `mk` beats the subscript in
 *    `global.c allocno_compare`, whose priority is
 *        floor_log2 (n_refs) * n_refs / live_length
 *    -- both have n_refs 3, so the SHORTER live range wins, and `mk`'s is far
 *    shorter. It then takes r5 (REG_ALLOC_ORDER puts 5 before 11), the
 *    subscript finds every register taken and spills. Moving `mk = ~0xc;` up to
 *    immediately after `*(int *)(actor + 0x44) = a3;` -- the first point at
 *    which it may be born without overlapping a3, and so the latest placement
 *    that still permits the r11 share -- lengthens its live range past the
 *    subscript's priority, and the whole allocation falls into the ROM's.
 *
 *    This is a NEW instance of a known class and the mechanism is worth the
 *    note: the lever is not "hoist the constant", it is "lengthen the live
 *    range of the quantity that must NOT win the register". Hoisting it one
 *    statement further, above the `[0x44]` store, breaks the a3/r11 share and
 *    spills again.
 *
 * 2. THE ZERO STORED FOUR TIMES IS ONE QUANTITY -- 76 -> 65. The ROM builds
 *    `mov r2, #0` once and reuses it for actor[0x55], p7[0x26], actor+0x30 and
 *    actor+0x34, across nine intervening instructions. Four literal `0`s
 *    rematerialise it. Named `int z`, it is one pseudo.
 *
 * 3. THE LAST THREE INSTRUCTIONS were the read-modify-write of p7[9] in the
 *    0x20000 block, and this is the destination-naming lever from
 *    src/overlays/rom_799abc/ovl_30_c_c_c_c_b.c with a new boundary. Measured,
 *    each against the finished rest of the function (of 170, the three
 *    `_divsi3_RAM` lines excluded):
 *
 *      p7[9] = (p7[9] & mk) | ((src[0] & 3) << 2);        4 differing
 *      p7[9] = ((src[0] & 3) << 2) | (p7[9] & mk);        2  (orr's dest wrong)
 *      t = src[0]; t &= 3;  ... | (t << 2)                2  (lsl one slot late)
 *      t = src[0]; t &= 3;  ... | ((t & 3) << 2)         11
 *      the same with the `&` and `|` split into separate
 *        accumulate statements (six spellings tried)   9, 10, 12, 16, 16
 *      t = src[0]; t &= 3; a = t << 2; ... | a           MATCH
 *      t = src[0]; t &= 3; t <<= 2;    ... | t           MATCH
 *
 *    So the fix is THREE separate statements -- load, mask, shift -- and then
 *    the ordinary `(p7[9] & mk) | a` expression. `t &= 3` puts the AND's result
 *    in the loaded byte's register, which is where the ROM has it, and giving
 *    the shift its own statement emits it before the `and r3,r1` of the other
 *    operand. Splitting FURTHER, into named `&` and `|` accumulators, is a
 *    regression at every spelling tried: this site wants fewer statements than
 *    that sibling's did, and the existing note should not be read as "more
 *    naming is always closer".
 *
 * INERT HERE, measured: spelling `mk` as a bare `~0xc` literal at both sites,
 * or as a second assignment at the second site (76 either way, identical to the
 * single named local -- cse merges them and the allocation does not move);
 * naming the subscript `i = flags & 0xf` (65, no change -- the shared quantity
 * is the SCALED offset, which gcse builds either way); dropping the named `s`
 * and writing the subscript in both divide branches (+5 instructions);
 * naming a local copy of `src` in the two late blocks (42, no change).
 *
 * SHIMS: none. No `register ... __asm__` pins and no `__asm__(".equ ...)`
 * lines. The one `__asm__` in the file is the asm-label on the `.L2418`
 * declaration, which is the tree's standard spelling for a dot-label symbol and
 * not a measurement aid.
 */
struct P3 { int w[3]; };
extern struct P3 gL2418 __asm__(".L2418");

extern unsigned char *__CreateActor(int kind, int a, int b, int c);
extern void __Actor_SetAnim(unsigned char *actor, int anim);
extern void __Actor_SetScript(unsigned char *actor, void *script);
extern void __Func_80929d8(unsigned char *actor, int a);
extern void OvlFunc_922_2009bdc(void);

void OvlFunc_922_2009c18(int a0, int a1, int a2, int a3, int a4, int a5,
                         unsigned int flags, unsigned char *src)
{
    struct P3 v;
    unsigned char *actor;
    unsigned char *p7;
    int *s;
    int mk;
    int z;
    int t, a;

    v = gL2418;
    actor = __CreateActor(0xde, a0, a1, a2);
    if (actor == 0)
        return;
    p7 = *(unsigned char **)(actor + 0x50);
    __Actor_SetAnim(actor, (flags + 1) & 0xf);
    __Actor_SetScript(actor, (void *)v.w[flags & 0xf]);
    z = 0;
    actor[0x55] = z;
    p7[0x26] = z;
    *(int *)(actor + 0x6c) = (int)OvlFunc_922_2009bdc;
    *(int *)(actor + 0x44) = a3;
    mk = ~0xc;
    *(int *)(actor + 0x48) = a4;
    *(int *)(actor + 0x4c) = a5;
    *(int *)(actor + 0x30) = z;
    *(int *)(actor + 0x34) = z;
    p7[9] = (p7[9] & mk) | 4;
    if ((flags & 0xffff0000) == 0)
        return;
    if (src == 0)
        return;
    if (flags & 0x10000)
        __Func_80929d8(actor, *(int *)(src + 4));
    if (flags & 0x20000) {
        actor[0x23] &= 0xfe;
        t = src[0];
        t &= 3;
        a = t << 2;
        p7[9] = (p7[9] & mk) | a;
    }
    if (flags & 0x80000) {
        *(int *)(actor + 0x18) = *(int *)(src + 8);
        *(int *)(actor + 0x1c) = *(int *)(src + 0xc);
    }
    if (flags & 0x40000) {
        s = (int *)v.w[flags & 0xf];
        if (flags & 0x80000) {
            *(int *)(actor + 0x30) = (*(int *)(src + 0x10) - *(int *)(actor + 0x18)) / s[3];
            *(int *)(actor + 0x34) = (*(int *)(src + 0x14) - *(int *)(actor + 0x1c)) / s[3];
        } else {
            *(int *)(actor + 0x30) = (*(int *)(src + 0x10) - 0x10000) / s[3];
            *(int *)(actor + 0x34) = (*(int *)(src + 0x14) - 0x10000) / s[3];
        }
    }
}

/* Func_8099070 -- 0x08099070  (asm/rom_8a000/rom_97b54_a_c_c_a_c_c_a.s)
 *
 * TWIN: Func_80990cc in the same .s is identical except that it SUBTRACTS the
 * shifted counter where this one adds it, so this park covers both -- and the
 * .s holds only these two, so solving it converts the whole TU with no split.
 *
 * BLOCKER: one register role. 7 of 45, exact length.
 *
 *     rom   asr r2, r3, #0x10 / cmp r2, #0 ... lsl r3, r2, #0xb
 *           / lsl r0, r2, #0x11 / add r1, r3
 *     ours  asr r0, r3, #0x10 / cmp r0, #0 ... lsl r3, r0, #0xb
 *           / add r1, r3 / lsl r0, #0x11
 *
 * The counter lands in r0 for us and r2 in the ROM. Because ours is in r0 --
 * which is also where the second shift's result must go -- the shift becomes
 * destructive and moves after the add; the ROM keeps r2 and writes r0, so both
 * shifts precede the add. The whole residue follows from that one choice.
 *
 * THE BIG STEP WAS BLOCK LAYOUT: 29 differing to 7. The ROM's `beq` jumps to the
 * short arm and FALLS THROUGH to the main block, so the main block is the `if`
 * body:
 *
 *     if (s != 0) { ...vector work... } else { a[0x6c] = Func_8099040; }
 *
 * Written as an early return for the zero case -- which reads more naturally --
 * the two arms swap and everything after diverges.
 *
 * MEASURED AND INERT at 7: naming both shift results so they precede the add;
 * naming the zero index used by the `ldrsh`. MEASURED AND WORSE at 13: merging
 * the decremented value and its sign-extension into ONE variable. The ROM keeps
 * them in r3 and r2, so two variables is right here -- worth recording because
 * the merge lever has been paying elsewhere and this is where it does not apply.
 */
extern void vec3_translate(int a, int b, int *v);
extern void Func_8099040(void);

void Func_8099070(int *a)
{
    int v[3];
    unsigned short *c;
    int t;
    int s;
    int ang;

    if (a == 0)
        return;
    c = (unsigned short *)((char *)a + 0x64);
    t = *c - 1;
    *c = t;
    s = (short)t;
    if (s != 0) {
        v[0] = a[0xe];
        v[1] = a[0xf];
        v[2] = a[0x10];
        ang = *(short *)((char *)a + 0x66 + (unsigned int)0) + (s << 11);
        vec3_translate(s << 17, ang, v);
        a[2] = v[0];
        a[3] = v[1];
        a[4] = v[2];
    } else {
        *(void **)((char *)a + 0x6c) = (void *)Func_8099040;
    }
}

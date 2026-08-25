/* THE SPRITE-FLAGS SETTER -- five identical copies, all NOT MATCHING at 4 of 11.
 *
 * Source asm: goldensun/asm/overlays/common/common0.s
 *
 * Members (the body is instruction-identical in all five):
 *
 *   OvlFunc_common0_0     asm/overlays/common/common0.s
 *   OvlFunc_927_20089dc   asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c.s
 *   OvlFunc_946_20089dc   asm/overlays/rom_7ced6c/ovl_30_a_a_c_c_c.s
 *   OvlFunc_964_20089dc   asm/overlays/rom_7ed0a0/ovl_30_a_a_a_c_c_c_c.s
 *   OvlFunc_965_20089dc   asm/overlays/rom_7ef4f4/ovl_30_a_a_a_c_c_c_c.s
 *
 * Blocker class: register allocation plus one load position. The TAIL IS EXACT
 * -- `lsl / and / orr / strb / bx` all match -- and four instructions in the
 * middle are permuted:
 *
 *     rom    mov r3,#0x3 / ldrb r2,[r0,#9] / and r1,r3 / mov r3,#0xd / neg r3,r3
 *     ours   mov r3,#0x3 / and r1,r3 / mov r2,#0xd / ldrb r3,[r0,#9] / neg r2,r2
 *
 * The ROM loads the flags byte EARLY, between building the 3 and using it, and
 * keeps the mask in r3. gcc loads it late and keeps the mask in r2.
 *
 * WHAT THE ROUND ESTABLISHED, AND IT MATTERS BEYOND THIS FAMILY:
 *
 * gcc-2.96 NARROWS A MASK THAT FEEDS A BYTE STORE. Written the obvious way --
 *
 *     s[9] = (s[9] & ~0xc) | ((f & 3) << 2);
 *
 * -- gcc emits a single `mov r3, #0xf3`, because it knows the result is stored
 * to a byte and the top 24 bits cannot matter. The ROM materialises the FULL
 * 32-bit 0xfffffff3 with `mov r3,#0xd / neg r3,r3`, two instructions, and comes
 * out one instruction longer. No spelling of the CONSTANT changes this: `-0xd`
 * and `~0xc` give identical output.
 *
 * WHAT DOES CHANGE IT is forcing the mask through an int local that is negated
 * in its own statement:
 *
 *     m = 0xd;
 *     m = -m;
 *     v &= m;          <- and it must be `v &= m`, not `m &= v`
 *
 * `m &= v` collapses back to the single narrowed `mov`, ten instructions again.
 * That asymmetry is not obvious and cost four screens.
 *
 * NARROWER THAN IT READS -- corrected in batch 56. OvlFunc_968_2009a50
 * (src/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_a_c_b.c) MATCHES with exactly the
 * `m &= f` form that fails here, mov/neg and all. So the rule is not "compound
 * assignment with the mask as destination collapses"; something else about this
 * function decides it. The likely difference is that here the OR'd-in value is
 * computed (`(f & 3) << 2`) where there it is the constant 4, but that is a
 * hypothesis and has not been tested.
 *
 * Do not read the paragraph above as a general law. Try BOTH forms.
 *
 * THE SAME NARROWING PARKED OvlFunc_931_2008c0c at 1 of 24 in batch 50, where
 * the ROM builds the same 0xfffffff3 by `sub`ing from a zero it already held.
 * Two functions, two different ROM spellings of the constant, one gcc
 * behaviour. Worth treating as a class.
 *
 * ORDERINGS TRIED, all at 11 instructions unless noted:
 *
 *   t = f & 3 before the byte load                      4  (this body)
 *   byte load before t = f & 3                          7  (10 insn, narrows)
 *   `t = 3; t &= f;` to force the and's operand order   7
 *   `s[9] = (m & v) | t` as one expression              4  (same as this)
 *   the mask built with `~0xc` instead of -0xd          9  (10 insn, narrows)
 *
 * NEXT: the two remaining defects are the load position and r2-vs-r3, which are
 * allocation, not expression shape. Nothing in the C reaches them. If the
 * narrowing above can be defeated more cleanly -- so the mask is simply an int
 * that gcc never proves byte-width -- the allocation may fall out with it.
 */
void OvlFunc_common0_0(void *a, int f)
{
    unsigned char *s;
    int t;
    int m;
    int v;

    s = *(unsigned char **)((unsigned char *)a + 0x50);
    t = f & 3;
    v = s[9];
    m = 0xd;
    m = -m;
    t <<= 2;
    v &= m;
    s[9] = v | t;
}

/* Cluster OvlFunc_959_2008ce0..OvlFunc_959_2008ce0 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_a.o and the rest of the overlay in
 * goldensun/overlays/rom_7e7574/overlay.ld.
 *
 * THE BASIC-BLOCK LEVER BREAKS CONSTANT-CSE, which is new and is what this
 * function is worth reading for.
 *
 * `__Func_8012330(0xc0 << 10, 0xc0 << 10, 0x80 << 9)` passes the SAME value
 * twice. Written as literals, gcc builds it once and copies:
 *
 *     rom    mov r0,#0xc0 / mov r1,#0xc0 / mov r2,#0x80 / lsl r0,#10 /
 *            lsl r1,#10 / lsl r2,#9
 *     ours   mov r1,#0xc0 / lsl r1,#10 / mov r0,r1 / ...
 *
 * Assigning them to TWO SEPARATE LOCALS in a dominating block -- here above the
 * two early returns -- makes gcc rematerialise each at the call instead. Until
 * now the lever was understood as fixing ORDER; it also defeats the CSE that
 * makes the ROM's redundant form unreachable. See reports/arg-interleave.md.
 *
 * The pooled 0xe666 in the second call needs the same treatment for the
 * ordinary reason -- pool loads come first unless the value crosses a block
 * boundary. With only that one levered the function is 4 of 46; with all four
 * values levered it matches.
 */
extern unsigned int iwram_3001ebc;
extern int __CheckPartyItem(int item);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void OvlFunc_959_2008c90(int a);
extern void __PlaySound(int id);
extern void __Func_8012330(int a, int b, int c);

void OvlFunc_959_2008ce0(void)
{
    unsigned char *base;
    short v;
    int k;
    int x, y, z, w;

    x = 0xc0 << 10;
    y = 0xc0 << 10;
    z = 0x80 << 9;
    w = 0xe666;
    base = (unsigned char *)iwram_3001ebc;
    if (__CheckPartyItem(0xea) == -1)
        return;
    v = *(short *)(base + (0xb6 << 1));
    k = v - 0x28;
    if (__GetFlag(0x941) && k == 4)
        return;
    OvlFunc_959_2008c90(k);
    __PlaySound(0x9d);
    __Func_8012330(x, y, z);
    __Func_8012330(-1, -1, w);
    __SetFlag(v + (0xca << 2));
}

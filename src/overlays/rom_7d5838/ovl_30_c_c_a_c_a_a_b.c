/* Cluster OvlFunc_950_200809c..OvlFunc_950_200809c extracted from goldensun/asm/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a.s.
 *
 * Slotted between ovl_30_c_c_a_c_a_a_a.o and the rest of the overlay.
 *
 * Sets two fields of the iwram_3001ebc block and hands off. The argument is
 * passed straight through -- the ROM never writes r0 before the `bl`.
 *
 * WRITTEN WITH PLAIN LITERAL OFFSETS, WHICH IS THE POINT. The ROM walks one
 * register through the whole function --
 *
 *     mov r3,#0xe0 / lsl r3,#1 / add r2,r1,r3 / add r3,#0x41 / str r3,[r2]
 *     sub r3,#0x39 / add r2,r1,r3 / mov r3,#0x18 / str r3,[r2]
 *
 * -- so 0x201 is built as 0x1c0+0x41 and the next offset as 0x201-0x39. That
 * reads like the offset-variable-reuse lever, and reproducing it in C
 * (`v = 0x1c0; q = p + v; v += 0x41; *q = v;`) is 10 of 15: gcc allocates the
 * base and offset to different registers than the ROM.
 *
 * The two plain stores below match. The add/sub chain is gcc doing its OWN
 * arithmetic on a constant it already had in a register, not the source
 * reusing a variable. See docs/elevation.md.
 */
extern unsigned char *iwram_3001ebc;
extern void __Func_8091e9c(int a);

void OvlFunc_950_200809c(int a)
{
    unsigned char *p;

    p = iwram_3001ebc;
    *(int *)(p + 0x1c0) = 0x201;
    *(int *)(p + 0x1c8) = 0x18;
    __Func_8091e9c(a);
}

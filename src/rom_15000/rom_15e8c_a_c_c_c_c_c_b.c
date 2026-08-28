/* Cluster Func_80167ac..Func_80167ac extracted from goldensun/asm/rom_15000/rom_15e8c_a_c_c_c_c_c.s.
 *
 * Total .text for this TU = 44 bytes (= 0x002c).
 * Slotted after asm/rom_15000/rom_15e8c_a_c_c_c_c_c_a.o in goldensun/stage1.ld.
 *
 * RECOVERED FROM A PARK BY TYPING, and it was TWO INSTRUCTIONS SHORT before --
 * 13 against the ROM's 15, 14 differing.  The park called it "FORMED POINTER vs
 * REGISTER-OFFSET STORE": the ROM builds a fresh destination pointer for each
 * store (`add r3, r2, r4 / strh r1, [r3]`) where we emitted register-offset
 * stores off one base, which is shorter.
 *
 * No spelling of the pointer arithmetic reached it.  Declaring BOTH SIDES as
 * structs and writing plain field assignments did, on the first try:
 *
 *      p->feae = a->f16;   p->feac = a->f18;   p->fea8 = a->f1a;
 *
 * The offsets are far too large for a store displacement, so with a struct gcc
 * forms an address per store -- which is exactly what the ROM does, and what the
 * arithmetic form would not produce however it was arranged.
 *
 * Second park recovered this way; see the type note in docs/elevation.md.  The
 * lesson is that a park describing an ADDRESSING-MODE difference is a candidate
 * for typing even when it looks like scheduling or allocation.
 */
struct Src {
    unsigned char pad0[0x16];
    unsigned short f16;
    unsigned short f18;
    unsigned short f1a;
};

struct Dst {
    unsigned char pad0[0xea8];
    unsigned short fea8;
    unsigned short feaa;
    unsigned short feac;
    unsigned short feae;
};

extern struct Dst *iwram_3001e8c;

void Func_80167ac(struct Src *a)
{
    struct Dst *p;

    p = iwram_3001e8c;
    p->feae = a->f16;
    p->feac = a->f18;
    p->fea8 = a->f1a;
}

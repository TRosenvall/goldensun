/* Cluster Field_Frost_Target..Field_Frost_Target extracted from
 * goldensun/asm/rom_8a000/rom_97b54_a_c_c_a_c_c.s.
 *
 * Total .text for this TU = 50 bytes (= 0x32).
 * Placed in the run in goldensun/stage1.ld.
 *
 * The targeted form of the field Frost psynergy: if a target actor exists, sets
 * a flag on the caster when its facing byte is non-zero, marks the target, and
 * runs the untargeted routine.
 *
 * THE FACING BYTE IS `signed char`, AND THAT IS WHAT THE SIGN-EXTENSION SAYS.
 * The ROM reads it with `ldrb` and then `lsl #24 / asr #24` -- an unsigned load
 * widened by hand, which is exactly how gcc loads a `signed char` on a target
 * with no `ldrsb` immediate form. Declaring it `unsigned char` drops both
 * shifts; declaring it `char` is implementation-defined and happens to work
 * here, but `signed char` says what the ROM does.
 *
 * The `|= 2` on the target's flag byte is a plain read-modify-write, not a
 * bitfield: the ROM's `mov r3, #2 / orr r3, r2` has no mask, because setting a
 * single bit needs none. A bitfield write would have produced one.
 */

struct A {
    unsigned char pad00[0x23];
    unsigned char f23;
};

struct S {
    unsigned char pad00[0x14];
    struct A *a;
    unsigned char pad18[8];
    unsigned char f20;
    unsigned char pad21[0x14];
    signed char f35;
};

extern struct S *iwram_3001f30;
extern void Field_Frost(void);

void Field_Frost_Target(void)
{
    struct S *s;
    struct A *a;

    s = iwram_3001f30;
    a = s->a;
    if (a == 0)
        return;
    if (s->f35 != 0)
        s->f20 = 1;
    a->f23 |= 2;
    Field_Frost();
}

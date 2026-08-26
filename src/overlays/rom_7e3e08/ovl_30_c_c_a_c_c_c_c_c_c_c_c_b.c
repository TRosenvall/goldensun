/* OvlFunc_957_2008ee0  --  0x02008ee0, cut from the head of
 * goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_c.s; the
 * remaining function follows as ovl_30_c_c_a_c_c_c_c_c_c_c_c_c.o.
 *
 * A four-phase cycle: two words at +0x18 and +0x1c are set from a table indexed
 * by two bits of a counter halfword at +0x64, and the counter then advances
 * modulo sixteen.
 *
 * BUILT WITH ALIAS_CFLAGS (`-fno-strict-aliasing`), and the reason is visible in
 * one instruction. The ROM RE-READS the counter for the increment:
 *
 *     ldrh r3, [r1]  ...  str r3, [r0, #0x18] / str r3, [r0, #0x1c]  ...
 *     ldrh r3, [r1]
 *
 * At `-O2` gcc reads it once and keeps it, because strict aliasing says an
 * `int` store cannot touch an `unsigned short`. With the flag both reads come
 * back and the function drops from 20 differing lines to one.
 *
 * THE LAST LINE WAS THE MASK'S WIDTH. `*p = (*p + 1) & 0xf` pools the 0xf as a
 * HImode constant -- `ldr r2, =0xf` -- where the ROM has `mov r2, #0xf`. An
 * `int` local holding the mask widens it to SImode and gcc uses the immediate.
 * Three other spellings (a named intermediate for `*p + 1`, an explicit
 * `(unsigned short)` cast, the increment split into two statements) all leave
 * the pool load in place; only re-typing the MASK moves it. That is batch 83's
 * width rule in its other direction -- there a named local of the narrow width
 * was needed, here a wide one.
 *
 * `ldr r2, =3` is left alone: the ROM pools that one too.
 */
struct A {
    unsigned char pad00[0x18];
    int f18;
    int f1c;
    unsigned char pad20[0x44];
    unsigned short f64;
};

extern int L4468[] __asm__(".L4468");

void OvlFunc_957_2008ee0(struct A *a)
{
    unsigned short *p;
    int v;
    int m;

    p = &a->f64;
    v = L4468[((short)*p >> 2) & 3];
    a->f18 = v;
    a->f1c = v;
    m = 0xf;
    *p = (*p + 1) & m;
}

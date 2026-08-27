/* Func_8079d1c  --  0x08079d1c
 *
 * Cut out of goldensun/asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a_c.s.
 *
 * Rolls for a weapon's unleash: the chance is the equipment crit bonus plus
 * five times the weapon's own rate, scaled into 16.16 and compared against a
 * 16-bit random number. Returns the move id on success and 1 otherwise.
 *
 * `m->fb * 5` is `lsl #2 / add` -- gcc's own strength reduction, not something
 * the source spells. The `<< 16` before the divide is the fixed-point scale, so
 * the division is by 100 rather than by 0x640000.
 *
 * 0x129 and 0xffff are both pooled because neither is reachable by an eight-bit
 * `mov`; that is ordinary and not the symbol tell.
 *
 * Matched on the first screen.
 */
struct M {
    unsigned char pad00[0xb];
    unsigned char fb;
    unsigned char pad0c[2];
    unsigned short fe;
};

extern struct M *Func_807882c(void *u, int n);
extern int CheckEquipmentCritBoost(void *u);
extern int RPGRandom(void);

int Func_8079d1c(void *u)
{
    struct M *m;
    int v;

    if (*((unsigned char *)u + 0x129) == 0)
        return 1;
    m = Func_807882c(u, 1);
    if (m == 0)
        return 1;
    if (m->fe == 0)
        return 1;
    v = ((CheckEquipmentCritBoost(u) + m->fb * 5) << 16) / 100;
    if (v > (RPGRandom() & 0xffff))
        return m->fe;
    return 1;
}

/* OvlFunc_957_2008d90  --  0x02008d90
 *
 * Cut out of goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_a_c.s.
 *
 * A once-only map repaint gated on a language/variant byte at
 * [iwram_3001f30]+0x35: if that byte is zero, redraw one rectangle, mark actor
 * 0xb, and record the visit with save bit 0x211.
 *
 * A SIGNED-CHAR READ HELD IN AN `int`. The ROM does
 *
 *      add r5, #0x35 / ldrb r5, [r5] / lsl r5, #0x18 / asr r5, #0x18
 *
 * -- an unsigned byte load followed by a 24-bit sign extension, three
 * instructions. Declaring the local `signed char` gives gcc `mov r3, #0 /
 * ldrsb r3, [r5, r3]` instead, which is two, and costs a second load later
 * because the narrow type will not stay in one register across the call.
 *
 * The local has to be an `int` holding a `signed char` read. That is what
 * produces the ldrb-plus-shifts pair AND keeps the value in r5 across
 * __MapActor_GetActor for the store at the end. (Both `*(signed char *)p` and
 * `(signed char)*(unsigned char *)p` work once the local is `int`; the type of
 * the LOCAL is what decides it, not the cast at the load.)
 *
 * That the value crosses a call is exactly the discriminator from batch 95: a
 * named local is forced here because a bare re-read would have to happen twice.
 *
 * THE BASE POINTER IS READ BEFORE THE CALL. `ldr r5, [r3]` sits ahead of
 * `bl __MapActor_GetActor` in the ROM, so the source loads iwram_3001f30 into a
 * local first and indexes it afterwards. Written inline it lands after the call
 * and costs the whole prologue.
 *
 * The two stack arguments are 0x49 and 0x11 -- two different values in the two
 * slots, so both are named locals; batch 95's rule.
 */
struct A {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x55 - 0x24];
    unsigned char f55;
};

extern char *iwram_3001f30;
extern struct A *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_957_2008d90(void)
{
    struct A *a;
    int v;
    char *q;
    int e, f;

    q = iwram_3001f30;
    a = __MapActor_GetActor(0xb);
    v = *(signed char *)(q + 0x35);
    if (v == 0) {
        e = 0x49;
        f = 0x11;
        __Func_8010704(0x4c, 0x10, 1, 1, e, f);
        if (a != 0) {
            a->f55 = 2;
            a->f23 = v;
        }
        __SetFlag(0x211);
    }
}

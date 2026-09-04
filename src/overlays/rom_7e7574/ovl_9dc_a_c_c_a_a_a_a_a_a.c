// fakematch
/* OvlFunc_959_2008bac  --  0x02008bac
 *
 * From goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_a_a_a_a.s, which held this function alone.
 *
 * Thirteenth member of the "arg_interleave_flat" class, and with it that class is
 * CLOSED -- all fourteen elevated, every one on its first screen. See
 * src/non_matching/overlays/arg_interleave_flat.c.
 *
 * The blocker throughout: the ROM writes one argument INSIDE another register's
 * two-instruction build, and gcc emits it before or after the whole block.
 * Pinning the argument registers and assigning them in the ROM's order places
 * it, because a pin names the hard register and so decides placement at the
 * assignment rather than through a dominating basic block.
 *
 * Two pin sites, and the second runs its registers in the order r2, r1, r0 --
 * the ROM's own order, not a normalised one. Read each call off the listing.
 */

extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void OvlFunc_959_2008b4c(void);

void OvlFunc_959_2008bac(void)
{
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 0x80;
        q2 = 0x80;
        q0 = 0xc;
        q1 <<= 9;
        q2 <<= 8;
        __MapActor_SetSpeed(q0, q1, q2);
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q2 = 0xbc;
        q1 = 0xf8;
        q2 <<= 1;
        q0 = 0xc;
        __MapActor_TravelTo(q0, q1, q2);
    }
    __MapActor_WaitMovement(0xc);
    __PlaySound(0xd7);
    __CutsceneWait(0x3c);
    OvlFunc_959_2008b4c();
    __SetFlag(0x943);
}

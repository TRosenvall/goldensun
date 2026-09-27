/* OvlFunc_927_2008d90  --  0x02008d90, was asm/overlays/rom_7b4558/ovl_30_a_c_c_a_a.s
 * (this function alone), so it converts whole. Matched from scratch.
 *
 * FAKEMATCH -- two register pins at the __MapActor_SetSpeed call (q0 = slot,
 * q1 = 0x30000, bound to r0/r1 in that order), booked in fakematch.txt. Every
 * unpinned spelling left 2 encodings: `lsl r1,#10` scheduled before
 * `mov r0,r6`. Literal forms, named locals, do{}while(0) and unsigned
 * parameters were all inert; pinning all three matches only in q0,q1,q2 order.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_8092158(int a, int b, int c);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_927_2008d90(int slot, int x, int y, int z)
{
    unsigned char *a;

    a = __MapActor_GetActor(slot);
    __Func_8092b08(slot, 1);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");

        q0 = slot;
        q1 = 0x30000;
        __MapActor_SetSpeed(q0, q1, 0x18000);
    }
    __PlaySound(0x98);
    *(int *)(a + 0x28) = z;
    *(int *)(a + 0x48) = 0x8000;
    *(int *)(a + 0x44) = 0;
    __Actor_SetSpriteFlags(a, 0);
    __Func_8092158(slot, x, y);
    __MapActor_SetPos(slot, x << 16, y << 16);
    __Actor_SetSpriteFlags(a, 1);
    *(int *)(a + 0x48) = 0x10000;
}

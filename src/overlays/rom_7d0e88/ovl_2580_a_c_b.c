/* Cluster OvlFunc_947_200a63c..OvlFunc_947_200a63c extracted from goldensun/asm/overlays/rom_7d0e88/ovl_2580_a_c.s.
 *
 * Slotted between ovl_2580_a_c_a.o and the rest of the overlay.
 *
 * Sets an actor's animation, hangs a callback off it, and hands its tile
 * coordinates to __Func_8010704.
 *
 * The stack-arg pair here is COMPUTED rather than constant -- the actor's x and
 * z shifted down 20 to tiles -- which is the first instance of that in this
 * tree. The pair still has to be two named locals stored before the call; what
 * changes is only where the values come from.
 *
 * The flag id is `slot + 0x1f5`, built with a three-operand `add r0, r6, r3`,
 * so the slot survives to be passed to __MapActor_SetBehavior at the end.
 */
extern void *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __Actor_SetAnim(void *a, int anim);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetBehavior(int slot, void *data);
extern void OvlFunc_947_200a0b8(void);
extern unsigned char OvlData_947_200ad64[];

void OvlFunc_947_200a63c(int slot)
{
    unsigned char *a;
    int x;
    int z;

    a = (unsigned char *)__MapActor_GetActor(slot);
    if (!__GetFlag(slot + 0x1f5))
        return;
    __Actor_SetAnim(a, 5);
    *(void **)(a + 0x6c) = (void *)OvlFunc_947_200a0b8;
    x = *(int *)(a + 8) >> 20;
    z = *(int *)(a + 0x10) >> 20;
    __Func_8010704(0x14, 0xe, 1, 1, x, z);
    __MapActor_SetBehavior(slot, OvlData_947_200ad64);
}

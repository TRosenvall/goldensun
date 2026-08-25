/* Cluster OvlFunc_932_200aa10..OvlFunc_932_200aa10 extracted from goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a.s.
 *
 * Total .text for this TU = 56 bytes (= 0x38).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a.o and
 * asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_c.o in
 * goldensun/overlays/rom_7b9cb4/overlay.ld.
 *
 * Matched on the first screen.  The read-modify-write on byte +0x09 of the
 * actor is ONE variable throughout: `z` is 0 for the strb at +0x55, then
 * `z - 0xd` gives the mask 0xfffffff3, which is the ROM's `sub r3, #0xd` on a
 * register still holding zero.  Same shape as
 * src/overlays/rom_79c0c4/ovl_30_c_c_c_c_a.c.
 *
 * r0 still holds the actor at the first call, which is why __Func_80929d8
 * takes `a` and not the pointer just loaded into r1.
 */
extern void __Func_80929d8(void *a, int n);
extern void __Actor_SetSpriteFlags(void *a, int n);

void OvlFunc_932_200aa10(unsigned char *actor)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *c;
    int z;
    int t;
    int m;
    int w;

    a = actor;
    p = a + 0x55;
    z = 0;
    *p = z;
    c = *(unsigned char **)(a + 0x50);
    t = c[9];
    z = z - 0xd;
    z = z & t;
    m = 4;
    z = z | m;
    c[9] = z;
    __Func_80929d8(a, 3);
    __Actor_SetSpriteFlags(a, 0);
    w = 0x4ccc;
    *(int *)(a + 0x18) = w;
    *(int *)(a + 0x1c) = w;
}

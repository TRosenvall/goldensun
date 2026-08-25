/* OvlFunc_968_20088c8 extracted from goldensun/asm/overlays/rom_7f2f14/ovl_30_a_c_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * A proximity test on two actors: set bit 1 of the flags byte at +0x23, then
 * clear it again if the caller`s actor is close enough to slot 0 in both
 * z and y. The two comparisons are asymmetric on purpose -- `<` on z, `<=` on
 * the y sum -- and the ROM says which by `bge` against `bgt`.
 *
 * gcc PARKS THE FLAGS-BYTE ADDRESS IN r12 across the arithmetic. Nothing in
 * this file asks for that; it falls out of register pressure, and it is the
 * only place in the elevated corpus where r12 is used as a scratch
 * (tools/find_solved_shape.py finds no other). The C is written the obvious way
 * and gcc does it unprompted -- worth recording so a future reader does not
 * hunt for the construct that "causes" it.
 *
 * Twin of src/overlays/rom_7ed0a0/ovl_30_a_a_c_a_c_c_b.c.
 */
extern void *__MapActor_GetActor(int slot);

int OvlFunc_968_20088c8(void *p)
{
    unsigned char *q;
    unsigned char *a;
    unsigned char *b;
    int v;
    int d;

    b = (unsigned char *)p;
    a = (unsigned char *)__MapActor_GetActor(0);
    q = b + 0x23;
    v = *q | 2;
    *q = v;
    if (*(int *)(a + 0x10) < *(int *)(b + 0x10)) {
        d = *(int *)(b + 0x10) - *(int *)(a + 0x10) + (0x80 << 11);
        if (*(int *)(a + 0xc) <= *(int *)(b + 0xc) + d) {
            v &= 0xfd;
            *q = v;
        }
    }
    return 0;
}

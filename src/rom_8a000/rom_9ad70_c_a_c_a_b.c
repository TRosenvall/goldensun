/* Func_809b11c (PlaceAbilityTargets)  --  0x0809b11c, split out of
 * asm/rom_8a000/rom_9ad70_c_a_c_a.s; the hand-written Field_Retreat that
 * followed it stays in rom_9ad70_c_a_c_a_c.s.
 *
 * Matched from scratch. Two things decided it:
 *  - The two Random() terms must stay in ONE expression; assigning the first
 *    to a local before the line costs 19 encodings (a register swap).
 *  - The state-1 decrement's `ldrb / sub #1 / strb` needs a `signed char`
 *    lvalue; through plain or unsigned char every spelling gives
 *    `add r3,#255`.
 */
extern int *iwram_3001f30;
extern int iwram_3001e40;
extern int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern void Func_80974d8(int *v);
extern int Func_809ba34(char *e);
extern void Func_809bb34(char *e);
extern void _PlaySound(int id);

void Func_809b11c(char *e)
{
    signed char *p;
    int *o;
    int v[3];
    int k;
    int r;

    p = (signed char *)(e + 0x40);
    o = (int *)iwram_3001f30[4];
    k = *p;
    if (k == 0) {
        v[0] = o[2];
        v[1] = o[3];
        v[2] = o[4];
        Func_80974d8(v);
        *(int *)(e + 4) = v[0];
        *(int *)(e + 8) = v[2] + (0x80 << 12);
        *(int *)(e + 0x18) = *(int *)(e + 8);
        *(int *)(e + 0x14) = *(int *)(e + 4);
        v[2] = *(int *)(e + 0x18);
        v[0] = *(int *)(e + 0x14);
        r = ((unsigned int)(Random() << 13) >> 16) - ((unsigned int)(Random() << 13) >> 16) + 0xc000;
        vec3_translate(0xf0 << 15, r, v);
        *(int *)(e + 0xc) = v[0];
        *(int *)(e + 0x10) = v[2];
        *(int *)(e + 0x24) = 0xa0 << 11;
        *(int *)(e + 0x20) = 0xa0 << 11;
        *(char *)(e + 0x42) = k;
        *p = *p + 1;
        if (iwram_3001e40 & 2)
            _PlaySound(0xf6);
    } else if (k == 1) {
        if (Func_809ba34(e) == 0)
            *p -= 1;
    } else if (k == 2) {
        if (Func_809ba34(e) == 0)
            Func_809bb34(e);
    }
}

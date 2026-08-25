/* Anim_Attack -- NOT MATCHING. 5 of 39, same length.
 *
 * Source asm: goldensun/asm/rom_c9000/rom_e3958_c_c_c_c.s
 *
 * Blocker: register allocation, and nothing else. Identical instruction
 * sequence with two registers exchanged:
 *
 *     rom    ldr r3,[r5] / mov r1,r3 / sub r1,#0x64 / cmp r1,#0x23 ... cmp r3,#0xc7
 *     ours   ldr r2,[r5] / mov r3,r2 / sub r3,#0x64 / cmp r3,#0x23 ... cmp r2,#0xc7
 *
 * TRIED: the two locals declared in the other order; the value read into the
 * unsigned temporary first and copied to the signed one. Both give the
 * identical 5.
 *
 * WHAT IS RIGHT: the range check is a SUBTRACTION into an unsigned local
 * followed by one compare -- `t = v - 0x64; if (t <= 0x23)` -- reproducing the
 * ROM's `sub`/`cmp`/`bhi`. The second bound is a separate SIGNED compare
 * (`ble`), so the two must not be fused; see batch 55 on compound conditions.
 *
 * NEXT: one of the 60 parks now known to sit within six instructions
 * (tools/near_parks.py). Nothing specific to try.
 */
extern void *galloc_ewram(int tag, int size);
extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void BaseAnim_SpecialAttack(void *p);
extern void Anim_CriticalHit(void *p);
extern void BaseAnim_Attack(void *p);

void Anim_Attack(int *p)
{
    int v;
    unsigned int t;

    galloc_ewram(0x29, 0x60e);
    galloc_iwram(0x27, 0x782c);
    galloc_iwram(0x28, 0x80 << 7);
    v = *p;
    t = v - 0x64;
    if (t <= 0x23)
        BaseAnim_SpecialAttack(p);
    else if (v > 0xc7)
        Anim_CriticalHit(p);
    else
        BaseAnim_Attack(p);
    gfree(0x28);
    gfree(0x27);
    gfree(0x29);
}

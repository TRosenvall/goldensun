/* OvlFunc_969_200d9f0  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7f6e64/ovl_314_c_c.s
 * Best screen: 9 instructions in disagreeing regions, of 27 (rom 27, ours 26).
 *
 * BLOCKER CLASS: register allocation, twice over, and the second is an elided
 * copy.
 *
 *  1. The byte and the word are loaded into SWAPPED registers:
 *
 *         rom   ldrb r2, [r5] / ldr r3, [r0, #0x4c]   ... add r3, r2
 *         ours  ldrb r3, [r5] / ldr r2, [r0, #0x4c]   ... add r2, r3
 *
 *     Both are destructive on the word, so the shape is right and only the
 *     names differ -- but every dependent instruction inherits it.
 *
 *  2. After the call the ROM copies the reloaded byte before testing it,
 *     `ldrb r3, [r5] / mov r2, r3 / cmp r2, #0`, and increments the ORIGINAL
 *     (`add r3, #1`). gcc uses one register for both and drops the copy.
 *
 * WHAT WAS TRIED
 *   1. Source order matching the ROM, byte load before word load (kept below).
 *   2. The two loads swapped, on the theory that gcc assigns registers in
 *      source order.  BYTE-IDENTICAL, still 9 of 27 -- it does not.
 *
 * The elided copy is the same shape as src/non_matching/rom_b5000/80bf54c.c,
 * and the same conclusion applies: the copy is a symptom of register pressure
 * in the original, not of how the source is spelled. Two named locals do not
 * produce it when nothing else competes for the register.
 */
extern void OvlFunc_969_200d688(void *a);

void OvlFunc_969_200d9f0(void *actor)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *q;
    unsigned int x;
    int v;
    int t;
    int u;

    a = (unsigned char *)actor;
    p = a;
    p += 0x63;
    if (*p == 0)
        return;
    q = a;
    q += 0x62;
    x = *q;
    v = *(int *)(a + 0x4c);
    x >>= 2;
    x <<= 16;
    v = v + x;
    *(int *)(a + 0xc) = v;
    OvlFunc_969_200d688(a);
    t = *q;
    u = t;
    if (u == 0)
        return;
    if ((unsigned int)u > 0x1f)
        return;
    t = t + 1;
    *q = t;
}

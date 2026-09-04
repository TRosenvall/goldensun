/* OvlFunc_935_20089c0  --  0x020089c0  [asm/overlays/rom_7bf5a8/ovl_2e0_c_c_a_c_c_c_a.s]
 *
 * NOT MATCHING. Best 61 of 94, ours 93 lines. The .s holds this function alone
 * with no data tail, so no split is needed when it is finished.
 *
 * A per-frame handler: bump two counters held in `.lcomm` cells, compare one
 * against a table, then sweep four actors resetting any that have gone out of
 * range. Three cells reached with the tree's asm-renamed extern idiom.
 *
 * THE BLOCKER IS A REGISTER THE ROM SPENDS AND OURS DOES NOT. The ROM pushes
 * r5, r6 and r7 and keeps the fetched actor in r5 across both the compare arm
 * and the loop; ours pushes only r6 and r7 and puts the actor wherever it can,
 * which displaces everything after instruction 17.
 *
 * MEASURED, three forms:
 *
 *     plain, no pins on the actor                    63 differ, 93 lines
 *     actor pinned to r5                             70 differ, 92 lines
 *     table base and index named, actor unpinned     61 differ, 93 lines
 *
 * PINNING THE ACTOR MAKES IT WORSE, and the reason is instructive: the ROM
 * copies the fetched pointer with `mov r5, r0` and then stores through r5,
 * while a pin lets gcc store through r0 directly and drop the copy -- the
 * function comes out a line SHORT. The pin removes an instruction the ROM
 * spends, which is the opposite of what a pin usually does.
 *
 * WHAT DID LAND is the table access: `ldr r2, =.L2214 / lsl r3, r0, #2 /
 * ldr r2, [r2, r3]` wants base and index in named registers, and naming them
 * takes 63 to 61 and fixes that pair.
 *
 * TWO ONE-REGISTER-TWO-ROLES PAIRS ARE PRESENT AND ARE BELIEVED CORRECT in the
 * candidate below: r6 carries the byte read from the actor and then the loop
 * counter, and r7 carries a cell pointer and then the zero stored through the
 * swept actors. Both are spelled as two `register` declarations naming one
 * register, the idiom this tree has now used four times. They are not the
 * residue.
 *
 * NEXT: the question is how to keep the `mov r5, r0` copy that the pin
 * destroys. The copy exists because the ROM's actor pointer is live across a
 * later call while r0 is not; a pin says WHERE a value lives and cannot say
 * that it must be COPIED rather than used in place. Compare
 * src/non_matching/ovl_7b9cb4/20086dc.c, parked the same round, where the pin
 * was necessary and the cost was a loop guard -- the two functions bracket what
 * a pin can and cannot buy, and are worth reading together.
 */
extern int L2224 __asm__(".L2224");
extern int L2228 __asm__(".L2228");
extern int L2214 __asm__(".L2214");
extern int OvlFunc_935_2008944(int a);
extern void OvlFunc_935_2008b8c(void);

extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __PlaySound(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);

void OvlFunc_935_20089c0(void)
{
    register int f __asm__("r6");
    register int *d __asm__("r7");
    unsigned char *s;
    int *c;
    int t;
    int k;

    f = __MapActor_GetActor(0xa)[0x5b];
    if (f == 0) {
        c = &L2224;
        t = *c;
        t += 1;
        *c = t;
        if (t > 0xbe)
            *c = f;
        d = &L2228;
        k = *d;
        {
            register unsigned char *tb __asm__("r2");
            register int ix __asm__("r3");
            tb = (unsigned char *)&L2214;
            ix = k << 2;
            if (*(int *)(tb + ix) == *c) {
            s = __MapActor_GetActor(k + 0xb);
            *(int *)(s + 0x48) = 0xa3d;
            t = *d;
            t += 1;
            *d = t;
            if (t > 3)
                *d = f;
            }
        }
        {
            register int i __asm__("r6");
            register int z __asm__("r7");
            i = 0;
            z = 0;
            do {
                s = __MapActor_GetActor(i + 0xb);
                if (*(int *)(s + 0x28) >= 0 && *(int *)(s + 0xc) <= 0xffff) {
                    OvlFunc_935_2008b8c();
                    *(int *)(s + 0xc) = 0xff << 16;
                    *(int *)(s + 0x48) = z;
                    *(int *)(s + 0x28) = z;
                    s[0x5b] = z;
                    __PlaySound(0x6a);
                }
                i++;
            } while (i <= 3);
        }
        if (OvlFunc_935_2008944(0xa) != 0) {
            register int p0 __asm__("r0");
            __MapActor_SetAnim(0xa, 1);
            p0 = 0x207;
            if (__GetFlag(p0) == 0) {
                p0 = 0x207;
                __SetFlag(p0);
                __PlaySound(0xcc);
            } else {
                __PlaySound(0x6a);
            }
        }
        if (OvlFunc_935_2008944(9) != 0)
            __PlaySound(0x6a);
    }
}

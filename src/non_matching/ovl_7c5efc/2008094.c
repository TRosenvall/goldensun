/* OvlFunc_941_2008094 -- NOT MATCHING. 9 of 30, and ours is one instruction SHORT.
 *
 * Source asm: goldensun/asm/overlays/rom_7c5efc/ovl_30_c_a_c_c_a_c_a.s
 *
 * Blocker: gcc CHAINS THE TWO FIELD ADDRESSES and the ROM builds both from the
 * base.
 *
 *     rom    mov r1,r0 / mov r2,#1 / add r1,#0x23 / strb r2,[r1]
 *            mov r2,r0 / mov r3,#0 / add r2,#0x55 / strb r3,[r2]
 *     ours   ... add r2,#0x23 / strb / add r2,#0x32 ...
 *
 * 0x23 + 0x32 is 0x55, so gcc reuses the first address and adds the difference,
 * saving the second `mov r2, r0`. That is the "ours is shorter" signature from
 * batch 55 in its CROSS-JUMPING form -- gcc found something cheaper and the
 * source cannot un-find it.
 *
 * TRIED, both identical at 9 of 30:
 *   two named pointer locals, each copied from the base and advanced
 *   the array form `a[0x23] = 1; a[0x55] = 0;`
 *
 * NEXT: nothing at the expression level. The two stores are independent in the
 * source and gcc relates them anyway.
 */
extern void *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);

void OvlFunc_941_2008094(void)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *q;
    int m;
    int n;

    a = (unsigned char *)__MapActor_GetActor(9);
    if (a != 0) {
        p = a;
        p += 0x23;
        *p = 1;
        q = a;
        q += 0x55;
        *q = 0;
    }
    m = 8;
    n = 0x20;
    __Func_8010704(7, 0x20, 1, 1, m, n);
    __SetFlag(0x81 << 2);
}

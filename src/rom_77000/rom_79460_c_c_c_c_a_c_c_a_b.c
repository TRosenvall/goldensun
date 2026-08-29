/* Cluster Func_8079c8c..Func_8079c8c extracted from goldensun/asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a.s.
 *
 * Slotted between rom_79460_c_c_c_c_a_c_c_a_a.o and the rest of stage1.ld.
 *
 * BRANCH POLARITY. The non-null path is the FALL-THROUGH: written as
 * `if (r == 0) return 4; return r->f14;` gcc emits `bne` and the two arms swap,
 * three positions out. Written `if (r != 0) return r->f14; return 4;` it emits
 * the ROM's `beq`. The ROM always says which arm falls through.
 */
extern void *GetUnit(void);
extern int Func_80798b4(void);
extern void *Func_807882c(void *u, int n);

int Func_8079c8c(void)
{
    unsigned char *u;
    void *r;

    u = (unsigned char *)GetUnit();
    if (*(u + 0x129) == 0)
        return Func_80798b4();
    r = Func_807882c(u, 1);
    if (r != 0)
        return *(int *)((unsigned char *)r + 0x14);
    return 4;
}

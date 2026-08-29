/* Cluster Func_8096af0..Func_8096af0 extracted from goldensun/asm/rom_8a000/rom_944ec_a_c_c_a_a.s.
 *
 * Slotted between rom_944ec_a_c_c_a_a_a.o and rom_944ec_a_c_c_a_a_c.o in
 * goldensun/stage1.ld.
 *
 * A three-way dispatch on a mode halfword.
 *
 * THE SELECTOR IS UNSIGNED AND THE LOAD IS SIGNED. The ROM does `ldrsh` and
 * then compares with `bhi`, so the value is read as a signed halfword and
 * WIDENED TO UNSIGNED before the switch. A plain `short v` gives `bgt` and is
 * one instruction out; `unsigned int v = (unsigned int)*(short *)...` gives the
 * ROM. Same tell as OvlFunc_881_200b448 -- the branch condition in the chain
 * reports the selector type -- with the wrinkle that the load and the compare
 * disagree, which a single variable declaration cannot express.
 */
extern unsigned int iwram_3001f30;
extern void Func_80984c0(void);
extern void Func_8099738(void);
extern void Func_809b648(void);

void Func_8096af0(void)
{
    unsigned char *p;
    unsigned int v;

    p = (unsigned char *)iwram_3001f30;
    v = (unsigned int)*(short *)(p + 0x1e);
    switch (v) {
    case 8:    Func_80984c0(); break;
    case 0xa:  Func_8099738(); break;
    case 0x10: Func_809b648(); break;
    }
}

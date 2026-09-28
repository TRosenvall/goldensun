/* OvlFunc_971_2008148 -- 0x02008148, the only function in
 * asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_a_c.s, which carries no data section.
 * Whole-file conversion: no split, and overlay.ld's `.o(.text)` line is unchanged.
 *
 * 400 bytes, 167 encodings and 28 relocations identical.
 *
 * SHIM, BOOKED: one `__asm__ ("" : "+r" (f));` below.  IT GOES ON THE FIRST USE OF
 * THE REPEATED POOL CONSTANT, NOT THE SECOND, and the pass is cse2.  The whole
 * residue was `ldr r5,=0x201 / mov r0,r5 ... mov r0,r5` where the ROM has
 * `ldr r0,=0x201` twice.  Barrier on the FIRST site: exact.  Barrier on the second
 * site: 71 differing, i.e. completely inert.  The mechanism is pinned from the other
 * side too -- `-fno-rerun-cse-after-loop` alone makes the stream identical to the
 * ROM, while `-fno-gcse` and `-fno-cse-follow-jumps` leave it (and it cannot be gcse:
 * want_to_gcse_p returns 0 for CONST_INT).
 *
 * BECAUSE A SOURCE ROUTE EXISTS, NO PER-FILE CSE FLAG ROW IS WARRANTED HERE -- and
 * OvlFunc_959_2009528, where such a row looked necessary, should be re-screened with
 * the barrier on its FIRST site before any row is added for it.
 *
 * `extern int L1f4c __asm__(".L1f4c");` is a name binding, not a pin, and it needs no
 * linker alias: GAS emits `R_ARM_ABS32 .L1f4c` straight from the C declaration and it
 * links because the defining ovl_30_c_c_c_c_c.s already has `.global .L1f4c`.  So
 * label.sym / _TBL_ aliases are needed only where the label is NOT already global;
 * the tree already relies on this at
 * src/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_a_b.c:9.
 *
 * Other load-bearing choices: `short *sp` declared BEFORE `int w` (4 of 167 the other
 * way round) -- the halfword `int` carrier must come after the address; the `int w`
 * carriers at all four halfword stores (103 of 167, 179 instructions, without);
 * and the `__GetFlag(0x303) == 0` then-arm order (25 without).
 *
 * Inert, and therefore not written: `int (*fp)()` vs `void (*fp)()`; `L1f4c++` vs
 * `L1f4c = L1f4c + 1`; a descending vs ascending four-iteration loop; a PIN1 on the
 * second 0x201 site.
 */
struct Actor {
    unsigned char pad00[0x10];
    int f10;
};

extern char *iwram_3001ebc;
extern int L1f4c __asm__(".L1f4c");
extern unsigned int ewram_2002024;
extern void Func_80008d4(void *dst, int len);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern int OvlFunc_971_200808c(int i);
extern void OvlFunc_971_2008128(int n);

int OvlFunc_971_2008148(void)
{
    char *p;
    char *q;
    int ret;
    int i;
    int f;
    void (*fp)(void *, int);

    p = iwram_3001ebc;
    ret = 1;
    if (__MapActor_GetActor(0)->f10 > (0xe0 << 16))
        __ClearFlag(0xc1 << 2);
    if (*(short *)(p + (0xc1 << 1)) != 2) {
        OvlFunc_971_200808c(0);
        if (__GetFlag(0x303) == 0) {
            L1f4c = L1f4c + 1;
            if (L1f4c > 0x19) {
                fp = Func_80008d4;
                q = (char *)&ewram_2002024;
                for (i = 0; i <= 3; i++) {
                    fp(q, 0x14);
                    q += 0x18;
                }
                L1f4c = 0;
                OvlFunc_971_2008128(4);
            }
        } else {
            L1f4c = 0;
        }
        if (L1f4c == 0) {
            if (OvlFunc_971_200808c(0) != 0
                && (OvlFunc_971_200808c(1) != 0 || OvlFunc_971_200808c(2) != 0)) {
                __SetFlag(0x201);
                if (__GetFlag(0x202) != 0) {
                    short *sp = (short *)(p + (0xc1 << 1));
                    int w = 1;
                    *sp = w;
                }
                ret = 1;
            } else {
                __ClearFlag(0x201);
                ret = 0;
            }
        }
        if (__GetFlag(0x201) != 0 && __GetFlag(0x202) != 0
            && __GetFlag(0x80 << 2) == 0) {
            short *sp = (short *)(p + (0xc1 << 1));
            int w = 1;
            *sp = w;
        }
    }
    f = 0x201;
    __asm__ ("" : "+r" (f));
    if ((__GetFlag(f) != 0 || __GetFlag(0x202) != 0)
        && __GetFlag(0x173) == 0
        && OvlFunc_971_200808c(0) == 0
        && L1f4c > 0x18) {
        { short *sp = (short *)(p + (0xc1 << 1)); int w = 2; *sp = w; }
        __SetFlag(0x205);
        __ClearFlag(0x201);
        __ClearFlag(0x202);
        OvlFunc_971_2008128(4);
    }
    if (__GetFlag(0x205) != 0) {
        short *sp = (short *)(p + (0xc1 << 1));
        int w = 2;
        *sp = w;
    }
    return ret;
}

/* Cluster OvlFunc_common1_1fb4..OvlFunc_common1_1fb4 extracted from goldensun/asm/overlays/common/common1_c_a_c_c.s.
 *
 * Slotted between common1_c_a_c_c_a.o and the rest of the overlay.
 *
 * THE 1 STORED INTO THE FIRST TWO HALFWORDS IS A NAMED int, and both defects it
 * causes come from the same omission. As the literal `1`, gcc-2.96 puts the
 * constant in a LITERAL POOL, loads it with `ldrh`, and needs a `b` over the
 * pool -- 30 instructions against 28. This is the inverted narrow_constant tell
 * from batch 48 (Func_808e118) and batch 50 (Func_8091858): where gcc pools
 * what the ROM builds with a `mov`, the source had a variable.
 *
 * THE ZERO STORED INTO THREE FIELDS IS __GetFlag'S RETURN VALUE, not a fresh
 * constant. The ROM reuses r0 -- known to be zero on that path because the `if`
 * tested it -- rather than building one. Same reading as OvlFunc_939_2008c10 in
 * batch 45.
 *
 * The store order is 0, 2, 8, 4, 6 -- the +8 field written before +4 and +6.
 * That is what the ROM does and it is not tidied.
 */
extern unsigned char *iwram_3001f3c;
extern unsigned short ewram_2001000[];
extern void *__GetFile(int id);
extern void __DecompressLZ(void *src, void *dst);
extern int __GetFlag(int id);
extern void OvlFunc_common1_1928(void);

void OvlFunc_common1_1fb4(int arg)
{
    unsigned char *p;
    unsigned short *q;
    int f;
    int v;
    int one;

    p = iwram_3001f3c;
    q = ewram_2001000;
    __DecompressLZ(__GetFile(arg), p + 0xf0);
    f = __GetFlag(0x109);
    if (!f) {
        one = 1;
        q[0] = one;
        q[1] = one;
        v = *(unsigned short *)(p + 0xe0);
        q[4] = f;
        q[2] = v;
        q[3] = f;
    }
    __StartTask(OvlFunc_common1_1928, 0xc85);
}

/* Func_80167ac  [rom_15000]
 * Source asm: goldensun/asm/rom_15000/rom_15e8c_a_c_c_c.s
 *
 * Parked: logic faithful, does NOT byte-match (endgame permuter seed).
 * Candidate: tools/runs/run_20260607T010203Z/Func_80167ac-iter-1.c
 * TODO(residual): reg-alloc/scheduling divergence (register swap / op-order); logic correct. Permuter seed.
 */
extern unsigned int iwram_3001e8c;

/* r0 = record. Copies three saved style values out of the record into the
 * global text-style fields:
 *     [r0+0x16] -> +0xEAE      [r0+0x18] -> +0xEAC      [r0+0x1A] -> +0xEA8
 * These are exactly the three fields Func_173ac resets to 0x0F, 0 and 0x0A, so
 * this is "restore the style this record was created with".
 */
void Func_80167ac(int a)
{
    unsigned char *base = (unsigned char *)iwram_3001e8c;
    *(unsigned short *)(base + 0xeae) = *(unsigned short *)((char *)a + 0x16);
    *(unsigned short *)(base + 0xeac) = *(unsigned short *)((char *)a + 0x18);
    *(unsigned short *)(base + 0xea8) = *(unsigned short *)((char *)a + 0x1a);
}

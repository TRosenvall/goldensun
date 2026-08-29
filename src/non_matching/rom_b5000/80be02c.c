/* Func_80be02c  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_b5000/rom_bbb0c_a_c_a_c.s
 * Best screen: 16 instructions in disagreeing regions, of 30 (streams same length).
 *
 * BLOCKER CLASS: register allocation. The control flow, the instruction
 * sequence and the loop shape all match -- read the streams side by side and
 * every mnemonic lines up. The count is high because r1/r2/r3 are permuted
 * throughout and each permuted line counts.
 *
 * The root of the permutation is at the top:
 *
 *      rom   ldr r1, [r3, #0x0]   /  mov r3, #0x80   <- global loaded FIRST
 *      ours  mov r2, #0x80        /  ldr r3, [r3, #0x0]
 *
 * WHAT WAS TRIED
 *  1. The global assigned before the constant, which is the ROM's order
 *     (kept below). 16 of 30.
 *  2. The constant assigned before the global. BYTE-IDENTICAL.
 *
 * That is worth recording, because the statement-order lever is real and DID
 * fix OvlFunc_908_20084c8 and OvlFunc_968_2008558 in batch 61. It does not
 * reach this one in either direction, so the ordering gcc chooses here is not
 * coming from the source.
 *
 * The ROM also recomputes `0x80 << 4` and its add a second time for the poll
 * loop's pointer, where gcc reuses the first. Two named pointer locals are
 * already used below; gcc CSEs them anyway, the same as
 * src/non_matching/rom_a1000/80ad69c.c.
 *
 * The do/while is correct as written -- the ROM's `bne .Lbe04c` is a back edge
 * and this spelling reproduces it (see docs/elevation.md).
 */
extern unsigned char *iwram_3001e74;
extern void Func_80bd898(void);
extern void StopTask(void (*fn)(void));
extern void Func_80bdfec(void);
extern void WaitFrames(int n);

void Func_80be02c(void)
{
    unsigned char *p;
    unsigned int k;
    int *q;
    int *r;
    int v;

    p = iwram_3001e74;
    k = 0x80 << 4;
    q = (int *)(p + k);
    v = *q;
    if (v == 0) {
        v = 1;
        *q = v;
    }
    if (v == 4)
        goto done;
    k = 0x80 << 4;
    r = (int *)(p + k);
    do {
        WaitFrames(1);
        v = *r;
    } while (v != 4);
done:
    StopTask(Func_80bd898);
    Func_80bdfec();
}

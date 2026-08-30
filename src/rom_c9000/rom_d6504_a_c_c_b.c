/* Cluster Anim_Func..Anim_Func extracted from
 * goldensun/asm/rom_c9000/rom_d6504_a_c_c.s.
 *
 * Three tagged allocations, a dispatch through a function-pointer table, and
 * the three frees in reverse order.  Two readings settled it.
 *
 *   THE INDIRECT CALL IS ORDINARY C.  `bl _call_via_r3` is not a hand-written
 *   thunk to work around -- it is what gcc emits for a call through a function
 *   pointer under -mthumb-interwork, and four already-elevated files produce it
 *   from plain C (tools/whodoesthis.py finds them).  So the table is declared
 *   `void (*Data_80ee2b4[])(int *)` and called as `Data_80ee2b4[k - 1](p)`; the
 *   `lsl r2, #2 / sub r2, #4` is gcc folding the -1 into the scaled index
 *   rather than biasing the base.
 *
 *   THE TABLE ADDRESS IS COMPUTED BEFORE THE FIELD IS READ.  Written as
 *   `k = *p; *(int **)(iwram_3001eec + 0x7828) = p;` gcc interleaves them --
 *   it loads the 0x7828 offset, loads k, then adds -- and uses two registers
 *   where the ROM uses one.  Naming the destination first
 *   (`q = (int **)(iwram_3001eec + 0x7828); k = *p; *q = p;`) finishes the
 *   address before the load and lets r2 serve both, which is the ROM's
 *   `ldr r2, =0x7828 / add r3, r2 / ldr r2, [r5]`.  3 differing -> exact.
 *
 *   Writing the store BEFORE the read matches too, byte for byte, but it
 *   reorders the source against the ROM's own sequence for no gain; the named
 *   address keeps the read first, as the ROM has it.
 *
 * `p[6] = 0` on the zero arm reuses the register the comparison already proved
 * zero, which is why the ROM has `str r2, [r5, #0x18]` and not a fresh mov.
 */
extern unsigned char *iwram_3001eec;
extern void (*Data_80ee2b4[])(int *p);

extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);

void Anim_Func(int *p)
{
    int k;
    int **q;

    galloc_iwram(0x29, 0x302);
    galloc_iwram(0x27, 0x782c);
    galloc_iwram(0x28, 0x80 << 7);
    q = (int **)(iwram_3001eec + 0x7828);
    k = *p;
    *q = p;
    if (k == 0)
        p[6] = 0;
    else
        Data_80ee2b4[k - 1](p);
    gfree(0x28);
    gfree(0x27);
    gfree(0x29);
}

/* Func_80aae14 (ComputeDjinnLayout, 0x080aae14) -- 149 instructions.
 * NON-MATCHING: 130 encodings of 156 differ (objcmp).
 * ours 145 encodings / 296 bytes against ref 156 / 324, so 130 is NOT a true
 * distance.  tools/tryc.py --align, which is the figure that ranks variants,
 * says 113 instructions in disagreeing regions of 160.
 *
 * Reference: asm/rom_a1000/rom_aa538_c_c_a_c_a.s -- TWO functions
 * (Func_80aad10 at 0x080aad10, already parked as 80aad10.c, and this one), so
 * landing needs a split.  datacheck clean and NO cross-function label
 * reference in either direction, so the split needs no export: .Laae90,
 * .Laae94 and .Laaf04 are all pool words inside this function's own body.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80aae14.c \
 *     asm/rom_a1000/rom_aa538_c_c_a_c_a.s --func Func_80aae14
 *
 * STRUCTURE, read from the .s before compiling anything:
 *  - frame: push{r5,r6,r7,lr} + r11/r10/r9 + r8, sub sp,#0xc = 32+12, so
 *    sp+0x2c is the FIRST STACK ARG, i.e. the FIFTH parameter.
 *    r12=a r8=b [sp+8]=out [sp+4]=p1 sp+0x2c=p2; r10=k (returned), r11=n1,
 *    [sp]=n2.
 *  - it is two symmetric halves: half 1 copies b's entries into out masked with
 *    0x3fff and sets bit 0x8000 on the ones absent from a; half 2 appends a's
 *    entries absent from b with bit 0x4000.  Both halves search the other list
 *    with an inner `for (j = 0; j < 0x20; j++) ... break;` whose j survives the
 *    loop because `j == 0x20` is tested after it.
 *  - half 1's biv r5 is out + 2*k - 2, which serves the store as [r5,#2] and
 *    the `|= 0x8000` as [r5,#0] after the increment: the source is
 *    `out[k] = ...; k++; ... out[k-1] |= 0x8000;`
 *
 * THE ONE LEVER THAT MATTERS HERE, and it is a loop.c readout, not a guess.
 * The two halves are spelled DIFFERENTLY in the source and the .s says which
 * way round.  Half 1 exits on a POINTER compare against b+0x7c whose bound is
 * recomputed inline every iteration (`mov r3,r8 / add r3,#0x7c / cmp`); that
 * un-hoisted recompute is the signature of maybe_eliminate_biv_1 rewriting
 * `i <= 0x1f` onto a giv -- it emits the bound AT the compare, after loop
 * invariant motion has run, so nothing can hoist it.  Half 2 keeps its counter
 * in r9 and compares `cmp r2,#0x1f`, i.e. the SAME rewrite FAILED there.
 *
 * Measured, --align of 160, part 1 spelling x part 2 spelling:
 *   pointer / pointer ....................................... 191   (len 151)
 *   pointer / counter+pointer ............................... 187   (len 156)
 *   index   / for-with-leading-break ........................ 118   (len 151)
 *   index   / index ......................................... 112   (len 151)
 *   index   / counter + separate walking pointer ............ 111   (len 155)
 *   index   / `a` as an int, address as *(u16*)(a + i*4) .... 124   (len 153)
 *   index   / explicit BYTE-OFFSET variable  <-- this file .. 113   (len 153)
 * The pointer spelling HOISTS the bound into a register (3 extra preheader
 * insns); only the index spelling produces the ROM's inline recompute.  That
 * is the single biggest step, 191 -> 112.
 *
 * WHY THIS FILE SHIPS 113 AND NOT THE 111: the 111 has the wrong FRAME and the
 * wrong ADDRESSING.  With the explicit byte offset this candidate reproduces
 * `sub sp,#0xc` (three stack slots, the ROM's) and the ROM's register-offset
 * load `ldrh r3,[r7,r6]` against `ldrh r3,[r7,r1]`; the 111 has `sub sp,#8` and
 * a single full-address biv.  Instruction KINDS decide, per batch 292's
 * refinement, and a true-distance-shaped smaller number is still the worse
 * candidate when the kinds disagree.
 *
 * BLOCKER -- global.c `allocno_compare`, the global register allocator.  Two
 * quantities contend for the last register: half 2's index and n2.  The ROM
 * spills n2 (its `ldr/add/str` per increment at [sp]) and keeps the index in
 * r9; we do the exact opposite, spilling the index to [sp] and keeping n2 in
 * fp.  Both have FOUR references (init, read+write of the increment, final
 * read), so the refs term ties and only live_length separates them -- and n2's
 * range is the whole function while the index's is half 2 only, which should
 * make the index win.  It does not, so the tie is not where the arithmetic
 * says it is and no declaration permutation reaches it:
 *   swap the n2/index DECLARATION order ..................... 113 (no change)
 *   move `n2 = 0` after `k`/`n1` ........................... 112
 *   both ................................................... 112
 *   loop condition via a[i*2] instead of the offset ........ 130 (worse)
 *
 * WHAT IS RIGHT: everything structural.  Both halves' loop shapes, both inner
 * searches, the 0xc frame, the out[k]/out[k-1] biv bias, the register-offset
 * load form, and the epilogue's argument order.  The residue is register names
 * plus the consequences of that one spill going the wrong way -- the ROM, with
 * all six high registers consumed, re-materialises 0x3fff from the POOL twice
 * in half 2 and hoists a fresh pool load into each inner loop's preheader,
 * where we still have a register to hold it in.
 *
 * INERT, measured: naming the 0x3fff mask; `(p[0] & 0x3fff) != (q[0] & 0x3fff)`
 * in place of the xor form; assigning the inner-loop pointer inside the body.
 */
int Func_80aae14(unsigned short *a, unsigned short *b, unsigned short *out, int *p1, int *p2)
{
    int i;
    int i2;
    int j;
    int k;
    int n1;
    int n2;
    unsigned short *p;
    unsigned short *q;
    int off;

    n2 = 0;
    k = 0;
    n1 = 0;
    if (b[0] != 0) {
        i = 0;
        do {
            out[k] = b[i * 2] & 0x3fff;
            k++;
            q = a;
            for (j = 0; j < 0x20; j++) {
                if (((b[i * 2] ^ q[0]) & 0x3fff) == 0)
                    break;
                q += 2;
            }
            if (j == 0x20) {
                n1++;
                out[k - 1] |= 0x8000;
            }
            i++;
        } while (i <= 0x1f && b[i * 2] != 0);
    }
    if (a[0] != 0) {
        i2 = 0;
        off = 0;
        do {
            q = b;
            for (j = 0; j < 0x20; j++) {
                if (((*(unsigned short *)((char *)a + off) ^ q[0]) & 0x3fff) == 0)
                    break;
                q += 2;
            }
            if (j == 0x20) {
                n2++;
                out[k] = (*(unsigned short *)((char *)a + off) & 0x3fff) | 0x4000;
                k++;
            }
            i2++;
            off += 4;
        } while (i2 <= 0x1f && *(unsigned short *)((char *)a + off) != 0);
    }
    *p1 = n1;
    *p2 = n2;
    return k;
}

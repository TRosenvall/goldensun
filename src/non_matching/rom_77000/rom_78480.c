/* Func_8078480 -- NOT MATCHING. 4 of 25, and ours is one instruction SHORT.
 *
 * Source asm: goldensun/asm/rom_77000/rom_78414_c_a_a.s
 *
 * Blocker: gcc keeps the result in r0 throughout and skips the ROM's final
 * `mov r0, r2`. Ours is 24 against 25 -- the "ours is shorter" signature, here
 * in its register-allocation form rather than a source rewrite.
 *
 * WHAT GOT IT FROM 25 OF 25 TO 4, and is the useful part:
 *
 * A `switch` WITH CONTIGUOUS CASES BECOMES A RANGE TEST. Written as
 *
 *     case 2: case 3: case 4: case 5: case 9:
 *
 * gcc recognises 2..5 as contiguous and emits `cmp #5 / bgt / cmp #2 / bge`,
 * which is nothing like the ROM's five individual `cmp`/`beq` pairs -- every
 * instruction after the third differs. An explicit if/else chain testing each
 * value with `==` produces the individual compares.
 *
 * That is the reverse of batch 59's finding, where a `switch` was needed to get
 * gcc's balanced tree and an if/else chain would not have produced it. The two
 * lower differently and the reference says which: individual `cmp`/`beq` pairs
 * mean the source compared individually.
 *
 * NEXT: only the r0-versus-r2 allocation remains.
 */
extern void *GetItemInfo(int id);

int Func_8078480(int id)
{
    unsigned char *info;
    int k;
    int r;

    info = (unsigned char *)GetItemInfo(id);
    k = info[2];
    r = 0;
    if (k == 1)
        r = 1;
    else if (k == 2)
        r = 2;
    else if (k == 3)
        r = 2;
    else if (k == 4)
        r = 2;
    else if (k == 5)
        r = 2;
    else if (k == 9)
        r = 2;
    return r;
}

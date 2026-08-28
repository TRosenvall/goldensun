/* OvlFunc_904_2008054 -- 0x02008054, asm/overlays/rom_799998/ovl_30_c_c.s
 *
 * 55 vs 56 lines, 21 differing.  Candidate at scratch/L8054b.c.
 *
 * SOLVED, and worth reusing: the ROM stores through a materialised address at
 * the second of two stores (`add r2, r1, r3 / str r3, [r2]`) where gcc folds
 * into `str r2, [r1, r3]`.  The reason is visible in the ROM -- the OFFSET
 * register is reused as the stored VALUE.  Writing it that way in the source,
 *
 *      off -= 0x3c;
 *      p = (int *)(base + off);
 *      off = 0x18;              /* the offset variable becomes the value */
 *      *p = off;
 *
 * took 49 differing to 21.  This is the same mechanism the named-pointer rule
 * describes (a value needing its own register forces the address out) but
 * reached from the other side: instead of the stored value being a load, it is
 * the offset itself, reassigned.  The first store in this function does the
 * same thing (0x1c0 -> 0x204) and reproduces for free.
 *
 * BLOCKER: base and offset occupy each other's registers -- the ROM keeps the
 * base in r1 and the offset in r3, ours the reverse -- and the ROM spends a
 * callee-saved r5 on the zero stored at actor+0x59 where ours does not push at
 * all.
 *
 * TRIED: swapping the declaration order of base and offset; hoisting `z = 0;`
 * into the dominating block so gcc would spill it.  Both 21.
 */

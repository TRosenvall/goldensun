/* OvlFunc_931_2008d08 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c.s
 * Best screen: 34 instructions against the ROM's 34, 7 differing.
 *
 * BLOCKER CLASS: the r2/r3 exchange. Every one of the seven differing lines is
 * the SAME instruction with those two registers swapped:
 *
 *     rom    mov r3, r5 / mov r2, #0x14 / add r3, #0x64 / strh r2, [r3]
 *     ours   mov r2, r5 / mov r3, #0x14 / add r2, #0x64 / strh r3, [r2]
 *
 * The ROM puts the ADDRESS in r3 and the VALUE in r2; we do the opposite. The
 * order of materialisation is the same in both -- address first, then value --
 * so this is allocation, not scheduling.
 *
 * Seventh member of the class. The live-range theory that batch 96 proposed for
 * it was refuted in batch 97 by OvlFunc_922_2008ed8, and this function is a
 * cleaner minimal case than any of the other six: 34 instructions, one basic
 * block of interest, three stores.
 *
 * TRIED, all 7 of 34 except where noted:
 *   `k` declared before `q` and `z` rather than after
 *   `k` assigned at the top of the function instead of beside the stores
 *     (18 of 34 -- worse, and consistent with the carried-value rule: this
 *     value must survive nothing, so distance only hurts)
 *   the two halfword stores through a `short *` walked with `p++`, which is
 *     what the ROM's `add r3, #2` looks like
 *   -fno-rerun-cse-after-loop
 *
 * WHAT IS RIGHT AND IS WORTH KEEPING: `mov r2, #0x14` serves BOTH the halfword
 * store at +0x64 and the word store at +0x68, which is a CARRIED value -- one
 * `int k` shared, named adjacent to its first use. The `strh r6` at +0x66 is
 * the masked `iwram_3001e40 & 3`, a value the enclosing `if` has proved is
 * zero; writing `= z` and writing `= 0` give the same code because gcc's value
 * numbering substitutes the register it knows holds zero.
 *
 * The four shifted arguments to __CreateActor needed the basic-block lever in
 * the usual form and that part matches.
  *
 * VOLATILE: TRIED, NO CHANGE. Batch-142-era work found that gKeyHeld and
 * iwram_3001e40 are declared volatile in some translation units and not
 * others, and that the difference unlocked OvlFunc_933_2008344 outright and
 * halved Func_80b86ec. This function was re-screened with every scalar global
 * marked volatile and the output is BYTE-IDENTICAL, so the missing re-reads
 * are not its problem. Do not try it again.
*/
struct Actor {
    unsigned char pad00[0x64];
    short f64;
    short f66;
    int f68;
    void (*f6c)(void);
};

extern unsigned int iwram_3001e40;
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void OvlFunc_931_2008c0c(void);
extern void OvlFunc_931_2008c44(void);

void OvlFunc_931_2008d08(void)
{
    struct Actor *q;
    int z;
    int k;
    int c1;
    int c2;

    c1 = 0x80 << 15;
    c2 = 0xc8 << 17;
    z = iwram_3001e40 & 3;
    if (z == 0) {
        q = __CreateActor(0xde, c1, 0, c2);
        if (q != 0) {
            k = 0x14;
            q->f64 = k;
            q->f66 = z;
            q->f68 = k;
            OvlFunc_931_2008c0c();
            q->f6c = OvlFunc_931_2008c44;
            __Actor_SetAnim(q, 1);
        }
    }
}

/* ---- MERGED from src/non_matching/overlays/2008d08.c ----
 * That file was a second park for the same function, written later under the
 * src/non_matching/overlays/ naming while this one already existed.  Its
 * analysis is kept verbatim below; the duplicate file is removed.
 *
 OvlFunc_931_2008d08 -- 0x02008d08,
 * asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c.s
 *
 * 34 of 34 lines, SEVEN differing.  Candidate at scratch/L8d08.c.
 *
 * SOLVED: the guarded interleave at __CreateActor
 * (`mov r1,#0x80 / mov r3,#0xc8 / mov r0,#0xde / lsl r1,#15 / mov r2,#0 /
 * lsl r3,#17`) reproduces from two named locals in the dominating block, and
 * the `and r6, r3` with the value as destination from `v &= 3;`.
 *
 * BLOCKER: the pointer and the stored constant occupy each other's registers.
 *      rom   mov r3, r5 / mov r2, #0x14 / add r3, #0x64 / strh r2, [r3]
 *      ours  mov r2, r5 / mov r3, #0x14 / add r2, #0x64 / strh r3, [r2]
 * and that decides the rest of the block, including whether the +0x68 word
 * store is scheduled before or after the second halfword store.
 *
 * TRIED: declaring the constant before the pointer; assigning it before the
 * pointer; writing the two halfword stores with explicit offsets instead of a
 * mutated pointer; casting the actor to `short *` and indexing by 0x32.
 * All 7.
 *
 * Same wall as src/non_matching/overlays/200807c.c and 20094ac.c -- naming the
 * stored constant moves the allocation but does not choose it.
 */

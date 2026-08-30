struct Actor {
    unsigned char pad00[0x64];
    short f64;
    short f66;
    int f68;
    void (*f6c)(void);
};

extern volatile unsigned int iwram_3001e40;
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

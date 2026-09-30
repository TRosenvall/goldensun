/* Func_801a7f4 (BuildPartyScreen) -- NON-MATCHING.
 * NON-MATCHING: 17 encodings of 134 differ (objcmp).
 * asm/rom_15000/rom_1a66c_a_c.s (the .s holds only this function; two in-function pools).
 *
 * objcmp: "XX ENCODINGS differ in 17 place(s) (ref 134, ours 134)" -- SIZE EXACT.
 * tryc --align: ~23 lines in disagreeing regions (label bookkeeping inflates it).
 *
 * Verify with:
 *   python3 tools/objcmp.py <this> asm/rom_15000/rom_1a66c_a_c.s --func Func_801a7f4
 *
 * LEVERS THAT WORKED (97 aligned -> 23):
 *  - REUSE prev AND count for the second loop's node pointer and x offset (the ROM
 *    keeps both in r6/r11 across the two loops).
 *  - split the chained `f12 = f1a = y` into a y local; store literal 6 (cse turns
 *    it into the fa register, as the ROM has).
 *  - test the list head directly and store literal 0 into n->prev (r2/r3 roles).
 *  - `y = 0x8c;` int intermediate gives the ROM's `mov` (HImode-literal rule);
 *    0x64 - count*8 must stay a literal expression (the ROM POOLS 0x64).
 *  - the second loop's head load through a one-member-plus-u16 UNION: alias set 0
 *    stops sched1 hoisting it above the two halfword stores.
 *  - `prev = 0; count = 0;` BEFORE the ids computation fixes the prologue.
 *
 * RESIDUE: the 0x396/0x398 stores: ROM uses r1/r2 for the two pointers and r3 for
 * the value; ours r0/r1 and r2.  Explicit pointer locals (and a hoisted 0x3b8
 * pointer) are worse (26+).  an int temp for the 0x64 value is worse (turns the
 * pooled 0x64 into a mov).
  *
 * *** BATCH-305 CORRECTION: THIS FILE ATTRIBUTES A RESIDUE TO sched1, AND sched1 DOES NOT
 * *** RUN IN THIS BUILD.  Verified with -da at production flags: the dump sequence is
 * *** 17.lreg 18.greg 19.flow2 20.ce2 23.sched2 25.jump2 26.mach -- there is NO sched1 dump,
 * *** because flag_schedule_insns is off at -O2 here, so only the post-reload scheduler runs.
 * *** Re-attribute to sched2 (rank_for_schedule), to combine, or to the ALLOCATION that fixed
 * *** the order.  Relatedly, any "-fno-schedule-insns is inert" note below rules nothing out:
 * *** that flag controls a pass that never runs.  The sched2 tie-break is priority ->
 * *** dependent count (more wins) -> INSN_LUID (lower wins), and LUID preserves EXPAND order.
 * *** See "sched1 DOES NOT RUN IN THIS BUILD" in docs/elevation.md.
*/
struct Node {
    struct Node *prev;
    struct Node *next;
    unsigned short f8;
    unsigned short fa;
    unsigned char pad0c[4];
    unsigned short f10;
    unsigned short f12;
    unsigned short f14;
    unsigned short f16;
    unsigned short f18;
    unsigned short f1a;
};

union U {
    struct Node *n;
    unsigned short h;
};

extern unsigned char *iwram_3001e98;
extern struct Node *Func_801a910(int alloc);
extern void Func_801bd98(int kind, int id, struct Node *e, int flag);
extern void Func_801c188(void);

void Func_801a7f4(void)
{
    unsigned char *p;
    unsigned int limit;
    unsigned int j;
    unsigned short *ids;
    struct Node *prev;
    struct Node *n;
    struct Node *head;
    int count;
    int kind, id;
    int x, y;

    p = iwram_3001e98;
    limit = *(unsigned short *)(p + (0xe5 << 2));
    j = *(unsigned short *)(p + (0xe7 << 2));
    prev = 0;
    count = 0;
    ids = (unsigned short *)(p + (0xd5 << 2)) + j;
    while (j < limit) {
        kind = ids[0];
        id = ids[0x10];
        n = Func_801a910(0);
        if (n == 0)
            break;
        Func_801bd98(kind, id, n, 0);
        if (*(struct Node **)(p + (0xd2 << 2)) == 0) {
            *(struct Node **)(p + (0xd2 << 2)) = n;
            n->prev = 0;
        } else {
            prev->next = n;
            n->prev = prev;
        }
        n->next = 0;
        count++;
        prev = n;
        if (count == 5)
            break;
        ids++;
        j++;
    }
    *(unsigned short *)(p + 0x396) = 0x64 - count * 8;
    y = 0x8c;
    *(unsigned short *)(p + (0xe6 << 2)) = y;
    prev = ((union U *)(p + (0xd2 << 2)))->n;
    count = 0;
    while (prev != 0) {
        x = *(unsigned short *)(p + 0x396) + count;
        prev->f10 = x;
        y = *(unsigned short *)(p + (0xe6 << 2));
        prev->f12 = y;
        prev->f1a = y;
        prev->f18 = x;
        if (prev->fa == 6 && *(unsigned short *)(p + (0xee << 2)) == 0) {
            prev->f12 = 6;
            prev->f1a = 6;
        }
        prev->f14 = 0;
        prev->f16 = 0;
        prev = prev->next;
        count += 0x10;
    }
    Func_801c188();
}

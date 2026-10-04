/* Func_801a7f4 (BuildPartyScreen) -- asm/rom_15000/rom_1a66c_a_c.s
 *
 * MATCHING.  objcmp --func: OK, 280 bytes, 134 encodings and 4 relocations
 * identical.  objcmp --whole: OK whole file.  Was a park at 17 of 134.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_1a66c_a_c.c asm/rom_15000/rom_1a66c_a_c.s \
 *     --func Func_801a7f4
 *   ... and the same with --whole (the .s holds this function alone).
 *
 * SPLIT: none.  tools/datacheck.py is silent and tools/split_s.py --dry-run says
 *   "rom_1a66c_a_c.s holds only Func_801a7f4 and no data; convert it directly,
 *    no split needed".
 * PINS: 0.  No `register ... __asm__`, no inline asm, no per-file flag group
 *   (objcmp reports the production flag set with no adjustment).
 *
 * ======================= WHAT THE RESIDUE WAS =======================
 *
 * The park read 17 of 134 at indices 71-83 and 91-94, and described it as the
 * two pool loads being SWAPPED, concluding "the order in which expand evaluates
 * the store's address and value".  It then listed ten spellings of that store,
 * all exactly inert.  The observation is right and the verdict is wrong twice:
 *
 *   * NO POOL WORD IS INVOLVED.  The two words sit at 0xcc (0x64) and 0xd0
 *     (0x396) in BOTH streams and the relocations are clean; what was swapped
 *     is the order of the two LOADS.  (Worth saying because "pool loads
 *     swapped" reads like the known dirty-relocation pool-order class, and it
 *     is not that class.)
 *
 *   * THE CAUSE IS REGISTER ALLOCATION, AND THE PSEUDO RESPONSIBLE IS NOT IN
 *     THAT BLOCK.  From .18.greg: p66 = `p + 0x396` -> r0, p75 = `p + 0x398`
 *     -> r1, where the ROM has r1 and r2.  `;; 75 conflicts: ... 43 ... 3 13`
 *     -- p75 conflicts with p43, p43 is allocated earlier and takes r2, so r2
 *     becomes a hard conflict of p75; find_reg walks REG_ALLOC_ORDER
 *     {3,2,1,0,...}, finds r3 conflicting and r2 gone, and takes r1.  p66,
 *     which conflicts with hard regs 2 and 3 and now with r1, takes r0 -- which
 *     is why a fourth value is live in the block and why 91-94 has to copy r0
 *     away before `mov r0,#0xee`.
 *
 * AND p43 IS `y` DOING TWO UNRELATED JOBS: the pre-loop
 * `*(p+(0xe6<<2)) = 0x8c` store and the in-loop reload of that same halfword.
 * One name for both makes the pseudo live across a block boundary, which makes
 * it a GLOBAL allocno, which is what gets it r2 that early.  The park's note
 * that `y = 0x8c;` as an int intermediate is a lever is correct -- the
 * HImode-literal rule needs it -- but nobody checked that sharing the NAME with
 * the loop's value is what breaks a block twenty instructions away.
 *
 * ===================== THE TWO EDITS, AND THE CROSSING =====================
 *
 * EDIT 1  a separate `y0` for the pre-loop 0x8c store.
 *     17 -> 8, and the first difference moves from index 71 to 101: all
 *     thirteen of 71-83 and all four of 91-94 close together.  But EIGHT NEW
 *     differences open at 101-108, because with `y` no longer global both loop
 *     values become LOCAL allocnos and local-alloc hands `y` r3 and `x` r2 --
 *     the ROM's pair the other way round.  Declaration placement of `y0` is
 *     exactly inert (three placements, all 8).
 *
 * EDIT 2  `prev->f18 = x;` moved above the two `y` stores.
 *     Both loop values are local allocnos with REG_N_REFS 3, so QTY_CMP_PRI is
 *     floor_log2(3)*3/(death-birth) and the span is the only free term.  In the
 *     park's order `x` spans insn 219..296 and `y` spans 245..279, so `y` wins
 *     the qty and takes r3 first.  Shortening `x`'s span makes them tie, and
 *     qty_compare_1 breaks a tie BY QTY NUMBER, which follows first use -- and
 *     `x` is used first.  So `x` takes r3 and `y` takes r2, as the ROM has.
 *
 * ALONE, EDIT 2 IS WORTH NOTHING (it is inside a region that already matched)
 * and EDIT 1 IS WORTH 9 OF 17 WHILE COSTING 8.  Crossed, they are worth 17.
 * Thirteen orders of the five loop statements, measured on the edit-1 body:
 *   a=f10=x  b=f18=x  c=y=*(p+(0xe6<<2))  d=f12=y  e=f1a=y
 *     a c b d e  0      a c d b e  0
 *     a b c d e  5      a c d e b  8  (the park's order)
 *     c a b d e  7      c a d b e  7      c a d e b  7
 *     c d a b e  7      c d a e b  7      c d e a b  8
 *     c hoisted above the x computation, two shapes   9
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
    int x, y, y0;

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
    y0 = 0x8c;
    *(unsigned short *)(p + (0xe6 << 2)) = y0;
    prev = ((union U *)(p + (0xd2 << 2)))->n;
    count = 0;
    while (prev != 0) {
        x = *(unsigned short *)(p + 0x396) + count;
        prev->f10 = x;
        y = *(unsigned short *)(p + (0xe6 << 2));
        prev->f18 = x;
        prev->f12 = y;
        prev->f1a = y;
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

/* Func_801a7f4 (BuildPartyScreen) -- PARK.
 * NON-MATCHING: 17 encodings of 134 differ (objcmp, production flags).
 * asm/rom_15000/rom_1a66c_a_c.s (the .s holds only this function; two in-function pools).
 * SIZE EXACT: ref 134 encodings, ours 134.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/801a7f4.c \
 *     asm/rom_15000/rom_1a66c_a_c.s --func Func_801a7f4
 *
 * LEVERS THAT WORKED (97 aligned -> 23), all still in the body:
 *  - REUSE prev AND count for the second loop's node pointer and x offset (the ROM
 *    keeps both in r6/r11 across the two loops).
 *  - split the chained `f12 = f1a = y` into a y local; store literal 6 (cse turns
 *    it into the fa register, as the ROM has).
 *  - test the list head directly and store literal 0 into n->prev (r2/r3 roles).
 *  - `y = 0x8c;` int intermediate gives the ROM's `mov` (HImode-literal rule);
 *    0x64 - count*8 must stay a literal expression (the ROM POOLS 0x64).
 *  - the second loop's head load through a one-member-plus-u16 UNION: alias set 0
 *    stops the load being hoisted above the two halfword stores.
 *  - `prev = 0; count = 0;` BEFORE the ids computation fixes the prologue.
 *
 * *** BATCH-305 CORRECTION, KEPT: THIS FILE USED TO ATTRIBUTE A RESIDUE TO sched1,
 * *** AND sched1 DOES NOT RUN IN THIS BUILD.  At production flags the dump sequence
 * *** is 17.lreg 18.greg 19.flow2 20.ce2 23.sched2 25.jump2 26.mach -- there is no
 * *** sched1 dump, because flag_schedule_insns is off at -O2 here.  Any
 * *** "-fno-schedule-insns is inert" note rules nothing out: that flag controls a
 * *** pass that never runs.  See docs/elevation.md.
 *
 * ============== BATCH-316b: THE RESIDUE, PRINTED WITH OPERANDS ==============
 *
 * The old header described it as "the 0x396/0x398 stores: ROM uses r1/r2 for the
 * two pointers and r3 for the value; ours r0/r1 and r2".  That is right as far as
 * it goes, but it is THREE coupled facts, and the first one is the cause:
 *
 *   ref                        | ours
 *   71 mov  r3,fp              | mov  r2,fp
 *   72 lsls r2,r3,#3           | lsls r3,r2,#3
 *   73 ldr  r3,[pc,#48]  0x64  | ldr  r0,[pc,#52]  0x396   <-- POOL LOADS SWAPPED
 *   74 ldr  r1,[pc,#52]  0x396 | ldr  r2,[pc,#48]  0x64
 *   75 subs r3,r3,r2           | movs r1,#0xe6
 *   76 movs r2,#0xe6           | subs r2,r2,r3
 *   77 add  r1,r9              | add  r0,r9
 *   78 lsls r2,r2,#2           | lsls r1,r1,#2
 *   79 strh r3,[r1]            | strh r2,[r0]
 *   80 add  r2,r9              | add  r1,r9
 *   81 movs r3,#0x8c           | movs r2,#0x8c
 *   82 strh r3,[r2]            | movs r3,#0xd2
 *   83 movs r3,#0xd2           | strh r2,[r1]
 *   91 movs r0,#0xee           | adds r5,r0,#0
 *   92 lsls r0,r0,#2           | movs r0,#0xee
 *   93 adds r5,r1,#0           | lsls r0,r0,#2
 *   94 adds r4,r2,#0           | adds r4,r1,#0
 *
 * THE ROM LOADS THE *VALUE* POOL WORD (0x64) FIRST AND SUBTRACTS IMMEDIATELY, so
 * only three low registers are ever live in the block (r1,r2,r3).  OURS LOADS THE
 * *ADDRESS* POOL WORD (0x396) FIRST, which keeps a fourth value live and pulls r0
 * into the block; every other difference here -- the one-register role shift and
 * the `movs r3,#0xd2` / `strh` rotation at 82/83 and the copy at 91/93 -- follows
 * from that.  So this is ONE defect with a long tail, not a register-naming tie:
 * name it "the order in which expand evaluates the store's address and value".
 *
 * ============ MEASURED INERT THIS BATCH -- 10 SPELLINGS OF THAT STORE ============
 * One container, all against the 17 base.  NOTHING MOVED IT.
 *     0x64 - (count << 3) instead of count * 8                     17
 *     an `unsigned short w` temp holding the value, stored after    17
 *     -(count * 8) + 0x64  (PLUS operands flipped in integer space) 17
 *     (unsigned short) cast on the whole value expression           17
 *     an `unsigned short *hp` local for the 0x396 address           17
 *     `*((unsigned short *)(p + (0xe5 << 2)) + 1)` for the address  17
 *     `p + 0x398` as a literal instead of `p + (0xe6 << 2)`         17
 *     the two stores swapped in source order                        17
 *     `y = 0x8c;` hoisted above the 0x396 store                     28  WORSE
 * The brief's PLUS-operand-order lever (lever 5) is among these and is EXACTLY
 * inert: `0x64 - count*8` and `-(count*8) + 0x64` are the same after fold, which
 * is the documented reason -- the flip has to happen somewhere fold cannot
 * normalise, and a constant-minus-product is not such a place.
 *
 * The allocno_compare inputs for the block, measured off `.17.lreg` (so the next
 * attempt does not have to guess them):
 *   ;; 19 regs to allocate: 120 37 57 58 43 36 40 41 35 34 32 39 75 147 66 124 94 82 33
 *     p39  R=15 L=120 0.3750 ->r11    p147 R=5  L=38  0.2632 ->r1
 *     p75  R=3  L=8   0.3750 ->r1     p66  R=3  L=13  0.2308 ->r0
 *     p124 R=3  L=20  0.1500 ->r0     p94  R=3  L=21  0.1429 ->r4
 *     p82  R=3  L=22  0.1364 ->r5     p33  R=3  L=36  0.0833 ->(none)
 * floor_log2(R)*R/L reproduces the printed order exactly here too, so if the
 * next attempt wants a different register for one of these four short-lived
 * pseudos, the quantity to move is R or L -- not the spelling of the store.
 *
 * RESIDUE SPELLINGS ALREADY RULED OUT BY EARLIER BATCHES: explicit pointer locals
 * and a hoisted 0x3b8 pointer are worse (26+); an int temp for the 0x64 value is
 * worse (it turns the pooled 0x64 into a mov -- the HImode-literal rule).
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

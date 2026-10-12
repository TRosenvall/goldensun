/* ===== BATCH 339 BRIEF D: A VERIFIED 0 EXISTS AND THE SPLIT IS BLOCKED =====
 *
 * PARK STAYS. 34 of 61 was re-derived from the recovered body (47 instructions
 * against the reference's 48, so a 1-instruction gap, not a distance).  Brief D
 * then reached BYTE-IDENTICAL -- candidate kept at
 * scratch_elev/b339/D/p2_candidate.c -- and it is NOT installed, for a reason
 * that is about the FILE, not the function.
 *
 * WHY IT CANNOT SHIP YET.  The landing needs its piece split out, and the split
 * was attempted and REVERTED:
 *
 *   1. split_s.py refused, naming two code labels that would cross files
 *      (.L2054, .L2057).  Those were exported, `make compare` stayed green, and
 *      the split then succeeded.
 *   2. The build FAILED at link: `undefined reference to .L20d0`, from both
 *      new pieces.
 *   3. `.L20d0` is `.lcomm .L20d0, 0x60` -- a LOCAL COMMON (BSS) symbol, 0x60
 *      bytes, declared in one piece and referenced from the other two.
 *
 * ***A .lcomm CROSSING IS A DIFFERENT CLASS FROM A CODE-LABEL CROSSING, AND
 * split_s.py DOES NOT CHECK FOR IT.***  A `.global` does not fix it either: the
 * symbol has to become `.comm` to get a symbol-table entry, which changes its
 * binding, and that is a tree change with its own risk -- not something to do
 * incidentally while landing one function.  The split was backed out and
 * `make compare` verified green at the target SHA1 before anything else.
 *
 * SO THE WORK IS BANKED AND THE BLOCKER IS A TOOLING ITEM, recorded as such:
 * split_s.py's pre-split check should scan for `.lcomm`/`.comm` crossings
 * alongside `.L` label crossings, and say so rather than letting the link fail.
 *
 * NOTE ALSO that the landing wanted CSE_CFLAGS (-fno-rerun-cse-after-loop) or
 * GCSE_CFLAGS, so even once the split is possible it carries a per-file flag
 * rule, and flag rules in this tree are explicit-not-pattern by standing
 * practice (the Makefile records five mis-scoped -O1 wildcards).
 *
 * THE MECHANISM, which stands regardless.  allocno_compare (global.c:598-620)
 * scores floor_log2(n_refs)*n_refs/live_length, and THE ONLY SOURCE HANDLE ON
 * live_length IS WHERE A DEFINITION SITS -- not the order of a block of
 * definitions, and not declaration order.  This park's prose had screened
 * DECLARATION permutations exhaustively and flat; moving `i = 0;` one slot took
 * it 34 -> 22.  The remaining residue was a missing `mov r2,#0` that no spelling
 * reaches and either -fno-gcse or -fno-rerun-cse-after-loop supplies -- and
 * BOTH ARE EXACTLY INERT ON THE PARKED BODY, which is the crossing-partner case
 * in its clearest form: a flag inert on the old body need not be inert on a
 * changed one.
 *
 * AND THIS PARK'S CLASS BOUND IS INHERITED BY THREE OTHERS, which should be
 * re-screened on INITIALISER PLACEMENT rather than on the register choice:
 * ovl_7ced6c/2009c84.c, ovl_7ced6c/200a16c.c and ovl_7d0e88/2008f58.c all cite
 * the same callee-saved coin flip "one dimension wider".  That extra dimension
 * is what gave way here.
 */
/* OvlFunc_951_20096a8 -- NOT MATCHING, a LENGTH gap: ref 48 instructions against ours 47 (-1).
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/N96a8_best.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * THE LENGTH-GAP CRITERION APPLIES, and it is this tree's most productive
 * targeting instrument -- four batches run on it closed 8 of 15, 7 of 25,
 * 8 of 25 and 10 of 32, and LOCATED the missing-or-extra instruction on
 * essentially every target.  Our side is 1 instruction(s) SHORTER, so the
 * search is which statement is missing -- see docs/elevation.md, 'OUR STREAM BEING
 * SHORTER IS A SIGNATURE'.
 * No encoding figure exists until the lengths agree.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7d6418/20096a8.c \
 *     asm/overlays/rom_7d6418/ovl_30_c_c_c_c_a.s --func OvlFunc_951_20096a8
 *
 * The original prose park is preserved below.
 */
struct Ent {
    int f0;
    int f4;
    int f8;
    short fc;
    short fe;
    short f10;
    short f12;
    short f14;
    unsigned char pad16[2];
};

struct Hdr {
    unsigned char pad00[4];
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x40 - 0x10];
    int f40;
    int f44;
    int f48;
    int f4c;
};

extern unsigned char L205a[] __asm__(".L205a");
extern unsigned char L205e[] __asm__(".L205e");
extern unsigned short L2062[] __asm__(".L2062");
extern struct Hdr L2070 __asm__(".L2070");
extern struct Ent L20d0[] __asm__(".L20d0");

extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern int __StartTask(void *f, int pri);
extern void OvlFunc_951_2008e5c(void);

void OvlFunc_951_20096a8(void)
{
    struct Hdr *h;
    unsigned char *a;
    unsigned char *b;
    unsigned short *c;
    struct Ent *e;
    int i;
    int z;

    h = &L2070;
    a = L205a;
    e = L20d0;
    c = L2062;
    b = L205e;
    i = 0;
    z = 0;
    do {
        e->f0 = *a << 16;
        e->f8 = *b << 16;
        i += 1;
        e->f4 = z;
        e->fc = *c;
        e->fe = z;
        e->f10 = z;
        e->f12 = z;
        e->f14 = z;
        a += 1;
        b += 1;
        c += 1;
        e += 1;
    } while (i != 4);
    h->f4 = 0xffe20000;
    h->f8 = z;
    h->fc = 0xc8 << 15;
    h->f40 = z;
    h->f44 = z;
    h->f48 = z;
    h->f4c = z;
    __Actor_SetAnim(__MapActor_GetActor(0x14), 2);
    __Actor_SetAnim(__MapActor_GetActor(0x15), 2);
    __StartTask(OvlFunc_951_2008e5c, 0xc83);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_951_20096a8 -- 0x020096a8,
 * asm/overlays/rom_7d6418/ovl_30_c_c_c_c.s
 *
 * 53 lines against the ROM's 54, 34 differing.  Candidate at
 * scratch/N96a8_best.c.  TWO residues, both in the allocator.
 *
 * 1. A FOUR-WAY REGISTER ROTATION.  Four pointers and a counter are live across
 *    the table loop.  The ROM assigns them r6, r5, r4, r7; we assign r7, r6, r5,
 *    r4 -- the same registers, each shifted by one, with the same prologue.  The
 *    materialisation ORDER is already identical to the ROM's, so there is
 *    nothing left in the source to reorder: three declaration permutations
 *    (counter first, pointers reversed, counter mid-list) are byte-identical to
 *    each other and to the original.  This is the class of
 *    src/non_matching/ovl_7ced6c/2009c84.c and .../200a16c.c one dimension
 *    wider, and those two already establish that the source has no handle on it.
 *
 * 2. THE POST-LOOP ZEROS ARE A SECOND VARIABLE.  The ROM materialises a fresh
 *    `mov r2, #0` after the loop for the six header stores, keeping the loop's
 *    own zero in r1; we reuse the loop's zero for both, which is the one missing
 *    line.  Declaring a separate `z2 = 0;` beside the header stores does NOT
 *    separate them -- gcc CSEs on the value, not the spelling, exactly as
 *    src/non_matching/ovl_7d30e0/20091d8.c records for the same question.
 *
 * SOLVED and worth keeping: the table walk itself screens clean.  The 0x18-byte
 * entry as a struct with three int fields and five shorts, the three source
 * cursors advancing by 1, 1 and 2, and the `*a << 16` byte-to-word widening all
 * reproduce; the five `.LNNNN` data labels are four hex digits so they need no
 * _TBL_ alias.
 *
 * WHY IT IS HERE AT ALL: this function has FIVE calls and the candidate filter
 * required eight until this round.  It is a fair test of the relaxed floor and
 * it failed on a wall that has nothing to do with call density.
 *)

*/
#endif

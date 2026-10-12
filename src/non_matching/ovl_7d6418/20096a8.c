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

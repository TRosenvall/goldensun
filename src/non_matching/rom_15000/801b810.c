/* Func_801b810 (PagePartyListBackward) -- 0x0801b810,
 * asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c.s
 *
 * NON-MATCHING: 161 encodings of 190 differ (objcmp).
 *
 * SIZE EXACT (408 bytes both).  Instruction count 185 against 184 -- ONE over.
 * RELOCATIONS DIFFER, and only by a uniform +4 byte shift: the candidate saves
 * r8 in the prologue and the ROM does not, which moves every `bl` four bytes
 * later.  All nine call targets are the ROM's nine, in the ROM's order.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801b810.c \
 *     asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c.s --func Func_801b810
 *
 * NO SHIMS, NO PINS, NO asm.  Its .s carries NO data section -- datacheck.py
 * prints nothing and exits 0 -- so no split is needed; but the file holds four
 * functions (Func_801b664, Func_801b810, Func_801b9a8, Func_801b9ec) and only
 * this one is attempted, so it cannot convert whole either.  Func_801b664 is
 * documented in the .s as "the forward counterpart, same shape", so whatever
 * closes this closes that.
 *
 * TWO LEVERS LANDED.
 *
 * 1. THE PAGE COUNTER IS `unsigned`.  `k = p->f394; if (k > 5)` with `k` an
 *    `int` emits `bgt`; the ROM has `bhi`.  `unsigned int k` gives `bhi` and is
 *    worth 13 (174 -> 161).  The value arrives by `ldrh`, so unsigned is also the
 *    honest type.
 *
 * 2. A HALFWORD DECREMENT IS `add` WITH A POOLED 0xffff, NOT `sub #1`.
 *    `p->f39c = p->f39c - 1;` written against the MEMBER stays in HImode and
 *    gives the ROM's `ldr r1,=0xffff / ldrh r3,[r5] / add r3,r1 / strh r3,[r5]`.
 *    Routed through an `int` local it becomes `sub r3,#1`.  Same for
 *    `n->step = -0xc;`, which the ROM pools as 0xfff4 while the +0xc store three
 *    blocks above is a plain `mov r2,#0xc`.  This is the const.sym halfword
 *    exception the Func_801dd28 park records, appearing four times here.
 *
 * THE EMPTY LOOP AT .L1b8e0 IS REAL AND IS REPRODUCED.
 *   mov r1,#0 / cmp r3,#5 / beq / ldrh r3 again / sub r3,#5
 *   .L1b8e0: add r1,#1 / cmp r1,r3 / bne .L1b8e0
 * A loop whose only effect is to leave r1 == r3.  gcc-2.96 has no pass that
 * deletes it.  `i = 0; if (p->f394 != 5) { k = p->f394 - 5; do { i++; } while
 * (i != k); }` reproduces it, INCLUDING the second `ldrh` of the same member --
 * the entry guard is the unsigned rewrite of `0 < (unsigned)(n - 5)` into
 * `n != 5`, which is why the compare is against 5 and not against 0.
 *
 * ONE MORE SOURCE FACT WORTH KEEPING: field 0x10 of the node is read BOTH as
 * `ldrh` (feeding `n->ty = n->y + v`, a halfword store, so combine drops the
 * extension) and as `ldrsh` (feeding `while (n->y != n->ty)`, a real signed
 * compare).  ONE `short y;` member gives both -- it does not need two types.
 *
 * MEASURED AND INERT / WORSE:
 *   a short-lived temp copied into `b` after the `(a|b)` test   172 (+2 insns)
 *   a named `unsigned short *pf = &p->f39e;` for both reads     161
 *   splitting `k = p->f394; k = k - 5;`                         161
 *   member-direct arms in the k<=5 else branch                  177 (6 insns short)
 *
 * BLOCKER: ONE ALLOCNO TOO MANY -- THE CANDIDATE BUYS r8 AND THE ROM DOES NOT.
 * PASS .18.greg.  The ROM lives inside `push {r5,r6,r7,lr}`; the candidate emits
 * `mov r7,r8 / push {r7}` and the matching two-instruction epilogue, +4
 * instructions, and pays for it by needing one fewer elsewhere (185 vs 184).
 *
 * The mechanism is a CONFLICT THE ROM DOES NOT HAVE.  The ROM keeps
 * &p->f39e in r6 from the prologue, kills it at `ldrh r2,[r6]` in the (a|b)
 * block, and then REUSES r6 for the long-lived copy of that halfword via
 * `mov r6,r2` -- so one hard register serves both roles on mutually exclusive
 * paths.  In the candidate the long-lived value is born directly out of the
 * address (`ldrh r5,[r1]`), so the two allocnos overlap at that instruction,
 * conflict, and the address is pushed to r8.  This is the exact shape the
 * "copying a derived value into its own local lets the source die in its
 * register" lever addresses, and the obvious spelling of it -- a short-lived
 * temp copied into `b` just inside the `if` -- costs two instructions instead of
 * saving four (172).  Something finer-grained is needed and I did not find it.
 *
 * A SECOND, SMALLER RESIDUE IS THE cse-ZERO CLASS, in the `(a | b) == 0` arm:
 * the ROM materialises `mov r0,#0 / strh r0,[r7,#0x3e]` while gcc reuses the
 * register that already holds the `orr` result, which is provably zero on that
 * path (`strh r1,[r7,#0x3e]`).  That is canon_reg substituting qty_first_reg,
 * and per the Func_801c49c finding the empty-asm barrier cannot reach it.  The
 * open route -- a dead store inside a conditionally skipped block, hitting
 * invalidate_skipped_block -- was NOT tried here; the r8 problem dominates and
 * should be solved first.
 */
struct Node {
    unsigned char pad0[4];
    struct Node *next;                  /* 0x04 */
    unsigned char pad1[0xa - 8];
    unsigned short id;                  /* 0x0a */
    unsigned char pad2[0x10 - 0xc];
    short y;                            /* 0x10 */
    unsigned char pad3[2];
    short step;                         /* 0x14 */
    unsigned char pad4[2];
    short ty;                           /* 0x18 */
};
struct Party {
    unsigned char pad0[8];
    unsigned short f8;                  /* 0x008 */
    unsigned short fa;                  /* 0x00a */
    unsigned char pad1[0x3e - 0xc];
    unsigned short f3e;                 /* 0x03e */
    unsigned char pad2[0x348 - 0x40];
    struct Node *head;                  /* 0x348 */
    unsigned char pad3[0x354 - 0x34c];
    unsigned short xs[0x10];            /* 0x354 */
    unsigned short ys[0x10];            /* 0x374 */
    unsigned short f394;                /* 0x394 */
    unsigned short f396;                /* 0x396 */
    unsigned char pad4[0x39c - 0x398];
    unsigned short f39c;                /* 0x39c */
    unsigned short f39e;                /* 0x39e */
    unsigned char pad5[2];
    unsigned short f3a2;                /* 0x3a2 */
};

extern void Func_801b9a8(struct Party *p, int a);
extern void Func_801b9ec(struct Party *p, int a);
extern void Func_801ba68(struct Party *p, int a);
extern void Func_801bd98(int x, int y, struct Node *n, int m);
extern void Func_801b010(int id, int a);
extern void WaitFrames(int n);

void Func_801b810(struct Party *p)
{
    struct Node *n;
    int a, b, i, v;
    unsigned int k;

    Func_801b9a8(p, p->f39e);
    p->f3a2 = 0x21;
    WaitFrames(1);
    k = p->f394;
    if (k > 5) {
        a = p->f39c;
        b = p->f39e;
        if ((a | b) != 0) {
            if (b == 1 && a != 0) {
                p->f8 = 8;
                p->f39c = p->f39c - 1;
                Func_801ba68(p, 0);
                if (p->f39c == 0)
                    p->fa = 0;
                p->f3e = b;
            } else {
                p->f39e = p->f39e - 1;
            }
        } else {
            n = p->head;
            p->f3e = 0;
            if (n->next != 0) {
                v = 0x40;
                do {
                    n->ty = n->y + v;
                    n->step = 0xc;
                    n = n->next;
                    v -= 0x10;
                } while (n->next != 0);
            }
            n = p->head;
            while (n->y != n->ty)
                WaitFrames(1);
            i = 0;
            if (p->f394 != 5) {
                k = p->f394 - 5;
                do {
                    i++;
                } while (i != k);
            }
            n = p->head;
            p->f39c = i;
            p->f39e = 4;
            if (n != 0) {
                unsigned short *q = &p->xs[i];
                do {
                    Func_801bd98(q[0], q[0x10], n, 1);
                    n = n->next;
                    q++;
                } while (n != 0);
            }
            n = p->head;
            v = p->f396;
            if (n->next != 0) {
                do {
                    n->ty = v;
                    n->step = -0xc;
                    n = n->next;
                    v += 0x10;
                } while (n->next != 0);
            }
            p->fa = 1;
        }
    } else {
        b = p->f39e;
        if (b != 0)
            p->f39e = b - 1;
        else
            p->f39e = k - 1;
    }
    p->f3a2 = 1;
    Func_801b9ec(p, p->f39e);
    WaitFrames(1);
    Func_801b010(p->head->id, 0);
    WaitFrames(1);
}

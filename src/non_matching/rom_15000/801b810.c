/* Func_801b810 (PagePartyListBackward) -- 0x0801b810, NON-MATCHING.
 *
 * 176 of 190 stream entries differ; ref 175 instructions / ours 168, ref 408
 * bytes / ours 392.  THIS BODY IS 7 INSTRUCTIONS SHORT, and that is the figure
 * to beat: the previously installed body was 161 of 190 at ref 175 / ours 176
 * with an ALIGNED residue of 53 of 202, and this one is 32 of 202.  The objcmp
 * figure RISES because the length gap widened from +1 to -7; the aligned number
 * is the one that means something (batch 346 brief, step 3).  PINS 0.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801b810.c \
 *     asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c_a.s --func Func_801b810
 * Aligned residue:
 *   ... python3 tools/tryc.py src/non_matching/rom_15000/801b810.c \
 *     --ref asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c_a.s --align
 *
 * NO SPLIT NEEDED.  split_s.py: "asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c_c_a.s
 * holds only Func_801b810 and no data; convert it directly, no split needed".
 * datacheck.py prints nothing.  (The old header's "the file holds four
 * functions" refers to the pre-split rom_1aeec_a_a_c_a_c_c_c_c.s and is stale.)
 *
 * ================= BATCH 346 BRIEF D: THE r8 BLOCKER IS SOLVED =================
 *
 * The old blocker read "ONE ALLOCNO TOO MANY -- THE CANDIDATE BUYS r8 AND THE
 * ROM DOES NOT ... the address is pushed to r8", with the proposed cure (a temp
 * copied into `b`) measured at 172 and declared insufficient.
 *
 * DECLARING `a` AND `b` AS `unsigned short` INSTEAD OF `int` REMOVES r8
 * ENTIRELY.  Both arrive by `ldrh` from `unsigned short` members, so this is
 * also the honest type.  The prologue and epilogue now match the ROM exactly --
 * tryc --align's first diff moves from index 1 to index 15 -- and all five r8
 * instructions (`mov r7,r8 / push {r7}`, `pop {r3} / mov r8,r3`, and three
 * `mov rN,r8` reloads) are gone.  ALIGNED RESIDUE 53 -> 32 of 202.
 *
 * THE PER-REGION SPLIT OF THE OLD +1, for the record (ten sites, not two):
 *   prologue +2, epilogue +2, `&p->f39e` birth +1, the (a|b) block +1,
 *   `p->f39e = k - 1` +1  --- against ---
 *   the `k` compare copy -1, the `b == 1` compare copy -1, the `a != 0` compare
 *   copy -1, `p->f3e = 0`'s `mov r0,#0` -1, the empty-loop guard's second
 *   `ldrh` -2.
 *
 * WHAT REMAINS IS -7, IN ONE CLASS PLUS TWO SINGLES:
 *
 *  1. FOUR `mov rX, rY` COPIES IMMEDIATELY BEFORE A `cmp` -- on `k` before
 *     `cmp r3,#5`, on `b` before `cmp r6,#1`, on `a` before `cmp r3,#0`, and on
 *     `b` again in the `k <= 5` arm.  In three of the four the ROM's original
 *     register stays live past the copy, so these are genuine SECOND PSEUDOS,
 *     not scheduling.  cse.c:5982-5997 is the gate that would turn such a copy
 *     into a dead store; it needs `prev_nonnote_insn (insn)` to be the insn
 *     that set the source (cse.c:5992-5994) AND
 *     `src_ent->first_reg == REGNO (SET_DEST)` (cse.c:5987).  The ROM's copies
 *     survived a gate ours never reaches, because ours never creates the second
 *     pseudo at all.  NEXT: find the spelling that creates it WITHOUT the +2
 *     penalty the obvious `bb = b;` carries (see the table).
 *  2. `mov r0,#0` before `strh r0,[r7,#0x3e]` -- the cse-zero class, unchanged
 *     from the old header: canon_reg substituting qty_first_reg, where gcc
 *     reuses the register already holding the provably-zero `orr` result.
 *  3. THE EMPTY-LOOP GUARD'S SECOND `ldrh` of `p->f394`.  *** THE OLD HEADER'S
 *     CLAIM THAT `if (p->f394 != 5) { k = p->f394 - 5; ... }` "reproduces it
 *     INCLUDING the second ldrh" IS REFUTED. *** Neither the old body nor this
 *     one emits it; cse commons the two reads because no call separates them.
 *     Three other loop shapes were tried and all are worse (table below).
 *
 * MEASURED THIS BATCH (objcmp / ref-ours insns / bytes / tryc --align):
 *   old body, `int a, b`                161  175/176  408/408   53
 *   THIS BODY, `unsigned short a, b`    176  175/168  408/392   32
 *   + `while (i != p->f394 - 5) i++;`   177  175/170  408/396   34
 *   + `for (i=0; i != p->f394-5; i++);` 177  175/169  408/396   40
 *   + `do {i++;} while (i != p->f394-5);` 175 175/168 408/392   --
 *   + `unsigned short k`                176  175/173  408/408   85
 *   + `kk = k; if (kk > 5)`             176  175/168  408/392   32  (cse unifies;
 *                                                     BIT-IDENTICAL to this body)
 *   + `bb = b;` at the top of the if    161  175/171  408/400   46
 *   + `bb`, `aa` and `kk` all three     161  175/171  408/400   46
 *   `ta`/`tb` temps outside, copied in  172  175/177  408/412   --   (this is the
 *                                            old header's recorded 172)
 *   autosweep on the OLD body: flag axis best 161 (`-fno-schedule-insns2` reads
 *   157 but its aligned residue is 69 -> 80, NOT progress); anchor axis 40 of 49
 *   boundaries, all inert -- boundaries 40..48 remain UNTESTED.
 *   Flag axis CROSSED with `unsigned short a, b`: no production group helps.
 *   (`-fno-force-mem`, which is NOT a Makefile group, takes aligncmp's differing
 *   count 50 -> 46 -- recorded only as a hint at the mechanism.)
 *
 * ================= CARRIED FORWARD =================
 * 1. THE PAGE COUNTER IS `unsigned`.  `unsigned int k` gives the ROM's `bhi`
 *    where `int` gives `bgt`.  Worth 13 (174 -> 161) when it was found.
 * 2. A HALFWORD DECREMENT IS `add` WITH A POOLED 0xffff, NOT `sub #1`.
 *    `p->f39c = p->f39c - 1;` written against the MEMBER stays in HImode and
 *    gives `ldr r1,=0xffff / ldrh r3,[r5] / add r3,r1 / strh r3,[r5]`; routed
 *    through an `int` local it becomes `sub r3,#1`.  Same for `n->step = -0xc;`
 *    (pooled 0xfff4) while the +0xc store three blocks above is `mov r2,#0xc`.
 * 3. THE EMPTY LOOP AT .L1b8e0 IS REAL and is reproduced: a loop whose only
 *    effect is to leave r1 == r3.  gcc-2.96 has no pass that deletes it.
 * 4. FIELD 0x10 OF THE NODE is read both as `ldrh` (feeding a halfword store,
 *    so combine drops the extension) and as `ldrsh` (a real signed compare).
 *    ONE `short y;` member gives both.
 * 5. Older negatives, still negatives: a named `unsigned short *pf = &p->f39e;`
 *    for both reads (161); splitting `k = p->f394; k = k - 5;` (161);
 *    member-direct arms in the k<=5 else branch (177).
 *
 * THE ROM'S REGISTER PLAN, for whoever picks this up: r7 = p; r6 = &p->f39e
 * from the prologue, REUSED as the long-lived `b` inside the `(a|b) != 0` arm
 * via `mov r6,r2` (one hard register, two roles, mutually exclusive paths);
 * r5 = &p->f39c, then reused as `n` in the else block; r1 = `k` and `a`.  The
 * inner else (`p->f39e = p->f39e - 1`), the `.L1b8e6` block and the common tail
 * all RECOMPUTE `&p->f39e` (`ldr r3,=0x39e / add r2,r7,r3`), so the address is
 * live only from the prologue to the (a|b) test and into the `k <= 5` arm.
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
    unsigned short a, b;
    int i, v;
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

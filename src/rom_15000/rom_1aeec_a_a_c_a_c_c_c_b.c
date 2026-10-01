/* Func_801b664 (PagePartyListForward) -- LANDS.
 * BYTE-IDENTICAL: 0 of 200 encodings differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_15000/rom_1aeec_a_a_c_a_c_c_c_b.c (LANDED; was src/non_matching/rom_15000/801b664.c) \
 *     asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c.s --func Func_801b664
 * objcmp: "OK Func_801b664 -- 428 bytes, 200 encodings and 9 relocations identical".
 * Production flags, no per-file Makefile row, no extra switch.  NOT flag-conditional:
 * -fno-strict-aliasing is explicitly NOT used -- see the correction below.
 *
 * LANDING PREREQUISITES
 *   1. *** THE REFERENCE HOLDS FOUR FUNCTIONS *** -- Func_801b664, Func_801b810,
 *      Func_801b9a8, Func_801b9ec -- so this cannot convert whole until the other
 *      three land, or until tools/split_s.py cuts it.  `split_s.py --dry-run` only.
 *      Its backward twin Func_801b810 is still parked at 161 of 190 and the twin
 *      should be re-read with the correction below before anyone retries it.
 *   2. tools/datacheck.py prints NOTHING and exits 0 -- NO text/data split, no
 *      data-object export.
 *   3. tools/shimcount.py reports 0 pins -- NO fakematch.txt row.
 *   4. the generated asm/rom_15000/rom_1aeec_a_a_c_a_c_c_c.s belongs in the commit.
 *
 * WHAT CLOSED THE LAST 2.  The peeled tail becomes, with the `do { } while (0)`
 * sched barrier DELETED OUTRIGHT:
 *
 *     n->ty = p->f396;
 *     n->f1a = *(volatile unsigned short *)&p->f398;
 *     n->step = -0xc;
 *
 * i.e. the barrier goes away and the tail's f398 read goes through a pointer cast.
 * FIVE spellings of the same escape are byte-exact, all measured:
 *     f398 read cast                                    (SHIPS)   0
 *     f398 read as *(volatile unsigned short *)((unsigned char *)p + 0x398)  0
 *     ty store cast, `*(short *)&n->ty = p->f396;`                 0
 *     ty store cast AND f398 read cast                             0
 *     both reads cast (f396 and f398)                              0
 * Exactly ONE fails: casting the f396 READ ALONE and leaving f398 a member read
 * is 11 differing.  So the escape must land on the pair (ty store, f398 read);
 * putting it on (f396 read, ty store) does nothing.
 *
 * *** THE PARK'S BLOCKER DIAGNOSIS WAS WRONG IN BOTH HALVES, AND BOTH ERRORS MATTER.
 * ***
 * *** (a) "PASS .19.sched2 ... the barrier ends region 1 at the ty store, so
 * ***     `mov r1,#0xe6` is region 2's first instruction and cannot move up."
 * ***     THERE IS NO REGION SPLIT.  The .23.sched2 dump says
 * ***     ";; -- basic block 5 from 238 to 854" and ";; --- Region Dependences ---
 * ***     b 5 bb 0" -- the whole tail is ONE basic block and ONE region, barrier or
 * ***     no barrier.  What the `do { } while (0)` actually leaves behind is a
 * ***     DEPENDENCE BARRIER INSIDE that one block: with it, insn 977 (`mov r1,#230`)
 * ***     has FOUR incoming dependencies, on insns 926, 245, 247 and 261 -- every
 * ***     insn ahead of it, including the ty store -- and so cannot be scheduled
 * ***     until the store is done.  Delete the barrier and the same insn (then
 * ***     numbered 964) has ZERO incoming dependencies and is ready at t = 0.
 * ***     That is a dependence fact, not a region boundary, and it is why the park's
 * ***     "three source orders x four barrier positions" cross could not reach it:
 * ***     every barrier position creates the same all-before -> all-after edge set,
 * ***     only at a different point.
 * ***
 * *** (b) "-fno-strict-aliasing IS NOT THE BETTER ROUTE HERE ... volatile is
 * ***     strictly better."  Half right, and the half that is wrong is the useful
 * ***     half.  The park measured the flag WITHOUT volatile.  The full cross,
 * ***     measured here (objcmp, of 200):
 * ***         volatile + barrier                   (the park)        2
 * ***         volatile + barrier + nsa                               5
 * ***         volatile, no barrier                                  11
 * ***         volatile, no barrier, + nsa                            3
 * ***     The last row is the tell.  Dropping the barrier AND turning strict
 * ***     aliasing off fixes the TAIL COMPLETELY -- indices 65/66 come out right --
 * ***     and leaves residue ONLY in the loop, at indices 54-56, where the step
 * ***     store floats above the ty store.  So the loop wants strict aliasing ON and
 * ***     the tail wants it OFF.  That is not a flag question at all; it is a
 * ***     request for a LOCAL escape in the tail, which is what ships.
 * ***
 * *** THE MECHANISM, for the next person.  In the no-barrier dump the ty store
 * *** (insn 261) has priority 3 -- its only dependents are the block-end jump and an
 * *** anti-edge to `lsl r2,#16` -- so it sinks to tenth in the block while the ROM
 * *** issues it fifth.  The two volatile loads f396 and f398 are ordered against
 * *** EACH OTHER (volatile-to-volatile), but a volatile load is NOT ordered against
 * *** the non-volatile `n->ty` store, because strict aliasing proves struct Node and
 * *** struct Party cannot overlap.  Giving the tail's f398 read an alias set that
 * *** conflicts with the ty store adds the missing edge, lifts the store's priority,
 * *** and pins the f398 address build after it -- which is precisely the slot the
 * *** ROM spends on `mov r1,#0xe6`.  The barrier was a blunt instrument standing in
 * *** for that one edge, and it bought the edge at the cost of forbidding the swap.
 * ***
 * *** THIS IS THE SAME LEVER THAT LANDED Func_8018efc IN THIS BATCH, from the other
 * *** side: there a LOAD was one dependent short of winning a sched2 tie and the
 * *** cast supplied it.  Read the two together.
 *
 * STILL TRUE FROM THE OLD PARK, keep every one:
 *  1. DO NOT NAME p->f394 IN A LOCAL -- cse's path-following wants two loads, and
 *     `cse_end_of_basic_block` following the unconditional `b .L1b7b2` into the
 *     else arm is what makes the asymmetry.  Worth 59.  The `> 5` test then needs
 *     `(unsigned int)p->f394` to keep `bhi`.
 *  2. THE TWO LOOP-INVARIANT LOADS ARE volatile.  Worth 145.  `loop_invariant_p`
 *     lets the f396/f398 loads float out of the .L1b6d2 loop preheader because
 *     DIFFERENT_ALIAS_SETS_P holds; volatile blocks the hoist and gives exactly one
 *     load per source read.  STILL REQUIRED -- the pointer cast added here does NOT
 *     replace it, it only supplies the one extra sched2 edge in the TAIL.
 *     Flagged for the owner, unchanged: volatile on two window fields is a
 *     MODELLING claim, not just a codegen device, and wants a look.
 *  3. THE DOCUMENTED ONE-MEMBER UNION LEVER DOES NOT REACH EITHER PASS.  All five
 *     union forms were byte-identical to no change.  CONFIRMED INDEPENDENTLY this
 *     batch on Func_8018efc, where `((union HW *)&win->w)->v` was likewise
 *     byte-identical while the plain pointer cast landed the function.  So the
 *     correction to that entry is now double-sourced: use a POINTER CAST, not a
 *     one-member union, when you want a MEM's alias set widened.
 *  4. AN EMBEDDED UNION AT AN ODD-HALFWORD OFFSET SILENTLY BREAKS THE LAYOUT.
 *     ARM's STRUCTURE_SIZE_BOUNDARY is 32, so a one-short union is padded to 4
 *     bytes; at 0x396 it pushed every later field +4 and READ 154 of 199 WITH SIZE
 *     MATCHING -- BETTER than the correct layout's 156.  Check pool CONSTANTS
 *     against the reference before believing an improvement.
 *  5. THE THREE NODE STORES GO ty, f1a, step IN BOTH PLACES.  Worth 4.
 *  6. b - 1 ON A HALFWORD MEMBER, NOT VIA AN int LOCAL, for the ROM's
 *     `ldr r1,=0xffff / add r3,r2,r1` HImode form.
 *  7. THE TWIN'S BLOCKER DOES NOT APPLY HERE.  Func_801b810's park names "one
 *     allocno too many"; this ROM's prologue buys both r8 and r10 itself.
 *
 * RETIRED.  The old park's whole "3b. THE sched2 BARRIER GOES BETWEEN THE ty STORE
 * AND THE f398 LOAD" section, and its "WHY IT IS A GENUINE VICE" argument, are
 * superseded: the barrier is not needed at all and was never buying what it looked
 * like it was buying.  Its measured table (ty/barrier/f1a/step 2, other orders
 * 7-9, the asm-barrier form identical at 2) is still correct AS A MEASUREMENT of
 * the barrier forms -- it is just no longer the frontier.
 *
 * ALSO MEASURED AND WORSE / INERT, unchanged: naming a = p->f39c  -2 insns;
 * f396/f398 as signed short  inert; f1a as unsigned short  inert; volatile on
 * Node.step  204 insns, 154 differing; the wait loop as an explicit if + do/while
 * inert; a 4-short union on Node 0x14  inert.
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
    short f1a;                          /* 0x1a */
};
struct Party {
    unsigned char pad0[8];
    unsigned short f8;                  /* 0x008 */
    unsigned short fa;                   /* 0x00a */
    unsigned char pad1[0x3c - 0xc];
    unsigned short f3c;                 /* 0x03c */
    unsigned short f3e;                 /* 0x03e */
    unsigned char pad2[0x348 - 0x40];
    struct Node *head;                  /* 0x348 */
    unsigned char pad3[0x354 - 0x34c];
    unsigned short xs[0x10];            /* 0x354 */
    unsigned short ys[0x10];            /* 0x374 */
    unsigned short f394;                /* 0x394 */
    volatile unsigned short f396;       /* 0x396 */
    volatile unsigned short f398;       /* 0x398 */
    unsigned char pad4[0x39c - 0x39a];
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

void Func_801b664(struct Party *p)
{
    struct Node *n;
    unsigned int m;
    int v;

    Func_801b9a8(p, p->f39e);
    p->f3a2 = 0x21;
    WaitFrames(1);
    p->f39e = p->f39e + 1;
    if ((unsigned int)p->f394 > 5) {
        m = p->f39c + p->f39e;
        if (m == p->f394) {
            n = p->head;
            p->fa = 0;
            if (n->next != 0) {
                do {
                    n->ty = p->f396;
                    n->step = -0xc;
                    n->f1a = p->f398;
                    n = n->next;
                } while (n->next != 0);
            }
            n->ty = p->f396;
            n->f1a = *(volatile unsigned short *)&p->f398;
            n->step = -0xc;
            while (n->ty != n->y)
                WaitFrames(1);
            n = p->head;
            if (n != 0) {
                unsigned short *q = &p->xs[0];
                do {
                    Func_801bd98(q[0], q[0x10], n, 1);
                    n = n->next;
                    q++;
                } while (n != 0);
            }
            p->f39e = 0;
            p->f39c = 0;
            n = p->head;
            v = n->y + 0x10;
            n = n->next;
            if (n != 0) {
                do {
                    n->ty = v;
                    n->step = 0xc;
                    n = n->next;
                    v += 0x10;
                } while (n != 0);
            }
            p->f3e = 1;
        } else if (p->f39e == 4 && m + 1 < p->f394) {
            p->f39e = p->f39e - 1;
            p->f3c = 8;
            p->f39c = p->f39c + 1;
            Func_801ba68(p, 1);
            if (p->f39c + p->f39e + 2 == p->f394)
                p->f3e = 0;
            p->fa = 1;
        }
    } else {
        if (p->f39e == p->f394)
            p->f39e = 0;
    }
    p->f3a2 = 1;
    Func_801b9ec(p, p->f39e);
    WaitFrames(1);
    Func_801b010(p->head->id, 0);
    WaitFrames(1);
}

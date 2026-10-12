/* OvlFunc_920_20087f8 -- NON-MATCHING, 32 differing STREAM encodings of 119.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch_elev/b256/small/final/ovl_30_c_c_a_c.PARK.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 119 / ours 119 stream encodings; 103/103 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7a6ae4/20087f8.c \
 *     asm/overlays/rom_7a6ae4/ovl_30_c_c_a_c.s --func OvlFunc_920_20087f8
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
/* OvlFunc_920_20087f8  --  0x020087f8, overlay rom_7a6ae4, ovl_30_c_c_a_c.
 *
 * PARKED at 32 of 119 encodings.  SIZE (268) and RELOCATION COUNT and ORDER are
 * IDENTICAL, the instruction stream is the ROM's insn-for-insn, and every one of
 * the 32 is a LOW-REGISTER ROTATION inside three otherwise-exact blocks.
 *
 *   XX ENCODINGS differ in 32 place(s) (ref 119, ours 119)
 *   first at index 9: ref 4c3a (ldr r4,=.L1064)  ours 493a (ldr r1,.L6)
 *
 * "Push the block one tile the leader is facing."  A cousin of the eighteen-copy
 * TryPushBlockOneTile family solved in src/overlays/rom_7b4558/
 * ovl_30_a_a_c_a_c_a.c, which supplied the struct, `d & 0xffff0000` / `d <<= 16`
 * for the packed table entry, the `*(short *)((char *)p + 0xa)` casts and the
 * `union { int u; }` fields.  This variant probes with tile coordinates
 * (`(field + dx) >> 4`) instead of the v[] vector, so it is NOT a substitution
 * of that source.
 *
 * WHAT WAS SOLVED (116 -> 32):
 *
 *   lever                                                        differing
 *   -----------------------------------------------------------  ---------
 *   first candidate, plain C                                        116   +
 *                                                                   4 bytes SHORT
 *   + `int *q = v;`  -- THE PUSH TELL.  The ROM pushes r8, r9 AND
 *     r10; plain C pushes only r8 and r10, because gcc reuses r7
 *     for the table base and then for the `mov r7,sp`.  A named
 *     pointer to the stack vector makes the two conflict, and the
 *     table base lands in r9 exactly as the ROM has it.  Size
 *     matches from here on.                                          61
 *   + `q = v;` placed just before `b->f22 = 2;` (sweep below)         41
 *   + `d <<= 16;` as its OWN statement instead of `(short)d`, at
 *     both probe sites -- the ROM's `lsl r3,#16 / asr r3,#16` is
 *     DESTRUCTIVE on d, which only the compound assignment gives     37
 *   + drop the named `y` local, keep the named `x`                    32
 *
 * `q = v;` PLACEMENT SWEEP, twelve positions, and it is a clean unimodal curve:
 *   first statement 55 | after `i =` 47 | after `d =` 47 | after `x =` 47 |
 *   after the b-guard 45 | before the o call 43 | after the o-guard 41 |
 *   after `if (o != 0) return;` 32 | before `b->f22 = 2;` 32 |
 *   after `b->f22 = 2;` 32 | before `q[0] =` 116 (loses r9 again)
 *
 * ------------------------------------------------------------------- NEW ----
 * THE FLOOR IS A LOW-REGISTER ROTATION, AND THE DISCRIMINATORS FIRED IN THE
 * RECORDED ORDER -- control flow, then alias, then allocation, then order.
 *
 * 1. CONTROL FLOW.  Three early `return`s cross-jumped to one exit is the ROM's
 *    shape; the fully nested `if (b != 0) { if (o == 0) { if (<=0) {...} } }`
 *    is EXACTLY 32 as well.  Eliminated.
 *
 * 2. ALIAS -- ELIMINATED BY THE ONE-COMPILE DIAGNOSTIC, THEN CONFIRMED.
 *    `-fno-schedule-insns2` REGRESSES (69 -> 77 differing lines), so sched2 is
 *    already producing the ROM's order and an alias lever cannot reach this.
 *    Confirmed by measurement anyway, because this function's family is the one
 *    where alias set 0 WAS the answer: wrapping the two high-half reads in
 *    `union { int u; struct { short lo; short hi; } h; }` and reading
 *    `p->x.h.hi` / `p->z.h.hi` is INERT at 32, as is `((short *)p)[5]`.
 *    The twin's `{z,f24,f2c}` union set is already present and is load-bearing
 *    for the tail, which IS exact here.
 *
 * 3. ALLOCATION.  Fifteen spellings, ALL exactly 32: a named `struct Actor *`
 *    copy for the second field read (the ROM's `mov r4,r8`); `unsigned int i`;
 *    per-site `x1/y1/x2/y2` (WORSE, 41); naming the SUM instead of the shifted
 *    value; `((short *)p)[5]` subscripts; the union fields above; the nested
 *    control flow above.  Adding pressure with a named `dx` (86) or `dx`+`dy`
 *    (83) is much worse; a named `int *t = L1064;` in any of three positions is
 *    112-114 and 8 bytes short.
 *
 * 4. ORDER -- run LAST, as recorded, and INERT.  All 24 permutations of the
 *    four scalar local declarations (`q`, `i`, `d`, `x`) give exactly 32.  This
 *    is a clean corpus datum for the "declaration order is usually inert"
 *    entry: 24 of 24, no spread at all, on a function whose residue IS
 *    allocation.  Register pins at the two probe call sites (PIN2, both fill
 *    directions) are WORSE at 41.
 *
 * THE BLOCKER CLASS.  Same instruction stream, one register rotation, in the
 * three blocks that load .L1064[i].  Block 2 is IDENTICAL insn-for-insn and
 * differs ONLY in register names; block 1 differs by a two-insn transposition
 * that is a CONSEQUENCE of the names (the ROM reuses r4 for both the table-base
 * reload and the `mov r4,r8` copy of `p`, which creates an anti-dependence that
 * pulls `mov r9,r4` earlier; ours uses r1 and r2, has no such dependence, and
 * sched2 lets `mov r9,r1` float to the end of the block).  So the ROOT is
 * reload's choice of low register for the r9 pseudo, and the ordering is
 * downstream of it:
 *
 *   ROM    ldr r4,=.L1064 ... ldr r3,[r4,r5] ... mov r9,r4 / mov r4,r8
 *   ours   ldr r1,.L6     ... ldr r4,[r1,r5] ... mov r2,r8 / mov r9,r1
 *
 * This is the shape docs/elevation.md records as "A recurring residue class:
 * the parameter pointer one register too low -- same instruction stream, one
 * register rotation ... the cause is in `find_reg`'s conflict/preference pass",
 * seen here on the CALL-USED registers rather than the callee-saved ones.  The
 * recorded entry says declaration reordering, copying into a local and deleting
 * a named pointer are all inert against it; all three were measured here and
 * all three are inert.  THE FLOOR IS 32 AND NO SOURCE-LEVEL AXIS MOVED IT.
 *
 * WHAT WOULD MOVE IT.  Nothing at the C level that was reachable in this round.
 * The next thing to try is the same one the REG_ALLOC_ORDER entry in HANDOFF.md
 * names: read `.18.greg` for this function against a candidate whose block-2
 * registers are right, and find what changes reload's preference.  Do not spend
 * another round on source permutations -- fifteen of them agree on 32.
 *
 * ---------------------------------------------------------------- LANDING --
 * (recorded for whoever closes it -- nothing lands from this park)
 *   tools/asmfacts.py -> WHOLE  convert directly.  ONE function, no data.
 *   tryc.makefile_flags('overlays/rom_7a6ae4/ovl_30_c_c_a_c') = set(); the only
 *   rom_7a6ae4 Makefile rule names ovl_30_c_a_c_c_a_c_c_c.o LITERALLY, so there
 *   is no wildcard hazard on this stem.
 *   The .ld line stays VERBATIM on the asm/ path:
 *       overlays/rom_7a6ae4/overlay.ld:40
 *           asm/overlays/rom_7a6ae4/ovl_30_c_c_a_c.o(.text)
 *   `.L1064` is ALREADY `.global .L1064` at
 *   asm/overlays/rom_7a6ae4/ovl_30_c_c_c.s:4 (label at :46) -- no export to add.
 *   OvlFunc_920_20087c4 is already a .c (src/overlays/rom_7a6ae4/ovl_30_c_c_a_b.c,
 *   which types it `unsigned int (unsigned int, unsigned int)`); it is declared
 *   here returning `struct Actor *` because r0 is used as a base pointer at the
 *   first probe site.  OvlFunc_920_2008304 is in
 *   asm/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a_c_c_c.s.
 *
 * -- worked in scratch_elev/b256/small
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short facing;
    int x;
    int y;
    union { int u; } z;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
    unsigned char pad23;
    union { int u; } f24;
    unsigned char pad28[0x2c - 0x28];
    union { int u; } f2c;
    int f30;
    int f34;
};

extern int L1064[] __asm__(".L1064");
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_WaitMovement(struct Actor *a);
extern int __TestCollision(struct Actor *a, int *v);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern struct Actor *OvlFunc_920_20087c4(int x, int y);
extern void OvlFunc_920_2008304(void);

void OvlFunc_920_20087f8(void)
{
    int v[3];
    struct Actor *p;
    struct Actor *b;
    struct Actor *o;
    int *q;
    int i;
    int d;
    int x;

    p = __MapActor_GetActor(0);
    i = p->facing >> 12;
    d = L1064[i];
    x = (*(short *)((char *)p + 0xa) + (d >> 16)) >> 4;
    d <<= 16;
    b = OvlFunc_920_20087c4(x, (*(short *)((char *)p + 0x12) + (d >> 16)) >> 4);
    if (b == 0)
        return;
    d = L1064[i];
    x = (*(short *)((char *)b + 0xa) + (d >> 16)) >> 4;
    d <<= 16;
    o = OvlFunc_920_20087c4(x, (*(short *)((char *)b + 0x12) + (d >> 16)) >> 4);
    if (o != 0)
        return;
    q = v;
    b->f22 = 2;
    d = L1064[i];
    q[0] = b->x + (d & 0xffff0000);
    q[1] = b->y;
    d <<= 16;
    q[2] = b->z.u + d;
    if (__TestCollision(b, q) > 0)
        return;
    __Actor_SetAnim(p, 8);
    __WaitFrames(0xf);
    __PlaySound(0xb9);
    b->f30 = 0x3333;
    b->f34 = 0x3333;
    __Actor_TravelTo(b, q[0], q[1], q[2]);
    p->f30 = 0x3333;
    p->f34 = 0x3333;
    __Actor_TravelTo(p, q[0], q[1], q[2]);
    __Actor_WaitMovement(b);
    b->x = q[0];
    b->z.u = q[2];
    b->f24.u = (int)o;
    b->f2c.u = (int)o;
    __Actor_SetAnim(p, 1);
    OvlFunc_920_2008304();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_920_20087f8 -- 0x020087f8, asm/overlays/rom_7a6ae4/ovl_30_c_c_a_c.s
 *
 * 32 differing of 119, AT THE ROM'S EXACT SIZE (268 bytes) with the relocation
 * count and ORDER identical, and the instruction stream the ROM's insn for insn.
 * Best candidate: scratch_elev/b256/small/final/ovl_30_c_c_a_c.PARK.c; the full
 * sweep is in that directory's NOTES.md.
 *
 * 116 -> 32 CAME FROM THE PUSH TELL, and it is a clean example of it. The ROM
 * pushes r8, r9 AND r10; plain C pushes two, because gcc reuses r7 for the table
 * base and then again for `mov r7, sp`. `int *q = v;` makes those conflict and
 * r9 appears. Then a 12-position placement sweep of `q = v;` -- UNIMODAL, at
 * 55/47/47/47/45/43/41/32/32/32/116 -- plus `d <<= 16;` as its own statement at
 * both probe sites, and DELETING a named `y` local.
 *
 * BLOCKER CLASS: low-register rotation from reload's preference pass. This is
 * the recorded "parameter pointer one register too low" shape, seen here on the
 * CALL-USED registers rather than the callee-saved ones. Block 2 is identical
 * insn for insn and differs only in register names; block 1's transposition is
 * downstream of it -- the ROM reuses r4 for both the table-base reload and
 * `mov r4, r8`, creating an anti-dependence that pulls `mov r9, r4` earlier.
 *
 * DISCRIMINATORS RUN IN THE RECORDED ORDER, AND THREE AXES ARE CLOSED:
 *
 *   CONTROL FLOW -- the nested form measures exactly 32. Eliminated.
 *   ALIAS -- `-fno-schedule-insns2` REGRESSES, 69 -> 77, so by the sign rule
 *            sched2 is already producing the ROM's order. Confirmed anyway with
 *            the union high-half fields that solved this function's OWN 18-copy
 *            family: INERT at 32, not one encoding moved.
 *   ALLOCATION -- 15 spellings, all exactly 32.
 *   ORDER, run last -- ALL TWENTY-FOUR declaration permutations give exactly
 *            32. That is a clean 24-of-24 datum for "declaration order is
 *            usually inert". Pins at the probe sites are WORSE, 41.
 *
 * So do not re-spend control flow, alias, or declaration order here.
 *
 * NEXT STEP IS `.18.greg`, NOT MORE SOURCE PERMUTATIONS. The residue is a
 * register naming rotation with the schedule already correct, which is what the
 * reload dump shows directly.
 *
 * LANDING SHAPE IF CLOSED: src/overlays/rom_7a6ae4/ovl_30_c_c_a_c.c.
 * `asmfacts.py` says WHOLE -- convert directly. One function, no data, no
 * split. `makefile_flags()` is empty with no wildcard hazard. The one `.ld`
 * line, overlays/rom_7a6ae4/overlay.ld:40, MUST KEEP its asm/ path. The data
 * label it reads is already `.global` in a sibling `.s` -- grepped.
 *)

*/
#endif

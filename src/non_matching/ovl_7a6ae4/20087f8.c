/* OvlFunc_920_20087f8 -- NON-MATCHING, 31 differing STREAM encodings of 119.
 *
 * MEASURED batch 343 brief D (was 32).  ref 119 / ours 119 stream encodings,
 * 103/103 16-bit INSTRUCTIONS, 13/13 RELOCATIONS IDENTICAL.  IN PHASE on both
 * counts, so 31 is a TRUE DISTANCE.  PINS: 0 (tools/shimcount.py -- the
 * `do { } while (0);` anchor emits nothing and is not a shim).
 * datacheck.py: no data under the reference.  tools/piece.py: the reference .s
 * holds THIS FUNCTION ALONE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7a6ae4/20087f8.c \
 *     asm/overlays/rom_7a6ae4/ovl_30_c_c_a_c.s --func OvlFunc_920_20087f8
 *
 * ------------------------------------------------------------------- 32 -> 31
 * 32 -> 31 came from ONE `do { } while (0);` anchor, placed AFTER the SECOND
 * probe's `x = (*(short *)((char *)b + 0xa) + (d >> 16)) >> 4;`.  Found by
 * tools/autosweep.py --anchor, which the previous header had never run: the
 * park's own claim that "fifteen source permutations agree on 32" was about
 * SPELLINGS, and the anchor axis was simply untested.  It is a one-encoding
 * improvement, not a mechanism, and it is recorded so the next agent does not
 * re-find it.
 *
 * THE ANCHOR AXIS IS NOW CLOSED, 37 of 37 placements plus six hand crossings:
 *   after the 2nd probe's `x = ...`                31  <- THIS BODY
 *   after the 2nd probe's following `d <<= 16;`    33
 *   BOTH of those together                         32
 *   after `q = v;`                                 32
 *   after `b->f22 = 2;`                            32
 *   after the 1st probe's `x = ...`                33
 * THE FLAG AXIS IS CLOSED on BOTH bodies.  Crossed against the 31-body:
 *   -fno-gcse 31, -fno-strict-aliasing 31, -fno-strength-reduce 31,
 *   -fno-rerun-loop-opt 31, -fno-schedule-insns2 54, -O1 62, -ffixed-r7 125,
 *   -fno-rerun-cse-after-loop 116.  Nothing crosses.
 *
 * -------------------------------------------------- WHAT THE 31 ACTUALLY IS
 * The park's "low-register rotation" verdict SURVIVES, and the side-by-side
 * disassembly localises it to the TWO PROBE BLOCKS and nothing else.  Block 3
 * (the `q[]` setup), the collision test, both `__Actor_TravelTo` calls, the
 * whole tail and the epilogue are byte-identical.
 *
 *   idx  9-26   first probe, 15 encodings
 *   idx 31-43   second probe, 13 encodings
 *   idx 52-56, 63-64    the `q[]` block's reads of the same two pseudos, 5
 *
 * The root is which low register receives the TABLE BASE and which receives the
 * loaded table word:
 *
 *   rom    ldr r4,[pc,#232]  ...  ldr r3,[r4,r5]  ...  mov r9,r4 / mov r4,r8
 *   ours   ldr r1,[pc,#232]  ...  ldr r4,[r1,r5]  ...  mov r2,r8 / mov r9,r1
 *
 * The ROM reuses r4 for BOTH the table-base load and the `mov r4,r8` copy of
 * the actor pointer, which creates the anti-dependence that pulls `mov r9,r4`
 * earlier; ours splits them across r1 and r2 and has no such dependence, so
 * sched2 lets `mov r9,r1` float to the end of the block.  Every differing
 * encoding in both probe blocks is downstream of that one choice -- the
 * instruction MULTISET of each block is identical, only the register names and
 * the resulting order differ.  Confirmed by reading the two streams
 * index-aligned, not inferred from a figure.
 *
 * NEXT STEP IS STILL `.18.greg`/`.17.lreg`, and the question is now sharper
 * than the old header put it: not "what changes reload's preference" in general
 * but WHAT MAKES THE TABLE-BASE PSEUDO AND THE `mov rX,r8` COPY SHARE ONE
 * REGISTER.  Do not spend another round on source permutations -- twenty-one
 * now agree.
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
    do { } while (0);
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

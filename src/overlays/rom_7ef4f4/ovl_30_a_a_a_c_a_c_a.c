/* OvlFunc_965_20080c4 -- TryPushBlockOneTile, overlay rom_7ef4f4, ovl_30_a_a_a_c_a_c_a.
 *
 * ONE OF EIGHTEEN BYTE-IDENTICAL COPIES.  The method, the levers, the
 * measurements and the family table are in the sibling
 * src/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_a_a_b.c, which is this same
 * source with three substitutions:  OvlFunc_965_20080c4 / .L2fd4 / OvlFunc_965_200806c.
 *
 *   OK OvlFunc_965_20080c4 -- 384 bytes, 176 encodings and 14 relocations identical
 *   (three passes, together with the other seventeen)
 *
 * LANDING: WHOLE, no split, no linker edit.  asmfacts.py says
 * `WHOLE  convert directly`; the .s holds one function and zero
 * .section/.global/incbin/.lcomm.  tryc.makefile_flags() = set().
 * The single .ld line stays VERBATIM on the asm/ path:
 *   overlays/rom_7ef4f4/overlay.ld:19
 *       asm/overlays/rom_7ef4f4/ovl_30_a_a_a_c_a_c_a.o(.text)
 * `.L2fd4` is referenced here and defined in another object; it is ALREADY
 * `.global` in asm/overlays/rom_7ef4f4/ovl_30_c_c_c.s -- grepped, no export to add.
 */
/* OvlFunc_965_20080c4  --  TryPushBlockOneTile, overlay 968 (rom_7f2f14),
 * ovl_30_a_a_a_c_c_a_a_b.
 *
 * EXACT.  Re-run eight times:
 *
 *   OK OvlFunc_965_20080c4 -- 384 bytes, 176 encodings and 14 relocations identical
 *
 * ===========================================================================
 * THIS CLOSES src/non_matching/ovl_780898/20080c4.c, THE PARK THAT COVERS
 * EIGHTEEN FUNCTIONS -- and all eighteen were verified byte-identical from
 * this one source, three times each.  See the family list and landing table at
 * the bottom.  The park itself calls this "the largest single lever left in the
 * tree"; the body it left behind was 169 of 176 correct and every word of its
 * three solved observations is kept here unchanged.
 * ===========================================================================
 *
 * THE RESIDUE THE PARK COULD NOT MOVE.  Seven instructions in the tail, same
 * multiset, same registers, different order:
 *
 *     rom    mov r1,sl / str r3,[r6,#0x10] / str r1,[r6,#0x24]
 *              / str r1,[r6,#0x2c] / movs r3,#0x80 / mov r2,r8 / lsls r3,#24
 *     ours   str r3,[r6,#0x10] / movs r3,#0x80 / mov r2,r8 / mov r1,sl
 *              / lsls r3,#24 / str r1,[r6,#0x24] / str r1,[r6,#0x2c]
 *
 * gcc hoists the whole `p`-side constant build above the two `b`-side zero
 * stores; the ROM issues the zero stores first and defers the constant.  Two
 * previous rounds tried fifteen spellings (statement permutations in the tail,
 * a named `k` for 0x80<<24 in three positions, a second pointer local for `p`,
 * literal zeros, an extra copy of `z`), and the park's own conclusion was
 * "anything that works here has to change what the STORES depend on, not what
 * the POINTER depends on".  That conclusion is exactly right, and it names the
 * lever it did not reach.
 *
 * ------------------------------------------------------------------- NEW ----
 * ALIAS SET 0 IS A **SCHEDULING** LEVER, NOT ONLY A LOAD/STORE-FORM LEVER, AND
 * IT IS APPLIED AS A SET OF FIELDS.
 *
 * The recorded entry for the one-member union ("a union member access is ALIAS
 * SET 0 and the member list is irrelevant -- even a one-member union; a struct
 * with the same member is not") is written from cases where it changes WHICH
 * MEMORY FORM gcc emits.  It also changes INSN PRIORITY in sched2, and that is
 * what this function needed.
 *
 * MECHANISM.  gcc's haifa scheduler orders the ready list by INSN_PRIORITY --
 * the longest dependence path from an insn to the end of the block -- and after
 * reload it breaks ties by original order.  Two same-base different-offset MEMs
 * disambiguate on the offset regardless of alias set, so the tail's stores
 * start out MUTUALLY INDEPENDENT, the chain feeding `mov r1,sl` is short, and
 * the constant build wins the ready list.  Wrapping a field in a one-member
 * union gives its accesses MEM_ALIAS_SET 0, which conflicts with EVERY other
 * memory reference; the stores through it become chained to the rest, the path
 * behind `mov r1,sl` lengthens, and its priority rises above `movs r3,#0x80`.
 *
 * IT IS A SET, AND ITS WIDTH WAS FOUND BY MEASUREMENT.  Three fields --
 * `z`, `f24`, `f2c` -- and dropping ANY ONE of the three costs 3, 4 or 5
 * encodings.  All 21 one- and two-field subsets and all 29 supersets up to four
 * fields were measured.  The set is minimal and no superset is better.
 *
 *   union'd fields                                 differing
 *   --------------------------------------------  ---------
 *   {} (the park's candidate)                          7
 *   {x}  {y}  {z}  {f38}  {f40}                        7   inert, each alone
 *   {f24}  {f2c}                                       6
 *   {x,f24}  {x,f2c}  {f24,f38}  {f24,f40}  ...        6
 *   {z,f2c}                                            5
 *   {z,f24}                                            4
 *   {f24,f2c}                                          3
 *   {z,f24,f2c}                                        0   EXACT  <- ships
 *   {z,f24,f2c} + any one of {x,y,f30,f34,f38,f40}     0   (tie; the narrower
 *                                                          set ships)
 *   {f24,f2c} + any one of {x,y,f30,f34,f38,f40}       3   (still 3)
 *
 * THE THREE FIELDS ARE EXACTLY THE ONES WHOSE STORES ARE IN THE DISPUTED
 * WINDOW.  `b->z` is the store the constant build jumps over; `b->f24` and
 * `b->f2c` are the two stores that must precede it.  Nothing outside the window
 * matters, which is a usable rule for the next instance: union the fields whose
 * STORES ARE MIS-ORDERED, not the fields whose values are.
 *
 * IT IS A sched2 RESIDUE AND THAT WAS CONFIRMED BEFORE THE SEARCH.  With
 * `-fno-schedule-insns2` the same seven-insn window collapses to four differing
 * (and the function breaks elsewhere, 41 overall), which proves the pre-sched
 * RTL order is nearly the ROM's and that sched2 is what moves it.  That
 * measurement is what made an ALIAS lever -- which changes the dependence graph
 * sched2 works from -- the right thing to try, ahead of any further permutation
 * of the source.  Control flow, then alias, then allocation, then order: the
 * park had exhausted ORDER twice and never tried ALIAS.
 *
 * MEASURED WORSE / INERT this round (ref 176 encodings / 384 bytes),
 * on top of the park's own fifteen:
 *
 *   spelling                                              differing
 *   ---------------------------------------------------  ---------
 *   `register int zr __asm__("r1")` for the four zero      longer, all
 *     stores in the tail                                   relocations moved
 *   p->x hoisted above p->f38/f40                          longer
 *   p->f24/f2c moved above p->x                                7
 *   named locals `t0`/`t2` for v[0]/v[2]                       7
 *   literal `0` at all four zero stores                        7
 *   `{ int k; k = 0x80 << 24; ... }` after the b stores         7
 *   `zz = z;` copied before the b stores                       7
 *   `z` typed `unsigned char`                              longer
 *   `z` typed `unsigned int`                                   7
 *   `0x80000000` for `0x80 << 24`                              7
 *   `p` given its own distinct struct type (different
 *     alias set from `b`)                                      7
 *
 * The distinct-struct-type experiment is the informative negative: making the
 * two actors NOT alias removes dependences and is inert, while making three
 * fields alias EVERYTHING adds them and is exact.  The lever's direction is to
 * ADD dependence, and `-fstrict-aliasing` (on at -O2) is not the axis.
 *
 * KEPT FROM THE PARK, ALL THREE STILL LOAD-BEARING:
 *   1. the table entry packs two signed halfwords into one word, extracted as
 *      `d & 0xffff0000` for X and `d <<= 16` for Z;
 *   2. the shift is DESTRUCTIVE, so `d <<= 16;` as its own statement, not
 *      `d << 16` in the argument -- one instruction at each of three sites;
 *   3. the shorts at +0xa and +0x12 are the high halves of x and z and must be
 *      `*(short *)((char *)p + 0xa)`, not struct fields (declaring them as
 *      fields shifts every offset after +8 and costs 32 instructions).
 *
 * ------------------------------------------------------- LANDING (this TU) --
 *   asmfacts.py: WHOLE  convert directly
 *   The .s holds exactly one function and ZERO `.section` / `.global` /
 *   `incbin` / `.lcomm` lines.  One .ld line names the object and it STAYS
 *   VERBATIM on the asm/ path -- the build rule is `asm/%.o: src/%.c`, so a
 *   line rewritten to `src/...o` matches nothing and is SILENTLY IGNORED:
 *     overlays/rom_7f2f14/overlay.ld:24
 *         asm/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_a_a_b.o(.text)
 *   tryc.makefile_flags() = set() -- tree default; every rom_7f2f14 pattern
 *   rule is scoped to `ovl_30_c_a_c_a_c_a%` / `ovl_30_c_a_c_a_c_c%` and cannot
 *   reach this stem.
 *   `.L2fd4` IS REFERENCED HERE AND DEFINED IN ANOTHER OBJECT -- and it is
 *   ALREADY EXPORTED: `.global .L2fd4` at
 *   asm/overlays/rom_7f2f14/ovl_30_c_c_c_c_c.s:256, with the label at :266.
 *   Grepped; no export needs adding.
 *
 * ------------------------------------------------ THE OTHER SEVENTEEN --------
 * Every one of the eighteen is BYTE-IDENTICAL apart from the data label and the
 * two overlay-local callee names, so this source lands all of them with three
 * substitutions.  All eighteen were built and compared, three passes, 18/18 OK
 * each pass.  Every one is `funcs=1 data=0`, every one has `makefile_flags() =
 * set()`, every one has EXACTLY ONE .ld line on the asm/ path in its own
 * overlay, and every one's `.L` table is ALREADY `.global` in a sibling .s of
 * the same overlay -- so no export, no split and no linker edit anywhere:
 *
 *   function              .s                                             label
 *   --------------------  ---------------------------------------------  -------
 *   OvlFunc_883_20080c4   rom_780898/ovl_30_a_a_a_c_c_a.s                .L6190
 *   OvlFunc_905_20080c4   rom_799abc/ovl_30_a_a_a_c_a_c_a.s              .L1554
 *   OvlFunc_913_20080c4   rom_7a04ac/ovl_30_a_a_a_c_a_c_a.s              .L2d68
 *   OvlFunc_914_20080c4   rom_7a1ff0/ovl_30_a_a_c_a_c_a.s                .Lec8
 *   OvlFunc_915_20080c4   rom_7a2bf0/ovl_30_a_a_a_c_c_a.s                .Lf10
 *   OvlFunc_923_20083a8   rom_7aa430/ovl_314_a_c_a_c_a.s                 .L2700
 *   OvlFunc_924_20083a8   rom_7ac2d8/ovl_314_a_c_a_c_a.s                 .L5d50
 *   OvlFunc_927_20080c4   rom_7b4558/ovl_30_a_a_c_a_c_a.s                .L2ef8
 *   OvlFunc_934_20083a8   rom_7bdeb0/ovl_314_a_a_a_c_c_a.s               .L1e48
 *   OvlFunc_946_20080c4   rom_7ced6c/ovl_30_a_a_a_c_c_a.s                .L315c
 *   OvlFunc_947_20083a8   rom_7d0e88/ovl_314_a_c_a_c_a.s                 .L2ca0
 *   OvlFunc_948_20080c4   rom_7d30e0/ovl_30_a_a_a_c_a_c_a.s              .L2644
 *   OvlFunc_957_20080c4   rom_7e3e08/ovl_30_a_a_a_c_a_c_a.s              .L3eb4
 *   OvlFunc_958_20083a8   rom_7e636c/ovl_314_c_a_c_a.s                   .L16c0
 *   OvlFunc_959_20080c4   rom_7e7574/ovl_30_c_a_c_a.s                    .L5ed8
 *   OvlFunc_964_20080c4   rom_7ed0a0/ovl_30_a_a_a_c_a_c_a.s              .L31f0
 *   OvlFunc_965_20080c4   rom_7ef4f4/ovl_30_a_a_a_c_a_c_a.s              .L2fd4
 *   OvlFunc_965_20080c4   rom_7f2f14/ovl_30_a_a_a_c_c_a_a_b.s            .L2fd4
 *
 * The ready-to-build sources are in scratch_elev/b253/f968/fam/, one per
 * function, generated from this file by substituting the label name, the
 * `OvlFunc_NNN_2008xxx` probe callee and the function name.
 *
 * -- worked in scratch_elev/b253/f968/8374
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
    int f38;
    unsigned char pad3c[0x40 - 0x3c];
    int f40;
    unsigned char pad44[0x59 - 0x44];
    unsigned char f59;
    unsigned char pad5a[0x62 - 0x5a];
    unsigned char f62;
};

extern int L2fd4[] __asm__(".L2fd4");
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_WaitMovement(struct Actor *a);
extern int __TestCollision(struct Actor *a, int *v);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Func_809202c(void);
extern struct Actor *OvlFunc_965_200806c(int *v, struct Actor *a);

void OvlFunc_965_20080c4(void)
{
    int v[3];
    struct Actor *p;
    struct Actor *b;
    struct Actor *o;
    int i;
    int d;
    int z;

    p = __MapActor_GetActor(0);
    i = p->facing >> 12;
    d = L2fd4[i];
    v[0] = p->x + (d & 0xffff0000);
    v[1] = p->y;
    d <<= 16;
    v[2] = p->z.u + d;
    b = OvlFunc_965_200806c(v, p);
    if (b == 0)
        return;
    d = L2fd4[i];
    v[0] = b->x + (d & 0xffff0000);
    v[1] = b->y;
    d <<= 16;
    v[2] = b->z.u + d;
    o = OvlFunc_965_200806c(v, b);
    if (o != 0 && (o->f59 & 1) != 0)
        return;
    v[0] = b->x;
    v[1] = b->y + (0x80 << 13);
    v[2] = b->z.u;
    o = OvlFunc_965_200806c(v, b);
    if (o != 0 && (o->f59 & 1) != 0)
        return;
    b->f22 = 2;
    d = L2fd4[i];
    v[0] = b->x + (d & 0xffff0000);
    v[1] = b->y;
    d <<= 16;
    v[2] = b->z.u + d;
    if (__TestCollision(b, v) > 0)
        return;
    z = b->f62;
    if (z != 0)
        return;
    __Actor_SetAnim(p, 8);
    __WaitFrames(0xf);
    __PlaySound(0xb9);
    b->f30 = 0x3333;
    b->f34 = 0x3333;
    __Actor_TravelTo(b, v[0], v[1], v[2]);
    p->f30 = 0x3333;
    p->f34 = 0x3333;
    __Actor_TravelTo(p, v[0], v[1], v[2]);
    __Actor_WaitMovement(b);
    __Func_809202c();
    b->x = v[0];
    b->z.u = v[2];
    b->f24.u = z;
    b->f2c.u = z;
    p->f38 = 0x80 << 24;
    p->f40 = 0x80 << 24;
    p->x = *(short *)((char *)p + 0xa) << 16;
    p->f24.u = z;
    p->f2c.u = z;
    p->z.u = *(short *)((char *)p + 0x12) << 16;
    __Actor_SetAnim(p, 1);
}

/* OvlFunc_883_20080c4 -- NOT MATCHING, and this park covers EIGHTEEN functions.
 *
 * Source asm: goldensun/asm/overlays/rom_780898/ovl_30_a_a_a_c_c_a.s
 * Best screen: 176 instructions against the ROM's 176, 7 differing.
 *
 * tools/prologue_families.py reports eighteen functions with identical first
 * twelve instructions and identical length (172), one per overlay:
 *
 *   OvlFunc_883_20080c4  OvlFunc_905_20080c4  OvlFunc_913_20080c4
 *   OvlFunc_914_20080c4  OvlFunc_915_20080c4  OvlFunc_927_20080c4
 *   OvlFunc_946_20080c4  OvlFunc_948_20080c4  OvlFunc_957_20080c4
 *   OvlFunc_964_20080c4  OvlFunc_965_20080c4  OvlFunc_959_20080c4
 *   OvlFunc_923_20083a8  OvlFunc_924_20083a8  OvlFunc_934_20083a8
 *   OvlFunc_947_20083a8  OvlFunc_958_20083a8  OvlFunc_968_2008374
 *
 * SEVEN INSTRUCTIONS FROM EIGHTEEN FUNCTIONS. This is the largest single lever
 * left in the tree and the body below is 169 of 176 correct, so what remains is
 * worth more than most whole functions.
 *
 * WHAT THE FUNCTION IS: the block-pushing routine. Take the player's facing,
 * index a sixteen-entry table by its top nibble to get a direction delta, probe
 * for a pushable actor in front, probe again beyond it and above it for
 * obstructions, test collision, and if everything is clear walk both the player
 * and the block one tile along.
 *
 * THREE THINGS WERE SOLVED and each was worth several instructions:
 *
 * 1. THE TABLE ENTRY PACKS TWO SIGNED HALFWORDS INTO ONE WORD, and the two are
 *    extracted differently:
 *
 *        mov r2, r1 / and r2, r3     (r3 = 0xffff0000)  -> the X delta, already
 *                                                          at 16.16 scale
 *        lsl r1, #16                                    -> the Z delta, shifted up
 *
 *    So `v[0] = a->x + (d & 0xffff0000)` and `v[2] = a->z + (d << 16)`.
 *
 * 2. THE SHIFT IS DESTRUCTIVE, so it is a compound assignment. `d << 16` in the
 *    argument gives the three-operand `lsl r2, r1, #16`; `d <<= 16;` as its own
 *    statement gives the ROM's two-operand `lsl r1, #16`. That is the
 *    add/shift-form rule in docs/elevation.md, and it costs one instruction at
 *    each of three sites.
 *
 * 3. THE SHORTS AT +0xa AND +0x12 ARE THE HIGH HALVES OF x AND z. The tail does
 *    `ldrsh r3, [r2, #0xa] / lsl r3, #16 / str r3, [r2, #8]` -- which is
 *    truncating the 16.16 x coordinate to a whole tile, written by reading its
 *    own top half back. They cannot be struct fields alongside `int x`; they are
 *    `*(short *)((char *)p + 0xa)`. Declaring them as fields shifts every offset
 *    after +8 and costs 32 instructions.
 *
 * BLOCKER: instruction scheduling in the tail, seven lines.
 *
 *     rom    mov r1, r10 / str r3, [r6, #0x10] / str r1, [r6, #0x24]
 *              / str r1, [r6, #0x2c] / mov r3, #0x80 / mov r2, r8 / lsl r3, #0x18
 *     ours   str r3, [r6, #0x10] / mov r3, #0x80 / mov r2, r8 / mov r1, r10
 *              / lsl r3, #0x18 / str r1, [r6, #0x24] / str r1, [r6, #0x2c]
 *
 * Same instructions, same registers. gcc schedules the independent
 * `mov r3, #0x80 / mov r2, r8` into the gap before the two zero stores; the ROM
 * does the zero stores first.
 *
 * MEASURED, all 7 of 176 except where noted:
 *   -fno-schedule-insns; -fno-rerun-cse-after-loop; -fno-gcse
 *   -fno-schedule-insns2                                   -- 41, much worse
 *   the zero as a bare literal at all six stores rather than the named `z`
 *   the zero test written `if (b->f62)` rather than `!= 0`
 *   p->f24/f2c moved above p->x                            -- 7
 *   p->f24/f2c moved above p->f38/f40                      -- 7
 *   b->f24 hoisted above b->x                              -- 10, worse
 *
 * The stored zero IS the value read from +0x62 -- the ROM keeps it in r10 and
 * moves it to r1 -- and writing `= 0` gives the same code, because the `if`
 * above has proved the register holds zero and gcc's value numbering
 * substitutes it. That part is right either way.
 *
 * NEXT: this is scheduler behaviour, not a construct, and the usual levers are
 * about WHERE a value is built rather than where a store is issued. The thing
 * that would crack it is a construct that makes the two zero stores depend on
 * something the 0x80 setup does not -- worth one focused attempt by anyone with
 * a fresh idea, because eighteen functions come with it.
  *
 * ROUND 3, batch 156. The park asks for "a construct that makes the two zero
 * stores depend on something the 0x80 setup does not". Tried exactly that, and
 * it is a CLEAN NEGATIVE:
 *
 *   giving the PLAYER actor its own pointer local, `q = p;`, assigned after the
 *   block stores and used for every p-> access in the tail          -- 7
 *
 * The reasoning was that a pseudo born after the block stores could not have
 * its `mov r2, r8` hoisted above them. gcc simply coalesces the copy -- q and p
 * are the same value with no intervening write, so the birth is not a barrier.
 * That closes off the "make the second actor's base arrive later" family of
 * ideas, which is the obvious reading of the park's own suggestion. Anything
 * that works here has to change what the STORES depend on, not what the
 * POINTER depends on.
 *
 * ===> HIGHEST-VALUE PARK IN THE TREE. tools/dupfuncs.py shows this function
 * has EIGHTEEN identical copies across overlays, differing only in which data
 * labels they name. Solving these seven lines elevates eighteen functions. <===
 *
 * whodoesthis.py on the residue: 40 matching functions emit the ROM's order
 * (two stores of one register, then a constant build), so the ordering is not a
 * wall. Reading Actor_SetPos in src/rom_9000/rom_ca6c_b.c, its source NAMES the
 * shifted constant in a local and leaves the zeros as bare literals, grouping
 * each constant's stores together.
 *
 * TRIED FROM THAT, both measured: naming `k = 0x80 << 24` immediately before
 * its two uses -- 7 differing, byte-identical to the literal; naming it earlier,
 * before the b-> stores -- 178 lines and 30 differing, much worse.
 *
 * The reverse order (our order: constant build, then the two stores) appears in
 * ZERO matching functions, which is consistent with the ROM's order being the
 * natural one and ours being the scheduled variant -- but that is a weak zero
 * and should not be read as proof.
 *
 * ROUND 2 ON THE SEVEN LINES. The exact ROM sequence, with the source it maps
 * to, so the next attempt starts from the right picture:
 *
 *     ldr r3, [r7]        v[0]            b->x = v[0];
 *     str r3, [r6, #8]
 *     ldr r3, [r7, #8]    v[2]
 *     mov r1, r10         <-- z copied to a low register HERE
 *     str r3, [r6, #0x10]                 b->z = v[2];
 *     str r1, [r6, #0x24]                 b->f24 = z;
 *     str r1, [r6, #0x2c]                 b->f2c = z;
 *     mov r3, #0x80 / mov r2, r8 / lsl r3, #24
 *
 * We emit the same instructions with `mov r1, r10` and the constant build
 * swapped: gcc materialises the constant into the gap and defers the z copy to
 * its first use. The ROM copies z one instruction earlier, between v[2]'s load
 * and its store.
 *
 * TRIED THIS ROUND, all measured against 176 lines / 7 differing:
 *   naming `k = 0x80 << 24` at its two uses            --  7, byte-identical
 *   naming k before the b-> stores                     -- 178 lines, 30
 *   p->x moved above p->f38/f40                        -- 175 lines, 28
 *   p->f24/f2c moved above p->x                        --  7
 *   p->f24/f2c moved below p->z                        -- 177 lines, 27
 *   `zz = z;` copied between b->x and b->z             --  9
 *   b->f24 moved between b->x and b->z                 -- moves the copy TOO
 *                                                         early, before b->x
 *
 * PLUS everything in the list above from the first round. That is fifteen
 * spellings across two rounds.
 *
 * REACHABLE, not a wall: whodoesthis.py finds 40 matching functions emitting
 * the ROM's order (two stores of one register, then a constant build). What
 * none of them show is a case where a value living in a HIGH register has to be
 * copied down into that same window, which is what this function needs.
 */
*/
struct Actor {
    unsigned char pad00[6];
    unsigned short facing;
    int x;
    int y;
    int z;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
    unsigned char pad23;
    int f24;
    unsigned char pad28[0x2c - 0x28];
    int f2c;
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

extern int L6190[] __asm__(".L6190");
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_WaitMovement(struct Actor *a);
extern int __TestCollision(struct Actor *a, int *v);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Func_809202c(void);
extern struct Actor *OvlFunc_883_200806c(int *v, struct Actor *a);

void OvlFunc_883_20080c4(void)
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
    d = L6190[i];
    v[0] = p->x + (d & 0xffff0000);
    v[1] = p->y;
    d <<= 16;
    v[2] = p->z + d;
    b = OvlFunc_883_200806c(v, p);
    if (b == 0)
        return;
    d = L6190[i];
    v[0] = b->x + (d & 0xffff0000);
    v[1] = b->y;
    d <<= 16;
    v[2] = b->z + d;
    o = OvlFunc_883_200806c(v, b);
    if (o != 0 && (o->f59 & 1) != 0)
        return;
    v[0] = b->x;
    v[1] = b->y + (0x80 << 13);
    v[2] = b->z;
    o = OvlFunc_883_200806c(v, b);
    if (o != 0 && (o->f59 & 1) != 0)
        return;
    b->f22 = 2;
    d = L6190[i];
    v[0] = b->x + (d & 0xffff0000);
    v[1] = b->y;
    d <<= 16;
    v[2] = b->z + d;
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
    b->z = v[2];
    b->f24 = z;
    b->f2c = z;
    p->f38 = 0x80 << 24;
    p->f40 = 0x80 << 24;
    p->x = *(short *)((char *)p + 0xa) << 16;
    p->f24 = z;
    p->f2c = z;
    p->z = *(short *)((char *)p + 0x12) << 16;
    __Actor_SetAnim(p, 1);
}

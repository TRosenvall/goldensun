/* OvlFunc_971_2008580 -- NON-MATCHING, 171 of 256 encodings differ.
 * Unattempted before batch 298.  Reference asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_a_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7fb4a8/2008580.c \
 *       asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_a_a.s --func OvlFunc_971_2008580
 *
 * NOT a distance: size 572 against 560 and count 262 against 256.  aligncmp 66.0%.
 * SHIMS: 1 pin, so a landing needs a fakematch.txt row.  1 of 3 functions, no data.
 *
 * DO NOT PICK THE COUNT-MATCHING VARIANT.  Pinning `i` to r7 makes the count match
 * exactly (256 == 256) and it is then a TRUE DISTANCE OF 221, at 47.7% aligned --
 * far worse than this 262-encoding variant's 66.0%.  A matching instruction count is
 * not evidence of being closer, and this function is the clearest example of it in
 * the corpus.
 */
/* OvlFunc_971_2008580  --  0x02008580   [PARK DRAFT -- 6 encodings long, 66.0% aligned]
 *   [asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_a_a.s, 1st of 3]
 *
 * REFERENCE: 254 instructions / 256 encodings / 560 bytes.  Anchored
 * thumb_func_start count = 3 (this one, OvlFunc_971_20087b0, OvlFunc_971_2008860)
 * and the .s has NO `.section .data` and NO `.incbin` -- tools/datacheck.py
 * prints nothing and exits 0.  Landing needs a FUNCTION SPLIT ONLY: no data
 * half, NO NEW EXPORTS, the two siblings stay as assembly in their own objects.
 *
 * STATE: objcmp  XX SIZE ref 560 bytes, ours 572
 *                XX ENCODINGS differ in 171 place(s) (ref 256, ours 262)
 *                first at index 24: ref 00b6 ours 4683
 * SIZE AND INSTRUCTION COUNT BOTH DIFFER, so 171 is NOT a distance.
 * tools/aligncmp.py: aligned-equal 169 of 256 (66.0%), 127 differing/ins/del in
 * 42 hunks -- and 66.0% IS the figure to compare against, not the 171.
 * SHIMS: 1 register pin (`buf` -> r8), 0 barriers, 0 .equ.  A landing costs a
 * fakematch.txt row for that one pin.
 * THIS IS THE WEAKEST OF THE FIVE and the largest; it is a characterised
 * negative, not a near-miss.
 *
 * WHAT IT DOES -- replaces the .s header's "CALL TRACE rather than a
 * description".  A THREE-PHASE LINK SESSION that sends up to three unit records
 * and then receives and FILTERS a table:
 *   phase 1: allocate 0x154, ask OvlFunc_971_200853c for a list of unit ids,
 *     zero an 8-byte scratch map, then for each id i: fetch the unit, run the
 *     IWRAM packer at iwram_3001388 into the buffer, stamp buf[0x12a] = 2,
 *     record `map[id] = i - 0x80` (so the map holds a NEGATIVE slot tag), send
 *     with __Func_80063bc, and poll to completion;
 *   phase 2: pad the session out to three sends with buf[0x12a] = 0;
 *   phase 3: free, allocate 0x140, run the packer again over __Func_8077330(0),
 *     then COMPACT the received entry table in place -- each 4-byte entry's byte
 *     at +2 is remapped through the phase-1 map, and an entry that maps to 0 is
 *     deleted by shifting the remainder down (the ROM's `ldr r3, [r1, #4] /
 *     stmia r1!, {r3}` pair) and decrementing the count at +0x108, with the loop
 *     index stepped back so the new occupant is examined -- and sends the
 *     compacted table.
 * THE MAP IS THE MECHANISM: phase 1 writes it and phase 3 reads it, which is why
 * its address is kept in a frame slot across the whole function (the ROM's
 * `mov r2, sp / add r2, #8 / str r2, [sp] / ldr r1, [sp]` is that variable being
 * spilled, not an idiom).
 *
 * THE THREE LEVERS THAT LANDED THE PROLOGUE.  The first 24 encodings, including
 * the allocation, the OvlFunc_971_200853c call, the map-address spill and the
 * backwards zeroing loop, are byte-identical.  Measured, from 259 encodings /
 * first diff at index 7 on the first draft:
 *   (a) A NAMED `size` LOCAL for the transfer size, exactly as in this batch's
 *       OvlFunc_971_2008398 -- the ROM holds 0x154 in r5 across the allocation
 *       and the sends, and literals make gcc rematerialise it.
 *   (b) `unsigned short arr[8]`, NOT `short`.  The ROM reads the id list with
 *       `ldrh`; a signed `short` gives `ldrsh`.  OvlFunc_971_200853c is already
 *       elevated as `int OvlFunc_971_200853c(short *out)`
 *       (src/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_b.c), so the array is
 *       unsigned and the argument is cast at the call.
 *   (c) THE INDIRECT CALL IS A FUNCTION-POINTER LOCAL, and this is the lever
 *       worth carrying forward.  The ROM has `ldr r3, =iwram_3001388 /
 *       bl _call_via_r3`.  Declaring the symbol as an array and CASTING it at
 *       the call site (`((void (*)(...))iwram_3001388)(...)`) does NOT work --
 *       gcc-2.96 devirtualises it to a plain `bl iwram_3001388`.  Declaring it
 *       as a function and assigning it into a local of function-pointer type
 *       first (`xfer = iwram_3001388; xfer(buf, u, size);`) DOES work: gcc-2.96
 *       does not constant-propagate the pointer back into a direct call at -O2.
 *       That is the lever recorded in src/rom_c9000/rom_e0524.c, and this is its
 *       second use in the tree -- confirmed to generalise from a named function
 *       to an IWRAM entry point.
 *   (d) `buf` PINNED TO r8.  The ROM keeps the buffer in r8 and the send index
 *       in r7; gcc swaps them.  Pinning buf -> r8 is worth 58.2% -> 66.0%
 *       aligned, and it is the only pin in the file.
 *
 * MEASURED INERT OR HARMFUL (14 builds), with the honest ranking.  Because the
 * instruction count does not match, objcmp's count is saturated here and
 * aligncmp is the ranking tool; the two DISAGREE on this function and that is
 * itself worth recording:
 *       variant                 encodings   objcmp diff   aligncmp
 *       (a)+(b)+(c)             264         186           58.2%
 *       drop (c)                261         217           59.4%
 *       drop (b)                262         236           55.1%
 *       drop (a)                264         246           57.0%
 *       + buf->r8   [SHIPPED]   262         171           66.0%
 *       + i->r7                 256 (!)     221           47.7%
 *       + buf->r8 and i->r7     256 (!)     234           50.0%
 *       + budget->r6            256 (!)     236           50.0%
 *       + tries->r5             254         236           --
 *   PINNING `i` TO r7 MAKES THE INSTRUCTION COUNT EXACT (256 = 256) AND THE CODE
 *   MUCH WORSE: a true distance of 221 out of 256, and alignment falls to 47.7%.
 *   THAT IS THE TRAP ON THIS FUNCTION.  An exact count with 221 of 256 differing
 *   means the registers are wholesale wrong, whereas the shipped 262-encoding
 *   variant has two thirds of its encodings in the right place.  Do not chase
 *   the matching count here; it is not progress, and a future session reading
 *   only "ref 256, ours 256" would pick the worse candidate.
 *
 * THE BLOCKER, NAMED BY PASS: the JUMP/BLOCK-LAYOUT pass, plus two sched2 pairs.
 *   (1) BLOCK ORDER AND AN EXTRA BRANCH ON THE FAILURE PATH.  The ROM emits
 *       `bne L3 / str r0, [sp, #4] / b L4` -- the `ret = rc` store on the
 *       fall-through, then one jump.  gcc CROSS-JUMPS the three identical
 *       `ret = rc; goto done;` sites (one per phase) into one and reaches it
 *       with `b L4 / L3: b L5`, i.e. two unconditional branches and no store.
 *       The ROM did NOT merge those three sites.  This is the same
 *       cross-jumping that shows up in OvlFunc_968_200cbd8 in this batch, and
 *       as there it has no source handle: the three sites really are the same
 *       operation, so there is nothing to make textually different.
 *   (2) The three poll loops' blocks come out in a different ORDER -- the ROM
 *       lays out wait, then retry-counter, then the poll test; gcc interleaves
 *       an extra label and branch.  Same pass.
 *   (3) Two sched2 ordering pairs: `lsl r6, #2` before `mov r11, r0` (ROM) and
 *       `ldr r3, =iwram_3001388` before `mov r0, r8` (ROM), both reversed here.
 *
 * NEXT IDEA IF PICKED UP: the three poll loops are written out three times
 * because they differ in which variables they use (phases 1 and 2 share the
 * budget in r6 and the retry count in r5; phase 3 uses r7 and r10, with the
 * budget RESET to 0x96 << 2 at the start of phase 3).  A `static` helper taking
 * the budget by pointer, inlined three times, is the one structural shape not
 * tried, and it is the plausible origin of three near-identical blocks that
 * gcc nevertheless did not merge.  Measure it before believing it -- it changes
 * the variable lifetimes that levers (a) and (d) depend on.
 *
 * This stem matches only the generic `asm/%.o: src/%.c` Makefile rule, so every
 * figure above is a PRODUCTION-FLAG figure at the tree default -O2.
 */
struct Ent {
    unsigned char b0;
    unsigned char b1;
    unsigned char b2;
    unsigned char b3;
};
struct Tbl {
    unsigned char pad00[8];
    struct Ent e[0x40];
    int n;
};

typedef void (*XferFn)(void *buf, void *unit, int len);
extern void iwram_3001388(void *buf, void *unit, int len);
extern unsigned short iwram_3001f64;

extern void *__Func_8004970(int size);
extern void __free(void *p);
extern void *__GetUnit(int party);
extern int __Func_80063bc(void *buf, int size);
extern int __Func_80064f4(void);
extern void __WaitFrames(int n);
extern void *__Func_8077330(int n);
extern int OvlFunc_971_200853c(short *out);

int OvlFunc_971_2008580(void)
{
    unsigned short arr[8];
    unsigned char q[8];
    int ret;
    unsigned char *qp;
    register unsigned char *buf __asm__("r8");
    struct Tbl *t;
    void *u;
    XferFn xfer;
    int size;
    int budget;
    int tries;
    int count;
    int rc;
    int c;
    int i;
    int j;
    int k;
    int h;

    size = 0xaa << 1;
    buf = __Func_8004970(size);
    ret = 0;
    count = OvlFunc_971_200853c((short *)arr);
    qp = q;
    budget = 0x96 << 2;
    for (j = 7; j >= 0; j--)
        qp[j] = 0;
    for (i = 0; i < count; i++) {
        u = __GetUnit(arr[i]);
        xfer = iwram_3001388;
        xfer(buf, u, size);
        buf[0x95 << 1] = 2;
        qp[arr[i]] = i - 0x80;
        rc = __Func_80063bc(buf, 0xaa << 1);
        tries = 0;
        if (rc == -1) {
            ret = rc;
            goto done;
        }
        for (;;) {
            if (__Func_80064f4() == 0)
                break;
            __WaitFrames(1);
            budget--;
            if (budget < 0 || (iwram_3001f64 & 3) != 3) {
                tries++;
                if (tries > 0x18) {
                    ret = -1;
                    goto done;
                }
            }
        }
        __WaitFrames(2);
    }
    while (i <= 2) {
        buf[0x95 << 1] = 0;
        rc = __Func_80063bc(buf, 0xaa << 1);
        tries = 0;
        if (rc == -1) {
            ret = rc;
            goto done;
        }
        for (;;) {
            if (__Func_80064f4() == 0)
                break;
            __WaitFrames(1);
            budget--;
            if (budget < 0 || (iwram_3001f64 & 3) != 3) {
                tries++;
                if (tries > 0x18) {
                    ret = -1;
                    goto done;
                }
            }
        }
        __WaitFrames(2);
        i++;
    }
    __free(buf);
    size = 0xa0 << 1;
    buf = __Func_8004970(size);
    u = __Func_8077330(0);
    xfer = iwram_3001388;
    xfer(buf, u, size);
    t = (struct Tbl *)buf;
    budget = 0x96 << 2;
    for (k = 0; k < t->n; k++) {
        c = qp[t->e[k].b2];
        t->e[k].b2 = c;
        if (c == 0) {
            for (h = k; h < t->n - 1; h++)
                t->e[h] = t->e[h + 1];
            t->n--;
            k--;
        }
    }
    rc = __Func_80063bc(buf, 0xa0 << 1);
    tries = 0;
    if (rc == -1) {
        ret = rc;
        goto done;
    }
    for (;;) {
        if (__Func_80064f4() == 0)
            break;
        __WaitFrames(1);
        budget--;
        if (budget < 0 || (iwram_3001f64 & 3) != 3) {
            tries++;
            if (tries > 0x18) {
                ret = -1;
                goto done;
            }
        }
    }
    __WaitFrames(1);
    __WaitFrames(2);
done:
    __free(buf);
    return ret;
}

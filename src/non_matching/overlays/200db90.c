/* OvlFunc_969_200db90 -- 0x0200db90, asm/overlays/rom_7f8b34/ovl_2b_c.s
 * and its twin OvlFunc_925_200b460 -- 0x0200b460
 *
 * The twins differ in ONE constant (0xa4 against 0x90), so one solution
 * elevates both.  41 of 41 lines, ELEVEN differing.  Candidate: scratch/Ldb90.c.
 *
 * SOLVED, and both halves generalise -- see docs/elevation.md:
 *
 *   SOURCE ORDER OF TWO LOADS DECIDES WHICH GETS r8 AND WHICH GETS r10.
 *   Two values are loaded before the first call and both survive it.  Written
 *   in the ROM's apparent order (halfword first, pointer second) gcc assigned
 *   them to the opposite high registers from the ROM.  Swapping the two
 *   assignment statements in the source -- while the emitted load order stayed
 *   the ROM's, because that follows first USE, not source position -- fixed the
 *   allocation, and fixed the mul operand order with it.  20 differing -> 11.
 *
 *   MUL COPIES THE SECOND OPERAND.  `mul rD, rS` computes rD = rD * rS, and
 *   gcc emits the copy for the RIGHT-hand operand of the C expression.  The ROM
 *   copies the addend-side value, so the source wants `c * r`, not `r * c`.
 *
 * BLOCKER: scheduling of the tail.  The same twelve instructions in a different
 * order.  Ours hoists the final halfword update's two loads (`ldr r1,=0xfffffe00`
 * and `ldrh r3,[r6]`) up past the three word stores; the ROM leaves them at the
 * bottom in source order.
 *
 * NOT AN ALIASING PROBLEM, despite appearances.  The hoist crosses stores to
 * a+0x10/0x38/0x40 while loading from a+0x64, but every one of those is a
 * constant offset from the SAME base register, so gcc disambiguates by
 * arithmetic and never consults alias analysis.  -fno-strict-aliasing changes
 * nothing, which is the confirmation rather than a surprise.
 *
 * TRIED, all 11: ALIAS, CSE, SCHED2 (worse, 15), O1 (worse, 29, and one line
 * short), -fno-schedule-insns; naming the computed word in a local and storing
 * it twice; moving the a+0x38 store above the a+0x10 store in the source.
 *
 * ================== BATCH 204: 11 DIFFERING DOWN TO 2 ==================
 *
 * The diagnosis above is right and the list of things tried has one gap: no
 * SCHEDULING BARRIER was tried, and the blocker is described as scheduling.
 *
 * 1. `do { } while (0)` IMMEDIATELY BEFORE THE TAIL takes it from 11 to 4.
 *    That is the batch-189 barrier used for exactly what this park describes --
 *    gcc hoisting the final halfword update's loads up past the three word
 *    stores. `__asm__ volatile("")` in the same place is byte-identical.
 *
 * 2. NAMING THE a+8 VALUE takes it from 4 to 2. The ROM interleaves
 *    `ldr r3, [r5, #8]` into the build of `0xa4 << 16`:
 *
 *        mov r2, #0xa4 / ldr r3, [r5, #8] / lsl r2, #0x10
 *
 *    Reading a+8 into a local BEFORE the a+0x10 store, and storing that local
 *    to a+0x38 afterwards, puts the load where the ROM has it.
 *
 * WHAT REMAINS -- TWO INDEPENDENT LOADS, TRANSPOSED:
 *
 *     rom    ldr r1, =0xfffffe00 / ldrh r3, [r6, #0]
 *     ours   ldrh r3, [r6, #0]   / ldr r1, =0xfffffe00
 *
 * MEASURED AGAINST IT, all byte-identical at 2 of 41:
 *
 *     `v = 0xfffffe00; v += *p;`   -- the constant written first
 *     both destinations PINNED, r1 and r3, assigned in the ROM's order
 *     `do { } while (0)` between the two
 *     `__asm__ volatile("")` between the two
 *     the halfword read into its own temp before the addition
 *
 * A PIN ORDERS TWO INDEPENDENT MOVS BUT NOT TWO INDEPENDENT LOADS, and that is
 * the distinction this park adds. Batch 197 closed
 * src/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_a_c.c by pinning three
 * argument registers whose fills the ROM ran backwards, and that worked because
 * each was a `mov` of an immediate -- the pin decides where that materialisation
 * happens. Here both instructions are LOADS, one from the pool and one from
 * memory. Naming their destination registers says nothing about when the loads
 * issue, so the scheduler still orders them and every spelling above is inert.
 *
 * The body below is the 2-differing form, not the 11-differing one the text
 * above was written against.
 */


extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_969_200db90(unsigned char *a)
{
    unsigned short *p;
    unsigned char *q;
    int ang;
    int r;
    int c, s;
    int v;
    int w;

    p = (unsigned short *)(a + 0x64);
    q = *(unsigned char **)(a + 0x68);
    ang = *p;
    c = __cos(ang);
    r = *(int *)(a + 0x30) + 0x1c;
    *(int *)(a + 8) = *(int *)(q + 8) + c * r;
    s = __sin(ang);
    w = *(int *)(a + 8);
    *(int *)(a + 0x10) = (s << 4) + (0xa4 << 16);
    *(int *)(a + 0x38) = w;
    *(int *)(a + 0x40) = *(int *)(a + 0x10);
    do { } while (0);
    v = 0xfffffe00;
    v += *p;
    *p = v;
}

/* ==================== CLOSED IN BATCH 271: A PIN CANNOT REACH THIS ====================
 *
 * Still 2 of 41. This park's own conclusion -- "a pin orders two independent movs
 * but not two independent loads" -- is right, and the reason is now readable
 * rather than inferred, which turns it from an open question into a closed one.
 *
 * From -fsched-verbose=5, every insn in the tail has IDENTICAL priority 36 (the
 * three stores, the ldrh and the constant), so the order is decided purely by
 * LUID. And the constant's LUID cannot be moved, because it is not an expand-time
 * insn. flow2 shows:
 *
 *     (insn 80  (set (reg r3) (zero_extend (mem:HI (reg r6)))))   <- the ldrh
 *     (insn 113 (set (reg:SI 1 r1) (const_int -512)))             <- RELOAD-created
 *     (insn 82  (set (reg r3) (plus (reg r3) (reg r1))))          <- the add
 *
 * RELOAD MATERIALISES -512 IMMEDIATELY BEFORE THE ADD, so it is always emitted
 * after the ldrh, and it additionally acquires a dependence on whatever precedes
 * it -- insn 80's INSN_DEPEND list is `119 118 86 82 113`, and the verbose log
 * shows 113 entering the ready list only once 80 is scheduled.
 *
 * SOURCE POSITION OF THE CONSTANT IS IRRELEVANT, because cse and reload discard
 * any earlier materialisation. Verified directly: with `int bias = 0xfffffe00`
 * assigned BEFORE the tail read, and again with
 * `register int bias __asm__("r1");` declaration split from assignment -- the
 * batch-269 bare-pin lever -- flow2 still shows the constant as a high-numbered
 * reload insn between the ldrh and the add.
 *
 * SO DO NOT ADD A FAKEMATCH ROW FOR THIS ONE. The pin is destroyed before
 * scheduling; no number of pins can reach it. That is worth stating explicitly
 * given how well the bare pin has worked elsewhere in batches 269-271.
 *
 * MEASURED: `bias` as a plain int assigned before the tail, 2 (unchanged); the
 * pinned form, 2 (unchanged); `*p = *p - 0x200;`, 2; `{ int h; h = 0xfffffe00;
 * h = h + (int)*p; *p = h; }`, 2; `{ int h; h = 0xfffffe00; *p = *p + h; }`, 2.
 *
 * AND THREE THAT ARE WORSE, which makes the park's current spelling load-bearing
 * rather than merely equivalent: `*p += 0xfffffe00;`, `{ int h = *p; *p = h +
 * 0xfffffe00; }`, and the fully-spelled-out halfword version are ALL 42 lines and
 * 11 differing. The named-int-intermediate form is one line SHORTER than the
 * obvious spellings and is what holds the length at 41.
 *
 * NOTE FOR LANDING WHENEVER IT IS SOLVED: datacheck says this .s carries .bss AND
 * .data, so it needs a TEXT/DATA split, not just a text split. The twin
 * OvlFunc_925_200b460 is in asm/overlays/rom_7b0400/ovl_314_c_c_c_c.s.
 */

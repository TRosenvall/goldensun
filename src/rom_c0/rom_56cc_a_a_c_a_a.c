/* SomethingSaveHeader -- LANDS, BUT ON A HEADER PREREQUISITE.
 * BYTE-IDENTICAL: 0 of 156 encodings differ, ONCE include/dma.h gains ONE NEW
 * helper (prerequisite 1 below).  The figure is NOT conditional on any flag.
 * Note that THIS body does not compile meaningfully against stock dma.h -- the
 * DMA3_FILL_OFS call is then an unresolved extern and objcmp reads 150 of 156 at
 * 336 bytes with an extra relocation.  The header addition is a HARD prerequisite,
 * not an improvement; the 2-of-156 figure in the old park belongs to the old body
 * (`DMA3_FILL(base + 0x40, z, 0x1000)`), which is preserved in the correction
 * section below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/rom_c0/rom_56cc_a_a_c_a_a.c (LANDED; was src/non_matching/rom_c0/8005920.c) \
 *     asm/rom_c0/rom_56cc_a_a_c_a_a.s --func SomethingSaveHeader
 * objcmp: "OK SomethingSaveHeader -- 344 bytes, 156 encodings and 10 relocations
 * identical".  Production flags, no per-file Makefile row, no extra switch.
 *
 * LANDING PREREQUISITES
 *   1. *** ADD DMA3_FILL_OFS TO include/dma.h *** -- see scratch_elev/b314c/dma.h.patch.
 *      It is a NEW NAME, so not one byte of any existing user changes: the 82
 *      landed DMA3_CLEAR / DMA3_FILL / DMA3_COPY callers the old park screened at
 *      the object level are untouched because they do not name it.  This is exactly
 *      the escape the old park itself proposed for the macro question -- "a SECOND,
 *      differently named macro used only where the ROM's order demands it" -- and
 *      it is the reason the dma.h DECISION recorded there (do NOT rewrite
 *      DMA3_CLEAR's body as a macro) still stands unchanged.  Nothing in include/
 *      was modified to produce the figure above; the probe used a shadow dma.h
 *      beside the candidate, which a quoted #include picks up first.
 *   2. tools/datacheck.py on the reference prints NOTHING and exits 0 -- NO
 *      text/data split.  The reference holds exactly ONE function, so no split_s.
 *   3. tools/shimcount.py reports 0 pins IN THIS FILE -- NO fakematch.txt row.
 *      The four `register ... __asm__` pins that reach the object still come from
 *      include/dma.h, a landed header, and DMA3_FILL_OFS adds none of its own to
 *      this .c.
 *   4. the generated asm/rom_c0/rom_56cc_a_a_c_a_a.s belongs in the commit.
 *
 * THE ONE-LINE CHANGE
 *     -  DMA3_FILL(base + 0x40, z, 0x1000);
 *     +  DMA3_FILL_OFS(base, 0x40, z, 0x1000);
 * DMA3_FILL_OFS is DMA3_FILL with the destination split into a base and an offset,
 * and with the ADD done in its own body: `register unsigned _dst __asm__("r1")
 * = (unsigned)(dst) + off;`.  Everything else -- the `u32 value` local, the
 * `_src __asm__("r0") = &value`, `*_src = _value`, the `"l"`-constrained
 * stmia/sub asm -- is DMA3_FILL's body verbatim.
 *
 * *** THE OLD PARK'S BLOCKER WAS RIGHT ABOUT THE PASS AND WRONG ABOUT THE REACH.
 * *** It proved the residue is an INSN_LUID tie-break in sched2 -- and I re-derived
 * *** every step of that and it holds exactly:
 * ***
 * ***   insn 22 = add r1,r1,sl   prio 4  INSN_DEPEND {37 asm, 447 mov r9,r1}  -> 2
 * ***   insn 28 = mov r0,sp      prio 4  INSN_DEPEND {37 asm, 29 str r3,[r0]} -> 2
 * ***
 * *** At t=10 of basic block 0 the ready list is {36, 28, 22} and 22 is ranked best.
 * *** Priority ties at 4 and cannot be separated: prio(22) = 1+prio(447) = 1+3 and
 * *** prio(28) = 1+prio(29) = 1+3, and raising prio(37) raises both.  depend_count
 * *** ties at 2, and the ROM's own `mov r9, r1` at index 16 proves the ROM's add had
 * *** the same two dependents, so that route really is closed.  last_scheduled_insn
 * *** at t=10 is insn 19 (`mov r3,#0`), whose INSN_DEPEND is {31, 29} -- neither
 * *** candidate -- so both are class 3; and every edge among these moves costs 1,
 * *** which rank_for_schedule's "or has latency of one" clause maps to class 3
 * *** anyway, so the class test can NEVER discriminate here.  LUID is all that is
 * *** left, and LUID(22) < LUID(28).
 * ***
 * *** WHERE THE PARK OVERREACHED is the conclusion: "UNREACHABLE FROM SOURCE while
 * *** dma.h's helpers are `static inline`", on the grounds that integrate.c
 * *** UNCONDITIONALLY copy_to_mode_reg's an inline argument before a single body
 * *** insn is copied, so a body insn can never precede an argument insn.  The
 * *** premise is false as stated.  integrate.c's pre-copy is GUARDED (gcc-2.96,
 * *** expand_inline_function, the `if (arg_vals[i] != 0 && (! TREE_READONLY (formal)
 * *** || ...))` test) -- and in any case the right move is not to defeat the
 * *** pre-copy but to GIVE IT SOMETHING CHEAPER TO COPY.  Pass the offset as its
 * *** own argument and the pre-copy becomes `(set rN (const_int 64))`, which is the
 * *** ROM's `mov r1, #0x40` at index 10 -- early, exactly where the ROM has it --
 * *** while the `add r1, r10` moves into the body, AFTER `_src = &value`.  The
 * *** resulting LUID order is 28 < 29 < 22, and sched2 then produces the ROM's
 * *** `mov r0,sp / add r1,r10 / str r3,[r0] / mov r9,r1` with no further help:
 * *** 22 beats 29 on priority (4 > 3) and 29 beats 447 on dependent count (2 > 1).
 * ***
 * *** THE PARK'S CONST PROBE IS CONFIRMED, NOT SUPERSEDED.  I re-ran it against a
 * *** shadow dma.h: `void *const dst` on DMA3_FILL's formal is BYTE-IDENTICAL to
 * *** the unqualified form, still 2 at indices 13/14.  So TREE_READONLY is not set
 * *** for a top-level-const parameter here (or another disjunct fires), and the
 * *** disjunct is NOT the handle.  Do not retry it.
 * ***
 * *** THE SHAPE WAS ALREADY IN THE TREE, which is how I found it.  A sweep of every
 * *** GENERATED .s for `mov r0, sp` followed within eight lines by an `add r1` and
 * *** an `stmia` returns four hits, and one is LANDED and exact:
 * *** asm/rom_9000/rom_11568_c_c_a_a.s (src/rom_9000/rom_11568_c_c_a_a.c,
 * *** Func_80118d8), where `DMA3_CLEAR(base + 0x18, 0xc0)` emits
 * *** `mov r0,sp / mov r1,r5 / str r6,[r0] / ldr r3 / add r1,r1,#24`.  An argument's
 * *** address arithmetic lands AFTER the body's first insn there, on stock dma.h.
 * *** That file is the proof the ordering is not rigidly LUID-fixed, and it is what
 * *** sent me looking for a helper shape rather than another spelling.
 *
 * ALSO WORTH KNOWING: OTHER PARKS MAY WANT THIS HELPER.  Eight parked files name
 * DMA3_FILL (src/non_matching/rom_c9000/Anim_Annihilation.c,
 * ovl_7fa4ec/20092ac.c, rom_a1000/80a1090.c, rom_b5000/80c02a4.c,
 * rom_8a000/StartRain.c, rom_f0000/80f0678.c, rom_9000/8010e14.c,
 * rom_15000/801edec.c).  Any of them whose residue is the same `mov r0,sp` /
 * dst-add transposition is now a one-token fix.  NOT screened here.
 *
 * STILL TRUE FROM THE OLD PARK, keep every one (each re-verified by the landing):
 *  1. A NAMED POINTER TO THE 16-BYTE HEADER BUFFER, assigned AFTER the DMA that
 *     fills it, not at its declaration.  Worth 19.
 *  2. THE HEADER BUFFER MUST BE DECLARED IN AN INNER BLOCK, so the inlined
 *     helper's `u32 value` takes sp+0 and the buffer sp+4.
 *  3. NAMING THE TWO COMPUTED INDICES of the final three stores, as TWO separate
 *     locals.  Worth 24, and it is what fixes the instruction COUNT; reusing one
 *     local scores 21.
 *  4. `int one = 1; *(unsigned short *)(h + 0xa) = one;` -- the HImode-literal
 *     lever.  Func_80056cc nine instructions away wants the bare literal for the
 *     same kind of store.  Same bank, opposite answers.
 *  5. `z = 0` stays CALLER-side and DMA3_FILL (not DMA3_CLEAR) is the helper:
 *     DMA3_CLEAR's body carries its own zero, so `mov r3,#0` becomes a body insn
 *     whose LUID lands after the argument and the file reads 4.  The same logic
 *     now explains why DMA3_FILL_OFS and not a DMA3_CLEAR_AT.
 *  6. BOTH Func_8005b24 AND Func_8005810 TAKE AN ARGUMENT AT THIS CALL SITE.  The
 *     ROM sets r0 before each.  Declaring them `(void)` here drops the two
 *     `mov r0, r8` and loses the match; the `(void)` definitions in
 *     src/rom_c0/rom_56cc_a_a_a_b.c and .../rom_56cc_a_a_c_a_b.c are consistent
 *     with that, not in conflict with it.
 *
 * TWO REFERENCE-COMMENT CORRECTIONS still outstanding, both in this .s's banner:
 * it names the function "StartMusicTrack" and describes it as starting a music
 * track, where it is save-media work; and it says "160 lines" for 147
 * instructions.
 *
 * MEASURED WORSE / INERT, do not repeat: all nineteen dst spellings of the old
 * park (best 2); the DMA3_SET/DMA3_COPY caller-side-zero-word rewrite (10, 10,
 * 11) -- the addressof pass turns the caller's `*zp = 0` back into
 * `str r3,[sp,#0]` and `&z` needs no pre-copy insn at all; 17 flags alone and in
 * all 136 pairs (best 2); -fno-schedule-insns2 worse; `void *const dst` (2).
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *iwram_3001f1c;
extern const unsigned char _TBL_79b8[] __asm__(".L79b8");
extern unsigned int Func_8005b24(int id);
extern unsigned int Func_8005810(int id);
extern int Func_8005ae0(void);
extern int Func_8005c2c(int id);
extern int Func_8005868(unsigned int n);
extern int Func_8005b64(unsigned int n);

int SomethingSaveHeader(int id, const void *src)
{
    unsigned char *base;
    unsigned int a;
    unsigned int b;
    unsigned int z;
    unsigned int k1;
    unsigned int k2;
    vu32 *dma;

    base = iwram_3001f1c;
    z = 0;
    DMA3_FILL_OFS(base, 0x40, z, 0x1000);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    a = Func_8005b24(id);
    b = Func_8005810(id);
    if (b > 0xf)
        return 1;
    DMA3_COPY(src, base + 0x50, 0xff0);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    {
        unsigned char hdr[16];
        unsigned char *h;

        DMA3_COPY(_TBL_79b8, hdr, 8);
        dma = (vu32 *)&REG_DMA3SAD;
        while (dma[2] & 0x80000000)
            ;
        h = hdr;
        h[7] = id;
        *(unsigned short *)(h + 8) = Func_8005ae0();
        *(unsigned short *)(h + 0xa) = Func_8005c2c(id) + 1;
        DMA3_COPY(h, base + 0x40, 0x10);
        dma = (vu32 *)&REG_DMA3SAD;
        while (dma[2] & 0x80000000)
            ;
        if (Func_8005868(b) != 0)
            return 1;
        if (a <= 0xf && Func_8005b64(a) != 0)
            return 1;
        if (*(unsigned short *)(h + 0xa) > 0xfde8) {
            int one = 1;
            *(unsigned short *)(h + 0xa) = one;
            DMA3_COPY(h, base + 0x40, 0x10);
            dma = (vu32 *)&REG_DMA3SAD;
            while (dma[2] & 0x80000000)
                ;
            if (Func_8005868(a) != 0)
                return 1;
            if (Func_8005b64(b) != 0)
                return 1;
            b = a;
        }
        base[b] = 1;
        k1 = b + 0x10;
        base[k1] = id;
        k2 = b * 2 + 0x20;
        *(unsigned short *)(base + k2) = *(unsigned short *)(h + 0xa);
    }
    return 0;
}

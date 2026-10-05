/* OvlFunc_969_200b600 -- 0x0200b600
 *
 * STILL NON-MATCHING, **2 differing encodings of 44** (ref 44 / ours 44, first
 * differing index 32).  WAS 6 of 44.  Sizes equal; objcmp prints no
 * INSTRUCTION COUNT line and no POOL WORD COUNT line, so the instruction stream
 * length and the single pool word both AGREE.  PIN-FREE, SHIM-FREE, FLAG-FREE,
 * DEVICE-FREE.
 *
 * Verify with: python3 tools/objcmp.py src/non_matching/ovl_7f6e64/200b600.c asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_a_a_a.s --func OvlFunc_969_200b600
 *
 * SPLIT: NONE NEEDED.  grep -c func_start = 1 and tools/datacheck.py is CLEAN.
 *
 * ===== HOW 6 BECAME 2: THE TWIN'S HEADER, THEN ONE CROSS =====
 *
 * OvlFunc_969_200db90 (src/non_matching/overlays/200db90.c) is this function's
 * near-twin -- same upstream module overlays/ovl_314.s, same OvlFunc_969_*,
 * same "orbiting actor" shape -- and IT HAD ALREADY SOLVED THE a+8 RELOAD that
 * this park was stuck on:
 *
 *     "NAMING THE a+8 VALUE takes 4 to 2: reading a+8 into a local BEFORE the
 *      a+0x10 store and storing that local to a+0x38 afterwards puts the load
 *      where the ROM has it."
 *
 * This park had both halves of that in its negatives, NEVER CROSSED:
 *
 *     naming the reloaded value in a local, f40 stored first      9   (park: 8)
 *     f38 stored first, value not named                           8
 *     BOTH -- named `w` + `s`, stores f10/f38/f40                 6, and the
 *                                 WHOLE STORE BLOCK (idx 28-31) becomes EXACT
 *     naming `w` without naming `s`                              45, RELOCDIFF,
 *                                 8 bytes long -> `s` IS A PREREQUISITE
 *     naming `s` alone                                            6  inert
 *
 * The park's recorded lever "THE TWO MIRROR STORES GO f40 BEFORE f38" is
 * therefore WITHDRAWN: it is true only in the absence of the named reload, and
 * the ROM's store order is f10, f38, f40.
 *
 * THEN THE SECOND CROSS, worth 6 -> 2.  With the store block exact, the residue
 * was the scratch register for the SECOND sl->low copy: ROM `mov r2, sl`, ours
 * `mov r1, sl`, with idx 26/27 transposed and idx 32/34 picking the other
 * register as a consequence.  Read out of the dumps:
 *
 *   .19.flow2   insn 47  (set (reg/v:SI 2 r2) ...)   <- `w = a->f8`
 *               insn 52  (set (reg:SI 3 r3) ...)     <- `b->f10`, needs an sl copy
 *
 * Reload assigns from the PRE-SCHED2 CHAIN.  Our `w` load preceded the b->f10
 * load, so r2 held `w` live across insn 52 and reload had to take r1.  The ROM
 * loads b->f10 FIRST, its sl copy gets r2, and `w` then REUSES r2 after the copy
 * dies.  Naming `t = b->f10` and placing it BEFORE `w = a->f8` puts the two
 * loads in the ROM's chain order.  Measured:
 *
 *     t named, t before w, no barrier                             8
 *     t named, w before t, with barrier                           6
 *     do { } while (0) before the tail WITHOUT `t`                7  (regression)
 *     t named, t before w, WITH the barrier                       2   <-- shipped
 *
 * So `t` and the barrier are JOINTLY load-bearing and individually inert or
 * worse.  Third instance of that shape in two batches.
 *
 * ===== WHAT IS LEFT IS THE TWIN'S RESIDUE, EXACTLY =====
 *
 *     rom    ldr r1, =0xfffff800 / ldrh r3, [r6] / add r3, r1
 *     ours   ldrh r3, [r6] / ldr r1, =0xfffff800 / add r3, r1
 *
 * This is INSTRUCTION-FOR-INSTRUCTION the residue 200db90 sits on, so the two
 * twins are now at the same 2 for the same reason, and 200db90's floor argument
 * is confirmed on an INDEPENDENT function.  Measured here: TWELVE tail
 * spellings, ELEVEN EXACTLY INERT at 2 of 44 --
 *
 *     *p = *p - 0x800;                                            2  (shipped)
 *     *p -= 0x800;                                                2
 *     named int v, four placements incl. 200db90's own spelling    2
 *     do { } while (0) BETWEEN the const assignment and the read   2
 *     the const assignment inside its own do { } while (0)         2
 *     __asm__ volatile ("") between the assignment and the read    2
 *     h-then-v and v-then-h local pairs                           2
 *   and ONE regression: *p = -0x800 + *p  ->  12 of 44 and FOUR BYTES SHORTER,
 *   i.e. a length change, not a distance.
 *
 * NEW EVIDENCE, AND IT REFINES 200db90'S DIAGNOSIS.  That park credits the order
 * to haifa's rank_for_schedule tiebreaks (dependent count, then INSN_LUID).
 * Measured here, sched2 never faces a tiebreak at all:
 *
 *   .19.flow2   insn 79  (set (reg:HI 3 r3) (mem:HI (reg/v:SI 6 r6) 6))
 *               note 81  NOTE_INSN_DELETED
 *               insn 110 (set (reg:SI 1 r1) (const_int -2048))   <- RELOAD-CREATED
 *               insn 83  (set (reg:SI 3 r3) (plus r3 r1))
 *   .23.sched2  Ready list (t=143): 79 / (t=144): 79 / (t=145): 110 / (t=146): 110
 *
 * Insns 79 and 110 are NEVER SIMULTANEOUSLY IN THE READY LIST, so no tiebreak
 * decides their order; it is forced one level earlier, at readiness.  I did NOT
 * identify why 110 becomes ready only at t=145 -- it is a plain const_int with
 * no memory operand -- so DO NOT BUILD ON THAT PART.  What is established is
 * (a) the const insn is reload-created and sits after the ldrh in flow2, and
 * (b) eleven tail spellings including every barrier placement cannot move it.
 *
 * NEXT: not a tail spelling.  The class is shared with 200db90 and with
 * 200db90's own twin OvlFunc_925_200b460, so ONE answer lands three functions --
 * which is the reason to spend a round on it, but not on another spelling.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    unsigned char pad34[4];
    int f38;
    unsigned char pad3c[4];
    int f40;
};

extern struct A *__MapActor_GetActor(int slot);
extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_969_200b600(struct A *a)
{
    struct A *b;
    unsigned short *p;
    int ang;
    int s;
    int t;
    int w;

    b = __MapActor_GetActor(0x18);
    p = (unsigned short *)((char *)a + 0x64);
    ang = *p;
    a->f8 = b->f8 + __cos(ang) * (a->f30 + 3);
    s = __sin(ang);
    t = b->f10;
    w = a->f8;
    a->f10 = t + (s << 1);
    a->f38 = w;
    a->f40 = a->f10;
    do { } while (0);
    *p = *p - 0x800;
}

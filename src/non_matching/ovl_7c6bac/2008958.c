/* OvlFunc_942_2008958 (0x02008958) -- NON-MATCHING.
 *
 * NON-MATCHING, 151 of 163 encodings  (re-measured batch 347 brief C; body
 * UNCHANGED).
 *   *** THE FIGURE IS NOT A DISTANCE. ***  136 instructions against the ROM's
 *   134, with the STREAM equal at 163 and the SIZE equal -- a pad absorbs the
 *   two, and objcmp prints its own NOTE saying so, directly above the figure.
 *   The old header said "RELOCATIONS differ ... the figure IS a distance" and
 *   its subject line said "ours 165 of ref 163"; the 16-bit instruction counts
 *   are 134 and 136.  This is the pure pad-absorption case: equal bytes, equal
 *   stream, unequal instructions.
 *   RELOCATIONS: the same 23 symbols at shifted offsets, a consequence of the
 *   +2, not a separate blocker.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7c6bac/2008958.c \
 *     asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_c_c.s --func OvlFunc_942_2008958
 *
 * ALIGNED RESIDUE (tools/aligncmp.py): 66 in 41 hunks.  0 devices.
 * One function in the piece, no data section, no exported non-function
 * symbols -- a landing needs NO split.
 *
 * *** THE +2 IS THREE REGIONS, NOT ONE, AND TWO OF THEM CANCEL. ***  This is
 * what the false distance claim had hidden: the old header read the residue as
 * "constant CSE of 0x95<<4, 0x8c<<17, 0xc0<<8 and a movs r0,#N argument-fill
 * position", which is about WHICH constants are commoned and says nothing
 * about COST.  Counted per region against the ROM:
 *
 *      prologue                                      +2
 *      epilogue                                      +2
 *      the `a->f50` block after __MapActor_GetActor   -2
 *      everything else                                0
 *
 *   PROLOGUE/EPILOGUE.  The ROM pushes `{r5, r6, lr}`.  We push
 *   `{r5, r6, r7, lr}` AND a high register: `mov r7,r8 / push {r7}` and
 *   `pop {r3} / mov r8,r3`.  r7 holds a commoned `0x95 << 4` and r8 a commoned
 *   `0xc0 << 8`.  Commoning is COUNT-NEUTRAL in the body -- two instructions to
 *   build plus one `mov` per use, against two to re-materialise per use -- so
 *   the first commoned constant is free (r7 only widens an existing push list)
 *   and the SECOND costs exactly the four save/restore instructions, because
 *   r5 and r6 are already taken by the ROM's own two long-lived values (0xc and
 *   the pooled zero).  THE COST IS THE SECOND CONSTANT, not the commoning.
 *
 *   THE `a->f50` BLOCK.  The ROM RELOADS `ldr rN,[r0,#0x50]` before each of
 *   the three stores through it; we load it once and reuse r2, which is two
 *   instructions FEWER.  So this park is simultaneously two long and two short,
 *   and any lever that fixes one region without the other moves the figure the
 *   wrong way -- which is precisely what the recorded single-flag rows below
 *   show, and why they were read as "a flag conflict".
 *
 * MEASURED FLAG TABLE (objcmp, instructions against the ROM's 134):
 *      none                                      136   (+2)  ndiff 151  bytes =
 *      -fno-gcse                                 133   (-1)  ndiff 149  bytes -4
 *      -fno-strict-aliasing                      138   (+4)  ndiff 158  bytes +4
 *      -fno-gcse -fno-strict-aliasing            135   (+1)  ndiff 125  bytes =
 *      -fno-rerun-cse-after-loop                 140   (+4)  ndiff 163
 *      -fno-rerun-cse-after-loop -fno-strict-al. 142   (+6)  ndiff 163
 *   This decomposes cleanly along the regions: -fno-gcse removes the constant
 *   commoning (-4, which overshoots by one because the f50 block is still -2
 *   and something else is +1), -fno-strict-aliasing restores the f50 reloads
 *   (+2).  The PAIR is the closest measured state -- +1, equal bytes, equal
 *   stream, ndiff 125 against 151 -- but it needs TWO flag groups at once and
 *   no such group exists in the Makefile, so it is recorded as a MEASUREMENT,
 *   not proposed as a landing.  An owner-facing decision, not an agent's.
 *
 * *** REFUTED: THE NAMED-CONSTANT LEVER IS NOT HARMFUL HERE, IT IS INERT, AND
 * THE OLD HEADER'S "163 -> 181" DOES NOT REPRODUCE. ***  Five spellings were
 * built and measured at production flags and under -fno-gcse:
 *      a named int per `0xc0 << 8` use, defined before its call
 *      one shared named int for all three
 *      a named int per `0x95 << 4` use
 *      both of the above together
 *      the three `a->f50` stores hoisted into a local `struct Sub *s`
 * EVERY ONE IS BIT-IDENTICAL TO THIS BODY -- same 151 of 163, same 136
 * instructions, same 380 bytes.  gcc propagates the constant straight back
 * through the name, so the "N names iff a CALL separates the N uses" lever
 * cannot reach a constant whose uses are ARGUMENTS at the call itself.  Check
 * that variants actually differ before recording them, in either direction.
 *
 * ALSO REFUTED: the UNION lever (c-common.c:3340-3345, alias set 0 through a
 * union, which should have restored the dependence that stops the `a->f50`
 * load being commoned) is INERT here.  Wrapping f9/f15/f1e/f26 in one-member
 * unions and storing through them is bit-identical to this body, at production
 * flags and under -fno-gcse.  -fno-strict-aliasing does reach it; the union
 * spelling does not.
 *
 * BOTH CHEAP AXES ARE CLOSED (tools/autosweep.py, batch 347): flag axis as
 * tabulated above -- and note -O1 (162/163), -fno-strict-aliasing (158/163)
 * and -fno-rerun-cse-after-loop (163/163) all IMPROVE the aligned residue
 * (66 -> 65, 55, 60) while moving the instruction count to +4, so none of them
 * is progress on its own.  Anchor axis: 28 of 28 boundaries, none better than
 * base; the two rows that read lower (149, 150) are a -4 dsize and an unmoved
 * `first`, i.e. misalignment.
 *
 * STILL TRUE FROM THE OLD HEADER: the whole gState prologue and both
 * comparisons are byte-exact -- `ldrsh` + `cmp #3`, then `ldrh` + `lsl #16` +
 * `cmp 1 << 16`; both tests are plain `short` and the named pointer
 * `g = gState;` keeps the offset out of the relocation.  And tryc MASKS
 * `ldrh <pool-label>` against `ldr <pool-label>` and is right to -- Thumb-1
 * has no PC-relative `ldrh`, so gas assembles both to the same halfword.
 *
 * NEXT: a source route that stops the SECOND constant commoning only.  The
 * cost is four instructions and it is entirely the second one; getting
 * `0xc0 << 8` re-materialised per use while leaving `0x95 << 4` in r7 is worth
 * the whole prologue/epilogue, and would leave the f50 block as the only
 * region still wrong -- and -fno-strict-aliasing already shows what fixes that.
 */
struct Sub {
    unsigned char pad0[9];
    unsigned char f9;
    unsigned char pad_a[0x15 - 0xa];
    unsigned char f15;
    unsigned char pad16[0x1e - 0x16];
    unsigned short f1e;
    unsigned char pad20[0x26 - 0x20];
    unsigned char f26;
};

struct Actor {
    unsigned char pad0[0x23];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Sub *f50;
    unsigned char pad54[0x59 - 0x54];
    unsigned char f59;
};

extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern struct Actor *__MapActor_GetActor(int slot);
extern void OvlFunc_942_2008af8(void);

void OvlFunc_942_2008958(void)
{
    unsigned char *g;
    struct Actor *a;
    struct Actor *b;

    OvlFunc_942_2008af8();
    if (__GetFlag(0x95 << 4))
        __Func_8092950(0xc, 2);
    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 3)
        __ClearFlag(0x12f);
    if (*(short *)(g + (0xe1 << 1)) == 1)
        __ClearFlag(0x8aa);
    if (__GetFlag(0x8aa)) {
        __MapActor_SetPos(8, 0xcc << 17, 0x94 << 17);
        __Func_8092adc(8, 0x80 << 8, 0);
    }
    if (__GetFlag(0x8ab)) {
        __MapActor_SetPos(0xd, 0x8c << 17, 0x94 << 17);
        __Func_8092adc(0xd, 0xc0 << 8, 0);
        __MapActor_SetPos(0x10, 0x90 << 17, 0x8c << 17);
        __Func_8092adc(0x10, 0xe0 << 8, 0);
        __MapActor_SetPos(0xa, 0xe8 << 16, 0x98 << 17);
        __Func_8092adc(0xa, 0x80 << 7, 0);
        __MapActor_SetPos(0xb, 0xf0 << 16, 0x9c << 17);
        __Func_8092adc(0xb, 0xc0 << 8, 0);
        a = __MapActor_GetActor(0xa);
        a->f59 = 0;
        a->f23 = 2;
        a->f50->f9 |= 0xc;
        a->f50->f26 = 0;
        a->f50->f1e = 0xc0 << 8;
        b = __MapActor_GetActor(0xb);
        b->f23 = 0;
        b->f50->f9 |= 0xc;
        b->f50->f15 |= 0xc;
    }
    if (__GetFlag(0x95 << 4)) {
        __Func_8010704(0x12, 0x12, 1, 1, 0xe, 0x12);
        __Func_8010704(0x12, 0x12, 1, 1, 0xf, 0x12);
    }
}

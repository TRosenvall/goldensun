/* OvlFunc_967_200904c -- NON-MATCHING, 134 of 180 encodings differ.
 * Unattempted before batch 298.  Reference asm/overlays/rom_7f21b8/ovl_30_c_c_c_c_c_c.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7f21b8/200904c.c \
 *       asm/overlays/rom_7f21b8/ovl_30_c_c_c_c_c_c.s --func OvlFunc_967_200904c
 *
 * NOT a distance: size 396 against 400 and count 178 against 180.  aligncmp 71.1%.
 * SHIMS: 0 pins, plus 2 `.equ` MEASUREMENT-ONLY shims that must be stripped before
 * landing.  Split is text/data and datacheck says no new exports.
 * BLOCKER, local-alloc: the ROM keeps the actor in r0, gcc coalesces the [0x50]
 * load onto r0 and pushes the actor to r1.  Every spelling that fixes that (the
 * load before the byte writes) makes sched2 hoist the ldr instead -- 18 builds, and
 * the two fixes are mutually exclusive.
 */
/* OvlFunc_967_200904c  --  0x0200904c   [PARK DRAFT -- 2 instructions short]
 *   [asm/overlays/rom_7f21b8/ovl_30_c_c_c_c_c_c.s, 1st of 1]
 *
 * REFERENCE: 172 instructions / 180 encodings / 400 bytes.  This .s holds ONE
 * function (anchored thumb_func_start count = 1) AND a .data section, so
 * landing needs a TEXT/DATA SPLIT -- tools/datacheck.py: "OvlFunc_967_200904c
 * reads no data label -> split needs NO new export".  The 13 data labels
 * (ActorCmd_ARRAY_944/967__02009314, gOvl_02009438, gOvl_02009690, .L16b0,
 * gOvl_020096d0, .L1734, .L189c, .L1974, .L1a94, gScript_887__02009ca4,
 * .L1eb4, .L2010) are already global and travel with the data half.
 *
 * WHAT IT DOES.  Post-cutscene state fixup, not a cutscene script: three
 * gState halfword tests gate three independent blocks.  Writes 0x209 through
 * *iwram_3001ebc at +0x1c0, then for area 0xb3 clears actor[0x23] / sets
 * actor[0x59] bit 2 / rewrites actor->[0x50][9] bits 2-3 for slots 0x14, 0x12,
 * 0x13; for area 0xb4 the same for slot 0xd, then a save-bit-gated behavior
 * swap and a sub-state 0x63 -> 0x15 transition.
 *
 * STATE: objcmp  XX SIZE ref 400 bytes, ours 396
 *                XX ENCODINGS differ in 134 place(s) (ref 180, ours 178)
 *                first at index 7: ref 4f59 ours 4f58
 * SIZE and INSTRUCTION COUNT BOTH DIFFER, so 134 is NOT a distance -- the
 * count saturates.  tools/aligncmp.py (no masking, alignment-tolerant) reads
 * aligned-equal 128 of 180 (71.1%), 65 differing/ins/del in 22 hunks.
 * tools/tryc.py --full reads "rom 176 lines, ours 174, first diff at 20".
 * SHIMS: 0 pins, 0 barriers.  Two `.equ` MEASUREMENT shims only (below).
 *
 * THE BLOCKER, NAMED BY PASS.  Everything above index 20 is byte-identical and
 * every register in the three flag-byte blocks now agrees with the ROM
 * (r5 = -0xd, r6 = 4, r8 = 8, r10 = 0, r7 = gState, r11 = &gState[0x1c2],
 * r9 = area).  Two residues remain, both from local register allocation
 * (local-alloc / global-alloc, NOT sched2 -- the instruction ORDER inside each
 * block already matches):
 *   (1) ACTOR MUST STAY IN r0.  The ROM addresses the actor as r0 throughout
 *       (`mov r3, r0` / `mov r2, r0` / `ldr r1, [r0, #0x50]`); gcc copies it to
 *       r1 first (`mov r1, r0`) because the [0x50] load's pseudo COALESCES with
 *       the actor pseudo when their live ranges do not overlap, taking r0 and
 *       pushing actor to r1.  MEASURED, eight-way sweep over
 *       {shared|distinct actor} x {shared|distinct sub} x {load before|after
 *       the two byte writes}, plus a mid-block position and a no-variable
 *       chained deref (14 builds):
 *          load AFTER the byte writes  -> ldr placed correctly, sub in r1,
 *                                         but actor in r1/r2  (best: 126)
 *          load BEFORE the byte writes -> actor in r0 and sub in r1 CORRECTLY,
 *                                         but sched2 then hoists the ldr to
 *                                         third instruction (137, 173 lines)
 *          load mid-block              -> 142/144, 171 lines
 *          chained deref, no local     -> actor back in r1 (126, 174 lines)
 *       The two halves of the tell are mutually exclusive in every spelling
 *       tried.  A named local for the iwram deref, and both declaration
 *       positions for it, change nothing (four builds, identical figures).
 *   (2) THE SAME SWAP ONE LEVEL UP: the ROM gives r2 to `off` and r1 to the
 *       *iwram_3001ebc deref; gcc gives r1 to `off` and r2 to the deref.  This
 *       is the FIRST diff (index 20 in the tryc view) and is independent of (1).
 *
 * THREE LEVERS THAT DID CLOSE, worth recording:
 *
 * (a) THE MASK IS BUILT WITH `neg`, WHICH MEANS A BITFIELD.  The ROM reads
 *     `mov rN,#0xd / neg rN,rN / and / mov #8 / orr / strb`.  Written directly
 *     as `q[9] = (q[9] & ~0xc) | 8` gcc-2.96 narrows the mask to `mov rN,#243`,
 *     because strb makes the top bits dead.  MEASURED on a six-way probe: the
 *     direct form folds; an `int`/`unsigned int` intermediate does NOT, and a
 *     2-bit bitfield at bit 2 does not either.  The bitfield and the `unsigned
 *     int` intermediate are byte-identical to each other in isolation, BUT NOT
 *     IN CONTEXT: the bitfield form is what flips the held-constant allocation
 *     to the ROM's (r5 = -0xd low, r8 = 8 high), worth 5 encodings and the
 *     whole mask block.  The `unsigned int` intermediate leaves 8 in r6 and
 *     -0xd in r8 -- the reverse -- and costs the four-instruction r8 round
 *     trip the ROM shows.  THE TWO SPELLINGS ARE NOT INTERCHANGEABLE HERE.
 *
 * (b) 0xb3 AND 0xb4 ARE POOLED, SO THEY ARE SYMBOLS -- and area.sym ALREADY
 *     HAS THE ROWS (_AREA_b3 at area.sym:128, _AREA_b4 at :129).  No new
 *     symbol is needed.  As plain literals gcc emits `mov r3, #0xb3` and the
 *     function comes out 7 encodings short (173 vs 180) -- the single largest
 *     step in this reconstruction.
 *     The two `__asm__(".equ ...")` lines below are MEASUREMENT SHIMS and must
 *     be STRIPPED BEFORE LANDING.  They exist because the reference .s spells
 *     these `ldr r3, =0xb3`, which the assembler resolves to a literal with no
 *     relocation, whereas the linker-script symbol emits a zero pool word plus
 *     an R_ARM_ABS32 -- so without the .equ, objcmp reports RELOCATIONS differ
 *     and two encoding diffs on a candidate that links byte-identically
 *     (area.sym: "An absolute symbol definition in a linker script emits no
 *     bytes, so the link is byte-identical").
 *
 * (c) `strh` OF A SMALL CONSTANT NEEDS AN `unsigned short` LOCAL.  `*(short *)b
 *     = 0x15` makes gcc-2.96 pool the value and load it with `ldrh r3, .LN`;
 *     the ROM has `mov r3, #0x15`.  Assigning through an `unsigned short`
 *     local first gives the `mov`.  Same trick as
 *     src/overlays/rom_77a7c8/ovl_30_c_c_a_b.c's `zero` local.
 *     Note the converse for LOADS: Thumb-1 `ldrsh` has NO immediate-offset
 *     form, so every `mov rN, #0` before an `ldrsh` is FORCED and is not a
 *     source construct -- do not try to write those zero offsets out.
 *
 * NEXT IDEA IF PICKED UP: the two residues are the same coalescing decision at
 * two scales.  Neither responds to source order, so the lever is probably not
 * in this function's text -- look for a spelling that lengthens the actor
 * pseudo's live range past the [0x50] load WITHOUT putting the load earlier
 * (an actor use after the bitfield write that the ROM optimises away, e.g. a
 * redundant re-read).  Three serious attempts, 18 builds, no new idea.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
typedef struct { unsigned char _pad[9]; unsigned char f0:2; unsigned char f2:2; unsigned char f4:4; } Sub;
extern GlobalState gState;
extern unsigned char iwram_3001ebc[];
extern unsigned char ActorCmd_ARRAY_944__02009314[];
/* MEASUREMENT SHIMS -- STRIP BEFORE LANDING (see (b) above). */
__asm__(".equ _AREA_b3, 0xb3");
__asm__(".equ _AREA_b4, 0xb4");
extern int _AREA_b3;
extern int _AREA_b4;

extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __Actor_SetBehavior(unsigned char *actor, int b);
extern void OvlFunc_967_2008508(void);

int OvlFunc_967_200904c(void)
{
    unsigned int base;
    unsigned int p;
    short area;
    unsigned int off;
    unsigned char *actor;
    Sub *s1;
    Sub *s2;
    Sub *s3;
    Sub *s4;
    unsigned int base2;
    unsigned short h;

    base = (unsigned int)&gState;
    off = 0xe1;
    off <<= 1;
    p = base + off;
    if (*(short *)p == 0x5a)
        __SetFlag(0x9a7);

    off = 0xe0;
    off <<= 1;
    *(int *)(*(unsigned int *)iwram_3001ebc + off) = 0x209;
    area = *(short *)((char *)base + off);
    if (area == (int)&_AREA_b3) {
        actor = __MapActor_GetActor(0x14);
        actor[0x23] = 0;
        actor[0x59] |= 4;
        s1 = *(Sub **)(actor + 0x50);
        s1->f2 = 2;

        actor = __MapActor_GetActor(0x12);
        actor[0x23] = 0;
        actor[0x59] |= 4;
        s2 = *(Sub **)(actor + 0x50);
        s2->f2 = 2;

        actor = __MapActor_GetActor(0x13);
        actor[0x59] |= 4;
        actor[0x23] = 0;
        s3 = *(Sub **)(actor + 0x50);
        s3->f2 = 2;

        __MapActor_SetAnim(0xf, 6);
        if (*(short *)p == 0xc) {
            *(short *)(base + (0xe2 << 1)) = area;
            *(short *)(base + (0xe3 << 1)) = 0xc;
        }
    }

    base2 = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    if (*(short *)(base2 + off) == (int)&_AREA_b4) {
        actor = __MapActor_GetActor(0xd);
        actor[0x59] |= 4;
        actor[0x23] = 0;
        s4 = *(Sub **)(actor + 0x50);
        s4->f2 = 2;
        if (__GetFlag(0xc0 << 2))
            __MapActor_SetBehavior(0xe, ActorCmd_ARRAY_944__02009314);
        off = 0xe1;
        off <<= 1;
        base2 += off;
        if (*(short *)base2 == 0x63) {
            OvlFunc_967_2008508();
            __Actor_SetBehavior(__MapActor_GetActor(0xc), 6);
            __Actor_SetBehavior(__MapActor_GetActor(0xb), 6);
            h = 0x15;
            *(short *)base2 = h;
        }
    }
    return 0;
}

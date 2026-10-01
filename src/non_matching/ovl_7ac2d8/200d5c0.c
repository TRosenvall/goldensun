/* OvlFunc_924_200d5c0 -- NON-MATCHING, 12 of 371 encodings differ (was 13).
 * (objcmp PRODUCTION-FLAG figure: -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi
 *  -fno-builtin -nostdinc -ffreestanding -fcall-used-r4.  No flag row needed or
 *  wanted -- see the flag table below.)
 *
 * SIZE EXACT (ref 832 bytes = ours) and COUNT EXACT (ref 371, ours 371), so 12
 * IS A TRUE DISTANCE.  aligncmp separately: 360 of 371 aligned-equal (97.0%),
 * 12 differing/ins/del in 9 hunks.  WAS 14 AT BATCH 305, 13 AT BATCH 310.
 *
 * ***** BATCH 316: THE BYTE TWIN'S EDIT, PORTED AND MEASURED.  13 -> 12. *****
 * tools/dupfuncs.py pairs this with OvlFunc_923_200a030
 * (src/non_matching/ovl_7aa430/200a030.c), whose batch-316 work closed one
 * encoding with a register pin on the `__vec3_translate` third argument:
 *     was   __vec3_translate(0x80 << 14, ang, p);
 *     now   { register int *q2 __asm__("r2"); q2 = p;
 *             __vec3_translate(0x80 << 14, ang, q2); }
 * MEASURED HERE, NOT ASSUMED -- brief D of this batch established that a
 * duplicate group is a transfer opportunity WITH WORK ATTACHED and that a twin's
 * figure can describe a different body entirely.  This twin's call site was
 * textually identical, the port is the three lines above, and it reads
 *     13 of 371 (first index 79)  ->  12 of 371 (first index 80)
 * i.e. the SAME figure and the SAME first index as 200a030.  The two bodies
 * remain in lockstep; an edit landed on either is still OWED on the other.
 *
 * ALSO FIXED THIS BATCH: the `Verify with:` recipe above pointed at
 * `src/non_matching/ovl_7ac2d8/200d5c0.c`, a gitignored scratch path
 * that no longer exists, so this park's figure was UNREPRODUCIBLE AS WRITTEN.
 * It now names the installed file.  (The one remaining scratch_elev mention
 * below is a pointer to deleted -da dumps, not a measurement recipe.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7ac2d8/200d5c0.c \
 *     asm/overlays/rom_7ac2d8/ovl_35b8_a_c_c_a.s --func OvlFunc_924_200d5c0
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py src/non_matching/ovl_7ac2d8/200d5c0.c \
 *     asm/overlays/rom_7ac2d8/ovl_35b8_a_c_c_a.s OvlFunc_924_200d5c0 -v
 *
 * SPLIT SHAPE: NONE.  asm/overlays/rom_7ac2d8/ovl_35b8_a_c_c_a.s holds
 * exactly ONE function and tools/datacheck.py is silent -- it CONVERTS WHOLE,
 * no split_s.py, no code/data cut.
 *
 * shimcount.py: 4 register pins (the two `register ... __asm__` operands inside
 * the call_via helper, plus one `register unsigned char *g __asm__("r1")` per
 * cell block).  Plus one `__asm__ volatile ("" : : "r" (t))` barrier at cell
 * site 1, which is LOAD-BEARING (removing it measures 16).  A fakematch.txt row
 * is due AT LANDING, not now.
 *
 * ============================================================
 * THE VENEER IS IRRELEVANT TO THIS PARK.  IT IS ALREADY BYTE-EXACT.
 * ============================================================
 * This function carries exactly ONE inline `.call_via r4` site (reference line
 * 364, `ldr r4, =Func_8000888 / .call_via r4`).  The helper is ALREADY INSTALLED
 * and the whole veneer region ALREADY MATCHES: the last differing hunk is at
 * encoding index 118 of 371 and the veneer sits past index 300.  So for this
 * park the answer to "how much of the residue IS the veneer?" is NONE OF IT.
 * Do not spend a round re-spelling the helper here.
 *
 * One thing is worth recording even so: this is a SINGLE-SITE function using the
 * UNPINNED `bx %1` form, which docs/elevation.md's "one site and several sites
 * want DIFFERENT spellings" table says is the two-or-more spelling.  It does not
 * matter -- the region is exact as written -- so the table's single-site advice
 * is a preference, not a requirement, when the callee is already the only one.
 * The "r2" clobber IS load-bearing (it moves the Func_8000888 pointer to r4).
 *
 * ============================================================
 * WHAT THIS BATCH CLOSED: 14 -> 13.  THE cell2 CARRIER MUST BE POINTER-TYPED.
 * ============================================================
 *     was   { register int g __asm__("r1"); ...
 *             g = (int)gBuffer; cell2 = (unsigned char *)(t * 4 + g); }
 *     now   { register unsigned char *g __asm__("r1"); ...
 *             g = gBuffer; cell2 = g + t * 4; }
 *
 *     rom   add r1, r3, r1 / str r1, [sp, #8]
 *     was   add r3, r3, r1 / str r3, [sp, #8]      (2 differ)
 *     now   add r1, r1, r3 / str r1, [sp, #8]      (1 differ)
 *
 * The INT carrier lands the sum in r3 and stores r3; the POINTER carrier lands
 * it in the pinned r1, which is the ROM's destination, and the `str` then falls
 * into place for free.  `cell2` is spilled the instant it is computed, so this
 * is reload's choice of reload register and the TYPE of the carrier is what
 * moves it -- a one-token edit that the old header had written off as "reload's
 * choice of reload register, not an allocation" and therefore unreachable.
 *
 * The LAST encoding in that hunk is the add's OPERAND ORDER, and it is fold
 * canonicalising a pointer PLUS so the pointer is always operand 1.  Measured,
 * all with the pointer carrier in place:
 *     cell2 = g + t * 4;                     13   <- installed
 *     cell2 = &g[t * 4];                     13   (byte-identical)
 *     g = t * 4 + g; cell2 = g;              14
 *     g += t * 4; cell2 = g;                 14
 *     cell2 = (unsigned char *)(t*4+(int)g); 14
 * So 13 is the floor from the carrier side.
 *
 * ============================================================
 * CORRECTION: REGION (b) IS *RELOAD*, NOT local-alloc.  THE OLD HEADER'S WHOLE
 * READING AND ITS "NEXT" PLAN ARE RETIRED.
 * ============================================================
 * The header said the eight encodings in the second division block were a
 * "four-way qty-number tie" in local-alloc, with QTY_CMP_PRI as the model and
 * the constant-creation order as the lever.  NONE OF THE FOUR QUANTITIES IS A
 * PSEUDO.  From the .15.regmove dump of the installed candidate at production
 * flags (reproduce with xgcc -da; the dumps are in scratch_elev/b310b/dump/):
 *
 *     (insn 191 (set (reg:SI 99)
 *         (plus:SI (reg:SI 99) (const_int 1048575 [0xfffff]))) 5 {*thumb_addsi3}
 *     (insn 200 (set (reg:SI 105) (mem:SI (reg/v:SI 32) 4)))
 *
 * The rounding constant NEVER becomes a pseudo -- it is a CONST_INT operand of
 * `*thumb_addsi3`, so `ldr rN, =0xfffff` is RELOAD materialising an operand that
 * does not fit the pattern.  And `mov rN, fp` is RELOAD giving the Thumb `ldr` a
 * LOW base for `(mem (reg/v 32))`, because `p` (reg 32) is allocated r11.  Four
 * reload registers.  QTY_CMP_PRI, allocno_compare, REG_EQUIV, declaration order
 * and live-range spelling CANNOT REACH ANY OF THEM.  That is why every probe in
 * the old header and in batch 305's addendum measured 14 or worse.
 *
 * THE MODEL THAT DOES FIT, AND IT MAKES REGION (b) ONE FACT INSTEAD OF EIGHT.
 * Both sides allocate those four reloads by the SAME rule -- walk r0..r3 round
 * robin from the last one used, skipping any register live at the insn -- and
 * they differ by EXACTLY ONE STEP OF PHASE:
 *
 *     rom    base1 r2 -> const1 r0 -> base2 r1 -> const2 r0
 *     ours   base1 r1 -> const1 r2 -> base2 r0 -> const2 r1
 *
 *     rom    start r2; r3 live (dividend) so skip -> r0; -> r1; now r2 is live
 *            too (`lsl r2, r3, #7` holds the high term) and r3 still is -> r0
 *     ours   start r1; -> r2; r3 live so skip -> r0; -> r1
 *
 * Same rule, different starting register.  So REGION (b) IS NOT EIGHT TIES, IT
 * IS ONE ROTATION PHASE, and no spelling INSIDE the block can move it: the phase
 * is set by how many reload registers were allocated BEFORE the block.  Every
 * reload in the cell1 block is identical on both sides (indices 0-78 match
 * exactly), so the divergence is introduced between the cell1 result and the
 * first cell2 reload -- which is exactly where region (a) sits.
 *
 * ============================================================
 * NEW: gcse OWNS REGION (d).  MEASURED, NOT INFERRED.
 * ============================================================
 *     rom   add r1, sp, #0x10      ours  mov r1, fp
 * at `__TestCollision(actor, v)`.  Adding -fno-gcse ALONE gives the ROM's
 * `add r1, sp, #16` and the whole function reads 12.  So the mechanism is gcse's
 * copy propagation substituting the `p` pseudo (in r11) for the frame address
 * `(plus sfp 16)` that expand emits for the array name.  The old header blamed
 * "the ROM re-derives &v from sp" and tried four spellings of the ADDRESS; the
 * address spelling is not the handle, the AVAILABILITY of `p = v` is.
 *
 * The batch-306 "TWO SETS PREVENT A HOIST" lever was tried here and is a WRONG
 * PROGRAM, not a near miss: a second `p = v;` (after `__CutsceneStart()`, and
 * again immediately before the call) both come out 4 instructions SHORT, 367 of
 * 371, with size short by 8 bytes.  `__TestCollision(actor, p)` is byte-identical
 * to `(actor, v)` at 13 -- gcse has already made them the same program.
 *
 * -fno-gcse is NOT proposed as a flag row: it buys 1 of 13 and leaves regions
 * (a), (b) and (c) untouched.  It is recorded because it NAMES THE PASS.
 *
 * ============================================================
 * THE REMAINING 13, by region
 * ============================================================
 * (a) 2 -- the `__vec3_translate` argument fill.  ROM fills arg2 FIRST:
 *         rom   mov r2, fp / lsl r0, #0xe / mov r1, r6
 *         ours  lsl r0, #0xe / mov r1, r6 / mov r2, fp
 *     sched2's tie-break is priority -> dependent count -> INSN_LUID, and all
 *     three have the call as their one dependent, so this is pure LUID, i.e.
 *     EXPAND order.  load_register_parameters walks argument 0 upward, so the
 *     ROM's order means arg2's hard-register write was emitted FIRST -- which no
 *     ordinary call expansion does.  Previously measured inert at 14: a named
 *     `int d = 0x80 << 14`, a named `int *pp = p`, three argument temporaries in
 *     reverse order.  A barrier before the call is 26.
 * (b) 8 -- the reload rotation phase, above.  ONE fact, not eight.
 * (c) 1 -- the cell2 add's operand order (fold's pointer-PLUS canonicalisation).
 * (d) 1 -- gcse cprop at __TestCollision, above.
 * (One encoding of the 13 is objcmp/aligncmp accounting at a hunk boundary:
 *  aligncmp counts 12 where objcmp counts 13.  Both figures are reported.)
 *
 * ============================================================
 * MEASURED THIS BATCH, ALL WORSE OR INERT (an inert spelling is UNTESTED, not
 * disproved -- but these are now tested)
 * ============================================================
 *     cell2 carrier pinned r2                              15
 *     cell2 carrier pinned r3                              21
 *     cell2 carrier pinned r0                      breaks count (369)
 *     cell2 with NO pin (`cell2 = gBuffer + t*4;`)          15
 *     `{ int t; register ... g; }` decl order swapped       14
 *     `g` assigned BEFORE the divisions                     20
 *     site-2 barrier restored                               14  (inert, confirmed)
 *     `t = 128 * (p[2]/0x100000) + p[0]/0x100000`           14
 *     a second pointer local `int *q = p` for the loads     14
 *     ONE function-level carrier `ct` for both cell blocks  17
 *     reusing the function-level `t` for both cell blocks  breaks count (367)
 *     -fno-rerun-cse-after-loop                            14
 *     -fno-cse-follow-jumps                                13  (inert)
 *     -fno-schedule-insns2                         breaks count (369)
 *     -fno-gcse                                            12  (region (d) only)
 *
 * ============================================================
 * CARRIED FORWARD, STILL TRUE
 * ============================================================
 * `i = 0;` MUST BE WRITTEN BEFORE `mask = 0xf;` (worth 2; sched2 emits the
 * high-register copy at the position its source statement occupies).
 * STACK-SLOT ORDER FOLLOWS C DECLARATION ORDER EXACTLY, and the spill map
 * confirms the installed order: scalars descending are base(sp+0xc),
 * cell2(sp+8), saved(sp+4), savep(sp+0) and the aggregate v[3] sits highest at
 * sp+0x10 -- both the scalar rule and the "aggregates reversed" rule hold as
 * written, so there is NOTHING to gain from the declaration list.
 * NOTE ON THE TWIN AS A SOURCE OF EVIDENCE: the briefing hoped two spill maps
 * would pin an order neither pins alone.  THEY CANNOT.  923_200a030 and
 * 924_200d5c0 have IDENTICAL instruction streams (one `bl` target and two script
 * symbols apart), so they have IDENTICAL spill maps and the pair carries exactly
 * the information one of them does.
 * tools/tryc.py --full prints "269 differ" on this file.  That is an ARTEFACT of
 * one extra label; objcmp is right at 13 and the count is exact.  Use objcmp.
 *
 * ============================================================
 * IF YOU EDIT ONE, EDIT BOTH -- OvlFunc_923_200a030 (src/non_matching/ovl_7aa430/200a030.c) IS AN EXACT TWIN
 * ============================================================
 * Normalising `.L<addr>` labels and the two per-overlay script symbols leaves
 * ONE differing line of 378 (the `bl OvlFunc_92{3,4}_...` target).  The
 * pointer-carrier fix was applied to both and both measure 13.  dupfuncs.py
 * does not pair them (it demands byte identity); the FOUR-HELPER SIGNATURE
 * `__vec3_translate` + `__TestCollision` + `__Func_8092158` + `Func_8000888`
 * does.
 *
 * NEXT.  Regions (a) and (b) are PROBABLY ONE FACT -- 10 of the 13 -- because
 * the reload phase diverges in exactly the window where the argument fill
 * differs.  Stop probing inside the cell2 block; it is proven barren from the
 * carrier, constant, dividend, division-spelling and pin sides.  Find the
 * spelling that makes gcc EXPAND the third argument of `__vec3_translate`
 * before the first, and check region (b) on the same candidate.
 */
extern unsigned int gState;
extern unsigned int gKeyHeld;
extern unsigned int gKeyPress;
extern unsigned char *iwram_3001edc;
extern short L5e44[] __asm__(".L5e44");
extern unsigned char gBuffer[];
extern int gScript_924__0200de20;
extern int gScript_924__0200de2c;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __ClearFlag(int f);
extern void __vec3_translate(int dist, int angle, int *v);
extern int __TestCollision(unsigned char *a, int *v);
extern void __Actor_SetScript(unsigned char *a, void *s);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __DeleteActor(unsigned char *a);
extern void __Sprite_SetAnim(unsigned char *s, int n);
extern void __Func_8092158(int a, int b, int c);
extern void OvlFunc_924_200d158(unsigned char *a);
extern int Func_8000888(int a, int b);

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12", "r2"
    );
    return _a;
}

struct E {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    unsigned char *f14;
    unsigned char *f18;
};

void OvlFunc_924_200d5c0(void)
{
    int v[3];
    int *p;
    unsigned char *base;
    unsigned char *cell2;
    int saved;
    unsigned char *savep;
    struct E *e;
    unsigned char *actor;
    unsigned char *cell1;
    unsigned char *a;
    unsigned char *spr;
    unsigned int r3;
    unsigned int r4;
    int idx;
    int off;
    int ang;
    int t;
    int n;
    int i;
    int j;
    short *tp;
    int mask;

    r3 = (unsigned int)&iwram_3001edc;
    e = *(struct E **)*(unsigned char **)r3;
    r3 -= 0x20;
    base = *(unsigned char **)r3;
    r4 = (unsigned int)&gState;
    r4 += 0xfa << 1;
    idx = *(int *)r4;
    off = idx * 4;
    off += 0x14;
    actor = *(unsigned char **)(base + off);
    savep = actor + 0x55;
    saved = *savep;
    tp = L5e44;
    i = (gKeyHeld >> 4) & 0xf;
    j = i * 2;
    ang = (unsigned short)*(short *)((char *)tp + j);
    if (*(short *)((char *)tp + j) == -1)
        return;

    p = v;
    p[0] = (*(int *)(actor + 8) & 0xfff00000) + (0x80 << 12);
    p[1] = *(int *)(actor + 0x14);
    p[2] = (*(int *)(actor + 0x10) & 0xfff00000) + (0x80 << 12);
    { register int g __asm__("r1"); int t;
      t = (p[2] / 0x100000) * 128 + p[0] / 0x100000; __asm__ volatile ("" : : "r" (t));
      g = (int)gBuffer;
      cell1 = (unsigned char *)(t * 4 + g); }
    { register int *q2 __asm__("r2"); q2 = p;
      __vec3_translate(0x80 << 14, ang, q2); }
    { register unsigned char *g __asm__("r1"); int t;
      t = (p[2] / 0x100000) * 128 + p[0] / 0x100000;
      g = gBuffer;
      cell2 = g + t * 4; }
    if (cell1[2] != e->f4 && cell2[2] == e->f4 && e->f0 == 0)
        return;
    __CutsceneStart();
    t = __TestCollision(actor, v);
    if (t)
        return;
    a = e->f18;
    if (a != 0) {
        *(unsigned short *)(a + 0x64) = t;
        __Actor_SetScript(a, &gScript_924__0200de2c);
        __Actor_SetAnim(a, 7);
        e->f18 = (unsigned char *)t;
    }
    if (cell2[2] == e->f4 && e->f0 != 0) {
        unsigned char *m = e->f14;
        a = __CreateActor(0x1a, *(int *)(m + 8), *(int *)(m + 0xc),
                          *(int *)(m + 0x10));
        if (a != 0) {
            spr = *(unsigned char **)(a + 0x50);
            *(unsigned char **)(a + 0x14) = *(unsigned char **)(m + 0x14);
            __Actor_SetScript(a, &gScript_924__0200de20);
            a[0x55] = t;
            *(unsigned short *)(a + 0x64) = t;
            a[0x23] = 2;
            *(int *)(a + 0x30) = 0x80 << 11;
            *(int *)(a + 0x34) = 0x80 << 10;
            __Actor_TravelTo(a, p[0], p[1], p[2]);
            if (spr != 0) {
                __Sprite_SetAnim(spr, 6);
                spr[0x26] = 0;
            }
            e->f18 = a;
        }
        n = e->f0 - 1;
        e->f0 = n;
        if (n == 0) {
            __DeleteActor(e->f14);
            e->f14 = (unsigned char *)n;
            __ClearFlag(0x161);
        } else if (e->f14 != 0) {
            __Actor_SetAnim(e->f14, 6 - n);
        }
    }
    __Actor_SetAnim(actor, 6);
    __WaitFrames(3);
    __PlaySound(0x98);
    __Actor_SetAnim(actor, 7);
    *(int *)(actor + 0x30) = 0xc0 << 10;
    *(int *)(actor + 0x34) = 0x80 << 10;
    *(int *)(actor + 0x28) = 0x80 << 11;
    *savep = *savep & 0x7e;
    __Actor_SetSpriteFlags(actor, 0);
    __Func_8092158(0, ((short *)p)[1], ((short *)p)[5]);
    __Actor_SetAnim(actor, 6);
    __WaitFrames(2);
    if (cell2[2] != e->f4)
        __Actor_SetSpriteFlags(actor, 1);
    else
        __PlaySound(0xd7);
    __WaitFrames(1);
    *savep = saved;
    if (cell2[2] == e->f4 && e->f18 == 0) {
        __Actor_SetAnim(actor, 0x12);
        __PlaySound(0xf1);
        i = 0;
        mask = 0xf;
        for (;;) {
            if ((i & mask) == 0)
                OvlFunc_924_200d158(actor);
            if (i > 0x1f && gKeyPress != 0)
                break;
            __WaitFrames(1);
            i++;
        }
        __PlaySound(0x90 << 1);
        __WaitFrames(1);
        *(int *)(actor + 8) = e->fc;
        *(int *)(actor + 0x10) = e->f10;
        __Actor_SetSpriteFlags(actor, 1);
    }
    e->f8 = 0;
    __CutsceneEnd();
    *(int *)(base + (0xda << 1)) +=
        call_via(Func_8000888, *(int *)(base + (0xd8 << 1)), 0x80 << 14);
}


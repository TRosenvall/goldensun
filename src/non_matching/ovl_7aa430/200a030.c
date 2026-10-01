/* OvlFunc_923_200a030 -- NON-MATCHING, 12 of 371 encodings differ (was 13).
 * PRODUCTION-FLAG FIGURE (-O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi
 * -fno-builtin -nostdinc -ffreestanding -fcall-used-r4).  No flag row.
 *
 * SIZE EXACT (832 bytes) and COUNT EXACT (371), so 12 IS A TRUE DISTANCE.
 * NOT POOL-INFLATED: every differing index is a real instruction (batch 316).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7aa430/200a030.c \
 *     asm/overlays/rom_7aa430/ovl_1a3c_a_c_c_a.s --func OvlFunc_923_200a030
 *   XX ENCODINGS differ in 12 place(s) (ref 371, ours 371)
 *      first at index 80
 *
 * SPLIT SHAPE: NONE.  The .s holds exactly ONE function and tools/datacheck.py
 * is silent -- it CONVERTS WHOLE, no split_s.py, no code/data cut.
 *
 * *** IF YOU EDIT ONE, EDIT BOTH.  OvlFunc_924_200d5c0
 * (src/non_matching/ovl_7ac2d8/200d5c0.c) IS A CONFIRMED BYTE DUPLICATE --
 * tools/dupfuncs.py pairs them as of its batch-315 fix, so the twinning no
 * longer rests on the four-helper signature.  The batch-316 edit below is DUE
 * ON 200d5c0 TOO and was not applied there. ***
 *
 * ========= BATCH 316: 13 -> 12.  THE OLD REGION (a) READING IS RETIRED. =========
 * THE EDIT:
 *     was   __vec3_translate(0x80 << 14, ang, p);
 *     now   { register int *q2 __asm__("r2"); q2 = p;
 *             __vec3_translate(0x80 << 14, ang, q2); }
 * One more class-1 register pin, of the kind this park already carries four of.
 * Byte-identical alternatives: an `int` carrier with a cast; assigning from `v`
 * instead of `p`; the assignment on its own line.  Also 12: adding an r0 pin, an
 * r1 pin, or all three pins in the ROM's order -- so the r2 pin is the whole
 * effect and the minimal form is the one installed.
 * *** THE UNPINNED FORM IS INERT AT 13, AND THE OLD HEADER HAD MEASURED EXACTLY
 * THAT ("a named `int *pp = p` ... inert at 14") AND CLOSED THE REGION ON IT.
 * THE PIN IS THE LOAD-BEARING HALF -- dfa18_Tackle.c's header says so in terms
 * ("the PIN is what works: an unpinned local measures 148 and 4 bytes larger").
 * An r0 pin alone is inert at 13, so it is specifically the r2 carrier. ***
 *
 * ===== THE OLD REGION (a) sched2 CLAIM WAS WRONG IN TWO PLACES =====
 * It read: "all three have the call as their one dependent, so this is pure
 * LUID, i.e. EXPAND order".  From .23.sched2 (-fsched-verbose=6), block 5:
 *      insn  prio  INSN_DEPEND
 *       167     5   187 184 178 175    (add r5,r3,r1 -- last scheduled, t=7)
 *      1022     5   187 178            (lsl r0,r0,#14   = argument 0)
 *       175     5   187 860 178        (mov r1,r6       = argument 1)
 *       177     5   187 178            (mov r2,fp       = argument 2)
 * (1) EACH HAS TWO DEPENDENTS, not one -- the call 178 AND the block-ending jump
 *     187, which `add_branch_dependences` makes a successor of every insn in the
 *     block -- and 175 has THREE.  The count rung does not tie across the three.
 * (2) THE LADDER HAS A RUNG THE OLD TEXT OMITTED.  rank_for_schedule
 *     (haifa-sched.c:4029-4113) is: priority -> CLASS vs last_scheduled_insn ->
 *     dependent count -> INSN_LUID, with INSN_REG_WEIGHT DEAD after reload
 *     (`!reload_completed`) and the CLASS rung SKIPPED at t=0.
 * WHICH TWO INSNS COMPETE AND WHICH RUNG DECIDES: at t=8 the ready list prints
 * `175 177 1022` (gcc prints worst-first; `ready[n_ready-1]` is taken).  175
 * loses at the CLASS rung -- it is ANTI-dependent on the just-scheduled 167,
 * which reads r1 -- so the real contest is **1022 against 177**: priority ties
 * at 5, CLASS ties at 3, dependent count ties at 2, and INSN_LUID decides.
 * The reference prefix is BYTE-IDENTICAL up to index 78, so the ROM's compiler
 * met the same ready list with the same last_scheduled_insn and chose 177.
 *
 * ===== AND THE DEEPER CORRECTION: REGIONS (a) AND (b) ARE ONE FACT, AND THE
 * ===== OLD HEADER HAD THE CAUSAL ARROW BACKWARDS
 * Side by side after the call (ROM | ours):
 *       mov r2,r11      | mov r1,fp         base1  reload for (mem (reg 32 = p))
 *       ldr r3,[r2,#8]  | ldr r3,[r1,#8]
 *       ldr r0,=0xfffff | ldr r2,=0xfffff   const1 reload of a CONST_INT operand
 *       mov r1,r11      | mov r0,fp         base2
 *       ldr r0,=0xfffff | ldr r1,=0xfffff   const2
 * The rotation table is right.  What it missed is the FEEDBACK: *** insn 175's
 * third dependent IS base1 -- an OUTPUT dependence, because ours allocates base1
 * to r1 and 175 (`mov r1,r6`) also writes r1. ***  In the ROM base1 is r2, so:
 *   - the ROM's pre-call `mov r2,fp` takes the output dependence instead and has
 *     THREE dependents against 1022's two, winning the three-way at the
 *     DEPENDENT-COUNT rung with no LUID argument needed at all; and
 *   - the ROM's `mov r1,r6` then has only TWO dependents, ties 1022 everywhere,
 *     and LUID (argument 0 before argument 1) puts 1022 first.
 * So BOTH of region (a)'s encodings fall out of region (b)'s phase FOR FREE.
 * The old NEXT guessed they were one fact but had it the wrong way round ("the
 * divergence is introduced between the cell1 result and the first cell2 reload
 * -- which is exactly where region (a) sits").  IT IS THE POST-CALL RELOAD PHASE
 * THAT DRIVES THE PRE-CALL SCHEDULE, through INSN_DEPEND of the argument fills.
 * *** STOP LOOKING FOR A sched2 LEVER IN REGION (a).  The pin buys 1 by brute
 * force; the phase buys both, and it is a reload-register question. ***
 * And LUID cannot be won the other way: `load_register_parameters`
 * (calls.c:1684-1696) walks arguments 0 upward unless LOAD_ARGS_REVERSED is
 * defined, and *** LOAD_ARGS_REVERSED IS DEFINED BY NO TARGET IN THIS COMPILER
 * *** (grep: calls.c's own #ifdef, tm.texi, ChangeLog.0, nothing else).  The
 * only way to lower argument 2's LUID is to give it its own source statement --
 * which is exactly what the installed pin does.
 *
 * ===== REGION (b): 8 ENCODINGS, ONE ROTATION PHASE.  STILL CLOSED, NOW PROBED
 * ===== FROM THE CARRIER SIDE TOO
 * None of the four quantities is a pseudo: the rounding constant is a CONST_INT
 * operand of `*thumb_addsi3` and `mov rN,fp` is reload giving the Thumb `ldr` a
 * low base, so declaration order, pinning, REG_EQUIV and live-range spelling
 * cannot reach any of them.  Both sides allocate the four by the same rule and
 * differ by EXACTLY ONE STEP OF PHASE:
 *     rom    base1 r2 -> const1 r0 -> base2 r1 -> const2 r0
 *     ours   base1 r1 -> const1 r2 -> base2 r0 -> const2 r1
 * *** CROSS-PARK: OvlFunc_968_2009af0's region (3) IS THE SAME MECHANISM, derived
 * independently in batch 316 -- its 0x1df loop bound is a CONST_INT operand of
 * `cbranchsi4` through local-alloc and global-alloc, materialised only at
 * reload.  Two parks, one fact, and it deserves a name in docs/elevation.md:
 * A CONST_INT OPERAND RELOAD MATERIALISES IS NOT AN ALLOCATION. ***
 * BATCH 316 NEGATIVES on region (b), measured on top of the pin:
 *     pinned r2 carrier for the post-call p[2] read            12  INERT
 *     pinned r2 carrier for the p[0] read                      14
 *     named local for the 0xfffff rounding constant           131, RELOCDIFF
 *     two pinned carriers on the same hard reg, or one pin
 *       spanning the call                                282-286, RELOCDIFF,
 *       4 bytes SHORT -- WRONG PROGRAMS, not near misses (a call-clobbered
 *       hard-register local cannot span a call)
 * WHAT REGION (b) ACTUALLY NEEDS: a spelling that makes reload pick **r2** for
 * the first post-call `(mem (reg 32))` base reload.  The handle is either one
 * more reload allocated before index 79, or r1 made unavailable at that insn --
 * NOT the carrier, which is now proven barren from both ends.
 *
 * ===== REGION (d): gcse cprop.  ATTRIBUTION RE-READ AT THE INSTRUCTION AND IT
 * ===== SURVIVES
 * `-fno-gcse` removes EXACTLY index 118 and leaves regions (a) and (b)
 * index-for-index unchanged (12 of 371 at idx 79 80 81 83 84 87 88 90 92 95 96
 * 101).  So this is NOT the "doubted at the figure" case: read at the site, the
 * gcse attribution is confirmed.  It is still not proposed as a flag row.
 *
 * ===== LEVER 5 (callee return type): SWEPT, AND IT DOES NOT REACH REGION (a)
 * All 14 `extern void` callees flipped to `extern int`: 13 INERT at 13, WORSE
 * __Actor_SetAnim 15.  *** __vec3_translate's own flip is INERT, so the
 * r0-output-dependence form of the lever has nothing to do here. ***
 *
 * ===== THE REMAINING 12, BY REGION
 * (a) 1 -- argument 1 against argument 0 at __vec3_translate, idx 80-81.  A
 *     consequence of (b); see above.  NOT independently reachable.
 * (b) 8 -- the reload rotation phase, idx 83-101.  ONE fact, not eight.
 * (c) 1 -- the cell2 add's operand order (fold's pointer-PLUS canonicalisation);
 *     13 is the floor from the carrier side, measured five ways.
 * (d) 1 -- gcse cprop at __TestCollision, idx 118.
 * (One encoding is objcmp/aligncmp accounting at a hunk boundary: aligncmp
 *  counts 11 where objcmp counts 12.  Both are reported.)
 *
 * ===== CARRIED FORWARD, STILL TRUE
 * The veneer region is ALREADY BYTE-EXACT and is none of the residue; do not
 * re-spell the helper here.  The "r2" clobber IS load-bearing.
 * `i = 0;` MUST BE WRITTEN BEFORE `mask = 0xf;` (worth 2).
 * STACK-SLOT ORDER FOLLOWS C DECLARATION ORDER EXACTLY and the installed order
 * is confirmed by the spill map, so there is nothing to gain from the
 * declaration list.
 * THE TWIN IS NOT A SECOND SOURCE OF EVIDENCE: 923_200a030 and 924_200d5c0 have
 * identical instruction streams and therefore identical spill maps.
 * tools/tryc.py --full prints "269 differ" on this file -- an ARTEFACT of one
 * extra label.  Use objcmp.
 * shimcount.py: now 5 register pins plus one `__asm__ volatile` barrier at cell
 * site 1 (load-bearing; removing it measures 16).  A fakematch.txt row is due AT
 * LANDING, not now.
 *
 * ===== MEASURED NEGATIVES FROM EARLIER BATCHES -- ALL AT THE OLD BASELINE OF 13
 * cell2 carrier pinned r2 15 / r3 21 / r0 breaks count; no pin 15; decl order
 * swapped 14; `g` before the divisions 20; site-2 barrier restored 14; the `t`
 * expression re-associated 14; a second pointer local 14; one function-level
 * carrier for both cell blocks 17; reusing `t` breaks count;
 * -fno-rerun-cse-after-loop 14; -fno-cse-follow-jumps 13; -fno-schedule-insns2
 * breaks count (369); a second `p = v;` 367 and 8 bytes short.
 * *** CAUTION: every one of those is a figure AT 13, and batch 316 broke this
 * park's closure by re-testing at a baseline the list was never measured at.
 * Re-measure before quoting any of them as a floor. ***
 */
extern unsigned int gState;
extern unsigned int gKeyHeld;
extern unsigned int gKeyPress;
extern unsigned char *iwram_3001edc;
extern short L27f4[] __asm__(".L27f4");
extern unsigned char gBuffer[];
extern int gScript_923__0200a7d0;
extern int gScript_923__0200a7dc;

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
extern void OvlFunc_923_2009bc8(unsigned char *a);
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

void OvlFunc_923_200a030(void)
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
    tp = L27f4;
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
        __Actor_SetScript(a, &gScript_923__0200a7dc);
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
            __Actor_SetScript(a, &gScript_923__0200a7d0);
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
                OvlFunc_923_2009bc8(actor);
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


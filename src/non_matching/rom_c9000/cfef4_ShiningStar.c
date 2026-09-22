/* Anim_ShiningStar (asm/rom_c9000/rom_cfef4.s, 415 instructions) -- NON-MATCHING,
 * 316 encodings of 432, size 954 against the ROM's 964 (-10).
 *
 * Blocker class: loop-strength reduction the ROM does not have, plus a stack-slot
 * permutation that includes a slot I cannot manufacture.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/cfef4_ShiningStar.c \
 *     asm/rom_c9000/rom_cfef4.s --func Anim_ShiningStar
 * The reference holds SIX functions, so --func is required and a split is needed
 * to land this one.
 *
 * READ THE NUMBERS CAREFULLY, BECAUSE THERE ARE TWO KINDS HERE AND ONLY ONE IS
 * QUOTABLE. objcmp says 316 of 432 and that is the authoritative figure. An
 * alignment-based differ (difflib over the 415-line body) puts it at 115 of 415,
 * and at 69 of 415 once register names and sp offsets are normalised away --
 * i.e. roughly two thirds of the visible residue is ONE register/stack-slot
 * permutation rather than wrong code. THOSE ARE NOT objcmp NUMBERS AND MUST NOT
 * BE QUOTED AS ONE; they are recorded only because they say where the work is.
 * This is the batch-280 measurement lesson applied correctly by the agent that
 * produced this candidate.
 *
 * Aligned-count progression, for the record: 386 -> 229 -> 215 -> 213 -> 140 ->
 * 132 -> 129 -> 115.
 *
 * ================================================================
 * THE rom_c9000 ANIMATION IDIOM -- the real deliverable of this park
 * ================================================================
 *
 * FIVE OF THE SIX FUNCTIONS IN THIS FILE ARE ONE SKELETON. `iwram_3001eec` is
 * the head of a POINTER TABLE; the animation reads four pointers out of it and
 * never allocates:
 *
 *     st   = ((char **)&iwram_3001eec)[0];
 *     pal  = ((char **)&iwram_3001eec)[1];
 *     gfx  = ((char **)&iwram_3001eec)[2];
 *     view = *(char **)((char *)&iwram_3001eec - 0x6c);
 *     draw = (DrawFn)((char **)&iwram_3001eec)[7];
 *     slot = (int **)(st + 0x7828);  *slot = desc;
 *     AnimStart(0 or 1);
 *
 * THE ARRAY-SUBSCRIPT SPELLING IS THE LOAD-BEARING PART: `((char **)&sym)[0]`
 * reproduces `ldmia r3!, {r1}` EXACTLY, with no walking pointer needed, and the
 * whole 16-instruction prologue matched ON THE FIRST COMPILE in every register.
 * `view` uses the `&sym - 0x6c` negative-offset lever already on file in
 * src/non_matching/rom_c9000/80d6504.c, and it appears in EVERY ONE of the five
 * prologues.
 *
 * Then, invariantly: GetFile -> the pinned Func_8001af8 DMA -> DecompressLZ ->
 * BuildDraw2DFuncEx(0x2e,7,7,3,2) -> state writes at st+(0xef<<7) and st+0x7784
 * -> StartTask(Task_BlitAnim, 0x90<<3) -> _GetBattleActor(desc[2]) and
 * _GetBattleActor(*(short *)((char *)desc+0x24)) -> a particle-init loop over
 * st+(0xe1<<7) with STRIDE 0x1c -> the frame loop with WaitFrames(1) at the
 * bottom -> StopTask / gfree(0x2f) / gfree(0x2e) / AnimEnd.
 *
 * The pinned DMA block was lifted verbatim from src/rom_c9000/rom_cc5d8_a_a_b.c
 * (Anim_UnleashIntro) and was correct first time:
 *
 *     register unsigned q0 __asm__("r0"); register int q2 __asm__("r2");
 *     q0 = 0xa0; copy = Func_8001af8; q2 = 0x80; q0 <<= 19;
 *
 * STRUCT SHAPES INFERRED, reusable across the bank:
 *   - particle at st + 0xe1*128, 7 ints: { x, y, z, dx, dy, dz, startFrame }
 *   - gBuffer is an array of 28-byte (7-int) draw records; fields 3 and 4 are
 *     screen x and y
 *   - st state offsets seen: 0x7824 (present flag), 0x7784, 0x77a8,
 *     0x7828 (desc slot), 0xef<<7, 0xe1<<7
 *   - desc: [2] = self id, +0x24 = short[] of target ids, [5] = target count
 *
 * ================================================================
 * TWO LEVERS MEASURED HERE THAT ARE NEW TO THE PROJECT
 * ================================================================
 *
 * `lsr #31 / add / asr #17` IS `(x / 2) >> 16`, NOT `x / 0x20000`. Worth 73
 * aligned lines by itself, and grepped absent from docs/elevation.md ("asr #17",
 * "lsr.*31", "division by", "signed halving", "rounding bias"). `x / 131072`
 * needs a +131071 bias; the ROM's three instructions are the /2 bias followed by
 * >>16, which combine merges into a single `asr #17`. CONFIRMED ASYMMETRICALLY
 * IN ONE LOOP, which is what makes it certain rather than plausible: the sin term
 * is `(t / 2) >> 16` (three instructions) and the cos term is plain `t >> 16`
 * (one `asr #16`).
 *
 * The multiply operand-order finding here is a RE-DERIVATION, not new -- it is
 * already in docs/elevation.md twice ("mul copies the second operand" and "WHICH
 * OPERAND COMES FIRST DECIDES A DESTRUCTIVE `mul`'s DESTINATION"). Recorded
 * because it was worth 8 aligned lines here and confirms the existing rule on a
 * third bank: ROM `mov r2, <amp> / mul r2, <trig>` comes from `trig * amp`, and
 * `m * delta` from source `delta * m`. Two independent sites flipped together.
 *
 * Other measured levers, with their aligned deltas: `i != 8` rather than `i < 8`
 * on every loop (all six loops in the ROM use !=); named constants for the
 * draw-call arguments (132 -> 129); a SEPARATE pointer variable for the second
 * loop over the same array (129 -> 115); re-deriving *(int **)(st+0x7828)
 * instead of reusing the `slot` local (fixed the frame 0x44 -> 0x40).
 *
 * ================================================================
 * THE RESIDUE, four parts
 * ================================================================
 *
 * 1. LOOP-STRENGTH REDUCTION THE ROM DOES NOT HAVE. The ROM recomputes i*280
 *    inside the loop as i*5, *7, *8 (lsl#2/add, lsl#3/sub, lsl#3); gcc creates an
 *    induction variable stepping by 280, which adds a third counter at the loop
 *    latch and a third zero-store at loop entry. MEASURED INDIFFERENT TO
 *    SPELLING: `gBuffer + i*10*28`, `gBuffer + i*280`,
 *    `&((Node *)gBuffer)[i*10]` with a 28-byte struct, and `gBuffer + jbase*28`
 *    all reduce identically. Only -fno-strength-reduce removes it (115 -> 107
 *    aligned). NO Makefile row was added, and one should not be added on
 *    115 -> 107 alone -- find the source cure first.
 *
 *    THE SOURCE CURE WAS FOUND LATER IN THE SAME BATCH AND IS NOT A FLAG.  A NAMED
 *    MULTIPLIER LOCAL (`off = i * 280;`) makes the pseudo REG_USERVAR_P, so
 *    strength_reduce's benefit loses copy_cost and
 *    `v->lifetime * threshold * benefit < insn_count` flips -- the giv is ignored
 *    and gcc recomputes the product in the loop, which is the ROM's i*5, *7, *8.
 *    It suppresses exactly ONE loop's giv, where -fno-strength-reduce kills every
 *    giv in the TU including the loops that already match (measured strictly worse
 *    elsewhere: 34 -> 59).  See docs/elevation.md and the landed
 *    src/rom_b5000/rom_b9b30_c_a_a_c.c, where it was worth 231 -> 179.
 *    CHECK FIRST that the reduction here is a DEST_REG giv and not a DEST_ADDR one
 *    -- loop.c:4502 gates REG_USERVAR_P on v->dest_reg, and the lever is inert on
 *    DEST_ADDR givs (src/non_matching/rom_b5000/80b9ec0.c is that case).
 *    THIS IS THE FIRST THING TO TRY ON THIS PARK.
 *
 * 2. CSE OF A REPEATED POOL CONSTANT, as the batch-280 class predicts, BUT the
 *    argument-pin cure does not apply because these are not call arguments. The
 *    ROM loads =gBuffer from the pool TWICE and keeps `jbase` in r11 across
 *    __modsi3; gcc commons gBuffer into r11 and reloads jbase from the stack
 *    twice. Hoisting `jb = jbase` above the j loop did not move it.
 *
 * 3. STACK-SLOT PERMUTATION, AND THE ROM HAS A DEAD SLOT I CANNOT MANUFACTURE.
 *    Frame size is correct (sub sp, #0x40) and the slot COUNT matches, but the
 *    map is rotated: the ROM has gfx/view at 0x14/0x18 and k at 0x28, ours the
 *    reverse. Slot offset descends with declaration order, so the ROM's order is
 *    roughly st, pal, k, <X>, draw, i, gfx, view, jbase -- and <X> is a slot at
 *    sp+0x24 that is ALLOCATED AND NEVER REFERENCED. Verified by grepping every
 *    `sp, #` form in the reference: 11 slots allocated, 10 used.
 *
 * 4. REGISTER PERMUTATION in the particle loop (ROM p=r6, tgt=r7; ours
 *    reversed), cascading into `mov r1,r10` vs `mov r0,r10`. Six declaration and
 *    assignment orderings measured, all 115 or worse. Per the batch-280 rule this
 *    is a SYMPTOM; the squatting value was not found.
 *
 * AND ONE MICRO-BLOCKER WORTH ITS OWN NOTE, because a sibling explains it.
 * The ROM has `mov r3, #0x4 / sub r3, #0x2 / ldrh r1, [r2, r3]` -- a
 * register-offset halfword load whose offset is the constant 2 materialised as
 * 4-2. Ours emits `ldrh r1, [r3, #4]` (gcc folded the -2 into the pool symbol).
 * Anim_PsyphonSeal in the same file has the SAME source construct with a
 * genuinely variable size: `sub r3, r7, #2 / ldrh r1, [r2, r3]` where
 * r7 = 2 * (5 - k/16). So the shape is `Data_edeXX[sz - 2]` with sz a VARIABLE --
 * and since ShiningStar's sz is 4 and gcc still did not fold it, ShiningStar's 4
 * is not a literal either. MEASURED AND REJECTED: naming `int four = 4` and
 * assigning it inside the m loop, above the j loop, above the i loop, and above
 * the k loop -- all four give the identical wrong `ldrh [r3,#4]` and the
 * identical aligned count. A named int local is NOT enough; the donor's
 * named-multiplier lever is specific to MULT (pre-expand synth_mult) and does
 * NOT generalise to a constant in an address. Unsolved, about 4 instructions.
 *
 * SYMBOL TELL REPORTED, NOT ADDED, AND IT IS NOT A .sym CASE. `.Lee158` (2 bytes
 * at ROM 0xee158, `20 10`) is a LOCAL rodata label this function references.
 * Modelled as `static const unsigned char kAmp[2] = { 0x20, 0x10 };`, which
 * produces an R_ARM_ABS32 to .rodata where the ROM has one to .Lee158 -- the same
 * kind of relocation, and objcmp's lists are otherwise structurally identical
 * (same 34 calls in the same order, same pool symbols). Control: every other
 * constant of this kind in the function (0x199a, 0x1dffff, 0x7784, 0x7828,
 * 0xaaab) reproduces as a plain literal. NO .sym entry is warranted. But the POOL
 * ORDER differs -- the ROM places .Lee158 between Task_BlitAnim and gBuffer, gcc
 * places .rodata after gBuffer -- and that alone fails compare.
 *
 * NEXT, in order: (1) the sp+0x24 dead slot, since it is upstream of the whole
 * slot map and nothing else can be read cleanly until the frame matches;
 * (2) the strength-reduction source cure; (3) the register permutation last, as
 * a symptom. Do NOT spend more budget on spellings for the `4` micro-blocker --
 * four are on file and flat.
 *
 * THE FILE'S OTHER FIVE FUNCTIONS, and the cheapest one is not the smallest by
 * call-family score:
 *   - Anim_Unused_ScreenMelt @ 0x080d0468, 198 insns -- A DIFFERENT IDIOM and the
 *     EASIEST IN THE FILE. Pushes only r8-r10 (not r11), never touches
 *     iwram_3001eec, reads iwram_3001ef4 instead, writes palette RAM at 0x5000040
 *     in a 32-iteration loop, DMAs through Func_8001af8 to 0x6008000. TAKE THIS
 *     FIRST next round.
 *   - Anim_Condemn @ 0x080cfef4, 580 insns, the largest. Same idiom plus
 *     REG_BG2PA / REG_BLDALPHA writes and GetFile(FILE_ab). Calls Func_80cdb24
 *     where the others call AnimStart -- possibly the same routine under two
 *     names, worth checking. IT HAS AN INTERIOR LITERAL POOL (.Lcff60 .word
 *     0x100 / .Lcff64 .word 0x1010 / .pool mid-body), so tryc is BLIND on it and
 *     objcmp is mandatory; it also carries the HImode-constant-store shape
 *     (strh of pooled 0x100/0x1010). The risky one.
 *   - Anim_PsyphonSeal (436, frame 0x78): AnimStart(0); gPtrs[0xb8/4] read
 *     directly; iwram_3001f0c is a SECOND function pointer; writes REG_BG2X;
 *     _PlaySound(0x8e); GetBattleActorPos2 into a sp+0x6c vec3; a
 *     MatrixScalev/MatrixRoll/MatrixYaw block; loop bound desc[5]*5*4 + 0x48 with
 *     the `!= -0x48` guard at entry being the count==0 peel; r11 holds a
 *     DESCENDING counter (add r11, -8).
 *   - Anim_AstralBlast (490, frame 0x74): the _GetBattleActor(desc[2]) call
 *     happens BEFORE the st+0x7828 store, with `mov r5, r0` stashing the
 *     parameter early -- so the parameter is a named local assigned first.
 *   - Anim_Bind (524, frame 0x84): `mov r5, r0` then `add r7, r1, r3` -- the slot
 *     pointer goes to r7 not r8, and r8 holds gfx. Early
 *     `if (desc[7] == 1) Anim_Djinni(desc, 3, &sp[0x50], &sp[0x4c], 0)` with two
 *     stack arguments that are ADDRESSES OF LOCALS, so the name-the-stack-
 *     arguments lever applies.
 *
 * ALL SIX ARE REACHABLE and converting the file whole is the right next move.
 * No per-file Makefile flag override exists or is needed for this stem.
 */
#include "gba/types.h"
#include "file_table.h"
#include "math.h"

extern char *iwram_3001eec;
extern unsigned char gBuffer[];
extern unsigned short Data_ede48[];

extern void AnimStart(int a);
extern void *GetFile(int id);
extern void Func_8001af8(volatile unsigned short *dst, void *src, int len);
extern void DecompressLZ(void *src, void *dst);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int flags, int e);
extern void StartTask(void *task, int prio);
extern void Task_BlitAnim(void);
extern int *_GetBattleActor(int id);
extern unsigned int Random(void);
extern void ColorCycleVFXPalette(int a, int b, int c, int d);
extern void _Func_80bd7dc(int a);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int Func_80e3944(void *in, void *out);
extern void _PlaySound(int id);
extern void Func_80d6888(int t, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(int n);
extern void StopTask(void *task);
extern void gfree(int idx);
extern void AnimEnd(void);

typedef void (*CopyFn)(volatile unsigned short *, void *, int);
typedef void (*DrawFn)(void *a, void *b, int c, int d, int e, int f);

static const unsigned char kAmp[2] = { 0x20, 0x10 };

void Anim_ShiningStar(int *desc)
{
    char *st;
    char *pal;
    int k;
    DrawFn draw;
    int i;
    char *gfx;
    char *view;
    int jbase;
    int **slot;
    CopyFn copy;
    int *src;
    int *tgt;
    int *p;
    int *q;
    int n, j, m;
    int base;
    int arg;
    int jb;
    int one, two, four;
    int v[3];

    st = ((char **)&iwram_3001eec)[0];
    pal = ((char **)&iwram_3001eec)[1];
    gfx = ((char **)&iwram_3001eec)[2];
    view = *(char **)((char *)&iwram_3001eec - 0x6c);
    slot = (int **)(st + 0x7828);
    *slot = desc;
    AnimStart(1);
    {
        register unsigned int q0 __asm__("r0");
        register int q2 __asm__("r2");
        void *data = GetFile(FILE_79);
        q0 = 0xa0;
        copy = Func_8001af8;
        q2 = 0x80;
        q0 <<= 19;
        copy((volatile unsigned short *)q0, data, q2);
    }
    DecompressLZ(GetFile(FILE_73), gfx);
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    *(int *)(st + (0xef << 7)) = 2;
    arg = 0x90;
    *(int *)(st + 0x7784) = 0x32;
    draw = (DrawFn)((char **)&iwram_3001eec)[7];
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);

    src = (int *)_GetBattleActor((*slot)[2])[0];
    tgt = (int *)_GetBattleActor(*(short *)((char *)*slot + 0x24))[0];

    p = (int *)(st + (0xe1 << 7));
    for (i = 0; i != 8; i++) {
        p[0] = src[2] / 2;
        p[1] = src[3] + (0xf0 << 15);
        p[2] = src[4];
        base = i * 8;
        p[3] = (tgt[2] + (((int)(Random() & 0x7f) - 0x40) << 16) - p[0]) / 12;
        p[4] = (tgt[3] - p[1] + (0xa0 << 13)) / 12;
        p[5] = (tgt[4] - p[2]) / 12;
        p[6] = (Random() & 0xf) + base;
        p += 7;
    }

    for (k = 0; k != 0x80; k++) {
        ColorCycleVFXPalette(k, 0xaaab, 0x5555, 0);
        if (k == 0x60)
            _Func_80bd7dc(0x86);
        jbase = 0;
        q = (int *)(st + (0xe1 << 7));
        for (i = 0; i != 8; i++) {
            if (k >= q[6]) {
                InitMatrixStack();
                MatrixSetLook(view, view + 0xc);
                Func_80e3944(q, v);
                v[0] >>= 1;
                if ((unsigned int)(v[0] + 8) <= 0x87 && v[1] <= 0x7f && v[1] >= -8) {
                    int *rec = (int *)(gBuffer + i * 10 * 28);
                    for (n = 0; n != 10; n++) {
                        rec[3] = v[0] + ((sin(n * 0x199a - ((k - q[6]) << 11)) * kAmp[n & 1] / 2) >> 16);
                        rec[4] = v[1] - ((cos(n * 0x199a - ((k - q[6]) << 11)) * kAmp[n & 1]) >> 16);
                        rec += 7;
                    }
                    jb = jbase;
                    for (j = 0; j != 10; j++) {
                        int *a = (int *)(gBuffer + (jb + j) * 28);
                        int *b = (int *)(gBuffer + (jb + (j + 1) % 10) * 28);
                        for (m = 0; m != 12; m++) {
                            int x = a[3] + ((b[3] - a[3]) * m) / 12;
                            int y = a[4] + ((b[4] - a[4]) * m) / 12;
                            one = 1;
                            two = 2;
                            four = 4;
                            draw(pal, gfx + *(unsigned short *)((char *)Data_ede48 + (four - 2)), x - one, y - 2, two, four);
                        }
                    }
                }
                if (q[1] <= 0x1dffff) {
                    q[4] = -q[4];
                    q[3] = q[3] / 2;
                    q[5] = q[5] / 2;
                    *(int *)(st + 0x77a8) = 4;
                    _PlaySound(0x86);
                    if (((int **)(st + 0x7828))[0][5] != 0) {
                        int t = 0;
                        do {
                            Func_80d6888(*(short *)((char *)((int **)(st + 0x7828))[0] + 0x24 + t * 2), 7, 5, t, 8);
                            t++;
                        } while (t != ((int **)(st + 0x7828))[0][5]);
                    }
                }
                q[0] += q[3];
                q[1] += q[4];
                q[2] += q[5];
            }
            jbase += 10;
            q += 7;
        }
        UpdateScreenShake(4, 4);
        Func_80cd52c();
        *(int *)(st + 0x7824) = 1;
        WaitFrames(1);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}

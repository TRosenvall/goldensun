/* BaseAnim_Attack -- 0x080e3aa0, asm/rom_c9000/rom_e3958_c_c_c_c_a.s line 53,
 * 655 ROM instructions.
 * NON-MATCHING, 653 of 688 encodings differ.
 *
 * SIZE AND COUNT ARE BOTH INEXACT, so 653 is NOT a distance: 1492 bytes against
 * the ROM's 1540 (-48) and 667 encodings against 688 (-21) -- we are SHORT, not
 * long.  tools/aligncmp.py reads 387 aligned-equal of 688 (56.2%), 380
 * differing/ins/del in 122 hunks, and 56.2% is the figure to beat.
 *
 * *** THE FRAME IS EXACT.  `sub sp, #0x5c` on both sides, and every scalar slot
 * lands on the ROM's offset (0x2c ctx, 0x28 dx, 0x24 g2, 0x20 cam, 0x1c kind,
 * 0x18 unit, 0x14 pp, 0x10 fp, 0x0c tp, 0x08 look).  That is BaseAnim_Breath's
 * residue (A) -- "one extra spilled scalar, our frame 0xac against 0xa8" --
 * CLOSED here, and how it closed is lever (4) below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/e3aa0_Attack.c \
 *     asm/rom_c9000/rom_e3958_c_c_c_c_a.s --func BaseAnim_Attack
 * FINAL INSTALLED PATH: src/non_matching/rom_c9000/e3aa0_Attack.c
 *
 * SHIMS: ZERO.  tools/shimcount.py prints the filename and nothing else.
 * PIN-FREE, and that is MEASURED, not assumed -- see "the pin that did not pay".
 *
 * ================================================================
 * THE SPLIT SHAPE -- and it CORRECTS the batch brief
 * ================================================================
 * tools/datacheck.py prints NOTHING for this .s: no data section, so this is a
 * TEXT-ONLY split and it needs NO new `.global`.  The four tables the function
 * reads (.Leedb2, .Leedb8, .Leedbe, .Leedca) live in rom_e3958_c_c_c_c_c.s and
 * are ALREADY `.global` there; Data_ede5c is an `.incdata` symbol in
 * asm/rom_c9000/rom_eda78.s.  Nothing to export.
 *
 * The .s holds THREE functions -- Anim_Attack (0x080e3a3c, a thin wrapper),
 * BaseAnim_Attack (0x080e3aa0), Anim_CriticalHit (0x080e40a4) -- and
 * BaseAnim_Attack is the MIDDLE one, so tools/split_s.py --dry-run cuts THREE
 * ways:
 *     rom_e3958_c_c_c_c_a_a.s   Anim_Attack         (1 function,  46 lines)
 *     rom_e3958_c_c_c_c_a_b.s   BaseAnim_Attack     (1 function, 711 lines)
 *     rom_e3958_c_c_c_c_a_c.s   Anim_CriticalHit    (1 function, 723 lines)
 * THE BRIEF SAID "a split for either puts the other in the same new .s".  THAT
 * IS WRONG FOR THIS ORDER: splitting for BaseAnim_Attack leaves Anim_CriticalHit
 * ALONE in _c.s, so the CriticalHit agent then needs NO split of its own.
 * Splitting for Anim_CriticalHit FIRST is the bad order -- it yields _a.s holding
 * Anim_Attack AND BaseAnim_Attack together and a second split is then needed.
 * SEQUENCE BaseAnim_Attack FIRST.  (Confirmed by --dry-run only; agents cannot
 * run make, so `make compare` after the split is still owed.)
 *
 * ================================================================
 * LEVERS THAT PAID, IN THE ORDER THEY PAID, WITH FIGURES
 * ================================================================
 * Baseline v1 (the whole family template transplanted unchanged, first
 * candidate): size 1488 (-52), count 665 (-23), 327 aligned (47.5%), 136 hunks,
 * objcmp 669.  The RELOCATION SEQUENCE was already almost exact on that first
 * candidate -- every call in the ROM's order -- which is the cheapest possible
 * confirmation that the program shape read off the asm is right.  Everything
 * below is register allocation and addressing, not program shape.
 *
 * (1) THE `ldmia` WALKING-POINTER PROLOGUE.  The ROM opens
 *     `ldr r2,=iwram_3001eec / mov r3,r2 / ldmia r3!,{r1} / ldr r3,[r3]`, which
 *     is BaseAnim_Blob's recorded spelling `g = iwram_3001eec; pp = g;
 *     base = *pp++; ctx = *pp;` -- one pool word plus a walking copy, NOT two
 *     indexed loads.  `g[0]`/`g[1]` gives `ldr r1,[r3] / ldr r2,[r3,#4]` and
 *     defers the pool word.  *** THIS TRANSFERS TO ALL FOUR TARGETS IN THIS
 *     BRIEF: `ldmia r3!, {rN}` is in the opening six instructions of
 *     BaseAnim_RapidSlash, BaseAnim_ParticleCloud and BaseAnim_Nova too. ***
 *
 * (2) BRACE-SCOPE THE LOOP TEMPORARIES PER REGION (batch 306's complement to the
 *     reuse lever).  `half` and `q` are needed in FIVE disjoint places -- the
 *     kind==4 arm, the kind<=2||==5 arm, the kind==3 arm, the gBuffer block and
 *     the PhysMove loop.  Declared once at function level they share one pseudo
 *     whose live_length is the SUM of all five, which inflated the allocno set
 *     until `frame` lost its register.  Declaring `int half` / `int q` INSIDE
 *     each block, together with (1): size 1488 -> 1500, count 665 -> 671 (both
 *     toward the ROM), aligned 47.5% -> 46.2%.  The aligned figure FELL and the
 *     two gating figures both ROSE; (3) is what settled it.
 *
 * (3) TWO WALKING POINTERS FOR ONE PARTICLE ARRAY, not one.  The ROM walks
 *     base + (0xe1 << 7) with r5 in the seed loop and recomputes the same
 *     address into r7 in the PhysMove loop -- two registers, therefore TWO
 *     variables.  Sharing one `Part *p` across both is the same live_length
 *     inflation as (2).  A block-scoped `Part *e` in the PhysMove loop:
 *     aligned 46.2% -> 49.6% with size and count UNCHANGED at 1500/671.
 *     Note this is the DISCRIMINATOR named in lever 3 of the brief working in
 *     the "split" direction, while the loop COUNTER goes the other way: `i` is
 *     r10 in BOTH loops in the ROM, so ONE `i` serves both (counters unify,
 *     walking pointers do not -- same function, both directions, measured).
 *
 * (4) *** THE FRAME-CLOSING CHANGE: WRITE THE PhysMove DESTINATION THROUGH ITS
 *     POINTER, NOT THROUGH THE AGGREGATE'S NAME. ***  The ROM does
 *     `mov r0,r8 / ldr r2,[r0] / ... / str r2,[r0]` and `mov r6,r8 /
 *     ldr r3,[r6,#4]` -- every access to the destination vec goes through the
 *     register holding its address, never sp-relative.  Spelling it `mv.x += dx`
 *     (sp-relative) against `mp->x += dx` (through the pointer) is worth:
 *         mv.x : size 1500, count 671, 341 aligned (49.6%), frame 0x60
 *         mp->x: size 1496, count 669, 374 aligned (54.4%), frame 0x5c  <-- ROM
 *     The frame went 0x60 -> 0x5c and EVERY scalar slot snapped onto the ROM's
 *     offset.  The mechanism: `&mv` taken at function level plus sp-relative
 *     reads keeps both the address and the slot live, so `tp` stayed in a
 *     register and `frame` was spilled instead; through the pointer, `tp` spills
 *     (as the ROM spills it, sp+0x0c) and `frame` keeps r9.  THE ROM'S OWN
 *     SPILL CHOICE WAS THE TARGET, not the absence of a spill.
 *     Reading the y component back either way is BYTE-IDENTICAL (`mp->y` and
 *     `mv.y` measured separately, 1496/669/54.4% both) -- it is the STORE that
 *     carries the lever, and that is the useful half of the finding.
 *
 * (5) THE TABLE VALUE AS A NAMED LOCAL in the kind==3 draw arm.  `.Leedb8[q]` is
 *     used TWICE there (as the sixth argument and inside `tp->y - (h >> 1)`),
 *     which satisfies batch 306's more-than-one-use precondition.  Naming it:
 *     aligned 54.4% -> 56.2%, 380 differing in 122 hunks, at the cost of two
 *     instructions gcc then commons (size 1496 -> 1492, count 669 -> 667).
 *     THE TWO RANKING VIEWS SPLIT HERE and this file keeps the named form:
 *         named `h`  (this) : size 1492 (-48), count 667 (-21), 56.2% aligned
 *         inline           : size 1496 (-44), count 669 (-19), 54.4% aligned
 *     Grounds: neither size nor count is exact so objcmp's count is not a
 *     distance and aligncmp ranks; the named form has a KNOWN mechanism and one
 *     identifiable extra defect, the inline form has 13 fewer exact encodings and
 *     no mechanism.  Whoever reopens this should re-rank the moment size and
 *     count go exact.  Naming BOTH table values (h and w) in BOTH arms is WORSE
 *     (55.2%, 109 hunks) and it also REVERSES the pool-word order -- ours comes
 *     out .Leedb8/.Leedb2/.Leedbe/.Leedca against the ROM's
 *     .Leedbe/.Leedca/.Leedb8/.Leedb2 -- so name the twice-used value only.
 *
 * (6) READ THE DISPATCH OFF THE ASM, and this function needs BOTH FORMS AT ONCE.
 *     The file selection is `if (kind == 4) ... else if (kind == 3) ... else
 *     switch (kind) { case 0: case 1: case 5: ...; case 2: ...; }`.  The tell is
 *     that there is NO `.word` table (so not one switch over 0..5, which would
 *     have got a table) but the tail IS emit_case_nodes output:
 *     `cmp r3,#2 / beq / cmp r3,#2 / bgt / cmp r3,#0 / blt / b` -- a balanced
 *     tree over THREE case nodes (the 0-1 RANGE, 2, and 5), and the repeated
 *     `cmp #2` is gcc's own redundancy, not a source duplication.  An if-chain
 *     cannot produce the duplicated compare and a single switch cannot avoid the
 *     jump table; the hybrid is the only shape that gives both.
 *
 * ================================================================
 * THE PIN THAT DID NOT PAY -- AND THE RECON PREDICTED IT
 * ================================================================
 * BaseAnim_Blob needs per-site r0/r2 pins on its THREE Func_8001af8 calls
 * because two of the three sites share a basic block and cse2 unifies
 * `0xa0 << 19` across them.  BaseAnim_Attack has TWO sites in DIFFERENT basic
 * blocks (the two arms of `slot->f8 > 7`), so there is nothing for the pin to
 * defeat, and the measurement says exactly that:
 *     pin-free (this)      : size 1492, count 667, 374/669-basis 54.4% -> 56.2%
 *     r0/r2 pinned, 4 pins : size 1496, count 669, 372 aligned (54.1%)
 * Identical size and count, 2 fewer exact encodings, 4 shims bought for nothing.
 * TENTH INSTANCE of a pin measuring no better across this tree, and the first
 * where the SITE-COUNT PRECONDITION was stated in advance and then confirmed.
 * *** THE RULE THIS SETTLES: the three-copy-site pin is not a family lever, it
 * is a BASIC-BLOCK lever.  Count the sites that SHARE A BLOCK before reaching
 * for it -- Blob has two in one block and needs it, Breath has one site and is
 * pin-free, Attack has two in two blocks and is pin-free. ***
 * Also note the ROM's argument order here is gcc's OWN natural order
 * (`mov r0,#0xa0 / ldr r3,=Func_8001af8 / lsl r0,#19 / mov r2,#0x80`), the exact
 * transposition src/rom_c9000/rom_cc5d8_a_a_b.c had to pin its way out of. The
 * ordinary-local spelling `d0 = 0xa0; copy = Func_8001af8; d0 <<= 19;` reaches
 * it with no shim, so do not transplant that file's pins along with its idiom.
 *
 * ================================================================
 * MEASURED INERT (untested, not disproved)
 * ================================================================
 *   - A `u16` carrier for REG_BG1CNT instead of an `int` one: BYTE-IDENTICAL.
 *   - Reversing the declaration order of the three `vec3_t` aggregates:
 *     BYTE-IDENTICAL (1492/667/56.2%), so the aggregate slots are not being
 *     chosen by declaration order in this function.
 *   - `mp->y` vs `mv.y` at the draw call (see lever 4).
 * MEASURED WORSE:
 *   - Declaring `frame` FIRST among the spilled scalars: 343 aligned (49.9%)
 *     against 387, same size and count.  Declaration order does not buy the
 *     register; shortening the competitors' live ranges did (levers 2-4).
 *
 * ================================================================
 * THE BLOCKER, BY PASS
 * ================================================================
 *
 * (A) *** 0x1f80 CANNOT BE POOLED, AND THIS IS A HARD gcc-2.96 LIMIT, NOT A
 *     SPELLING PROBLEM. ***  The ROM writes REG_BG1CNT twice from WORD pool
 *     words at its own local labels (`.Le3b04 @ 0x1f80`, `.Le3ca4 @ 0x1f81`):
 *     `ldr r2,=REG_BG1CNT / ldr r3,.Le3b04 / strh r3,[r2]`.  We emit
 *     `ldr r3,=REG_BG1CNT / movs r2,#0xfc / lsls r2,#5 / strh r2,[r3]`.
 *     0x1f80 == 0xfc << 5 IS `thumb_shiftable_const`, and gcc-2.96's Thumb
 *     define_split rewrites EVERY shiftable SImode CONST_INT from a pool load
 *     into mov+lsl unconditionally, so a shiftable constant can never stay in
 *     the pool once it reaches a `(set reg const_int)`.  An `int` carrier is
 *     what makes 0x1010 pool on Blob and Breath -- 0x101 > 255 so 0x1010 is NOT
 *     shiftable -- and it CANNOT work here.  A `u16` carrier is byte-identical
 *     (gcc promotes it back to SImode).  *** SO THE `int` CARRIER LEVER IS
 *     CONDITIONAL ON THE CONSTANT NOT BEING SHIFTABLE, and the brief's claim
 *     that it "transfers to all four" is HALF TRUE: it transfers wherever the
 *     constant is unshiftable (0x1010 in RapidSlash and Nova, both `.word`
 *     pools in the ROM) and is unreachable for 0x1f80. ***  Cost here: +1
 *     instruction and -1 pool word per site, two sites.  Batch 300's rule
 *     "pooled + shiftable + SImode implies a SYMBOL" would predict a symbol, but
 *     the reference word carries NO relocation, so the rule has a third case it
 *     does not cover and this is the counter-example.
 *
 * (B) CONSTANT-POOL PLACEMENT -- arm_reorg / dump_table, downstream of (C).
 *     1540 bytes is well past Thumb's 1020-byte `ldr rd,[pc]` reach so the pool
 *     MUST split; the ROM dumps 0x64..0x70 (4 words, behind a `b .Le3b14` gcc
 *     inserted itself), 0x208..0x240, and 0x5d8..0x600, and we dump two blocks
 *     instead of three.  ONE VISIBLE CONSEQUENCE IS WORTH RECORDING: the ROM has
 *     exactly ONE `iwram_3001ad0` pool word serving BOTH the `[2] = dx` store at
 *     ~0x2a0 and the `[3] = frame` store at ~0x5c0 (824 bytes apart, inside the
 *     reach); we emit TWO because our first block lands between them.  A missing
 *     or extra pool word is a size-and-count defect, so part of the -48/-21 is
 *     this and not a missing instruction.  Do not chase it directly.
 *
 * (C) THE REMAINING SHORTFALL IS CSE UNIFYING THINGS THE ROM RECOMPUTES, and it
 *     is where the next round should go.  The three places aligncmp shows ref
 *     instructions with no counterpart:
 *       - `(*slotC)->ids[0]` is loaded TWICE in the ROM, once for
 *         _GetBattleActor and again for _Func_80b8530 across the call
 *         (`ldr r3,[r5] / ldr r6,[r0] / movs r2,#0x24 / ldrsh r0,[r3,r2]`, 5
 *         instructions).  We keep the first result live.  Both calls clobber
 *         memory, so this is gcc choosing a callee-saved register over a reload;
 *         the handle is pressure at that point, not the expression.
 *       - `movs r5,#0x24 / ldrsh r0,[r3,r5]` for the _SetBattleActorKnockback
 *         argument, unified with the slotD read two instructions later.  NOTE
 *         the ROM's four `0x24` offsets each land in a DIFFERENT register
 *         (r1/r2/r5/r6) -- that is NOT a lever, Thumb-1 LDRSH has no immediate
 *         form at all, so `slot->ids[0]` gives the register+register form for
 *         free and there is nothing to spell here.
 *       - two of the six `ldr r4,[r5,#4]` (fp[1]) loads, cross-jumped in ours.
 *     Per batch 305, sched2's tie-break cannot be the explanation for any of
 *     these and sched1 does not run; they are CSE/local-alloc.
 *
 * (D) The high-register rotation is GONE as of lever (4): `base` is r9 in ours
 *     and r11 in the ROM, but the SET is now right -- r8, r9, r10, r11 all
 *     carry a quantity on both sides and `frame` is in a register.  Worth
 *     recording against BaseAnim_Breath's residue (B), which reads the same
 *     rotation as "one EXTRA allocno": here it was one MISPLACED allocno, and
 *     the fix was the ROM's own spill choice (lever 4), not another quantity.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];
extern short iwram_3001ad0[];
extern void *gPtrs[];
extern unsigned char gBuffer[];
extern unsigned short Leedbe[] __asm__(".Leedbe");
extern unsigned char  Leedca[] __asm__(".Leedca");
extern unsigned char  Leedb8[] __asm__(".Leedb8");
extern unsigned char  Leedb2[] __asm__(".Leedb2");
extern unsigned short Data_ede5c[];

extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern int  Random(void);
extern void GetBattleActorPos(int unit, int *dest);
extern void GetBattleActorPos2(int unit, vec3_t *out);
extern int *_GetBattleActor(int id);
extern int  _Func_80b8530(int id);
extern unsigned char *_GetUnit(int id);
extern int  _GetEnemyUnk(int id);
extern void _SetBattleActorKnockback(int id, int n);
extern void _Func_80bd7dc(int a);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int  PhysMove(Part *p, vec3_t *out);
extern void Func_80e38b8(Part *p, int a, int b);
extern void Func_80c9048(void);
extern void InitRenderTilemapBG1(void);
extern void Func_80cdd14(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void BaseAnim_Attack(void *context)
{
    vec3_t t2;
    vec3_t pos;
    vec3_t mv;
    DrawFn fns[2];
    void *ctx;
    int dx;
    void *g2;
    void *cam;
    int kind;
    unsigned char *unit;
    vec3_t *pp;
    DrawFn *fp;
    vec3_t *tp;
    void *look;
    void **g;
    void **w;
    unsigned char *base;
    State **slotA;
    State **slotB;
    State **slotC;
    State **slotD;
    DrawFn f1;
    CopyFn copy;
    Part *p;
    int *rec;
    int i;
    int frame;
    int mask;
    int d2;
    int arg;

    g = iwram_3001eec;
    w = g;
    base = (unsigned char *)*w++;
    ctx = *w;
    g2 = g[2];
    cam = *(void **)((char *)g - 0x6c);
    kind = ((State *)context)->f0;
    slotA = (State **)(base + 0x7828);
    *slotA = (State *)context;
    unit = _GetUnit(((State *)context)->f8);
    WaitFrames(1);
    Func_80c9048();
    InitRenderTilemapBG1();
    { int b0 = 0x1f80; REG_BG1CNT = b0; }
    WaitFrames(1);
    if (kind == 5) {
        if ((*slotA)->f4 == 0) {
            BuildDraw2DFuncEx(0x2e, 7, 7, 0xb, 3);
            BuildDraw2DFuncEx(0x2f, 7, 7, 0xb, 2);
        } else {
            BuildDraw2DFuncEx(0x2e, 7, 7, 0xf, 3);
            BuildDraw2DFuncEx(0x2f, 7, 7, 0xf, 2);
        }
    } else {
        if ((*slotA)->f4 == 0) {
            BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
            BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
        } else {
            BuildDraw2DFuncEx(0x2e, 7, 7, 7, 3);
            BuildDraw2DFuncEx(0x2f, 7, 7, 7, 2);
        }
    }
    fns[0] = (DrawFn)gPtrs[0x2e];
    f1 = (DrawFn)gPtrs[0x2f];
    fp = fns;
    fp[1] = f1;
    WaitFrames(1);
    if (kind == 4) {
        LoadVFXFile(FILE_6b, base, 1, 1);
    } else if (kind == 3) {
        LoadVFXFile(FILE_c5, base, 0, 0);
    } else {
        switch (kind) {
        case 0:
        case 1:
        case 5:
            LoadVFXFile(FILE_b5, base, 1, 1);
            break;
        case 2:
            LoadVFXFile(FILE_b6, base, 1, 1);
            break;
        }
    }
    if ((*(State **)(base + 0x7828))->f8 > 7) {
        void *s;
        int d0;
        s = GetFile(FILE_8e);
        d0 = 0xa0;
        copy = Func_8001af8;
        d0 <<= 19;
        copy((volatile u16 *)d0, s, 0x80);
    } else {
        void *s;
        int d0;
        s = GetFile(FILE_4a);
        d0 = 0xa0;
        copy = Func_8001af8;
        d0 <<= 19;
        copy((volatile u16 *)d0, s, 0x80);
    }
    WaitFrames(1);
    LoadVFXFile(FILE_76, g2, 0, 0);
    LoadVFXFile(FILE_99, gBuffer, 1, 0);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x32;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    slotB = (State **)(base + 0x7828);
    { int b1 = 0x1f81; REG_BG1CNT = b1; }
    pp = &pos;
    GetBattleActorPos((*slotB)->ids[0], (int *)pp);
    if ((*slotB)->f4 == 0)
        dx = 0x60 - pp->x;
    else
        dx = 0x20 - pp->x;
    if (dx > 0)
        dx = 0;
    if (dx < -0x80)
        dx = -0x80;
    pp->x += dx;
    iwram_3001ad0[2] = dx;
    iwram_3001ad0[3] = 0x50;
    slotC = (State **)(base + 0x7828);
    WaitFrames(1);
    rec = (int *)*_GetBattleActor((*slotC)->ids[0]);
    d2 = _Func_80b8530((*slotC)->ids[0]) / 2;
    p = (Part *)(base + (0xe1 << 7));
    i = 0;
    mask = 0xff;
    do {
        p->x = rec[2];
        p->y = rec[3] + d2;
        p->z = rec[4];
        p->vx = (Random() & mask) << 10;
        p->vy = ((Random() & mask) - 0x20) << 10;
        p->vz = ((Random() & mask) - 0x7f) << 10;
        if (p->x > 0)
            p->vx = -p->vx;
        p->vx = -p->vx;
        p->t = i + 0x10;
        i++;
        p++;
    } while (i != 0x40);
    tp = &t2;
    look = (char *)cam + 0xc;
    frame = 0;
    do {
        if (frame == 5) {
            if (_GetEnemyUnk(unit[0x94 << 1]))
                _Func_80bd7dc(0x86);
            else
                _Func_80bd7dc(0x85);
        }
        if (frame == 4)
            _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[0], 0);
        slotD = (State **)(base + 0x7828);
        GetBattleActorPos2((*slotD)->f8, tp);
        {
        int yy;
        int fr;
        yy = tp->y;
        tp->y = yy + 0x10;
        if (kind == 4) {
            if (frame <= 0xb) {
                int half = frame / 2;
                int q = 5 - half;
                if ((*slotD)->f4 == 0)
                    fp[1](ctx, base + ((q * 2 + q) << 8),
                          tp->x + dx - 0x30, yy + 8, 0x30, 0x10);
                else
                    fp[1](ctx, base + ((q * 2 + q) << 8),
                          tp->x + dx, yy + 8, 0x30, 0x10);
            }
        } else if ((unsigned)kind <= 2 || kind == 5) {
            if (frame <= 0xb) {
                int half = frame / 2;
                int q = (half * 8 - half) * 4 - half;
                if ((*slotD)->f4 == 0)
                    fp[1](ctx, base + (q << 7),
                          tp->x + dx - 0x30, yy - 0x18, 0x30, 0x48);
                else
                    fp[1](ctx, base + (q << 7),
                          tp->x + dx, yy - 0x18, 0x30, 0x48);
            }
        } else {
            if (frame <= 0x11) {
                int q = frame / 3;
                if ((*slotD)->f4 == 0) {
                    int h = Leedb8[q];
                    fp[1](ctx, base + Leedbe[q],
                          tp->x + Leedca[q] + dx - 0x3a,
                          tp->y - (h >> 1), Leedb2[q], h);
                } else
                    fp[1](ctx, base + Leedbe[q],
                          tp->x - Leedca[q] + dx - Leedb2[q] + 0x3a,
                          tp->y - (Leedb8[q] >> 1),
                          Leedb2[q], Leedb8[q]);
            }
        }
        fr = frame - 4;
        if ((unsigned)fr <= 0xb) {
            int half = fr / 2;
            fns[0](ctx, gBuffer + (((half << 4) - half) << 7),
                   pp->x - 0x10, pp->y - 0x18, 0x28, 0x30);
        }
        InitMatrixStack();
        MatrixSetLook(cam, look);
        if ((unsigned)fr <= 0x1b) {
            vec3_t *mp = &mv;
            i = 0;
            do {
                int half = i / 2;
                Part *e = (Part *)(base + ((half * 8 - half) * 4) + (0xe1 << 7));
                if (e->t > 0) {
                    int tq;
                    int sz;
                    PhysMove(e, mp);
                    mp->x += dx;
                    tq = (e->t >> 3) + 2;
                    sz = tq * 2;
                    fp[half & 1](ctx,
                        (char *)g2 + *(unsigned short *)((char *)Data_ede5c + (sz - 2)),
                        mp->x - tq, mp->y - tq, sz, sz);
                    Func_80e38b8(e, 0x3c, -0x400);
                    e->t -= 1;
                }
                i++;
            } while (i != 0x40);
        }
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x20);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    iwram_3001ad0[3] = frame;
    Func_80cdd14();
}

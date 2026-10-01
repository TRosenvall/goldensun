/* BaseAnim_Heal -- 0x080d89ac, asm/rom_c9000/rom_d8948_c_c_c_c_c_c.s line 12,
 * 879 ROM instructions.
 * NON-MATCHING, 824 of 915 encodings differ.
 *
 * SIZE AND COUNT ARE BOTH INEXACT, so 824 is NOT a distance: 2028 bytes against
 * the ROM's 2024 (+4) and 917 encodings against 915 (+2) -- we are LONG by two
 * instructions.  tools/aligncmp.py reads 639 aligned-equal of 915 (69.8%), 316
 * differing/ins/del in 176 hunks, and 69.8% is the figure to beat.
 *
 * *** THE RELOCATION SEQUENCE IS EXACT.  84 relocations on both sides, every
 * call and every symbol in the ROM's order, `_call_via_r3` included.  That is
 * rung 4 of the brief's ladder passing, and it means the program shape, the call
 * order and the pool-word order are all right -- what is left is register
 * allocation and two instructions. ***
 *
 * FRAME: ours `sub sp, #0x88`, the ROM's `sub sp, #0x80` -- 8 bytes LONG (two
 * scalars), attributed in blocker A below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/d89ac_Heal.c \
 *     asm/rom_c9000/rom_d8948_c_c_c_c_c_c.s --func BaseAnim_Heal
 * FINAL INSTALLED PATH: src/non_matching/rom_c9000/d89ac_Heal.c
 *
 * SHIMS: ZERO.  tools/shimcount.py prints the filename and nothing else.
 * PIN-FREE, and that is MEASURED: no pin was ever installed, so no pin is owed
 * a measurement here.
 *
 * ================================================================
 * THE SPLIT SHAPE -- THERE IS NONE, AND THIS IS THE CHEAPEST TARGET IN THE BRIEF
 * ================================================================
 * `grep -c thumb_func_start asm/rom_c9000/rom_d8948_c_c_c_c_c_c.s` prints 1:
 * BaseAnim_Heal is ALONE in its .s.  tools/datacheck.py prints NOTHING for the
 * file -- no data section at all.  So NO SPLIT, NO new `.global`, NO linker
 * script edit, NO split_s.py run.  The same holds for BaseAnim_StatUp
 * (rom_d9194_c_c_c_c_c_c.s).  Those two are the only split-free targets of the
 * five; Tiamat and Bite_Sting are alone but carry `.rodata`, and Meteor shares
 * an eight-function file.
 *
 * Every external the file names already exists: Data_ede84 / Data_ede96 are
 * `.incdata` at asm/rom_c9000/rom_eda78.s:33-34, Data_ede48 is declared in
 * src/non_matching/rom_c9000/decls.h, and the rest are ordinary calls.
 *
 * ================================================================
 * LEVERS THAT PAID, IN THE ORDER THEY PAID, WITH FIGURES
 * ================================================================
 * Baseline v1 -- the family skeleton read straight off the asm, first candidate:
 * size 2028 (+4), count 917 (+2), objcmp 864, 514 aligned (56.2%), 190 hunks.
 * *** THE RELOCATION SEQUENCE WAS ALREADY ALMOST EXACT ON THAT FIRST CANDIDATE
 * -- 86 relocations, every call and every symbol in the ROM's order, with only
 * two differences (a `_call_via_r2` for `_call_via_r3`, and the Data_ede84 /
 * Data_ede96 pool pair transposed).  That is rung 4 of the ladder passing on v1,
 * and it says the program shape read off the asm is right.  Everything below is
 * register allocation and compare spelling, not program shape.
 *
 * (1) THE `ldmia` WALKING-POINTER PROLOGUE, transplanted from BaseAnim_Nova.
 *     `g = iwram_3001eec; pp = g; base = *pp++; ctx = *pp; g2 = g[2];` gives the
 *     ROM's `ldr r2,=iwram_3001eec / mov r3,r2 / ldmia r3!,{r0} / ldr r3,[r3] /
 *     ldr r2,[r2,#8]`.  Byte-exact in the opening ELEVEN encodings on v1.
 *     *** CONFIRMED TRANSFERRING: identical in BaseAnim_StatUp, which makes it
 *     six siblings deep.  This is the one thing in the family that is NOT
 *     per-function. ***
 *
 * (2) HOIST `2 - (variant != 6)` INTO A NAMED LOCAL, and the asm says to.
 *     The ROM computes `movs r3,#6 / eors r3,r0 / negs r2,r3 / orrs r2,r3 /
 *     lsrs r1,r2,#31 / movs r3,#2 / subs r1,r3,r1` ONCE, BEFORE the
 *     `cmp #6 / beq / cmp #0 / bne` dispatch.  Written inline in both
 *     Anim_Djinni arms the sequence is DUPLICATED, one copy per arm.  Naming it
 *     `int kind` above the `if`:
 *         inline : size 2028 (+4), count 917 (+2), objcmp 864, 56.2%
 *         named  : size 2020 (-4), count 913 (-2), objcmp 810, 56.9%
 *     *** THE GENERAL FORM: a value the ROM materialises BEFORE a dispatch and
 *     uses in more than one arm is a DECLARED LOCAL; one materialised inside
 *     each arm is not.  The position of the computation relative to the branch
 *     is source information, the same way the brief's access count is. ***
 *
 * (3) *** AN EQUALITY CHAIN, NEVER A RANGE COMPARE -- gcc FOLDS IT AND THE
 *     FOLD IS UNSIGNED. ***  The ROM's file dispatch is
 *     `cmp r1,#1 / BLS / cmp r1,#3 / beq / cmp r1,#4 / beq / cmp r1,#5 / bne`.
 *     `bls` is UNSIGNED.  Writing `variant <= 1 || variant == 3 || ...` gives
 *     `BLE` -- signed, and wrong.  Writing the chain out,
 *     `variant == 0 || variant == 1 || variant == 3 || variant == 4 ||
 *     variant == 5`, makes fold_range_test merge the two adjacent equalities
 *     into an UNSIGNED `(unsigned)variant <= 1` and the `bls` lands.  Same fix
 *     at the second site (`variant <= 2 || variant == 6` -> four equalities).
 *     Two sites: objcmp 810 -> 808, aligned 56.9% -> 57.2%.
 *     *** THIS IS A SPELLING TRAP OF EXACTLY THE `char`-IS-UNSIGNED CLASS: the
 *     range compare is SHORTER to write and reads as the obvious source, and it
 *     is a different instruction.  Grep any candidate for `bls`/`bhi` against
 *     `ble`/`bgt` before believing an if-chain. ***
 *
 * (4) UNIFY THE SEED COUNTER AND THE PARTICLE COUNTER INTO ONE VARIABLE.
 *     The ROM uses r8 for the counter of all five `gBuffer` seed loops AND for
 *     the inner particle loop -- disjoint ranges, one register.  Per the brief
 *     that is a suggestion and not proof, so both were measured:
 *         split (two vars `i`, `j`) : size 2020 (-4), count 913 (-2), objcmp 808, 523 aligned (57.2%)
 *         unified (one var `i`)     : size 2036 (+12), count 921 (+6), objcmp 850, 574 aligned (62.7%)
 *     *** THE TWO RANKING VIEWS SPLIT HERE and this file keeps the UNIFIED form.
 *     Grounds, and they are structural rather than numeric: unifying the same
 *     two counters in the sibling BaseAnim_StatUp improved ALL FOUR axes at once
 *     (size -12 -> -8, count -6 -> -4, objcmp 854 -> 844, aligned 63.4% ->
 *     64.5%) with its frame staying exact.  So unification is the ROM's shape,
 *     confirmed on a second function from independent evidence, and the +16
 *     bytes it costs here is a SEPARATE defect it exposed rather than a defect
 *     it caused.  The brief's rank-size-and-count-first rule would keep the
 *     split form; whoever reopens this should re-rank the moment the three
 *     spills in blocker A are closed, because that is where the +16 lives. ***
 *     The split form is `int i; int j;` with `j` in the particle loop -- it is
 *     one sed away and both figure sets are above.
 *
 * (5) *** DECLARE THE TABLE-DERIVED SOURCE POINTER ABOVE THE WIDTH, AND IT FIXES
 *     THE POOL-WORD ORDER -- the single biggest lever on this function. ***
 *     The draw arm reads TWO tables at the same site: `Data_ede84[u]` for the
 *     source offset and `Data_ede96[u]` for the width.  Anim_Frost's park
 *     records "name the source pointer first", and Frost's own spelling is
 *     `unsigned char w = Data_ede96[u];` with `base + Data_ede84[u]` left
 *     INSIDE the call -- which is NOT sufficient, because gcc evaluates a call's
 *     arguments RIGHT TO LEFT, so a width declared above the call is still
 *     referenced before any argument.  The pool came out Data_ede96 then
 *     Data_ede84, the ROM's order being Data_ede84 then Data_ede96.
 *     Lifting the pointer into its own declaration ABOVE the width --
 *         unsigned char *s = base + Data_ede84[u];
 *         unsigned char  w = Data_ede96[u];
 *         fp[1](ctx, s, q->x - (w >> 1), q->y - (w >> 1), w, w);
 *     -- puts Data_ede84 first and is worth, on every axis at once:
 *         embedded : size 2036 (+12), count 921 (+6), objcmp 850, 574 aligned (62.7%), frame 0x8c
 *         named    : size 2028  (+4), count 917 (+2), objcmp 824, 639 aligned (69.8%), frame 0x88
 *     +7.1 points of alignment, 8 instructions, 4 frame bytes, and it is what
 *     takes the RELOCATION SEQUENCE EXACT (the `_call_via_r2` for
 *     `_call_via_r3` defect went with it, so that was downstream of the pool
 *     order, not an independent pin problem).
 *     *** THE CORRECTION THIS MAKES TO Anim_Frost'S RULE: with two tables at one
 *     site, pool order follows the order of the DECLARATIONS, and an expression
 *     left embedded in the argument list is referenced LAST, not first, because
 *     of right-to-left argument evaluation.  "Name the source pointer first"
 *     must mean a separate declaration placed first, not merely leaving the
 *     other one embedded. ***
 *     Doing the same to the OTHER draw arm (the single-table `Data_ede48[u-1]`
 *     site) is BYTE-IDENTICAL, and so is the analogous change in the sibling
 *     BaseAnim_StatUp -- which narrows the lever precisely: it bites only where
 *     TWO tables share one draw site.
 *
 * ================================================================
 * MEASURED INERT (untested, not disproved)
 * ================================================================
 *   - Writing the per-actor slot pointer `(State **)(base + 0x7828)` INLINE at
 *     both of its uses instead of naming it: BYTE-IDENTICAL (2020/913/808/57.2%
 *     either way).  cse commons the address within the iteration regardless, so
 *     the brief's global-pointer rule does not bite here -- and the discriminator
 *     it gives says why: what crosses the call is the pointer's ADDRESS, which
 *     is a constant, not its value.  This file keeps the inline form because it
 *     is the ROM's shape (`ldr r0,[sp,#0x48] / add r6,r0,r2` at the top of each
 *     iteration, re-derived, not hoisted).
 *   - `int yaw = 0;` at the declaration, to lower yaw's priority and make it
 *     lose r11 (the brief's initialise-at-declaration lever): BYTE-IDENTICAL.
 *     *** AND THE REASON IS A NARROWING OF THAT LEVER WORTH RECORDING: `yaw` is
 *     unconditionally assigned before any read, so the initialiser is DEAD and
 *     flow deletes it -- the pseudo never becomes live at entry and the live
 *     length never grows.  The lever needs the initialiser to be LIVE, which is
 *     true of the `int clen = 0x80 << 7;` case it was found on (the value is the
 *     one actually used) and false of any variable the body overwrites first.
 *     So: initialise-at-declaration lowers priority only where the initialised
 *     VALUE is what the function uses. ***
 * MEASURED WORSE:
 *   - Naming the three squares of the magnitude sum (`int sx = p->x >> 8;` etc).
 *     The ROM's `mov r3,r1 / add r0,r3` extra copy reads like a source local and
 *     is not one: size 2016 (-8), count 911 (-4), objcmp 842, aligned 56.0%
 *     against 56.9%.  It is a reload copy.
 *   - Naming the draw width and height in the two frame-draw arms (`int w =
 *     0x28;` for the arm that passes one value twice, `int w`/`int h` for the
 *     arm that passes 0x14 and 0x28).  The ROM holds both in registers (r6, and
 *     r8 shared with the counter) and reuses them across the arm's two calls,
 *     which reads exactly like the brief's reuse lever -- it is not: size 2036
 *     (+12), count 921 (+6), aligned 53.9% against 57.2%.  gcc's own constant
 *     reuse, not a declared quantity.
 *   - Materialising `&fns` at the BuildDraw2DFuncs call and assigning `fp` after
 *     it, to reproduce the ROM's `mov r3,sp / adds r3,#0x4c / adds r1,r3 /
 *     str r3,[sp,#0x28]` quartet: objcmp 808 -> 810, aligned 57.2% -> 57.0%.
 *
 * ================================================================
 * THE BLOCKER, BY PASS
 * ================================================================
 *
 * (A) *** THE REMAINING 8-BYTE FRAME EXCESS IS TWO PSEUDOS THAT LOSE THEIR HARD
 *     REGISTERS IN local-alloc / global-alloc, AND BOTH ARE NAMED. ***
 *     Our spill offsets are the ROM's plus a constant at every single slot, so
 *     the extra words sit at the BOTTOM of the scalar region -- allocated last,
 *     lowest priority.  19 scalar slots against the ROM's 17.  Read out of the
 *     generated .s the survivor is `frame` at ours sp+0x10, which the ROM keeps
 *     in r11 (19 references); in exchange we give r11 to `yaw`, which the ROM
 *     SPILLS to its sp+0x14.  (Before lever 5 the excess was 12 bytes and three
 *     pseudos -- `frame`, `lim` and the per-actor slot pointer; lever 5 closed
 *     one of them, which is why a pool-order fix is reported as a frame lever.)
 *     So this is not a missing spill and not an extra quantity: it is a
 *     four-way PARTITION of the high registers landing on a different set.
 *     ROM: r8 counters, r9 lim, r10 variant, r11 frame.
 *     Ours: r8 (counters), r9 variant, r10, r11 yaw.
 *     What rules out the alternatives, measured rather than assumed: it is not
 *     a declaration-order effect (the slot map is already in descending
 *     declaration order and matches the ROM's variable-for-variable above the
 *     three extras); it is not the initialise-at-declaration lever (inert, and
 *     for a known reason -- see above); and it is not the vec-store lever that
 *     closed BaseAnim_Attack's frame, because every vec destination here is
 *     ALREADY written through its pointer (`tvp->x`, `q->x`, `p->x`) and there
 *     is no sp-relative aggregate store left to convert.  The remaining lever
 *     from the brief's allocation family that was NOT reachable is "reuse a
 *     variable to inherit its register" for `frame` -- its four preconditions
 *     need a donor whose earlier range already lands in r11, and there is none:
 *     r11 is live across the entire outer loop in the ROM, so no disjoint donor
 *     exists.  That is the next thing to try and it needs a different idea, not
 *     another spelling of this one.
 *
 * (B) MID-FUNCTION POOL PLACEMENT, arm_reorg / dump_table, downstream of (A).
 *     2024 bytes is far past Thumb's 1020-byte `ldr rd,[pc]` reach so the pool
 *     MUST split.  The ROM dumps a 0x100 word at its own local label behind a
 *     `b` it inserted itself; ours is 12 bytes longer in the body so every pool
 *     offset before the first dump differs by a constant.  NOT a residue to
 *     chase independently of (A).
 *
 * (C) NOT A RESIDUE, recorded so it is not chased: `ldr r3, <pool> @ 0x100` for
 *     `REG_BG2PA = 0x100`.  0x100 is `thumb_shiftable_const` so a pooled WORD
 *     looks impossible after BaseAnim_Attack's finding -- but REG_BG2PA is
 *     `vu16`, gcc emits `ldrh rX,<pool>` for a HImode volatile store, and gas
 *     assembles Thumb `ldrh <pool-label>` to the same halfword as `ldr`.  This
 *     is the FIFTH time that has been recorded (Anim_Frost has the fourth) and
 *     it reproduces byte-exactly here from the plain `REG_BG2PA = 0x100;`.
 *     The sibling BaseAnim_StatUp has the same shape with 0xcc.
 */
#include "decls.h"

extern void *iwram_3001e80;
extern void *GetFile(int id);
extern int  DecompressLZ(void *src, void *dst);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern void MatrixTranslatev(vec3_t *v);
extern void Anim_Djinni(void *context, int a, int b, int c, int *p1, int *p2);
extern int  Func_8000948(int v);
extern unsigned short Data_ede84[];
extern unsigned char  Data_ede96[];

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef int (*MagFn)(int v);

void BaseAnim_Heal(void *context, int variant)
{
    vec3_t vec;
    vec3_t pv;
    vec3_t tv;
    int p58;
    int p54;
    DrawFn fns[2];
    unsigned char *base;
    void *ctx;
    int nframes;
    int b;
    void *g2;
    int cnt;
    int dx;
    int mode;
    DrawFn *fp;
    void *look;
    void *look2;
    vec3_t *tvp;
    int vmode;
    int yaw;
    int io;
    int ang;
    int poff;
    int kind;
    int **g;
    int **pp;
    State **slot;
    int i;
    int frame;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    g2 = g[2];
    dx = 0;
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(0);
    if ((*slot)->f1c == 1) {
        kind = 2 - (variant != 6);
        if (variant == 6 || variant == 0)
            Anim_Djinni(context, kind, (*slot)->f4, 0, &p58, &p54);
        else
            Anim_Djinni(context, kind, (*slot)->f4, 1, &p58, &p54);
        (*(State **)(base + 0x7828))->f18 = 0;
    }
    if (variant == 0) {
        GetBattleActorPos2((*(State **)(base + 0x7828))->ids[0], &vec);
        dx = 0x40 - vec.x;
        REG_BG2X = dx << 8;
        REG_BG2PA = 0x100;
        mode = 0;
    } else {
        mode = 1;
    }
    LoadVFXFile(FILE_73, g2, 0, 0);
    LoadVFXFile(FILE_ba, base, 0, 0);
    {
        int id;
        void *s;
        int d0;
        CopyFn copy;

        if (variant == 0 || variant == 1 || variant == 3 || variant == 4
            || variant == 5) {
            if ((*(State **)(base + 0x7828))->f18 == 0)
                id = FILE_b3;
            else
                id = FILE_b9;
        } else if (variant == 6) {
            id = FILE_8d;
        } else {
            id = FILE_c0;
        }
        s = GetFile(id);
        d0 = 0xa0;
        copy = Func_8001af8;
        d0 <<= 19;
        copy((volatile u16 *)d0, s, 0x80);
    }
    {
        int id;
        void *s;

        if (mode == 0) {
            if (variant == 6)
                id = FILE_8d;
            else
                id = FILE_91;
        } else {
            if (variant == 6)
                id = FILE_8e;
            else
                id = FILE_92;
        }
        s = GetFile(id);
        DecompressLZ((char *)s + 0x80, base + (0x80 << 5));
    }
    fp = fns;
    BuildDraw2DFuncs((*(State **)(base + 0x7828))->f4, fp);
    if (variant == 0 || variant == 6) {
        Part *p = gBuffer;
        i = 0;
        do {
            p->x = ((Random() & 0xff) - 0x7f) << 15;
            p->y = ((Random() & 0x7f) + 0x40) << 15;
            p->z = ((Random() & 0xff) - 0x7f) << 15;
            p->t = 0;
            i++;
            p++;
        } while (i != 0x200);
        nframes = ((*(State **)(base + 0x7828))->f14 << 3) + 0x58;
    } else if (variant == 1) {
        Part *p = gBuffer;
        i = 0;
        do {
            p->x = ((Random() & 0xff) - 0x7f) << 15;
            p->y = ((Random() & 0xff) - 0x7f) << 15;
            p->z = ((Random() & 0xff) - 0x7f) << 15;
            p->t = 0;
            i++;
            p++;
        } while (i != 0x200);
        nframes = ((*(State **)(base + 0x7828))->f14 << 3) + 0x58;
    } else if (variant == 2) {
        Part *p = gBuffer;
        i = 0;
        do {
            int a;
            int rr;
            a = Random() & 0xffff;
            rr = (Random() & 0x3f) + 0x20;
            p->x = sin(a) * rr;
            p->y = 0xffce0000;
            p->z = cos(a) * rr;
            p->vy = ((Random() & 0x1f) + 0x20) << 13;
            p->t = 0;
            i++;
            p++;
        } while (i != 0x200);
        nframes = ((*(State **)(base + 0x7828))->f14 << 3) + 0x58;
    } else if (variant == 3) {
        Part *p = gBuffer;
        i = 0;
        do {
            p->x = ((Random() & 0xff) - 0x7f) << 15;
            p->y = ((Random() & 0xff) - 0x7f) << 14;
            p->z = ((Random() & 0xff) - 0x7f) << 15;
            p->t = 0;
            i++;
            p++;
        } while (i != 0x200);
        nframes = ((*(State **)(base + 0x7828))->f14 << 3) + 0x48;
    } else {
        Part *p = gBuffer;
        i = 0;
        do {
            p->x = ((Random() & 0xff) - 0x7f) << 15;
            p->y = ((Random() & 0xff) - 0x7f) << 15;
            p->z = ((Random() & 0xff) - 0x7f) << 15;
            p->t = 0;
            i++;
            p++;
        } while (i != 0x200);
        nframes = ((*(State **)(base + 0x7828))->f14 << 3) + 0x48;
    }
    cnt = 0x40;
    if ((*(State **)(base + 0x7828))->f18 == 0)
        cnt = 0x20;
    else if ((*(State **)(base + 0x7828))->f18 == 2)
        cnt = 0x80;
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    {
        int arg;
        arg = 0x90;
        arg <<= 3;
        StartTask(Task_BlitAnim, arg);
    }
    frame = 0;
    if (nframes != 0) {
        ang = 0;
        do {
            look = iwram_3001e80;
            if (frame == 0x28)
                _Func_80bd7dc(0);
            if ((*(State **)(base + 0x7828))->f1c == 1) {
                int x;
                int y;
                void *src;

                if (mode == 0) {
                    x = ((sin(ang) * 20) >> 16) + p58 + dx - 0x14;
                    y = ((cos(ang) * 4) >> 16) + p54 - 0x18;
                    if (frame > 0x20)
                        y = y - frame * 2 + 0x40;
                    src = base + (0x80 << 5);
                    fns[0](ctx, src, x, y, 0x28, 0x28);
                    if (frame <= 3)
                        fp[1](ctx, src, x, y, 0x28, 0x28);
                } else {
                    x = ((sin(ang) * 10) >> 16) + p58 / 2 - 0xa;
                    y = ((cos(ang) * 4) >> 16) + p54 - 0x18;
                    if (frame > 0x20)
                        y = y - frame * 2 + 0x40;
                    src = base + (0x80 << 5);
                    fns[0](ctx, src, x, y, 0x14, 0x28);
                    if (frame <= 3)
                        fp[1](ctx, src, x, y, 0x14, 0x28);
                }
            }
            b = 0;
            if ((*(State **)(base + 0x7828))->f14 != 0) {
                look2 = (char *)look + 0xc;
                tvp = &tv;
                yaw = frame << 9;
                io = 0x24;
                poff = 0;
                do {
                    int *a;
                    int lim;

                    a = (int *)*(int **)_GetBattleActor(
                            *(short *)((char *)*(State **)(base + 0x7828) + io));
                    lim = b << 3;
                    InitMatrixStack();
                    MatrixSetLook(look, look2);
                    tvp->x = a[2];
                    tvp->y = 0xa0 << 14;
                    tvp->z = a[4];
                    MatrixTranslatev(tvp);
                    if (frame == lim + 0x14)
                        _PlaySound(0x7e);
                    if (frame == lim + 0x24)
                        Func_80d6888(*(short *)((char *)*(State **)(base + 0x7828) + io),
                                     7, -1, b, 0x1c);
                    if (frame > lim) {
                        if (variant == 0 || variant == 6) {
                            MatrixYaw(yaw);
                        } else if (variant == 1) {
                            int r = frame << 9;
                            MatrixPitch(r);
                            MatrixRoll(r);
                        } else if (variant == 2) {
                            MatrixYaw((frame - b * 40) << 9);
                        } else if (variant == 3) {
                            MatrixYaw(yaw);
                        } else {
                            MatrixYaw(yaw);
                            MatrixPitch(yaw);
                        }
                        i = 0;
                        if (cnt != 0) {
                            Part *p;
                            vmode = variant - 3;
                            p = (Part *)((int *)gBuffer + poff);
                            do {
                                int hi;
                                int lo;

                                if ((unsigned int)vmode <= 2)
                                    hi = i / 2 + lim + 0x20;
                                else
                                    hi = 0x80 << 9;
                                lo = i / 4 + lim;
                                if (frame > lo && frame < hi) {
                                    int d;
                                    MagFn mag;
                                    mag = Func_8000948;
                                    d = mag((p->x >> 8) * (p->x >> 8)
                                            + (p->y >> 8) * (p->y >> 8)
                                            + (p->z >> 8) * (p->z >> 8)) >> 9;
                                    if (d != 0) {
                                        vec3_t *q = &pv;
                                        int u;
                                        int t;

                                        Func_80e3944((vec3_t *)p, q);
                                        if (variant == 0)
                                            q->x = q->x + dx;
                                        else
                                            q->x = q->x >> 1;
                                        q->y = q->y + 0x10;
                                        if (q->z <= 0x139)
                                            q->z = 0x9d << 1;
                                        if (q->z > 0x27a)
                                            q->z = 0x27a;
                                        t = q->z - 0x13a;
                                        if (t < 0)
                                            t = q->z - 0xbb;
                                        u = 3 - (t >> 7);
                                        if (variant == 0)
                                            u = (i * 4 + frame) % 9;
                                        if (variant == 0 || variant == 3
                                            || variant == 4 || variant == 5) {
                                            unsigned char *s = base + Data_ede84[u];
                                            unsigned char w = Data_ede96[u];
                                            fp[1](ctx, s, q->x - (w >> 1),
                                                  q->y - (w >> 1), w, w);
                                        } else {
                                            fp[1](ctx,
                                                  (char *)g2 + Data_ede48[u - 1],
                                                  q->x - u / 2, q->y - u,
                                                  u, u * 2);
                                        }
                                        if (variant == 0 || variant == 1 || variant == 2 || variant == 6) {
                                            p->x -= p->x / d;
                                            p->y -= p->y / d;
                                            p->z -= p->z / d;
                                        }
                                    }
                                }
                                i++;
                                p++;
                            } while (i != cnt);
                        }
                    }
                    yaw -= 0x1000;
                    io += 2;
                    poff += 448;
                    b++;
                } while (b != (*(State **)(base + 0x7828))->f14);
            }
            Func_80cd52c();
            *(int *)(base + 0x7824) = 1;
            WaitFrames(1);
            ang += 0x800;
            frame++;
        } while (frame != nframes);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}

/* ActorCmd_Player (0x0800ebec) -- NON-MATCHING, 795 of 833 encodings differ (objcmp,
 * PRODUCTION FLAGS).  804 instructions; at this size objcmp's count SATURATES, so LEAD
 * WITH aligncmp.
 *
 * SIZE 1804 vs 1784 (20 SHORT), INSTRUCTIONS 833 vs 822 (11 SHORT) -- neither is exact,
 * so 795 carries no distance.  THE RANKING VIEW IS aligncmp: 523 of 833 ALIGNED-EQUAL
 * (62.8%), 403 differing/inserted/deleted in 130 HUNKS.
 * RELOCATIONS: 59 against 59 and THE SYMBOL SEQUENCE IS EXACT -- all 59 symbols in the
 * ROM's order, on the FIRST candidate and on every one since, so no call and no data
 * reference is missing, extra or misordered.  Only ONE of the 59 OFFSETS matches; every
 * other is displaced by the count gap below.  objcmp still prints `RELOCATIONS differ`
 * for that reason, and the two halves must be read separately.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/800ebec.c asm/rom_9000/rom_ebec_a.s \
 *       --func ActorCmd_Player
 *   docker run ... python3 tools/aligncmp.py src/non_matching/rom_9000/800ebec.c \
 *       asm/rom_9000/rom_ebec_a.s ActorCmd_Player
 *
 * ================= IT IS NOT STRUCTURALLY BLOCKED.  I MEASURED THAT. =================
 * Batch 307's brief was amended mid-flight to abandon this function as blocked by INLINE
 * `.call_via` sites, citing docs/elevation.md:1473 (".call_via IS AN INLINE VENEER gcc
 * NEVER EMITS -- A STRUCTURAL BLOCKER CLASS").  THAT SECTION IS RETRACTED AT
 * elevation.md:5482 ("RETRACTED: `.call_via rN` is a hard wall.  It is reachable, and 51
 * functions were written off"), which names Func_8097a10 as ELEVATED AND BYTE-EXACT on the
 * inline-asm helper -- and 1473 was never struck, so a reader arriving there first still
 * gets the write-off.  Three independent checks here:
 *   1. THE COMPILED OUTPUT OF THIS FILE CONTAINS `mov r12, pc / bx r4` TWICE, at the two
 *      offsets where the ROM has `.call_via r4`, and aligncmp scores both pairs
 *      ALIGNED-EQUAL.  gcc-2.96 did emit the veneer.
 *   2. The site count in the amendment was wrong: this function has TWO inline sites, not
 *      four.  Screen with
 *        awk -v f=NAME '$0 ~ ("\\.thumb_func_start "f"([^A-Za-z0-9_]|$)"){p=1} \
 *                       p&&/^\.func_end/{print "";p=0} p' <ref.s> | grep -cE '^\s*\.call_via'
 *      (a bare `grep -c func_start ... /,/func_end/` range in awk is a zsh word-splitting
 *      trap -- it silently reported 0 for every function).
 *   3. Its TWIN ActorCmd_Player_World, parked in batch 306 at 86.5% aligned, has the SAME
 *      TWO inline sites and the same helper.  If inline sites blocked a function that park
 *      would be void too.
 * > A correction that leaves the original claim standing has not landed (batch 302's rule,
 * > third instance).  elevation.md:1473 needs striking in place.
 *
 * SPLIT SHAPE (measured with tools/split_s.py --dry-run; NOT yet performed):
 *     asm/rom_9000/rom_ebec_a.s holds TWO functions, ActorCmd_Player FIRST.
 *       ->  rom_ebec_a_b.s   ActorCmd_Player        (882 lines)  <- the target
 *       ->  rom_ebec_a_c.s   ActorCmd_Player_World  (603 lines)
 *     FINAL INSTALLED PATH (match): src/rom_9000/rom_ebec_a_b.c
 *     `make compare` must be green after the split and before any .c is written.
 *     NOTE -- THE TWIN'S PARK HAS THIS WRONG.  src/non_matching/rom_9000/800f2f8.c claims
 *     the split writes `_a` and `_b` and that ActorCmd_Player_World installs at
 *     src/rom_9000/rom_ebec_a_b.c.  split_s.py actually writes `_b` and `_c`, so
 *     rom_ebec_a_b.c is THIS function and the twin belongs at rom_ebec_a_c.c.  Fix that
 *     park's recipe before either is landed.
 * datacheck.py: nothing (no data section, so no code/data split).
 * `.global` REQUIREMENTS: NONE.  Both labels this file names -- .L13254 and .L13274 -- are
 *     defined AND already `.global` in asm/rom_9000/rom_ebec_c.s (lines 247/248, 252/254).
 *
 * SHIMS -- NOT pin-free.  shimcount reports 3 REGISTER PINS, all inside the `call_via`
 * helper (`_a` in r0, `_b` in r1, and `g` bound to r4).  This is the tree's ESTABLISHED
 * LANDING ROUTE for the inline veneer, not a verification device, and the r4 binding is
 * what produces the ROM's `bx r4`; the pointer is bound in an enclosing block and passed
 * unpinned, the documented high-register shape.  A fakematch.txt ROW IS DUE AT LANDING
 * (shimcount already flags "has a fakematch-class shim and NO fakematch.txt row").
 *
 * ================= LEVERS THAT PAID, IN ORDER, WITH FIGURES =================
 * First candidate: 799 of 833, size 1764 (40 short), count 812 (21 short), 59.5% aligned
 * in 145 hunks -- with the relocation symbol sequence ALREADY EXACT.
 *
 * 1. THE 0x68 FRAME IS THE TWIN'S DECLARATION BLOCK, AND IT TRANSFERRED VERBATIM.
 *    Referenced slots are 0, 4, 8, 0xc, 0x10, 0x14 (six ints), 0x18 (a short array),
 *    0x44 / 0x50 / 0x5c (three vec3_t) -- leaving 0x2c..0x43, TWENTY-FOUR BYTES, that
 *    nothing touches.  `vec3_t p,q,rr; vec3_t dead1,dead2; short dirs[10];` closes it
 *    exactly: p 0x5c, q 0x50, rr 0x44, dead1 0x38, dead2 0x2c, dirs 0x18, ending at the
 *    six scalar slots.  THE TWIN CARRIES THREE DEAD vec3_t AND A short[10]; THIS ONE
 *    CARRIES TWO AND THE SAME short[10], because its array sits 0xc higher.  Same block,
 *    two frames -- second confirmation, and confirmation of batch 306's rule that an
 *    unused AGGREGATE gets a slot and an unused SCALAR does not.
 *    The six scalars land in DECLARATION ORDER, first-declared highest: flags 0x14,
 *    hit 0x10, heading 0xc, anim 8, angx 4, ang2 0.  Reload's alter_reg walks pseudos in
 *    ascending REGNO and expand_decl creates one pseudo per decl in declaration order, so
 *    declaration order IS slot order and it is the only handle on it.  Every
 *    `str rX, [sp, #K]` is mis-attributed until this is right.
 *
 * 2. ONE VARIABLE PER REGION, ON THE ANGLE TEMPORARY -- worth 59.5% -> 62.3% aligned and
 *    it is what MOVED `angx` OFF ITS REGISTER AND ONTO THE STACK.  Written as one `u`
 *    reused by the probe chain, the dirs-building block, the dirs loop and the tail, `u`
 *    became one pseudo with a whole-function live range; it took a slot, `angx` kept r8,
 *    and the frame came out 0x6c (SEVEN spill slots) with a 7th slot spent on a
 *    `str r4 / ldr r4` pair around vec3_translate because `-fcall-used-r4` makes r4
 *    caller-saved.  Brace-scoping a fresh temporary per region deleted the spill, freed
 *    the register pressure, and let angx lose r8 to the entity-loop pointer -- which is
 *    the ROM's allocation.  Batch 306's complement lever, third instance.
 *
 * 3. THE NAMED LOOP BOUND, worth 62.3% -> 62.8% AND 143 -> 130 HUNKS AND THE LAST SPILL
 *    SLOT.  `while (i < 6)` folds to `cmp #5 / ble`; `n = 6; ... while (i < n)` gives the
 *    ROM's `cmp r0, #6 / blt`.  Third confirmation of the cmp #K/blt retraction, on a loop
 *    back-edge (the twin has the second).  Its side effect is the bigger half: the extra
 *    allocno pushed the frame from 0x64 to 0x68, i.e. TO THE ROM'S SIZE, with all six
 *    slots in the ROM's order.
 *
 * 4. THE POINTER POST-INCREMENT IN THE COUNT LOOP.  The ROM has
 *    `ldrb r3,[r1] / add r1,#1 / cmp r3,#0xff / bne / add r0,#1` -- the pointer steps
 *    IMMEDIATELY after the load and the counter increment is the if-body.  `if (*tab == 0xff)
 *    cnt++; tab++;` puts the step at the loop bottom beside the count decrement;
 *    `if (*tab++ == 0xff) cnt++;` is the ROM.
 *
 * 5. THE gState OFFSET AS A NAMED LOCAL, twice (`koff = 0x87 << 2` and `koff = 0xfa << 1`),
 *    giving `ldr r3,=gState / mov r1,#K / lsl r1,#n / add r3,r1` where the bare offset
 *    folds into one pool word.  Batch 303's lever, fifth and sixth confirmations.
 *
 * ================= THE RESIDUE, ATTRIBUTED =================
 * The 11 missing instructions and most of the 130 hunks are ONE MECHANISM.
 *
 * (A) gcse COMMONS FOUR RECOMPUTATIONS THE ROM KEEPS -- 4 of the 11 directly, and the pool
 *     and branch displacements that follow.  The ROM computes `(unsigned)angx >> 16` FIVE
 *     SEPARATE TIMES -- `lsr r1,r3,#16` at 0x800ecec (compare + first vec3_translate arg),
 *     `lsr r6,r3,#16` at 0x800ed48 (the probe chain, five uses), `lsr r3,r0,#16` at
 *     0x800ee14 (building dirs[]), `lsr r3,r0,#16` at 0x800f14c (the facing clamp) and
 *     `lsr r2,#16` at 0x800f1fa -- each from a fresh reload of sp+4, into a different
 *     register.  Nine `lsr #16` in the ROM against five in this candidate.  gcse's PRE
 *     finds the expression available (the first block dominates all four) and deletes
 *     them, which is also why the value has to live in a register across the calls.
 *     RULED OUT, MEASURED:
 *       - `-fno-gcse` IS NOT THE ANSWER and is not a flag-row candidate.  It restores 3 of
 *         the 4 (5 -> 8 `lsr`) and costs more than it buys: 841 lines -> 822 against the
 *         ROM's 853, i.e. further from the count, not closer.
 *       - FIVE MUTUALLY DISTINCT SPELLINGS chosen to differ at gcse time and converge in
 *         combine -- `(unsigned)angx>>16`, `(unsigned short)(angx>>16)`,
 *         `(unsigned)(angx & 0xffff0000)>>16`, `(unsigned short)((unsigned)angx>>16)`,
 *         `((unsigned)angx>>8)>>8` -- recover only ONE (5 -> 6 `lsr`) and read WORSE:
 *         62.8% in 144 hunks against 130, objcmp 813 against 795.  Combine collapses three
 *         of the four back to the same pseudo before allocation.  (File scratch_elev p5.)
 *       - A SINGLE ALTERNATIVE SPELLING SHARED BY ALL FOUR SITES is the trap: they then
 *         common WITH EACH OTHER instead, which frees angx back into r8 and drops the
 *         frame to 0x64 -- 62.3% in 143 hunks.  (File scratch_elev p3.)
 *       - `volatile int angx` -- the memory-resident reading, and the one I expected to
 *         pay, because a value gcc must reload at every use cannot be commoned across.
 *         MEASURED AND REJECTED: size 1788 (the CLOSEST of any candidate, 16 short) but
 *         count 824 and 62.5% in 150 HUNKS against p4's 130.  A closer size on a worse
 *         program, the documented trap: it buys the reloads and pays for them with extra
 *         stores at every set plus a different slot region.  (File scratch_elev p6.)
 *     SO ALL FOUR READINGS OF (A) ARE NOW ELIMINATED and the class is open.  What has NOT
 *     been tried is making the four sites read a value with MORE THAN ONE SET -- gcse's
 *     oprs_available_p kills an expression whose operand is set between, so a second
 *     assignment to angx on a path that reaches the later sites should defeat PRE without
 *     paying volatile's store cost.  That is the next probe, and it is the same
 *     more-than-one-set mechanism the twin used to keep a narrowing alive (its lever 4),
 *     pointed at availability instead of at combine.
 *
 * (B) TWO SCHEDULE TRANSPOSITIONS inside the probe blocks, each `str r3,[r5,#8]` against
 *     the constant build that feeds the next argument, count local to the hunk already
 *     right.  Not sched1 -- `flag_schedule_insns` is off at -O2 in this build and no
 *     `-fno-schedule-insns` result says anything.  sched2's tie-break is priority ->
 *     dependent count -> INSN_LUID, and the competing insns here are an independent store
 *     against an argument-register chain, so only the dependent-count term is in play.
 *
 * (C) POOL OFFSETS AND BRANCH DISPLACEMENTS, the bulk of the hunk count, every one an
 *     `ldr [pc,#N]` or `b.n` off by halves and carrying no information of its own.  They
 *     close when (A) closes.
 *
 * NEGATIVES WORTH THE SPACE:
 *   - `*(unsigned char *)&flags` is WRONG here exactly as in the twin.  The ROM's
 *     `add r0,sp,#0x14 / ldrb r0,[r0]` at the CreateActor block and the `ldrh r2,[sp,#0x14]`
 *     at 0x800f288 are PLAIN NARROWING STORES through the spilled `flags` pseudo -- reload
 *     turns (subreg:QI/HI (reg:SI flags)) into a mem and then has to build the address,
 *     because Thumb has no `ldrb rX,[sp,#imm]`.  Taking the address pins the slot to 0 and
 *     inverts the whole scalar order.  Second confirmation.
 *   - THE HImode LOCAL SET TO ZERO IS LOAD-BEARING AND IT IS A POOL WORD.  `w = 0` on the
 *     `unsigned short w` at 0x800f290 is the ROM's `ldr r2, .Lf2b0  @ 0` -- *thumb_movhi_insn
 *     has no immediate alternative so movhi force_const_mem's it -- and `w = 1` at
 *     0x800f2c6 is the ROM's `ldr r2, =1`.  Spelling either as an int literal deletes the
 *     pool word, which is a size-and-count defect.  Conversely every HImode literal STORE
 *     (`*tmr = 0`, `= 2`, `= 0xc`, `= 0x12`) is `mov rX,#K / strh` in the ROM and needs an
 *     INT local (`va = 0xc; *tmr = va;`) -- the twin's lever 5, four sites here.
 *   - THE TIMER HALFWORD IS READ BEFORE ITS OWN TEST (`e = *tmr;` outside the `if`), which
 *     is what gives the ROM's non-destructive `sub r3, r2, #1`.  Twin's lever 6.
 *   - THE DOUBLE `TestCollision(ent, &q)` WITH NO INTERVENING SETUP at 0x800f052 is REAL
 *     and must be written twice.  gcc does not CSE a call and does not cross-jump two
 *     sequential blocks with the same successor, so it survives; it reads like an original
 *     copy-paste slip and removing it costs an encoding.
 *   - Func_800eba0 takes FOUR arguments and r1 is `a->0x20 - 2` computed at the TOP of the
 *     entity-loop body, before the three early-continue tests, so it is its own statement
 *     there; r3 is the symmetric `pos->0x18 - 2`.  Calls in the loop keep the load out of
 *     loop.c's hands, so it is NOT hoisted and must not be.
 */
#include "gba/types.h"

extern unsigned char gState[];
extern volatile unsigned int gKeyHeld;
extern unsigned char gDebugMode;
extern unsigned char gSpriteAllocTable[];
extern unsigned char *iwram_3001e64;
extern unsigned char *iwram_3001ebc;
extern unsigned char *iwram_3001e70;

extern short L13254[] __asm__(".L13254");
extern unsigned char L13274[] __asm__(".L13274");

extern int _GetFlag(int id);
extern void _PlaySound(int id);
extern unsigned char *_GetUnit(int id);
extern void vec3_translate(int mag, int ang, vec3_t *v);
extern int TestCollision(void *a, vec3_t *v);
extern int Func_800eba0(vec3_t *p, int ha, vec3_t *t, int hb);
extern int Func_800d924(void *e, vec3_t *v);
extern int atan2(int dz, int dx);
extern void Actor_TravelTo(void *a, int x, int y, int z);
extern void Actor_SetAnim(void *a, int anim);
extern int FastIntSqrtFP1616_RAM(int x);
extern void *CreateActor(int kind, int x, int y, int z);
extern void Actor_SetScript(void *a, void *script);
extern void Sprite_SetAnim(void *s, int anim);
extern void Func_800eaf8(void);
extern fx32 Func_8000888(fx32, fx32);

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
        : "memory", "r12"
    );
    return _a;
}

int ActorCmd_Player(void *r0)
{
    unsigned char *a;
    vec3_t p;
    vec3_t q;
    vec3_t rr;
    vec3_t dead1;
    vec3_t dead2;
    short dirs[10];
    int flags;
    int hit;
    int heading;
    int anim;
    int angx;
    int ang2;

    int t;
    int koff;
    int n;
    int i;
    unsigned int u;
    unsigned int v;
    int cnt;
    int mask;
    int d;
    int e;
    int va;
    int ang;
    unsigned int u3;
    unsigned int ut;
    int ha;
    unsigned short *tmr;
    unsigned short *fsel;
    unsigned short w;
    unsigned char *cam;
    unsigned char *spr;
    unsigned char *act;
    unsigned char *ent;
    unsigned char *tab;
    vec3_t *pos;
    int mx;
    int mz;
    int mag;

    a = r0;
    flags = 0;
    hit = 0;
    if (gDebugMode && _GetFlag(0xaf << 1)) {
        cnt = 0;
        n = 0x80 << 2;
        tab = gSpriteAllocTable;
        do {
            if (*tab++ == 0xff)
                cnt++;
            n--;
        } while (n != 0);
        if (cnt - 0x88 < 0)
            _PlaySound(0x87);
    }
    if (gDebugMode) {
        mask = 0x80 << 2;
        if (gKeyHeld & mask) {
            n = mask;
            do {
                n--;
            } while (n != 0);
            n = 0x5f;
            do {
                n--;
            } while (n >= 0);
            n = 0x3f;
            do {
                n--;
            } while (n >= 0);
            n = 0x3f;
            do {
                n--;
            } while (n >= 0);
        }
    }
    koff = 0x87 << 2;
    if (gKeyHeld & *(unsigned short *)(gState + koff)) {
        *(int *)(a + 0x30) = 0xc0 << 9;
        *(int *)(a + 0x34) = 0x80 << 7;
        anim = 5;
    } else {
        *(int *)(a + 0x30) = 0x80 << 9;
        *(int *)(a + 0x34) = 0x80 << 7;
        anim = 2;
    }
    if (_GetFlag(0x17f) && (gKeyHeld & 2)) {
        *(int *)(a + 0x30) = 0x80 << 11;
        *(int *)(a + 0x34) = 0x80 << 9;
        anim = 5;
    }
    t = L13254[(gKeyHeld >> 4) & 0xf];
    angx = t << 16;
    u = (unsigned int)angx >> 16;
    if (u == 0xffff) {
        flags |= 4;
    } else {
        flags = 0;
        p.x = *(int *)(a + 8);
        p.y = *(int *)(a + 0xc);
        p.z = *(int *)(a + 0x10);
        vec3_translate(0x80 << 12, u, &p);
        if (gDebugMode) {
            heading = angx >> 16;
            if (gKeyHeld & (0x80 << 2))
                goto done;
        }
        if (!TestCollision(a, &p)) {
            v = (unsigned int)angx >> 16;
            va = 0x80 << 12;
            q.x = *(int *)(a + 8);
            q.y = *(int *)(a + 0xc);
            q.z = *(int *)(a + 0x10);
            vec3_translate(va, v + (0x80 << 5), &q);
            if (!TestCollision(a, &q)) {
                q.x = *(int *)(a + 8);
                q.y = *(int *)(a + 0xc);
                q.z = *(int *)(a + 0x10);
                vec3_translate(va, v - 0x1000, &q);
                if (!TestCollision(a, &q)) {
                    q.x = *(int *)(a + 8);
                    q.y = *(int *)(a + 0xc);
                    q.z = *(int *)(a + 0x10);
                    vec3_translate(va, v + (0x80 << 6), &q);
                    if (!TestCollision(a, &q)) {
                        q.x = *(int *)(a + 8);
                        q.y = *(int *)(a + 0xc);
                        q.z = *(int *)(a + 0x10);
                        vec3_translate(va, v - 0x2000, &q);
                        if (!TestCollision(a, &q)) {
                            heading = angx >> 16;
                            ang2 = heading << 16;
                            goto move;
                        }
                    }
                }
            }
        }
        {
            unsigned int b = (unsigned int)angx >> 16;
            dirs[0] = b + (0x80 << 5);
            dirs[1] = b - 0x1000;
            dirs[2] = b + (0x80 << 6);
            dirs[3] = b - 0x2000;
            dirs[4] = b + (0xc0 << 6);
            dirs[5] = b - 0x3000;
        }
        n = 6;
        i = 0;
        do {
            unsigned int z;
            heading = dirs[i];
            p.x = *(int *)(a + 8);
            p.y = *(int *)(a + 0xc);
            p.z = *(int *)(a + 0x10);
            ang2 = heading << 16;
            z = (unsigned int)ang2 >> 16;
            vec3_translate(0x80 << 12, z, &p);
            if (!TestCollision(a, &p)) {
                q.x = *(int *)(a + 8);
                q.y = *(int *)(a + 0xc);
                q.z = *(int *)(a + 0x10);
                vec3_translate(0x80 << 12, z + (0x80 << 5), &q);
                if (!TestCollision(a, &q)) {
                    q.x = *(int *)(a + 8);
                    q.y = *(int *)(a + 0xc);
                    q.z = *(int *)(a + 0x10);
                    vec3_translate(0x80 << 12, z - 0x1000, &q);
                    if (!TestCollision(a, &q)) {
                        q.x = *(int *)(a + 8);
                        q.y = *(int *)(a + 0xc);
                        q.z = *(int *)(a + 0x10);
                        vec3_translate(0x80 << 12, z + (0x80 << 6), &q);
                        if (!TestCollision(a, &q)) {
                            q.x = *(int *)(a + 8);
                            q.y = *(int *)(a + 0xc);
                            q.z = *(int *)(a + 0x10);
                            vec3_translate(0x80 << 12, z - 0x2000, &q);
                            if (!TestCollision(a, &q))
                                goto move;
                        }
                    }
                }
            }
            i++;
        } while (i < n);
        p.x = *(int *)(a + 8);
        p.y = *(int *)(a + 0xc);
        p.z = *(int *)(a + 0x10);
        flags |= 1;
move:
        rr.x = *(int *)(a + 8);
        rr.y = *(int *)(a + 0xc);
        rr.z = *(int *)(a + 0x10);
        vec3_translate(0x80 << 11, (unsigned int)ang2 >> 16, &rr);
        ent = iwram_3001e64;
        pos = (vec3_t *)(ent + 8);
        i = 0x3f;
        do {
            ha = *(unsigned short *)(a + 0x20) - 2;
            if (*(int *)ent == 0)
                goto next;
            if ((*(unsigned char *)(ent + 0x59) & 1) == 0)
                goto next;
            if (ent == a)
                goto next;
            if (Func_800eba0(pos, ha, &rr,
                             *(unsigned short *)((char *)pos + 0x18) - 2) < 0)
                goto next;
            if ((*(int *)((char *)pos + 0x50) & 0xff000200) != (0x80 << 2))
                goto blocked;
            ang = atan2(pos->z - *(int *)(a + 0x10), pos->x - *(int *)(a + 8));
            q.x = pos->x;
            q.y = pos->y;
            heading = (short)ang;
            u3 = (unsigned short)ang;
            q.z = pos->z;
            vec3_translate(0x80 << 7, u3, &q);
            if (Func_800d924(ent, &q))
                goto blocked;
            q.x = pos->x;
            q.y = pos->y;
            q.z = pos->z;
            vec3_translate(0xa0 << 12, u3, &q);
            if (TestCollision(ent, &q))
                goto blocked;
            q.x = pos->x;
            q.y = pos->y;
            q.z = pos->z;
            vec3_translate(0xa0 << 12, u3 + (0x80 << 5), &q);
            if (TestCollision(ent, &q))
                goto blocked;
            if (TestCollision(ent, &q))
                goto blocked;
            q.x = pos->x;
            q.y = pos->y;
            q.z = pos->z;
            vec3_translate(0xa0 << 12, u3 - 0x1000, &q);
            if (TestCollision(ent, &q))
                goto blocked;
            vec3_translate(0x80 << 7, u3, pos);
            *(int *)((char *)pos + 0x30) = 0x80 << 24;
            *(int *)((char *)pos + 0x34) = 0x80 << 24;
            *(int *)((char *)pos + 0x38) = 0x80 << 24;
            hit |= 1;
            goto next;
blocked:
            flags |= 2;
next:
            ent += 0x70;
            pos = (vec3_t *)((char *)pos + 0x70);
            i--;
        } while (i >= 0);
        if (flags == 0 && hit != 0) {
            *(int *)(a + 0x30) = 0x80 << 7;
            *(int *)(a + 0x34) = 0x80 << 6;
        }
    }
done:
    cam = iwram_3001ebc;
    if (cam) {
        if (flags & 3)
            *(unsigned short *)(cam + (0xce << 1)) += 1;
        else
            *(unsigned short *)(cam + (0xce << 1)) = 0;
    }
    if (hit) {
        Actor_SetAnim(a, 8);
    } else if (flags) {
        koff = 0xfa << 1;
        n = 9;
        if (*(short *)(_GetUnit(*(int *)(gState + koff)) + 0x38) == 0)
            n = 0x16;
        Actor_SetAnim(a, n);
    } else {
        Actor_SetAnim(a, anim);
    }
    if (flags) {
        *(int *)(a + 0x38) = 0x80 << 24;
        *(int *)(a + 0x3c) = 0x80 << 24;
        *(int *)(a + 0x40) = 0x80 << 24;
        *(int *)(a + 0x24) = 0;
        *(int *)(a + 0x2c) = 0;
        if (flags & 3) {
            d = (short)(((unsigned int)angx >> 16) - *(unsigned short *)(a + 6));
            if (d > (0x80 << 5))
                d = 0x80 << 5;
            if (d < -0x1000)
                d = -0x1000;
            *(unsigned short *)(a + 6) += d;
        }
        tmr = (unsigned short *)(a + 0x64);
        va = 0;
        *tmr = va;
        va = 2;
        *(unsigned short *)(a + 0x66) = va;
    } else {
        Actor_TravelTo(a, p.x, p.y, p.z);
        {
            register int (*g)(int, int) __asm__("r4") = Func_8000888;
            mx = call_via(g, *(int *)(a + 0x24), *(int *)(a + 0x24));
            mz = call_via(g, *(int *)(a + 0x2c), *(int *)(a + 0x2c));
        }
        mag = FastIntSqrtFP1616_RAM(mx + mz);
        *(int *)(a + 0x24) = flags;
        *(int *)(a + 0x2c) = flags;
        vec3_translate(mag, (unsigned short)heading, (vec3_t *)(a + 0x24));
        tmr = (unsigned short *)(a + 0x64);
        e = *tmr;
        if (*(short *)tmr) {
            va = e - 1;
            *tmr = va;
        }
    }
    ut = (unsigned int)angx >> 16;
    if (iwram_3001e70[0x17] && *(short *)tmr == 0 && flags == 0) {
        act = CreateActor(0x19, *(int *)(a + 8), *(int *)(a + 0xc),
                          *(int *)(a + 0x10));
        if (act) {
            *(int *)(act + 0x14) = *(int *)(a + 0x14);
            spr = *(unsigned char **)(act + 0x50);
            Actor_SetScript(act, L13274);
            *(unsigned char *)(act + 0x23) = 2;
            *(unsigned char *)(act + 0x55) = flags;
            if (spr) {
                Sprite_SetAnim(spr, 1);
                *(unsigned char *)(spr + 0x26) = flags;
                *(unsigned short *)(spr + 0x1e) = (0x80 << 7) + ut;
                *(unsigned char *)(spr + 9) |= 0xc;
            }
            fsel = (unsigned short *)(a + 0x66);
            w = *fsel;
            if (*(short *)fsel == 2) {
                Sprite_SetAnim(spr, 2);
                *fsel = flags;
                w = 0;
            }
            if (w)
                *(unsigned short *)(act + 6) = 0x80 << 8;
            if (anim == 5)
                *tmr = 0xc;
            else
                *tmr = 0x12;
            w = 1;
            *fsel ^= w;
        }
    }
    Func_800eaf8();
    *(unsigned short *)(a + 4) += 1;
    return 1;
}

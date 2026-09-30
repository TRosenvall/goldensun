/* ActorCmd_Player_World (0x0800f2f8) -- NON-MATCHING, 313 of 578 encodings differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_9000/800f2f8.c asm/rom_9000/rom_ebec_a.s \
 *       --func ActorCmd_Player_World
 *
 * SIZE 1248 vs 1252 (4 SHORT), INSTRUCTIONS 577 vs 578 (1 SHORT), so 313 SATURATES and
 * cannot rank.  THE RANKING VIEW IS aligncmp: 500 of 578 ALIGNED-EQUAL (86.5%), 94
 * differing/inserted/deleted in 46 HUNKS.  All 41 relocations are present with the same
 * symbols; only the POSITION of one five-word pool block differs (see "the pool" below).
 *   docker run ... python3 tools/aligncmp.py src/non_matching/rom_9000/800f2f8.c \
 *       asm/rom_9000/rom_ebec_a.s ActorCmd_Player_World -v
 *
 * A SIZE-EXACT SIBLING CANDIDATE EXISTS: scratch_elev/b306b/c8.c measures 315 of 578 with
 * SIZE EXACT (1252 == 1252) and count 579 vs 578, aligncmp 497 (86.0%) / 105 in 45 hunks.
 * It differs from this file in ONE place -- it writes literal `1` where this file writes
 * `st` in the two Func_8012204 arms.  Keep both in view: c8 is the one to reach for if a
 * later lever needs size held fixed.
 *
 * SPLIT SHAPE (measured with tools/split_s.py --dry-run; NOT yet performed):
 *     asm/rom_9000/rom_ebec_a.s holds TWO functions
 *       ->  rom_ebec_a_a.s   ActorCmd_Player         (882 lines)
 *       ->  rom_ebec_a_b.s   ActorCmd_Player_World   (603 lines)  <- the target
 *     `make compare` must be green after the split and before any .c is written.
 *     FINAL INSTALLED PATH (match): src/rom_9000/rom_ebec_a_b.c
 * datacheck.py: nothing.  No label needs `.global`: .L13254, .L13280 and .L1328c are all
 *     defined AND already `.global` in asm/rom_9000/rom_ebec_c.s.
 *
 * SHIMS -- NOT pin-free.  shimcount reports 3 REGISTER PINS, all of them inside the
 * `call_via` helper (`_a` in r0, `_b` in r1, and `g` bound to r4).  These are NOT
 * speculative: the ROM calls Func_8000888 through `.call_via r4`, which gcc-2.96 cannot
 * emit from any C spelling -- its only indirect-call pattern is `bl _call_via_rN`.  This
 * is the tree's own established landing route (elevation.md "RETRACTED: `.call_via rN` is
 * a hard wall"; Func_8097a10 is ELEVATED and byte-exact on it), so the helper is a
 * LANDING ROUTE here, not a verification device -- but it still needs a fakematch.txt row
 * on landing, and the r4 binding is what produced the ROM's `bx r4` (unbound it is
 * `bx r2`).  Per elevation.md the two-site case normally wants the callee passed
 * unpinned; here the pointer is bound in an ENCLOSING BLOCK and passed unpinned, which is
 * the documented high-register shape applied to r4, and it gives the ROM's single
 * `ldr r4, =Func_8000888`.
 *
 * ================= WHAT GOT THIS FROM 569 OF 569 TO 94 IN 46 HUNKS =================
 * First candidate: 569 of 569, size 1244 vs 1252.  The RELOCATION SYMBOL SEQUENCE was
 * already the ROM's on that first candidate (bar the `_call_via_r6` veneer a plain
 * function-pointer call produced), which is what said the program shape was right before
 * anything was spent on allocation.  Then, with figures:
 *
 * 1. THE FRAME IS NINE STACK OBJECTS AND THE ROM'S 0x5c FRAME NAMES ALL OF THEM.  This is
 *    the single largest result and it is transferable to the twin.  The referenced slots
 *    are 0, 4, 8 (three ints), 0xc (a short array) and 0x44 / 0x50 (two vec3_t), leaving
 *    a FORTY-BYTE HOLE at 0x1c..0x43 that nothing in the function touches.  The hole is
 *    not padding and not spill space -- it is DECLARED LOCALS THIS FUNCTION NEVER USES.
 *    The reading that fits is `short dirs[10]` plus THREE MORE vec3_t, and it is
 *    CONFIRMED BY THE TWIN: ActorCmd_Player at 0x0800ebec has a 0x68 frame with the array
 *    at 0x18 and vec3_t at 0x44 / 0x50 / 0x5c, and the SAME declaration block
 *    (6 ints, short[10], five vec3_t) closes its gap exactly too -- 0x18+20 = 0x2c, then
 *    0x2c/0x38/0x44/0x50/0x5c, ending at 0x68.  Two independent frames, one declaration
 *    block.  gcc-2.96 DOES allocate a stack slot for an unused local AGGREGATE (the three
 *    dead vec3_t appear in the frame); it does NOT for an unused scalar, which is how the
 *    first attempt at the hole (`int spare`) came back 4 bytes short and was replaced by
 *    widening the array to `short dirs[10]`.
 *    The ALLOCATION ORDER is reverse-declaration within the aggregate region and the
 *    scalars sit BELOW it, so `vec3_t p; vec3_t q; vec3_t r2,r3,r4; short dirs[10]; int
 *    flags; int heading; int anim;` is what puts p at 0x50 and anim at 0x0.  Getting this
 *    right closed `sub sp, #0x5c` and every `add rX, sp, #K`: 569 -> 565 on its own, and
 *    it is a PRECONDITION for everything after it, because a wrong slot number
 *    mis-attributes every `str rX, [sp, #K]`.
 *
 * 2. `*(unsigned char *)&flags` WAS WRONG AND IT MOVED THE SLOT, NOT JUST AN INSTRUCTION.
 *    The ROM reads flags as a byte with `add r0, sp, #8 / ldrb r0, [r0]`, which looks like
 *    an address-taken local (Thumb has no `ldrb rX, [sp, #imm]`, so a byte read of a stack
 *    slot MUST compute the address).  It is not.  It is a PLAIN NARROWING STORE
 *    `*(unsigned char *)(act + 0x55) = flags;` through a SPILLED pseudo: reload turns
 *    `(subreg:QI (reg:SI flags))` into `(mem:QI (sp+8))` and then has to build the
 *    address.  Taking the address for real makes the variable addressable at expand time
 *    and PINS ITS SLOT TO 0, which pushed anim and heading up to 4 and 8 and inverted the
 *    whole scalar triple against the ROM.  Dropping the `&` plus the two levers below:
 *    565 -> 322.
 *    > A `ldrb`/`strb` through a computed stack address is NOT evidence of `&local`.
 *
 * 3. THE LONG-LIVED VALUE IS THE SHIFTED ANGLE, NOT THE ANGLE -- and it is worth 2 of the
 *    3 instructions the first size-exact candidate was missing.  r11 holds `raw << 16`
 *    and the ROM derives BOTH `(unsigned short)` (lsr, for the vec3_translate argument)
 *    and `(short)` (asr, for the stored heading) FROM IT.  `lsl r2, #16` is DESTRUCTIVE on
 *    the `ldrsh` result, which is the tell: the raw value is dead the moment it is
 *    shifted.  So the C declares `int angx` and writes
 *        angx = dirs[i] << 16;  u = (unsigned int)angx >> 16;  ... heading = angx >> 16;
 *    A `short ang` carrier cannot reach it: gcc then KNOWS the value is sign-extended,
 *    folds `(short)ang` to a copy and elides the `asr` entirely.
 *    MEASURED ALONE THIS LEVER IS A LOSS -- 322 -> 358, count 583 vs 578.  It only pays
 *    in combination with lever 4 (322 -> 327 raw but 79.1% -> 85.6% aligned, 64 -> 49
 *    hunks, and SIZE became EXACT).  An interaction, not an additive pair; a probe that
 *    reads worse alone is not disproved.
 *
 * 4. A NARROWING SURVIVES ONLY IF ITS INTERMEDIATE HAS MORE THAN ONE SET -- the facing
 *    update.  The ROM has `lsls r3, r0, #16 / lsrs r3, r3, #16 / subs r3, r3, r2` for
 *    `(unsigned short)heading - *(unsigned short *)(a + 6)`; written that way in one
 *    expression, combine PROVES the zero-extension redundant (the enclosing `(short)`
 *    truncation makes it so) and emits a bare `subs r3, r0, r2`.  Routing it through `u`
 *    -- the ANGLE variable, which already has three sets elsewhere in the function --
 *    keeps it, because combine will not substitute through a pseudo with more than one
 *    set.  This is the reuse lever used for a NEW purpose: not to inherit a register, but
 *    to make a value un-foldable.  318 -> 337 raw, 79.1% -> 80.8% aligned on its own, and
 *    it is what makes lever 3 pay.
 *
 * 5. EVERY HImode LITERAL STORE HAD TO BECOME AN INT LOCAL.  `*tmr = 0`, `= 2`, `= 0xa`
 *    and `*tmr = *tmr - 1` all compile to `ldrh r3, .Ln` + a `.word`, because Thumb's
 *    `*thumb_movhi_insn` has no immediate alternative and `movhi` force_const_mem's a
 *    CONST_INT bound for memory (elevation.md "Halfword constant ZERO: the pooling class
 *    has a fix").  The ROM has `mov r3, #0` / `mov r3, #2` / `mov r3, #0xa` /
 *    `sub r3, r2, #1` -- SImode registers throughout.  `v = 0; *tmr = v;` fixes each one.
 *    FOUR such sites here, and they were also dragging an unwanted mid-function pool.
 *
 * 6. TWO IDENTIFICATIONS FROM ORDER ALONE, both read straight off the ROM:
 *    - THE MOTION PAIR IS ZEROED BEFORE vec3_translate, not after (`str r3, [r6, #0x24]`
 *      precedes the `bl`), and writing the stores as `*(int *)(a + 0x24) = flags`
 *      AFTER the call let gcc CSE `a + 0x24` with the argument and cost a `mov`.
 *    - THE TIMER HALFWORD IS READ BEFORE ITS OWN TEST: `ldrh r2, [r3]` precedes
 *      `ldrsh r3, [r3, r0] / cmp / beq`, so `w = *tmr;` is its own statement outside the
 *      `if`, which is what gives the ROM's NON-destructive `subs r3, r2, #1`.
 *
 * 7. gcc CROSS-JUMPED TWO ARMS THE ROM KEEPS DISTINCT, and the cause was a literal where
 *    the ROM has a variable.  Both arms of the `Func_8012204(...) == 9` test end in
 *    `strb rX, [spr + 0x26]`, so with `= 1` in one arm and `= 1` in the other gcc
 *    tail-merged them to a single store at a shared label and hoisted `mov r3, #0` /
 *    `mov r3, #1` into the arms.  The ROM stores `r7` -- the `*(a + 0x54)` variable, whose
 *    value IS 1 on this path -- so writing `st` instead of the literal makes the arms
 *    genuinely different, blocks the merge, and restores the ROM's per-arm shape.
 *    105 -> 94 differing.  It also needed `st` and the Func_8012204 RESULT to be TWO
 *    variables; reusing one destroys r7, which the ROM holds live across the call.
 *    > A literal equal to a variable's known value is not interchangeable with it:
 *    > the literal lets cross-jumping fire.
 *
 * ================= THE RESIDUE, AND WHERE IT IS ATTRIBUTED =================
 * The 94 fall into three groups, and NONE of them is sched1 -- `flag_schedule_insns` is
 * off at -O2 in this build, so no spelling and no `-fno-schedule-insns` result says
 * anything here.
 *
 * (A) POOL PLACEMENT -- the largest group, ~20 encodings, every one an `ldr rX, [pc, #N]`
 *     whose OFFSET differs while its relocation symbol matches.  The ROM dumps a
 *     five-word pool (Func_8000888, iwram_3001e70, gKeyHeld, .L1328c, 0xfffffe00)
 *     MID-FUNCTION at 0x42c; ours defers the whole block to the end.  THE CAUSE IS
 *     IDENTIFIED AND IT IS ONE MISSING POOL WORD: the ROM's arm-1 store
 *     `*(unsigned char *)(spr + 0x26) = 0` is a QImode literal that
 *     `force_const_mem`s (`ldr r3, .Lf720 / .word 0 / .pool`), and it is that short-range
 *     fixup that forces `arm_reorg` to dump the pending pool right there.  We emit
 *     `mov r3, #0 / strb` instead, so nothing forces the dump.  This accounts for the
 *     whole of the 4-byte size gap and the 1-instruction count gap as well (the `.word 0`
 *     plus its `.align 2, 0` padding, which objdump renders as an extra encoding).
 *     THIS IS THE NEXT THING TO ATTACK and it should close A, the size and the count
 *     together.  elevation.md's "A QImode literal store also goes to the POOL" says a
 *     plain `p[off] = 0;` always pools; here it did not, and the difference is that gcc
 *     had already split the store into `(set reg 0)` + `(set mem reg)` before the backend
 *     saw it.  Spellings NOT yet tried: a `const unsigned char` source, a separate
 *     `unsigned char *` pointer local for `spr + 0x26`, and reordering the two stores
 *     within arm 1.
 *
 * (B) TWO ALLOCATION ROTATIONS, both of them one quantity, both with the count local to
 *     the hunk already right:
 *     - the loop head.  ROM: `lsls r2, #16 / lsrs r7, r2, #16 / ... / mov fp, r2`.
 *       Ours copies to fp FIRST and then reads it back (`mov fp, r3 / ... / mov r1, fp /
 *       lsrs r7, r1, #16`) -- one extra `mov`, because `angx` was given r11 and reload
 *       emits the high-register copy at the definition.  Moving `u = ...` adjacent to
 *       `angx = ...` in the source does NOT change it (measured: identical).  This is
 *       reload's placement of a high-register copy, not an ordering choice.
 *     - the camera approach.  The ROM keeps the clamped delta in r2 and the `(short)`
 *       intermediate in r3 (the subtraction's own register, destructive `asrs r3, r3, #16`);
 *       ours has the intermediate in r1 and then shifts r1 destructively, so the roles
 *       are swapped.  `e = tgt - *cam; e = (short)e; d = e / 8;` (three statements, two
 *       variables) was the closest of what was tried and is what is in the file.
 *       NAMING THE DEREFERENCED GLOBAL (`ibase = iwram_3001e70;`) IS A NET GAIN BUT NOT
 *       FREE: it is what puts the iwram pointer load ahead of the gKeyHeld read as the ROM
 *       has it, and it is worth 117 -> 105, but it also pushes `cam` into r12 and costs a
 *       `mov ip, r2` / `mov r0, ip` pair.  Removing it measures WORSE (94 -> 117 on the
 *       c8 line), so it stays, and its r12 cost is a known, separately-priced defect.
 *
 * (C) BRANCH DISPLACEMENTS -- ~8 encodings, every one a `b.n` / `beq.n` / `blt.n` off by
 *     exactly one halfword, i.e. CONSEQUENCES of (A) and (B), carrying no information of
 *     their own.  They will move when the instruction count closes.
 *
 * NEGATIVES WORTH THE SPACE:
 *   - the named loop bound (`n = 8; for (i = 0; i < n; i++)`) WORKS exactly as the recon
 *     predicted -- `cmp r0, #8 / blt` instead of `cmp r0, #7 / ble`.  The recon's
 *     correction to elevation.md's "cmp #K / blt with K>0 is UNREACHABLE" is confirmed
 *     here on a loop back-edge, a second instance beside batch 302's switch-range ones.
 *   - `(unsigned short)heading` written inline at the facing update is INERT-LOOKING and
 *     is actually WRONG: see lever 4.  It is the folding, not the spelling.
 *   - the 0x21c gState offset as a named local (`koff = 0x87 << 2;`) gives the ROM's
 *     `ldr r3, =gState / mov r2, #0x87 / lsl r2, #2 / add r3, r2` instead of one pooled
 *     `gState+540`.  Batch 303's lever, fourth confirmation.
 *   - `iwram_3001ebc + 0x19c` and `iwram_3001e70 + 0x11a` need NO named offset: 0x19c is
 *     `0xce << 1` and 0x11a is `0x8d << 1`, both shiftable, and gcc builds them with
 *     `mov`/`lsl` unprompted because the base is a POINTER VALUE, not a SYMBOL_REF -- so
 *     there is no pool word to fold into in the first place.
 */
#include "gba/types.h"

extern unsigned char gState[];
extern volatile unsigned int gKeyHeld;
extern volatile unsigned int gKeyRepeat;
extern unsigned char gDebugMode;
extern unsigned char *iwram_3001ebc;
extern unsigned char *iwram_3001e70;

extern short L13254[] __asm__(".L13254");
extern int L1328c[] __asm__(".L1328c");
extern unsigned char L13280[] __asm__(".L13280");

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
extern void vec3_translate(int mag, int ang, vec3_t *v);
extern int Func_80122ac(void *a, vec3_t *v);
extern void Actor_TravelTo(void *a, int x, int y, int z);
extern void Actor_SetAnim(void *a, int anim);
extern int FastIntSqrtFP1616_RAM(int x);
extern int Func_8012204(void *pos);
extern void *CreateActor(int kind, int x, int y, int z);
extern void Actor_SetScript(void *a, void *script);
extern void Sprite_SetAnim(void *s, int anim);
extern void Func_800eaf8(void);

int ActorCmd_Player_World(void *r0)
{
    unsigned char *a;
    vec3_t p;
    vec3_t q;
    vec3_t r2;
    vec3_t r3;
    vec3_t r4;
    
    short dirs[10];
    int flags;
    int heading;
    int anim;
    int angx;
    int t;
    int koff;
    int n;
    unsigned short u;
    int i;
    int st;
    int k;
    unsigned short *tmr;
    unsigned short *cam;
    int tgt;
    int d;
    int e;
    int v;
    int w;
    unsigned char *ibase;
    unsigned char *spr;
    unsigned char *act;
    int mx;
    int mz;
    int mag;

    a = r0;
    anim = 2;
    flags = 0;
    koff = 0x87 << 2;
    if (gKeyHeld & *(unsigned short *)(gState + koff)) {
        *(int *)(a + 0x30) = 0x10000;
        *(int *)(a + 0x34) = 0x14000;
        anim = 5;
    } else {
        *(int *)(a + 0x30) = 0x8000;
        *(int *)(a + 0x34) = 0x4000;
    }
    if (gKeyRepeat & 0x200)
        *(int *)(a + 0x30) = 0x40000;
    t = L13254[(gKeyHeld >> 4) & 0xf];
    heading = t;
    angx = t << 16;
    u = (unsigned int)angx >> 16;
    if (u == 0xffff) {
        flags |= 4;
    } else {
        flags = 0;
        p.x = *(int *)(a + 8);
        p.y = *(int *)(a + 0xc);
        p.z = *(int *)(a + 0x10);
        vec3_translate(0x70000, u, &p);
        if (gDebugMode && (gKeyHeld & 0x200))
            goto done;
        if (!Func_80122ac(a, &p)) {
            q.x = *(int *)(a + 8);
            q.y = *(int *)(a + 0xc);
            q.z = *(int *)(a + 0x10);
            vec3_translate(0x70000, u + 0x1000, &q);
            if (!Func_80122ac(a, &q)) {
                q.x = *(int *)(a + 8);
                q.y = *(int *)(a + 0xc);
                q.z = *(int *)(a + 0x10);
                vec3_translate(0x70000, u - 0x1000, &q);
                if (!Func_80122ac(a, &q)) {
                    q.x = *(int *)(a + 8);
                    q.y = *(int *)(a + 0xc);
                    q.z = *(int *)(a + 0x10);
                    vec3_translate(0x70000, u + 0x2000, &q);
                    if (!Func_80122ac(a, &q)) {
                        q.x = *(int *)(a + 8);
                        q.y = *(int *)(a + 0xc);
                        q.z = *(int *)(a + 0x10);
                        vec3_translate(0x70000, u - 0x2000, &q);
                        if (!Func_80122ac(a, &q))
                            goto done;
                    }
                }
            }
        }
        dirs[0] = u + 0x1000;
        dirs[1] = u - 0x1000;
        dirs[2] = u + 0x2000;
        dirs[3] = u - 0x2000;
        dirs[4] = u + 0x3000;
        dirs[5] = u - 0x3000;
        dirs[6] = u + 0x4000;
        dirs[7] = u - 0x4000;
        n = 8;
        for (i = 0; i < n; i++) {
            angx = dirs[i] << 16;
            u = (unsigned int)angx >> 16;
            p.x = *(int *)(a + 8);
            p.y = *(int *)(a + 0xc);
            p.z = *(int *)(a + 0x10);
            vec3_translate(0x70000, u, &p);
            if (!Func_80122ac(a, &p)) {
                q.x = *(int *)(a + 8);
                q.y = *(int *)(a + 0xc);
                q.z = *(int *)(a + 0x10);
                vec3_translate(0x70000, u + 0x1000, &q);
                if (!Func_80122ac(a, &q)) {
                    q.x = *(int *)(a + 8);
                    q.y = *(int *)(a + 0xc);
                    q.z = *(int *)(a + 0x10);
                    vec3_translate(0x70000, u - 0x1000, &q);
                    if (!Func_80122ac(a, &q)) {
                        q.x = *(int *)(a + 8);
                        q.y = *(int *)(a + 0xc);
                        q.z = *(int *)(a + 0x10);
                        vec3_translate(0x70000, u + 0x2000, &q);
                        if (!Func_80122ac(a, &q)) {
                            q.x = *(int *)(a + 8);
                            q.y = *(int *)(a + 0xc);
                            q.z = *(int *)(a + 0x10);
                            vec3_translate(0x70000, u - 0x2000, &q);
                            if (!Func_80122ac(a, &q)) {
                                heading = angx >> 16;
                                goto done;
                            }
                        }
                    }
                }
            }
        }
        flags |= 1;
    }
done:
    if (iwram_3001ebc) {
        if (flags & 3)
            *(unsigned short *)(iwram_3001ebc + 0x19c) += 1;
        else
            *(unsigned short *)(iwram_3001ebc + 0x19c) = 0;
    }
    if (flags)
        Actor_SetAnim(a, 9);
    else
        Actor_SetAnim(a, anim);
    if (flags) {
        *(int *)(a + 0x38) = 0x80000000;
        *(int *)(a + 0x3c) = 0x80000000;
        *(int *)(a + 0x40) = 0x80000000;
        *(int *)(a + 0x24) = 0;
        *(int *)(a + 0x2c) = 0;
        if (flags & 3) {
            u = heading;
            d = (short)(u - *(unsigned short *)(a + 6));
            if (d > 0x1000)
                d = 0x1000;
            if (d < -0x1000)
                d = -0x1000;
            *(unsigned short *)(a + 6) += d;
        }
        tmr = (unsigned short *)(a + 0x64);
        v = 0;
        *tmr = v;
        v = 2;
        *(unsigned short *)(a + 0x66) = v;
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
        w = *tmr;
        if (*(short *)(a + 0x64)) {
            v = w - 1;
            *tmr = v;
        }
    }
    ibase = iwram_3001e70;
    tgt = L1328c[(gKeyHeld >> 4) & 0xf];
    cam = (unsigned short *)(ibase + 0x11a);
    e = tgt - *cam;
    e = (short)e;
    d = e / 8;
    if (d > 0x200)
        d = 0x200;
    if (d < -0x200)
        d = -0x200;
    if (d >= -0xf && d <= 0xf)
        d = tgt - *cam;
    *cam = *cam + d;
    st = *(unsigned char *)(a + 0x54);
    if (st == 1) {
        spr = *(unsigned char **)(a + 0x50);
        k = Func_8012204(a + 8);
        if (k == 9) {
            *(unsigned char *)(*(unsigned char **)(spr + 0x2c) + 6) = st;
            *(unsigned char *)(spr + 0x26) = 0;
        } else {
            *(unsigned char *)(*(unsigned char **)(spr + 0x2c) + 6) = 9;
            *(unsigned char *)(spr + 0x26) = st;
        }
        if (k == 6 && *(short *)(a + 0x64) == 0 && flags == 0) {
            act = CreateActor(0x18, *(int *)(a + 8), *(int *)(a + 0xc), *(int *)(a + 0x10));
            if (act) {
                spr = *(unsigned char **)(act + 0x50);
                Actor_SetScript(act, L13280);
                *(unsigned char *)(act + 0x55) = flags;
                *(unsigned char *)(act + 0x22) = 1;
                if (spr) {
                    Sprite_SetAnim(spr, 1);
                    *(unsigned char *)(spr + 0x26) = flags;
                    *(unsigned char *)(spr + 5) = (*(unsigned char *)(spr + 5) & ~0xc) | 4;
                    *(unsigned char *)(spr + 9) = (*(unsigned char *)(spr + 9) & ~0xc) | 8;
                }
                v = 0xa;
                *tmr = v;
            }
        }
    }
    Func_800eaf8();
    *(unsigned short *)(a + 4) += 1;
    return 1;
}

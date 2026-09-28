/* Anim_Torch  [rom_c9000]  --  asm/rom_c9000/rom_e6638_a_a.s @ 0x080e6638
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/80e6638.c \
 *     asm/rom_c9000/rom_e6638_a_a.s --whole
 * 316 instructions.  Batch 292 brief E, target 2.  WHOLE-FILE CONVERSION CHANCE:
 * that .s holds exactly ONE function (`grep -c func_start` = 1) and NO data
 * section (`python3 tools/datacheck.py asm/rom_c9000/rom_e6638_a_a.s` prints
 * nothing and exits 0) -- both verified here.  So no split is needed and the
 * verdict that matters is --whole.
 *
 * NON-MATCHING: 173 encodings of 348 differ (objcmp).
 *
 * 173 IS A TRUE DISTANCE.  objcmp prints NO SIZE line (784 bytes against the
 * ROM's 784), the encoding count is EXACT (348 against 348), and the
 * relocation SET is exact -- 26 relocations, the same symbols in the same
 * order, only their offsets sliding by +/-2 bytes.  The literal pool is a
 * 30-word multiset IDENTICAL to the ROM's, in the ROM's order, with ONE word
 * out of place (0x04000208 sits at index 24 where the ROM has it at 15).
 * Every remaining difference is instruction PLACEMENT inside a region, not a
 * wrong construct and not a wrong length.
 *
 * Verify with:
 *   scratch_elev/b292/E/cmp.sh src/non_matching/rom_c9000/80e6638.c \
 *     asm/rom_c9000/rom_e6638_a_a.s --whole
 * (from the repo root; --func Anim_Torch gives the same body, --whole also
 * checks the section tail, which is the one that matters for a whole file.)
 *
 * ================================================================
 * THE BLOCKER, WITH ITS PASS: global.c WILL NOT SPILL THE FRAME COUNTER
 * ================================================================
 *
 * The ROM keeps FOUR values in r8-r11 across the 0x60-iteration frame loop --
 * ev(r8), &ewram_201007e(r9), &REG_IME(r10), base(r11) -- and puts the frame
 * counter in **r4**, which is CALL-CLOBBERED under this tree's
 * `-fcall-used-r4`.  reload then brackets all three calls in the loop with
 * `str r4, [sp, #8]` / `ldr r4, [sp, #8]`: twice around `bl sin` inside the
 * 0xa0-iteration inner loop and once around `bl WaitFrames`.  Four
 * instructions the candidate does not have, plus three `mov rN, r8` copies the
 * ROM does not need because its counter is already a low register
 * (`cmp r4,#8`, `lsl r1,r4,#1`, `cmp r4,#0x7f` against our
 * `mov r1,r8 / cmp r1,#8` and so on).
 *
 * The pass is **global.c / reload**: `allocno_compare` ranks by
 * log2(n_refs)*freq/live_length, and the frame counter -- 6 references against
 * ewram_201007e's 2 -- outranks the pointers, so gcc hands it r8 and hoists
 * only two of the three pointers.  docs/elevation.md "A PIN CANNOT ASK FOR A
 * SPILL" (~line 16233) records that this outcome is the one allocation result
 * no source construct commands, and everything tried here agrees:
 *
 *   - `register u16 *top __asm__("r9")`                        200 (worse)
 *   - `register ... __asm__("r9")` + `__asm__("sl")` together  327 (much worse)
 *   - `"+r"` barrier on top and ime BOTH, before the loop      241
 *   - `"+r"` barrier on top and ime BOTH, in-loop              325
 *   A pin on a value assigned before the loop and used after a call inside it
 *   is dropped, exactly as batch 210 recorded.
 *
 * ONE HALF of the problem DID yield, and it is the entry worth keeping: a
 * `"+r"` barrier on the `top` pointer AT ITS FIRST IN-LOOP USE (not before the
 * loop) buys it r9.  195 -> 173.  That is consistent with the barrier's
 * recorded scope -- it moves live-range and copy-direction problems, never a
 * constant -- and here it is a pure live-range/priority nudge.  Placing the
 * same barrier on `ime` as well undoes it (325): the two compete.
 *
 * THE REPEATED-CONSTANT / SEPARATE-INLINE-BODIES LEVER DOES NOT APPLY HERE,
 * and this was checked rather than assumed.  The candidate's literal pool is
 * already the ROM's 30-word multiset, and both `mov r1,#0x90 / lsl r1,#3`
 * sites are already rebuilt separately (2 sites against the ROM's 2), as are
 * both `=REG_BLDALPHA` loads and all three `0x1000` words.  cse2 is collapsing
 * nothing in this function, so there is no constant to un-share.
 *
 * ================================================================
 * LOAD-BEARING CONSTRUCTS, each with its single drop from the 173 file
 * ================================================================
 *
 *  (a) THE QUADRANT FILL NEEDS TWO INDEPENDENT UPPER-HALF CURSORS: an int
 *      `off2` for q2 and a SEPARATE `u8 *hi` for q3.  Drop `hi` and derive q3
 *      from `off2` and it is 289 (and 12 bytes short).  With one cursor gcc
 *      commons `base+off2`, frees a register, and keeps `off` in a register --
 *      the ROM spills `off` to sp+0x10 precisely because r0-r11 are all busy.
 *      Two cursors reproduce the ROM's r9 (offset) + r10 (reduced giv
 *      base+offset) pair and force the spill.  The two forms are
 *      behaviourally identical (both cursors hold the same value) -- this is a
 *      register-pressure construct, and it is the single largest lever here.
 *
 *  (b) THE SHARED ZERO: `y = 0; off = y;` and not `off = 0; y = 0;`.  279.
 *      The ROM's `mov r1,#0 / ... / str r1,[sp,#0x10]` stores the counter's
 *      own zero register into the spill slot.  NOTE (a) AND (b) ARE COUPLED:
 *      measured one at a time on the pre-barrier file each was a REGRESSION
 *      (283 and 284 against 289) and only together did they pay (202).  A
 *      one-at-a-time sweep would have discarded both.
 *
 *  (c) `SqrtFn sq = Func_8000948;` AS A VARIABLE inside the inner loop, for
 *      the ROM's `ldr r3,=Func_8000948 / bl _call_via_r3`.  A direct call is
 *      249 and one instruction short -- the recorded rom_e0524 idiom, firing
 *      again.
 *
 *  (d) THE PALETTE ADDRESS AS A REBUILT CONSTANT, `*(u16 *)(0x5000000 + i*2)`.
 *      270 as a hoisted walking pointer `pal[i]`.  Spelled as the plain
 *      constant, gcc synthesizes `mov r5,#0xa0 / lsl r5,#0x13` INSIDE the loop
 *      and indexes off it, which is the ROM; a named pointer gets strength-
 *      reduced to a second walking cursor.  The shadow copy `*shadow++`
 *      alongside it IS a walking pointer, and that asymmetry is the ROM's.
 *
 *  (e) `ime = &REG_IME;` AS A NAMED LOCAL, with the SET_IO value written as
 *      `(u32)ime` and NOT `REG_ADDR_IME`.  Dropping the local: 199 (and 8
 *      bytes short).  Dropping only the `(u32)ime` cast: 197, and it costs a
 *      31st pool word -- the ROM's `mov r2,r10 / mov r3,r10 / strh r2,[r3]`
 *      writes the IME ADDRESS into IME from the SAME register that addresses
 *      it, and only `(u32)ime` gives cse one pseudo instead of a pointer and
 *      an unrelated integer constant.  This is what took the pool from 31
 *      words to the ROM's 30 and the encoding count from 349 to exactly 348.
 *
 *  (f) THE FRAME LOOP IS A `goto` LOOP, not a do-while.  180 as `do { } while
 *      (frame != 0x60)`.  The ROM's `cmp r4,#0x60 / beq done / b top` is the
 *      inverted-sense pair a `goto` loop gives; a do-while gives `bne top`.
 *      Confirms the brief's "the FRAME LOOP of an animation entry point
 *      usually wants to be a goto loop".
 *
 *  (g) THE PALETTE CHANNELS NARROW BY COMPOUND ASSIGNMENT: `r >>= 3; g >>= 3;
 *      b >>= 3;` as three statements before the pack, not `(b>>3)<<10 | ...`
 *      inside it.  176.  In-place `>>=` gives the ROM's two-operand
 *      `asr r1,#3 / asr r2,#3 / asr r0,#3`; the expression form gives
 *      three-operand `asr r3,r1,#3` into fresh pseudos.
 *
 *  (h) `top = ewram_201007e;` as a named local AT ALL: 194 without it (the
 *      barrier in (the 173 file) has nothing to attach to).  On the
 *      pre-barrier file the same local was INERT at 199 -- it only earns its
 *      keep together with the barrier.
 *
 * MEASURED INERT, with its number (all 173, i.e. no change):
 *   - `dy = y/8 + 0x40; dy = y - dy;` as two statements vs the single
 *     expression `y - (y/8 + 0x40)`.  173 either way on the final file, though
 *     it WAS worth 3 earlier (248 -> 245): gcc reassociates to
 *     `(y - y/8) - 0x40` in both spellings on the final file.
 *   - every ordering of the four quadrant cursor declarations (8 permutations,
 *     all 173 or worse; 0123/0321/3012 all 173, 0213/2301 258, 1032/3210 283).
 *   - `off2 -= 0x80` vs `off2 = off2 - 0x80` vs `off2 += -0x80`; moving `y++`
 *     among the cursor updates.  173 each.
 *   - `&base[off2]` and `(u8 *)((u32)base + off2)` and `off2 + base` for q2.
 *     173 each.
 *   - swapping which cursor feeds q2 and which feeds q3.  173.
 *   - `v = sq(...); v /= 2;` / `v = v / 2;` / an open-coded
 *     `(v + ((unsigned)v>>31)) >> 1`.  173 each.  The ROM's
 *     `add r3,r0,r3 / asr r0,r3,#1` (three-operand, fresh dest) against our
 *     two-operand in-place pair is allocation, not spelling.
 *   - `c = b<<10 | g<<5; c |= r;` and `c = b<<10; c |= g<<5; c |= r;`.  173.
 *   - `ev = frame;` instead of `ev = 0;`.  173.
 *   - reordering the local declarations so top/ime come first.  173.
 *   - the `register void *tf __asm__("r0")` sched2 pin on the SECOND StartTask
 *     (the Anim_Vine lever): inert here, and the ROM's own order differs
 *     between the two StartTask sites (`ldr r0` after the `lsl` at the first,
 *     before it at the second), so the pin would be needed at one site only.
 *
 * MEASURED WORSE (recorded so nobody re-spends it):
 *   - SET_IO on the three post-Anim_Djinni IO writes: 325.  Unnecessary
 *     anyway: `ldrh rX, <literal>` and `ldr rX, <literal>` ASSEMBLE TO THE
 *     IDENTICAL `ldr rd,[pc,#imm]` ENCODING (verified with arm-none-eabi-as:
 *     Thumb has no PC-relative LDRH, so gas emits LDR), so gcc's HImode pool
 *     load already IS the ROM's `ldr r3,.Le66b4 @ 0x2784`.  Worth knowing
 *     generally -- a `ldrh`/`ldr` literal mismatch in a diff is never real.
 *   - array-index form `base[off + x]` etc. for the quadrant stores instead of
 *     four walking cursors: 331.
 *   - lifting the queue push into AnimStart2's `QueuePush` static inline, or
 *     even just naming `queue = &gDMATaskCount` as a local: 183 both, and 4
 *     bytes short.  The ROM DOES keep gDMATaskCount in one register across the
 *     block and we reload it three times, but naming it costs more elsewhere.
 *   - `off2 = (0x7f - y) << 7` or `(0xfe<<6) - off` computed in the body
 *     instead of a carried cursor: 246 / 239.
 *
 * FLAG DIAGNOSTICS (diagnostic only -- no per-file flag row is proposed):
 *   -fno-strength-reduce 173, -fno-thread-jumps 173 (both inert),
 *   -fno-schedule-insns2 207, -fno-gcse 277, -fno-expensive-optimizations 291.
 *   Nothing here points at a flag; the residue is allocation, and no flag
 *   moves allocation in the right direction.
 *
 * NO .sym PROPOSAL.  Nothing in this function is gated on a symbol name: all
 * 26 relocations already resolve to the ROM's symbols in the ROM's order.
 *
 * SHIMS IN THIS FILE -- the complete list, for fakematch bookkeeping:
 *   1. `__asm__ volatile ("" : "+r" (top));`  ONE "+r" barrier, at the first
 *      in-loop use of `top` in the frame loop.  Worth 22 (195 -> 173).
 *   2. `Dma3Raw`, the `stmia r3!,{r0,r1,r2} / sub r3,#0xc` register-pinned
 *      inline asm, copied verbatim from the LANDED
 *      src/rom_b5000/rom_b5a0c_a_a_b.c.  Four `register ... __asm__` pins
 *      (r0-r3) inside it.
 *   3. `SET_IO` from include/gba/io.h -- the tree's own macro, not a shim of
 *      mine, but it carries a `do { } while (0)` scheduling barrier.
 *   No register pins on any local of Anim_Torch itself, no `volatile` local,
 *   no DMA3_SET, no `.equ`, no per-file flag.
 *
 * ================================================================
 * WHAT THE FUNCTION DOES (read off the disassembly)
 * ================================================================
 * Torch/fire battle animation.  Stores the caster in base+0x7828, AnimStart,
 * Anim_Djinni, sets BG2CNT/BLDALPHA/BG2PA, builds the 2D draw thunks, starts
 * Task_BlitAnim, then fills a 128x128 byte radial-distance ramp by four-fold
 * symmetry (four cursors, one per quadrant edge), builds a 63-entry fire
 * gradient into palette RAM and a shadow copy at ewram_2010002, blits it,
 * starts Func_80dbb9c, and runs 0x60 frames of a sin-driven 160-row scanline
 * offset table with an alpha fade in and out and a per-frame palette rotation
 * pushed through DMA3 and the DMA task queue.
 *
 * NOTE ON PRACTICE: every line below is original C written from this project's
 * own disassembly of rom_e6638_a_a.s, in the way docs/elevation.md prescribes.
 */
#include "gba/types.h"
#include "gba/io.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef int (*SqrtFn)(int v);

typedef struct {
    int f0, f4;
} State;

struct DmaTransfer {
    const void *src;
    void *dest;
    u32 control;
};

struct DmaQueue {
    u16 count;
    struct DmaTransfer tasks[32];
};

extern struct DmaQueue gDMATaskCount;
extern int *iwram_3001eec[];
extern u16 gBuffer[];
extern u16 ewram_2010002[];
extern u16 ewram_201007c[];
extern u16 ewram_201007e[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void Anim_Djinni(void *context, int a, int b, int c, int *p1, int *p2);
extern void BuildDraw2DFuncs(int a, DrawFn *out);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Func_80dbb9c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);
extern int sin(int a);
extern int Func_8000948(int v);
extern int _call_via_r3(void);

static inline void Dma3Raw(const void *src, void *dst, u32 cnt)
{
register vu32 *_base __asm__("r3") = &REG_DMA3SAD;
register const void *_src __asm__("r0") = src;
register void *_dst __asm__("r1") = dst;
register u32 _cnt __asm__("r2") = cnt;
    __asm__ volatile (
        "stmia\tr3!, {r0, r1, r2}\n\t"
        "sub\tr3, #0xc"
        :
        : "r" (_base), "r" (_src), "r" (_dst), "r" (_cnt)
        : "r0", "r1", "r2", "memory"
    );
}

void Anim_Torch(void *context)
{
    int p1;
    int p2;
    DrawFn d[2];
    char **tbl;
    char **pp;
    u8 *base;
    void *ctx;
    State **slot;
    u16 *shadow;
    int *w;
    int off;
    int off2;
    u8 *hi;
    int x;
    int y;
    int i;
    int frame;
    int ev;
    int amp;
    int ang;
    int k;
    int arg;
    int count;
    u32 saved;
    u16 *top;
    vu16 *ime;

    tbl = (char **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    ctx = (void *)*pp;
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(0x80 << 6);
    Anim_Djinni(context, 6, (*slot)->f4, 2, &p1, &p2);
    REG_BG2CNT = 0x2784;
    REG_BLDALPHA = 0x1000;
    REG_BG2PA = 0xaa;
    BuildDraw2DFuncs((*slot)->f4, d);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    arg = 0x90;
    StartTask(Task_BlitAnim, arg << 3);

    y = 0;
    off = y;
    off2 = 0xfe << 6;
    hi = base + (0xfe << 6);
    do {
        u8 *q0 = base + off;
        u8 *q1 = base + off + 0x7f;
        u8 *q2 = base + off2;
        u8 *q3 = hi + 0x7f;
        x = 0;
        do {
            int dy;
            int dx;
            int v;
            SqrtFn sq;
            dy = y / 8 + 0x40;
            dy = y - dy;
            dx = x - 0x40;
            sq = Func_8000948;
            v = sq(dx * dx + dy * dy) / 2;
            if (v == 0) {
                v = 1;
            }
            if (v > 0x3f) {
                v = 0x3f;
            }
            *q0++ = v;
            *q1-- = v;
            *q2++ = v;
            *q3-- = v;
            x++;
        } while (x != 0x40);
        off += 0x80;
        off2 -= 0x80;
        hi -= 0x80;
        y++;
    } while (y != 0x40);

    shadow = ewram_2010002;
    i = 1;
    do {
        int v;
        int r;
        int g;
        int b;
        int c;
            if (i > 0x1f) {
            v = 0x40 - i;
        } else {
            v = i;
        }
        r = v * 9;
        g = v * 7 - 0x2a;
        b = v * 7 - 0x38;
        if (r < 0) {
            r = 0;
        }
        if (g < 0) {
            g = 0;
        }
        if (b < 0) {
            b = 0;
        }
        if (r > 0xff) {
            r = 0xff;
        }
        if (g > 0xff) {
            g = 0xff;
        }
        if (b > 0xfa) {
            b = 0xfa;
        }
        r >>= 3;
        g >>= 3;
        b >>= 3;
        c = (b << 10 | g << 5) | r;
        *(u16 *)(0x5000000 + i * 2) = c;
        *shadow++ = c;
        i++;
    } while (i != 0x40);

    d[0](ctx, base, 0, 0, 0x80, 0x80);
    *(int *)(base + 0x7824) = 1;
    arg = 0x90;
    StartTask(Func_80dbb9c, arg << 3);

    top = ewram_201007e;
    ime = &REG_IME;
    frame = 0;
    ev = 0;
loop:
    if (frame <= 8) {
        REG_BLDALPHA = ev | 0x1000;
        amp = ev;
    } else {
        amp = frame * 2;
    }
    if (frame > 0x58) {
        REG_BLDALPHA = (0xc0 - ev) | 0x1000;
    }
    w = (int *)(base + (0xd3 << 7));
    ang = -(amp << 9);
    k = 0;
    do {
        *w++ = ((k << 18) - (sin(ang) << 7) + (0x80 << 11)) >> 10;
        ang += 0x80 << 2;
        k++;
    } while (k != 0xa0);
    if (frame > 0x7f) {
        *(int *)(base + 0x7824) = 1;
    } else {
        __asm__ volatile ("" : "+r" (top));
        gBuffer[1] = top[0];
        Dma3Raw(ewram_201007c, top, 0x80a0003e);
        saved = *ime;
        SET_IO(*ime, (u32)ime);
        count = gDMATaskCount.count;
        if (count <= 0x1f) {
            u32 *task = (u32 *)(count * 12 + (u32)&gDMATaskCount + 4);
            *(u16 *)&gDMATaskCount = count + 1;
            *task++ = (u32)ewram_2010002;
            *task++ = 0x5000002;
            *task = 0x8000003f;
        }
        SET_IO(*ime, saved);
    }
    WaitFrames(1);
    frame++;
    ev += 2;
    if (frame == 0x60) {
        goto done;
    }
    goto loop;
done:
    StopTask(Func_80dbb9c);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}

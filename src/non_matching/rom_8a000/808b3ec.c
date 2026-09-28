/* LoadMapActors  --  asm/rom_8a000/rom_8ace0_a_c_c.s  (0x0808b3ec)
 *
 * NON-MATCHING: 251 encodings of 281 differ (objcmp).
 * Working distance: 114 instructions in disagreeing regions of 294 (tryc --align).
 * NOT a true distance: length is 290 against the ROM's 294, four instructions short.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808b3ec.c asm/rom_8a000/rom_8ace0_a_c_c.s --whole
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_8a000/808b3ec.c --ref asm/rom_8a000/rom_8ace0_a_c_c.s --align
 *
 * Whole-file conversion: one function, no data. No pins, no flags. ZERO shims.
 *
 * WHAT THE FUNCTION IS
 *   (record *p, int slot). It first registers p in the four-entry table at the head of
 *   *(iwram_3001ebc), then walks the 0x18-byte record array until (short)p->id == -1 or
 *   slot > 0x41, instantiating each record: Func_808d428 gates on the kind, _CreateActor or
 *   _Actor_SetPos places it, a follower pair (b[0x54] == 1 && a[0x54] == 1) swaps palette
 *   bytes through Func_8003f3c, then position/bearing/behaviour are written and the actor is
 *   stored into the slot table at base + 0x14.
 *   `Random() % 0x1e` is an honest `__umodsi3` -- 0x1e is not a power of two, so per the
 *   batch-292 divisor table the libcall proves nothing either way about the divisor.
 *
 * BOTH LOOPS ARE `goto` LOOPS, AND THAT WAS THE FIRST LEVER: 136 -> 117.
 *   The natural spellings are wrong in two separate ways and BOTH are fixed by writing the
 *   loop out of labels and gotos:
 *   (a) `for (; (short)p->id != -1 && slot <= 0x41; p++)` gets its exit test COPIED to the
 *       loop entry by jump.c:1137 duplicate_loop_exit_test -- 11 extra instructions, and the
 *       copy is what makes every exit branch from the head block a two-instruction
 *       `bne over / b exit` pair. The pass runs only when a NOTE_INSN_LOOP_BEG is followed by
 *       an unconditional jump (jump.c:315), and it bails on a CALL_INSN, a CODE_LABEL or more
 *       than 20 insns in the exit code. A source-level goto loop emits no loop notes at all,
 *       so the pass cannot fire -- the same reason the landed sibling
 *       src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_a_c_a.c uses one.
 *   (b) the four-entry registration loop is also a goto loop: the ROM recomputes
 *       `lsl r2, r1, #2` every iteration, where a real `do {} while` gets strength reduction
 *       and an `add r2, #4` induction variable.
 *   The ROM's shape is therefore: preheader computes the test operands and `b`s to the test at
 *   the bottom; the body; a `step:` block that does `p++` and reloads; then the test. Every
 *   `continue` is `goto step`.
 *
 * SECOND LEVER: A FRESH VARIABLE FOR THE _Func_8011f54 RESULT, 117 -> 114.
 *   Reusing the general-purpose `t` there costs a `mov r5, r0` because t is live earlier; the
 *   ROM stores r0 straight into a+0x14 and adds it into a+0xc. Same family of finding as the
 *   sibling's "a fresh variable and a reused one are different levers here".
 *
 * MEASURED WORSE
 *   a separate test variable `sv = (short)raw` with the body recomputing `v = (short)raw`
 *     135 at length 295 -- it DOES buy the ROM's body-top `lsl/asr` (positional differences
 *     fall 265 -> 203) but it pushes the head block's `goto scan` branches out of `b<cond>`
 *     range and costs five instructions there. This is a coupled pair waiting to be found:
 *     the sign-extension shape and whatever keeps `scan:` inside 256 bytes.
 *   value-into-a-temp before the two `/ 0x10000` stores (either alone or with the 0x55
 *     address derived from the 0x66 one)                                              144
 * MEASURED INERT
 *   `idx` as a one-element array (gcc promotes it to a register anyway)               114
 *   `idx` declared last                                                              114
 *   a named `one = 1` at both `|=` sites                                              114
 *
 * THE REMAINING 114, IN FOUR CLUSTERS
 *  1. slot AND idx ARE IN EACH OTHER'S PLACES, and this is the largest cluster (~15 rows
 *     spread over the prologue and every use). The ROM puts `slot` in r11 (`mov r11, r1` in
 *     the prologue) and SPILLS `idx` to sp+4, storing it at both assignment sites and
 *     reloading it before each of its three uses. We do the exact opposite. Both are global
 *     allocnos crossing calls, and the register budget is identical -- r5/r6/r7 plus all four
 *     of r8..r11, with -fcall-used-r4 -- so exactly one of the two has to go to memory and
 *     global_alloc picks the other one. By refs/live_length `idx` (5 refs, ~100 insns) should
 *     beat `slot` (3 refs, the whole loop), which is what we get and NOT what the ROM has, so
 *     the ROM's choice is not explained by the ratio alone. Next attempt should read
 *     `.17.lreg`/`.18.greg` for both and check allocno_compare directly rather than nudging
 *     the source; three source nudges were all inert.
 *  2. THE SIGN-EXTENSION PAIR (~5 rows). The ROM reads p->id TWICE in the preheader --
 *     `ldrsh r3,[r7,r1]` with r1 = 0 for the test operand and `ldrh r2,[r7]` for the body's
 *     raw value -- and computes `v = (short)raw` AGAIN at the top of the body
 *     (`lsl r3,r2,#16 / asr r3,#16`), while at `step:` a single `ldrh` feeds both. Note the
 *     thumb constraint behind it: there is no `ldrsh rD,[rN,#imm]`, only the register-offset
 *     form, which is why a zero register appears next to every sign-extending load in this
 *     function. See MEASURED WORSE for the spelling that buys this and what it costs.
 *  3. THE FOLLOWER-PAIR BLOCK (~6 rows). The ROM keeps b->f50 in r0 and reuses r0 as the
 *     address for its three accesses; a->f50 also lands in r0 and is copied out with
 *     `mov r8, r0` mid-block so it survives Func_8003f3c. We assign the r8 copy FIRST and
 *     then copy back (`mov r8,r3` ... `mov r0,r8`), one instruction more. The ROM's shape
 *     reads like two pseudos -- a short-lived one for the local accesses and the saved copy --
 *     but cse merges any two-variable spelling of that.
 *  4. move2add on the actor offsets (~4 rows). The ROM derives a+0x55 from the a+0x66 address
 *     it already has (`sub r2, #0x11`); we recompute `mov r2,r6 / add r2,#0x55`. Same
 *     reload_cse_move2add same-hard-register requirement as ScreenTransitionIn's cluster 2.
 *
 * ONE THING TO SETTLE BEFORE THE NEXT ATTEMPT, because it may be worth 1 instruction and
 * affects other parks: the ROM calls Func_8000888 with `.call_via r3`, and
 * include/macros.inc:64 expands that to an INLINE `mov r12, pc / bx r3` (plus `.align 2,0`),
 * where gcc emits `bl _call_via_r3`. Both are 4 bytes but they are different encodings. NO
 * generated .s in the tree contains `.call_via` (33 hand-written ones do), so if the ROM
 * really holds the inline pair here, a function-pointer local cannot reach it and this is a
 * hard 1-instruction residue; if the .s author used the macro where a `bl` to a veneer would
 * have done, it is free. The landed sibling rom_8d9a4_c_c_c_a_a_a_c_a_c_b.c reaches
 * Func_8001af8 through a pointer local and its ROM .s spells that `bl _call_via_r3`, which is
 * evidence the two forms are genuinely distinct in this ROM.
 */
#include "gba/types.h"

struct Rec {
    u16 id;          /* 0x00 */
    s16 kind;        /* 0x02 */
    void *script;    /* 0x04 */
    int x;           /* 0x08 */
    int y;           /* 0x0c */
    int z;           /* 0x10 */
    u16 anim;        /* 0x14 */
    u8 f16;          /* 0x16 */
    u8 flags;        /* 0x17 */
};

struct MapState {
    struct Rec *recs[4]; /* 0x00 */
    int f10;             /* 0x10 */
    unsigned char *slots[1];  /* 0x14 */
};

extern struct MapState *iwram_3001ebc;

extern int Func_808d428(int kind);
extern int Func_808b398(int id);
extern unsigned char *GetFieldActor(int slot);
extern unsigned char *_CreateActor(int kind, int x, int y, int z);
extern void Func_8003f3c(int n);
extern int _GetFlag(int flag);
extern void _Actor_AddSpriteLayer(unsigned char *a, int n);
extern void _Actor_SetPos(unsigned char *a, int x, int y, int z);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern unsigned Random(void);
extern void Actor_SetBehavior(unsigned char *a, void *script);
extern int _Func_8011f54(int a, int b, int c);
extern int Func_8000888(int a, int b);

void LoadMapActors(struct Rec *p, int slot);

void LoadMapActors(struct Rec *p, int slot)
{
    struct MapState *st;
    struct Rec *r;
    unsigned char *a;
    unsigned char *o;
    int i;
    int idx;
    int kind;
    int v;
    int raw;
    int t;
    int d;
    int (*fp)(int, int);

    st = iwram_3001ebc;
    i = 0;
    if (st->recs[0] == p)
        goto scan;
    if (st->recs[0] == 0) {
        st->recs[0] = p;
        goto scan;
    }
next:
    i++;
    if (i > 3)
        goto scan;
    r = st->recs[i];
    if (r == p)
        goto scan;
    if (r != 0)
        goto next;
    st->recs[i] = p;
scan:
    raw = p->id;
    v = (short)p->id;
    goto test;
body:
    if (v <= 7) {
        idx = v;
    } else {
        if (v > 0x2705)
            goto step;
        idx = slot;
        slot++;
    }
    t = (short)p->kind;
    if (Func_808d428(t) == 0)
        goto step;
    if ((unsigned)(t - 0x30) <= 0x4f
     && *(s16 *)((char *)st + (0xcf << 1)) != 3
     && Func_808d428(t + 0x50) == 0)
        goto step;
    kind = Func_808b398((short)p->id);
    a = GetFieldActor(idx);
    if (a == 0) {
        a = _CreateActor(kind, p->x, p->y, p->z);
        if (p->flags & 1) {
            unsigned char *b = GetFieldActor(idx - 1);
            if (b[0x54] == 1 && a[0x54] == 1) {
                unsigned char *q = *(unsigned char **)(b + 0x50);
                q[0x1d] |= 1;
                t = q[0x1c];
                o = *(unsigned char **)(a + 0x50);
                o[0x1d] |= 1;
                Func_8003f3c(o[0x1c]);
                o[0x1c] = t;
            }
        }
        if (_GetFlag(0x21) != 0 && (unsigned)(kind - 0x12) <= 1)
            _Actor_AddSpriteLayer(a, 0xe2);
    } else if (_GetFlag(0x109) == 0) {
        _Actor_SetPos(a, p->x, p->y, p->z);
    }
    if (a != 0) {
        _Actor_SetAnim(a, 1);
        if (a[0x54] == 1) {
            o = *(unsigned char **)(a + 0x50);
            if (o != 0)
                o[0x24] = Random() % 0x1e;
        }
        *(u16 *)(a + 6) = p->anim;
        a[0x59] = 1;
        Actor_SetBehavior(a, p->script);
        _Actor_SetAnim(a, 1);
        *(s16 *)(a + 0x64) = *(int *)(a + 8) / 0x10000;
        *(s16 *)(a + 0x66) = *(int *)(a + 0x10) / 0x10000;
        if (*(int *)(a + 0xc) != 0) {
            a[0x55] = 4;
            *(int *)(a + 0xc) += 0x80 << 8;
        }
        if (*(s16 *)((char *)st + (0xcf << 1)) == 3) {
            a[0x55] &= 0xfe;
            if (_GetFlag(0x21) == 0) {
                fp = Func_8000888;
                *(int *)(o + 0x18) = fp(*(int *)(o + 0x18), 0xc0 << 8);
            }
        } else {
            d = _Func_8011f54(0, *(int *)(a + 8), *(int *)(a + 0x10));
            *(int *)(a + 0x14) = d;
            *(int *)(a + 0xc) += d;
        }
        a[0x23] = 1;
    }
    st->slots[idx] = a;
step:
    p++;
    raw = p->id;
    v = (short)raw;
test:
    if (v != -1 && slot <= 0x41)
        goto body;
}

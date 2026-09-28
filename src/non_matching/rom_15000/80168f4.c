/* AdvanceMsgText (0x080168f4) -- PARK CANDIDATE, batch 293 brief H.
 * NON-MATCHING, 212 encodings of 707.  NOT a distance (ref 1592 bytes / 707 encodings against ours 1596 / 708, one instruction and four
 * bytes over).  READ `--align` INSTEAD: 23 instructions in disagreeing regions of 724.  This is a
 * FIRST candidate for a 628-instruction function and everything structural is already right --
 * frame, prologue, the 31-entry jump table, block order, all thirteen cases, both DrawText sites,
 * and all 63 relocations in order until a 2-byte shift.
 *
 * (The claim line is first on purpose: parkcheck reads the FIRST `N encodings of M` in
 *  the header, and a drop ladder below is full of `N of M` strings whose earliest is the
 *  ladder's worst rung.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/80168f4.c \
 *     asm/rom_15000/rom_15e8c_c_a_a_a_c_a_a.s --func AdvanceMsgText
 * Distance while iterating (the number that ranks variants here):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_15000/80168f4.c \
 *     --ref asm/rom_15000/rom_15e8c_c_a_a_a_c_a_a.s --align
 * Reference: asm/rom_15000/rom_15e8c_c_a_a_a_c_a_a.s, 628 instructions by asmfacts,
 * 724 lines / 707 instructions by tryc, ONE function (grep -ci func_start = 1) and NO
 * data section, so landing it is a plain whole-file conversion: no split, no linker
 * script change, no new exports.
 *
 * STATE:  23 instructions in disagreeing regions of 724 (tryc --align)
 *         objcmp 212 of 707, ours 708 instructions, size 1596 against ref 1592.
 *         Two residue clusters, both characterised below.  NOT a true distance:
 *         instruction count is one over and size four bytes over.
 *
 * Ladder (tryc --align, rom 724):
 *      phase 1, skeleton with case 1 stubbed ......  441   (188 of it the absent body)
 *        + 0xffff / addressing / int-carrier fixes .  426
 *        + offset local at the opcode fetch ........  387
 *      phase 2, case 1 written ...................... 307  (ours 730)
 *        + frame slots by declaration order,
 *          glyph window pointer made block-local .... 214  (ours 726)
 *        + case 1's two window pointers split ....... 182  (ours 723)
 *        + 0xde/0xdf merge written in source ........ 175  (ours 726)
 *        + case 10/7 merge written in source ........ 136  (ours 724)
 *        + the 0x63 carrier moved AFTER its call ....  32  (ours 724)
 *        + volatile on iwram_3001cd0 ................  30  (ours 725)
 *        + case 10/7 merge UNDONE again .............  23  (ours 726)
 *
 * ================= STRUCTURE, AND TWO CORRECTIONS TO t3_notes.c =================
 *
 * 1. THE FUNCTION IS A LOOP.  .L16972 is the loop head; .L16efc is
 *    `if (--count != 0) goto .L16972`.  The sp+0x20 slot t3_notes.c called "a
 *    per-frame character-advance count ... written six times and read once" is the
 *    do-while TRIP COUNT, and the case bodies reassign it.  Written as one local
 *    round a `do { } while (--count != 0)`.
 *
 * 2. THE SWITCH HAS NO `default:`, AND THE 215-INSTRUCTION BLOCK IS NOT CASE 1.
 *    Case 1 (.L16a80 .. .L16c2c) is 188 instructions; the glyph path (.L16d76 ..
 *    .L16ede) is another ~150.  The jump table sends 11-14 and 16-29 to .L16d64,
 *    every non-returning case body ends `b .L16d64`, and .L16d64 is
 *    `if (base[0xea5] == 0) count = 1;` -- so .L16d64 is the BREAK LABEL plus the
 *    post-switch code, which is where a no-default switch sends out-of-range values
 *    too.  A `default:` body cannot be emitted AFTER the break label, and .L16d76
 *    is, so .L16d76 is reached by an explicit `if (op > 0x1e) goto glyph;`.  The one
 *    `cmp r7,#0x1e / bls .L1698e / b .L16d76` is that goto in long-branch form
 *    (Thumb `bhi` cannot reach 0x3ee bytes); the switch's own redundant range check
 *    is removed by jump.c's thread_jumps.  Writing a real `default:` for the glyph
 *    path is therefore wrong.
 *
 * THE JUMP TABLE MATERIALISED AS A TABLE ON THE FIRST TRY, with no filler case.
 * 13 case nodes (0,1,2,3,4,5,6,7,8,9,10,15,30) over a range of 31 clears both
 * case_values_threshold() and the sparseness test in expand_end_case.  Batch 292's
 * note that an explicit `case N` identical to the default is what tips the threshold
 * does not generalise: it was needed there because that switch had too FEW nodes.
 *
 * SOURCE ORDER IS EMITTED BLOCK ORDER: 3, 1, 2, 5, 6, 4, 8, 9, 10, 7, 15, {0,30}.
 * Not by value.  Cases 11-14 and 16-29 are not written at all.
 *
 * ================= THE LEVERS, EACH WITH ITS SINGLE DROP =================
 *
 * A. FRAME SLOTS ARE DECLARATION ORDER, AND THAT IS WORTH ~90 INSTRUCTIONS.
 *    FRAME_GROWS_DOWNWARD, so the FIRST declared local gets the HIGHEST sp offset.
 *    The four address-taken ints must be declared nx, ny, nw, nh to land at
 *    sp+0x30 / 0x2c / 0x28 / 0x24, and the spilled pseudos take 0x20 / 0x1c / 0x18
 *    in PSEUDO-NUMBER order, which is again declaration order -- so `count` must be
 *    declared before `ow` and `oh` to get the ROM's sp+0x20.  Declaring them the
 *    other way round puts count at 0x18 and moves every reference.  307 -> 214 with
 *    the glyph pointer change below.
 *
 * B. A HALFWORD INT CARRIER MUST BE MATERIALISED AFTER ANY CALL IT FOLLOWS.
 *    This was the single largest drop in the whole attempt: 136 -> 32.  The store
 *    `*(u16 *)(base + 0x12b6) = 0x63` needs the SET_IO int-carrier route (a bare
 *    literal pools as HImode: `ldr r3,=0x63` where the ROM has `mov r3,#0x63`).
 *    With the carrier written BEFORE `Func_8003f3c(*p)` gcc hoists the constant into
 *    a CALLEE-SAVED register across the call, which costs a callee-saved register and
 *    permutes r5/r6/r7 over the whole function -- m and op swap registers and ~90
 *    instructions move.  Writing it after the call costs nothing and fixes all of it.
 *    The documented rule "a halfword int carrier must be declared AFTER the address"
 *    is one instance of a bigger one: the carrier must be declared after everything
 *    that would otherwise make its live range cross a call.
 *
 * C. TWO WINDOW POINTERS IN CASE 1, NOT ONE.  The ROM loads m->f0 into r0 in the
 *    `Func_80199ec(m) == 0` arm and into r5 in the other; they are separate allocnos.
 *    One shared variable makes them one, costs a callee-saved register and forces
 *    &base[0x12f8] into call-clobbered r4 with a spill/reload pair.  214 -> 182.
 *    Same shape in the glyph path: its window pointer is dead at DrawText's return
 *    and wants r0, so it is a block-local there rather than the function-scope one.
 *
 * D. AN INT CARRIER FOR `m->f1c - 1`.  On a u16 member, `m->f1c = m->f1c - 1` is
 *    done in HImode and pools 0xffff; the ROM has a plain `sub r3,#1`, which is
 *    SImode.  `{ int t = m->f1c; m->f1c = t - 1; }` gets it.  Dropping this is
 *    23 -> 78, the largest single-drop regression measured here.
 *
 * E. `volatile u16` ON iwram_3001cd0 adds the ROM's `mov r2,r3` copy at the 0..2
 *    clamp.  32 -> 30 in count, at +1 length.  A plain two-local spelling
 *    (`u = g; v = u;`) is inert -- gcc coalesces the copy.
 *
 * F. THE DEAD READ OF gKeyHeld IS LOAD-BEARING; ITS `volatile` IS NOT.
 *    `ldr r1,=gKeyHeld / ldr r3,[r1]` sits in the first block with r3 immediately
 *    overwritten by `ldr r3,=gState`.  Removing the bare `gKeyHeld;` statement is
 *    23 -> 164.  Removing only the `volatile` qualifier measures 23, i.e. exactly
 *    inert -- gcc-2.96 keeps the load either way.  The qualifier is kept because it
 *    is the honest reason a dead read exists and the tree already carries
 *    `extern volatile unsigned int gKeyHeld;` in src/rom_c9000/rom_e3958_c_c_c_b.c,
 *    but it should not be reported as a lever.
 *
 * G. THE OPCODE FETCH NEEDS AN INT OFFSET LOCAL.  The ROM reaches
 *    `ldrh rD,[base, offset]` (register + register).  Any pointer-shaped spelling --
 *    `((u16 *)(base + 0xeb0))[i]` or `*(u16 *)(base + (0xeb0 + i * 2))` -- folds base
 *    in first and emits `add r3,r8` before the 0xeb0.  `{ int o = 0xeb0 + i * 2;
 *    ... base + o; }` is what gives the ROM's shape.  426 -> 387 across six sites.
 *    Same mechanism at Data_32224: written as `Data_32224 + (op - 0x20) * 32`, fold
 *    reassociates it to `(Data_32224 - 0x400) + op * 32` and emits
 *    `ldr r3,=0xfffffc00 / add r1,r2,r3`; a pointer local plus `i -= 0x20; i <<= 5;`
 *    in separate statements keeps the ROM's `sub r2,#0x20 / lsl r2,#5`.
 *
 * ================= THE CROSS-JUMP QUESTION, WHICH CUTS BOTH WAYS =================
 *
 * Three places in this function have two blocks sharing a tail.  Whether to WRITE
 * the merge or to let find_cross_jump make it is NOT uniform, and the deciding fact
 * is whether cse's availability across the join matters:
 *
 *  - cases 5/6 (.L16c6a, a shared `strh r3,[r6,#0x14]`): written as a `goto` to a
 *    shared label.  Matched first try.
 *  - the 0xde/0xdf pair (.L16df4): MUST be written as a merge, because the ROM
 *    RE-READS m->f12 in the merged block.  cse_basic_block stops at a label with
 *    multiple references, so an expression available before the branch is NOT
 *    available after a written join -- which is exactly why the ROM recomputes
 *    `(m->f12 + 1) & 0x1ff` from a fresh load instead of reusing the value computed
 *    for the next-opcode index.  Two duplicated bodies let cse share it and the
 *    recomputation disappears.  175 with the merge, against a duplicated form that
 *    measured 182 -- and the merged cursor update is 4 instructions of the residue
 *    if it is not written.
 *  - cases 10/7 (.L16d24): must NOT be written as a merge.  The ROM's `mov r0,r6`
 *    appears in BOTH predecessors, which is the signature of find_cross_jump merging
 *    only the longest identical tail after the scheduler placed the argument setup
 *    differently in each.  A written merge produces one `mov r0,r6` in the joined
 *    block instead.
 *
 * AND THAT LAST ONE IS A COUPLED PAIR, WHICH IS WHY THE LADDER LOOKS ODD.
 * Writing the 10/7 merge was worth 175 -> 136 BEFORE lever B, and costs 23 -> 32
 * AFTER it.  A one-at-a-time sweep that had already applied B would have reported
 * the written merge as a regression and discarded it, and a sweep that had not would
 * have kept it.  The same two changes, opposite signs, depending only on order.
 *
 * ================= WHAT IS LEFT: TWO CLUSTERS =================
 *
 * CLUSTER A -- case 7, 3 instructions.  The ROM materialises 0xf into r2 BEFORE the
 * f18 store so r3 can carry 0 and then 0xa:
 *      mov r2,#0xf / strh r3,[r6,#0x18] / mov r0,r6 / mov r3,#0xa / strh r2,[r6,#0x16]
 * where this emits `strh r3,[f18] / mov r3,#0xf / strh r3,[f16] / mov r3,#0xa`.
 * Same instruction count, different register and order.  Inert: reordering the three
 * stores (42, worse), an `unsigned` carrier for the 0xf (23, no change), a named
 * `int k = 0xf` (23, no change).
 *
 * CLUSTER B -- the 0xde/0xdf test, ~8 instructions and the +1 length.  gcc hoists
 * `t = 0x4000` ABOVE `cmp r4,#0xde` and inverts the branch, so the 0xde arm becomes
 * empty:  mov r3,#0x80 / lsl r3,#7 / cmp r4,#0xde / beq <join>  where the ROM has
 * cmp r4,#0xde / bne <0xdf test> / mov r3,#0x80 / lsl r3,#7 / b <join>.
 * Knock-on: the hoist takes r2 for the base copy (`mov r2,r8` where the ROM has
 * `mov r0,r8`), which kills the 0x1ff the ROM keeps live into the merged block, so
 * the merged cursor update needs its own `ldr r2,=0x1ff` -- and that extra pool word
 * forces a `b` over a mid-block literal pool.  One source change, three symptoms.
 * Inert or worse: `op |= K` inside each arm instead of a merged `t` (37, and +3
 * length); a separate merge variable not shared with the 10/7 merge (30, no change);
 * `unsigned t` (23); an explicit `else { if (...) goto; }` block (23); a named
 * `int mask = 0x1ff` shared by the index and the update (60, much worse); an
 * `int mk = 0x1ff` carrier in the merged block only (40 by --align but that figure
 * is inflated by label renumbering -- it is the SAME two clusters, and it reaches
 * EXACT aligned length 724 with objcmp 209 of 707 at 706 instructions / 1588 bytes,
 * so it is the better candidate on length and the worse on count; kept as v_mk).
 * Flags, diagnostic only: gcc-2.96 here has no -fno-if-conversion; -fno-thread-jumps
 * is exactly inert; -fno-gcse is 196 and -fno-sched2 is 191, i.e. both passes are
 * doing almost all the right work and this is one wrong decision inside them.
 *
 * NO SHIMS.  Zero `register ... __asm__` declarations in the code.
 *
 * NO .sym PROPOSAL.  t3_notes.c's reading of 0xea4 / 0xea5 / 0x397 / 0x103 / 0xcff
 * as base-relative offsets and flag ids is confirmed: 0xea4 and 0xea5 are byte flags
 * at base, 0x397 and 0xa and 0x14 / 0x3c / 0x78 are frame counts written to m->f14,
 * and 0xcff / 0xd00 / 0xf00 are 8.8 fixed-point scroll steps on m->f6.  None is a
 * message id and none completes the function.
 *
 * SYMBOLS: iwram_3001e8c, gKeyHeld, gState, iwram_3001af8, iwram_3001cd0,
 * Data_32224 and the three .L738xx tables, all already `.global` where they live --
 * the __asm__ aliases below are the only wiring needed.  Callee returns: Func_80199ec
 * is the only one whose result is read; every other callee is `void`, which is the
 * direction t3_notes.c predicted for this bank and it held on all eleven.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_15000/80168f4.c \
 *     --ref asm/rom_15000/rom_15e8c_c_a_a_a_c_a_a.s --align
 */
typedef unsigned char u8;
typedef unsigned short u16;

/* the record at m->f0 */
struct TextWin {
    u16 f0;
    u16 f2;
    u16 f4;
    u16 f6;
    u16 f8;
    u16 fa;
    u16 fc;
    u16 fe;
    u16 f10;
    u16 f12;
    u16 f14;
    u16 f16;
};

struct MsgBox {
    struct TextWin *f0;
    u16 f4;
    u16 f6;
    u16 f8[4];          /* 0x08 .. 0x0e, indexed by f10 */
    u16 f10;
    u16 f12;
    u16 f14;
    u16 f16;
    u16 f18;
    u16 f1a;
    u16 f1c;
    u16 f1e;
    u16 f20;
    u16 f22;
    u16 f24;
};

extern u8 *iwram_3001e8c;
extern volatile unsigned int gKeyHeld;
extern u8 gState[];
extern unsigned int iwram_3001af8;
extern volatile u16 iwram_3001cd0;
extern u8 Data_32224[];
extern u8 L73808[] __asm__(".L73808");
extern u8 L7380b[] __asm__(".L7380b");
extern u8 L7380e[] __asm__(".L7380e");

extern void Func_80167e0(int a);
extern void Func_80167d8(struct MsgBox *m);
extern int  Func_80199ec(struct MsgBox *m);
extern void Func_801999c(struct MsgBox *m);
extern void Func_80167ac(struct MsgBox *m);
extern void Func_8016478(struct TextWin *w);
extern void ClearUIRegion(int x, int y, int w, int h);
extern void Func_80170f8(int x, int y, int w, int h);
extern void Func_801868c(int i, int *a, int *b, int *c, int *d, u16 *arr, int f);
extern void Func_8003f3c(int a);
extern int  DrawText(struct TextWin *w, int ch, int x, int y, int a);
extern void _PlaySound(int id);

int AdvanceMsgText(struct MsgBox *m)
{
    int nx;
    int ny;
    int nw;
    int nh;
    int count;
    int ow;
    int oh;
    int i;
    u8 *base;
    struct TextWin *w;
    unsigned int op;
    unsigned int next;
    int idx;
    int t;
    int x;
    int y;
    int snd;
    int ret;
    int adv;

    base = iwram_3001e8c;
    gKeyHeld;
    count = L7380b[gState[0x20c]];
    if (base[0xea5] != 0) {
        int v;
        v = iwram_3001cd0;
        if (v < 0)
            v = 0;
        if (v > 2)
            v = 2;
        count = v * 5 + 3;
    }
    if (m->f1c != 0) {
        Func_80167e0(1);
        {
            int t = m->f1c;
            m->f1c = t - 1;
        }
        return 0;
    }
    if (gKeyHeld == 0 && m->f22 != 0) {
        m->f22 = m->f22 + 0xffff;
        return 0;
    }
    do {
        op = 0;
        if (m->f20 == 0) {
            int o = 0xeb0 + m->f12 * 2;
            op = *(u16 *)(base + o);
        }
        if (op > 0x1e)
            goto glyph;
        switch (op) {
        case 3:
            m->f4 = m->f1e;
            if (m->f0->f16 & 8) {
                if (m->f6 > 0xcff) {
                    Func_80167d8(m);
                    count = 1;
                } else {
                    m->f6 = m->f6 + 0xd00;
                }
            } else {
                m->f6 = m->f6 + 0xf00;
                if (m->f10 <= 2)
                    m->f10 = m->f10 + 1;
            }
            break;
        case 1:
            if (base[0xea4] != 0 && m->f14 < 0x384)
                iwram_3001af8 = 0;
            m->f14 = 0x397;
            if (Func_80199ec(m) == 0) {
                struct TextWin *v = m->f0;
                if (v->f8 == 0)
                    break;
                if (v->fa == 0)
                    break;
                if (base[0x12f8] != 0)
                    break;
                DrawText(v, 1, v->f8 * 4 - 8, v->fa * 8 - 0x10, 1);
                base[0x12f8] = 1;
                break;
            }
            w = m->f0;
            nx = w->fc;
            ny = w->fe;
            ow = w->f8;
            oh = w->fa;
            i = m->f12;
            base[0x12f8] = 0;
            Func_8016478(w);
            if (m->f24 == 0 && (w->f8 | w->fa) != 0)
                ClearUIRegion(w->fc, w->fe, w->f8, w->fa);
            i = (i + 1) & 0x1ff;
            {
                int o = 0xeb0 + i * 2;
                if (*(u16 *)(base + o) != 0 && (w->f8 | w->fa) != 0) {
                    if (m->f24 != 0) {
                        ClearUIRegion(w->fc, w->fe, w->f8, w->fa);
                    } else {
                        Func_801868c(i, &nx, &ny, &nw, &nh, m->f8, 0);
                        if (w->f16 & 0x80) {
                            if (oh != nh)
                                ny = ny - (nh - oh);
                            if (ny < 0)
                                ny = 0;
                        }
                        if ((w->f16 & 0x100) == 0) {
                            nx = nx + (ow - nw) / 4;
                            Func_801868c(i, &nx, &ny, &nw, &nh, m->f8, 2);
                        }
                        w->fc = nx;
                        w->fe = ny;
                        w->f8 = nw;
                        w->fa = nh;
                    }
                    Func_80170f8(w->fc, w->fe, w->f8, w->fa);
                }
            }
            m->f4 = m->f1e;
            m->f6 = 0;
            m->f10 = 0;
            {
                u16 *p = (u16 *)(base + 0x12b6);
                Func_8003f3c(*p);
                {
                    unsigned v = 0x63;
                    *p = v;
                }
            }
            break;
        case 2:
            if (base[0xea4] != 0 && m->f14 < 0x384)
                iwram_3001af8 = 0;
            if (Func_80199ec(m) != 0)
                return 9;
            m->f14 = 0x397;
            break;
        case 5:
            if (m->f14 == 0)
                m->f14 = 0x14;
            goto stopSound;
        case 6:
            if (m->f14 == 0)
                m->f14 = 0x78;
        stopSound:
            {
                u16 *s = (u16 *)(base + 0x12f6);
                unsigned z = 0;
                *s = z;
            }
            Func_801999c(m);
            break;
        case 4:
            if (m->f14 == 0)
                m->f14 = 0x3c;
            {
                u16 *s = (u16 *)(base + 0x12f6);
                unsigned z = 0;
                *s = z;
            }
            break;
        case 8:
            m->f12 = (m->f12 + 1) & 0x1ff;
            {
                int o = 0xeb0 + m->f12 * 2;
                m->f16 = *(u16 *)(base + o);
            }
            Func_80167ac(m);
            break;
        case 9:
            m->f12 = (m->f12 + 1) & 0x1ff;
            {
                int o = 0xeb0 + m->f12 * 2;
                m->f18 = *(u16 *)(base + o);
            }
            Func_80167ac(m);
            break;
        case 10:
            m->f12 = (m->f12 + 1) & 0x1ff;
            {
                int o = 0xeb0 + m->f12 * 2;
                t = *(u16 *)(base + o);
            }
            m->f1a = t;
            Func_80167ac(m);
            break;
        case 7:
            m->f18 = 0;
            m->f16 = 0xf;
            m->f1a = 0xa;
            Func_80167ac(m);
            break;
        case 15:
            m->f12 = (m->f12 + 1) & 0x1ff;
            {
                int o = 0xeb0 + m->f12 * 2;
                m->f0->f12 = *(u16 *)(base + o);
            }
            m->f14 = 0xa;
            m->f12 = (m->f12 + 1) & 0x1ff;
            break;
        case 0:
        case 30:
            m->f20 = 1;
            return 8;
        }
        if (base[0xea5] == 0)
            count = 1;
        goto tick;
    glyph:
      {
        struct TextWin *gw;
        x = (m->f4 + 0x80) / 0x100;
        y = (m->f6 + 0x80) / 0x100;
        snd = L7380e[gState[0x20c]];
        idx = m->f12;
        if (base[0xea4] != 0)
            x = x + 8;
        {
            int o = 0xeb0 + ((idx + 1) & 0x1ff) * 2;
            next = *(u16 *)(base + o);
        }
        if (next == 0xde)
            t = 0x4000;
        else if (next == 0xdf)
            t = 0x8000;
        else
            goto noWide;
        op |= t;
        m->f12 = (m->f12 + 1) & 0x1ff;
    noWide:
        gw = m->f0;
        if ((gw->f16 & 8) == 0 && op > 0x20 && next > 0x20) {
            u8 *tbl;
            int i;
            int j;
            u16 sum;
            i = op;
            j = next;
            tbl = Data_32224;
            i -= 0x20;
            j -= 0x20;
            i <<= 5;
            j <<= 5;
            sum = *(u16 *)(tbl + i) + *(u16 *)(tbl + j);
            if (sum <= 0xf) {
                op |= next << 8;
                m->f12 = (m->f12 + 1) & 0x1ff;
            }
        }
        ret = DrawText(gw, op, x, y, 0);
        m->f22 = L73808[gState[0x20c]];
        if (ret != 0) {
            u16 *p;
            p = (u16 *)(base + 0x12f4);
            if (*p != 0) {
                u16 *q;
                q = (u16 *)(base + 0x12f6);
                if (*q == 0) {
                    if (op != 0x20) {
                        _PlaySound(*p + (op & 3));
                        *q = snd;
                    }
                } else {
                    *q = *q + 0xffff;
                }
            }
            adv = ret << 8;
            if (op == 0x20)
                adv = adv + m->f8[m->f10];
            m->f4 = m->f4 + adv;
        }
        if (op == 0x20 && base[0xea5] == 0)
            count = 1;
      }
    tick:
        if (m->f14 != 0) {
            m->f14 = m->f14 + 0xffff;
            if (m->f14 != 0)
                goto next_char;
        }
        m->f12 = (m->f12 + 1) & 0x1ff;
    next_char:
        ;
    } while (--count != 0);
    return 0;
}

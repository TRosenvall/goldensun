/* Func_807905c (ApplyLevelUp, 0x0807905c) -- NON-MATCHING.
 * 8 of 286 encodings differ.  264 instructions against 264, 22 32-bit entries
 * against 22, 616 bytes against 616 -- all three length dimensions AGREE, so
 * the 8 IS A DISTANCE.  Aligned residue (tools/aligncmp.py) 8 in 8 hunks.
 * Batch 348 brief E took it from 38 of 286 at +2 instructions.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/807905c.c \
 *     asm/rom_77000/rom_79008_a_c.s --func Func_807905c
 *
 * NO SPLIT IS NEEDED, AND THE OLD HEADER WAS WRONG ABOUT THIS.  It said the .s
 * "holds TWO functions, Func_8079008 (CanLevelUp, 40 instructions) and
 * Func_807905c" and prescribed a two-stem split with a stage1.ld edit.  The file
 * holds ONE `.thumb_func_start`, Func_807905c, at line 9 and nothing else;
 * Func_8079008 already has its own stem (rom_79008_a_b.s).  Verbatim:
 *     $ python3 tools/split_s.py asm/rom_77000/rom_79008_a_c.s Func_807905c --dry-run
 *     asm/rom_77000/rom_79008_a_c.s holds only Func_807905c and no data;
 *     convert it directly, no split needed
 *     $ python3 tools/datacheck.py asm/rom_77000/rom_79008_a_c.s   # silent, exit 0
 * piece.py agrees (1 function, no non-function exports).  So the landing is a
 * plain conversion to src/rom_77000/rom_79008_a_c.c with no linker-script work
 * at all.  PIN COUNT ZERO, tools/shimcount.py clean -- no device.
 *
 * ============================================================
 * WHAT THE FUNCTION IS   (unchanged from the previous header, still correct)
 * ============================================================
 * Levels a PC up by one and returns the eight-halfword gain record it was
 * handed. `g->f0` is the new level, `g->f2` is 0xffff, `g->f4..g->fe` are the
 * six stat gains. A 0x2c-byte Func_8004970 scratch holds the old +0x129 byte,
 * the old level, and the PCStats pointer, and is `free`d on both exits.
 *
 * Each of the six gains is
 *     (unsigned)(((unsigned)(Random() * 20) >> 16) + (curve[k+1] - curve[k])) / 20
 * where k = clamp(g->f0 / 20, 0, 4) is the twenty-level bracket. The six curves
 * live at base+0x50, +0x5c, +0x68, +0x74, +0x80 (six shorts each) and +0x8c
 * (six bytes) inside the 0xb4-byte PCStats record GetPCBaseStats returns.
 *
 * ============================================================
 * THE FIVE OLD LOAD-BEARING FACTS ALL SURVIVED -- re-measured, batch 348
 * ============================================================
 * 1. The curve lookup must be a struct array index, not byte arithmetic.
 * 2. The first two curves are `short`, the next three `unsigned short`, the
 *    last `unsigned char`.
 * 3. The PCStats pointer must be re-read from the scratch at every site,
 *    inline -- no hoisted `b = s->f8;`.
 * 4. The bracket index k is a `short`.
 * 5. The last gain is held in a temporary spent twice, into `u[0x1e]` and into
 *    `g->fe`.
 * None of these was re-screened; they were inherited and the body still
 * reproduces the stream they predict.
 *
 * ============================================================
 * WHAT BATCH 348 CHANGED, AND WHY -- the +2 was ONE SITE, AND IT WAS A MODE
 * ============================================================
 * The old body's whole excess was `lsl r1,#0x10 / asr r1,#0x10`: the widening
 * of the tail temporary before `add r3,r1`, present because the temporary was
 * `unsigned short`.  Re-measured and still exactly that: ONE region, ONE site,
 * two instructions, everything else aligned.  `short`, `unsigned short`,
 * `u[0x1e] += (unsigned char)m`, `u[0x1e] += m & 0xff`,
 * `u[0x1e] = (unsigned char)(u[0x1e] + m)` and `u[0x1e] = u[0x1e] + m` are all
 * BIT-IDENTICAL to each other -- the old header's "inert at 38/288" for the
 * cast was a measurement of nothing, which is the recorded-negative trap.
 *
 * The old header's bound was "`int` costs more than it saves, 214 of 290", and
 * THAT BOUND WAS WRONG -- it was true only of the one-statement spelling.
 * `int m` with `m = g->fe + <quotient expression>;` does cost +4, and the
 * mechanism is now named: LOCAL-ALLOC owns this pseudo, not global-alloc.
 * `-da` shows `;; Register 41 in 5.` in the `.17.lreg` dump (that line is
 * local-alloc.c:2251's dump_local_alloc) for the `int` spelling and NO entry
 * for the `unsigned short` one, because with HImode the DECL pseudo dies and
 * the value lives in a chain of short-lived pseudos (451/453/454/455, all
 * assigned r1).  `.17.lreg`'s "crosses" lines show pseudo 41 crossing ZERO
 * calls, so find_reg's call-clobbered exclusion (local-alloc.c:1964-1969) is
 * NOT what pushes it to r5; QTY_CMP_PRI (local-alloc.c:1495-1497,
 * floor_log2(n_refs)*n_refs*size/(death-birth)) is -- one long-lived qty sorts
 * late, r0-r3 are already marked by post_mark_life, and REG_ALLOC_ORDER
 * (arm.h:989: 3,2,1,0,12,14,4,5) hands it r5.  r5 was `k`'s, so `k` went to sl
 * and the clamp paid `movs r2,#4 / mov sl,r2 / mov r0,sl` plus one more copy:
 * the four extra instructions, all in the CLAMP region, none in the tail.
 *
 * THE FIX IS TO NAME THE QUOTIENT, so the temporary is an SImode value defined
 * by an ADD rather than by a copy of the libcall return:
 *     n = <quotient expression>;      (int, the previously-dead `n`)
 *     m = g->fe + n;                  (int)
 * 264/264 instructions, 616/616 bytes, 22/22 32-bit entries, 9 of 286.
 * `d = <quotient>; m = g->fe + d;` measures IDENTICALLY (9 of 286), so the
 * carrier's identity does not matter -- only that the add is not the copy.
 * The near misses say what the lever is:
 *     m = <quotient>; m += g->fe;     266 insns, 50 of 286 -- m gets r0 from
 *                                     the copy suggestion and the long-lived
 *                                     `mov r0, sl` unit copy is pushed to r2,
 *                                     paying two extra `mov r2, sl`
 *     m = g->fe; m += <quotient>;     268 insns, 214 of 286 -- gcc merges it
 *                                     back into the one-statement form
 * So the axis is NOT "int vs short" and NOT declaration order (nine positions
 * were already measured inert and still are); it is WHICH INSN DEFINES THE
 * PSEUDO.
 *
 * `g->fe = m;` BETWEEN `u[0x20] = 0;` AND `u[0x21] = 0;` is worth one more
 * encoding: it puts `strh r1,[r6,#14]` in the ROM's slot.  The four positions
 * measure 15 / 11 / 8 / 9 of 286 (after u[0x1e], u[0x1f], u[0x20], u[0x21]).
 * The natural trailing position is the 9; this file takes the 8.  A successor
 * that prefers the plainer order loses exactly one encoding.
 *
 * ============================================================
 * THE RESIDUE: 8 encodings, FOUR one-slot sched2 swaps, no excess
 * ============================================================
 * Every one is OURS SCHEDULING AN INSTRUCTION ONE SLOT EARLY, in a pair of
 * adjacent independent insns:
 *   (a) `ldrh r1,[r6,#14]`  before  `ldrh r3,[r2,#16]`   (ROM: after)
 *   (b) `ldrh r3,[r0,#18]`  before  `ldrh r2,[r6,#6]`    (ROM: after)
 *   (c) `ldrh r3,[r0,#26]`  before  `ldrh r2,[r6,#10]`   (ROM: after)
 *   (d) `mov r0, fp`        before  `strb r2,[r3,#0]`    (ROM: after)
 * (b) and (c) are the only two of the five halfword updates where the u-side
 * base is the long-lived `r0` copy rather than a fresh `mov r2, sl`.  In the
 * other three the g-side load TARGETS r2, which the u-side load uses as its
 * base, so the order is forced by an anti-dependence and both streams agree.
 * In (b) and (c) nothing forces it: both loads have exactly one dependent (the
 * `adds`), so rank_for_schedule falls past INSN_PRIORITY
 * (haifa-sched.c:4061-4064), past the last-scheduled-insn class rung
 * (:4069-4096) and past the dependent-count rung (:4099-4111) to
 * `INSN_LUID (tmp) - INSN_LUID (tmp2)` at :4114, i.e. ORIGINAL ORDER.
 *
 * MEASURED on the closed body, all at 264/264 instructions:
 *   `*(unsigned short *)(u + 0x12) = g->f6 + *(unsigned short *)(u + 0x12);`
 *   at sites 0x12 and 0x1a                       8 of 286, aligned 8 in 6
 *       -- it FIXES the order of (b) and (c) and SWAPS THEIR REGISTERS, because
 *          the add's first operand is what gets r3.  The order and the register
 *          assignment are the same knob pulled from opposite ends; this is the
 *          precondition a successor needs, not the arithmetic.
 *   the same swap at 0x10 / 0x18 / 0x1c          31 / 10 / 10 of 286 -- worse
 *   all five swapped                             29 of 286
 *   an `int` carrier for the g-side value at 0x12 and/or 0x1a
 *                                                +2 instructions, 44-56 of 286
 *          -- the carrier gets its own register and costs a `mov`
 *   `m = g->fe + n;` moved after update 1..5     +1/+1/+2/+2/+0 instructions,
 *          51 / 54 / 56 / 54 / 50 of 286 -- the m-position axis is CLOSED
 *
 * BOTH MECHANICAL AXES ARE CLOSED ON **THIS** BODY -- re-run, not inherited:
 *   FLAG AXIS, 8 groups: -fno-strength-reduce and -fno-rerun-loop-opt are
 *     bit-identical at 8; -fno-strict-aliasing 37, -fno-schedule-insns2 61,
 *     -fno-gcse 220 (-1 insn), -fno-rerun-cse-after-loop 233 (+2),
 *     -O1 244 (+1), -ffixed-r7 294 (+8).  Nothing beats base.
 *   ANCHOR AXIS, 56 of 56 boundaries swept (NOT capped): all inert or worse,
 *     best 8 = base.  Five placements tie at 8 (boundaries 3, 4, 8, 9, 10).
 * So the next round should NOT re-screen either; spend it on the tie below.
 *
 * NEXT.  The question is narrow and it is a tie, not a cost: make the g-side
 * load of `g->f6` / `g->fa` precede the u-side load WITHOUT making it the add's
 * first operand.  The operand swap does both at once and the int carrier costs
 * an instruction, so the remaining idea is a spelling that lowers the g-side
 * load's LUID while leaving the PLUS's operand order alone -- or emptying the
 * ready list at that point, which is what the anchor axis reaches and what
 * batch 342's Func_80a7440 turned out to need.  (d) is a separate tie with the
 * call's argument setup and may well fall out with it.
 */
struct Gain {
    short f0;
    unsigned short f2;
    unsigned short f4;
    unsigned short f6;
    unsigned short f8;
    unsigned short fa;
    unsigned short fc;
    unsigned short fe;
};

struct Base {
    unsigned char pad00[0x50];
    short hp[6];
    short pp[6];
    unsigned short atk[6];
    unsigned short def[6];
    unsigned short agi[6];
    unsigned char lck[6];
};

struct Scratch {
    int f0;
    int f4;
    struct Base *f8;
    unsigned char pad0c[0x2c - 0x0c];
};

extern unsigned char *GetUnit(int id);
extern void *Func_8004970(int size);
extern void free(void *p);
extern struct Base *GetPCBaseStats(unsigned int idx);
extern int Func_8079008(int id, int amount);
extern int Random(void);
extern void Func_8078bf0(int id);
extern void CalcStats(int id);

struct Gain *Func_807905c(int id, struct Gain *g)
{
    unsigned char *u;
    struct Scratch *s;
    struct Base *b;
    unsigned int *p;
    int lvl;
    int r;
    short k;
    int m;
    int d;
    int n;

    u = GetUnit(id);
    s = Func_8004970(0x2c);
    s->f0 = u[0x129];
    lvl = u[0xf];
    s->f4 = lvl;
    g->f0 = lvl;
    g->f2 = 0xffff;
    g->f4 = 0;
    g->f6 = 0;
    g->f8 = 0;
    g->fa = 0;
    g->fc = 0;
    g->fe = 0;
    if (lvl <= 0x62) {
        u[0xf] = u[0xf] + 1;
        g->f0 = lvl + 1;
        r = Func_8079008(id, u[0xf]);
        if (r != -1) {
            p = (unsigned int *)(u + 0x124);
            if (*p < (unsigned int)r)
                *p = r;
        }
        b = GetPCBaseStats(id);
        s->f8 = b;
        if (g->f0 == 1) {
            g->f4 += b->hp[0];
            g->f6 += b->pp[0];
            g->f8 += b->atk[0];
            g->fa += b->def[0];
            g->fc += b->agi[0];
            g->fe += b->lck[0];
        }
        k = g->f0 / 20;
        if (k < 0)
            k = 0;
        if (k > 4)
            k = 4;
        d = s->f8->hp[k + 1] - s->f8->hp[k];
        g->f4 += (unsigned int)(((unsigned int)(Random() * 20) >> 16) + d) / 20;
        d = s->f8->pp[k + 1] - s->f8->pp[k];
        g->f6 += (unsigned int)(((unsigned int)(Random() * 20) >> 16) + d) / 20;
        d = s->f8->atk[k + 1] - s->f8->atk[k];
        g->f8 += (unsigned int)(((unsigned int)(Random() * 20) >> 16) + d) / 20;
        d = s->f8->def[k + 1] - s->f8->def[k];
        g->fa += (unsigned int)(((unsigned int)(Random() * 20) >> 16) + d) / 20;
        d = s->f8->agi[k + 1] - s->f8->agi[k];
        g->fc += (unsigned int)(((unsigned int)(Random() * 20) >> 16) + d) / 20;
        d = s->f8->lck[k + 1] - s->f8->lck[k];
        n = (unsigned int)(((unsigned int)(Random() * 20) >> 16) + d) / 20;
        m = g->fe + n;
        *(unsigned short *)(u + 0x10) += g->f4;
        *(unsigned short *)(u + 0x12) += g->f6;
        *(unsigned short *)(u + 0x18) += g->f8;
        *(unsigned short *)(u + 0x1a) += g->fa;
        *(unsigned short *)(u + 0x1c) += g->fc;
        u[0x1e] += m;
        u[0x1f] = 1;
        u[0x20] = 0;
        g->fe = m;
        u[0x21] = 0;
        Func_8078bf0(id);
        CalcStats(id);
    }
    free(s);
    return g;
}

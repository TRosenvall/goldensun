/* Func_807905c (ApplyLevelUp, 0x0807905c) -- NON-MATCHING, 38 of 288 encodings differ.
 * COUNT DOES NOT MATCH: ref 286, ours 288, size 616 against 620. So the 38 is
 * NOT a distance -- read it as "the stream aligns except for five instructions".
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/807905c.c \
 *     asm/rom_77000/rom_79008_a.s --func Func_807905c
 *
 * SPLIT SHAPE. asm/rom_77000/rom_79008_a.s holds TWO functions, Func_8079008
 * (CanLevelUp, 40 instructions, also unattempted) and Func_807905c. Converting
 * this one needs
 *     asm/rom_77000/rom_79008_a.s -> rom_79008_a_a.s  (Func_8079008, stays asm)
 *                                  + rom_79008_a_b.c  (Func_807905c)
 * with the two stems in place of the one in stage1.ld. EXPORTS: Func_807905c
 * only; it calls its own file-mate Func_8079008, which must stay `.global`.
 * `python3 tools/datacheck.py asm/rom_77000/rom_79008_a.s` is CLEAN -- no data
 * section rides along, so the split is text-only.
 *
 * RELOCATION SEQUENCE IS EXACT from the first draft on: GetUnit, Func_8004970,
 * Func_8079008, GetPCBaseStats, __divsi3, then six Random/__udivsi3 pairs, then
 * Func_8078bf0, CalcStats, free. Twenty relocations, same symbols, same order.
 * Only the offsets differ, and only because of the five instructions below.
 *
 * ============================================================
 * WHAT THE FUNCTION IS
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
 * FIVE THINGS THAT ARE LOAD-BEARING, each measured alone
 * ============================================================
 * 1. THE CURVE LOOKUP MUST BE A STRUCT ARRAY INDEX, NOT BYTE ARITHMETIC.
 *    `*(short *)(b + i2 + 0x52)` makes gcc fold `b + i2` into the address and
 *    then materialise a zero register for the `ldrsh`, which has no immediate-
 *    offset form in Thumb -- two extra instructions per site, five sites:
 *        ours  add r3, r0, r5 / mov r2, r3 / add r2, #0x52 / add r3, #0x50
 *              / mov r1, #0 / ldrsh r2, [r2, r1] / mov r1, #0 / ldrsh r3, [r3, r1]
 *        rom   mov r3, r8 / add r3, #0x52 / ldrsh r2, [r1, r3] / sub r3, #0x2
 *              / ldrsh r3, [r1, r3]
 *    `b->hp[k + 1] - b->hp[k]` on a `struct Base { ... short hp[6]; ... }` gives
 *    the ROM's form, with `k * 2` hoisted to its own pseudo (the ROM's r8) and
 *    ONE offset register walked back by `sub r3, #2`. 302 instructions -> 286.
 *
 * 2. THE FIRST TWO CURVES ARE `short`, THE NEXT THREE `unsigned short`, THE
 *    LAST `unsigned char`. The ROM reads +0x50 and +0x5c with `ldrsh` and
 *    +0x68/+0x74/+0x80 with `ldrh` in the SAME expression shape, which is a
 *    signedness fact, not a scheduling one. The `g->f0 == 1` arm reads all six
 *    with `ldrh`/`ldrb` because the result is truncated to a halfword there --
 *    combine turns the sign-extending load into a zero-extending one when only
 *    the low 16 bits survive, so element 0 needs no separate type.
 *
 * 3. THE PCStats POINTER MUST BE RE-READ FROM THE SCRATCH AT EVERY SITE, INLINE.
 *    `s->f8->hp[k + 1] - s->f8->hp[k]` gives the ROM's `mov r3, r9 / ldr rX,
 *    [r3, #8]` per block. Hoisting it into one `b = s->f8;` variable assigned
 *    six times gives that variable ONE hard register for all six blocks (r0 in
 *    every one) where the ROM uses r1 in the first and r2 in the rest: 220
 *    differing against 74. Six separate block-scoped locals measure exactly the
 *    same as the inline form -- gcc merges them -- so the lever is "no variable",
 *    not "a fresh variable".
 *
 * 4. THE BRACKET INDEX k IS A `short`. The ROM truncates the division result
 *    back to 16 bits (`lsl r0,#16 / asr r5,r0,#16`) before clamping, and reads
 *    it unextended as the byte-curve index later. `int k` loses the pair.
 *
 * 5. THE LAST GAIN IS HELD IN A TEMPORARY WHOSE STORE TO `g->fe` COMES AFTER
 *    THE UNIT UPDATES. The ROM computes `ldrh r1,[r6,#0xe] / add r1, r0` once
 *    and spends it twice -- into `u[0x1e]` and, at the very end, back into
 *    `g->fe`. Writing `g->fe += n;` first forces a RELOAD, because the five
 *    `*(unsigned short *)(u + 0x10..0x1c) +=` stores in between have the same
 *    alias set as `g->fe` (c-common.c's lang_get_alias_set maps signed and
 *    unsigned variants of a type to ONE set, so no signedness game separates
 *    them). That reload is `ldrb r2,[r6,#0xe]`, and it is one of the two extra
 *    instructions when the temporary is omitted.
 *
 * ============================================================
 * THE RESIDUE: 38 encodings in FOUR places, 2 of them a real excess
 * ============================================================
 * (a) 2 instructions of EXCESS -- `lsl r1, #0x10 / asr r1, #0x10`, the widening
 *     of the tail temporary before `add r3, r1`. It is there because the
 *     temporary is `unsigned short`; the ROM needs no widening at all, because
 *     its temporary is an `int` fed by a zero-extending `ldrh`.
 *
 *     AND THAT IS THE WHOLE BLOCKER, because `int` costs more than it saves:
 *         unsigned short m   38 differing, 288 instructions  <- this file
 *         int m             214 differing, 290 instructions
 *     With `int m` the stream shape is otherwise IDENTICAL (one insert/delete
 *     pair in 290), but the ALLOCATION rotates wholesale -- g r6->r7, unit
 *     r10->r8, the temporary into r5 -- and pays four extra `mov`s in the unit
 *     update block. The temporary is HImode-vs-SImode, one basic block wide, and
 *     that one bit decides whether it gets the ROM's call-used r1 or a
 *     callee-saved register. MEASURED and inert: nine declaration positions for
 *     `int m` (every slot in the local list) all give 214/290 -- DECLARATION
 *     ORDER DOES NOT REACH IT, which is worth knowing because stack-slot order
 *     usually does. `unsigned int` 214/290, `unsigned char` 217/292,
 *     `register int __asm__("r1")` 217/292, reusing `d` 214/290, reusing `lvl`
 *     218/290, reusing `r` 218/292, reusing the `short k` 58/288 (the sext moves
 *     but does not go away), `u[0x1e] += (unsigned char)m` inert at 38/288.
 *
 * (b) 3 encodings -- three loads scheduled one slot early:
 *     `ldrh r1,[r6,#0xe]`, `ldrh r3,[r0,#0x12]`, `ldrh r3,[r0,#0x1a]`.
 *
 * (c) the rest -- the base-pointer reload in blocks 2..6 sits one slot earlier
 *     than the ROM's, which forces a different reload register:
 *         rom   ldrh r3,[r6,#N] / add r3,r0 / mov r0,r9 / ldr r2,[r0,#8] / strh
 *         ours  ldrh r3,[r6,#N] / mov r1,r9 / add r3,r0 / ldr r2,[r1,#8] / strh
 *     NOT an aliasing fact: the ROM itself hoists the load above the `strh` in
 *     blocks 4 and 6 and not in 2, 3 and 5, so sched2 is choosing freely in both
 *     streams. `do { ... } while (0)` around each block (batch 295's sched2
 *     barrier) measures 217/293 -- it separates them but at the cost of the
 *     allocation, so the barrier is not the tool here.
 *
 * ============================================================
 * THE COUNT-EQUAL ALTERNATIVE, kept on the record
 * ============================================================
 * Lever 1 + 2 + 4 with a single `b = s->f8;` variable and no tail temporary
 * measures 220 of 286 with SIZE AND COUNT BOTH EQUAL. That is the only spelling
 * found with a legal distance, and it is 220, so it is not useful as a starting
 * point -- but a successor testing a whole-function hypothesis should re-derive
 * it rather than trust 38.
 *
 * NEXT: the question is narrow and answerable. Get an `int` tail temporary into
 * r1 without disturbing g=r6 / unit=r10. The four extra `mov`s it costs are all
 * copies of `unit` out of r10 in the update block, so the probe is a spelling of
 * those five `*(unsigned short *)(u + N) +=` statements that needs one fewer
 * low-register copy of `unit` -- not another type for the temporary.
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
    unsigned short m;
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
        m = g->fe + (unsigned int)(((unsigned int)(Random() * 20) >> 16) + d) / 20;
        *(unsigned short *)(u + 0x10) += g->f4;
        *(unsigned short *)(u + 0x12) += g->f6;
        *(unsigned short *)(u + 0x18) += g->f8;
        *(unsigned short *)(u + 0x1a) += g->fa;
        *(unsigned short *)(u + 0x1c) += g->fc;
        u[0x1e] += m;
        u[0x1f] = 1;
        u[0x20] = 0;
        u[0x21] = 0;
        g->fe = m;
        Func_8078bf0(id);
        CalcStats(id);
    }
    free(s);
    return g;
}

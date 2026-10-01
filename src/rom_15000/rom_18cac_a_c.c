/* Func_8018efc -- LANDS.  BYTE-IDENTICAL: 0 of 119 encodings differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/rom_15000/rom_18cac_a_c.c (LANDED; was src/non_matching/rom_15000/8018efc.c) \
 *       asm/rom_15000/rom_18cac_a_c.s --func Func_8018efc
 * objcmp: "OK Func_8018efc -- 260 bytes, 119 encodings and 4 relocations identical".
 * Production flags, no per-file Makefile row, no extra switch.
 *
 * LANDING PREREQUISITES: none beyond the .c.
 *   * tools/datacheck.py on the reference prints NOTHING and exits 0 -- the file
 *     carries no data section, so NO text/data split is required.
 *   * the reference holds exactly ONE function (Func_8018efc), so no split_s.
 *   * tools/shimcount.py reports 0 pins -- NO fakematch.txt row.
 *   * the generated asm/rom_15000/rom_18cac_a_c.s belongs in the commit.
 *
 * WHAT CLOSED THE LAST 2, AND WHY THE OLD PARK COULD NOT FIND IT.  One token:
 *
 *     s->x = (win->x << 3) + ((unsigned short)(*(unsigned short *)&win->w - 2) << 3) + 4;
 *                                               ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 *
 * i.e. the `win->w` read at THIS ONE SITE goes through a pointer cast instead of
 * the member.  Three other spellings of the same escape are equally byte-exact
 * (all measured): `*(unsigned short *)((unsigned char *)win + 8)`,
 * `*(unsigned short *)(void *)&win->w`, and a declared `unsigned short *wp;`
 * with `wp = &win->w;` as its own statement then `*wp`.  The member spelling
 * `win->w` is the ONLY form that fails.
 *
 * *** THE PARK'S OWN DIAGNOSIS WAS WRONG, AND IT IS THE REASON 26 PROBES AND 11
 * *** FLAGS MISSED THIS.  The park called the residue "an exact sched2 priority
 * *** tie" between `ldrh r3,[r7,#8]` and `ldr r1,=0xfffe` and reasoned that only a
 * *** DAG-shape change could break it, while every DAG-shape change moved the size.
 * *** It is NOT a tie that falls to INSN_LUID.  Read off the .23.sched2 dump of
 * *** basic block 6 at production flags:
 * ***
 * ***     insn 119 = ldrh r3,[r7,#8]   prio 23   INSN_DEPEND {270, 123}        -> 2
 * ***     insn 416 = ldr r1,=0xfffe    prio 23   INSN_DEPEND {270, 150, 123}   -> 3
 * ***
 * *** Priority DOES tie at 23, and both ties are dominated by the same path
 * *** (cost 2 + prio(123)=21), so no reassociation can separate them -- which is
 * *** exactly why the park's 26 probes were inert.  But rank_for_schedule never
 * *** reaches INSN_LUID: the test BEFORE it, "prefer the insn which has more later
 * *** insns that depend on it", is 3 against 2 and the constant load wins outright.
 * ***
 * *** The asymmetry is structural and has nothing to do with the expression.  416
 * *** sets r1 and insn 123 (`add r3,r1`) only READS r1, so r1 is still live-dead at
 * *** 123 and the later `ldr r1,=0xfffffe00` (insn 150) takes an OUTPUT dependence
 * *** on 416 -- a third dependent.  119 sets r3 and 123 is `add r3,r1`, which
 * *** RE-WRITES r3, so the later `ldr r3,=0x1ff` hangs its output dependence on 123,
 * *** not on 119.  Two loads feeding one two-operand add, and the operand that is
 * *** also the destination loses a dependent.  Nothing in the arithmetic can change
 * *** that; only the dependence COUNT can be equalised.
 * ***
 * *** THE POINTER CAST EQUALISES IT.  `*(unsigned short *)&win->w` gives the load
 * *** an alias set that conflicts with the later struct stores in the block, so 119
 * *** picks up anti-dependences on them and reaches 3 dependents.  At 3 == 3 and
 * *** priority 23 == 23, rank_for_schedule finally does fall to INSN_LUID, and
 * *** LUID(119) < LUID(416) puts the `ldrh` first -- the ROM's order.
 * ***
 * *** -fno-strict-aliasing CONFIRMS THE MECHANISM AND IS NOT THE FIX.  With the
 * *** flag, 119's dependents go to EIGHT ({270, 262, 245, 241, 222, 203, 160, 123})
 * *** and indices 45/46 come out exactly right -- but the SAME flag also makes the
 * *** `win->h` and `win->y` ldrb reads, which sit AFTER the `s->x` store in source
 * *** order, conflict with it, so they are forced below the store and five
 * *** encodings (58-62) break instead.  The flag is measured at 5 differing; the
 * *** escape has to be local to the ONE load whose dependent count is short, and
 * *** the member spelling of every OTHER access has to stay.
 *
 * MEASURED INERT, so the union lever does NOT substitute for the pointer cast:
 *   * `union HW { unsigned short v; };` with `((union HW *)&win->w)->v` is
 *     byte-identical to no change at all -- still 2 at indices 45/46.  This is a
 *     SECOND independent confirmation of the finding in
 *     src/rom_15000/rom_1aeec_a_a_c_a_c_c_c_b.c (LANDED; was src/non_matching/rom_15000/801b664.c)'s park, that a one-member union
 *     pointer-cast does not reach a MEM's alias set in this compiler.  Use the
 *     plain pointer cast.
 *
 * STILL TRUE FROM THE OLD PARK, keep these (each re-verified by the landing):
 *   1. `base` AND `q` ARE ONE VARIABLE.  The ROM's `add r6, r2` advances the base
 *      pointer in place; writing two pointers costs 14 encodings.
 *   2. A SEPARATE INDEX LOCAL FOR THE HALFWORD STORE -- `idx = pos * 2;` then
 *      `*(unsigned short *)(p + idx)` -- for `strh r1, [r5, r2]`.  The array form
 *      inverts the base/index order; the pointer spelling alone is not enough.
 *   3. DISTRIBUTE THE SHIFT BY HAND.  `(win->x << 3) + ((unsigned short)(win->w - 2) << 3) + 4`.
 *      The ROM emits one `lsl #3` because combine folds the two back together;
 *      the undistributed source loses the range fold and reads 82.
 *   4. `(unsigned char)(win->h - 2)` for the y term, against `(unsigned short)`
 *      for the x term.  Same shape, different widths.
 *
 * MEASURED WORSE, do not re-try: -fno-schedule-insns2 45, -fno-strict-aliasing 5,
 * -fno-expensive-optimizations 88, `* 8` for `<< 3` 84, the undistributed forms
 * 59-82, `slot` computed after the AllocSpriteSlot if 13.
 *
 * ONE NOTE RETIRED: the old park's "-fno-schedule-insns (sched1) inert" line ruled
 * nothing out -- sched1 does not run in this build.  Only sched2 does, and it is
 * sched2's dependent-count test, not its LUID tie-break, that mattered here.
 */
struct Spr {
    unsigned int a;
    unsigned char y;
    unsigned char b;
    unsigned short x : 9;
    unsigned short rest : 7;
};

struct Node {
    unsigned int next;
    unsigned char f4;
    unsigned char f5;
    unsigned short x;
    unsigned short y;
    unsigned char pad[4];
    unsigned char slot;
    unsigned char fF;
    struct Spr spr;
    unsigned int pad2;
};

typedef unsigned char B7[7];

struct Win {
    unsigned char pad[8];
    unsigned short w;
    unsigned short h;
    unsigned short x;
    unsigned short y;
};

extern unsigned char *iwram_3001e8c;
extern struct Node *Func_8015e8c(void);
extern int AllocSpriteSlot(void);
extern int Func_8016584(struct Win *, struct Node *);

#define n ((struct Node *)p)
void Func_8018efc(struct Win *win, unsigned int ch, unsigned int x, unsigned int y, int mode)
{
    unsigned char *p;
    struct Spr *s;
    unsigned short *q;
    int slot;
    unsigned int pos;
    unsigned int idx;

    p = iwram_3001e8c;
    q = (unsigned short *)p;
    if (y > win->h - 2)
        return;
    if (x > win->w - 2)
        return;
    if (mode == 1) {
        p = (unsigned char *)Func_8015e8c();
        if (p == 0)
            return;
        slot = (B7 *)n - (B7 *)((unsigned char *)q + 0x698);
        n->f5 = 2;
        q += 0x95b;
        s = &n->spr;
        if (*q == 0x63)
            *q = AllocSpriteSlot();
        s->x = (win->x << 3) + ((unsigned short)(*(unsigned short *)&win->w - 2) << 3) + 4;
        s->y = (win->y << 3) + ((unsigned char)(win->h - 2) << 3) - 1;
        n->x = s->x;
        n->y = s->y;
        n->next = 0;
        n->slot = slot;
        if (n->f5 == 0)
            n->f5 = mode;
        Func_8016584(win, n);
    } else {
        if (ch > 0xff)
            return;
        y++;
        x++;
        pos = ((win->y + y) << 5) + (win->x + x);
        if (pos >= 0x280)
            return;
        idx = pos * 2;
        *(unsigned short *)(p + idx) = ch | 0xf000;
    }
}

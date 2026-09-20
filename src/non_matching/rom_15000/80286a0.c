/* Func_80286a0 -- 0x080286a0, asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c.s (2 functions; the
 * sibling Func_8028574 stays in assembly, so landing needs a two-way split).
 *
 * NOT MATCHING: 4 differing of 85 encodings, SIZE IDENTICAL -- and only THREE are real.
 * Candidate below.
 *
 * THE FOURTH IS A PHANTOM: `ldr r3, =0x1f` against our `ldr r3, =_CONST_1f`, a pool word plus a
 * relocation. `_CONST_1f` ALREADY EXISTS in const.sym, so NO .sym edit is needed -- this is the
 * documented const.sym phantom-relocation class where objcmp cannot return OK and `make compare`
 * is the only authority.
 *
 * THE THREE REAL ONES ARE A ROTATION IN THE LOOP PREHEADER, and it is priced out. From .23.sched2
 * the block is `{38: r8=r0 prio 1, 252: r3=0x92 prio 3, 41: r3=r3+r6 prio 2, 255: fp=r3 prio 1,
 * 43: jump}`. rank_for_schedule is strictly priority-first, so `r8 = r0` cannot issue at t=0
 * unless its priority reaches 3 -- and `cur`'s initialiser has no in-block successor other than
 * the jump, so there is nothing for it to inherit from. Ties break on ascending LUID, confirmed
 * both ways: `cur = start` before `m = ...` puts it third, after puts it fourth.
 *
 * THREE LEVERS LANDED, 67 differing to 33 to 7 to 4:
 *
 * 1. A `goto` INTO A `do/while` IS THE LOOP FORM.
 *
 *        goto entry; do { update; entry: body; } while (cur != target);
 *
 *    The ROM's `b ENTRY / TOP: update / ENTRY: body / bne TOP` is NOT reachable from
 *    `while (1) { body; if (x) break; update; }`, which lays the body first. 67 to 33.
 *    `for (;;)`-with-`break` and `while (cur != target)` wrapped the same way are BYTE-IDENTICAL
 *    to the do/while, so only the `goto` matters -- the loop keyword does not.
 * 2. `if (d >= 0) j = d; else j = target - *c;` -- 33 to 7. The plain `j = d; if (d < 0) j = ...;`
 *    lets gcc COALESCE `j` with `d` and loses the ROM's `mov r2, r3`; the explicit if/else keeps
 *    it. The inverted `if (d < 0) ... else j = d;` is 18. Declaring `j` first, `unsigned j`, and
 *    moving a load between are all INERT at 33.
 * 3. `step = 1; extra = 0xc;` BEFORE `c = u + 0x8c;` -- 7 to 4, fixing the second rotation.
 *
 * INERT: -fno-gcse, -fno-rerun-cse-after-loop. WORSE: -fno-schedule-insns2 at 16.
 * `cur = start` in both if-arms (hoping for cross-jumping) is 89 lines and 38.
 */
struct Ui {
    unsigned char pad0[0x78];
    void *box;
};

extern unsigned char *iwram_3001f38;
extern unsigned char L373ef[] __asm__(".L373ef");
extern int _CONST_1f;

extern void Func_8016478(void *w);
extern void Func_801e7c0(int id, void *w, int a, int y);
extern void WaitFrames(int n);
extern void _PlaySound(int id);

int Func_80286a0(int start, int target)
{
    struct Ui *u;
    short *c;
    short *m;
    unsigned char *t;
    int cur;
    int step;
    int extra;
    int id;
    int k;
    int d;
    int j;

    u = (struct Ui *)iwram_3001f38;
    step = 1;
    extra = 0xc;
    c = (short *)((unsigned char *)u + 0x8c);
    *c = start;
    if (target < start)
        step = -1;
    cur = start;
    m = (short *)((unsigned char *)u + 0x92);
    goto entry;
    do {
        *c += step;
        _PlaySound(0x6f);
        extra = 0;
        cur += step;
    entry:
        Func_8016478(u->box);
        if (*m != 0) {
            id = *m + *c;
        } else {
            k = *c + 0x84;
            id = *((unsigned char *)u + k) + (int)&_CONST_1f;
        }
        Func_801e7c0(id, u->box, 0, 0);
        t = L373ef;
        d = *c - target;
        if (d >= 0)
            j = d;
        else
            j = target - *c;
        WaitFrames(t[j] + extra);
    } while (cur != target);
    WaitFrames(0x30);
    _PlaySound(0x70);
    return target;
}

/* Func_808ae74 -- 0x0808ae74 (PlaySurfaceStepEffect).  PARKED at 116 of 200.
 * ref: asm/rom_8a000/rom_8ace0_a_a_a_c.s  (ONE function, NO data section --
 *      grep -ci func_start = 1; converts WHOLE FILE, no split needed)
 *
 * NON-MATCHING: 116 encodings of 200 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808ae74.c \
 *     asm/rom_8a000/rom_8ace0_a_a_a_c.s --whole
 *
 * IT IS A TRUE DISTANCE, but only just: ref 440 bytes / 200 encodings, ours
 * 440 bytes / 200 encodings.  The control flow is right everywhere, both
 * indirect-call shapes are right, the table stride and both selection loops are
 * right.  What is left is REGISTER AND SCRATCH-REGISTER SELECTION, cascading
 * from one decision, and 116 is therefore a count of knock-ons rather than of
 * causes -- the distinct causes are four.
 *
 * NO SHIMS in this draft.
 *
 * ------------------------------------------------------- THE FIRST CAUSE ----
 * THE ROM SPILLS BOTH PARAMETERS AND WE SPILL ONLY ONE.
 *     rom    sub sp, #8 / str r0, [sp, #4] / str r1, [sp]     ... 4 high regs
 *     ours   sub sp, #4 / str r1, [sp]     / mov r11, r0
 * `a` is live from the entry to the very last statement (`Func_808b320(a)`),
 * so it needs a call-safe home; the ROM gives it a stack slot and we give it
 * r11.  Everything downstream -- e in r10 instead of r9, ap in r9, the three
 * `ldr rN, [sp, #4]` reloads becoming `mov r2, r11`, and the r1/r2/r3 rotation
 * on every scratch afterwards -- follows from that one register being spent.
 * The ROM's live set at the Random block is m, e, ap, w, q, v2, v3 in
 * r5-r11 with a and b in memory; ours is the same seven values plus `a`, so
 * gcc has one register in hand where the ROM had none.
 *
 * `volatile` ON THE PARAMETERS IS NOT THE CURE, MEASURED: `volatile int a` is
 * 193 of 200 at 206 encodings and `volatile int a, volatile int b` is 198 of
 * 200 at 210 -- both add a re-read at every use, which the ROM does not have
 * (it reloads `a` three times, which is exactly a SPILL, not a volatile).  The
 * open question for a future round is what eighth call-crossing value the
 * original had, or which statement order makes global-alloc run out.  Note the
 * ROM pushes ALL FOUR high registers, so it is not short of them by accident.
 *
 * ---------------------------------------------- THE OTHER THREE CAUSES ------
 * 2. A SHARED `mov r0, #0 / b <exit>` BLOCK.  The ROM reaches `.L8af26` from
 *    four places (both early GetFlag guards, the GetUnit level check, and the
 *    fallthrough of the gState+0x244 test) and materialises the zero there
 *    once; we hoist a zero above the GetUnit compare and grow an extra
 *    `b <exit>`.  Writing the two GetFlag guards as one `||`, and the GetFlag
 *    (5) / GetUnit pair as one `&&`, are both INERT (115 each) -- this follows
 *    the allocation above, not the guard shape.
 * 3. `bl _call_via_r2` where the ROM has `bl _call_via_r3`.  The function
 *    pointer landed one register lower.  Same root cause.
 * 4. Scratch rotation: `ldr r2, =0xfffff`-class reloads go to r1 for us and to
 *    r2/r3 in the ROM (allocate_reload_reg / last_spill_reg, reload1.c:5003).
 *
 * ------------------------------------- THE LEVERS ALREADY IN THIS DRAFT -----
 * a) TWO DIFFERENT INDIRECT-CALL SPELLINGS IN ONE FUNCTION, and the ROM says
 *    which is which.  `ldr r3, =Func_80008ac / bl _call_via_r3` is an ordinary
 *    call through a POINTER LOCAL -- `divide = Func_80008ac; divide(x, y);`,
 *    the idiom src/rom_f4000/rom_f4008_a_a_c.c uses for the same callee.
 *    `ldr r3, =Func_8000888 / .call_via r3` (which assembles to
 *    `mov r12, pc / bx r3`) is the inline-asm helper, copied verbatim from
 *    src/overlays/rom_7b9cb4/ovl_30_c_c_a_a.c's `call_via`.  Do not try to
 *    reach the second with a pointer local or the first with the helper.
 * b) THE FOUR Random() RESULTS MUST BE COMBINED IN SEPARATE DESTRUCTIVE
 *    STATEMENTS.  `q = q - v2 + v3 - Random();` as one expression gives
 *    three-operand `sub rD, rA, rB` and 201 encodings against 200; splitting it
 *    into `q -= v2; q += v3; q -= Random();` gives the ROM's two-operand
 *    `sub r5, r2` chain and fixes the length.  [201 -> 200, 115 -> 116 --
 *    the count went UP by one while the LENGTH became right, which is the
 *    "a count is not a distance" case: take the length.]
 * c) A LOCAL `unsigned char *` BASE PER gState REGION.  The ROM loads the
 *    symbol three times (0x24c, 0x244, 0x238) and each `+ K` is real
 *    arithmetic; a direct `gState + 0x244` folds to `ldr r3, =gState+580`.
 *    All three locals are needed -- dropping the 0x244 one is 162 of 200 at
 *    198 encodings, dropping the 0x238 one leaves `ldr r2, =gState+568`.
 * d) THE TABLE ENTRY IS 28 BYTES and `&L9c610[a]` produces the ROM's
 *    `lsl r3, r2, #3 / sub r3, r2 / lsl r3, #2` synth_mult chain unaided.
 *    The layout is read off the accesses: two halfwords, then eight halfwords
 *    at +4, then eight bytes at +0x14.
 * e) `e` IS READ ON A PATH THAT NEVER SETS IT.  When GetFlag(0x15f) is set the
 *    ROM jumps straight to the tail, which uses r9 (`e`) -- an uninitialised
 *    read in the original.  The C is written the same way on purpose; do not
 *    "fix" it.
 * f) THE 8-ENTRY WEIGHT SUM IS WRITTEN COUNTING UP (`for (j = 0; j < 8; j++)`
 *    over a walking pointer) even though the ROM counts r1 down from 7 --
 *    check_dbra_loop reverses it.  Counting down in the source measures the
 *    same here (115), so this one is a preference, not a lever.
 *
 * -- worked in scratch_elev/b292/A
 */
struct Surf {
    unsigned short f00;
    unsigned short f02;
    unsigned short snd[8];
    unsigned char wt[8];
};

extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern struct Surf L9c610[] __asm__(".L9c610");
extern int _GetFlag(int id);
extern unsigned char *_GetUnit(int n);
extern int _Func_8077348(void);
extern unsigned int Random(void);
extern void Func_808b320(int a);
extern int Func_80008ac(int a, int b);
extern int Func_8000888(int a, int b);

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
        : "memory", "r12", "r2"
    );
    return _a;
}

int Func_808ae74(int a, int b)
{
    unsigned char *m;
    unsigned char *s;
    unsigned char *s2;
    unsigned char *s3;
    unsigned char *u;
    unsigned char *wp;
    struct Surf *e;
    int (*divide)(int, int);
    int *ap;
    int *acc;
    int w;
    int k;
    int q;
    int v2;
    int v3;
    int sum;
    int i;
    int j;
    int r;

    m = iwram_3001ebc;
    if (_GetFlag(0x15f) == 0) {
        if (_GetFlag(0xb0 << 1) != 0)
            return 0;
        if (_GetFlag(0x161) != 0)
            return 0;
        if (a == 0)
            return 0;
        s = gState;
        if (*(short *)(s + 0x24c) != 0)
            return 0;
        e = &L9c610[a];
        w = e->f00;
        if (w == 0)
            return 0;
        if (_GetFlag(5) != 0) {
            u = _GetUnit(5);
            if (*(int *)(u + 0x124) > 0x82)
                return 0;
        }
        k = _Func_8077348() - e->f02;
        if (k < 0)
            k = 0;
        if (k > 5)
            k = 5;
        s2 = gState;
        if (k > 0 && *(int *)(s2 + 0x244) != 0)
            return 0;
        w += k * 5;
        ap = (int *)(m + 0x1a8);
        q = *ap;
        if (q == 0) {
            q = Random();
            v2 = Random();
            v3 = Random();
            q -= v2;
            q += v3;
            q -= Random();
            q = q / 2;
            *ap = q;
        }
        divide = Func_80008ac;
        r = divide((w << 20) + (w * 16 - 16) * q, 0x80 << 13);
        r = call_via(Func_8000888, r, b);
        s3 = gState;
        acc = (int *)(s3 + 0x238);
        *acc += r;
        if (*acc < *(int *)(m + 0x1ac))
            return 0;
    }
    *(int *)(m + 0x1a8) = 0;
    wp = e->wt;
    sum = 0;
    for (j = 0; j < 8; j++)
        sum += *wp++;
    if (sum == 0)
        return 0;
    k = sum * Random() >> 16;
    k -= e->wt[0];
    i = 0;
    if (k >= 0) {
        wp = e->wt;
        do {
            i++;
            if (i > 7)
                break;
            wp++;
            k -= *wp;
        } while (k >= 0);
    }
    sum = e->snd[i];
    Func_808b320(a);
    return sum;
}

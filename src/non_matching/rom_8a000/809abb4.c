/* ===================== BATCH 297a DELTA -- Field_Halt =====================
 * RE-MEASURED: still 2 of 191 (ref 191 enc / 444 bytes / 30 rel, ours the same),
 * index 97: ref 2f0b `cmp r7, #0xb` against ours 2f0a `cmp r7, #0xa`, plus the
 * branch.  The park's conclusion stands.  TWO THINGS ARE NEW.
 *
 * 1. THE BLOCKER IS NOT "A SEVENTH CALL-CROSSING ALLOCNO IS INEVITABLE".  IT IS
 *    global-alloc CHOOSING TO ALLOCATE THE BOUND, and the dump says so.  With
 *    `for (; i < n; i++)` and `n = 11`, x.c.17.lreg carries
 *        (insn 37 ...) (set (reg/v:SI 39) (const_int 11 [0xb]))
 *            (expr_list:REG_EQUIV (const_int 11 [0xb]) (nil))
 *    -- i.e. the bound ALREADY HAS A REG_EQUIV CONSTANT NOTE, which is exactly
 *    what reload needs to rematerialise `#0xb` into the compare and keep
 *    combine's un-canonicalised `blt`.  It does not, because .18.greg hands the
 *    pseudo a hard register first:
 *        (insn 37 ...) (set (reg/v:SI 11 fp) (reg:SI 2 r2))
 *            (expr_list:REG_EQUIV (const_int 11 [0xb]) (nil))
 *        (jump_insn 101 ...) if_then_else (lt (reg/v:SI 7 r7) (reg/v:SI 11 fp))
 *    reg_equiv_constant is only consulted for a pseudo that got NO hard register,
 *    so the escape is not "find a spelling with six allocnos" but "make greg
 *    decline this one".  fp (r11) is free in thumb at -O2, so there is no
 *    pressure to make it decline, and an eighth allocno would have to be real
 *    code.  That is a sharper and more checkable statement of the floor.
 *
 * 2. THE BOUND PLACEMENTS RE-MEASURED, confirming the park's numbers exactly:
 *        bound literal (this file)                     2  (191 enc, 444 bytes)
 *        const int n = 11 (folded away)                2  (191 enc)
 *        n = 11 where this file puts it               179  (195 enc, 452 bytes)
 *        n = 11 as the first statement                178  (195 enc)
 *        n = 11 after the `if (p == 0) return`        180  (195 enc)
 *        n = 11 immediately before the loop           180  (195 enc)
 *        unsigned n, `i < (int)n`                     179  (195 enc)
 *    NOTE the CFG fact the park did not record: the whole body after
 *    `if (p == 0) return;` is at a LABEL -- the ROM emits `cmp r6,#0 / bne
 *    .L9abea / b .L9ad52` -- so the loop IS in a jumped-to block and cse1
 *    genuinely cannot fold a bound assigned before that branch.  That is why
 *    naming the bound produces `blt` at all; the cost is purely greg's.
 *
 * 3. FLAG SWEEP, 12 toggles, none reaches index 97.  Still 2 with
 *    -fno-cse-follow-jumps, -fno-expensive-optimizations, -fno-thread-jumps,
 *    -fno-caller-saves, -fno-move-all-movables, -fno-reduce-all-givs,
 *    -fno-unroll-loops.  WORSE: -fno-gcse 98 (189 enc), -fno-rerun-cse-after-loop
 *    62, -fno-schedule-insns2 59, -fno-strength-reduce 46 (189 enc),
 *    -fno-peephole 71.  (-fno-if-conversion is not an option this cc1 accepts.)
 * -- scratch_elev/b297a/t2
 */

/* Field_Halt -- 0x0809abb4.  PARKED at 2 of 191.
 * ref: asm/rom_8a000/rom_9a44c_c_c_c.s
 *
 * NON-MATCHING: 2 encodings of 191 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/809abb4.c \
 *     asm/rom_8a000/rom_9a44c_c_c_c.s --func Field_Halt
 *
 * IT IS A TRUE DISTANCE: ref 444 bytes / 191 encodings, ours 444 bytes / 191
 * encodings, and the two differing encodings are ADJACENT and are the same
 * instruction pair.  Everything else -- prologue push list, both loops, the
 * frame, every call and every constant -- is byte-identical.
 *
 * batch 292, brief A, target 2.  NO SHIMS in this draft: no pins, no barriers,
 * no volatile, default flags.  One union, for the same alias-set reason as its
 * twin (see below); it is inert here and could be dropped, but is kept so the
 * two files read alike.
 *
 * FILE SHAPE / SPLIT.  rom_9a44c_c_c_c.s is `grep -ci func_start` = 1 plus a
 * .rodata tail:
 *      .section .rodata
 *      .global .La012c
 *      .La012c:  .incrom 0xa012c, 0xa0138
 * Field_Halt itself references NO data label (checked instruction by
 * instruction), so the split is TEXT-ONLY: cut the function out, leave the
 * 12-byte .rodata piece in the residual `_b.s`.  `.La012c` is ALREADY
 * `.global` and is already reached from C by
 * src/rom_8a000/rom_9a44c_a_a_a_c.c as
 *      extern struct Script *La012c[] __asm__(".La012c");
 * so NO new export and NO linker alias is needed for the split.
 *
 * ------------------------------------------------------------ THE BLOCKER ---
 * The residue is loop 1's exit test:
 *      ROM   cmp r7, #0xb / blt   <loop top>
 *      ours  cmp r7, #0xa / ble   <loop top>
 *
 * This is combine.c's `simplify_comparison`, gcc-2.96 lines 10138-10150:
 *      case LT:   if (const_op > 0) { const_op -= 1; code = LE; }
 * applied UNCONDITIONALLY for MODE_INT, and reached from
 * `combine_simplify_rtx` (combine.c:4339) for ANY comparison rtx combine
 * visits.  So with a literal bound gcc-2.96 CANNOT emit `cmp #K / blt` for
 * K > 0.  Eleven literal spellings were compiled and every one measures
 * exactly 2:  `i < 11`, `i <= 10`, `11 > i`, `i < 11L`, `i < (int)11`,
 * `(i | 0) < 11`, `(i < 11) != 0`, `(i < 11) ? 1 : 0`, `!(i >= 11)`,
 * a `while` with the bump in the body, and a `do/while`.  (`i - 11 < 0` and
 * `!(i - 11 >= 0)` are WORSE -- 87-90 differing, 193 insns.)
 *
 * THE ONE ESCAPE IS A REGISTER-RESIDENT BOUND, AND IT COSTS MORE THAN IT BUYS.
 * docs/elevation.md's "CORRECTION: `cmp rN, #K / bge` with K > 0 IS reachable"
 * says to name the bound, and naming it DOES produce `blt`.  But `n` is then a
 * seventh call-crossing allocno: the ROM's push list is exactly
 * {r5,r6,r7} + {r8,r9,r10} = six (t/ep, p, i/v, to/v, o, from/k), and a named
 * bound takes r11, adds `mov r7, r8 / push {r7}` to the prologue and its mirror
 * to the epilogue, and measures 195 encodings against 191 with 178 differing.
 * Assigning `n` at four different points (first statement, before `i = 0`,
 * immediately before the loop, inside the loop) is 178-180 every time.
 *
 * So the two instructions are a HARD FLOOR unless a spelling is found that
 * gives gcc a sixth-or-fewer allocno set WITH a live bound.  Reusing an
 * existing local as the bound cannot work: the only candidate with a disjoint
 * live range is `t`/`ep` in r5, and the ROM CLOBBERS r5 inside loop 1
 * (`ldr r5, [r0]` / `add r5, r0` / `str r5, [r6, #8]`), so no register holds a
 * stable 11 across that loop in the ROM's own allocation.
 *
 * Corpus check: every `cmp rN, #K / blt` with K > 0 among the 19 sites in the
 * tree's generated `.s` is a `switch` dispatch chain, never a loop bound
 * (grep over asm/ (all banks) paired with an existing src/ (elevated)).  Two of those
 * exemplars are src/overlays/rom_78dee8/ovl_30_c_c_c_a_a.c and
 * src/rom_b5000/rom_bb588_c_c_b.c and both say the same thing in their headers.
 *
 * -------------------------------------- THE FOUR LEVERS THAT GOT IT TO TWO ---
 * 1. THE 16-ITERATION SECOND LOOP IS WRITTEN COUNTING UP.  The ROM counts DOWN
 *    (`mov r0,#1 / neg r0,r0 / add r10,r0 / cmp r0,#0 / bge`), and writing it
 *    down as `for (k = 0xf; k >= 0; k--)` measures 7; writing it UP as
 *    `for (k = 0; k < 16; k++)` and letting check_dbra_loop reverse it measures
 *    2.  That is docs/elevation.md's "a count-down loop whose counter is unused
 *    is written counting UP", confirmed: `k` appears nowhere in the body, only
 *    `ep += 0x48` walks.  The countdown spelling also mis-assigns the preheader
 *    registers (ROM `add r3, sp, #0x18` + `mov r0, #0xf`; the countdown gives
 *    `add r0, sp, #0x18` + `mov r3, #0xf`), which is the same r3-first
 *    REG_ALLOC_ORDER readout as its twin.  A `do/while` countdown is also 7.
 *    A named `int *vp = v;` for the vector is WORSE (187 insns, 90 differing) --
 *    it deletes the ROM's `mov r7, r8` second copy of the address.
 *
 * 2. THE 0xc000 MULTIPLIER IS A LOCAL ASSIGNED INSIDE THE LOOP.  Written
 *    inline (`w = i * (0xc0 << 8) / 10 + ...`) loop.c strength-reduces it:
 *    `.08.loop` says "Insn 238: giv reg 124 src reg 36 benefit 9 lifetime 1
 *    mult 49152" then "giv at 238 reduced", and the ROM's
 *    `mov r3,#0xc0 / lsl r3,#8 / mul r0,r3` becomes `add sl, sl, r0` on a new
 *    callee-saved register -- 195 encodings, 178 differing.  Assigning
 *    `c = 0xc0 << 8;` BEFORE the loop does not help (cse folds it back).
 *    Assigning it INSIDE the loop, immediately before the multiply, does: the
 *    multiplier is then not loop-invariant, no giv is formed, and the constant
 *    build stays in the loop exactly as the ROM has it.  Reusing `w` itself as
 *    the carrier brings the giv back (187 differing).
 *
 * 3. `(to[k] - from[k]) * i`, WITH THE COUNTER SECOND.  `i * (to - from)` gives
 *    `mov r0, r3 / mul r0, r0, r7` -- the destination tied to the delta -- and
 *    the ROM ties it to the counter (`mov r0, r7 / mul r0, r3`).  NOTE THIS IS
 *    THE OPPOSITE SOURCE ORDER FROM ITS TWIN Field_Whirlwind, where `i * delta`
 *    is what ties the destination to `i`.  The two functions have the same ROM
 *    shape for this multiply; what differs is that Whirlwind's counter lives in
 *    r8 (so a `mov r0, r8` is needed anyway) and Halt's lives in r7.  TRY BOTH
 *    ORDERS -- the mul lever has no fixed direction.
 *
 * 4. THREE SEPARATE int[3] ARRAYS in declaration order v, from, to, for the
 *    same sub sp, #0x24 / last-declared-lowest reason as the twin.
 *
 * ALSO MEASURED AND INERT: the union on the 0x68-equivalent store (there is no
 * such store here); `short`/`signed char` for `i` (193-195, worse); a `goto`
 * loop for loop 1 (197 insns -- it kills the giv but then synth_mult expands
 * the 0xc000 multiply into `lsl/add/lsl` and the bound takes fp anyway).
 *
 * -- worked in scratch_elev/b292/A
 */
union blob { unsigned char *pp; short hh; int ii; };

extern int *iwram_3001f30;
extern unsigned char *CreateParticleActor(int a, int b, int c, int d);
extern void Func_8097384(void);
extern void _Actor_SetAnim(unsigned char *p, int n);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern unsigned int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern void Func_80974d8(int *v);
extern void Func_809ba90(unsigned char *e, int a, int b, int c);
extern void Func_809ba7c(unsigned char *e, void (*f)(unsigned char *));
extern void _Sprite_SetColorswap(void *s, int n);
extern void _DeleteActor(unsigned char *p);
extern void Func_809748c(void);
extern void Func_809aa98(unsigned char *e);

void Field_Halt(void)
{
    int v[3];
    int from[3];
    int to[3];
    int *o;
    int *t;
    unsigned char *p;
    unsigned char *ep;
    int i;
    int k;
    int w;
    int n;
    int c;
    int *vp;

    o = iwram_3001f30;
    t = (int *)o[4];
    o[2] = t[3];
    p = CreateParticleActor(0xfa, 0, 0, 0);
    i = 0;
    n = 11;
    _Actor_SetAnim(p, 0);
    if (p == 0)
        return;
    Func_8097384();
    from[0] = t[2];
    from[1] = t[3] + (0x80 << 13);
    from[2] = t[4];
    to[0] = o[1];
    to[1] = o[2] + (0x80 << 12);
    to[2] = o[3];
    for (; i < 11; i++) {
        *(int *)(p + 8) = from[0] + (to[0] - from[0]) * i / 10;
        *(int *)(p + 0xc) = from[1] + (to[1] - from[1]) * i / 10;
        *(int *)(p + 0x10) = from[2] + (to[2] - from[2]) * i / 10;
        c = 0xc0 << 8;
        w = c * i / 10 + (0x80 << 7);
        *(int *)(p + 0x18) = w;
        *(int *)(p + 0x1c) = w;
        WaitFrames(1);
    }
    WaitFrames(5);
    _Actor_SetAnim(p, 1);
    _PlaySound(0x6c);
    WaitFrames(10);
    _PlaySound(0x6c);
    WaitFrames(10);
    _PlaySound(0x6c);
    WaitFrames(10);
    _PlaySound(0x6d);
    ep = (unsigned char *)o + 0x58;
    for (k = 0; k < 16; k++) {
        v[0] = *(int *)(p + 8);
        v[1] = *(int *)(p + 0xc) + (0x80 << 12);
        v[2] = *(int *)(p + 0x10);
        Func_80974d8(v);
        vec3_translate(0x80 << 11, Random(), v);
        Func_809ba90(ep, 0x11d, v[0], v[2]);
        Func_809ba7c(ep, Func_809aa98);
        _Sprite_SetColorswap(*(void **)ep, 7);
        ep += 0x48;
    }
    v[0] = *(int *)(p + 8);
    v[1] = *(int *)(p + 0xc) + (0x80 << 12);
    v[2] = *(int *)(p + 0x10);
    WaitFrames(8);
    _DeleteActor(p);
    WaitFrames(4);
    WaitFrames(0x1e);
    Func_809748c();
}

/* OvlFunc_922_200a094  --  0x0200a094, cut from
 * goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_c_c_c_c_c_c_c.s.
 *
 * NON-MATCHING, 2 of 199 encodings differ.  Size 452 bytes and 199 encodings
 * both EXACT; the two are one swapped pair at index 142.  Re-measured as
 * installed in batch 305 -- the figure is current.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7a8c8c/200a094.c \
 *     asm/overlays/rom_7a8c8c/ovl_30_c_c_c_c_c_c_c_c.s --func OvlFunc_922_200a094
 *
 * NEEDS A TEXT/DATA SPLIT, and the split MUST EXPORT `.global .L2464` -- the
 * function reads that table and `datacheck` names it as the one symbol the data
 * object has to publish.  No shims, no pins.
 *
 * THE RESIDUE.  The ROM builds the loop's third argument as `add r2, sp, #8`
 * and issues it BEFORE `lsl r0, #0xd`; we issue `mov r2, r5` AFTER it:
 *
 *     rom    ldr r1, [sp, #4] / add r2, sp, #0x8 / lsl r0, #0xd / bl
 *     ours   ldr r1, [sp, #4] / lsl r0, #0xd     / mov r2, r5   / bl
 *
 * BATCH 305 READ THE RTL AND THE CAUSE IS NOT THE ARGUMENT SPELLING.  It is
 * loop.c's invariant motion, and the whole chain is visible in `-da` dumps of
 * this very file (four passes, one pseudo):
 *
 *   .00.rtl   precompute_register_parameters splits the call's arguments into
 *             two pseudos -- insn 372 = 0x100000, insn 374 = the address
 *             `(plus virtual-stack-vars -12)` -- then loads the hard regs in
 *             order, insn 376 = r0, 378 = r1, 380 = `(set r2 (reg 130))`.
 *             BOTH call sites look identical here, and so does the ROM's shape.
 *   .03.cse   insn 374 is still a real address computation:
 *             `(set (reg 130) (plus (reg 25 sfp) (const_int -12)))`.
 *   .08.loop  loop.c hoists the loop-invariant `(plus sfp -12)` into reg 61 and
 *             REWRITES insn 374 into a plain copy, `(set (reg 130) (reg 61))`.
 *   .09.cse2  copy-propagates reg 61 into insn 380 -- `(set r2 (reg 61))` --
 *             leaving insn 374 dead; combine deletes it.
 *
 * So the ROM keeps TWO insns for that argument (the address computation at
 * LUID 374, coalesced into r2, plus a deleted copy) and we keep ONE (the copy
 * at LUID 380).  sched2 then does the rest: the two candidates are equally
 * ready, so its tie-break is the ORIGINAL ORDER, and LUID 374 lands before the
 * r0 shift while LUID 380 lands after.  The residue is a LUID difference
 * produced by loop.c, not a scheduling preference -- which is why
 * -fno-schedule-insns2 does not fix it but destroys everything else (51 of 199).
 *
 * WHAT THIS RULES OUT, all measured in batch 305:
 *
 * 1. EVERY FLAG.  Nineteen swept at plain -O2 against the ref.  INERT, still
 *    exactly 2 of 199 at 452 bytes / 199 encodings: -fno-schedule-insns,
 *    -fno-strength-reduce, -fno-force-mem, -fno-caller-saves, -fno-peephole,
 *    -fno-cse-follow-jumps, -fno-cse-skip-blocks, -fno-thread-jumps,
 *    -fomit-frame-pointer, -fno-function-cse, -fno-delayed-branch,
 *    -fno-move-all-movables, -fno-reduce-all-givs.  WORSE: -fno-schedule-insns2
 *    51, -fno-gcse 70 (201 encodings, 456 bytes), -fno-expensive-optimizations
 *    166, -fno-rerun-cse-after-loop 177 (203 encodings, 460 bytes).
 *    -fno-move-all-movables is the interesting negative: it only disables the
 *    aggressive "move every movable" mode, not the ordinary profitability test,
 *    and the ordinary test is what fires here.  gcc-2.96 has NO switch for
 *    loop invariant motion, so the flag rung is CLOSED.
 * 2. SIX MORE ADDRESS SPELLINGS on top of the park's original six, every one
 *    exactly 2 of 199 with the same first difference at index 142:
 *      - a pointer local for every ELEMENT access with the array name kept at
 *        both calls (`p[0]`/`p[1]`/`p[2]`, `p = v` after the `k` build)
 *      - the same, but with the PRE-LOOP call also passing `p` and only the
 *        in-loop call passing `v` -- the ROM's own two forms written out
 *        literally.  Still 2.
 *      - an inner-block pointer at the in-loop call only,
 *        `{ int *pp = v; __vec3_translate(..., pp); }`
 *      - `&v[0]` at the in-loop call only
 *      - `(int *)(void *)v` at the in-loop call only
 *      - `int v[3]` declared inside the `for (;;)` body instead of at the top
 *    They are inert for one reason, and it is the reason to stop trying
 *    spellings: EVERY spelling of a local array's address expands to the same
 *    `(plus virtual-stack-vars -12)`, so they all arrive at insn 374 identical
 *    and loop.c makes the same decision about all of them.  `k << 1` for the
 *    first argument instead of the literal is 165 of 199 at 201 encodings --
 *    the split `mov`+`lsl` build is required there.
 * 3. NOT the argument-precompute class the park originally named.  Both sides
 *    precompute; the difference is downstream of it.
 *
 * WHERE THE NEXT RUNG IS.  move_movables' profitability test is a function of
 * the LOOP as a whole -- its insn_count, the movable's lifetime and savings,
 * and `threshold`, which is `(loop has a call ? 1 : 2) * (3 + n_non_fixed_regs)`.
 * None of those is a property of how the address is written, and the loop body
 * already matches the ROM byte for byte, so the next rung is NOT in this call.
 * It is either (a) the callee return types, which the park already flagged and
 * which change the loop's insn_count -- `int __vec3_translate` was measured at
 * 3, worse, but the other three callees have not been varied -- or (b) some
 * statement in the loop body that is byte-neutral yet changes the movable's
 * lifetime.  Anything that does not move insn 374 is not worth compiling.
 *
 * WHAT CLOSED THE OTHER 190, in the order it mattered:
 *
 * 1. THE LOOP IS NOT IRREDUCIBLE.  Batch 298 read `.L21c6` as a second entry
 *    into the `.L2184` body.  It is a `do`/`while` with a `goto` INTO the middle
 *    of the body -- single-entry, and the layout is exactly what gcc emits:
 *
 *        goto step;
 *        do { <height check, travel> step: <translate, probe>; } while (t != 0xff);
 *
 *    The `b .L21c6` that looked like a second entry is the `goto step`.  Written
 *    as `while ((translate, t = probe()) != 0xff) { body }` instead,
 *    `expand_end_loop`'s rotation drags the body's leading `if (...) break` up
 *    into the condition block and the two blocks come out split in the wrong
 *    place: 57 differing rather than 28.
 *
 * 2. AN ALIASING STORE BETWEEN THE READ AND THE ARGUMENTS.  The ROM loads
 *    `v[0]` and `v[2]` twice -- once into the saved target and once for
 *    `__Actor_TravelTo` -- because `a->f30 = ...` sits between them and kills
 *    the load.  With the stores written FIRST the two reads CSE, the saved
 *    target feeds the call directly, and r8/r9/r10 all shift.  Moving two
 *    assignments took 28 differing to 2 and fixed the high-register roles as a
 *    side effect: this was worth more than any declaration permutation.
 *
 * 3. THE ELSE BRANCH IS OUTSIDE THE LOOP.  `.L217c` (`a->f6 = dir`) sits
 *    between the pre-loop `b .L21c6` and the loop body, which only happens if
 *    the `while` is AFTER the if/else and the else ends in a `goto` past it --
 *    gcc threads the then-branch's `b <after the if>` straight through to the
 *    loop's entry jump.  Nesting the loop inside the then-branch puts the else
 *    block at the end instead.
 *
 * 4. `(unsigned short)dir == 0xffff`, not `(short)dir == -1`.  `dir` comes from
 *    an `ldrsh`, so gcc knows it is sign-extended and compares it directly
 *    (`mov r3, #1 / neg r3, r3 / cmp`).  The unsigned spelling is an AND
 *    against 0xffff, which `simplify_comparison` turns into the ROM's high-half
 *    compare `lsl r3, r2, #16 / cmp r3, =0xffff0000`.
 *
 * 5. `k = 0x80 << 12` as a named local.  The ROM holds 0x80000 in r11 across
 *    the vec3 setup and the height test, and rebuilds the same constant inside
 *    the loop -- so it is a local whose live range ends before the loop, and
 *    the in-loop comparison keeps the literal.
 *
 * 6. `h -= a->fc;` as its own statement, for the ROM's two-operand
 *    `sub r0, r3` with the destination on the call result.
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0xe];
    unsigned char f22;
    unsigned char pad23[0xd];
    int f30;
    int f34;
    unsigned char pad38[0x22];
    unsigned char f5a;
    unsigned char pad5b[9];
    short f64;
    unsigned char pad66[6];
    int f6c;
};

extern unsigned char gState[];
extern unsigned int gKeyHeld;
extern short L2464[] __asm__(".L2464");

extern struct Actor *__GetFieldActor(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __Func_8012038(int a, int b, int c);
extern int __Func_8011f54(int a, int b, int c);
extern void __vec3_translate(int dist, int dir, int *v);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_SetAnimSpeed(struct Actor *a, int n);
extern void __Actor_WaitMovement(struct Actor *a);
extern void __WaitFrames(int n);
extern void OvlFunc_922_200a014(void);

void OvlFunc_922_200a094(void)
{
    struct Actor *a;
    unsigned char *g;
    int v[3];
    unsigned char *q;
    int dir;
    int t, first, h;
    int tx, tz;
    int k;

    g = gState;
    a = __GetFieldActor(*(int *)(g + (0xfa << 1)));
    for (;;) {
        dir = L2464[(gKeyHeld >> 4) & 0xf];
        if ((unsigned short)dir == 0xffff)
            return;
        __CutsceneStart();
        k = 0x80 << 12;
        v[0] = (a->f8 & 0xfff00000) + k;
        v[1] = a->fc;
        v[2] = (a->f10 & 0xfff00000) + k;
        tx = v[0];
        tz = v[2];
        q = &a->f22;
        first = __Func_8012038(*q, tx, tz);
        __vec3_translate(0x80 << 13, dir, v);
        t = __Func_8012038(*q, v[0], v[2]);
        if (t != 0xff && __Func_8011f54(*q, v[0], v[2]) - a->fc <= k) {
            v[0] = tx;
            v[2] = tz;
            a->f30 = 0x80 << 10;
            a->f34 = 0x1999;
            a->f64 = 0;
            __Actor_TravelTo(a, tx, a->fc, tz);
            __Actor_SetAnim(a, 2);
            __Actor_SetAnimSpeed(a, 0x30);
            __Actor_WaitMovement(a);
            a->f6c = (int)OvlFunc_922_200a014;
        } else {
            a->f6 = dir;
            goto done;
        }
        goto step;
        do {
            h = __Func_8011f54(*q, v[0], v[2]);
            h -= a->fc;
            if (h > (0x80 << 12))
                break;
            tx = v[0];
            tz = v[2];
            a->f30 = 0x80 << 10;
            a->f34 = 0x1999;
            __Actor_TravelTo(a, v[0], v[1], v[2]);
            __Actor_WaitMovement(a);
            if (t != first)
                goto blocked;
        step:
            __vec3_translate(0x80 << 13, dir, v);
            t = __Func_8012038(*q, v[0], v[2]);
        } while (t != 0xff);
        a->f30 = 0x80 << 10;
        a->f34 = 0x80 << 9;
        __Actor_TravelTo(a, tx, a->fc, tz);
        __Actor_WaitMovement(a);
        __WaitFrames(2);
        continue;
    blocked:
        a->f6c = 0;
        a->f5a |= 1;
        a->f34 = 0x80 << 7;
    done:
        __WaitFrames(0xa);
        __CutsceneEnd();
        return;
    }
}

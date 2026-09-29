/* OvlFunc_922_200a094  --  0x0200a094, cut from
 * goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_c_c_c_c_c_c_c.s.
 *
 * NON-MATCHING, 2 of 199 encodings differ.  Size 452 bytes and 199 encodings
 * both EXACT; the two are one swapped pair at index 142.
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
 * Both are one instruction, so this is the argument-precompute class: the raw
 * `(plus sp 8)` costs more than one insn and `precompute_register_parameters`
 * hoists it ahead of the r0 build, while a register copy is cheap and gets the
 * last slot.  The ROM's own PRE-LOOP call to the same function has our form
 * (`mov r2, r5` last), so the ROM has the address as a REGISTER there and as
 * the un-CSEd expression inside the loop -- which is the thing no spelling
 * reached.  Six spellings all measured EXACTLY 2: `v`, `&v[0]`, `v + 0`, a
 * pointer local assigned inside the loop, a pointer local for every access, and
 * no pointer local at all.  Per the notebook, identical counts across unrelated
 * spellings indict the variable's existence, so the next rungs are the callee
 * return types (`int __vec3_translate` is 3, worse) and the flags.
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

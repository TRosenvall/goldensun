/* Field_Douse  --  0x080999f0, was asm/rom_8a000/rom_97b54_c_c_a.s (this
 * function alone; tools/datacheck.py reports NO data section), so it converts
 * whole with NO split and no extra exports.
 *
 * NON-MATCHING, 330 of 365 encodings differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/80999f0.c \
 *     asm/rom_8a000/rom_97b54_c_c_a.s --func Field_Douse
 *   -> XX SIZE  ref 808 bytes, ours 816
 *      XX ENCODINGS differ in 330 place(s) (ref 365, ours 369)
 *
 * Shims: NONE (tools/shimcount.py reports 0).
 *
 * READ src/rom_8a000/rom_9a44c_c_c_a_a.c FIRST -- Field_Whirlwind is an EXACT
 * structural twin of this function and its header is the template: same
 * [iwram_3001f30] read, same from/to vec3 pair on the stack, same
 * `from[k] + i * (to[k] - from[k]) / 10` interpolation over 11 frames through
 * __divsi3, same `vec3_translate(Random() * 5 + (0xc0 << 10), Random(), v)`.
 * Four of its five levers transfer to this draft unchanged; the fifth (its
 * union / alias-set-0 scheduling dependence) has no counterpart here.
 *
 * WHAT CLOSED, and the measurement for each (ref is 365 instructions):
 *
 * 1. `int *o = iwram_3001f30;` INDEXED AS WORDS, not `unsigned char *` with
 *    `*(int *)(m + 0x10)` casts -- o[1]..o[5] for the struct's word fields and
 *    `*(signed char *)((char *)o + 0x20)` for its byte flags.  Field_Whirlwind's
 *    spelling.  [331 -> 330 differing]
 *
 * 2. `i = 0;` AND `n = 0xb;` AT THE TOP OF THE FUNCTION, before
 *    CreateParticleActor.  The ROM sets r8 = 0 in the FIRST basic block, reusing
 *    the `mov r1, #0` it already needs for the call's second argument, so `i` is
 *    live across the whole body -- which is what drives `t` out of r8 into r10
 *    and stops the v1 pointer being spilled to sp+0.  [375 -> 368, and the
 *    disagreeing region falls from 360 lines to 320]
 *
 * 3. THE LOOP BOUND MUST ARRIVE THROUGH A LOCAL (`n`), NOT AS A LITERAL.
 *    Field_Whirlwind's lever 3, confirmed here: combine.c's simplify_comparison
 *    rewrites `LT C` (C > 0) to `LE C-1` for a comparison whose constant it can
 *    see, so a literal `i < 0xb` gives `cmp #0xa / ble` where the ROM has
 *    `cmp #0xb / blt`.  A bound reaching the compare through a local escapes the
 *    rewrite and reload rematerialises it as the immediate.
 *    (CAUTION -- an earlier note here claimed a sweep of the tree found `blt`
 *    after `cmp #imm` ONLY with the immediate 0.  That was a sampling error: the
 *    sweep had covered 400 of the 4,339 gcc-generated `.s` files.  The full
 *    sweep finds `blt` against #1, #4, #9, #11, #15, #193 and #201.  The
 *    mechanism above is the real one and is the one Field_Whirlwind documents.)
 *
 * 4. THE SCALE MULTIPLICAND MUST ALSO ARRIVE THROUGH A LOCAL.  The ROM has a
 *    register multiply, `mov r3, #0xc0 / lsl r3, #8 / mov r0, r8 / mul r0, r3`.
 *    A literal `i * (0xc0 << 8)` never produces it: 0xc000 == 3 << 14, so
 *    expand_mult SYNTHESISES the multiply as `lsl r0, r3, #1 / add r0, r8 /
 *    lsl r0, #14` and creates a giv for it as well, which costs r9 and forces
 *    the v1 pointer into memory.  `sa = 0xc0 << 8; h = i * sa / 10 + (0x80 << 7)`
 *    keeps the `mul`; reload then rematerialises the constant at the use site,
 *    which is why the ROM materialises it INSIDE the loop.  Note
 *    Field_Whirlwind needs NO local for its own scale -- its constant is
 *    0x10ccc, which expand_mult cannot synthesise cheaply, so a literal already
 *    gives `mul`.  The local is needed exactly when the constant is
 *    shift-and-add cheap.  [fixes the whole rom[120..133] block]
 *
 * 5. `i * (v1[k] - v2[k])`, NOT the reverse -- *thumb_mulsi3 ties operand 0 to
 *    operand 1, so the ROM's `mov r0, r8 / mul r0, r3` names `i` first.
 *    (Field_Whirlwind lever 2, third confirmation.)
 *
 * 6. THE TWO INTERPOLATION LOOPS MUST BE `goto`, NOT `for`.  Unlike
 *    Field_Whirlwind, here the `for (; i < n; i++)` spelling is 8 instructions
 *    WORSE (377 vs 369): loop.c spills both vec3 pointers and reloads them per
 *    access.  Both spellings were measured on the same body.
 *
 * 7. THE FRAME.  `sub sp, #0x2c`, with v3 at sp+0x20, v2 at sp+0x14, v1 at sp+8
 *    and reload spills at sp+0 and sp+4.  gcc-2.96 lays locals out DOWNWARD in
 *    declaration order, so they must be declared v3, v2, v1 -- last-declared
 *    lowest, the Field_Whirlwind lever-1 rule.
 *
 * 8. `mp = o; h = *mp++; vec3_translate(0x80 << 13, h, mp);` is what produces
 *    the ROM's single-register `ldmia r5!, {r1}`.  Do NOT write the
 *    post-increment inside the argument list -- the ROM passes the pointer
 *    AFTER the increment, which an argument list does not guarantee.
 *
 * BLOCKER, by pass: global.c / reload -- ONE SPILL DECISION.
 *
 *   ROM   sp+4 = &v2 (master, reloaded ONCE into r1 per block)
 *         r11  = &v1 (master)   r10 = &v1 loop copy   r9 = cnt, then &v2 copy
 *   ours  r9   = &v2            r10 = &v1             r11 = n / cnt
 *
 * The ROM carries FOUR pointer allocnos for the two arrays -- a master for the
 * straight-line fill plus a fresh copy per loop (`mov r10, r11`, `mov r9, r2`) --
 * so all seven call-preserved registers (r5,r6,r7 + r8..r11; r4 is call-used
 * under -fcall-used-r4) are full and &v2 spills.  A spilled master is CHEAPER
 * here, not dearer: one `ldr r1, [sp, #4]` serves three `str r3, [r1, #k]`,
 * where ours pays `mov r2, r9` before every single store because &v2 sits in a
 * HIGH register.  That is most of the 330.
 *
 * Ours has only TWO pointer allocnos: cse1 unifies the loop's `sp + N` address
 * with the pre-loop fill's, so loop.c has no movable left to hoist into the
 * preheader and no copy is ever created.
 *
 * MEASURED and REJECTED (count of ref 365):
 *   369  this draft
 *   369  named pointers for the two FILLS, array names in the loops
 *   369  named pointers for the two LOOPS, array names in the fills
 *        -- both coalesced straight back by gcse's cprop_insn, the mechanism
 *        Field_Ply's one pin exists for
 *   375  `for` loops with literal scale and bound
 *   375  unsigned cast on the multiply; the product split into its own statement
 *   377  the Field_Whirlwind `for (; i < n; i++)` form with everything else kept
 *   387  the scale computed at the TOP of the loop body instead of after the
 *        three interpolated stores
 *   360  -fno-gcse (diverges at instruction 1 -- not the lever)
 *
 * WHAT WOULD MOVE IT.  A spelling that gives each array a separate address
 * pseudo for the fills and for each loop without the copy being coalescible.
 * Field_Whirlwind's lever 4 is the nearest precedent: there the knob was
 * move_movables' lifetime test at loop.c:1803, and a named pointer on ONE of
 * two stores (not both, not neither) put the copy where the ROM has it.  The
 * equivalent asymmetry has not been searched here.
 */
extern int *iwram_3001f30;
extern unsigned char *CreateParticleActor(int id, int x, int y, int z);
extern void Func_8097384(void);
extern void _PlaySound(int id);
extern void vec3_translate(int d, int a, int *v);
extern int _Func_8011f54(int a, int b, int c);
extern void WaitFrames(int n);
extern unsigned int Random(void);
extern void Func_8099920(void);
extern void Func_80999a8(void);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void _Actor_SetColorswap(unsigned char *a, int n);
extern void _DeleteActor(unsigned char *a);
extern void Func_809748c(void);

void Field_Douse(void)
{
    int v3[3];
    int v2[3];
    int v1[3];
    int *o;
    int *t;
    int *mp;
    unsigned char *p;
    unsigned char *q;
    unsigned char *s;
    int cnt;
    int i;
    int h;
    int n;
    int sa;
    int sb;

    o = iwram_3001f30;
    t = (int *)o[4];
    i = 0;
    n = 0xb;
    p = CreateParticleActor(0xef, 0, 0, 0);
    if (p == 0)
        return;
    Func_8097384();
    _PlaySound(0x8a);
    if (o[5] == 0) {
        o[1] = t[2];
        o[3] = t[4];
        mp = o;
        h = *mp++;
        vec3_translate(0x80 << 13, h, mp);
        o[2] = _Func_8011f54(0, *mp, o[3]);
    }
    v2[0] = t[2];
    v2[1] = t[3] + (0x80 << 13);
    v2[2] = t[4];
    v1[0] = o[1];
    v1[1] = o[2] + (0x80 << 14);
    v1[2] = o[3];
    if (*(signed char *)((char *)o + 0x34) != 0)
        v1[1] = o[2] + (0xa0 << 15);
    sa = 0xc0 << 8;
loopA:
        *(int *)(p + 8) = v2[0] + i * (v1[0] - v2[0]) / 10;
        *(int *)(p + 0xc) = v2[1] + i * (v1[1] - v2[1]) / 10;
        *(int *)(p + 0x10) = v2[2] + i * (v1[2] - v2[2]) / 10;
        h = i * sa / 10 + (0x80 << 7);
        *(int *)(p + 0x18) = h;
        *(int *)(p + 0x1c) = h;
        WaitFrames(1);
    i++;
    if (i < n)
        goto loopA;
    WaitFrames(0xa);
    if (*(signed char *)((char *)o + 0x45) == 0) {
        cnt = 0xa;
        if (*(signed char *)((char *)o + 0x20) == 0)
            cnt = 0x18;
        for (i = 0; i < cnt; i++) {
            v3[0] = *(int *)(p + 8);
            v3[1] = *(int *)(p + 0xc);
            v3[2] = *(int *)(p + 0x10);
            vec3_translate(Random() * 5 + (0xc0 << 10), Random(), v3);
            if (i == cnt - 1) {
                WaitFrames(0x19);
                v3[0] = *(int *)(p + 8);
                v3[1] = *(int *)(p + 0xc);
                v3[2] = *(int *)(p + 0x10);
            }
            q = CreateParticleActor(0xf0, v3[0], v3[1], v3[2]);
            if (q != 0) {
                *(int *)(q + 0x14) = v3[1] + 0xffe00000;
                *(void (**)(void))(q + 0x6c) = Func_8099920;
                q[0x55] = 2;
            }
            _PlaySound(0x84);
            WaitFrames(6);
        }
        WaitFrames(0xa);
    } else {
        cnt = 0xa;
        if (*(signed char *)((char *)o + 0x20) == 0)
            cnt = 0x1e;
        for (i = cnt; i != 0; i--) {
            v3[0] = *(int *)(p + 8);
            v3[1] = *(int *)(p + 0xc);
            v3[2] = *(int *)(p + 0x10);
            vec3_translate(Random() * 5 + (0xc0 << 10), Random(), v3);
            q = CreateParticleActor(0x8e << 1, v3[0], v3[1], v3[2]);
            if (q != 0) {
                *(void (**)(void))(q + 0x6c) = Func_80999a8;
                q[0x55] = 0;
                s = *(unsigned char **)(q + 0x50);
                s[9] = (s[9] & ~0xc) | 8;
                _Actor_SetAnim(q, 8);
                _Actor_SetColorswap(q, 7);
            }
            WaitFrames(6);
        }
        WaitFrames(0x46);
    }
    sb = -0xc000;
    i = 0;
loopB:
        *(int *)(p + 8) = v1[0] + i * (v2[0] - v1[0]) / 10;
        *(int *)(p + 0xc) = v1[1] + i * (v2[1] - v1[1]) / 10;
        *(int *)(p + 0x10) = v1[2] + i * (v2[2] - v1[2]) / 10;
        h = i * sb / 10 + (0x80 << 9);
        *(int *)(p + 0x18) = h;
        *(int *)(p + 0x1c) = h;
        WaitFrames(1);
    i++;
    if (i < n)
        goto loopB;
    _DeleteActor(p);
    Func_809748c();
}

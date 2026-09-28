/* Field_Move_Target -- 0x08097c3c, asm/rom_8a000/rom_97b54_a_c_a_a_a_c_a_a.s
 * NON-MATCHING, 326 encodings of 368.  NOT a distance (ref 836 bytes / 368 encodings against ours 852 / 376, eight over).
 * READ `--align`: 196 of 368.  All 45 relocations exact and in order.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/8097c3c.c \
 *     asm/rom_8a000/rom_97b54_a_c_a_a_a_c_a_a.s --func Field_Move_Target
 * Distance while iterating:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_8a000/8097c3c.c --ref asm/rom_8a000/rom_97b54_a_c_a_a_a_c_a_a.s --align
 * 355 instructions, ONE function, no data section (grep -ci func_start = 1):
 * whole-file conversion.  batch 294, brief E, target 3.
 *
 * PARKED.  --align 196 of 368.  objcmp: 326 of 368 differ (ours 376), SIZE ref
 * 836 bytes / ours 852 -- so EIGHT INSTRUCTIONS OVER and the 326 is NOT a true
 * distance.  All 45 RELOCATIONS are present, in the same order, with the same
 * symbols -- only their offsets shift -- so the callee set, the literal pool
 * contents and the pool ORDER are all correct and the residue is entirely
 * inside the code.
 * NO SHIMS: zero `register ... __asm__` declarations, zero `__asm__(".equ ...")`
 * lines, no volatile, no barriers, default flags.
 *
 * BLOCKER: global register allocation (global.c `allocno_compare` /
 * `find_reg`).  Quantified below.  Not arithmetic, not scheduling, not source
 * shape -- the block order, the branch senses, the frame layout, every spill
 * slot offset and every relocation already agree.
 *
 * ================= THE READING (all of it verified) =================
 *
 * `iwram_3001f30` is the bank's field-move context pointer; the park
 * src/non_matching/rom_8a000/809802c.c (Field_Move, the immediate sibling)
 * fixes the idiom.  +0x10 caster, +0x14 target, +0x00 facing, +0x18 a short
 * map-actor slot, +0x38/+0x3c the saved script pair, +0x44 a colourswap byte.
 *
 * FRAME 0x34.  v[3] at sp+0x28, w[3] at sp+0x1c: both are address-taken so
 * both are real declared arrays, and ARM's FRAME_GROWS_DOWNWARD gives the
 * FIRST-declared the HIGHEST offset -- hence `v` before `w`.  The seven scalar
 * slots below are reload spills, and their order is g(0x18) caster(0x14)
 * sx(0x10) sz(0xc) dir(8) flag(4) ap(0).
 *
 * THREE ROM SHAPES THAT ARE NOT SOURCE CONSTRUCTS, for the next reader:
 *
 *  1. `push {r5,r6,r7,lr}` with r4 UNSAVED while r4 is used, and r4 spilled
 *     around calls.  This is not a flag to discover: Makefile:131 already
 *     passes -fcall-used-r4 globally, so r4 is call-clobbered.  1,597 tracked
 *     .s files open this way.  Do not go looking for -fcall-saved-r4 here.
 *
 *  2. `str r5, [r0, #0x24]` / `[0x28]` / `[0x2c]` in the d==0xffff arm stores
 *     ZERO, not 0x100000.  r5 is the `gKeyPress & 0x303` value; cse's
 *     record_jump_cond knows it is 0 on the `beq`'s fall-through edge, and
 *     cse_end_of_basic_block extends that path through the loop's terminating
 *     `b` into the body.  Written as a plain `= 0` here, and our output
 *     reproduces `str r5` unaided.
 *
 *  3. `add r2, sp, #4 / ldrb r2, [r2]` for the byte store at tgt+0x5a reads the
 *     low byte of the SImode `flag` slot.  Thumb ldrb has no sp-relative form,
 *     hence the separate `add`.  `tgt[0x5a] = flag;` with `int flag` is what
 *     produces it.
 *
 * LOOP SHAPE.  `while (1) { WaitFrames(1); if (gKeyPress & 0x303) break; ... }`.
 * The ROM's entry `b .L97ee4` jumps forward into the block that physically
 * FOLLOWS the body, and that block ends `b` back UP to the body: that is
 * stmt.c:2257 `expand_end_loop` rolling the first exit test to the bottom,
 * which fires ONLY for a jump to the loop's own end_label -- a `break`.  A
 * `goto` there leaves last_test_insn NULL and does not roll (batch 293,
 * finding 4).  Reproduced first try.
 *
 * BLOCK ORDER.  The `hit` arm (.L97e16) is written BEFORE the flag=1 / move arm
 * even though control reaches it later, because gcc lays bodies out in SOURCE
 * order; and the last test is spelled `if (_Func_8011fd8(...) == 0) goto
 * setflag;` with `hit:` falling through, which is the ROM's `beq .L97e32`
 * sense and costs no extra jump.
 *
 * ================= TWO LEVERS, EACH SINGLY MEASURED =================
 *
 * 1. DECLARATION ORDER OF THE SPILLED SCALARS IS A LEVER HERE.  [211 -> 203
 *    aligned]  Reload's stack slots come out in declaration order, and with
 *    FRAME_GROWS_DOWNWARD that means first-declared = highest offset.  Declaring
 *    g, caster, sx, sz, dir, flag, ap in that order puts all seven slots on the
 *    ROM's offsets; before it every `[sp, #imm]` in the function was wrong.
 *    *** This CONTRADICTS docs/elevation.md / the batch-293 brief, which record
 *    spill-slot order as "a consequence of the allocation, not a handle on it.
 *    Measured inert; do not spend budget reordering declarations."  On this
 *    function it is worth 8 aligned instructions and it fixes ~20 encodings.
 *    The distinction is probably that here the quantities are SPILLED for their
 *    whole lives (no hard register at all), so the slot is the only thing the
 *    declaration can decide. ***
 *
 * 2. THE POINTER CARRIES THE OFFSET, NOT THE SUBSCRIPT.  [203 -> 196 aligned,
 *    and -2 instructions]  The ROM does `bl MapActor_GetActor / add r0, #0x5a /
 *    ldrb r2, [r0]`, destroying the returned pointer.  `p = MapActor_GetActor(
 *    ...) + 0x5a; *p &= 0xfe;` gives exactly that; `p = MapActor_GetActor(...);
 *    p[0x5a] &= 0xfe;` keeps p live and emits `mov r1, r0 / add r1, #0x5a`.
 *    Both sites (the in-loop `&= 0xfe` and the post-loop `|= 1`) need it.
 *    This is docs/elevation.md's "Name the store's DESTINATION pointer when the
 *    ROM computes the address first", from the read side.
 *
 * ============ THE BLOCKER, QUANTIFIED ============
 *
 * The eight extra instructions are ALL the constant 0x100000 (0x80 << 13).  The
 * ROM builds it ONCE in the preamble (`mov r5,#0x80 / lsl r5,#0xd`), copies it
 * into r11 in the loop preheader (`mov r11, r5`), and then every one of its
 * nine further uses is free because it rides an instruction that exists anyway
 * (`add r3, r11`, `mov r0, r11`, `add r2, r11`).  Three instructions in total.
 * We rebuild it at SEVEN sites, 14 instructions.
 *
 * WHY, exactly.  A source carrier (`int up = 0x80 << 13;` used at all nine
 * sites) is BYTE-IDENTICAL to writing the literal -- measured, 378 lines either
 * way.  gcc keeps the REG_EQUIV constant on the pseudo, the pseudo loses the
 * allocation, and reload then REMATERIALISES a REG_EQUIV constant instead of
 * spilling it.  Two independent confirmations:
 *   - The -dL loop dump of this very file lists every constant-building pseudo
 *     as `regno N (life 1), move-insn savings 1 not desirable`, against the two
 *     that ARE hoisted (`regno 87 (life 107), savings 3 moved`, `regno 91
 *     (life 42) moved`).  move_movables' test is
 *         threshold * savings * m->lifetime >= insn_count
 *     (loop.c:1803) with savings = 1 (loop.c:1038) and
 *     threshold = (has_call ? 1 : 2) * (1 + n_non_fixed_regs) (loop.c:651).
 *     A life-13 pseudo in the same dump is still "not desirable", so the bar is
 *     a lifetime of roughly 15 LUIDs and a per-use pseudo can never clear it.
 *   - `__asm__ ("" : "+r" (up))` removes the REG_EQUIV and reload then SPILLS
 *     rather than rematerialising: 375 lines but the frame grows to 0x38 and
 *     the uses become `ldr` from the stack.  It is not the ROM's shape, so the
 *     shim is not the answer and is not in this file.
 *
 * SO THE CONSTANT NEEDS A CALLEE-SAVED REGISTER, AND THERE IS NONE LEFT.  With
 * -fcall-used-r4 the call-crossing part of REG_ALLOC_ORDER (arm.h:989) is
 * 5, 6, 7, 8, 10, 9, 11 -- seven registers.  Both versions fill all seven.
 *
 *   ROM   r5 = gKeyPress&0x303 AND &w (they share; keys dies before &w is born)
 *         r6 = tgt   r7 = &v   r8 = d   r9 = &v(2nd copy) then 0x3333
 *         r10 = a    r11 = 0x100000
 *   ours  r5 = keys  r6 = tgt  r7 = &w  r8 = &v  r9 = a  r10 = d  r11 = &v(2nd)
 *         and 0x100000 gets nothing.
 *
 * `&w` is the whole difference.  The -dg dump gives the allocno order
 *     41 164 44 45 144 81 42 39 73 132 87 76 184 43 40 50 33 91 186 36 32 ...
 * and the -dl figures behind it:
 *     132  &w address temp  24 refs / 105 insns, crosses 11 calls  -> 0.23
 *      87  &v address temp  63 refs / 350 insns, crosses 21 calls  -> 0.18
 * allocno_compare ranks by refs/live_length, so our short-lived `&w` temp
 * OUTRANKS the long-lived `&v` temp, takes r7 (third in the order), and shifts
 * &v, a, d and the second &v copy each one place down the list -- which pushes
 * the constant off the end.  In the ROM `&w` is ranked BELOW all seven and
 * lands on r5 by sharing with the (already dead) keys value.
 *
 * Note the coupling, which is why no single drop finds this: the constant
 * cannot get a register until `&w` gives up r7, and `&w` will not give up r7
 * until its refs/live_length ratio falls below `&v`'s.  Read `n_refs` from the
 * dump, not from the source -- flow.c:4948 weights each reference by
 * loop_depth + 1, and everything in this loop counts double.
 *
 * WHAT WOULD HAVE TO CHANGE, for the next attempt: lower `&w`'s ratio, either
 * by lengthening its live range (a first use earlier in the body) or by cutting
 * its reference count.  Its nine raw references are the three stores, the
 * vec3_translate argument, the two reads in the collision compare, and the
 * three reads in the `flag == 1` block -- and all nine are forced by the ROM's
 * own instructions, so there is no slack in the count.  That leaves the live
 * range, and the ROM computes `add r5, sp, #0x1c` at exactly the same point we
 * do.  I could not find a spelling that moves it.
 *
 * MEASURED AND INERT / WORSE (all against the 196 baseline, aligned):
 *   -fno-strict-aliasing  198        -fno-schedule-insns2  214
 *   -fno-gcse             218        -fno-gcse + `int up` carrier  212
 *   `int up = 0x80 << 13;` carrier, default flags   214 (same 378 lines)
 *   `int up` + `__asm__("" : "+r"(up))`             223, 375 lines, frame 0x38
 * -fno-gcse is worth recording as a dead end: it makes the rematerialisation
 * WORSE (eight sites, not seven), so gcse's cprop is not what folds the
 * carrier -- reload's REG_EQUIV path is.
 *
 * -- worked in scratch_elev/b294/E
 */

extern unsigned char *iwram_3001f30;
extern int iwram_3001e40;
extern int gKeyHeld;
extern int gKeyPress;
extern unsigned char L9f118[] __asm__(".L9f118");
extern unsigned char L9f0bc[] __asm__(".L9f0bc");

extern void Func_8097384(void);
extern unsigned short Func_8097b54(int keys);
extern unsigned char *Func_8098070(unsigned char *a);
extern void Func_8098184(unsigned char *a);
extern void Func_809748c(void);
extern void Func_8097174(void);
extern void Func_80981b0(unsigned char *a);
extern void Func_8096b88(void);
extern void vec3_translate(int mag, int dir, int *v);
extern void _Actor_SetScript(unsigned char *a, unsigned char *s);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void _Actor_SetColorswap(unsigned char *a, int n);
extern void _Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void _Actor_WaitMovement(unsigned char *a);
extern int _TestCollision(unsigned char *a, int *v);
extern unsigned char *_Func_800d98c(unsigned char *a, int *v);
extern int _Func_8011fd8(int n);
extern unsigned char *MapActor_GetActor(int slot);
extern void _PlaySound(int id);
extern void WaitFrames(int n);

void Field_Move_Target(void)
{
    int v[3];
    int w[3];
    unsigned char *g;
    unsigned char *caster;
    int sx;
    int sz;
    int dir;
    int flag;
    unsigned char *ap;
    unsigned char *tgt;
    unsigned char *a;
    unsigned char *o;
    unsigned char *p;
    int d;
    int x0;
    int z0;

    g = iwram_3001f30;
    caster = *(unsigned char **)(g + 0x10);
    tgt = *(unsigned char **)(g + 0x14);
    dir = *(int *)g + (0x80 << 8);
    flag = 0;
    if (tgt == 0)
        return;
    Func_8097384();
    *(unsigned char **)(caster + 0x68) = tgt;
    _Actor_SetScript(caster, L9f0bc);
    a = Func_8098070(caster);
    if (a == 0) {
        Func_809748c();
        return;
    }
    *(unsigned char **)(a + 0x68) = tgt;
    v[0] = *(int *)(tgt + 8);
    v[1] = *(int *)(tgt + 0xc) + (0x80 << 13);
    v[2] = *(int *)(tgt + 0x10);
    vec3_translate(0x80 << 13, dir, v);
    _Actor_TravelTo(a, v[0], v[1], v[2]);
    Func_8098184(a);
    *(int *)(a + 0x30) = 0x80 << 11;
    *(int *)(a + 0x34) = 0x80 << 8;
    ap = a + 0x55;
    *ap = 4;
    *(void **)(tgt + 0x6c) = (void *)Func_8096b88;
    *(int *)(tgt + 0x30) = 0x6666;
    *(int *)(tgt + 0x34) = 0x3333;
    tgt[0x5a] = flag;
    tgt[0x22] = 2;
    while (1) {
        WaitFrames(1);
        if ((gKeyPress & 0x303) != 0)
            break;
        d = Func_8097b54(gKeyHeld);
        if (d == 0xffff) {
            v[0] = *(int *)(tgt + 8);
            v[1] = *(int *)(tgt + 0xc) + (0x80 << 13);
            v[2] = *(int *)(tgt + 0x10);
            vec3_translate(0x80 << 13, dir, v);
            _Actor_TravelTo(a, v[0], v[1], v[2]);
            _Actor_SetAnim(a, 1);
            *(int *)(a + 0x24) = 0;
            *(int *)(a + 0x28) = 0;
            *(int *)(a + 0x2c) = 0;
            continue;
        }
        v[0] = *(int *)(tgt + 8);
        v[1] = *(int *)(tgt + 0xc) + (0x80 << 13);
        v[2] = *(int *)(tgt + 0x10);
        vec3_translate(0x80 << 13, dir, v);
        vec3_translate(0x80 << 10, d, v);
        _Actor_TravelTo(a, v[0], v[1], v[2]);
        _Actor_WaitMovement(a);
        v[0] = *(int *)(tgt + 8);
        v[1] = *(int *)(tgt + 0xc);
        v[2] = *(int *)(tgt + 0x10);
        vec3_translate(0x80 << 13, d, v);
        w[0] = *(int *)(tgt + 8);
        w[1] = *(int *)(tgt + 0xc);
        w[2] = *(int *)(tgt + 0x10);
        vec3_translate(0x80 << 14, d, w);
        if (_TestCollision(tgt, v) > 0)
            goto hit;
        o = _Func_800d98c(tgt, v);
        if (o == 0)
            goto move;
        if (o != caster)
            goto hit;
        x0 = *(int *)(caster + 8) & 0xfff00000;
        z0 = *(int *)(caster + 0x10) & 0xfff00000;
        if (x0 == (v[0] & 0xfff00000) && z0 == (v[2] & 0xfff00000))
            goto hit;
        if (x0 != (w[0] & 0xfff00000))
            goto move;
        if (z0 != (w[2] & 0xfff00000))
            goto move;
        if (_Func_8011fd8(caster[0x22]) == 0)
            goto setflag;
      hit:
        _Actor_SetAnim(a, 4);
        if ((iwram_3001e40 & 0xf) == 0)
            _PlaySound(0x72);
        continue;
      setflag:
        flag = 1;
      move:
        _PlaySound(0xaf);
        sx = v[0];
        sz = v[2];
        _Actor_SetAnim(a, L9f118[(unsigned short)(dir - d) >> 14]);
        WaitFrames(0xf);
        tgt[0x5b] = 0;
        *(int *)(tgt + 0x30) = 0x3333;
        *(int *)(tgt + 0x34) = 0x3333;
        _Actor_TravelTo(tgt, v[0], v[1], v[2]);
        *ap = 0;
        *(int *)(a + 0x30) = 0x3333;
        *(int *)(a + 0x34) = 0x3333;
        vec3_translate(0x80 << 13, d, v);
        _Actor_TravelTo(a, v[0], v[1] + (0x80 << 13), v[2]);
        if (flag == 1) {
            p = MapActor_GetActor(*(short *)(g + 0x18)) + 0x5a;
            *p &= 0xfe;
            *(int *)(caster + 0x30) = 0x3333;
            *(int *)(caster + 0x34) = 0x3333;
            _Actor_TravelTo(caster, w[0], w[1], w[2]);
        }
        _Actor_WaitMovement(tgt);
        *(int *)(tgt + 8) = sx;
        *(int *)(tgt + 0x10) = sz;
        *(int *)(tgt + 0x24) = 0;
        *(int *)(tgt + 0x2c) = 0;
        break;
    }
    _Actor_SetColorswap(tgt, g[0x44]);
    _Actor_SetScript(tgt, *(unsigned char **)(g + 0x3c));
    *(void **)(tgt + 0x6c) = *(void **)(g + 0x38);
    Func_8097174();
    if (flag == 1) {
        p = MapActor_GetActor(*(short *)(g + 0x18)) + 0x5a;
        *p |= 1;
    }
    Func_809748c();
    Func_80981b0(a);
}

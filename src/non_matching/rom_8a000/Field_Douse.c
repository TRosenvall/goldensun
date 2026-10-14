/* Field_Douse -- 0x080999f0.  PARK, 330 of 365 encodings differ (ours 369).
 * ref: asm/rom_8a000/rom_97b54_c_c_a.s -- ONE function, tools/datacheck.py
 * reports NO data section, so this converts WHOLE with NO split and no exports.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/Field_Douse.c \
 *     asm/rom_8a000/rom_97b54_c_c_a.s --whole
 *   -> XX Field_Douse  330 of 365 differ (ours 369), first at index 7
 *      XX SIZE  ref 808 bytes, ours 816
 *
 * batch 350, brief E.  THE PROGRAM IS UNCHANGED FROM THE PRIOR PARK -- every
 * variant measured this pass was inert or worse.  What changed is the header.
 * Pins/shims: 0 (tools/shimcount.py reports nothing).
 *
 * FOUR NUMBERS, re-derived this pass (not inherited):
 *   encodings  ref 365   ours 369     (+4, so NOT a true distance)
 *   bytes      ref 808   ours 816     (+8)
 *   relocations  37 / 37, same symbols, all offsets shifted
 *   aligncmp   aligned-equal 241 (66.0%), differing/ins/del 147 in 53 hunks
 *
 * ================ THE BRIEF'S MECHANISM DOES NOT APPLY HERE ================
 * Batch 350 sent this park the Field_Halt switch-case-run mechanism
 * (src/rom_8a000/rom_9a44c_c_c_c_b.c; stmt.c:6346-6369's pure-leaf arm of
 * emit_case_nodes, the compiler's only producer of `(lt X (const_int 11))`),
 * on the strength of a corpus scan that found two hand-written backward
 * `cmp r1,#0xb / blt` sites in this reference, at
 * asm/rom_8a000/rom_97b54_c_c_a.s:146 (-> .L99aa4, line 96) and
 * :369 (-> .L99c7e, line 323).  Both ARE backward, so the shape fits.
 *
 * BUT BOTH SITES ALREADY MATCH IN THIS BODY, by a DIFFERENT route.  Compiled at
 * production flags this park emits
 *     cmp r2, #11 / blt .L6      (its .s line 154)
 *     cmp r3, #11 / blt .L25     (its .s line 376)
 * i.e. the ROM's encodings, from the park's OWN lever 3 -- the NAMED BOUND
 * `n = 0xb;` with `if (i < n)`.  There was nothing here for the case run to fix.
 *
 * WHY THE NAMED BOUND WORKS HERE AND COSTS FOUR INSTRUCTIONS ON Field_Halt.
 * Field_Halt's header states the rule and this park is the confirming case:
 * the bound reaches `cmp` as an immediate only when the bound pseudo gets NO
 * HARD REGISTER, so that reload substitutes its `reg_equiv_constant` into
 * cbranchsi4's "rI" operand.  That needs r11 ALREADY TAKEN by another
 * call-crossing allocno (find_reg takes r11 whenever it is free --
 * global.c:1010-1050, with r11 last in REG_ALLOC_ORDER at arm.h:989-995).
 * This function's ROM prologue saves r5-r7 AND r8-r11, so r11 is occupied and
 * the bound spills into the immediate for free.  Field_Halt's r11 is free, which
 * is why the same construct taxed it four instructions there.
 * THIRD CONFIRMATION of this rule (Field_Whirlwind, Field_Halt, here).
 *
 * MEASURED THIS PASS AND WORSE -- the case run is a REGRESSION here:
 *   switch case run on loop A only        330 of 365, ours 373 (+16 bytes)
 *   switch case run on BOTH loops         332 of 365, ours 375 (+20 bytes)
 * Both keep the right `cmp #11 / blt`, because both routes reach it; the case
 * run just adds structure the named bound did not need.  DO NOT RETRY IT.
 *
 * ===================== FLAG AXIS, swept this pass =====================
 * The park's recorded flag list held only -fno-gcse.  All four groups the tree
 * actually carries are now measured, as (objcmp ndiff / ours encodings / bytes /
 * aligncmp differing):
 *   production            330 / 369 / 816 / 147 in 53 hunks   <- shipped
 *   -fno-strength-reduce  330 / 369 / 816 / 147  BIT-IDENTICAL to production
 *   -fno-strict-aliasing  330 / 369 / 816 / 147  BIT-IDENTICAL to production
 *   -fno-schedule-insns2  325 / 367 / 812 / 178 in 67 hunks   AMBIGUOUS
 *   -fno-gcse             337 / 355 / 788 / 179 in 57 hunks   worse
 * -fno-strength-reduce measuring NOTHING is expected and is a positive result:
 * this body already emits 8 `mul`s, exactly the ROM's 8, because the park's
 * lever 4 (both scale constants through locals `sa`/`sb`) already defeated the
 * giv.  There is no strength-reduction defect left to fix here.
 *
 * -fno-schedule-insns2 IS LEFT UNRESOLVED ON PURPOSE.  It moves the raw figure
 * DOWN (330 -> 325) and the length TOWARD the ref (+4 -> +2 encodings), while
 * moving the aligned residue sharply UP (147 -> 178, 53 -> 67 hunks).  The two
 * instruments disagree and neither is obviously the misleading one here, so it
 * is recorded with both numbers rather than filed as a lever or a negative.
 * Whoever takes the next pass should read the sched2 dump before spending a
 * round on it -- note -fsched-verbose=6 writes to the .23.sched2 FILE, not
 * stderr.  A SCHED2_CFLAGS group already exists in the Makefile (line 469), so
 * if it IS the lever the install cost is one rule.
 *
 * ============== WHAT THE REMAINING 330 IS, and what it is NOT ==============
 * NOT a frame-layout question, which rules out the cheapest class.  Measured:
 * the ROM's frame is `sub sp, #0x2c` and ours is `sub sp, sp, #44` -- THE SAME
 * 44 BYTES -- and both the ROM and this body build THREE sp-relative address
 * pseudos (`add rN, sp, #K`, three each).  So the park's lever 7 (declaration
 * order, FRAME_GROWS_DOWNWARD at arm.h:1297) is fully landed and the vec3
 * offsets are right.
 *
 * The residue is the REGISTER ASSIGNMENT of those pointer allocnos, exactly as
 * the park says: the ROM holds &v2 spilled at sp+4 as a master reloaded once per
 * block, where ours keeps &v2 in a HIGH register and pays `mov r2, r9` before
 * every store.  That diagnosis REPRODUCED this pass and is retained.
 *
 * NOTE FOR THE NEXT PASS -- this park's blocker is NOT the same as its brief-mate
 * Field_Catch's, and the two should stop being treated as one problem.
 * Field_Catch (src/non_matching/rom_8a000/809ad70.c) is still SHORT A STACK SLOT
 * (0x24 against the ROM's 0x28) because its eighth long-lived pseudo never
 * spills; this function's frame is already right and only the assignment is
 * wrong.  Same bank and same animation shape, different pass.
 *
 * STILL UNSEARCHED, and still the nearest precedent: Field_Whirlwind's lever 4 --
 * move_movables' lifetime test at loop.c:1803, where a named pointer on ONE of
 * two stores (not both, not neither) put the loop copy where the ROM has it.
 * The park already records that both-pointers and neither-pointer spellings
 * coalesce straight back through gcse's cprop_insn; the ASYMMETRIC spelling is
 * the untried one.  Brief E tested that asymmetry on Field_Catch and found all
 * three pointer spellings there collapse to ONE PROGRAM, so expect the same here
 * unless the asymmetry is placed inside the loop rather than at the fills.
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

/* OvlFunc_921_2009fa4 -- *** BYTE-IDENTICAL. LANDS. ***  452 bytes, 199
 * encodings and 24 relocations identical.  THE FIGURE IS A PER-FLAG FIGURE:
 * it needs this object in the EXISTING GCSE_CFLAGS group (-fno-gcse), which
 * eight objects in the Makefile already use.  Under production -O2 this same
 * body is 1 of 199 -- the park's figure, unchanged -- so the flag is the whole
 * difference and nothing here is a device.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     -e OBJCMP_EXTRA="-fno-gcse" goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7a7298/2009fa4.c \
 *     asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_c.s --func OvlFunc_921_2009fa4
 *   OK OvlFunc_921_2009fa4 -- 452 bytes, 199 encodings and 24 relocations identical
 * (OBJCMP_EXTRA is the harness hook; the SHIPPING form is a GCSE_CFLAGS rule in
 * the Makefile for this object, exactly like asm/rom_8a000/rom_8ba38_a_c_b.o.)
 *
 * SPLIT.  `python3 tools/datacheck.py asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_c.s`
 *   data sections : .bss, .data      functions : OvlFunc_921_2009fa4
 *   -> converting a function here needs a TEXT/DATA SPLIT
 *   OvlFunc_921_2009fa4 reads .L2430 -- *** SPLIT MUST EXPORT: .global .L2430 ***
 * One function in the .s, so no TEXT split; the DATA split and the `.global
 * .L2430` export are the build prerequisite the old header already named.
 *
 * SHIMS: 6 register pins (tools/shimcount.py).  NEEDS A fakematch.txt ROW.
 *
 * ================== WHAT CLOSED IT: THREE EDITS AND ONE FLAG ==================
 * 1. `int *vp;` declared LAST among the locals, after the r11 pin.
 * 2. `vp = v;` placed immediately after `lim = 0x80 << 12;`, and EVERY v[]
 *    element access rewritten `vp[i]`.  The step call still passes `v`.
 * 3. The step block's pin order changed from `q2, q0, q1, shift` to
 *    `q0 = 0x80; q1 = dir; q2 = (int)v; q0 <<= 13;`.
 * 4. -fno-gcse (GCSE_CFLAGS).
 *
 * WHY, and it retires this park's own "landing mechanism" paragraph.
 * The residue was one encoding at index 142: ref `add r2, sp, #8` (aa02), ours
 * `adds r2, r5, #0` (1c2a).  The dump names the culprit exactly -- and it is
 * the ONLY thing PRE does in this function:
 *     PRE: redundant insn 350 (expression 14) in bb 8, reaching reg is 133
 *     PRE: bb 2, insn 503, copy expression 14 in insn 67 to reg 133
 *     PRE GCSE of OvlFunc_921_2009fa4, pass 1: 1 substs, 1 insns created
 * Expression 14 is `(plus:SI (reg:SI 25 sfp) (const_int -12))`.
 *
 * *** THE DELETION IS FORCED, SO THE RESIDUE IS NOT SOURCE-REACHABLE AT -O2. ***
 * lcm.c `compute_insert_delete`: `delete[bb] = antloc[bb] & ~laterin[bb]`.
 *   - antloc[bb8] = 1.  `oprs_anticipatable_p` can only fail if an operand is
 *     set earlier in the block; the sole operand is `(reg 25 sfp)`, which no
 *     insn sets, and which is FIXED, so it is not in `regs_invalidated_by_call`
 *     either -- the CALL_INSN arm of `compute_hash_table` cannot touch it.
 *   - laterin[bb8] = 0.  bb2 computes it, so on every edge out of bb2
 *     `later[e] = earliest[e] | (laterin[bb2] & ~antloc[bb2])` is 0 (antloc[bb2]
 *     zeroes the second term; `earliest` is 0 because avout[bb2] = 1), and 0
 *     propagates down to bb8.  Checked against the dump: laterin[bb2] IS 1
 *     (earliest[bb1->bb2] = 1), which is why bb2's own occurrence survives --
 *     the model reproduces the compiler's single subst exactly.
 *   - transp is 1 everywhere (sfp never set) so ae_kill is 0 everywhere, and
 *     `may_trap_p` of the plus is 0 so the abnormal-edge arm cannot clear
 *     antloc.
 * And the plus CANNOT be kept out of the hash table from C: expand always
 * materialises `&local` into a FRESH PSEUDO (expr.c:5896-5898 discards a
 * non-pseudo target at -O2, ADDR_EXPR's `force_operand` is unconditional), and
 * a pseudo-dest SImode plus is always hashed -- this gcse.c's `hash_scan_set`
 * has NO REG_EQUIV exclusion, `want_to_gcse_p` is true, `can_copy_p[SImode]` is
 * true, `do_not_record_p` is false.  *** THE AGGREGATE LEVER DOES NOT ESCAPE IT
 * EITHER: measured, `ap.p = v` with a two-word struct gives
 * `(set (reg 118) (plus vsv -12))` + `(set (subreg:SI (reg/v:DI 43) 0) (reg 118))`
 * -- the SUBREG lands on the COPY, the plus still has a pseudo dest. ***
 *
 * *** AND THE PARK'S STATED MECHANISM IS WRONG ON TWO INDEPENDENT COUNTS. ***
 * It said "local-alloc.c update_equiv_regs substitutes a single-set/single-use
 * pseudo's REG_EQUIV into its one use".  The gate reads
 *     if (REG_N_REFS (regno) == 2 && REG_BASIC_BLOCK (regno) < 0
 *         && rtx_equal_p (XEXP (note, 0), SET_SRC (set)))
 * and (a) `REG_BASIC_BLOCK < 0` REQUIRES the pseudo to live in more than one
 * basic block -- the code's own comment says "if the register is only used in
 * one basic block, this can't succeed or combine would have done it" -- while
 * the step-site address temp is set and used inside bb8; and (b) `REG_N_REFS`
 * is LOOP-DEPTH WEIGHTED (`flow.c`: `REG_N_REFS (i) += loop_depth`), and the
 * step site is inside the outer `for(;;)` AND the inner do/while, so a
 * set-once/used-once pseudo there scores 4, never 2.  No amount of making the
 * temp single-use could ever have reached this park.
 * The real mechanism is plainer: the plus insn only has to EXIST in bb8; its
 * pseudo is already allocated r2, so the hard-reg fill becomes a noop and is
 * deleted, leaving `add r2, sp, #8`.
 *
 * ========= HOW THE FLAG WENT FROM "71 of 199, WORSE" TO BYTE-IDENTICAL =========
 * The park recorded -fno-gcse as 71 of 199 and +4 bytes and treated it as
 * refuted.  Re-measured at the installed baseline the number is real -- but it
 * is 201 INSTRUCTIONS against 199, so by the brief's own rule the 71 is a
 * MISALIGNMENT, not a distance.  Read aligned, -fno-gcse already emits the
 * ROM's `aa02` at the step site; the +2 is one extra `add r5, sp, #8` in bb8
 * plus its wake, because with PRE off the step block's OWN address temp is
 * reused by cse1 for the post-call `v[0]`/`v[2]` reads, so it has three uses
 * and takes a call-saved register instead of r2.
 * That is EXACTLY this park's "STAGE 1, cse1 -- SOURCE-DEFEATABLE", and the
 * cure is the park's own rejected entry `int *vp` -- which it had measured ONLY
 * with gcse ON (72, worse).  ONE HALF IN THE REJECTED LIST, THE OTHER HALF IN
 * THE EXISTENCE PROOF (docs/repro-2009fa4-vp/A1_vp_r5.c), NEVER CROSSED.
 * Crossed, with `vp` UNPINNED and `vp = v` moved inside the loop:
 *     A1 as written (vp pinned r5), -fno-gcse                    179, +4
 *     A1 with vp UNPINNED, -fno-gcse                              74, size EXACT
 *     the same, `vp = v` after `lim = 0x80 << 12`                   2, size EXACT
 *     + step pin order q0,q1,q2,shift                               0
 * The pin is what cost A1 its 179: unpinned, `vp` takes the ROM's r5 by itself.
 * `vp = v` before the `top:` label is worth 72 of the 74 on its own, because the
 * plus is then emitted outside the loop and cse1's table is flushed at the
 * label, so the FIRST call recomputes it too.
 *
 * MEASURED THIS BATCH, all at the 1-of-199 baseline, all exactly INERT at 1
 * with size 0 and relocations ok (so they are free to keep or drop):
 *   a decoy `(plus sfp -12)` in bb8 in six spellings -- cse1 DELETES a dead
 *   store to a pseudo before gcse sees it (confirmed at .03.cse: the decoy
 *   insns are gone), so the decoy route is structurally closed;
 *   `vp` for only the step block's reads; `vp` for the step and walk blocks;
 *   `vp` everywhere with an explicit `vp = v` after the stores; `vp` declared
 *   first.  WORSE at production: a two-word aggregate at the step site (2);
 *   `vp` pinned to r5 (157-186, +8).
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int gKeyHeld;
extern short L2430[] __asm__(".L2430");

extern unsigned char *__GetFieldActor(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern int __Func_8012038(int a, int b, int c);
extern int __Func_8011f54(int a, int b, int c);
extern void __vec3_translate(int len, int dir, int *v);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetAnimSpeed(unsigned char *a, int n);
extern void __Actor_WaitMovement(unsigned char *a);
extern void OvlFunc_921_2009f24(void);

void OvlFunc_921_2009fa4(void)
{
    unsigned char *a;
    unsigned char *b22;
    unsigned char *f;
    int v[3];
    unsigned int base;
    unsigned int off;
    int dir;
    int first;
    int t;
    int px;
    int pz;
    register int lim __asm__("r11");
    int *vp;

    base = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    base += off;
    a = __GetFieldActor(*(int *)base);
top:
    dir = L2430[(gKeyHeld >> 4) & 0xf];
    if (dir << 16 == (int)0xffff0000)
        return;
    __CutsceneStart();
    lim = 0x80 << 12;
    vp = v;
    vp[0] = (*(int *)(a + 8) & 0xfff00000) + lim;
    vp[1] = *(int *)(a + 0xc);
    vp[2] = (*(int *)(a + 0x10) & 0xfff00000) + lim;
    pz = vp[2];
    px = vp[0];
    b22 = a + 0x22;
    first = __Func_8012038(*b22, px, pz);
    __vec3_translate(0x80 << 13, dir, v);
    t = __Func_8012038(*b22, vp[0], vp[2]);
    if (t == 0xff)
        goto face;
    if (__Func_8011f54(*b22, vp[0], vp[2]) - *(int *)(a + 0xc) > lim)
        goto face;
    vp[0] = px;
    vp[2] = pz;
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x1999;
    { register int h3 __asm__("r3"); h3 = 0; *(short *)(a + 0x64) = h3; }
    __Actor_TravelTo(a, px, *(int *)(a + 0xc), pz);
    __Actor_SetAnim(a, 2);
    __Actor_SetAnimSpeed(a, 0x30);
    __Actor_WaitMovement(a);
    *(int *)(a + 0x6c) = (int)OvlFunc_921_2009f24;
    goto step;
face:
    *(short *)(a + 6) = dir;
    goto tail;
walk:
    if (__Func_8011f54(*b22, vp[0], vp[2]) - *(int *)(a + 0xc) > (0x80 << 12))
        goto settle;
    px = vp[0];
    pz = vp[2];
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x1999;
    __Actor_TravelTo(a, vp[0], vp[1], vp[2]);
    __Actor_WaitMovement(a);
    if (t != first)
        goto stop;
step:
    { register int q0 __asm__("r0"); register int q1 __asm__("r1");
      register int q2 __asm__("r2");
      q0 = 0x80; q1 = dir; q2 = (int)v; q0 <<= 13;
      __vec3_translate(q0, q1, (int *)q2); }
    t = __Func_8012038(*b22, vp[0], vp[2]);
    if (t != 0xff)
        goto walk;
settle:
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x80 << 9;
    __Actor_TravelTo(a, px, *(int *)(a + 0xc), pz);
    __Actor_WaitMovement(a);
    __WaitFrames(2);
    goto top;
stop:
    *(int *)(a + 0x6c) = 0;
    f = a + 0x5a;
    { register int m3 __asm__("r3"); m3 = 1; m3 |= *f; *f = m3; }
    *(int *)(a + 0x34) = 0x80 << 7;
tail:
    __WaitFrames(0xa);
    __CutsceneEnd();
}

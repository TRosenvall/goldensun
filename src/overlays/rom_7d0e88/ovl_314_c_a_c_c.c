/* asm/overlays/rom_7d0e88/ovl_314_c_a_c_c.s -- BOTH functions, MATCHED.
 *
 * Whole-file verification (objcmp's dump() on the FULL .s against the FULL .c,
 * via objcmp_whole.py in this directory):
 *
 *     ref 448 bytes / 202 encodings / 21 relocs ; ours 448 / 202 / 21
 *
 * and per function, with the alias map resolved against THIS OVERLAY's ELF
 * (objcmp_ovl.py, see "objcmp tool artifact" below):
 *
 *     OK OvlFunc_947_2009268 -- 328 bytes, 145 encodings and 18 relocations identical
 *     ~~ relocation _divsi3_RAM / __divsi3 is ONE symbol (same address in the linked ELF)
 *     OK OvlFunc_947_20093b0 -- 120 bytes, 57 encodings and 3 relocations identical
 *
 * Verified against the ORIGINAL path asm/overlays/rom_7d0e88/ovl_314_c_a_c_c.s.
 *
 * ============================ LANDING ============================
 *
 * THE WHOLE FILE LANDS AS ONE .c.  NO tools/split_s.py, NO linker edit.  The
 * one line in overlays/rom_7d0e88/overlay.ld that names the object --
 *
 *              asm/overlays/rom_7d0e88/ovl_314_c_a_c_c.o(.text)
 *
 * -- is already correct and must NOT be touched.  b237 planned a two-way split
 * because only 2009268 was solved; that plan is now superseded.
 *
 * ONE Makefile rule is required.  tryc.makefile_flags() returns the empty set
 * for src/overlays/rom_7d0e88/ovl_314_c_a_c_c.c and no pattern rule covers it,
 * so this is a NEW explicit target (explicit beats pattern, so nothing spreads
 * onto a neighbouring stem):
 *
 *     asm/overlays/rom_7d0e88/ovl_314_c_a_c_c.o: src/overlays/rom_7d0e88/ovl_314_c_a_c_c.c
 *     	$(GCC296_CC) $(ALIAS_CFLAGS) -S -o $(@:.o=.s) $<
 *     	printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
 *     	arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude -o $@ $(@:.o=.s)
 *
 * ALIAS_CFLAGS is -fno-strict-aliasing.  OvlFunc_947_2009268 was RE-MEASURED
 * under the flag before committing to it -- it is inert there and still gives
 * its OK line -- so the flag costs the sibling nothing.
 *
 * A FLAGLESS ALTERNATIVE EXISTS and is byte-identical too: whole_volatile.c in
 * this directory reaches the same 448/202/21 on DEFAULT flags by spelling three
 * lvalues `volatile`.  It is recorded, not recommended: it needs THREE volatile
 * casts on ordinary object fields (each measured load-bearing, table below),
 * which is a semantic lie about the struct, where the flag is a build fact and
 * already has ~20 users in this Makefile.  Use it only if some later TU-wide
 * constraint forbids ALIAS_CFLAGS here.
 *
 * ========================= THE FUNCTION ==========================
 *
 * 20093b0 is a per-frame integrator on one object: add velocity to position,
 * damp each velocity component by 1/10, 1/3, 1/10, add two more deltas, then
 * bump a halfword through a pointer the object holds at +0x50.
 *
 * `/` is safe here: overlays/rom_7d0e88/overlay.ld already carries
 * `__divsi3 = _divsi3_RAM;`.
 *
 * ========================= LEVER 1: NO LOCALS =====================
 *
 * b237 left this at "56 vs 56, ~17 differing, ALL LOAD SCHEDULING", tried the
 * eager and lazy orderings of three `int vx, vy, vz` locals, and swept the two
 * sched flags (23/19/20/17).  Re-measured here and confirmed: eager 20, lazy 20
 * (17 real + the 3 `bl` name lines).  Both are wrong for ONE reason.
 *
 * DELETE THE LOCALS.  Writing every access straight against the parameter --
 * `a[8/4] += a[0x44/4];` -- takes it from 20 to 8 and fixes the whole first
 * region on its own.
 *
 * MECHANISM, read out of the reload/sched behaviour rather than guessed.  The
 * two damped components live in HIGH registers (r8, r10), and thumb cannot
 * `ldr` into a high register, so reload inserts `ldr rLOW, mem` + `mov rHIGH,
 * rLOW` and sched2 -- which runs AFTER reload -- is free to pull the `ldr` up
 * into the load-use gap of the PREVIOUS component's add.  Which low register
 * reload picks decides whether that is legal:
 *
 *     ROM   ldr r2, [r5,#0x48]   <- r2 free across the add/str, so it hoists
 *     locals ldr r3, [r5,#0x48]  <- r3 holds the position being added, pinned
 *
 * With the locals in the source the pseudo is born early and reload hands it
 * the same scratch r3 the position chain is using, so nothing can move.  With
 * no locals the loads are born at their uses, reload picks r2, and sched2
 * produces the ROM's interleave by itself.  This is "NAMING A VALUE gcc ALREADY
 * CARRIES DESTROYS THE CARRY" in its scheduling form, and it is the same
 * reload-scratch reading as elevation.md's "A reload SCRATCH register is a
 * statement-order tell".
 *
 * ================== LEVER 2: -fno-strict-aliasing ==================
 *
 * The residue after lever 1 is 5 real diffs, both of them a LOAD HOISTED ABOVE
 * A STORE that the ROM leaves below:
 *
 *     ldr r3, [r5,#0x18]  above  str r2, [r5,#0x4c]      (swap)
 *     ldr r1, [r5,#0x50]  above  add r3,r2 / str r3,[r5,#0x1c]
 *
 * This is elevation.md's "A PARTIAL REORDER IS A PRIORITY TIE" exactly, and
 * -fsched-verbose=6/8 (the recorded probe for this class) reads it out:
 *
 *     insn  code  bb  dep  prio  cost           forward dependences
 *      125   173   0    6    43     2   190 189 152 130          str [r5+0x4c]
 *      128   173   0    4    43     2   190 189 152 134 132      ldr [r5+0x18]
 *
 * PRIORITY TIES AT 43.  reload_completed kills the register-pressure test, the
 * block is single so the interblock tests are skipped, and both come out class
 * 3 against the last-scheduled insn -- so rank_for_schedule falls all the way
 * to the DEPENDENT COUNT, 5 against 4, and the load wins the slot.  Had it
 * reached the final INSN_LUID tie-break the store would have won, because the
 * store is the earlier insn.  So the whole diff is ONE dependent.
 *
 * The dependent is a MEMORY dependence that strict aliasing deletes.  The
 * halfword pair at the end is reached through `unsigned short *`, the stores at
 * +0x18/+0x1c/+0x4c through `int`; different alias sets, so
 * `DIFFERENT_ALIAS_SETS_P` returns early and gcc never asks whether
 * `[r1+0x1e]` might be `[r5+0x4c]`.  With -fno-strict-aliasing the sets all
 * collapse to 0, the `ldrh`/`strh` gain true/output dependences on all three
 * int stores, the store's priority jumps clear of the load's, and BOTH residual
 * hoists disappear at once.  5 -> 0.
 *
 * WHY NO SOURCE SPELLING REPLACES THE FLAG (except volatile).  Alias sets can
 * only REMOVE dependences, never add them: `true_dependence` consults them
 * before `memrefs_conflict_p`, and once it gets there, two `(plus (reg r5) K)`
 * addresses with different K are provably distinct whatever their alias sets
 * are.  So the recorded "give EACH store its own struct tag" lever -- which
 * BUYS SEPARATION -- is the opposite of what is wanted here and cannot work;
 * measured, a struct tag on the halfword pair is inert (8, unchanged).  The one
 * source-level escape is `MEM_VOLATILE_P(x) && MEM_VOLATILE_P(mem) -> 1`, which
 * short-circuits ABOVE the alias-set test.  That is why whole_volatile.c works
 * and why it needs volatile on BOTH sides of each pair it wants ordered.
 *
 * ==================== MEASURED WORSE / INERT =======================
 *
 * All figures are norm.py instruction-stream diffs against the ROM; 3 of them
 * are always the `bl _divsi3_RAM` / `bl __divsi3` name lines, so 3 == matched.
 *
 *   spelling / flag                                     diff
 *   -------------------------------------------------   ----
 *   THIS FILE (no locals) + -fno-strict-aliasing           3   <- ships
 *   whole_volatile.c, default flags                        3   <- alternative
 *   no locals, default flags                               8
 *   no locals + -fno-gcse                                  8   inert
 *   no locals + -fno-rerun-cse-after-loop                  8   inert
 *   no locals + -fno-strength-reduce                       8   inert
 *   no locals + -fno-schedule-insns                        8   inert (sched1
 *                                                              does nothing;
 *                                                              this is a sched2
 *                                                              function)
 *   no locals, struct tag on the halfword pair (tag1.c)    8   inert -- see
 *                                                              "WHY NO SOURCE
 *                                                              SPELLING"
 *   no locals, second `+=` operands swapped (t2.c)         8
 *   no locals, first `+=` operands swapped (t1/t3.c)      10
 *   no locals, halfword written out longhand (t5.c)       10
 *   no locals + -fno-schedule-insns2                      14   WORSE: it is
 *                                                              sched2 that
 *                                                              builds the ROM's
 *                                                              region-1
 *                                                              interleave
 *   no locals + -O1                                       14
 *   int vx,vy,vz lazy (b237 sib2)                         20
 *   int vx,vy,vz eager (b237 sib1)                        20
 *   halfword pointer bound to a local (t4.c)              20   and 57 insns
 *   full struct type for the object (v_struct.c)          23
 *
 * The volatile set is minimal -- each of the three is load-bearing by deletion:
 *
 *   volatile on ...                                     diff
 *   -------------------------------------------------   ----
 *   halfword pair only                                     8
 *   halfword pair + [0x4c] store                           6
 *   halfword pair + [0x1c] store                           5
 *   halfword pair + both stores                            3   <- minimal
 *   halfword LOAD only (store plain) + both stores         5
 *   halfword pair plain, both stores volatile              6
 *
 * ================== NEW: an objcmp tool artifact ===================
 *
 * NOT in elevation.md (grepped for setdefault / _alias_addrs / same_symbol /
 * "alias map" / divsi3_RAM before writing this).  It is a THIRD shape of the
 * recorded "objcmp CAN REPORT A BYTE-IDENTICAL MATCH AS FAILING", with a
 * different cause from the _call_via_fp/_call_via_r11 one.
 *
 * objcmp._alias_addrs() walks goldensun.elf FIRST and then the overlay ELFs,
 * recording each name with setdefault.  `__divsi3` is therefore bound to the
 * MAIN-ROM copy at 0x080022ec and never re-bound.  An overlay's own
 * `__divsi3 = _divsi3_RAM;` -- the alias the whole "overlay divide-alias" sweep
 * installed -- is invisible to it, so same_symbol() says no and EVERY overlay
 * function that divides reports
 *
 *     XX RELOCATIONS differ
 *
 * with SIZE and ENCODINGS SILENT, on a byte-identical object.  Proof for this
 * one, from the ELF that actually governs the link:
 *
 *     $ arm-none-eabi-nm overlays/rom_7d0e88/overlay.elf | grep divsi3
 *     0200aa2c T __divsi3
 *     0200aa2c T _divsi3_RAM
 *
 * Same address.  objcmp_ovl.py in this directory is the workaround: it seeds
 * objcmp._ALIASES from the overlay ELF named in $ELF and then defers to
 * objcmp.main(), which is what produced the OK lines quoted at the top.
 *
 * The durable fix, for whoever touches tools/objcmp.py next: resolve the alias
 * map against the ELF that owns the reference .s (asm/overlays/<ov>/... ->
 * overlays/<ov>/overlay.elf) instead of first-wins across every ELF, or at
 * least record BOTH addresses per name and treat a match on ANY as a match.
 * Until then, an overlay function that divides cannot be cleared by objcmp
 * alone -- check the overlay's own nm output by hand.
 *
 * ==================== PRE-WORK / provenance ========================
 *
 * The .s carries no data of its own; the only pool words are the assembler's,
 * and the whole-file comparison above covers them.  20093b0's three pool-free
 * `bl`s are its only relocations.
 */
extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern void __vec3_translate(int a, int b, int *v);
extern int __TestCollision(unsigned char *e, int *v);
extern int OvlFunc_947_2008350(int *v, unsigned char *e);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_SetAnim(unsigned char *e, int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(unsigned char *e, int f);
extern void __Func_8092158(int a, int b, int c);

int OvlFunc_947_2009268(void)
{
    int v[3];
    int *p;
    unsigned char *e;
    unsigned char *f;
    int saved;
    int n1;

    e = __MapActor_GetActor(0);
    f = e + 0x55;
    saved = *f;
    n1 = ((*(unsigned short *)(e + 6)) + (0x80 << 6)) & (0xc0 << 8);
    if (gState[0x1f2] != 0) {
        return 0;
    }
    p = v;
    p[0] = (*(int *)(e + 8) & 0xfff00000) + (0x80 << 12);
    p[1] = *(int *)(e + 0xc);
    p[2] = (*(int *)(e + 0x10) & 0xfff00000) + (0x80 << 12);
    __vec3_translate(0x80 << 13, n1, p);
    if (__TestCollision(e, p) == 1) {
        goto fail;
    }
    if (OvlFunc_947_2008350(p, e) != 0) {
        goto fail;
    }
    p[0] = (*(int *)(e + 8) & 0xfff00000) + (0x80 << 12);
    p[1] = *(int *)(e + 0xc);
    p[2] = (*(int *)(e + 0x10) & 0xfff00000) + (0x80 << 12);
    __vec3_translate(0x80 << 14, n1, p);
    if (OvlFunc_947_2008350(p, e) != 0) {
        goto fail;
    }
    if (__TestCollision(e, p) != 0) {
        goto fail;
    }
    __CutsceneStart();
    __Actor_SetAnim(e, 6);
    __WaitFrames(6);
    __PlaySound(0x98);
    __Actor_SetAnim(e, 7);
    *(int *)(e + 0x30) = 0xc0 << 10;
    *(int *)(e + 0x34) = 0x80 << 10;
    *(int *)(e + 0x28) = 0x80 << 11;
    *f = *f & 0x7e;
    __Actor_SetSpriteFlags(e, 0);
    __Func_8092158(0, *(short *)((char *)p + 2), *(short *)((char *)p + 0xa));
    __Actor_SetAnim(e, 6);
    __Actor_SetSpriteFlags(e, 1);
    *f = saved;
    __CutsceneEnd();
    return 1;
fail:
    return 0;
}

void OvlFunc_947_20093b0(int *a)
{
    a[8 / 4] += a[0x44 / 4];
    a[0xc / 4] += a[0x48 / 4];
    a[0x10 / 4] += a[0x4c / 4];
    a[0x44 / 4] -= a[0x44 / 4] / 10;
    a[0x48 / 4] -= a[0x48 / 4] / 3;
    a[0x4c / 4] -= a[0x4c / 4] / 10;
    a[0x18 / 4] += a[0x30 / 4];
    a[0x1c / 4] += a[0x34 / 4];
    *(unsigned short *)(a[0x50 / 4] + 0x1e) += *(unsigned short *)((char *)a + 0x64);
}

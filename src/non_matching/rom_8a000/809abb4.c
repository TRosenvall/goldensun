/* ===================== BATCH 297a DELTA -- Field_Halt =====================
 *
 * ===== OWNER DECISION, 2026-10-04: per-file `-ffixed-r11` DECLINED. =====
 * This function is BYTE-IDENTICAL under a per-file FIXEDR11_CFLAGS rule, and that
 * was refused.  A per-file flag asserts something about how the original object
 * was built, and the counter-evidence is in this bank: the landed twin
 * Field_Whirlwind (src/rom_8a000/rom_9a44c_c_c_a_a.c, byte-exact) USES fp FREELY.
 * Park at the flag-free figure instead.  One encoding is a cheap price for not
 * making an unsupported assertion about the build.
 * Full reasoning and the revisit condition: docs/owner-decisions.md.
 *
 * AND THE FLAG-FREE BODY IS NOW INSTALLED, 2 -> 1, ON ONE TOKEN:
 *     was   for (; i < 11; i++)
 *     now   for (; i != 11; i++)
 * This is why the park's ELEVEN literal spellings all measured 2 and could not
 * reach it: combine's `simplify_comparison` rewrites `LT C>0` into `LE C-1`, so
 * every `<` form collapses to the same comparison -- and `!=` is not subject to
 * that transformation at all.  The park's own conclusion that the bound was
 * "unreachable from any literal" was therefore true OF LITERALS and false of the
 * operator.  Device-free, 0 pins.
 *
 * NON-MATCHING, 1 of 191 encodings (batch 322; was 2 at the batch-319 backfill).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_8a000/809abb4.c \
 *     asm/rom_8a000/rom_9a44c_c_c_c.s --func Field_Halt
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * RE-MEASURED batch 321: 2 of 191 (ref 191 enc / 444 bytes / 30 rel, ours the same);
 * NOW 1, on the `!=` bound recorded above.
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
    for (; i != 11; i++) {
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

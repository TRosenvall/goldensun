/* PARKED -- OvlFunc_common0_10c, objcmp 212 of 223 / --align 39 of 233
 * NON-MATCHING, 212 encodings of 223.  NOT a distance (ref 472 bytes / 223 encodings against ours 464 / 219, four instructions short).
 * READ `--align`: 39 of 233.
 *
 * THE SPLIT IS NOT A ONE-LINE REHOME.  common0_c.o is named three times -- (.text), (.data) and
 * (.data1) -- in each of NINETEEN overlays/<ovl>/overlay.ld, so 57 edits, and in four of them the
 * .data and .data1 lines are not adjacent, so each must be replaced in place.  Stems
 * common0_c_a / common0_c_b are free.  One export is required and it must be ADDED: the file
 * carries zero .global lines today, and `.L4` is the only data label the function reads (the
 * other thirteen `.L` tokens in its body are its own branch targets, and .L1/.L2/.L3 are reached
 * only from .L4's own .words inside the data object).  So the "already exports" set and the
 * "split requires" set are disjoint here, exactly as datacheck.py's docstring warns.
 *
 * HAZARD: the short-`.LN` extern currently binds only by luck -- gcc emits .L2, .L3 and .L8..L21
 * for this candidate and happens to skip .L4.  The safe form is the recorded alias
 * `_TBL_L4 = .L4;` in all nineteen scripts.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_common/common0_10c.c \
 *     asm/overlays/common/common0_c.s --func OvlFunc_common0_10c
 *   [asm/overlays/common/common0_c.s -- the file's ONLY function, confirmed
 *   with `grep -ci func_start` = 1.  220 instructions.]
 *
 * VERDICT
 *   objcmp --func -- SIZE ref 472 / ours 464, 212 of 223 encodings differ
 *                    (ours 219).  NOT a true distance: we are FOUR instructions
 *                    short, so every position after index 7 is shifted and 212
 *                    is a positional artefact.
 *   --align       -- 39 instruction(s) in disagreeing regions, of 233.  This is
 *                    the working number.
 *
 * Verify with:
 *   python3 tools/tryc.py src/non_matching/ovl_common/common0_10c.c \
 *       --ref asm/overlays/common/common0_c.s --align
 *   python3 tools/objcmp.py src/non_matching/ovl_common/common0_10c.c \
 *       asm/overlays/common/common0_c.s --func OvlFunc_common0_10c
 *
 * ================ THE SPLIT, CONFIRMED INDEPENDENTLY OF datacheck ============
 *
 * The .s carries `.section .data` (.L1/.L2/.L3, three 0x38-byte `.incbin`
 * script blobs out of overlays/rom_78ef88/orig.bin) and `.section .data1`
 * (.L4, three `.word`s at those blobs).  So a TEXT/DATA SPLIT is required:
 *   src/overlays/common/common0_c_a.c   the function
 *   asm/overlays/common/common0_c_b.s   BOTH data sections, unchanged
 * Both stems are FREE: no file of either name exists under asm/ or src/, and no
 * .ld line anywhere names either object.
 *
 * ONE EXPORT, AND IT IS `.global .L4`.  `tools/datacheck.py` says so; checked
 * independently by listing every `.L` token inside the function body
 * (`sed -n '/thumb_func_start/,/func_end/p' | grep -oE '\.L[0-9a-f]+' | sort -u`):
 * thirteen of the fourteen names are the function's OWN branch targets
 * (.L17a .L17e .L18e .L21c .L248 .L25c .L28a .L2a0 .L2a6 .L2c2 .L2d4 .L2e6
 * .L2f6) and `.L4` is the only data label.  `.L1`, `.L2` and `.L3` are reached
 * ONLY from `.L4`'s three `.word`s, which stay inside the data object, so they
 * need no export.  The file today carries **zero** `.global` lines
 * (`grep -c '\.global'` = 0), so `.global .L4` has to be ADDED to the new `_b.s`
 * -- it is not already there, and the "EXPORTS" set and the "split requires"
 * set are here disjoint, exactly as datacheck's own docstring warns.
 *
 * THE COMMON-OVERLAY LINK SHAPE: 19 SCRIPTS x 3 LINES = 57 EDITS.
 * `grep -rc 'common0_c\.o' --include='*.ld' .` returns **3** for each of
 * NINETEEN overlay.ld files (rom_793768, 797990, 798dc4, 79aad8, 78ef88,
 * 7a5214, 7aa430, 7ac2d8, 7b0400, 7b2078, 7b6668, 7b7790, 7bc690, 7bdeb0,
 * 7c460c, 7d0e88, 7e636c, 7eaf28, 7f6e64) -- one `(.text)` line, one `(.data)`
 * and one `(.data1)`.  No `.ld` outside overlays/ names it; stage1.ld and
 * goldensun.ld do not.  So this is NOT the ordinary one-line rehome:
 *     common0_c.o(.text)   -> common0_c_a.o(.text)     x19
 *     common0_c.o(.data)   -> common0_c_b.o(.data)     x19
 *     common0_c.o(.data1)  -> common0_c_b.o(.data1)    x19
 * and the three lines are NOT adjacent in several of them (rom_7bdeb0 has .data
 * at 69 and .data1 at 76; rom_7d0e88 at 85 and 87; rom_7ac2d8 at 117 and 120;
 * rom_7a5214 at 66 and 70), so each must be replaced IN PLACE, keeping its own
 * position.  That is the shared-code cost of common/: the same bytes appear
 * nineteen times in the ROM and every copy has to be re-pointed.
 *
 * THE SHORT-`.LN` CAPTURE HAZARD IS REAL HERE AND IS CURRENTLY MISSED BY LUCK.
 * `.L4` is exactly the kind of name elevation.md warns about.  On THIS candidate
 * gcc emits .L2 .L3 .L8 .L9 .L10 .L11 .L12 .L14 .L15 .L16 .L18 .L19 .L20 .L21
 * and skips .L4 .L5 .L6 .L7 .L13 .L17, so `extern ... __asm__(".L4")` binds to
 * the data label and the pool word is right.  That is an accident of how many
 * labels this particular source shape makes, and it must be re-checked on
 * whatever finally lands (`grep -oE '^\.L[0-9]+:' out.s`).  The safe form is the
 * recorded linker alias `_TBL_L4 = .L4;` added to all nineteen scripts.
 *
 * ================================ THE BLOCKER ================================
 *
 * REGISTER PRESSURE: THE ROM SPILLS TWO VALUES AND WE SPILL NONE.  The ROM's
 * frame is `sub sp, #8` holding (a) parameter 4 at [sp,#4], stored at entry and
 * reloaded for `e->f44 = dx`, and (b) the address `e + 0x64` at [sp,#0], stored
 * once and reloaded in the 0x800000 arm.  Our build keeps both in callee-saved
 * registers and is therefore FOUR INSTRUCTIONS SHORT (no `sub sp`/`add sp`, no
 * two stores, no two reloads, net -4 after the extra `mov`s we gain).  The high
 * registers go to different tenants all the way through as a result: the ROM has
 * r8=z r9=MapActor r10=flags r11=(flags&0xf)<<2, we have r8=flags r10=z
 * r11=MapActor.  Everything after index 7 in the diff follows from that, so the
 * 39 is one decision, not 39.
 *
 * WHAT WAS FOUND AND IS KEPT (each a single drop from the finished base):
 *
 * 1. TWO `__CreateActor` CALL SITES, NOT AN `id` LOCAL -- 137 -> 80 aligned.
 *    The tell is `mov r2, r6` (the y argument) appearing in BOTH arms of the
 *    id selection.  One call with `id = cond ? p->f18 : 0xde` emits that once
 *    after the join.  Two calls plus jump.c's cross-jumping merges only the
 *    common tail `mov r1,r5 / mov r3,r8 / bl / mov r6,r0`, and stops before the
 *    `mov r2,r6` because the two arms order their last two insns differently.
 *    **A duplicated argument setup in both arms of an if/else is the signature
 *    of two call sites cross-jumped, and it cannot be reached from one call.**
 *
 * 2. THE `|` OPERANDS OF THE f9 BITFIELD COPY ARE ORDERED -- 80 -> 48, and
 *    71 -> 39 on the pointer variant.  `o->f9 = (m->f50->f9 & 0xc) |
 *    (o->f9 & ~0xc)` beats `(o->f9 & ~0xc) | (m->f50->f9 & 0xc)` even though
 *    `|` is commutative and both spellings compute the same value in the same
 *    registers.  This is a counter-example to the recorded scope of the mul
 *    lever ("it does not carry to `and` or to a commutative `add` at all"):
 *    for `|` at a byte-field read-modify-write it carries, and the procedure is
 *    still only "measure both".
 *
 * 3. A NAMED `struct Script **` FOR THE TABLE SLOT -- 80 -> 71 / 48 -> 39.
 *    `sp = &L4[flags & 0xf]` then `*sp` at both uses, rather than
 *    `L4[flags & 0xf]` twice or a named `int` index.  It costs one instruction
 *    (`add r5, r3, r2 / ldr r1, [r5]` where the ROM has `ldr r1, [r2, r3]`) and
 *    buys back more downstream.  On the finished shape the `int si` form is 48
 *    and the bare repeated subscript 48 as well, so the pointer is the lever.
 *
 * MEASURED AND REJECTED (full 2x2x2 sweep of {pointer, int index} x {named
 * mask, literal mask} x {swapped `|`, natural `|`}, all on the two-call base):
 *   pointer, literal mask, swapped   39   <- ships
 *   pointer, named mask,   swapped   45
 *   int idx, literal mask, swapped   48
 *   int idx, named mask,   swapped   55
 *   pointer, literal mask, natural   71
 *   int idx, literal mask, natural   80
 *   int idx, named mask,   natural   82
 *   pointer, named mask,   natural   73
 * and, on the one-call base: a named `unsigned short *` for `&e->f64` -- 137,
 * i.e. exactly inert, gcc had already CSE'd the address.
 *
 * ON THE `~0xc` MASK, WHICH IS STILL WRONG AND IS 2 OF THE 39.  The ROM builds
 * the mask as `mov r3,#0xd / neg r3,r3` (0xfffffff3) and keeps it in r9 across
 * both read-modify-writes; we get `mov r3,#0xf3`, because combine narrows
 * `(and (zero_extend (mem:QI)) (const_int -13))` to QImode using the load's
 * nonzero_bits.  A `int m9 = ~0xc;` local was the hypothesis -- cse will not
 * substitute a CONST_INT whose `arm_rtx_costs` is COSTS_N_INSNS(2) for a REG
 * that costs 1, so the AND would stay register-register and combine could not
 * narrow it -- and it MEASURED WORSE at every combination (45 against 39, 55
 * against 48).  So the mechanism is right and the spelling is not the handle;
 * the mask width is downstream of the same allocation decision as the spills.
 *
 * NEXT LEVER TO TRY, unattempted here: the ROM's `mov r5, #3` for the
 * `(p->f0 & 3)` mask is materialised in a CALLEE-SAVED register before the
 * `__Func_80929d8` call, i.e. it is a live value across a call that we do not
 * have.  That is one more long-lived quantity, and one more is exactly what the
 * spills need.  It is the only source-visible route to the pressure found.
 *
 * SHIMS: NONE (both classes checked separately).
 */
struct Sub {
    unsigned char pad0[9];
    unsigned char f9;
    unsigned char pad0a[0x1e - 0xa];
    unsigned short f1e;
    unsigned char pad20[0x26 - 0x20];
    unsigned char f26;
};

struct Ent {
    unsigned char pad00[0x18];
    int f18;
    int f1c;
    unsigned char pad20[0x23 - 0x20];
    unsigned char f23;
    unsigned char pad24[0x30 - 0x24];
    int f30;
    int f34;
    unsigned char pad38[0x44 - 0x38];
    int f44;
    int f48;
    int f4c;
    struct Sub *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    unsigned short f64;
    unsigned char pad66[0x6c - 0x66];
    void (*f6c)(struct Ent *);
};

struct Script {
    unsigned char pad0[0xc];
    int fc;
};

struct Param {
    unsigned char f0;
    unsigned char pad01[3];
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    short f18;
    unsigned char pad1a[2];
    struct Script *f1c;
    unsigned short f20;
    unsigned short f22;
    void (*f24)(struct Ent *);
};

extern struct Script *L4[] __asm__(".L4");

extern struct Ent *__MapActor_GetActor(int slot);
extern struct Ent *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetAnim(struct Ent *e, int anim);
extern void __Actor_SetScript(struct Ent *e, struct Script *s);
extern void __Func_80929d8(struct Ent *e, int n);
extern void OvlFunc_common0_d4(struct Ent *e);

void OvlFunc_common0_10c(int x, int y, int z, int dx, int dy, int dz,
                         int flags, struct Param *p)
{
    struct Ent *e;
    struct Sub *o;
    struct Ent *m;
    struct Script *s;
    struct Script **sp;

    m = __MapActor_GetActor(0);
    if ((flags & 0x100000) != 0 && p != 0)
        e = __CreateActor(p->f18, x, y, z);
    else
        e = __CreateActor(0xde, x, y, z);
    if (e == 0)
        return;
    o = e->f50;
    __Actor_SetAnim(e, (flags + 1) & 0xf);
    sp = &L4[flags & 0xf];
    __Actor_SetScript(e, *sp);
    e->f55 = 0;
    o->f26 = 0;
    e->f6c = OvlFunc_common0_d4;
    e->f44 = dx;
    e->f48 = dy;
    e->f4c = dz;
    o->f9 = (m->f50->f9 & 0xc) | (o->f9 & ~0xc);
    e->f30 = 0;
    e->f34 = 0;
    e->f64 = 0;
    if ((flags & 0xffff0000) == 0 || p == 0)
        return;
    if ((flags & 0x10000) != 0)
        __Func_80929d8(e, p->f4);
    if ((flags & 0x20000) != 0) {
        e->f23 &= 0xfe;
        o->f9 = ((p->f0 & 3) << 2) | (o->f9 & ~0xc);
    }
    if ((flags & 0x80000) != 0) {
        e->f18 = p->f8;
        e->f1c = p->fc;
    }
    if ((flags & 0x40000) != 0) {
        s = *sp;
        if ((flags & 0x80000) != 0) {
            e->f30 = (p->f10 - e->f18) / s->fc;
            e->f34 = (p->f14 - e->f1c) / s->fc;
        } else {
            e->f30 = (p->f10 - 0x10000) / s->fc;
            e->f34 = (p->f14 - 0x10000) / s->fc;
        }
    }
    if ((flags & 0x200000) != 0) {
        __Actor_SetAnim(e, 1);
        __Actor_SetScript(e, p->f1c);
    }
    if ((flags & 0x400000) != 0)
        o->f1e = p->f20;
    if ((flags & 0x800000) != 0)
        e->f64 = p->f22;
    if ((flags & 0x1000000) != 0)
        e->f6c = p->f24;
}

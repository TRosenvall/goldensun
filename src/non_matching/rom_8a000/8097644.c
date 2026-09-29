/* Task_08097644 -- NON-MATCHING, 86 of 246 encodings differ.
 * Unattempted before batch 298.  Reference asm/rom_8a000/rom_97384_c_a_c_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_8a000/8097644.c \
 *       asm/rom_8a000/rom_97384_c_a_c_a.s --func Task_08097644
 *
 * NOT a distance: size 544 against 548 and count 245 against 246 -- ONE SHORT -- so 86
 * is saturated.  The real view is aligncmp: 197 of 246 (80.1%), 56 differing in 28
 * hunks.  SHIMS: 0.  Whole-file replacement when it closes.
 * Encodings 0-52 are BYTE-IDENTICAL -- the prologue, the early-out, the wave index and
 * the entire 160-iteration sin loop -- and everything after is a one-register shift
 * plus three count deltas that all follow from it.
 * BLOCKER, proven from the dumps rather than inferred: reload's allocate_reload_reg
 * round robin on last_spill_reg.  The constant is ONE rtx at .17.lreg and is split by
 * a reload-generated insn at .18.greg, while the same constant's earlier use agrees
 * with the ROM.  Inert across 15 source spellings and 5 flag variants.
 */
/* PARK -- Task_08097644  --  0x08097644, asm/rom_8a000/rom_97384_c_a_c_a.s
 * (SINGLE function in the file: a whole-file replacement, no split needed.
 *  stage1.ld names asm/rom_8a000/rom_97384_c_a_c_a.o(.text) on one line.
 *  `python3 tools/datacheck.py asm/rom_8a000/rom_97384_c_a_c_a.s` prints NOTHING.)
 *
 * NOT MATCHING.  objcmp:
 *     XX SIZE  ref 548 bytes, ours 544
 *     XX ENCODINGS differ in 86 place(s) (ref 246, ours 245)
 *   The 86 is NOT a distance -- the instruction counts differ, so objcmp's count
 *   saturates.  The alignment-tolerant figure is the real one:
 *     tools/aligncmp.py: aligned-equal 197 (80.1% of ref), 56 differing/ins/del
 *     in 28 hunks.
 *   SIZE 544 vs 548 (4 bytes / 1 encoding short).  shimcount.py: 0 shims.
 *
 * THE BODY IS RIGHT.  Encodings 0..52 -- the prologue, the +0x294 early-out, the
 * wave-buffer index and the whole 160-iteration sin loop -- are BYTE-IDENTICAL,
 * and every one of the 28 hunks after that is either a register NAME or one of
 * exactly three instruction-count deltas, all of which are downstream of the
 * same allocator decision.  No field offset, no expression, no control-flow edge
 * is in doubt.
 *
 * ============ RESIDUE, ALL THREE ITEMS, ONE ROOT CAUSE ============
 *
 * The visible pattern is a ONE-REGISTER SHIFT that starts at encoding 53 and
 * never recovers: where the ROM picks r4 we pick r0, where it picks r0 we pick
 * r1, where it picks r1 we pick r2.  The ROM uses r4 five times; our object
 * never mentions r4 at all.  The three count deltas all fall out of it:
 *
 *  R1. `ldr r2, =0x28b` in the ROM vs `sub r1, r1, #2` in ours (the third of the
 *      three signed bytes at +0x28b/+0x28c/+0x28d that get packed for
 *      Func_8091200).  This is reload_cse_move2add (postreload.c): it rewrites
 *      `set rN, 651` into `set rN, rN - 2` when rN already holds 653.  It fires
 *      for us because OUR reload put both 0x28d and 0x28b in r1; in the ROM they
 *      are in r0 and r2, so move2add cannot see the relation.  The ROM's spare
 *      pool word `.word 0x0000028b` is the only pool difference in the function.
 *      Not a source question -- a consequence of the register pair.
 *  R2. The ROM spends an extra `mov r4, r8` before `ldr r1, [r4]` at
 *      `_CreateActor(0x119, v2[0])`, because ITS copy of the v2 base sat in r0
 *      and `ldr r0, =0x119` destroyed it.  Ours put that copy in r1, which is
 *      also _CreateActor's second argument register, so `ldr r1, [r1]` needs no
 *      second copy.  We are one instruction SHORT here.
 *  R3. Ours spends an extra `mov r2, r8` inside the atan2 argument setup, for the
 *      mirror-image reason: the ROM has r2 = v2-base and r1 = v1-base (so
 *      `ldr r1,[r1]` can clobber the v1 base harmlessly), ours has them swapped
 *      and must rebuild the v2 base.  We are one instruction LONG here.
 *
 * ============ THE BLOCKER, NAMED BY THE PASS ============
 *
 * RELOAD -- reload1.c `allocate_reload_reg`, its `last_spill_reg` round-robin.
 * From the `-da` dumps (kept in scratch_elev/b298d/da/): at .17.lreg the insn is
 * still one RTX,
 *     (insn 219 (set (reg:SI 126) (plus:SI (reg/v:SI 32) (const_int 650))))
 * and no separate constant pseudo exists.  At .18.greg it has been SPLIT, and the
 * split insn is reload-generated:
 *     (insn 816 (set (reg:SI 0 r0) (const_int 650)))     <- reload chose r0
 *     (insn 219 (set (reg:SI 1 r1) (plus (reg r7) (reg r0))))
 * The FIRST use of the same constant, 170 encodings earlier, is
 *     (insn 807 (set (reg:SI 1 r1) (const_int 650)))
 * and there the ROM agrees with us on r1.  So local-alloc and global-alloc agree
 * with the ROM everywhere; the divergence is entirely in which spill register
 * reload hands out, and `allocate_reload_reg` walks `spill_regs` from a cursor
 * (`last_spill_reg`) that persists across the whole function.  One extra or one
 * missing reload anywhere earlier rotates every later choice, which is exactly
 * the off-by-one signature above.  Since encodings 0..52 are byte-identical, the
 * desynchronisation is invisible in the output until the first wrap.
 *
 * WHAT THAT MEANS FOR RE-OPENING: this cannot be steered by naming variables or
 * reordering statements -- it needs a source change that alters the NUMBER of
 * reloads in the byte-identical opening, which by definition leaves that opening
 * byte-identical.  The productive probe is to look for an offset or constant in
 * encodings 0..52 that the ROM materialises with one more (or one fewer) insn
 * pair than we do and that we happen to be matching for a different reason.
 *
 * ============ MEASURED, ALL INERT AT 197/56 -- DO NOT RE-RUN ============
 *
 *   flags (each alone, on top of production):  -fno-schedule-insns2 (worse, 128),
 *     -fno-gcse (84), -fno-strict-aliasing (96), -fno-rerun-cse-after-loop
 *     (90 and 247 insns), -fno-schedule-insns (86).  None changes the shift.
 *     PRODUCTION FLAGS are what the 197/56 figure above is measured at.
 *   `signed char f28b/f28c` members vs `unsigned char` + `(signed char)` casts:
 *     BYTE-IDENTICAL output.  gcc only reaches for `ldrsb` when the offset is
 *     already zero from a materialised base; at +0x28b/+0x28c it emits
 *     `ldrb / lsl #24 / asr #24` either way, so this .s CANNOT distinguish the
 *     two declarations.  (Recorded: the signedness of a char member is not
 *     readable off a large-offset load.)
 *   0x200000 as the first OR term instead of the last: inert.
 *   the three packed bytes read into named locals first: 180 differing, worse.
 *   f28b's term written first (reversing the OR): 192 aligned, worse.
 *   dropping the `t` local (three inline `b->f295 ==` compares): inert -- gcc
 *     cse's the load, so the local was never a pseudo.
 *   dropping the `h` local; adding an `ang` local for the atan2 result;
 *     `struct Vec {x,y,z}` instead of `int[3]` for both vectors: all inert.
 *   swapping the two array declarations: 184, worse (and the slots swap, so
 *     v1-declared-first IS confirmed by the frame: v1 at sp+0xc, v2 at sp+0).
 *   named `int *u, *z` pointers to the two arrays: 191 and 247 insns, worse.
 *   `k = 1 ^ b->f28a` as its own statement; `&b->wave[k][0]`;
 *     `(short *)b + k * 162`; `if (b->f294)` / `b->f294 -= 1`;
 *     the loop as do/while; `i <= 0x9f`; a named `q` for the divide result:
 *     ALL inert.  `int i` instead of `unsigned int i`: 196, worse.
 *
 * ============ WHAT THE REFERENCE ESTABLISHES (all reproduced) ============
 *
 *  - The state block is `iwram_3001ea8` DEREFERENCED, and the sibling
 *    src/rom_8a000/rom_97384_c_a_c_b.c already declares it (`extern Blk
 *    *iwram_3001ea8;`) with f28a at +0x28a and f294 at +0x294, and already proves
 *    the record stride: it writes `i = i * 81; off = i * 4;`.  81 words = 324
 *    bytes, and 2 * 324 = 0x288, so the wave buffer is `short wave[2][162]` and
 *    +0x288 is the first scalar after it.  The ROM's
 *    `lsl r3,r2,#2 / add r3,r2 / lsl r3,#4 / add r3,r2 / lsl r3,#2` is gcc's
 *    synthesised *324 and needs nothing in the source.
 *  - `mov r0,#0 / ldrsh r0,[r3,r0]` and `mov r2,#8 / ldrsb r2,[r0,r2]` are NOT
 *    hand-written zero/eight indices: Thumb-1 `ldrsh`/`ldrsb` have no immediate
 *    form, so every signed narrow load costs an index register.
 *  - `mov r3,#0xd / neg r3,r3` is ~0xc, not ~0xd -- a 2-bit bitfield at bit 2 of
 *    byte +9 of the actor's +0x50 sub-object, set to 1 (docs/elevation.md, "A run
 *    of separate constant ANDs on one loaded byte is the BITFIELD tell").
 *  - `ldr r2, .L97814 @ 0` is a POOLED zero for a BYTE store (`strb`), the QImode
 *    twin of the recorded HImode rule; the neighbouring `n->f55 = 0` gets
 *    `mov r3,#0` because it is an SImode zero narrowed at the store.
 *  - `_CreateActor` is called with TWO arguments here (r2/r3 are never set up),
 *    although four other elevated files in this bank declare it with four.  The
 *    local two-parameter extern is required and is what the call site says.
 *  - `lsl r3,#24 / cmp r3, r0(0xf0<<22)` is `(unsigned char)(f295 + 1) > 0x3c`
 *    reusing the incremented value rather than reloading the byte.
 *
 * The `@` prose in the reference ("EncounterTransitionTask ... recomputes the
 * per-scanline distortion") is consistent with what the body does; unlike
 * rom_944ec_a_a_a_a_c_c_a_c.s's prose, nothing here contradicts it.
 */
struct Enc {
    short wave[2][162];
    unsigned short f288;
    unsigned char f28a;
    signed char f28b;
    signed char f28c;
    signed char f28d;
    unsigned char pad28e[2];
    unsigned short f290;
    unsigned short f292;
    unsigned char f294;
    unsigned char f295;
};

struct Ent {
    unsigned char pad00[8];
    int f08;
    int f0c;
    int f10;
};

struct Sub {
    unsigned char pad00[9];
    unsigned char b09lo : 2;
    unsigned char f09m : 2;
    unsigned char b09hi : 4;
    unsigned char pad0a[0x26 - 10];
    unsigned char f26;
};

struct Actor {
    unsigned char pad00[6];
    unsigned short f06;
    unsigned char pad08[0x30 - 8];
    int f30;
    int f34;
    unsigned char pad38[0x50 - 0x38];
    struct Sub *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x6c - 0x56];
    void *f6c;
};

struct Info {
    unsigned char pad00[8];
    signed char f08;
};

extern struct Enc *iwram_3001ea8;

extern int sin(int x);
extern int atan2(int dz, int dx);
extern void Func_8091200(int a, int b);
extern void Func_8091254(int a);
extern void Func_80978c4(void);
extern struct Ent *MapActor_GetActor(unsigned short id);
extern void Func_808e0b0(void *actor, int flag);
extern short *Func_808d394(unsigned short id);
extern struct Info *_GetSpriteInfo(short id);
extern struct Actor *_CreateActor(int kind, int x);
extern void _Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void _PlaySound(int id);
extern void Func_8097a54(void);

void Task_08097644(void)
{
    struct Enc *b;
    int v1[3];
    int v2[3];
    short *p;
    unsigned int i;
    int t;
    struct Ent *a;
    struct Ent *c;
    struct Actor *n;
    struct Sub *w;
    int h;

    b = iwram_3001ea8;
    if (b->f294 != 0) {
        b->f294--;
        return;
    }
    p = b->wave[1 ^ b->f28a];
    for (i = 0; i < 0xa0; i++)
        *p++ = sin(((unsigned)(b->f288 + i * 8) << 16) / 0xa0) >> 14;
    b->f288 += 4;
    b->f28a ^= 1;
    if (b->f28a) {
        Func_8091200((b->f28d << 10) | (b->f28c << 5)
                     | b->f28b | 0x200000, 1);
        Func_8091254(1);
        Func_80978c4();
    }
    Func_808e0b0(MapActor_GetActor(b->f290), 0);
    t = b->f295;
    if (t == 0 || t == 8 || t == 0x10) {
        a = MapActor_GetActor(b->f290);
        c = MapActor_GetActor(b->f292);
        if (a != 0 && c != 0) {
            v1[0] = a->f08;
            h = _GetSpriteInfo(*Func_808d394(b->f290))->f08;
            v1[1] = a->f0c + (h << 16) - 0x20000;
            v1[2] = a->f10;
            v2[0] = c->f08;
            h = _GetSpriteInfo(*Func_808d394(b->f292))->f08;
            v2[1] = c->f0c + (h << 16) - 0x20000;
            v2[2] = c->f10;
            n = _CreateActor(0x119, v2[0]);
            if (n != 0) {
                w = n->f50;
                n->f55 = 0;
                n->f30 = 0xa3d7;
                n->f34 = 0xa3d7;
                n->f06 = atan2(v1[2] - v2[2], v1[0] - v2[0]);
                n->f6c = Func_8097a54;
                w->f26 = 0;
                w->f09m = 1;
                _Actor_TravelTo(n, v1[0], v1[1], v1[2]);
            }
        }
    }
    if (b->f295 == 0)
        _PlaySound(0x82);
    b->f295++;
    if (b->f295 > 0x3c)
        b->f295 = 0;
}

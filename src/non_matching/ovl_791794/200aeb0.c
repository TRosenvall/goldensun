/* OvlFunc_897_200aeb0 -- PARKED, **PIN-FREE, 10 DIFFERING ENCODINGS OF 148**.
 * ref 148 / ours 148 stream encodings; **135 / 135** 16-bit instructions --
 * IN PHASE, so a TRUE DISTANCE.  dsize 0.  RELOCATIONS CLEAN.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_791794/200aeb0.c \
 *     asm/overlays/rom_791794/ovl_30_c_c_c_a_a_c_c_c.s \
 *     --func OvlFunc_897_200aeb0
 *   XX ENCODINGS differ in 10 place(s) (ref 148, ours 148)
 *      first at index 31
 *
 * ========= BATCH 341 D: THE PIN WAS WORTH 110, NOT 112, AND IS GONE =========
 *
 * This body is PIN-FREE and replaces a body that carried
 * `register unsigned char *p __asm__("r3")`.  THE PIN-FREE FIGURE WAS 120 AND
 * IS NOW 10, on one statement:
 *
 *     p = &a->f55;
 *     ((struct ZB *)p)->b = 0;
 *     p += 0xf;                      <-- THIS, as its own STATEMENT
 *     ((struct ZH *)p)->h = 0;
 *
 * THE OLD HEADER'S CONTROL HAD EXPIRED.  It said "writing
 * `p = (unsigned char *)&a->f64;` instead of `p += 0xf;` is BYTE-IDENTICAL ...
 * which is why the walk spelling never mattered".  That was measured WITH THE
 * PIN IN.  Pin-free the walk spelling is worth 108 encodings and 4 bytes:
 *   installed shape / `(p + 0xf)` in the expression / no named pointer /
 *   plain `register` (no asm) / two temps / `struct ZB *` / `char *`
 *       ... all SEVEN measure 120 of 148 vs 150, 135/136 ins, +4 bytes, reloc shifted
 *   `p += 0xf;` as a statement .... 12 of 148, 135/135, dsize 0, reloc CLEAN
 *   + one `do { } while (0);` anchor .. **10**
 * `p += 0xf` keeps a pseudo whose SOURCE is not trivially a copy (the batch-332
 * copy law), so it survives to reload with one set and one death and passes
 * local-alloc's gate at local-alloc.c:362
 * (`REG_BASIC_BLOCK >= 0 && REG_N_DEATHS == 1 && (alternate == NO_REGS
 *   || ! CLASS_LIKELY_SPILLED_P (preferred))`).  With the pin in, that pseudo is
 * a hard register and the gate is MOOT -- which is exactly why it read inert.
 *
 * ***AND THE PIN AND THE STATEMENT ARE MUTUALLY EXCLUSIVE.***  `p += 0xf` on the
 * PINNED body measures 131-141 with dsize -8/+8 across 30 anchor placements.
 * The old header's "two-at-a-time law" (split + pin, neither alone) is the
 * INVERSE of what holds here: either edit alone is fine, TOGETHER they regress.
 *
 * PINNED ALTERNATE, 6 of 148, SAME 135/135 and clean, kept at
 * scratch_elev/b341/D/p1_pinned_alt.c.  To rebuild it from this file: drop
 * `p += 0xf;`, write the f64 store `((struct ZH *)&a->f64)->h = 0;`, and declare
 * `register unsigned char *p __asm__("r3");`.  IT IS NOT INSTALLED: pin policy
 * (docs/brief-template.md, owner decision batch 319) prefers the pin-free body,
 * and a pinned landing needs a fakematch.txt row.
 *
 * THE RESIDUE IS THREE ONE-SLOT sched2 TRANSPOSITIONS, in one direction, and the
 * full aligned disassembly is the ROM's instruction for instruction:
 *   A  `adds r3,r0,#0` then `ldr r5,[r0,#80]`  -- we issue the load one slot early
 *   B  `strh r2,[r3,#0]` then `mov r8,r1`      -- `mov r8` one slot early
 *   C  `negs r1,r1` then `strh r3,[r5,#8]`     -- the strh one slot early
 * plus, pin-free only, the r2/r3 ROLE SWAP: the walk pointer takes r2 and the
 * shared SI zero r3 (r3 heads REG_ALLOC_ORDER, config/arm/arm.h:989-995).
 * Each contested pair is independent of the last scheduled insn, so both members
 * score tmp_class = 3 at rank_for_schedule's class rung
 * (haifa-sched.c:4068-4096, `if (link == 0 || insn_cost (...) == 1)`), the tie
 * falls through dependent count (:4099-4110) to INSN_LUID (:4115).
 *
 * MEASURED AND CLOSED THIS BATCH (do not re-screen):
 *  * autosweep FLAG axis on the old body: `-fno-strength-reduce`,
 *    `-fno-rerun-cse-after-loop`, `-fno-rerun-loop-opt` EXACTLY INERT at 8;
 *    `-fno-strict-aliasing` 28, `-fno-schedule-insns2` 84, `-O1` and
 *    `-fno-gcse` 144, `-ffixed-r7` 152.  Not flag-conditional.
 *  * 116-VARIANT FACTORIAL, {s/p source order} x {f8_a vs the f5_b/f5_c mask}
 *    x 29 anchor placements: swapping `s = a->f50` with `p = &a->f55` is
 *    EXACTLY INERT in all 29 anchor columns; the f8_a/mask swap is WORSE in
 *    every one.  Floor 6 pinned, 10 pin-free.
 *  * CLUSTER C, 8 spellings on this body: named `int` temp for the f8_a value
 *    INERT at 10; `f5_c` before `f5_b` 22; `f7_b` between 23; `f5_b` last 26;
 *    mask pair first 49 RELOCDIFF; +anchor 134; anchor before either 142 (+12B).
 *  * THE ZERO'S QUANTITY, 7 spellings on this body: `a->f68 = c;` written
 *    `*(struct Actor **)(p + 4) = c;` (one more ref on p) 11; `p = &a->f55;`
 *    hoisted above the f14 store 12; and NAMING THE ZERO COSTS 8 BYTES --
 *    `int z` 141, two separate zeros 141, `unsigned char z` 141,
 *    `p += 0xa; p += 0x5;` 141.  A counter-example to batch 333's "a copy
 *    survives when its DESTINATION is a named variable".
 *
 * CORRECTION TO THE OLD HEADER: it said "Instruction count exact (148 = 148)".
 * 148 is the STREAM count; the instructions are 135 / 135.
 *
 * asm/overlays/rom_791794/ovl_30_c_c_c_a_a_c_c_c.s is ONE function, no data
 * (datacheck prints nothing) -- no split needed, CONVERTS WHOLE.
 */
/* OvlFunc_897_200aeb0 (0x0200aeb0) -- NON-MATCHING, **8 ENCODINGS OF 148**,
 * DOWN FROM 12.  Instruction count exact (148 = 148), size exact (dsize 0),
 * pool identical, all relocations identical.  A TRUE DISTANCE.
 * Production flags, no per-file Makefile adjustment (checked).
 *
 * tools/shimcount.py: **1 register pin** (`p` to r3), load-bearing.
 * A PINNED LANDING NEEDS A fakematch.txt ROW; there is none yet.
 *
 * asm/overlays/rom_791794/ovl_30_c_c_c_a_a_c_c_c.s is ONE function, no data
 * (datacheck prints nothing) -- no split needed, CONVERTS WHOLE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_791794/200aeb0.c \
 *     asm/overlays/rom_791794/ovl_30_c_c_c_a_a_c_c_c.s \
 *     --func OvlFunc_897_200aeb0
 *   XX ENCODINGS differ in 8 place(s) (ref 148, ours 148)
 *      first at index 32
 *
 * FAMILY: OvlFunc_887_200968c (src/non_matching/ovl_787e04/200968c.c) with a
 * leading __PlaySound(0x124) and a different tail.  THE 12 -> 8 FIX WAS FOUND
 * THERE AND PORTED HERE UNCHANGED; the residue is 200968c's two sched2
 * clusters shifted by 3.  The park's old "INERT / WORSE here" entries were
 * recorded as BYTE SIZES (324, 332), which are not comparable to the figure
 * and must not be inherited. *
 * ================== BATCH 316: THE BLOCKER WAS MISFILED ==================
 *
 * The park said: "store_fixed_bit_field (the BLKmode field store) leaves a
 * REG_UNUSED (set (reg:QI) (const_int 0)); local-alloc gives that dead QI r3
 * (1-insn life, top priority)".  THE DEAD QI IS NOT A CAUSE.  `.17.lreg`
 * block 2 shows pseudo 50 (the dead QI) and pseudo 52 (the SI zero) BOTH
 * getting r3, and they do NOT conflict -- 50 is REG_UNUSED and dies at birth,
 * and `;; 50 conflicts:` does not list 52.  The dead QI consumes no register,
 * so removing it can free nothing, and every lever aimed at it was aimed at a
 * non-cause.
 *
 * THE REAL CHAIN, four steps, each read off a dump:
 *   1. `.00.rtl` insn 82 is (set (reg 37) (plus (reg 37) (const_int 15))) --
 *      the walk `p += 0xf`.  By `.03.cse` it reads
 *      (set (reg 37) (plus (reg 34) (const_int 100))): **cse1 rewrites the
 *      increment against the base `a`**.
 *      CONTROL: writing `p = (unsigned char *)&a->f64;` instead of `p += 0xf;`
 *      is BYTE-IDENTICAL to the old park body.  Both spellings reach reload as
 *      the same RTL, which is why the walk spelling never mattered.
 *   2. Reg 37 therefore has two disjoint live ranges, so REG_N_DEATHS == 2.
 *   3. local-alloc.c:362 gates local allocation on
 *      `REG_BASIC_BLOCK >= 0 && REG_N_DEATHS == 1 && (alternate == NO_REGS ||
 *      ! CLASS_LIKELY_SPILLED_P (preferred))`.  Reg 37 fails it TWICE: two
 *      deaths, and `pref STACK_REG`, where STACK_REG = {sp} has size 1 so
 *      CLASS_LIKELY_SPILLED_P is true (regs.h:190).
 *   4. local-alloc thus reaches the ZERO first and gives it r3 (r3 heads
 *      REG_ALLOC_ORDER); global_alloc then puts the walk pointer in r2.
 *
 * ============ THE FIX IS TWO EDITS THAT ARE EACH A REGRESSION ============
 *
 * Neither edit works alone.  Measured on this body, as INSTRUCTIONS (encoding
 * counts inflate here because a longer function moves every `ldr [pc,#N]`):
 *
 *   two address temps (p for f55, `&a->f64` direct for f64), no pin ... 101
 *   `register unsigned char *p __asm__("r3")` on the single walk ....... 121
 *   BOTH TOGETHER ....................................................... 8
 *
 * and the pair costs NOTHING in length (nins and pool identical, dsize 0),
 * where each edit alone cost +2 instructions and +4 bytes.  This is the
 * two-at-a-time law: splitting the walk gives the f64 address its own
 * 1-set/1-death pseudo, and the r3 pin supplies the hard register that
 * local-alloc's eligibility gate refuses to supply for the f55 one.  Together
 * the ROM's assignment is reproduced EXACTLY -- walk pointer in r3, the shared
 * SI zero in r2 -- and move2add chains the second address into `adds r3, #15`.
 *
 * WHY THE SPLIT ALONE LOOKS LIKE A REGRESSION, and this is the generalisable
 * part: with the split and no pin, the f64 temp (pseudo 56) gets
 * `pref BASE_REGS` -- LO_REGS and STACK_REG tie at cost 0, so regclass picks
 * their union, which is NOT likely-spilled -- so it IS local-alloc eligible,
 * wins r3, and correctly pushes the zero to r2.  The f55 temp (pseudo 49)
 * keeps `pref STACK_REG`, falls through to global_alloc and lands in r1, so
 * move2add cannot chain and the function grows.  **The right half of the
 * answer was already visible inside a variant whose TOTAL had risen.**
 *
 * WHAT REMAINS AT 8 IS PURE sched2 PLACEMENT, in two clusters:
 *   A. 6 of the 8.  `ldr r5, [r0, #0x50]` (`s = a->f50`) and `mov r8, r1` sit
 *      in different slots; every register agrees.  The ROM issues the f50 load
 *      immediately after `adds r3, r0, #0`; we issue it three slots later.
 *      Rung: PRIORITY, not CLASS -- the load is independent of
 *      `adds r3, r0, #0` (CLASS 3, which would win) but its result feeds only
 *      `cmp r5, #0`, while `adds r3, #85` heads the strb/add/strh chain and so
 *      carries the longer path.  To move it, the LOAD's priority must rise.
 *   B. 2 of the 8.  `movs r1,#33 / negs r1,r1 / strh r3,[r5,#8]`: the ROM
 *      hoists only the `movs` above the strh, we hoist the `negs` too.  Every
 *      register agrees, so the only rung left is INSN_LUID.
 *
 * MEASURED INERT AT 8 (13 variants): `s = a->f50` at FIVE source positions
 * (after p, after the stores, before the f14 copy, after f68, plus an extra
 * f68 store) -- statement order is NOT the handle; the f8_a value through a
 * named temp; and the brief's UNION alias-set device on BOTH the ZB and the ZH
 * store (alias set 0 manufactures no useful dependence here, because the two
 * competing insns are not a MEM/MEM pair).  WORSE: the f5 bitfields before the
 * f8_a store 69, f28 before the bitfields 26, f7_b first 21, f5_c first 18,
 * the f68 store above the zero stores 121.
 *
 * FLAG SWEEP, 29 flags, on the 12-figure body: 19 EXACTLY INERT,
 * -fno-force-mem 26, -fno-strict-aliasing 45, -fno-regmove 49,
 * -fno-schedule-insns2 80, -fno-expensive-optimizations 93, -fno-gcse 123.
 * Not flag-conditional.  All `extern void` -> `extern int`: INERT.
 */
struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

struct Spr {
    unsigned char pad0[5];
    unsigned char f5_a : 5;
    unsigned char f5_b : 1;
    unsigned char f5_c : 2;
    unsigned char pad6;
    unsigned char f7_a : 6;
    unsigned char f7_b : 2;
    unsigned short f8_a : 10;
    unsigned short f8_b : 2;
    unsigned short f8_c : 4;
    unsigned char pad_a[0x12];
    unsigned char f1c;
    unsigned char f1d;
    unsigned char pad1e[8];
    unsigned char f26;
    unsigned char pad27;
    unsigned char *f28;
};

struct Spr9 {
    unsigned char pad[9];
    unsigned char a : 2;
    unsigned char b : 2;
};

struct Actor {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0xb];
    unsigned char f23;
    unsigned char pad24[0x2c];
    struct Spr *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0xe];
    unsigned short f64;
    unsigned char pad66[2];
    struct Actor *f68;
    void *f6c;
};

/* BLKmode (3 and 6 bytes) on purpose -- see the header. */
struct ZB { unsigned char b; unsigned char c[2]; };
struct ZH { unsigned short h; unsigned char c[4]; };

extern struct SpriteSlot gSpriteSlots[];
extern unsigned char *iwram_3001f30;

extern struct Actor *__CreateActor(int kind, int x, int y, int z);
extern void __Sprite_SetAnim(struct Spr *s, int n);
extern void __Func_8003f3c(int n);
extern void __PlaySound(int id);
extern void OvlFunc_897_200ae5c(void);
extern void OvlFunc_897_200ae0c(void);
extern struct Actor *__MapActor_GetActor(int id);

void OvlFunc_897_200aeb0(struct Actor *c)
{
    struct Actor *arr[2];
    struct Actor *a;
    struct Spr *s;
    unsigned char *w;
    unsigned char *p;
    int i;

    w = iwram_3001f30;
    __PlaySound(0x124);
    for (i = 0; i <= 1; i++) {
        a = __CreateActor(0x1a, c->f8, c->fc, c->f10);
        arr[i] = a;
        if (a != 0) {
            a->f14 = c->f14;
    do { } while (0);
            s = a->f50;
            p = &a->f55;
            ((struct ZB *)p)->b = 0;
            p += 0xf;
            ((struct ZH *)p)->h = 0;
            a->f68 = c;
            if (s != 0) {
                __Sprite_SetAnim(s, 0);
                s->f26 = 0;
                __Func_8003f3c(s->f1c);
                s->f1c = *(unsigned short *)(w + 0x46);
                s->f1d |= 1;
                s->f8_a = gSpriteSlots[s->f1c].vramOffset >> 5;
                s->f5_b = 0;
                s->f5_c = 1;
                s->f7_b = 2;
                *(s->f28 + 0x16) = 0;
            }
        }
    }
    arr[0]->f6c = OvlFunc_897_200ae5c;
    ((struct Spr9 *)arr[0]->f50)->b = ((struct Spr9 *)__MapActor_GetActor(0xf)->f50)->b;
    ((struct Spr9 *)arr[1]->f50)->b = ((struct Spr9 *)__MapActor_GetActor(0xf)->f50)->b;
    arr[1]->f6c = OvlFunc_897_200ae0c;
    arr[1]->f23 = 2;
}

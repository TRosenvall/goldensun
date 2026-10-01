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
    register unsigned char *p __asm__("r3");
    int i;

    w = iwram_3001f30;
    __PlaySound(0x124);
    for (i = 0; i <= 1; i++) {
        a = __CreateActor(0x1a, c->f8, c->fc, c->f10);
        arr[i] = a;
        if (a != 0) {
            a->f14 = c->f14;
            s = a->f50;
            p = &a->f55;
            ((struct ZB *)p)->b = 0;
            ((struct ZH *)&a->f64)->h = 0;
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

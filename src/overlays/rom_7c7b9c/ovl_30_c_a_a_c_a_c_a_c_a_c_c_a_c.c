/* Cluster OvlFunc_943_2009f90..OvlFunc_943_200a618 extracted from
 * goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_c_a.s.
 *
 * 289, 300 and 354 instructions. Never attempted before batch 280.
 *
 * THIS FILE CONVERTS WHOLE: the `.s` held exactly these three functions and all three matched, so no
 * split was needed and no linker line changed. Each function was also verified separately against its
 * own filtered reference.
 *
 * Scaffolding at greedy-drop fixpoint, re-run after every structural change: 16, 12, and 30 PIN3 plus
 * 4 PIN2 sites, and ONE `do { } while (0);` in the third -- three fakematch rows. No flags, no
 * volatile. The third also requires _AREA_6d, added to area.sym this batch.
 *
 * ===== A POOLED ZERO CAN BE A HImode CONSTANT RATHER THAN A SYMBOL, AND ONE HImode FIX
 *       REARRANGES THE WHOLE LITERAL POOL =====
 *
 * This is the finding worth more than the landings. OvlFunc_943_200a2c0 loads 0 FROM THE POOL for two
 * byte stores, while storing 1 and 2 at the same offsets with `movs`; and its ROM splits the pool
 * into THREE chunks with inserted `b`s only 248 and 356 bytes apart, where its file-mate reaches an
 * end-of-function pool from 726 bytes.
 *
 * One cause, named in arm.md: `*thumb_movhi_insn` alternative 1 prints `ldrh %0, %1`, which GAS
 * assembles as a two-byte `ldr rX,[pc,#N]` (verified), and it carries `pool_range = 64` against
 * `*thumb_movsi_insn`'s 1020 -- while MINIPOOL_FIX_SIZE rounds HImode up to 4, so it still emits a
 * full `.word`. ONE SUCH FIX CLAMPS `minipool_vector_head->max_address` FOR THE ENTIRE POOL in
 * `add_minipool_forward_ref`, and the ROM's zero word is indeed the FIRST entry of the first chunk,
 * 60 bytes from its load. The duplicated words in later chunks are ordinary minipool duplication
 * across a split, not extra rtx objects.
 *
 * THE SPELLING IS THE BARE LITERAL, and a named local destroys it: `int zero = 0; a->f63 = zero;`
 * gives `movs r5, #0` at 320 encodings against 325 and 69 differing, where `a->f63 = 0;` is exact.
 * The trigger is that the function must ALSO perform a HImode store -- measured both ways on cut-down
 * probes.
 *
 * SO THIS IS AN EXCEPTION TO THE WORD-`ldr`-OF-A-SMALL-CONSTANT SYMBOL TELL. Both `(int)&_CONST_0`
 * and a cast version give the right two-byte `ldr` but a range-1020 movsi fix, so the pool does not
 * split -- 318 encodings, 45 to 96 differing. CHECK FOR A HALFWORD STORE BEFORE INVENTING A SYMBOL.
 *
 * ===== THREE MORE, EACH MEASURED =====
 *
 * A `do/while` COUNTER INCREMENT GOES AFTER THE CALL. The ROM has `mov r0,#1 / add r5,#1 /
 * bl __WaitFrames`. `i++` BEFORE the call is hoisted ahead of the second `str` (2 differing); after
 * the call, or `while (++i <= N)`, is exact -- its path to the loop-closing `cmp`/`bls` gives it
 * priority 3 against the store's 2. The same fix applied to a second function this batch.
 *
 * A SHIFT SPLIT FROM ITS `mov` LANDS BY RELATION, NOT POSITION. Where the ROM writes
 * `mov r1 / mov r2 / lsl r2 / mov r0 / lsl r1`, the source needs `q1 <<= 17;` BEFORE `q2 <<= 18;` --
 * the scheduler inverts the pair. A 30-permutation sweep gives 0 for ALL EIGHT orders with that
 * relation and 4 to 8 otherwise.
 *
 * THE ACTOR POINTER IS PER-BLOCK IN ONE FUNCTION AND PER-FUNCTION IN ITS NEIGHBOUR. 200a2c0 shares
 * one pointer across three getter results -- the ROM's `mov r6,r0 / mov r3,r6` copy is the tell for a
 * shared global allocno -- while 200a618 uses r0 directly everywhere, where a function-scope local
 * costs a second callee-saved register (`push {r5,r6,lr}` against the ROM's `{r5,lr}`) plus a copy.
 * Block-scoping fixed both at once. Two functions in ONE FILE wanting opposite answers, which is why
 * this is per-site rather than a rule.
 *
 * METHOD NOTE THAT PAID TWICE: reading /opt/camelot-gcc/gcc-2.96/gcc/config/arm/arm.{c,md} in the
 * build container. `create_fix_barrier`, `add_minipool_forward_ref`, `MINIPOOL_FIX_SIZE` and the
 * per-alternative `pool_range` table turned "gcc will not split this pool" from a dead end into a
 * measurement -- required range about 64, matched by the 60-byte distance in the ROM -- and it is
 * what identified the HImode fix. Cross-checked against a landed multi-chunk precedent whose first
 * reference sits at offset 988, confirming the SImode range really is about 1020.
 */
/* Cluster OvlFunc_943_2009f90..OvlFunc_943_200a618 extracted from
 * goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_c_a_c.s.
 *
 * THE WHOLE .s CONVERTS: all three of its functions are here (289 / 300 / 354 ROM instructions;
 * 305 / 325 / 373 encodings; 816 / 856 / 956 bytes), there is no data section and NO SPLIT is
 * needed. Total .text for this TU = 1003 encodings / 2628 bytes.
 *
 * Scaffolding: 16 + 12 + 34 PIN2/PIN3 sites, greedy-minimal, one `do { } while (0);` barrier in
 * OvlFunc_943_200a618. No flags. One fakematch row.
 *
 * NEW SYM ENTRY REQUIRED -- `_AREA_6d = 0x6d;` in area.sym, for
 * `__Func_8091f90((int)&_AREA_6d, 0x10)` at the end of OvlFunc_943_200a618. The ROM writes
 * `ldr r0, =0x6d / mov r1, #0x10`, pooling a value gcc builds in one `movs r0, #109` -- measured:
 * the literal spelling emits `movs r0, #109` and leaves the function one pool word short (3
 * differing). That is exactly the criterion area.sym's `_AREA_05`, `_AREA_bb` and `_AREA_15`
 * entries were added on, feeding the same `__Func_8091f90` first argument. Without the entry the
 * link fails on an undefined symbol.
 *
 * objcmp reports two phantom encodings and two extra relocations for OvlFunc_943_200a618
 * (`_AREA_6f` at pool index 371 and `_AREA_6d` at 372) because both are absolute symbols the
 * LINKER fills; all 373 encodings and every other relocation are identical. Same note as
 * src/overlays/rom_7ca63c/ovl_30_c_c_a_c_a.c carries for `_AREA_6f`.
 *
 * ------------------------------------------------------------------------------------------
 * THE ONE FINDING WORTH CARRYING: A POOLED ZERO CAN BE A HImode CONSTANT, NOT A SYMBOL, AND IT
 * REARRANGES THE WHOLE LITERAL POOL.
 *
 * OvlFunc_943_200a2c0 loads 0 from the pool -- `ldr r5, [pc, #60]` over a `.word 0` -- to store
 * into the byte fields at actor+0x63 and actor+0x55, while storing 1 and 2 at the same offsets
 * with `movs`. Its ROM ALSO splits its literal pool into THREE chunks with two inserted
 * `b` instructions, at 0xf8 and 0x27c, only 248 and 356 bytes apart, where the sibling
 * OvlFunc_943_2009f90 in the same .s reaches its end-of-function pool from 726 bytes away.
 *
 * Both facts have ONE cause, and `arm.md` names it. `*thumb_movhi_insn` alternative 1 prints
 * `ldrh %0, %1`, which GAS assembles as a TWO-BYTE `ldr rX, [pc, #N]` (verified), and carries
 * `pool_range = 64` against `*thumb_movsi_insn`'s 1020. `MINIPOOL_FIX_SIZE` rounds a HImode fix up
 * to 4, so it emits a full `.word`. One HImode fix therefore clamps
 * `minipool_vector_head->max_address` for the entire pool via `add_minipool_forward_ref`, forcing
 * the early dump -- and the ROM's pool word for the zero is indeed the FIRST entry of the first
 * chunk, 60 bytes from its load. The duplicated `.word 0`, `.word 0x19999` and `.word 0xcccc` in
 * the ROM's later chunks are ordinary minipool duplication across a split, not extra rtx objects.
 *
 * THE SPELLING THAT PRODUCES IT IS THE BARE LITERAL, and a named `int` local destroys it:
 *     int zero = 0; a->f63 = zero;          `movs r5, #0`; 320 encodings against 325; 69 differing
 *     a->f63 = 0;                           the HImode pool load, three pool chunks, EXACT
 * The trigger is that the function ALSO performs a HImode store (`a->f06 = 0xd0 << 8` here, with
 * `short f06` in the struct): with one in the function, a QImode store of 0 takes its constant
 * from a HImode pool entry; with none, `movs` -- measured both ways on cut-down probes.
 *
 * Do NOT reach for a symbol here. `(int)&_CONST_0` and `(unsigned short)(int)&_CONST_0` both give
 * the right two-byte `ldr` but a `*thumb_movsi_insn` fix with range 1020, so the pool does not
 * split: 318 encodings and 45-96 differing. The word-`ldr`-of-a-small-constant symbol tell has an
 * EXCEPTION, and this is it -- check whether the function contains a halfword store first.
 * ------------------------------------------------------------------------------------------
 *
 * THREE MORE LEVERS, each measured:
 *
 * 1. THE COUNTER INCREMENT OF A `do/while` COUNTING LOOP GOES AFTER THE CALL. The ROM schedules
 *    `mov r0, #1 / add r5, #1 / bl __WaitFrames`. `i++` written before the call is hoisted two
 *    insns earlier, ahead of the second `str` (2 differing); after the call, or as `while (++i <=
 *    0xf)`, is exact. The increment's path to the loop-closing `cmp`/`bls` gives it sched2 priority
 *    3 against the `str`'s 2, so it is pulled forward from any earlier position.
 *
 * 2. A SHIFT SPLIT OFF FROM ITS `mov` LANDS BY RELATION, NOT POSITION. Where the ROM writes
 *    `mov r1 / mov r2 / lsl r2 / mov r0 / lsl r1`, the source needs `q1 <<= 17;` BEFORE
 *    `q2 <<= 18;` -- the scheduler inverts the pair. A 30-permutation sweep gives 0 for all eight
 *    orders with that relation and 4-8 for the rest.
 *
 * 3. THE ACTOR POINTER IS PER-BLOCK, NOT PER-FUNCTION, IN OvlFunc_943_200a618, AND THE OPPOSITE IN
 *    OvlFunc_943_200a2c0. 200a2c0 reuses ONE `Actor *a` for three `__MapActor_GetActor` results and
 *    the ROM's `mov r6, r0 / mov r3, r6` copy is the tell for a shared global allocno. 200a618 uses
 *    r0 directly at every site: a function-scope local there costs a second callee-saved register
 *    (`push {r5, r6, lr}` against the ROM's `{r5, lr}`) and an extra copy, and block-scoped
 *    `{ Actor *b = ...; }` inside each site fixes both at once.
 *
 * Also: the three repeated `0x256`/`0x262` arguments of `__Func_80921c4` must NOT be commoned --
 * PIN3 on each of the three adjacent calls keeps the ROM's three separate pool loads, where plain
 * literals let cse1 hoist one into a callee-saved register (`adds r2, r5, #0`, and a second pushed
 * register). And `gState.f22b = 3;` wants a `do { } while (0);` in front of it, or its address
 * arithmetic is scheduled ahead of the `gState.f1c6` store.
 *
 * Idiom source: src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_c_a_b.c (OvlFunc_943_2009db0,
 * batch 279) for the prototypes, the PIN3 macro and the `*(int *)(iwram_3001ebc + (0xe0 << 1))`
 * idiom; src/overlays/rom_7ca63c/ovl_30_c_c_a_c_a.c for the GlobalState struct and `_AREA_6f`.
 */
typedef struct {
    unsigned char pad00[6];
    short f06;
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[0x18 - 0x14];
    int f18;
    int f1c;
    unsigned char pad20[0x28 - 0x20];
    int f28;
    unsigned char pad2c[0x44 - 0x2c];
    int f44;
    int f48;
    unsigned char pad4c[0x55 - 0x4c];
    unsigned char f55;
    unsigned char pad56[0x63 - 0x56];
    unsigned char f63;
    unsigned char pad64[0x6c - 0x64];
    void *f6c;
} Actor;

typedef struct {
    unsigned char pad00[0x1c2];
    short f1c2;
    short f1c4;
    short f1c6;
    unsigned char pad1c8[0x22b - 0x1c8];
    unsigned char f22b;
    unsigned char pad22c[0x2c0 - 0x22c];
} GlobalState;

extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _AREA_6d;
extern int _AREA_6f;
extern unsigned char L5160[] __asm__(".L5160");
extern unsigned char L5268[] __asm__(".L5268");
extern unsigned char L5340[] __asm__(".L5340");
extern unsigned char L5418[] __asm__(".L5418");
extern unsigned char gScript_943__0200c764[];
extern unsigned char gScript_943__0200c7a8[];
extern unsigned char gScript_943__0200c7ec[];
extern unsigned char gScript_943__0200c80c[];
extern unsigned char gScript_943__0200c814[];
extern unsigned char gScript_943__0200c888[];
extern unsigned char gScript_943__0200c8b0[];
extern unsigned char gScript_943__0200c8c4[];
extern unsigned char gScript_943__0200c8d8[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(Actor *a, int f);
extern void __LoadFieldActors(unsigned char *p);
extern void __MapActor_SetAnim(int slot, int n);
extern void __MapActor_SetBehavior(int slot, unsigned char *p);
extern void __MapActor_RunScript(int slot, unsigned char *p);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern void __SetCameraTarget(int slot, int a);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __PlaySound(int id);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8019aa0(int a, int b, int c);
extern void __Func_800c5b4(void);
extern void __Func_800c5fc(void);
extern void __Func_8091e9c(int a);
extern void __Func_8091eb0(int a, int b);
extern void __Func_8091f90(int a, int b);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8093304(int a);
extern void OvlFunc_943_2008bb8(void);
extern void OvlFunc_943_200ba00(int a, int b);
extern void OvlFunc_943_20088c0(void);

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")

#define PIN3 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1"); \
             register int q2 __asm__("r2")

void OvlFunc_943_2009f90(void)
{
    unsigned char *p;
    int zero;
    int one;

    __CutsceneStart();
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __LoadFieldActors(L5160);
    __WaitFrames(1);
    __LoadFieldActors(L5268);
    __WaitFrames(1);
    __MapActor_SetAnim(0x1f, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x18), 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x19), 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1a), 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1b), 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1c), 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1d), 1);
    { PIN3; q1 = 0x80; q2 = 0xa0; q0 = 0x16; q1 <<= 17; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    p = gScript_943__0200c80c;
    __MapActor_SetBehavior(0x16, p);
    { PIN3; q1 = 0x86; q2 = 0xad; q0 = 0x15; q1 <<= 17; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetBehavior(0x16, p);
    { PIN3; q1 = 0xf2; q2 = 0x97; q0 = 0x18; q1 <<= 16; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0x84; q2 = 0x96; q0 = 0x19; q1 <<= 17; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xfe; q2 = 0xa7; q0 = 0x1a; q1 <<= 16; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetPos(0x1b, (0x8d) << 17, 0x2920000);
    zero = 0;
    __MapActor_GetActor(0x18)->f63 = zero;
    one = 1;
    __MapActor_GetActor(0x19)->f63 = one;
    __MapActor_GetActor(0x1a)->f63 = zero;
    __MapActor_GetActor(0x1b)->f63 = one;
    p = gScript_943__0200c7a8;
    __MapActor_SetBehavior(0x18, p);
    __MapActor_SetBehavior(0x19, p);
    p = gScript_943__0200c764;
    __MapActor_SetBehavior(0x1a, p);
    __MapActor_SetBehavior(0x1b, p);
    __MapActor_SetPos(0x14, 0, 0);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0xc8 << 1);
    { PIN3; q1 = 0xfe; q2 = 0xb9; q0 = 0x1c; q1 <<= 16; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 0x1d; q1 <<= 13; q2 = 0x24a0000; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q0 = 0x1c; q1 = 0x19999; q2 = 0xcccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 0x1d; q1 = 0x19999; q2 = 0xcccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0xa1; q0 = 0x1d; q1 = 0xac; q2 <<= 2; __MapActor_TravelTo(q0, q1, q2); }
    { PIN3; q2 = 0xa5; q0 = 0x1c; q1 = 0xc8; q2 <<= 2; __Func_8092158(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 11; q2 <<= 10; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x9b; q0 = 0; q1 = 0xae; q2 <<= 2; __MapActor_TravelTo(q0, q1, q2); }
    __Func_8092158(0x1c, 0xb4, (0x91) << 2);
    __PlaySound(0x92);
    p = gScript_943__0200c7ec;
    __MapActor_SetBehavior(0x1c, p);
    __MapActor_SetBehavior(0x1d, p);
    __PlaySound(0xf0);
    { PIN3; q1 = 0x86; q2 = 0x2520000; q0 = 0x1f; q1 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetBehavior(0x1f, gScript_943__0200c814);
    __CutsceneWait(0xa);
    { PIN3; q1 = 0x86; q2 = 0x92; q0 = 0x1e; q1 <<= 16; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetSpeed(0x1e, (0x80) << 11, (0x80) << 10);
    __MapActor_GetActor(0x1e)->f28 = 0x80 << 12;
    __Func_8092158(0x1e, 0xba, (0x99) << 2);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1e), 1);
    __CutsceneWait(0xa);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0x1e; q1 <<= 10; q2 <<= 9; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092158(0x1e, 0xd8, (0x96) << 2);
    OvlFunc_943_200ba00(0x1e, 0xc0 << 8);
    OvlFunc_943_2008bb8();
    __CutsceneWait(0xa);
    p = gScript_943__0200c888;
    __MapActor_SetBehavior(0x1e, p);
    __CutsceneWait(0xa);
    __MapActor_SetBehavior(0x1c, p);
    __CutsceneWait(0xa);
    __MapActor_RunScript(0x1d, p);
    __CutsceneWait(0x14);
    __PlaySound(0x93);
    __MapTransitionOut();
    __WaitMapTransition();
    __MapActor_SetIdle(0x18);
    __MapActor_SetIdle(0x19);
    __MapActor_SetIdle(0x1a);
    __MapActor_SetIdle(0x1b);
    __CutsceneWait(0xa);
    __Func_800c5b4();
    __Func_8093304(0x15);
    __Func_8019aa0(0x1e45, 1, 0);
    __Func_800c5fc();
    __Func_8091e9c(0xd);
}

void OvlFunc_943_200a2c0(void)
{
    unsigned int i;
    Actor *a;
    unsigned char *p;

    __CutsceneStart();
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __LoadFieldActors(L5160);
    __WaitFrames(1);
    __LoadFieldActors(L5340);
    __WaitFrames(1);
    __MapActor_SetPos(0x16, (0xb0) << 16, (0xae) << 18);
    __MapActor_GetActor(0x16)->f06 = 0xd0 << 8;
    __MapActor_SetPos(0x15, (0x84) << 17, 0x2960000);
    __MapActor_GetActor(0x15)->f06 = 0xb0 << 8;
    { PIN3; q1 = 0xb8; q2 = 0xa8; q1 <<= 16; q0 = 0x18; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xca; q2 = 0xad; q0 = 0x19; q1 <<= 16; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xfc; q0 = 0x1a; q1 <<= 16; q2 = 0x2860000; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0x80; q0 = 0x1b; q1 <<= 17; q2 = 0x2ae0000; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0xac; q2 = 0x9e; q0 = 0x1c; q1 <<= 16; q2 <<= 18; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetPos(0x1d, (0x80) << 17, 0x26e0000);
    __MapActor_GetActor(0x18)->f63 = 0;
    __MapActor_GetActor(0x19)->f63 = 1;
    __MapActor_GetActor(0x1a)->f63 = 0;
    __MapActor_GetActor(0x1b)->f63 = 2;
    __MapActor_SetPos(0x14, 0, 0);
    p = gScript_943__0200c8c4;
    __MapActor_SetBehavior(0x18, p);
    __MapActor_SetBehavior(0x19, p);
    p = gScript_943__0200c8b0;
    __MapActor_SetBehavior(0x1a, p);
    __MapActor_SetBehavior(0x1b, p);
    p = gScript_943__0200c8d8;
    __MapActor_SetBehavior(0x1c, p);
    __MapActor_SetBehavior(0x1d, p);
    __Func_8092950(0x18, 3);
    __Func_8092950(0x19, 3);
    __Func_8092950(0x1a, 3);
    __Func_8092950(0x1b, 3);
    __Func_8092950(0x1c, 3);
    __Func_8092950(0x1d, 3);
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x50);
    __PlaySound(0x93);
    a = __MapActor_GetActor(0x1f);
    a->f18 = 0x1999;
    a->f1c = 0x1999;
    a->f08 = 0xc2 << 16;
    a->f10 = 0x2820000;
    i = 0;
    do {
        a->f18 += 0xf5c;
        a->f1c += 0xf5c;
        __WaitFrames(1);
        i++;
    } while (i <= 0xf);
    a = __MapActor_GetActor(0x1e);
    a->f18 = 0x11999;
    a->f1c = 0x11999;
    a->f08 = 0xc2 << 16;
    a->f0c = 0xa0 << 15;
    a->f10 = 0x2820000;
    a->f06 = 0xa0 << 7;
    a->f44 = 0x6666;
    a->f48 = 0x80 << 10;
    __CutsceneWait(0x50);
    __PlaySound(0x93);
    __MapActor_SetPos(0x1f, 0, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x1e), 1);
    { PIN3; q1 = 0x19999; q2 = 0xcccc; q0 = 0; __MapActor_SetSpeed(q0, q1, q2); }
    a = __MapActor_GetActor(0);
    a->f55 = 0;
    { PIN3; q2 = 0x99; q0 = 0; q1 = 0xd8; q2 <<= 2; __MapActor_TravelTo(q0, q1, q2); }
    { PIN3; q0 = 0x1e; q1 = 0x19999; q2 = 0xcccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x96; q0 = 0x1e; q1 = 0xc4; q2 <<= 2; __Func_8092158(q0, q1, q2); }
    { PIN3; q2 = 0x96; q1 = 0xd8; q2 <<= 2; q0 = 0x1e; __Func_8092158(q0, q1, q2); }
    __MapActor_SetIdle(0x1c);
    __WaitFrames(1);
    { PIN3; q2 = 0xcccc; q0 = 0x1c; q1 = 0x19999; __MapActor_SetSpeed(q0, q1, q2); }
    p = gScript_943__0200c888;
    __MapActor_SetBehavior(0x1c, p);
    OvlFunc_943_200ba00(0x1e, 0xd0 << 8);
    OvlFunc_943_2008bb8();
    __CutsceneWait(0xa);
    __MapActor_SetBehavior(0x1e, p);
    __MapActor_SetIdle(0x1d);
    __WaitFrames(1);
    { PIN3; q2 = 0xcccc; q0 = 0x1d; q1 = 0x19999; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_RunScript(0x1d, p);
    __CutsceneWait(0x14);
    __MapTransitionOut();
    __WaitMapTransition();
    __MapActor_SetIdle(0x18);
    __MapActor_SetIdle(0x19);
    __MapActor_SetIdle(0x1a);
    __MapActor_SetIdle(0x1b);
    __MapActor_SetIdle(0x1c);
    __MapActor_SetIdle(0x1d);
    __CutsceneWait(0xa);
    __Func_800c5b4();
    __Func_8093304(0x15);
    __Func_8019aa0(0x1e45, 1, 0);
    __Func_800c5fc();
    __Func_8091e9c(0xe);
}

void OvlFunc_943_200a618(void)
{
    int v;

    __CutsceneStart();
    __LoadFieldActors(L5418);
    __WaitFrames(1);
    __SetCameraTarget(0x19, 1);
    __WaitFrames(1);
    __MapActor_SetAnim(0x15, 5);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x15), 0);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_GetActor(0)->f06 = 0x80 << 7;
    *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x14);
    OvlFunc_943_2008bb8();
    __CutsceneWait(0xa);
    { PIN3; q1 = 0xd8; q0 = 0; q1 <<= 16; q2 = 0x24a0000; __MapActor_SetPos(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x256; q1 = 0xd8; q0 = 0; __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(0x14);
    OvlFunc_943_200ba00(0, 0xc0 << 7);
    __MapActor_Jump(0, 2, 0xa);
    { PIN3; q0 = 0; q1 = 0x19999; q2 = 0xcccc; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x9c; q1 = 0xc2; q2 <<= 2; q0 = 0; __Func_80921c4(q0, q1, q2); }
    __PlaySound(0xb5);
    { PIN3; q0 = 0x80; q1 = 0x80; q2 = 0x80; q0 <<= 11; q1 <<= 11; q2 <<= 9; __Func_8012330(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN2; q0 = -1; q1 = -1; __Func_8012330(q0, q1, 0xe666); }
    __CutsceneWait(0xa);
    __Func_8092adc(0, (0xc0) << 8, 0x14);
    __MapActor_GetActor(0x19)->f55 = 0;
    { PIN3; q1 = 0x80; q2 = 0x80; q1 <<= 10; q0 = 0x19; q2 <<= 9; __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_TravelTo(0x19, 0xd8, (0x99) << 2);
    __PlaySound(0x95);
    __Func_8092b08(0x16, 2);
    __MapActor_SetAnim(0x16, 5);
    {
        Actor *b = __MapActor_GetActor(0x16);
        b->f28 = 0x80 << 12;
        b->f48 = 0xb333;
        b->f18 = 0xd0 << 9;
        b->f1c = 0xd0 << 9;
        b->f6c = OvlFunc_943_20088c0;
        v = 0x80 << 8;
        b->f44 = v;
    }
    { PIN3; q1 = 0xc0; q2 = 0xc0; q1 <<= 11; q0 = 0x16; q2 <<= 10; __MapActor_SetSpeed(q0, q1, q2); }
    __Func_8092158(0x16, 0xb6, 0x26a);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x16), 0);
    OvlFunc_943_200ba00(0, 0xa0 << 8);
    __MapActor_Jump(0, 6, 0x50);
    { PIN3; q2 = 0x8d; q0 = 0x19; q1 = 0xe8; q2 <<= 2; __MapActor_TravelTo(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xcc; q2 = 0x262; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xd0; q2 = 0x256; __Func_80921c4(q0, q1, q2); }
    { PIN3; q0 = 0; q1 = 0xf8; q2 = 0x256; __Func_80921c4(q0, q1, q2); }
    {
        Actor *c = __MapActor_GetActor(0);
        if (c != 0)
            __MapActor_SetPos(1, c->f08, c->f10);
    }
    {
        Actor *c = __MapActor_GetActor(0);
        if (c != 0)
            __MapActor_SetPos(2, c->f08, c->f10);
    }
    {
        Actor *c = __MapActor_GetActor(0);
        if (c != 0)
            __MapActor_SetPos(3, c->f08, c->f10);
    }
    { PIN3; q1 = 0x80; q2 = v; q0 = 1; q1 <<= 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = v; q0 = 2; q1 <<= 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0x80; q2 = v; q0 = 3; q1 <<= 9; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x92; q0 = 0; q1 = 0xfa; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    { PIN3; q2 = 0x96; q0 = 1; q1 = 0xf0; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    { PIN3; q2 = 0x96; q0 = 2; q1 = 0xfe; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    __Func_80921c4(3, 0xf8, (0x9a) << 2);
    __MapActor_SetAnim(0, 1);
    __MapActor_SetAnim(1, 1);
    __MapActor_SetAnim(2, 1);
    { PIN3; q1 = 0xc0; q0 = 0; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 1; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q0 = 2; q1 <<= 8; q2 = 0; __Func_8092adc(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0x14; q1 <<= 8; q0 = 3; __Func_8092adc(q0, q1, q2); }
    __PlaySound(0x95);
    __CutsceneWait(0x28);
    { PIN2; q1 = 0x81; q0 = 0; q1 <<= 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q1 = 0x81; q0 = 1; q1 <<= 1; __MapActor_Surprise(q0, q1); }
    { PIN2; q1 = 0x81; q0 = 2; q1 <<= 1; __MapActor_Surprise(q0, q1); }
    __MapActor_Surprise(3, (0x81) << 1);
    __CutsceneWait(0x3c);
    { PIN3; q0 = 0; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 1; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 2; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q0 = 3; q1 = 0xcccc; q2 = 0x6666; __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q2 = 0x8d; q0 = 0; q1 = 0xf8; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    { PIN3; q2 = 0x8d; q1 = 0xf8; q2 <<= 2; q0 = 1; __Func_809218c(q0, q1, q2); }
    __CutsceneWait(0x14);
    { PIN3; q2 = 0x8d; q0 = 2; q1 = 0xf8; q2 <<= 2; __Func_809218c(q0, q1, q2); }
    { PIN3; q2 = 0x8d; q1 = 0xf8; q2 <<= 2; q0 = 3; __Func_809218c(q0, q1, q2); }
    __CutsceneWait(0x14);
    gState.f1c4 = (int)(&_AREA_6f);
    gState.f1c6 = 0x1e;
    do { } while (0);
    gState.f22b = 3;
    __Func_8091f90((int)(&_AREA_6d), 0x10);
    __Func_8091eb0(0x3e, 3);
    __CutsceneEnd();
}

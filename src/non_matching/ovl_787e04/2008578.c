/* OvlFunc_887_2008578 -- NON-MATCHING, 4 ENCODINGS OF 453.  Size 1172 = 1172,
 * relocations identical, instruction count 453 = 453.
 *
 * Blocker class: reload's ROUND-ROBIN SPILL-REGISTER COUNTER.  BATCH 282 CORRECTED
 * THIS PARK'S ATTRIBUTION: it is NOT local-alloc, and REG_ALLOC_ORDER is not what
 * decides it.
 *
 * BOTH INSTRUCTIONS ARE RELOAD-CREATED.  .18.greg / .19.flow2 show
 * `(insn 1351 (set (reg:SI 2 r2) (reg/v:SI 10 sl)))` -- UID 1351, MANUFACTURED BY
 * RELOAD as a high-to-low copy because `zero` lives in r10 and *thumb_movsi_insn
 * cannot store from a high register -- and the same shape for `g` in r9.  NEITHER IS
 * A LOCAL-ALLOC QUANTITY, so QTY_CMP_PRI -- the priority formula this park originally
 * reasoned from -- NEVER SEES THEM.
 *
 * What decides them is allocate_reload_reg, reload1.c:4925-4945:
 *
 *       /* I is the index in spill_regs.
 *          We advance it round-robin between insns to use all spill regs
 *          equally, so that inherited reloads have a chance
 *          of leapfrogging each other.  *\/
 *       i = last_spill_reg;
 *       for (count = 0; count < n_spills; count++)
 *         { i++; if (i >= n_spills) i -= n_spills; regnum = spill_regs[i]; ...
 *
 * with `i = last_spill_reg` at :5003 and `last_spill_reg = i` on success at :4937.
 * `last_spill_reg` IS FUNCTION-SCOPED STATE ADVANCING ONCE PER SUCCESSFUL
 * RELOAD-REGISTER ALLOCATION.  An exactly complementary r2<->r3 swap across two
 * consecutive high-to-low copies is a PHASE DIFFERENCE IN THAT COUNTER, decided by
 * every earlier reload in the function -- which is precisely why 112 spellings at the
 * two sites were inert and why the identical construct 60 instructions later is right
 * (the phase has rotated back into agreement).
 *
 * TWO COROLLARIES WORTH THE ROW:
 *   AN IDENTICAL INSTRUCTION STREAM UP TO THE DIVERGENCE DOES NOT IMPLY IDENTICAL
 *   RELOAD STATE.  objcmp's first difference is index 129 with everything before it
 *   equal, yet the counter is out of phase -- inherited and shared reloads advance
 *   last_spill_reg WITHOUT EMITTING AN INSTRUCTION.
 *
 *   THE ONLY HANDLE IS THE COUNT OF RELOAD-REGISTER ALLOCATIONS *EARLIER* IN THE
 *   FUNCTION.  That reframes the target from "find the right spelling for these two
 *   stores" to "find an earlier site where an equally-matching spelling costs one more
 *   or one fewer reload".
 *
 * The obvious alternative was ruled out: r3 IS free at the first copy
 * (`str r3,[r6,#8]` immediately precedes and kills it), and -fno-schedule-insns2
 * leaves the whole region unchanged, so reload-time order equals final order.
 *
 * 36 NEW SPELLINGS IN BATCH 282, none better than 4 and most catastrophic: an r3 or r2
 * pinned temp for the `zero` store 409 of 453 and -8 BYTES (the pin destroys the
 * pooled-zero / struct HalfWord arrangement this park's own header documents); a
 * pinned `register unsigned char **gp` in r2 or r3 for `g[0]` 361/362 and +4 bytes; a
 * pinned `register unsigned char *b2` in r1 with `g[0]` hoisted 314 and +4; an `int c3`
 * r3 carrier for the 0x8d << 18 store 4 (inert); dropping the first `do{}while(0)` 5.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_787e04/2008578.c \
 *     asm/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_a_a_a_c.s \
 *     --func OvlFunc_887_2008578
 *
 * THIS WAS THE BATCH'S HIGHEST-VALUE NEAR MISS.  Its file-mate OvlFunc_887_20083f8
 * IS exact and landed (src/overlays/rom_787e04/..._a_a_a_b.c), so the file was one
 * step from converting whole and was split two ways instead.  Four encodings.
 *
 * THE RESIDUE:
 *
 *     ref                      ours
 *     str  r3, [r6, #8]        str  r3, [r6, #8]
 *     mov  r3, sl              mov  r2, sl
 *     str  r3, [r6, #12]       str  r2, [r6, #12]
 *     ldr  r3, [pc]            ldr  r3, [pc]
 *     ...two calls...
 *     mov  r2, r9              mov  r3, r9
 *     ldr  r1, [r2, #0]        ldr  r1, [r3, #0]
 *
 * The ROM uses r3 then r2; we use r2 then r3.  Both are three-register solutions of
 * the same shape, and REG_ALLOC_ORDER STARTS AT r3, so OURS IS THE NATURAL CHOICE
 * and the ROM's implies a conflicting quantity claimed r3 first.
 *
 * WHAT SAYS THIS IS ONE ALLOCATION DECISION RATHER THAN A READING ERROR: THE
 * IDENTICAL SOURCE CONSTRUCT 60 INSTRUCTIONS LATER (`= 0x201` / `= 0x10`) MATCHES
 * EXACTLY.  That is the check to make before treating any permutation as a
 * structural problem.
 *
 * MEASURED FLOOR: 4, ACROSS 112 SPELLINGS.  Reusing an existing local as the
 * carrier; `register int __asm__("r3")` on either temp (19 and 4); naming g[0] (4);
 * naming the offset (15, 26); 0x1c0/0x1c8 literals against (0xe0<<1) (4); five
 * reorderings of the three p6 stores (5-7); `register int zero __asm__("r10")` (4);
 * `unsigned int zero` (4); a v10 int carrier for 0x2b30000 at four positions (4-7);
 * five do{}while(0) positions in that block (4-5).  Flags, diagnostic only:
 * -fno-cse-follow-jumps 4 (inert), -fno-schedule-insns2 78, -fno-gcse 425.
 *
 * EVERY SHIPPED DEVICE IS LOAD-BEARING, each measured by single drop:
 * `register int p8 __asm__("r8")` 406; `struct HalfWord z` 349; `int h = 0x555`
 * carrier 361; `register unsigned char bit __asm__("r6")` 192; four of five
 * do{}while(0) barriers 6/25/5/5.  Two devices were inert singly AND JOINTLY and
 * are dropped from the shipped file.  Path: 384 -> 386 -> 62 (pin pass) -> 49 (r8
 * pin) -> 29 (r6 pin) -> 24 (0x555 carrier) -> 14 -> 4.
 *
 * ================================================================
 * TWO RULES THIS FUNCTION ESTABLISHED
 * ================================================================
 *
 * POOL ORDER IS A READOUT OF EACH CONSTANT'S MODE, AND IT IS A CHEAPER SIGNAL THAN
 * THE POOL'S POSITION.  The ROM's mid-function pool is ordered
 * `0, iwram_3001ebc, 0x555, 0x28a0000, 0x2160000`.  Sorting by max_address (insn
 * address + pool_range) reproduces that order ONLY IF 0 is HImode (range 64) AND
 * 0x555 IS SImode (1020) -- with 0x555 HImode it sorts first, which is what the
 * candidate did.  Adding the `int` carrier for 0x555 ALONE moved the pool to the
 * ROM's position, 27 -> 24.  READ THE ROM'S POOL ORDER TO DECIDE WHICH CONSTANTS
 * NEED THE `int` CARRIER, BEFORE TOUCHING ANY REGISTER.
 *
 * A POOLED ZERO REACHING A `strb` IS THE `struct HalfWord` CASE.  The ROM has
 * `ldr r5, =0 / add r0,#0x55 / strb r5,[r0]`.  *thumb_movqi_insn's alternative 1 is
 * "l" <- "m" (memory only, no `n`), SO A QImode CONSTANT CAN NEVER POOL -- only
 * HImode can.  Six spellings gave `mov` (bare 0, short z, unsigned short z,
 * unsigned char z, pointer-named, a reused int); `volatile short` gives the pool
 * load but costs a stack slot.  The fix is
 * `struct HalfWord { unsigned short v; }; z.v = 0; q[0x55] = z.v;` -- PROMOTE_MODE
 * widens a plain `short` local to SImode BUT NOT A STRUCT FIELD.
 *
 * That second rule was ALREADY IN THE TREE, in
 * src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_c_a_c_a_b.c from batch ~275, and
 * NOT in docs/elevation.md -- which cost the agent that rediscovered it real time.
 * It is now in the doc.  The one-line search that finds it:
 * grep asm/ for a generated `ldrh rN, .L` immediately followed by `strb`.
 *
 * No per-file Makefile flag override applies to this stem.
 */
struct HalfWord { unsigned short v; };

extern unsigned char *iwram_3001ebc[];
extern unsigned char ActorCmd_ARRAY_887__02009ab4[];
extern unsigned char gScript_887__02009b04[];
extern unsigned char gScript_887__02009b34[];

extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __StartThunder(void);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Actor_AddSpriteLayer(unsigned char *a, int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_WaitScript(int slot);
extern void __SetCameraTarget(int slot, int a);
extern void __Func_800c5b4(void);
extern void __Func_800c5fc(void);
extern void __Func_800fe9c(void);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8019aa0(int a, int b, int c);
extern void __Func_80118a8(int n);
extern void __Func_80118c0(int n);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8091e9c(int n);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_8093304(int a);
extern void __Func_8095240(void);
extern void __Func_8095268(void);
extern void OvlFunc_887_20097e4(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_887_2008578(void)
{
    unsigned char **g;
    unsigned char *p7;
    unsigned char *p6;
    register int p8 __asm__("r8");
    unsigned char *p;
    unsigned char *t;
    unsigned char *sc;
    register unsigned char bit __asm__("r6");
    int m;
    int zero;
    struct HalfWord z;

    g = iwram_3001ebc;
    do { } while (0);
    t = g[0];
    p7 = *(unsigned char **)((unsigned char *)iwram_3001ebc - 0x4c);
    p6 = *(unsigned char **)(t + (0xf0 << 1));
    p8 = *(int *)(__MapActor_GetActor(0x11) + 0x50);
    __CutsceneStart();
    __MapActor_SetPos(0xb, 0, 0);
    __MapActor_SetPos(0xc, 0, 0);
    __MapActor_SetPos(0xd, 0, 0);
    __MapActor_SetPos(0xe, 0, 0);
    __MapActor_SetPos(0xf, 0, 0);
    { PIN3; q2 = 0; q1 = 0; q0 = 0x10; __MapActor_SetPos(q0, q1, q2); }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    { PIN2; q1 = 0x12; q0 = 0; __MapActor_SetAnim(q0, q1); }
    zero = 0;
    { unsigned char *r = (unsigned char *)p8;
      int h = 0x555;
      *(short *)(r + 0x1e) = h; }
    z.v = 0;
    __MapActor_GetActor(0x11)[0x55] = z.v;
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x11), 0);
    { PIN3; q1 = 0x90 << 18; q2 = 0x28a0000; q0 = 0x11;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_80118a8(7);
    { PIN3; q2 = 0xac; q1 = 0x2160000; q2 <<= 18; q0 = 8;
      __MapActor_SetPos(q0, q1, q2); }
    __Func_800c5b4();
    __Func_8093304(8);
    do { } while (0);
    m = 0xe52;
    { PIN3; q1 = 1; q0 = m; q2 = 0; __Func_8019aa0(q0, q1, q2); }
    __CutsceneWait(0x28);
    { PIN3; q0 = 0x80; q0 <<= 9; q1 = 0x80; q2 = 0x80; q1 <<= 9; q2 <<= 9;
      __Func_8012330(q0, q1, q2); }
    __Func_8093304(8);
    { PIN3; q1 = 1; q0 = m + 1; q2 = 0; __Func_8019aa0(q0, q1, q2); }
    __Func_800c5fc();
    __CutsceneWait(0x28);
    *(int *)(p7 + 0xec) = 0xa4 << 17;
    *(int *)(p7 + 0xf0) = 0x96 << 18;
    *(int *)(p7 + 0xf4) = 0x9c << 18;
    *(int *)(p7 + 0xf8) = 0xcc << 18;
    *(int *)(p6 + 8) = 0x8d << 18;
    do { } while (0);
    *(int *)(p6 + 0xc) = zero;
    do { } while (0);
    *(int *)(p6 + 0x10) = 0x2b30000;
    __Func_800fe9c();
    __WaitFrames(1);
    *(int *)(g[0] + (0xe0 << 1)) = 0x209;
    *(int *)(g[0] + (0xe4 << 1)) = 0x40;
    __StartThunder();
    { short *pp = (short *)(g[3] + 0x1f84);
      int one = 1;
      *pp = one; }
    __Func_8095240();
    __WaitFrames(0x1e);
    __MapTransitionIn();
    __WaitMapTransition();
    __Func_8095268();
    { PIN2; q1 = 4; q0 = 8; __MapActor_DoAnim(q0, q1); }
    __MessageID(m + 2);
    { PIN3; q2 = 0x3c; q0 = 0x9008; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x28);
    { PIN2; q1 = 1; q0 = 8; __Func_80925cc(q0, q1); }
    __CutsceneWait(0x28);
    { PIN3; q2 = 0x14; q0 = 0x9008; q1 = 0; __Func_8093040(q0, q1, q2); }
    { PIN2; q1 = 2; q0 = 0; __Func_80925cc(q0, q1); }
    __Func_80118c0(7);
    __CutsceneWait(0x14);
    __Func_80118a8(8);
    { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __MapActor_SetAnim(0, 0x13);
    { PIN3; q1 = 0x22d; q2 = 0x2a7; q0 = 0; __Func_8092158(q0, q1, q2); }
    __Func_80118c0(8);
    __Func_80118a8(9);
    { PIN3; q2 = 0xaa; q1 = 0x22b; q2 <<= 2; q0 = 0;
      __Func_8092158(q0, q1, q2); }
    __CutsceneWait(0x1e);
    { PIN3; q1 = 0xd0; q2 = 0; q1 <<= 8; q0 = 8; __Func_8092adc(q0, q1, q2); }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 1);
    __MapActor_Jump(0, 4, 0);
    { PIN3; q2 = 0x2a2; q0 = 0; q1 = 0x21f; __Func_80921c4(q0, q1, q2); }
    __Func_8092b08(0, 3);
    { PIN3; q1 = 0x80; q2 = 0x28; q0 = 0; q1 <<= 7; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 4; q0 = 8; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    { PIN2; q0 = 0x9008; q1 = 0; __ActorMessage(q0, q1); }
    OvlFunc_887_20097e4();
    __Func_809259c(8, 2);
    { PIN3; q1 = 0; q2 = 0x14; q0 = 0x9008; __Func_8093040(q0, q1, q2); }
    p = __MapActor_GetActor(8) + 0x5a;
    *p = 0xfe & *p;
    { PIN3; q2 = 0xaa; q2 <<= 2; q1 = 0x21e; q0 = 8;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(1);
    p = __MapActor_GetActor(8) + 0x5a;
    bit = 1;
    *p = bit | *p;
    __CutsceneWait(0xa);
    __Func_80925cc(8, 2);
    __Actor_AddSpriteLayer(__MapActor_GetActor(0), 0xe2);
    __SetFlag(0x21);
    __PlaySound(0x7e);
    { PIN2; q1 = 7; q0 = 0; __Func_8092950(q0, q1); }
    __CutsceneWait(0xa);
    { PIN2; q1 = 0; q0 = 0; __Func_8092950(q0, q1); }
    __CutsceneWait(0x14);
    p = __MapActor_GetActor(8) + 0x5a;
    *p = 0xfe & *p;
    { PIN3; q2 = 0xac; q1 = 0x216; q2 <<= 2; q0 = 8;
      __Func_80921c4(q0, q1, q2); }
    __CutsceneWait(1);
    p = __MapActor_GetActor(8) + 0x5a;
    *p = bit | *p;
    __CutsceneWait(0x14);
    { PIN3; q1 = 0xc0; q2 = 0xc0; q0 = 8; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    { PIN3; q1 = 0xc0; q2 = 0xc0; q0 = 0; q1 <<= 9; q2 <<= 8;
      __MapActor_SetSpeed(q0, q1, q2); }
    __SetCameraTarget(8, 1);
    p = __MapActor_GetActor(0) + 0x23;
    sc = ActorCmd_ARRAY_887__02009ab4;
    *p = bit | *p;
    __MapActor_SetBehavior(8, sc);
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(0, sc);
    __MapActor_WaitScript(8);
    { PIN3; q0 = 8; q1 = 0x1a3; q2 = 0x295; __Func_80921c4(q0, q1, q2); }
    { PIN3; q1 = 0xcc; q2 = 0x295; q0 = 8; q1 <<= 1;
      __Func_80921c4(q0, q1, q2); }
    __MapActor_SetAnim(8, 1);
    __MapActor_SetAnim(0, 1);
    { PIN3; q1 = 0x80; q0 = 8; q1 <<= 7; q2 = 0xa; __Func_8092adc(q0, q1, q2); }
    { PIN2; q1 = 0; q0 = 0x8008; __Func_8092c40(q0, q1); }
    if (__Func_8091c7c(0, 0) == 0)
        *(short *)(g[0] + (0xec << 1)) += 1;
    __CutsceneWait(0x14);
    { PIN3; q2 = 0x14; q0 = 0x8008; q1 = 0; __Func_8093040(q0, q1, q2); }
    __MapActor_SetAnim(0, 3);
    { PIN2; q1 = 3; q0 = 8; __MapActor_DoAnim(q0, q1); }
    __CutsceneWait(0x14);
    __MapActor_SetBehavior(8, gScript_887__02009b04);
    __MapActor_SetBehavior(0, gScript_887__02009b34);
    __CutsceneWait(0x14);
    *(int *)(g[0] + (0xe0 << 1)) = 0x201;
    *(int *)(g[0] + (0xe4 << 1)) = 0x10;
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x14);
}

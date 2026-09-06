/* OvlFunc_928_20089dc  --  0x020089dc
 *   [asm/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_c_c.s, 1st of 1]
 *
 * 304 instructions of straight-line cutscene behind one save-flag fork: the
 * short arm re-plays a line and ends, the long arm walks actor 0 out of the
 * way, runs NINE OvlFunc_common0_10c effect calls around two tasks
 * (OvlFunc_928_2008370 and OvlFunc_928_2008358, each started at priority
 * 0xc80 and stopped again), drives actor 0xe along two TravelTo legs and sets
 * save bit 0x202.  Message bases 0x17f2 / 0x17f4.  The frame is
 * `sub sp, #0x44` = 16 bytes of outgoing argument area + a 0x28 `struct Cmd`
 * at sp+0x10 + `int v[3]` at sp+0x38.
 *
 * =====================================================================
 * THE EVICTION RECIPE APPLIED, EXACTLY AS WRITTEN -- AND THE TWO PINS ARE
 * A SET, NOT TWO LEVERS
 * =====================================================================
 *
 * Scored 29 shared symbols with 31 high-register references across four
 * distinct high registers, the top of its band.  Eight of the 31 are the
 * prologue/epilogue save-restore boilerplate any four-high-register frame
 * carries (`mov r7,r11 / mov r6,r10 / mov r5,r9 / push` and the mirror), so
 * the real count is 23 across four values:
 *
 *     r8  = 0x9c << 17 = 0x01380000   the shared Z of eight effect calls,
 *                                     1 def + 8 uses
 *     r9  = 0xa8 << 16 = 0x00a80000   the X of four of them, 1 def + 4 uses
 *     r10 = &s, then 0xffff8000       TWO disjoint ranges, 2 defs + 4 uses
 *     r11 = 0xffffcccd                1 def + 2 uses
 *
 * THE PLAIN FIRST TRANSCRIPTION (v1.c: literal arguments, no pins, no
 * reordering) IS 68 OF 320 ENCODINGS AT EXACTLY THE ROM'S 784 BYTES, and every
 * one of those 23 high-register references is already correct.  gcc reaches
 * the ROM's high-register assignment unaided here too; that is the fifth
 * function in a row.  No pin in the shipped file touches r8-r11.
 *
 * WHAT IS WRONG IN v1 IS THE RECIPE'S "WRONG TENANTS", in the LOW callee-saved
 * file.  The ROM keeps the task function pointer in r5 across thirteen calls
 * (`ldr r5,=OvlFunc_928_2008370` ... `mov r0,r5 / bl __StopTask`) and
 * rematerialises two constants at both of their sites: `mov r1,#0xc8 / lsl
 * r1,#4` (0xc80, the two __StartTask priorities) and `mov r2,#0x9c / lsl
 * r2,#1` (0x138, the two __MapActor_TravelTo destinations).  gcc does the
 * opposite -- it commons BOTH constants into registers (0xc80 into r9, 0x138
 * into r5) and then has nothing left for the pointer, which it reloads from
 * the pool at each of the two sites.  Same instruction count, same 784 bytes:
 * a pure swap of tenants.  YOU-HAVE-MORE-REGISTERS is not the tell here; the
 * tell is TWO REBUILT CONSTANTS HELD AND ONE HELD VALUE REBUILT.
 *
 * THE PIN IS AN EVICTION DEVICE.  `register int q1 __asm__("r1")` at the first
 * __StartTask and `register int q2 __asm__("r2")` at both __MapActor_TravelTo
 * sites take 0xc80 and 0x138 out of the commoning pool; the allocator then
 * picks the ROM's set itself and the pointer lands in r5 with no pin on it at
 * all.  68 -> 4 in one step (v4.c).
 *
 * NEW -- AND IT WOULD HAVE BEEN THROWN AWAY BY A GREEDY PASS.  Grepped first
 * as "greedy", "one at a time", "individually inert", "as a set", "both pins",
 * "commoned constant" -- docs/elevation.md has the removal-side rule
 * ("Re-verify the survivors AS A SET", "strip one at a time to get CANDIDATES,
 * then remove greedily") and the addition-side caution at "add the sites ONE
 * AT A TIME, because the failure is not gradual", but nothing says this:
 *
 *     EVICTION PINS COMPETE FOR THE SAME REGISTER, SO THEY MUST BE ADDED
 *     TOGETHER.  EACH ALONE IS STRICTLY WORSE THAN NONE.
 *
 *     no pin at all                    68 of 320, size and count right
 *     the __StartTask pin alone       171 of 320, 780 bytes (4 SHORT)
 *     the two TravelTo pins alone     184 of 320, 780 bytes (4 SHORT)
 *     both                              4 of 320
 *
 * The mechanism is visible in both failures and it is the same one.  With only
 * the __StartTask pin, 0xc80 is duly rebuilt at both sites -- and 0x138 is
 * STILL commoned, still in r5, and the pointer is still pooled twice.  With
 * only the TravelTo pins, 0x138 is duly rebuilt -- and 0xc80 MOVES INTO THE
 * REGISTER THE FIRST PIN FREED, `mov r5,#0xc8 / lsl r5,#4` with `mov r1,r5` at
 * both __StartTasks, and the pointer is pooled twice again.  Evicting one
 * tenant of a one-register slot just lets the other tenant in.  A screen that
 * adds candidate pins one at a time and keeps only the ones that improve
 * REJECTS BOTH OF THESE and reports the function as blocked at 68.
 *
 * Practically: when the residue is "N constants the ROM rebuilds are held and
 * M values the ROM holds are rebuilt", the eviction pins are a SET of size N
 * and the first useful measurement is all N at once.  Minimise afterwards --
 * here one of the three sites turned out to be free (see below).
 *
 * =====================================================================
 * WHAT THE REMAINING 4 WERE: TWO ORDERING PINS, DIFFERENT WIDTHS AND
 * DIFFERENT DIRECTIONS, IN ONE FUNCTION
 * =====================================================================
 *
 * The 4 encodings left after eviction are two transposed pairs of the same
 * shape -- the ROM issues a CHEAP `mov` for one argument BEFORE the last
 * `lsl` of an expensive one, and we issue it after:
 *
 *     rom   ... lsl r1,#0xa / mov r0,#0xe / lsl r2,#0x9 / bl SetSpeed
 *     ours  ... lsl r1,#0xa / lsl r2,#0x9 / mov r0,#0xe / bl SetSpeed
 *
 *     rom   mov r2,#0x9c / mov r1,#0x92 / lsl r2,#0x1 / bl TravelTo
 *     ours  mov r2,#0x9c / lsl r2,#0x1 / mov r1,#0x92 / bl TravelTo
 *
 * Both are the recorded "the SLOT `mov` scheduled ahead of the expensive
 * argument's build" ordering pin, and NEITHER TRANSFERS TO THE OTHER:
 *
 *   - __MapActor_SetSpeed(0xe, 0xc0<<10, 0xc0<<9) needs r0 AND r1 named.  r0
 *     alone is 4 (WORSE than the 2 of no pin at all); no pin is 2; adding r2
 *     as well is an exact tie, so the shipped width is the minimum.  Its
 *     DIRECTION IS FREE -- assigning q1 before q0 is an exact tie.
 *   - __MapActor_TravelTo(0xe, 0x92, 0x9c<<1) needs r1 AND r2 named, ASCENDING.
 *     r2 alone is 2, no pin is 2, and DESCENDING (q2 then q1) is also 2, i.e.
 *     identical to no pin.  Here the direction IS load-bearing.
 *   - The other TravelTo site needs r2 only; widening it to r0+r2 or r0+r1+r2
 *     is an exact tie.
 *
 * That is "fill direction does NOT transfer" confirmed WITHIN ONE FUNCTION,
 * between two sites twelve instructions apart, both `mov`-before-`lsl`
 * transpositions, both fixed by naming two registers -- one needing ascending
 * and one indifferent.  Re-measure it per site; there is no function-wide
 * answer even when the two residues look identical.
 *
 * =====================================================================
 * THE HOLE TEST, COMPUTED AND THEN CHECKED
 * =====================================================================
 *
 * Walking the reference with a symbolic register file, every argument the ROM
 * supplies with `mov rLOW, rHIGH` or `mov rLOW, rLOW` is a hole BY
 * CONSTRUCTION: all nine OvlFunc_common0_10c sites (r5/r6/r7/r8/r9/r10/r11
 * feeding r0-r3 and the four outgoing stack words) and the `mov r0, r5` at the
 * first __StopTask.  NOT ONE of them is pinned, and the three that were
 * measured all hurt:
 *
 *     PIN r0-r3 at the 1st OvlFunc_common0_10c    192 of 320, 4 bytes SHORT
 *     PIN r0-r3 at the 7th OvlFunc_common0_10c     74 of 320
 *     PIN r0 for the task pointer at __StartTask  209 of 320, 4 bytes SHORT
 *
 * The last is the sharpest: it is the ROM's HELD value, and pinning it to a
 * call-clobbered register destroys exactly the live range the whole eviction
 * exercise was buying.  "Leave the ROM's HELD values as plain locals" is not
 * advice about tidiness; the pin actively costs 209.
 *
 * THE ALL-CHEAP RULE HELD.  Pins added at __CutsceneWait(0x1e) and at the
 * all-`mov #imm8` __Func_80925cc(0, 2) are exact TIES.
 *
 * =====================================================================
 * THE FOUR NON-PIN LEVERS
 * =====================================================================
 *
 * 1. `int v[3]` IS DECLARED BEFORE `struct Cmd s`, and it is 2 encodings the
 *    other way.  Frame layout follows DECLARATION ORDER, LAST-DECLARED LOWEST
 *    (docs/elevation.md, and src/overlays/rom_7f2f14/ovl_30_a_c_c_a_c_a.c
 *    measures the same pair the other way round because ITS `s` is the higher
 *    slot).  Here `s` is at sp+0x10 and `v` at sp+0x38, so `v` goes first.
 *    The array must also be an ARRAY: three scalars `v0, v1, v2` handed to
 *    __vec3_translate as `&v0` is 267 of 320 at 768 bytes, 16 SHORT.
 *
 * 2. `s.f0 = 1;` HAS AN EXACT POSITION -- between `__Func_80925cc(0xe, 2)` and
 *    the first OvlFunc_common0_10c, which is where the ROM materialises
 *    `add r2, sp, #0x10 / str r3,[r2] / mov r10,r2`.  One statement earlier is
 *    11; after the two effect calls is 231 at 4 bytes SHORT; at the top of the
 *    function is 281 at 792 bytes, 8 LONG.  The store is what forces &s into a
 *    register early, and the whole r10 range hangs off it.
 *
 * 3. THE THREE `s` FIELD STORES GO IN THE ROM'S ORDER, f18 then f1c then f20;
 *    reversed is 9 with relocations differing.
 *
 * 4. THE HALFWORD STORE OF 0x8000 MUST GO THROUGH A **SIGNED** `short *`, AND
 *    THIS IS THE OPPOSITE OF THE RECORDED ADVICE.  docs/elevation.md's
 *    "The HImode literal cuts both ways" says a pooled `*(short *)p = 0x80<<8`
 *    is cured by an `unsigned short` destination, which gives `mov r3,#0x80 /
 *    lsl r3,#8`.  Here the ROM WANTS the pooled form, because that pool word is
 *    SHARED: 0xffff8000 is the 4th argument of the sixth OvlFunc_common0_10c,
 *    lives in r10 across two calls, and the store is `mov r2,r10 / strh
 *    r2,[r3,#0x1e]` -- one pool entry, no rebuild.
 *
 *        `*(short *)(q + 0x1e) = (short)0xffff8000;`   exact  (shipped)
 *        `*(short *)(q + 0x1e) = 0x8000;`              exact  (tie; warns)
 *        `*(unsigned short *)(q + 0x1e) = 0x8000;`     284 of 320, 796 bytes,
 *                                                      12 LONG
 *
 *    NEW, grepped as "unsigned short *", "sign-extend", "shared pool", "one
 *    pool entry", "HImode": the recorded rule is written for a value used ONCE.
 *    The BOUNDARY is whether the halfword constant is shared with an SImode use
 *    elsewhere in the function.  If it is, the signed destination is REQUIRED:
 *    only through it does the truncated `const_int` equal the SImode value, so
 *    only then does CSE reach the register the ROM reuses.  Making the pointer
 *    unsigned "to fix the pool word" splits one value into two and costs 12
 *    bytes.  Read the ROM first: `mov rLOW, rHIGH` feeding the `strh` means DO
 *    NOT apply the unsigned cure.
 *
 *    The two SMALL halfword constants in this function need nothing at all --
 *    `s.f18 = 0x11b` (the ROM pools it too) and `s.f20 = 0x80 << 7` (the ROM
 *    builds it `mov r3,#0x80 / lsl r3,#7`) are both exact as plain literals,
 *    and an `int` intermediate for either is an exact tie.  That is
 *    "the HImode-literal rule is narrower than stated: only 0 and >= 0x8000"
 *    confirmed on both sides in one function.
 *
 * A READING NOTE ON r7, WHICH COSTS NOTHING.  The ROM's callee-saved zero IS
 * the __GetFlag result: `mov r7,r0 / cmp r7,#0 / beq .La2a`, and on the arm the
 * branch proves it zero r7 is then spent as the constant 0 at fourteen sites
 * (three `str` into `v`, one actor field, ten outgoing stack words), while the
 * OTHER arm uses plain `mov rN,#0`.  gcc's cse records the branch equivalence
 * and reproduces this from bare `0` literals with nothing named.  Both
 * spellings ship: an explicit `int zero = 0;` at the top, used at the same
 * sites, is an exact TIE.  Worth knowing only so the shape is not mistaken for
 * the `int zero = 0;` lever the sibling
 * src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_c_b.c genuinely needed -- there
 * the ROM has a `mov r5,#0` with no flag in sight.
 *
 * =====================================================================
 * NO FLAG GROUP
 * =====================================================================
 *
 * `tryc.makefile_flags` returns the EMPTY set for the scratch path AND for
 * src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_c_c.c, so the two compile under
 * identical flags and the "flags depend on the path you screen from" trap does
 * not apply.  Plain -O2.  objcmp run against the ORIGINAL asm/ path prints no
 * `(built with: ...)` line.
 *
 * =====================================================================
 * MEASURED WORSE, from the shipped file, one change at a time
 * =====================================================================
 *
 *     drop the __StartTask r1 pin            183 of 320, 780 bytes (4 SHORT),
 *                                            relocations differ
 *     drop the 1st TravelTo r2 pin           172 of 320, 780 bytes (4 SHORT),
 *                                            relocations differ
 *     both eviction pin sets dropped (v1)     68 of 320, size and count right,
 *                                            relocations differ
 *     only the __StartTask pin               171, 780 bytes (4 SHORT)
 *     only the TravelTo pins                 184, 780 bytes (4 SHORT)
 *     drop the 2nd TravelTo pin                2
 *     2nd TravelTo pin narrowed to r2 only     2
 *     2nd TravelTo pin filled DESCENDING       2
 *     drop the SetSpeed pin                    2
 *     SetSpeed pin narrowed to r0 only         4
 *     SetSpeed pin as r0 only, on top of the
 *       eviction set but before the TravelTo
 *       ordering pin                           6
 *     `struct Cmd s` declared before `int v[3]` 2
 *     `int v0, v1, v2;` instead of `int v[3]` 267, 312 encodings, 768 bytes
 *                                            (16 SHORT), relocations differ
 *     `s.f0 = 1` one statement earlier         11, relocations differ
 *     `s.f0 = 1` after the two effect calls   231, 780 bytes (4 SHORT)
 *     `s.f0 = 1` at the top of the function   281, 792 bytes (8 LONG)
 *     f20/f1c/f18 store order reversed          9, relocations differ
 *     `unsigned short *` + 0x8000 for the
 *       shared halfword store                 284, 796 bytes (12 LONG)
 *     `v[0] >> 16` for `v[0] / 0x10000`       262, 311 encodings, 764 bytes
 *                                            (20 SHORT)
 *     PIN r0-r3 at the 1st effect call        192, 780 bytes (4 SHORT)
 *     PIN r0-r3 at the 7th effect call         74
 *     PIN r0 for the task pointer             209, 780 bytes (4 SHORT)
 *
 * EXACT TIES -- the set is minimal but not unique:
 *
 *     a pin at the 2nd __StartTask, in addition
 *     the 1st TravelTo pin widened to r0+r2, or to r0+r1+r2
 *     the SetSpeed pin widened to r0+r1+r2
 *     the SetSpeed pin filled DESCENDING (q1 before q0)
 *     pins at the all-cheap __CutsceneWait(0x1e) and __Func_80925cc(0, 2)
 *     `s.f20 = 0x4000` instead of `0x80 << 7`
 *     an `int h` intermediate for the `s.f20` store
 *     `unsigned short f18` instead of `short f18`
 *     `unsigned char *f1c` instead of `void *f1c`
 *     `void *` for OvlFunc_common0_10c's 8th parameter
 *     unsigned parameter types on __vec3_translate
 *     `extern void __CutsceneEnd();` (empty parameter list)
 *     naming the __GetFlag result in a local, and `if/else` instead of the
 *       early `return`
 *     a pin on r0 at __GetFlag(0x202) -- the sibling's lever, inert here
 *       because 0x202's two sites are already both plain `ldr r0,=0x202`
 *     an explicit `int zero = 0;` for the flag-result zero
 *     naming either `__MapActor_GetActor(0) + 0x5a` in a local, or the
 *       actor 0x13 pointer chased for the halfword store
 *
 * MINIMISED TO A FIXPOINT.  Two locals and one pin present in the first
 * matching candidate were removed at zero cost (the `p` and `q` pointer
 * locals and the pin at the second __StartTask), and every surviving pin was
 * then narrowed to its minimum width; a second full teardown pass over the
 * survivors finds all four pins and all four non-pin levers still load-bearing.
 *
 * =====================================================================
 * LANDING NEEDS NOTHING BUT THE FILE, AND NO LINKER EDIT
 * =====================================================================
 *
 * The .s holds exactly ONE function (`grep -c thumb_func_start` = 1) and emits
 * no `.section`, `.data`, `.rodata`, `.bss`, `.word`, `.byte`, `.align`,
 * `.incbin` or `.global` line at all, so there is no section to remap.
 * Grepped across the whole tree on FULL PATH, exactly one line anywhere names
 * the object:
 *
 *     overlays/rom_7b6668/overlay.ld:37
 *         asm/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_c_c.o(.text)
 *
 * and it stays as it is -- the tree default rule `asm/%.o: src/%.c`
 * (Makefile:146) builds `asm/.../X.o` from `src/.../X.c`, which is why the
 * already-landed siblings leave their overlay.ld lines saying `asm/`.  There is
 * no second tree hit for the basename, in prose or anywhere else.  Landing is:
 * add this .c as src/overlays/rom_7b6668/ovl_314_c_c_a_c_c_a_c_c.c, delete the
 * .s.
 *
 * The `.L1714` extern resolves inside this overlay: it is `.global` in
 * asm/overlays/rom_7b6668/ovl_314_c_c_c.s:215 and defined at :229 as an
 * `.incbin` of orig.bin 0x1714..0x1740, and that object's `.data` is already
 * on overlays/rom_7b6668/overlay.ld:55
 * (`asm/overlays/rom_7b6668/ovl_314_c_c_c.o(.data)`).  The number is high
 * enough to be safe from the recorded "a short `.LN` extern can be captured by
 * gcc's own labels" trap -- gcc emits only `.L2`..`.L7` for this function,
 * checked in the generated .s.
 *
 * VERDICT:
 *   OK OvlFunc_928_20089dc -- 784 bytes, 320 encodings and 61 relocations
 *   identical
 *
 * -- worked in scratch_elev/b247/f20089dc; sweep.sh re-measures any set of
 *    candidates in one container invocation and fulldiff.py prints the full
 *    aligned instruction diff for one.
 */
struct Cmd {
    int f0;
    unsigned char pad04[0x18 - 0x04];
    short f18;
    unsigned char pad1a[0x1c - 0x1a];
    void *f1c;
    short f20;
    unsigned char pad22[0x28 - 0x22];
};

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ActorMessage(int slot, int a);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, void *script);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_WaitScript(int slot);
extern void __StartTask(void (*fn)(void), int prio);
extern void __StopTask(void (*fn)(void));
extern void __vec3_translate(int a, int b, int *v);
extern void __Func_809202c(void);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct Cmd *p);
extern unsigned char gScript_928__020096a0[];
extern unsigned char L1714[] __asm__(".L1714");
extern void OvlFunc_928_2008358(void);
extern void OvlFunc_928_2008370(void);

#define PIN_R0 register int q0 __asm__("r0")
#define PIN_R1 register int q1 __asm__("r1")
#define PIN_R2 register int q2 __asm__("r2")

void OvlFunc_928_20089dc(void)
{
    int v[3];
    struct Cmd s;

    __CutsceneStart();
    if (__GetFlag(0x202) != 0) {
        __Func_809280c(0xe, 0, 0);
        __CutsceneWait(0xa);
        __MessageID(0x17f4);
        __ActorMessage(0xe, 0);
        __Func_8092adc(0xe, 0, 0xa);
        __CutsceneEnd();
        return;
    }
    __MessageID(0x17f2);
    __ActorMessage(0xe, 0);
    __Func_80925cc(0, 2);
    *(__MapActor_GetActor(0) + 0x5a) &= 0xfe;
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;
    __vec3_translate(0xfff80000,
                     *(unsigned short *)(__MapActor_GetActor(0) + 6), v);
    __MapActor_SetAnim(0, 2);
    __Func_809228c(0, v[0] / 0x10000, v[2] / 0x10000);
    __MapActor_WaitMovement(0);
    *(__MapActor_GetActor(0) + 0x5a) |= 1;
    __CutsceneWait(0x1e);
    __Func_80925cc(0xe, 2);
    s.f0 = 1;
    OvlFunc_common0_10c(0xc0 << 16, 0, 0x9c << 17, 0x1999, 0x3333, 0, 0x20001, 0);
    OvlFunc_common0_10c(0xc0 << 16, 0, 0x9c << 17, 0x3333, 0x1999, 0, 0x20001, 0);
    __PlaySound(0x84);
    { PIN_R1; q1 = 0xc8 << 4; __StartTask(OvlFunc_928_2008370, q1); }
    *(int *)(__MapActor_GetActor(0xe) + 0x28) = 0xc0 << 11;
    *(int *)(__MapActor_GetActor(0xe) + 0x48) = 0x80 << 9;
    *(int *)(__MapActor_GetActor(0xe) + 0x44) = 0;
    { PIN_R0; PIN_R1; q0 = 0xe; q1 = 0xc0 << 10;
      __MapActor_SetSpeed(q0, q1, 0xc0 << 9); }
    { PIN_R2; q2 = 0x9c << 1; __MapActor_TravelTo(0xe, 0xa8, q2); }
    __MapActor_WaitMovement(0xe);
    __PlaySound(0x86);
    __MapActor_SetBehavior(0x13, gScript_928__020096a0);
    __StartTask(OvlFunc_928_2008358, 0xc8 << 4);
    s.f18 = 0x11b;
    s.f1c = L1714;
    s.f20 = 0x80 << 7;
    OvlFunc_common0_10c(0xa8 << 16, 0, 0xa6 << 17, 0, 0, 0, 0xe4 << 15, &s);
    { PIN_R1; PIN_R2; q1 = 0x92; q2 = 0x9c << 1;
      __MapActor_TravelTo(0xe, q1, q2); }
    __MapActor_WaitMovement(0xe);
    __StopTask(OvlFunc_928_2008370);
    OvlFunc_common0_10c(0x90 << 16, 0, 0x9c << 17, 0, 0, 0, 0x20001, 0);
    OvlFunc_common0_10c(0x90 << 16, 0, 0x9c << 17, 0xffffcccd, 0x1999, 0, 0x20001, 0);
    OvlFunc_common0_10c(0x90 << 16, 0, 0x9c << 17, 0xffff8000, 0, 0, 0x20001, 0);
    __MapActor_WaitScript(0x13);
    __PlaySound(0x7c);
    OvlFunc_common0_10c(0xa8 << 16, 0x80 << 12, 0x9c << 17, 0, 0, 0, 0x20001, 0);
    OvlFunc_common0_10c(0xa8 << 16, 0x80 << 12, 0x9c << 17, 0x3333, 0, 0, 0x20001, 0);
    OvlFunc_common0_10c(0xa8 << 16, 0x80 << 12, 0x9c << 17, 0xffffcccd, 0, 0, 0x20001, 0);
    __StopTask(OvlFunc_928_2008358);
    *(short *)(*(unsigned char **)(__MapActor_GetActor(0x13) + 0x50) + 0x1e)
        = (short)0xffff8000;
    *(int *)(__MapActor_GetActor(0xe) + 0x44) = 0x80 << 7;
    *(int *)(__MapActor_GetActor(0xe) + 0x48) = 0x80 << 9;
    __CutsceneWait(0x1e);
    __Func_8092adc(0xe, 0, 0x14);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x14);
    __Func_8093040(0xe, 0, 0x14);
    __SetFlag(0x202);
    __Func_809202c();
    __CutsceneEnd();
}

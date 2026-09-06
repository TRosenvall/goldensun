/* OvlFunc_964_200a0a4 -- elevated. Extracted from
 * goldensun/asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a.s, which holds THREE
 * functions:
 *
 *   OvlFunc_964_2009fdc   43 instructions   BLOCKED (see below)
 *   OvlFunc_964_200a040   43 instructions   BLOCKED (see below)
 *   OvlFunc_964_200a0a4  241 instructions   THIS FILE -- byte-identical
 *
 * VERDICT
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a.s --func OvlFunc_964_200a0a4
 *
 *   OK OvlFunc_964_200a0a4 -- 604 bytes, 246 encodings and 53 relocations identical
 *
 *   (604 = 0x25c = 0x200a300 - 0x200a0a4, the whole ROM span including the
 *   trailing literal pool; overlay.map:499 gives the .o's .text as 0x324 at
 *   0x2009fdc.)  Re-run twice, same line both times.
 *
 * ============================ SAY IT LOUDLY ============================
 * THE MATCH DEPENDS ON A FLAG GROUP, AND THE FLAG GROUP THIS PATH INHERITS
 * TODAY IS THE WRONG ONE.
 *
 * Makefile:853 carries
 *     asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o: src/.../ovl_30_c_c_c_a_a%.c
 *         $(GCC296_CC) $(O1_CFLAGS) ...
 * and `%` = `_a`, so this TU inherits -O1.  tools/tryc.py and tools/objcmp.py
 * both read that rule, so BOTH TOOLS SCREEN THIS FILE AT -O1 BY DEFAULT.
 *
 * Measured on the final C, objcmp against the ORIGINAL asm/ path:
 *     inherited -O1 (what the tree does today)  604 vs 608 bytes, 150 encodings differ
 *     plain -O2                                 604 vs 608 bytes,  89 encodings differ
 *     -O2 -fno-rerun-cse-after-loop (CSE_CFLAGS)              OK, identical
 *
 * So landing this file REQUIRES an explicit Makefile rule -- per
 * §"An explicit rule beats a pattern rule -- use that instead of narrowing"
 * (docs/elevation.md:4089) -- applying $(CSE_CFLAGS) (Makefile:723,
 * GCC296_CFLAGS -fno-rerun-cse-after-loop) to this object.  Do NOT narrow the
 * ovl_30_c_c_c_a_a% wildcard: src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_b.c
 * (OvlFunc_964_200a300) is green under it today.
 *
 * This is the same prefix trap as Makefile:354 ("THREE MIS-SCOPED O1
 * WILDCARDS") and docs/elevation.md:15719 ("the FOURTH mis-scoped -O1
 * wildcard") -- an instance of a known class, not a new finding.  It also
 * re-confirms §"Screening cannot see a wildcard" (docs/elevation.md:6660) from
 * the other direction: here screening DID see the wildcard, and the wildcard
 * was wrong.
 *
 * The flag itself was predicted from the listing before any C was written, by
 * §"`GetFlag(id)` guarding a block that ends `SetFlag(id)` means `CSE_CFLAGS`"
 * (docs/elevation.md:6384).  The ROM materialises `ldr r0, =0x984` FOUR times:
 * once before each of two conditional branches and once inside each guarded
 * block.  At -O2 gcc commons the pair inside the else arm into r5 and reaches
 * it with `mov r0, r5`.
 * ======================================================================
 *
 * LEVERS, WITH MECHANISMS
 *
 * 1. THE FLAG GROUP: CSE_CFLAGS.  Mechanism as recorded at :6384 -- the rerun
 *    of CSE after loop optimisation commons the guard's flag id with the id
 *    used by the SetFlag/ClearFlag that the guard dominates.  Worth 2
 *    instructions and every following relocation.
 *
 *    REFINEMENT (new, small): all five prior cases were `GetFlag(id) ...
 *    SetFlag(id)`.  This one has TWO guard/act pairs in OPPOSITE ARMS of one
 *    if/else, and the arm that actually commons is the one whose act is
 *    __ClearFlag, not __SetFlag.  The rule is about DOMINANCE and the identity
 *    of the id, not about which setter is called; `ClearFlag` appears once
 *    elsewhere in docs/elevation.md (line 1908) and never in this rule.  Sixth
 *    confirmation of the rule, first with ClearFlag.
 *
 *    Also measured, as :6384 predicts: SEPARATE NAMED LOCALS DO NOT DEFEAT IT.
 *    Three distinct `int id = 0x984;` locals, one per site, in the arms that
 *    need them, measured exactly the same 6 differing at plain -O2 as the bare
 *    literals.  Six of six for the rule now.
 *
 * 2. `int n = 0;` AT THE DECLARATION, not at the natural place just above the
 *    five tallying `if`s.  This is §"An accumulator, again -- and now with a
 *    second confirmation" (docs/elevation.md:11706, "an accumulator's
 *    initialiser wants to be as early as the source will allow") and
 *    §"Counter initialisation wants to come first" (:8322).  Third instance.
 *
 *    Mechanism, and a SYMPTOM worth recognising: with `n = 0;` written late,
 *    n's live range does not overlap the actor values, so gcc reuses r5 for it
 *    and the function pushes {r5, lr} where the ROM pushes {r5, r6, lr}.  That
 *    is 8 of the 13 residual differences.  The other 3 are NOT in the counting
 *    section at all -- they are a THREE-INSTRUCTION SCHEDULE DIFFERENCE inside
 *    the FIFTH __Func_8010704 argument setup, the block immediately before n's
 *    first use, where our `asr r5,#20` sits four positions early.  Dropping r5
 *    from n's candidate set changes that block's pressure and the -O2 scheduler
 *    then places the `asr` exactly where the ROM has it.  One lever, two
 *    distant symptoms; the same mechanism as §"A PARTIAL PIN CAN BE WORSE THAN
 *    NO PIN AT ALL" (:16652), which is where the concept is already recorded.
 *
 *    Recognition rule (the new bit): a schedule-only residue confined to the
 *    LAST basic block before a counter's first use is a tell for the counter's
 *    INITIALISER POSITION, not for a scheduling flag.  Reading it as the latter
 *    sends you to -fno-schedule-insns2, which §:10797's discriminator would
 *    then correctly reject.
 *
 * 3. STACK-ARGUMENT PAIRS, ONE PAIR PER SITE.  §"The stack-arg-pair lever"
 *    (:1849), §"Each stack-argument SITE needs its own pair of locals" (:4018),
 *    §"Stack arguments must be named PER CALL SITE" (:8438).  __Func_8010704
 *    is called eight times here; the three sites whose two stack arguments are
 *    both CONSTANTS each need their own pair of named locals.  Without them gcc
 *    materialises one value, stores it, reuses the same register for the
 *    second and stores that -- `mov r3 / str / mov r3 / str` -- where the ROM
 *    holds both at once in r3 and r2 and stores both.  Measured per site:
 *
 *      all three pairs present (this file)      OK
 *      t1/t2 dropped (the 0x53 site)             3 differing
 *      u1/u2 and v1/v2 dropped (the two arms)    6 differing
 *      all three dropped                         9 differing
 *
 *    The five sites whose stack arguments are COMPUTED (`->f8 >> 20`,
 *    `->f10 >> 20`) need NOTHING -- see the deleted lever below.
 *
 * 4. THE LEVER THAT HAD TO BE DELETED.  At -O1 the five computed sites emit
 *    `asr r5` immediately after the load, where the ROM defers it across the
 *    second __MapActor_GetActor call; naming the raw field
 *    (`int x = ...->f8; ... x >> 20`) fixes that, 122 differing -> 102.  It is
 *    the obvious lever and it is INERT once the flag group is right: at
 *    CSE_CFLAGS the named-local form and the plain inline-expression form are
 *    BYTE-IDENTICAL (both 13 differing before lever 2, both OK after).  The
 *    scaffolding was built against the wrong flag group and does not ship --
 *    the file passes the two GetActor reads straight into the call.
 *
 * MEASURED-WORSE TABLE  (tryc --align against a single-function ref extract;
 * "differ" = instructions in disagreeing regions, of 250)
 *
 *   C form                                    -O1   -O2   CSE_CFLAGS
 *   ---------------------------------------------------------------
 *   inline exprs, `n = 0;` late                122    19       13
 *   named x/z locals, `n = 0;` late            102    19       13
 *   named x/z locals, `n = 0;` before          ---   ---       OK
 *     __CutsceneStart()
 *   named x/z locals, `n = 0;` after            ---   ---      OK
 *     __CutsceneStart()
 *   named x/z, `int n = 0;` declaration          48     6       OK
 *   + separate `int id = 0x984;` per site        51     6       OK
 *   inline exprs, `int n = 0;` declaration      ---   ---       OK   <- THIS FILE
 *
 *   Flag probes on the final C (tryc, -O2 base):
 *     -fno-gcse alone            250 vs 252 lines, first diff at 160, 6 differing
 *     -fno-rerun-cse-after-loop                  OK
 *   Only the rerun-CSE flag reaches it, as §:5420 and §:5565 both say.
 *
 * BLOCKED SIBLINGS -- why this is a SPLIT and not a whole-TU replacement
 *
 * The other two functions in the .s are 43-instruction straight-line call
 * scripts.  Both were written and screened (scratch_elev/b243/f200a0a4/b1.c,
 * b2.c) and both stall on recorded blocker classes:
 *
 *   OvlFunc_964_2009fdc -- EXACT for its first 23 instructions, including both
 *     six-argument __Func_8010704 calls with the shared `mov r5,#0x31` written
 *     as one shared local per §:4018's second half.  It then fails on TWO
 *     things at once, separated by perturbing one copy of the constant
 *     (§"Perturb one copy of a CSE-able constant", :5084):
 *       (a) 0xc6 << 18 is passed to __MapActor_SetPos at BOTH sites and gcc
 *           hoists it into r5 -- the straight-line repeated-constant wall,
 *           §"Straight-line call scripts: check for a repeated constant BEFORE
 *           writing C" (:4669).  --no-rerun-cse, -fno-gcse,
 *           -fno-cse-follow-jumps and -fno-expensive-optimizations all measure
 *           the identical 14 differing; -O1 is 21.
 *       (b) underneath (a), with the constant perturbed, the residue is the
 *           r0-IN-THE-MIDDLE interleave: ROM `mov r1 / mov r2 / mov r0 / lsl r1
 *           / lsl r2`, ours `mov r1 / mov r2 / lsl r1 / lsl r2 / mov r0`.  That
 *           is §:6128 exactly, and §:6128's own condition -- "Named locals
 *           assigned at the top, WITH A BRANCH BETWEEN the assignments and the
 *           call ... The branch is not optional" -- cannot be met: this
 *           function has no conditional branch at all.  It is one of the 98
 *           straight-line members :6128 sizes as out of reach.
 *
 *   OvlFunc_964_200a040 -- the same shape with `neg` instead of `lsl`
 *     (§:6128's "and it covers `neg` too"), same missing branch, plus gcc
 *     DERIVING the shared -1 from the shared 0x31 as `sub r5, #0x32`
 *     (§"The INVERSE constant problem: the ROM derives, gcc does not", :5776).
 *     13 differing of 43 at CSE_CFLAGS.
 *
 * Neither is a register-allocation park and neither is new; both are worth
 * re-attacking only if §:6128 ever gains a straight-line form.
 *
 * LANDING SHAPE
 *
 *   .s function count: 3.  Only the last one matches, so this is a SPLIT.
 *
 *   tools/split_s.py asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a.s \
 *       OvlFunc_964_200a0a4
 *   gives, the target being LAST in the file:
 *       asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a_a.s   (2009fdc + 200a040)
 *       asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a_b.s   -> replaced by this .c
 *   Neither name is taken in rom_7ed0a0 today.
 *
 *   LINKER LINES NAMING THE .o, matched on FULL PATH and cited by content.
 *   `grep -rn "asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a\.o" overlays/` gives
 *   exactly ONE line in a linker script:
 *
 *       overlays/rom_7ed0a0/overlay.ld:78
 *               asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a.o(.text)
 *
 *   and nothing else -- no .data, no .rodata, no .bss line anywhere in any
 *   .ld.  The only other mentions of the full path are in the generated
 *   overlay.map (lines 252, 253, 255, 498, 499, 693), which is build output.
 *   The .s carries no .data section (map:499 gives .text 0x324 at 0x2009fdc,
 *   and 0x324 is exactly the three functions), so per §"Splitting a single-
 *   function `.s`: remap EVERY section" (:5516) there is one section to remap
 *   and one line to become two, in order:
 *
 *       asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a_a.o(.text)
 *       asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a_b.o(.text)
 *
 *   LET split_s.py REWRITE THE SCRIPT rather than hand-aiming the line.
 *
 *   THE MAKEFILE EDIT IS PART OF THE LANDING, NOT AN AFTERTHOUGHT.  The split
 *   product `ovl_30_c_c_c_a_a_a_b.o` STILL matches the -O1 wildcard at
 *   Makefile:853 (`%` = `_a_b`), which is exactly the trap
 *   docs/elevation.md:15725 names: "When a split product's name shares a prefix
 *   with a flag wildcard, check the wildcard BEFORE screening."  Add an
 *   explicit rule:
 *
 *       asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a_b.o: \
 *               src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a_b.c
 *               $(GCC296_CC) $(CSE_CFLAGS) -S -o $(@:.o=.s) $<
 *               printf '\n\t.text\n\t.align\t2, 0\n' >> $(@:.o=.s)
 *               arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork -Iinclude \
 *                   -o $@ $(@:.o=.s)
 *
 *   `ovl_30_c_c_c_a_a_a_a.o` has no .c, so the wildcard's pattern rule does not
 *   fire for it and it falls to the asm/%.o rule unchanged.
 *
 *   `.L33ec` and `.L340c` need NO export step: both are already `.global` in
 *   asm/overlays/rom_7ed0a0/ovl_30_c_c_c_c_c_b.s:1093-1094, and they are
 *   reached here with the asm-label extension per §"Technique: reaching a
 *   file-local `.L` symbol from C" (:910).  objcmp confirms both R_ARM_ABS32
 *   relocations by name.
 *
 * SCREENING NOTE.  objcmp derives its flags from the reference path via
 * tryc.makefile_flags, so it cannot be told to use CSE_CFLAGS while the
 * Makefile still says -O1.  The OK line above was produced by pointing
 * GCC296_DIR at scratch_elev/b243/f200a0a4/gcccse/, a directory holding
 * symlinks to /opt/gcc296/{cc1,cpp,tradcpp} and a two-line xgcc that execs the
 * real one with `-O2 -fno-rerun-cse-after-loop` appended (last -O wins), i.e.
 * exactly $(CSE_CFLAGS).  gcco2/ beside it does the same for plain -O2.  Once
 * the Makefile rule above exists, plain
 *     objcmp <this> asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a.s \
 *         --func OvlFunc_964_200a0a4
 * reproduces the OK line with no wrapper.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __PlaySound(int id);
extern void __Func_8010560(void *a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern unsigned char L33ec[] __asm__(".L33ec");
extern unsigned char L340c[] __asm__(".L340c");

void OvlFunc_964_200a0a4(void)
{
    int n = 0;

    __CutsceneStart();
    {
        int t1 = 0x13;
        int t2 = 0x2d;
        __Func_8010704(0x53, 0x2d, 0xb, 8, t1, t2);
    }
    __Func_8010704(0x14, 0x38, 1, 1, __MapActor_GetActor(0x13)->f8 >> 20,
                   __MapActor_GetActor(0x13)->f10 >> 20);
    __Func_8010704(0x14, 0x38, 1, 1, __MapActor_GetActor(0x14)->f8 >> 20,
                   __MapActor_GetActor(0x14)->f10 >> 20);
    __Func_8010704(0x14, 0x38, 1, 1, __MapActor_GetActor(0x15)->f8 >> 20,
                   __MapActor_GetActor(0x15)->f10 >> 20);
    __Func_8010704(0x14, 0x38, 1, 1, __MapActor_GetActor(0x16)->f8 >> 20,
                   __MapActor_GetActor(0x16)->f10 >> 20);
    __Func_8010704(0x14, 0x38, 1, 1, __MapActor_GetActor(0x17)->f8 >> 20,
                   __MapActor_GetActor(0x17)->f10 >> 20);
    if (__MapActor_GetActor(0x13)->f8 >> 20 == 0x19
        && __MapActor_GetActor(0x13)->f10 >> 20 == 0x31)
        n++;
    if (__MapActor_GetActor(0x14)->f8 >> 20 == 0x17
        && __MapActor_GetActor(0x14)->f10 >> 20 == 0x31)
        n++;
    if (__MapActor_GetActor(0x15)->f8 >> 20 == 0x19
        && __MapActor_GetActor(0x15)->f10 >> 20 == 0x2f)
        n++;
    if (__MapActor_GetActor(0x16)->f8 >> 20 == 0x17
        && __MapActor_GetActor(0x16)->f10 >> 20 == 0x2f)
        n++;
    if (__MapActor_GetActor(0x17)->f8 >> 20 == 0x18
        && __MapActor_GetActor(0x17)->f10 >> 20 == 0x30)
        n++;
    if (n == 5) {
        if (__GetFlag(0x984) != 0) {
            __CutsceneEnd();
            return;
        }
        __CutsceneWait(0x14);
        __Func_80933d4(0xcccc, 0x1999);
        __Func_80933f8(0xec << 17, -1, 0xc3 << 18, 1);
        __Func_8093530();
        __CutsceneWait(0x1e);
        __SetFlag(0x984);
        __PlaySound(0x9e);
        __Func_8010560(L33ec, 0x20, 0x2e);
        {
            int u1 = 0x20;
            int u2 = 0x2f;
            __Func_8010704(0x18, 0x3c, 1, 1, u1, u2);
        }
        __CutsceneWait(0x28);
    } else if (__GetFlag(0x984) != 0) {
        __CutsceneWait(0x14);
        __Func_80933d4(0xcccc, 0x1999);
        __Func_80933f8(0xec << 17, -1, 0xc3 << 18, 1);
        __Func_8093530();
        __CutsceneWait(0x1e);
        __ClearFlag(0x984);
        __PlaySound(0x9f);
        __Func_8010560(L340c, 0x20, 0x2e);
        {
            int v1 = 0x20;
            int v2 = 0x2f;
            __Func_8010704(0x1f, 0x2f, 1, 1, v1, v2);
        }
        __CutsceneWait(0x28);
    }
    __CutsceneEnd();
}

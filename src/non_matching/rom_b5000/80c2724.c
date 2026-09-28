/* Func_80c2724 -- 0x080c2724, asm/rom_b5000/rom_c1a34_a_c_c.s
 * NON-MATCHING, 215 encodings of 303.  NOT a distance (ref 740 bytes / 303 encodings against ours 736 / 301).  READ `--align`: 49 of 307.
 *
 * CARRIES ONE VERIFICATION SHIM, legitimate in a park and NOT to be landed:
 * `__asm__(".equ _MSG_8a0, 0x08a0")`, which the draft added to measure a symbol proposal.
 * The proposal itself is UNDECIDED pending its agent's final report -- do not treat the shim's
 * presence as the row having been accepted.  Note 0x8a0's namespace is not decided by value:
 * _FILE_5b and _AREA_5b already exist at a different value, and the bar is that the symbol
 * COMPLETES the function with in-function control.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_b5000/80c2724.c \
 *     asm/rom_b5000/rom_c1a34_a_c_c.s --func Func_80c2724
 *
 * NOT MATCHING.  tryc --align: 49 of 307.  objcmp --func Func_80c2724, verbatim:
 *     XX SIZE  ref 740 bytes, ours 736
 *     XX ENCODINGS differ in 215 place(s) (ref 303, ours 301)
 *        first at index 7: ref 4ba5  ours 4ba4
 * So 215 is NOT yet a true distance -- two instructions and four bytes short -- but ALL
 * FIFTY-THREE relocations are present with the SAME symbols in the SAME ORDER (the call
 * offsets drift 2-4 bytes and re-converge, and the three pool words iwram_3001e74 /
 * Func_8001af8 / ewram_200047c are in the ROM's order).  Both missing instructions lie
 * inside the single blocker below.
 *
 * TWO FUNCTIONS IN THE FILE, confirmed by `grep -ci func_start`: Func_80c24f0 (lines
 * 8-297, parked at src/non_matching/rom_b5000/80c24f0.c) and Func_80c2724 (lines 304-616).
 * Func_80c2724 is the SECOND and LAST, so the split is a TAIL split.  No `.data` in the
 * file; the only label that crosses is the `.Lc29c4` pool word (`.word 0`) which belongs to
 * Func_80c2724 itself and is emitted by gcc, and nothing crosses the other way.  NO EXPORT
 * IS NEEDED.
 *
 * TAKES NO ARGUMENTS and returns void: r0 is dead from the first instruction.  The file
 * comment's "r0.. = parameters" is wrong.
 *
 * FAMILY.  `struct Blk` and `iwram_3001e74[0] + 0x530` are lifted verbatim from the landed
 * sibling src/rom_b5000/rom_c1a34_a_c_b.c (Func_80c24b0): `int a[3]` at +0 and
 * `unsigned short b[4]` at +0xc, which is exactly what this function reads.
 *
 * SIX LEVERS LANDED, 120 differing -> 49, each measured as a single drop:
 *
 * 1. `int w` FOR THE ABILITY HALFWORD, 120 -> 118, AND IT MUST BE `unsigned int`.
 *    `unsigned short w` emits `ldrsh` plus `lsl #16 / lsr #16` -- ARM's PROMOTE_MODE
 *    (arm.h:597) gives HImode `UNSIGNEDP = TARGET_MMU_TRAPS != 0` and overrides the
 *    declared signedness, the batch-293 rule.  A signed `int` then costs `asr r3,r1,#0xf`
 *    where the ROM has `lsr r3,r5,#0xf`, so the carrier has to be UNSIGNED int: the
 *    HImode-carrier rule has a SIGNEDNESS as well as a width.
 *
 * 2. DECLARATION ORDER SETS THE WHOLE FRAME, and for this function that is two separate
 *    orderings.  Addressable ARRAYS are allocated at expand time and take the HIGH end of
 *    the frame in declaration order, so `short res[8]` must be declared BEFORE
 *    `unsigned short list[8]` to get the ROM's res@0x28 / list@0x18.  SPILLED SCALARS are
 *    allocated later, below the arrays, in increasing pseudo number -- i.e. declaration
 *    order again -- so the ROM's blk@0x14, n@0x10, lp@0xc, i@0x8, id@0x4 requires exactly
 *    that declaration order, with `save` (which wants a register) declared before `blk` so
 *    its absence from the spill set does not shift anything.
 *
 * 3. THE BYTE OFFSET INTO `list` IS A GIV, NOT A VARIABLE.  The ROM initialises sp+0x0 to
 *    zero in the loop PREHEADER (`.Lc277c`), after the `0 < n` guard, and bumps it by 2 at
 *    the bottom -- that is loop.c's strength reduction of `lp[i]`, not a programmer's
 *    `ofs`.  Writing `ofs = 0;` before the loop and `ofs += 2;` at the end puts the
 *    initialisation BEFORE the guard and costs a slot; writing `lp[i]` and deleting the
 *    variable moved the store into the preheader and was 118 -> 115.  It also explains the
 *    register-offset operand ORDER: the giv comes first, `ldrh r3,[r3,r0]`.
 *
 * 4. THE SECOND LOOP NEEDS ITS OWN COUNTER, 111 -> 95.  Reusing `i` for the item scan makes
 *    one allocno whose live range spans the whole function; the ROM keeps the item counter
 *    in r6 while the first loop's `i` is spilled at sp+0x8, so they are two variables.  This
 *    is the largest single drop in the function and it is pure reference accounting:
 *    `allocno_compare` (global.c) is
 *    `floor_log2(n_refs) * n_refs / live_length`, so merging two counters both lengthens the
 *    range and pools the references.
 *
 * 5. `&ewram_200047c` IS A NAMED POINTER ASSIGNED BEFORE THE LOOP, 84 -> 49, AND IT FIXED
 *    THE LENGTH.  The ROM's `.Lc2944: ldr r0,=ewram_200047c / mov r10,r0` sits between the
 *    coin block and the loop top `.Lc2948` -- a source statement in the preheader, not
 *    loop-invariant motion, because gcc does not hoist a single-use constant load there
 *    (measured: with the global written inline, those two instructions are simply absent and
 *    r10 stays free).  Declaring `unsigned short *ep;` and assigning it before the `for (;;)`
 *    produces them, and the extra callee-saved consumer is what finally made the instruction
 *    count exact.
 *
 * 6. THE SECOND `w & mask` MUST BE A LITERAL SO CSE CANNOT SHARE THE FIRST, 95 -> 84.  The
 *    ROM computes the mask twice: `mov r3,r5 / and r3,r0` for the `!= 0` test at the top of
 *    the ability loop, and `mov r3,r9 / and r5,r3` destructively before the `_Func_8019908`
 *    call.  Written with the same `mask` variable at both sites, cse shares the first result
 *    and BOTH instructions disappear (measured: two instructions short and 95 differing).
 *    Four spellings were measured -- same variable twice (95), two variables both 0x3fff
 *    (83, and it reproduces the pair but perturbs the allocation), literal first and
 *    variable second (52), variable first and LITERAL SECOND (84 alone, 49 in combination).
 *    The last is the one that reproduces `mov r3,r9 / and r5,r3` EXACTLY, because the
 *    constant is cse'd into the same hoisted pseudo r9 while the two AND expressions stay
 *    distinct.  The reason cse cannot share them in the ROM is that the second sits in
 *    `.Lc2814`, a block with TWO predecessors, so it starts a new extended basic block.
 *
 * ALSO ESTABLISHED, each with its measurement:
 *  - THE TABLE SEARCH IS THE EXPLICIT-POINTER `while` WITH AN INNER `break`, not the array
 *    form.  `while (w != save->ab[j].id) { j++; if (j > 0x1f) break; }` measures 125 against
 *    115.  This CONFIRMS lever 1 of the file-mate park (src/non_matching/rom_b5000/80c24f0.c)
 *    in the same file, for a different loop.
 *  - `.Lc28ee` IS THE ROLLED FIRST EXIT TEST OF A `for (;;)` WITH A `break`, batch 293's
 *    `stmt.c:2257` lever: the preheader jumps forward to the copy-and-test block, the body
 *    is laid out first, and the test branches back.  Written this way it was right on the
 *    first candidate.
 *  - Func_8001af8 runs from IWRAM in ARM state (asm/rom_c0/rom_770.s:1085, @ 0x03001388), so
 *    it is reached through a function-pointer local and `bl _call_via_r3`.  The tree's
 *    spelling (`copy = Func_8001af8; copy(...)`, src/rom_c9000/rom_d82b0_b.c:168) with the
 *    ASSIGNMENT INSIDE the loop reproduces the ROM's in-loop `ldr r3,=Func_8001af8`.  This is
 *    a data point for batch 293's open `.call_via` question: gcc's `bl _call_via_r3` is what
 *    the ROM has here, so no bespoke helper and no pin was needed.
 *  - `ldr r3, .Lc29c4  @ 0` is a HImode literal store going through the pool: the bare `0`
 *    is correct and an int carrier would be wrong.
 *  - The six `res[2..7]` blocks are six separate `if`s, not a loop; every `short` read needs
 *    the `mov rN,#off / ldrsh` pair because Thumb-1 has no immediate-offset LDRSH.
 *
 * A SYMBOL TELL, MEASURED, THAT DOES NOT COMPLETE THE FUNCTION -- 50 -> 49.
 * `0x8a0` is the ONLY one of the function's eleven message ids that gcc can build with one
 * `mov` plus one shift (`0x45 << 5`), and the ROM POOLS IT (`ldr r0, =0x8a0`) while emitting
 * `mov r0,#0x8a / lsl r0,#4` is what a plain literal gives.  gcc never pools a constant a
 * `mov` can build, so the operand is a SYMBOL_REF.  The other ten (0x83a, 0x83b, 0x83c,
 * 0x89a, 0x89b, 0x89c, 0x89d, 0x89e, 0x89f, 0x8a1) are each unshiftable and pool either way,
 * so they are consistent with literals AND with symbols -- which means the in-function
 * control here is ONE-SIDED, weaker than batch 293's `_MSG_cae` run where four neighbours
 * provably had to be literals.  PROPOSAL WITHHELD on the batch's own rule that a `.sym` entry
 * must COMPLETE its function; the measurement is recorded so the owner can decide, and the
 * natural reading is that the whole run 0x83a..0x8a1 are `_MSG_` ids of which only 0x8a0's
 * symbol-hood is observable.  Note that with the `.equ` in this TU the assembler resolves the
 * pool word to the absolute 0x8a0 and objcmp's relocation list matches the ROM exactly; a
 * real `message.sym` landing would instead carry one `R_ARM_ABS32 _MSG_8a0`, which is the
 * signature batch 293 documented.
 *
 * BLOCKER: `blk` AND `save` HAVE SWAPPED HOMES, and that is the entire residue.  The ROM
 * spills `blk` to sp+0x14 and reloads it thirteen times while `save` lives in r11; this
 * candidate keeps `blk` in r9 and spills `save` to sp+0x14.  Everything else -- frame size,
 * every slot, block order, every loop shape, all fifty-three relocations -- already agrees,
 * and both missing instructions are in this cluster (the ROM's `mov r2,r11 / ... /
 * add r2,#0x58` pair against this candidate's single reload-and-add).  The knock-on is that
 * `mask` takes r11 here and r9 in the ROM.
 *
 * The pass is global.c's `allocno_compare`, quoted from
 * /opt/camelot-gcc/gcc-2.96/gcc/global.c:
 *     pri = ((double) (floor_log2 (n_refs) * n_refs) / live_length) * 10000 * size
 * with `n_refs` loop-depth weighted by `flow.c:4948`.
 *
 * READ FROM THE DUMPS RATHER THAN COUNTED, which is the point batch 293 made about n_refs.
 * With `-dg`, `save` is pseudo 32 and `blk` is pseudo 33 (declaration order; ARM's first
 * pseudo is 32 because FIRST_PSEUDO_REGISTER plus the virtual registers take 27-31), and the
 * greg dump's priority list reads
 *     ;; 34 regs to allocate: 91 40 75 97 106 115 124 133 86 142 44 47 42 39 166 188 50 54
 *                             149 46 43 183 45 38 163 33 48 37 32 35 194 36 34 49
 * -- `33` at position 26 and `32` at position 29, so blk outranks save by four places, and
 * the dispositions line confirms `33 in 9` with 32, 34, 35, 36, 37 and 194 absent, i.e.
 * spilled.  Exactly six spills, exactly the ROM's six slots, with the wrong member.
 *
 * AND THE TWO COST MODELS DISAGREE, WHICH IS WORTH RECORDING ON ITS OWN.  The `-dl` lreg
 * dump gives `Register 32 costs: ... MEM:3088` against `Register 33 costs: ... MEM:842`, so
 * regclass thinks spilling `save` is 3.7x more expensive than spilling `blk` -- and
 * global_alloc spills `save` anyway, because `allocno_compare` looks at `n_refs / live_length`
 * and never at those costs.  regclass weights a reference by basic-block FREQUENCY (which
 * grows geometrically with loop depth) while `n_refs` weights it by `loop_depth + 1` (which
 * grows linearly), so a variable with three deep references loses to one with a dozen shallow
 * ones.  That is the mechanism here: blk's ~17 shallow weighted references beat save's ~9
 * deep ones even though save's are in a loop nested three deep.
 *
 * ONE ALTERNATIVE MECHANISM TO RULE OUT FIRST, because it would make the whole
 * reference-counting route the wrong tree: the greg dump also carries `Spilling for insn
 * 714. / Spilling for insn 715. / Spilling for insn 730.` lines, so RELOAD is spilling here
 * too, and a register global_alloc handed out can still be taken away by `spill_hard_reg`.
 * If the ROM's `blk` won a register from global_alloc and lost it in reload, the lever is
 * instruction-level pressure at those three insns rather than `n_refs` at all.  Check which
 * pseudo those three spills are for before spending another pass on the priority formula.
 *
 * What this means for the next attempt: the ROM's blk reload pattern is ALREADY reproduced
 * (thirteen reloads, each forced by an intervening call), so blk's `n_refs` cannot be cut
 * without losing an instruction -- the lever has to RAISE save's.  The likeliest place is the
 * search-loop base, where the ROM's single `mov r2,r11` serves both `save->ab[0].id` and
 * `bp = save->ab`; a spelling that stops regmove sharing that copy would add a weighted
 * reference at loop depth 3, worth +4, and by the formula save needs about +7.
 *
 * MEASURED AND INERT on the swap (all still 49): declaring `save` first of all locals;
 * an `int base` temp for the `iwram_3001e74[0] + 0x530` computation; an `int sv = (int)save`
 * base for the first search compare with `bp` derived from `save` separately; `save` as
 * `unsigned char *` with explicit `+ 0x58` casts; hoisting `bp = save->ab` above the first
 * compare.  A diagnostic `register struct Unit *save __asm__("r11")` measures 50, i.e. the
 * pin does NOT take -- so this is NOT the "right instructions, wrong registers" class a
 * barrier fixes.  Retry it TOGETHER with a change that adds a deep `save` reference, not on
 * its own.
 *
 * SHIMS: ONE, and it must never land.
 *   `register ... __asm__` declarations: ZERO.
 *   `__asm__(".equ ...")` lines: ONE -- `.equ _MSG_8a0, 0x08a0`, added so the symbol
 *   proposal above could be measured standalone.  Strip it and restore the literal `0x8a0`
 *   (that variant is scratch_elev/b294/D/t1_k1_nosym.c, 50 differing) if the symbol is not
 *   admitted.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_b5000/80c2724.c \
 *     asm/rom_b5000/rom_c1a34_a_c_c.s --func Func_80c2724
 */
struct Blk {
    int a[3];
    unsigned short b[4];
};

struct Ability {
    unsigned short id;
    unsigned short x;
};

struct Unit {
    unsigned char pad_00[0xf];
    unsigned char f0f;
    unsigned char pad_10[0x48];
    struct Ability ab[32];
    unsigned char pad_d8[0x4c];
    int f124;
    unsigned char pad_128;
    unsigned char f129;
};

__asm__(".equ _MSG_8a0, 0x08a0");  /* SHIM (.equ class): measuring the symbol proposal */
extern int _MSG_8a0;
extern char *iwram_3001e74[];
extern unsigned short ewram_200047c;
extern void _Func_8019908(int a, int b);
extern void _Func_80175a0(int id);
extern void WaitTextPrompt(void);
extern int Func_80b6b40(int kind, unsigned short *buf);
extern void *Func_8004970(int size);
extern struct Unit *_GetUnit(int id);
extern void _Func_80198dc(void);
extern void _PlaySound(int id);
extern int _Func_80792c4(int id, short *res);
extern void Func_8001af8(volatile unsigned short *dst, void *src, int len);
extern void free(void *p);
extern void _AddCoins(int n);
extern int Func_80c2470(int v);
extern int _GiveItem(int item);

void Func_80c2724(void)
{
    short res[8];
    unsigned short list[8];
    struct Unit *save;
    struct Blk *blk;
    int n;
    unsigned short *lp;
    int i;
    int id;
    struct Unit *unit;
    struct Ability *ap;
    struct Ability *bp;
    void (*copy)(volatile unsigned short *dst, void *src, int len);
    int j;
    int k;
    int v;
    int best;
    int bestIdx;
    unsigned int w;
    int mask;
    unsigned short *ep;
    int m;

    blk = (struct Blk *)(iwram_3001e74[0] + 0x530);
    if (blk->a[1] != 0) {
        _Func_8019908(blk->a[1], 5);
        _Func_80175a0(0x83a);
        WaitTextPrompt();
    }
    lp = list;
    n = Func_80b6b40(1, lp);
    save = Func_8004970(0xa6 * 2);
    for (i = 0; i < n; i++) {
        id = lp[i];
        unit = _GetUnit(id);
        unit->f124 += blk->a[1];
        for (;;) {
            copy = Func_8001af8;
            copy((volatile unsigned short *)save, unit, 0xa6 * 2);
            if (_Func_80792c4(id, res) == 0)
                break;
            _PlaySound(0x59);
            _Func_80198dc();
            _Func_8019908(unit->f129, 3);
            _Func_8019908(lp[i], 1);
            _Func_8019908(unit->f0f, 5);
            _Func_80175a0(0x89a);
            WaitTextPrompt();
            mask = 0x3fff;
            ap = unit->ab;
            k = 0x1f;
            do {
                w = ap->id;
                ap++;
                if ((w & mask) != 0 && (w >> 15) != 0) {
                    j = 0;
                    if (w != save->ab[0].id) {
                        bp = save->ab;
                        do {
                            j++;
                            if (j > 0x1f)
                                break;
                            bp++;
                        } while (w != bp->id);
                    }
                    if (j == 0x20) {
                        _Func_80198dc();
                        _Func_8019908(unit->f129, 3);
                        _Func_8019908(id, 1);
                        w &= 0x3fff;
                        _Func_8019908(w, 4);
                        _PlaySound(0x9a);
                        _Func_80175a0(0x89b);
                        WaitTextPrompt();
                    }
                }
                k--;
            } while (k >= 0);
            if (res[2] != 0) {
                _Func_8019908(res[2], 5);
                _Func_80175a0(0x89c);
                WaitTextPrompt();
            }
            if (res[3] != 0) {
                _Func_8019908(res[3], 5);
                _Func_80175a0(0x89d);
                WaitTextPrompt();
            }
            if (res[4] != 0) {
                _Func_8019908(res[4], 5);
                _Func_80175a0(0x89e);
                WaitTextPrompt();
            }
            if (res[5] != 0) {
                _Func_8019908(res[5], 5);
                _Func_80175a0(0x89f);
                WaitTextPrompt();
            }
            if (res[6] != 0) {
                _Func_8019908(res[6], 5);
                _Func_80175a0((int)&_MSG_8a0);
                WaitTextPrompt();
            }
            if (res[7] != 0) {
                _Func_8019908(res[7], 5);
                _Func_80175a0(0x8a1);
                WaitTextPrompt();
            }
        }
    }
    free(save);
    if (blk->a[0] != 0) {
        _Func_8019908(blk->a[0], 5);
        _Func_80175a0(0x83b);
        _AddCoins(blk->a[0]);
        WaitTextPrompt();
    }
    ep = &ewram_200047c;
    for (;;) {
        bestIdx = -1;
        best = -1;
        for (m = 0; m <= 3; m++) {
            if (blk->b[m] != 0) {
                v = Func_80c2470(blk->b[m]);
                if (v >= best) {
                    best = v;
                    bestIdx = m;
                }
            }
        }
        if (bestIdx == -1)
            break;
        _Func_8019908(blk->b[bestIdx], 2);
        _Func_80175a0(0x83c);
        WaitTextPrompt();
        if (_GiveItem(blk->b[bestIdx]) == -1) {
            *ep = blk->b[bestIdx];
            break;
        }
        blk->b[bestIdx] = 0;
    }
}

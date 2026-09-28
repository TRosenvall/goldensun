/* Func_8095c08 -- 0x08095c08.  PARKED at 102 instructions in disagreeing
 * NON-MATCHING, 199 encodings of 213.  NOT a distance (ref 456 bytes / 213 encodings against ours 472 / 221).  `--align`: 102 of 217.
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M` in the header.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/8095c08.c \
 *     asm/rom_8a000/rom_944ec_a_c_a_c_a_a.s --func Func_8095c08
 * regions of 217 (tryc --align).  NOT a true distance: 221 encodings against
 * 213 and 472 bytes against 456, so objcmp's positional 199 of 213 means
 * nothing -- use the --align figure.
 * ref: asm/rom_8a000/rom_944ec_a_c_a_c_a_a.s  (ONE function, no data section;
 *      grep -ci func_start = 1; would convert the WHOLE FILE)
 * batch 293, brief A, target 2.  No shims in the draft.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_8a000/8095c08.c \
 *     --ref asm/rom_8a000/rom_944ec_a_c_a_c_a_a.s --align
 *
 * WHAT IS RIGHT.  The control flow, all six dispatch arms, every offset, both
 * vec3 blocks and all the arithmetic are structurally identical to the ROM,
 * and the whole residue is ONE REGISTER SWAP plus its knock-on copies.
 *
 * Levers that landed on the way down (each confirmed by a single drop):
 *  - `o = 0xfa << 1;` as a named local so gState+0x1f4 is built as
 *    `ldr r3,=gState / mov r2,#0xfa / lsl r2,#1 / add r3,r2` instead of one
 *    pooled `gState+500`.  Same lever as Func_808d5dc, same bank.
 *  - `int m = ~0xc;` as a named local at each of the three mask sites.  Written
 *    inline, `q[9] &= ~0xc` narrows the mask to QImode and gcc emits the single
 *    `mov r3,#0xf3`; the ROM has `mov r3,#0xd / neg r3,r3`, i.e. -13 in SImode.
 *    A named `int` keeps it SImode.  Five other spellings measured (a plain
 *    `~0xc`, `0xfffffff3`, `-13`, an int temp for the loaded byte, an
 *    `unsigned char` mask local) all give the narrowed form.
 *  - `k = *st; *st = k - 1;` for the decrement.  `(*st)--` and `*st = *st - 1`
 *    both give `add r3,#0xff`; the int temp gives the ROM's `sub r3,#1`.
 *  - `int k = 0x28;` for `*(short *)(a+0x3a) = 0x28`.  Written as a literal it
 *    becomes a HImode pool entry (`ldrh r3,.Lpool`) where the ROM has
 *    `mov r3,#0x28`.  Same mode-of-the-constant mechanism as Func_808d5dc's
 *    0xffff, in the other direction: there the fix put the constant INTO the
 *    pool as SImode, here it keeps it out of the pool entirely.
 *  - the bit-copy at +9 split into `p2`/`t` temps so the source mask (`& 0xc`)
 *    is computed before the destination mask, which is the ROM's order.
 *
 * THE BLOCKER, named with its pass and proved from gcc's own dumps.
 * The ROM keeps the dispatch byte `s` in r6 and `&v` (case 0) in r8:
 *     mov r6,#0 / ldrsb r6,[r2,r6]   ...   cmp r6,#0 ... cmp r6,#5
 *     mov r8,sp / str r2,[sp,#0] ... mov r2,r8 / mov r1,r8 / ldr r3,[r1,#0]
 * We produce the two the other way round -- `&v` in r6, `s` in r8 -- and pay
 * `mov r8,r2` once plus `mov rX,r8` before each of the five later compares and
 * each of the three zero stores.  That is the entire 102, and the ROM's own
 * `mov r1,r8` (needed because r8 cannot be a `ldr` base) is the mirror cost it
 * pays for the other assignment.
 *
 * This is NOT global.c's allocno_compare.  `-dl` (the .lreg dump, i.e. AFTER
 * local-alloc and BEFORE global-alloc) already says
 *       ;; Register 51 in 6.
 * where pseudo 51 is `&v` in the case-0 body.  local-alloc.c's find_free_reg
 * walks REG_ALLOC_ORDER {3,2,1,0,12,14,4,5,6,7,8,10,...}; the quantity crosses
 * three calls so every call-used register is excluded, the case-0 Random result
 * already holds r5, and r6 is the next entry.  local_alloc runs BEFORE
 * global_alloc and knows nothing about global allocnos, so by the time
 * global.c ranks `s` (pseudo 36, ninth of twelve in "12 regs to allocate:
 * 35 32 40 41 79 99 115 126 36 34 117 33") r6 is gone and the first free
 * call-saved register is r8.  The rest of the greg dump then follows the same
 * order exactly: 34 (`st`) -> r10, 33 (`e`) -> r9, which is what the ROM has.
 *
 * `s` cannot share r6 with pseudo 51 either, because cse substitutes `s` for
 * the three literal zeros in the case-0 body -- the .lreg dump has
 * `(subreg:QI (reg/v:SI 36))` as the source of the +0x42 and +0x47 `strb` and
 * `(subreg:HI (reg/v:SI 36))` as the source of the +0x38 `strh`, from
 * record_jump_equiv learning s == 0 off the `cmp r6,#0 / bne`.  So `s` is live
 * straight through the block that pseudo 51 owns and the two genuinely conflict.
 *
 * MEASURED AND DID NOT MOVE IT (all 102-104 of 217):
 *  - naming the case-0 vec3 address in an `int *pv` and using it for the call
 *    argument only, for the reads only, or for both;
 *  - writing the three zero stores as `s` explicitly, or as a named `int z = 0`
 *    set at the top of the case-0 body (cse folds z back onto `s` either way);
 *  - `signed char s` instead of `int s`;
 *  - declaring `int v[3]` inside each arm's own block instead of at function
 *    scope (104);
 *  - dropping the `t0`/`t1` temps for the two `a+0x14`/`a+0x18` loads (103).
 * WORSE: hoisting `pv = v;` above the dispatch so the address becomes a global
 *    allocno -- 141 either way, whether case 3 shares `pv` or keeps the array.
 * DIAGNOSTIC: -fno-schedule-insns2 is 125 (the pass is doing the right work);
 *    -fno-rerun-cse-after-loop is 102, i.e. the cse that substitutes `s` for the
 *    zeros is cse1/cse2 proper, not the post-loop rerun, so the flag cannot
 *    reach it.
 *
 * WHY THIS MATTERS BEYOND THIS FUNCTION.  HANDOFF.md's standing hypothesis for
 * the register-allocation class is that the original toolchain had a different
 * REG_ALLOC_ORDER, and its proposed test is to start the order at 4.  That
 * change cannot fix THIS function: the order would still reach r5 and r6 before
 * r8, local-alloc would still claim r6 for pseudo 51, and `s` would still be
 * exiled.  Whatever produced the ROM either did not run local-alloc first on
 * this shape or had a reason to exclude r6 that gcc-2.96 does not.  That is
 * evidence the REG_ALLOC_ORDER hypothesis is not the whole class.
 *
 * NEXT THING TO TRY.  Everything here points at one question: what makes
 * pseudo 51 NOT be a block-local quantity, or makes find_free_reg see r6 as
 * used over its range, WITHOUT lengthening its live range (which is what made
 * the `pv` variants 141).  A second block-local call-crossing value inside the
 * case-0 body would do it by taking r6 first, and the ROM's case 0 has no
 * visible candidate -- so the answer is more likely a different shape for the
 * two pre-call `v` stores, which the ROM does through `sp` (`str r2,[sp,#0]`)
 * while we do them through the pointer (`str r2,[r6,#0]`).  Find the spelling
 * that keeps those two stores sp-relative and pseudo 51 drops from six
 * references to four.
 */
extern unsigned char gState[];
extern unsigned char *MapActor_GetActor(int slot);
extern unsigned int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern int Func_809ba34(unsigned char *e);
extern void Func_809bb34(unsigned char *e);
extern void Func_80974d8(int *v);
extern void _PlaySound(int id);
extern int iwram_3001800;

void Func_8095c08(unsigned char *a)
{
    int v[3];
    unsigned char *e;
    unsigned char *st;
    unsigned char *q;
    int s;
    int o;
    int t0;
    int t1;
    int m;
    int k;
    int t;
    unsigned char *p2;

    o = 0xfa << 1;
    e = MapActor_GetActor(*(int *)(gState + o));
    st = a + 0x40;
    s = *(signed char *)st;
    if (s == 0) {
        *(int *)(a + 4) = *(int *)(a + 0x14);
        *(int *)(a + 8) = *(int *)(a + 0x18);
        v[0] = *(int *)(a + 0x14);
        v[2] = *(int *)(a + 0x18);
        vec3_translate(0xf0 << 15,
                       ((Random() * 3) << 11 >> 16) - ((Random() * 3) << 11 >> 16) + (0xc0 << 8),
                       v);
        *(int *)(a + 0xc) = v[0];
        *(int *)(a + 0x10) = v[2];
        *(int *)(a + 0x24) = 0xa0 << 11;
        *(int *)(a + 0x20) = 0xa0 << 11;
        *(unsigned char *)(a + 0x42) = 0;
        (*st)++;
        p2 = *(unsigned char **)(e + 0x50);
        q = *(unsigned char **)a;
        t = p2[9] & 0xc;
        m = ~0xc;
        q[9] = (q[9] & m) | t;
        *(unsigned char *)(a + 0x47) = 0;
        *(short *)(a + 0x38) = 0;
        if (iwram_3001800 & 1)
            _PlaySound(0x86);
    } else if (s == 1) {
        if (*(short *)(a + 0x38) == 3) {
            m = ~0xc;
            q = *(unsigned char **)a;
            q[9] &= m;
            *(unsigned char *)(a + 0x47) = 4;
        }
        goto L4;
    } else if (s == 2) {
        if (Func_809ba34(a) == 0) {
            *(int *)(a + 0x14) = *(int *)(a + 4);
            *(int *)(a + 0x18) = *(int *)(a + 8);
            m = ~0xc;
            q = *(unsigned char **)a;
            q[9] &= m;
            *(unsigned char *)(a + 0x47) = 4;
            *(unsigned char *)(a + 0x44) = 0;
            (*st)++;
            k = 0x28;
            *(short *)(a + 0x3a) = k;
        }
    } else if (s == 3) {
        *(unsigned char *)(a + 0x44) = 1;
        *(int *)(a + 4) = *(int *)(a + 0x14);
        *(int *)(a + 8) = *(int *)(a + 0x18);
        v[0] = *(int *)(e + 8);
        v[1] = *(int *)(e + 0xc) + (0xa0 << 13);
        v[2] = *(int *)(e + 0x10);
        Func_80974d8(v);
        vec3_translate(0x80 << 11, Random(), v);
        *(int *)(a + 0xc) = v[0];
        *(int *)(a + 0x10) = v[2];
        (*st)++;
        if (iwram_3001800 & 1)
            _PlaySound(0x91);
    } else if (s == 4) {
L4:
        if (Func_809ba34(a) == 0) {
            k = *st;
            *st = k - 1;
        }
    } else if (s == 5) {
        if (Func_809ba34(a) == 0)
            Func_809bb34(a);
    }
}

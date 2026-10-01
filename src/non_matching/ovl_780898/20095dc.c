/* OvlFunc_883_20095dc -- NON-MATCHING, 1885 of 2061
 *
 * Source asm: asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a_a.s
 * Batch 312, brief B.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_780898/20095dc.c \
 *     asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a_a.s --func OvlFunc_883_20095dc
 *
 * THE CLAIM LINE IS NOT A TRUE DISTANCE. Production flags give size 5200
 * against 5240 and 2042 encodings against 2061, so the 1885 is objcmp's
 * index-wise count across a length mismatch. Rank this file by the table below,
 * not by that number.
 *
 * SPLIT SHAPE. `python3 tools/datacheck.py` on the reference prints nothing (no
 * interleaved data); `.thumb_func_start` / `.func_end` pairs put TWO functions
 * in the file -- OvlFunc_883_20095dc at lines 8-2026 and OvlFunc_883_200aa54 at
 * 2033-2278 -- so a TWO-WAY split is needed on install. Reported from
 * `tools/split_s.py --dry-run`; no destructive run was made.
 * `python3 tools/shimcount.py` on the file reports no shims.
 *
 * FRAME, read with the three greps. `sub sp, #0x24` is the only grep-1 hit.
 * Grep 2 (`mov rX, sp`) finds NOTHING -- there are no aggregates. Grep 3
 * (`add rX, sp`) finds one site, `add r1, sp, #0x20` at the epilogue, and it
 * feeds a LOAD (`ldrb r1, [r1]`), so it is a spill read, not argument staging.
 * The frame is therefore: sp+0x00..0x18 = seven outgoing-argument words (the
 * eleven-argument __Func_80931ec call writes sp, +4, +8, +0xc, +0x10, +0x14,
 * +0x18), sp+0x1c = spilled pointer, sp+0x20 = spilled int. TWO spill slots,
 * no aggregates.
 *
 * The sp+0x20 slot is written with `str` and read with `ldrb`. That is an `int`
 * local, not an `unsigned char` one: combine narrows the reload to QImode
 * because only the low byte feeds the final `strb`, and Thumb-1 has no
 * sp-based `ldrb`, which is what forces the `add r1, sp, #0x20`.
 *
 * LOOP-COMPARISON CENSUS, RUN ON THIS FUNCTION'S SLICE ONLY.
 *   b 4 | bne 5 | beq 0 | signed (blt/ble/bgt/bge) 0 | unsigned (bhi/bls) 0
 * ALL FOUR `b` ARE POOL SKIPS (`b .LX` / `.pool_aligned` / `.LX:` at 438, 854,
 * 1266, 1693), so there is no unconditional control flow at all. The five `bne`
 * are three `if`s on __Func_8091c7c's return and two do/while backedges
 * (`cmp r5,#4` and `cmp r5,#5`), both `!=`. 1996 instructions in what is
 * effectively one basic block plus five two-block regions.
 *
 * POOLED-CONSTANT MULTISET -- 49 distinct values over 81 `ldr rX, =` sites.
 * Top of the reload histogram: 0x4ccc x9, 0x2666 x8, 0x34b x5, 0x101 x5,
 * 0x105 x4, iwram_3001ebc x2, 0xe666 x2, 0x357 x2, 0x33b x2, 0x179 x2,
 * 0x1790000 x2, 0x33b x2; the other 37 appear once.
 *
 * ===== RESULT: THE CONSTANT SET IS IDENTICAL, ENTRY FOR ENTRY =====
 * All 49 distinct pooled values and symbols are present on both sides and
 * NOTHING is in one and not the other (numeric normalisation needed -- the
 * reference writes hex, gcc writes decimal, and an un-normalised comparison
 * reports all 49 as differing on both sides, which reads exactly like a wrong
 * reconstruction). Per band doc section 5 that single comparison establishes
 * that every constant, actor slot, coordinate, message id, script pointer and
 * call in a 1996-instruction reconstruction is correct. The ENTIRE residue is
 * the RELOAD COUNT and the register allocation:
 *
 *   value           ref sites   ours
 *   0x4ccc              9         4
 *   0x2666              8         4
 *   0x101               5         2
 *   0x34b               5         3
 *   0x105               4         1
 *   iwram_3001ebc       2         3
 *
 * ===== MEASURED TABLE (all figures from tools/objcmp.py's own comparator) =====
 * Instruments: SIZE and ENCODINGS from objcmp; insns and pool words counted
 * SEPARATELY, never summed; aligned/hunks from an LCS over the encoding
 * streams (aligncmp's instrument); histogram = per-opcode counts.
 *
 * | candidate / flags                    | size | insns | pool ld | highreg | mov rlo,rhigh | relocs | aligned | hunks |
 * |--------------------------------------|------|-------|---------|---------|---------------|--------|---------|-------|
 * | REFERENCE                            | 5240 |  1996 |      81 |      21 |            13 |    501 |    100% |     0 |
 * | this file, PRODUCTION -O2            | 5200 |  1979 |      79 |      63 |            46 |    502 |   81.4% |   334 |
 * | this file, CSE_CFLAGS                | 5240 |  1996 |      78 |      42 |            31 |    502 |   85.0% |   282 |
 * | this file + blanket pin, PRODUCTION  | 5236 |  1994 |      81 |      19 |            11 |    501 |   85.9% |   338 |
 * | this file + blanket pin, CSE_CFLAGS  | 5236 |  1994 |      81 |      19 |            11 |    501 |   85.9% |   338 |
 *
 * CSE_CFLAGS is the existing Makefile group `$(GCC296_CFLAGS)
 * -fno-rerun-cse-after-loop` (Makefile line 835), so that row is reproducible
 * by the build; it is NOT the production row and does not appear in the claim
 * line. The blanket-pin rows are reproduced by applying ONE mechanical rule to
 * this file: wrap every call whose argument list is four or fewer arguments and
 * which has a numeric literal argument outside 0..255 in a braced block that
 * declares each such argument `register int qN __asm__("rK") = <literal>;`,
 * K being the zero-based argument index, and passes qN. 162 call sites match.
 *
 * ===== 1. -fno-rerun-cse-after-loop TAKES SIZE AND COUNT TO EXACT =====
 * 5240/5240 and 1996/1996 instructions, from -40 and -17. This CONTRADICTS
 * docs/band-800plus.md section 2, which measured the flag on
 * OvlFunc_881_2008c28 (47 -> 43 high-register mentions, aligned WORSE) and
 * concluded "no flag reaches it" for the straight-line population. The
 * difference is that 2008c28 has no loop: cse2 runs after loop.c either way,
 * but on a function with two real do/while loops loop.c has rewritten the
 * CFG and cse2 then commons across the rewritten blocks. Here it is worth 17
 * instructions and 15 of the 33 excess reuse copies. The flag's own row in
 * the band doc should be narrowed to "inert on the loopless members".
 *
 * ===== 3. AN EXACT INSTRUCTION COUNT CAN BE A COINCIDENCE, AND THE =====
 * ===== PER-OPCODE HISTOGRAM IS THE INSTRUMENT THAT SEES IT =====
 * This is a new rung on the figures-that-lie ladder, one above "an exact SIZE
 * can be a coincidence". The CSE_CFLAGS row is 1996 instructions against 1996.
 * Its histogram against the reference:
 *
 *      mov 1106 -> 1134  (+28)      lsl 161 -> 139  (-22)
 *      neg   7 -> 3      (-4)       ldr  90 -> 87   (-3)
 *      sub   2 -> 3      (+1)       everything else exact
 *
 * +28 -22 -4 -3 +1 = 0. FIFTY instructions are in the wrong place and the
 * total is exact because the errors cancel to the encoding. Meanwhile the
 * blanket-pin row, which is TWO instructions SHORT, has:
 *
 *      str 41 -> 40 (-1)            ldr 90 -> 89 (-1)
 *      mov 1106 -> 1106, lsl 161 -> 161, bl 496 -> 496, and every other
 *      opcode EXACT
 *
 * So the candidate with the exact count is structurally fifty instructions
 * wrong and the candidate two short is two instructions wrong. Compare the
 * HISTOGRAM, not the total -- the existing discipline (compare instructions
 * and pool words separately rather than their sum) does not reach this,
 * because here it is `mov` against `lsl` INSIDE the instruction count.
 *
 * ===== 2. THE RESIDUE OF THE BLANKET-PIN CANDIDATE IS ONE SPILL PAIR =====
 * One `str` and one `ldr`, and the frames say which: ours is `sub sp, #32`
 * with slots 0..0x1c, the reference is `sub sp, #0x24` with slots 0..0x20.
 * The reference spills BOTH long-lived values -- the `&a->f55` pointer at
 * sp+0x1c (str + ldr) and the saved byte at sp+0x20 (str + add/ldrb). We spill
 * only the saved byte and give the pointer `fp`.
 *
 * The quantity SET is otherwise identical. Reference: r8=0, r9=2,
 * r10=gScript_883__0200e590, r11=&iwram_3001ebc, r5={1,0,0xfe,1},
 * r6=actor pointer, r7={0x10000,0xfe}, spill q, spill saved. Ours: r8=script,
 * r9=&iwram, sl=2, fp=q, r5={1,0,0xfe,1}, r6={0,0x10000,0xfe}, r7=actor
 * pointer, spill saved. Same nine live-range classes, one fewer spilled,
 * permuted between the low and high banks. That is the `global.c` priority
 * blocker recorded for CalcStats in batch 311, with the extra observation
 * that OUR allocator merges `zero`, `0x10000` and `0xfe` into one register
 * (r6) where the reference keeps `zero` separate in r8 -- the freed register
 * is what `q` takes instead of spilling.
 *
 * ===== 4. THE PURE/MIXED DISCRIMINATOR: PARTITION THE REUSE COPIES BY =====
 * ===== WHAT THE SOURCE REGISTER HOLDS. THE RAW COUNT CONFLATES FOUR CLASSES =====
 * Batch 311's screen is "nothing reloaded more than ~3x and few reuse copies
 * means pure rebuild; a value reloaded seven or eight times with ~30 reuse
 * copies means mixed". BOTH HALVES FAIL HERE. This function reloads 0x4ccc
 * NINE times and is a PURE REBUILD, and its 13 `mov rlo,rhigh` are not 13 of
 * anything -- they partition into four unrelated classes:
 *
 *   (a) PROLOGUE HIGH-SAVE moves                     4  -- structural, ignore
 *   (b) SYMBOL ADDRESSES (pooled, named locals)      5  -- &gScript, &iwram
 *   (c) ONE-INSTRUCTION constants (`mov #imm8`)      4  -- 0 x3, 2 x1
 *   (d) MULTI-INSTRUCTION or POOLED constants        0  <-- the only signal
 *
 * Only class (d) says anything about rebuild-versus-reuse, because only a
 * multi-instruction constant is a value whose commoning changes the
 * instruction count. And every value this reference RELOADS from the pool has
 * ZERO copies: 0x4ccc (9 reloads), 0x2666 (8), 0x101 (5), 0x34b (5), 0x105 (4)
 * are each rebuilt at every single site.
 *
 * So THE RELOAD COUNT CARRIES NO INFORMATION AT ALL, and the raw copy count
 * carries almost none. The rule is mechanical and needs no tooling: for
 * each `mov rlo,rhigh`, find the last `mov rhigh, rlo` that set it, then walk
 * back for that low register's defining insn and keep only the ones defined by
 * `ldr =<number>` or by `mov #imm8` followed by a shift.
 *
 * Measured on four band references, with the pin residue where it is known:
 *
 * | reference            | insns | prologue | (b) addr | (c) imm8 | (d) MIXED-SIGNAL | pin residue |
 * |----------------------|-------|----------|----------|----------|------------------|-------------|
 * | OvlFunc_883_20095dc  |  1996 |        4 |        5 |        4 |            **0** |  **2 opcodes** |
 * | OvlFunc_897_2009410  |  1932 |        3 |       10 |        2 |                1 | not measured |
 * | OvlFunc_911_20088ec  |  2800 |        2 |        0 |        2 |            **7** | not measured |
 * | OvlFunc_883_200b4c8  |  1024 |        4 |        5 |        6 |           **11** | **17 opcodes** |
 *
 * (b) for 2009410 is ten copies of a POINTER -- two __MapActor_GetActor return
 * values -- which the raw count would have read as heavy constant reuse.
 *
 * The two points where the pin residue is known are d=0 -> 2 and d=11 -> 17,
 * so the screen is also a PREDICTION of how far the blanket pin gets. Its
 * standing prediction for the two targets not reconstructed: the pin should
 * nearly close OvlFunc_897_2009410 (d=1) and leave a substantial residue on
 * OvlFunc_911_20088ec (d=7).
 *
 * ===== 4b. THE CROSS-CHECK ON A SECOND FUNCTION, WHICH KILLED ONE CLAIM =====
 * The band doc's own guard -- measure a second function before writing the
 * first one up as a general finding -- was run on OvlFunc_883_200b4c8, which
 * already has a candidate in the tree (src/non_matching/overlays/200b4c8.c),
 * is in the SAME overlay, is the same cutscene-script shape and HAS A LOOP
 * (one `bls`, two `b`). Reference now at
 * asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_c.s -- the path in
 * that park's own header no longer exists, the file having been re-split.
 *
 *   -fno-rerun-cse-after-loop on 200b4c8: BYTE-IDENTICAL to production.
 *   Same 2792 bytes, same 1077 encodings, same 960 differing, same 29 copies,
 *   same 70.4% aligned, same 295 hunks.
 *
 * So FINDING 1 DOES NOT TRANSFER and must not be written up as a band lever.
 * It is worth 17 instructions and 15 copies here and exactly nothing there,
 * on two functions that are the same shape in the same overlay and both have
 * a loop. "Has a loop" is NOT the precondition. Whatever the precondition is,
 * it is finer than any shape measured in this batch, so the flag has to be
 * swept per function -- the same conclusion the corpus already reached for the
 * loop-comparison census, reached here for a FLAG rather than a spelling.
 *
 *   The blanket pin on 200b4c8, PRODUCTION flags:
 *     unpinned: size 2792/2792 EXACT, insns 1022/1024, pool loads 63/78,
 *               highreg 44, copies 29, 70.4% aligned, 295 hunks
 *               histogram: mov +26, ldr -15, lsl -14, str +1  (56 misplaced)
 *     pinned (103 sites): size 2764 (-28), insns 1009, pool loads 77/78,
 *               highreg 31, copies 22, 75.9% aligned, 282 hunks
 *               histogram: mov -12, lsl -3, ldr +1, add -1    (17 misplaced)
 *
 * So FINDING 2 TRANSFERS: the blanket pin cuts the histogram error more than
 * threefold and improves BOTH the aligned figure and the hunk count, while
 * losing a size figure that was exact. And 200b4c8 is a SECOND instance of
 * finding 3 -- its park reads its exact size as "two instructions from the
 * right length", when 26 extra `mov` are cancelling 29 missing `ldr`/`lsl`.
 * That park's own next-step question ("whether the fifteen-way constant hoist
 * can be defeated at all") has an answer: yes, by the pin, at 103 sites.
 *
 * ===== 5. SELECTIVE PINS MEASURED NEGATIVE. THE PASS IS ALL-OR-NOTHING =====
 * This was the brief's central open question and the answer here is clean.
 * All rows under CSE_CFLAGS, compared against no pins at 5240 / 1996 insns /
 * highreg 42 / copies 31 / 85.0% / 282 hunks:
 *
 * | pin set                              | sites | size | insns | highreg | copies | aligned | hunks |
 * |--------------------------------------|-------|------|-------|---------|--------|---------|-------|
 * | none                                 |     0 | 5240 |  1996 |      42 |     31 |   85.0% |   282 |
 * | 0x4ccc + 0x2666 (the max-reload pair)|     9 | 5236 |  1995 |      46 |     33 |   84.6% |   297 |
 * | 0xc000 only                          |    15 | 5252 |  2002 |      38 |     26 |   85.2% |   297 |
 * | every __Func_8092adc argument        |    68 | 5260 |  2006 |      36 |     25 |   83.3% |   375 |
 * | 0xc000 + 0x4ccc + 0x2666             |    24 | 5248 |  2000 |      43 |     30 |   84.4% |   306 |
 * | ALL 162                              |   162 | 5236 |  1994 |      19 |     11 |   85.9% |   338 |
 * | ALL except 0xc000                    |   147 | 5228 |  1991 |      27 |     18 |   86.4% |   328 |
 * | ALL except 0x4ccc,0x2666             |   153 | 5244 |  2000 |      25 |     17 |   84.8% |   340 |
 * | ALL except 0xa00000                  |   156 | 5236 |  1994 |      19 |     11 |   85.7% |   348 |
 * | ALL except 0x10000,0x8000            |   154 | 5224 |  1987 |      30 |     21 |   84.8% |   329 |
 *
 * Pinning the MAX-RELOAD PAIR -- the sites the brief predicted would want a pin
 * -- makes high-register mentions and reuse copies WORSE (42->46, 31->33). Every
 * partial set is worse than both endpoints on size and count. The mechanism is
 * that allocation is a ZERO-SUM COMPETITION: removing one constant's allocno
 * does not free a register, it hands that register to the next constant in
 * `global.c`'s priority order. So the two levers do not compose SITE BY SITE,
 * which is why OvlFunc_969_20088b4's mixed case resisted selective pinning --
 * the thing being selected over is not local to the site.
 *
 * `ALL except 0xa00000` is BYTE-IDENTICAL to `ALL` on every axis but hunks,
 * which BOUNDS the pin lever: a value that occurs at only four sites and never
 * survives a call has no allocno to remove, so pinning it is inert.
 *
 * ===== 6. UNDER THE BLANKET PIN, -fno-rerun-cse-after-loop IS INERT =====
 * pin_all at production -O2 and pin_all under CSE_CFLAGS are the same object:
 * 5236 bytes, 2059 encodings, 1857 differing, 501 relocations, 19 high-register
 * mentions. The flag and the pin reach the SAME commoning, so they do not
 * compose. Read the other way: the pin is strictly stronger than the flag, and
 * a function that needs both needs neither twice.
 *
 * ===== 7. THE EIGHT-BIT-MOVABLE POOLED CONSTANT SCREEN IS INERT HERE =====
 * The brief's strongest lever for this batch -- a reference that POOLS a word
 * loadable with one `mov #imm8` has a RELOCATION there, not a literal -- finds
 * NOTHING. The smallest numeric value in all 49 pooled entries is 0x101, and
 * every one of the 40 numeric entries is above 0xff. Same screen on the batch's
 * other two targets: OvlFunc_897_2009410's smallest is 0x101, and
 * OvlFunc_911_20088ec's is 0x101. Zero candidate sites across 120 distinct
 * pooled values in three functions. The lever's MECHANISM is sound and its
 * POPULATION is not this one -- the same shape as batch 311's switch-material
 * correction.
 *
 * ===== 8. TWO RELOCATION DIFFERENCES, BOTH NAMED =====
 * Under CSE_CFLAGS without pins the sequences differ in exactly two places:
 * one `__CutsceneWait` call sits four entries earlier than the reference's
 * because the first literal-pool dump lands before the call rather than after
 * it, and there is one EXTRA `iwram_3001ebc` pool word (ref 2, ours 3). The
 * blanket pin fixes BOTH -- 501 against 501 in identical symbol order. So the
 * global's address being reloaded twice and not three times is a consequence of
 * the allocation, not of how the four use sites are spelled.
 *
 * ===== WHAT WAS ESTABLISHED AND SHOULD NOT BE RE-DERIVED =====
 *   - The four `b` are pool skips. There is no join point; .L1a50, .L1e8c,
 *     .L2714 and .L22bc are all `b .LX / .pool_aligned / .LX:`.
 *   - `&iwram_3001ebc + 0x1c0 / + 0x1c8` take a zero and 0x20; `+ 0x1d8` is an
 *     unsigned short incremented at three sites guarded by __Func_8091c7c
 *     returning 0, 1 and 1 respectively.
 *   - The 0x10000 in the reference is built in TWO SETS (`mov r7,#0x80` before
 *     a call, `lsl r7,#9` after it), which is the REG_N_SETS-defeating form.
 *     This file writes it that way (`v = 0x80; v <<= 9;`) and the value lands
 *     in a register, not a REG_EQUIV rematerialisation.
 *   - Struct offsets used: short at 0xa and 0x12 (register-offset `ldrsh` --
 *     Thumb-1 has no immediate form, so a constant offset always costs a
 *     `mov`), int at 0x10/0x18/0x1c, pointer at 0x50 with a byte store at its
 *     +0x26, unsigned char at 0x23/0x55/0x5a.
 *   - `__MapTransitionIn` takes NO arguments; draft_script.py over-guesses its
 *     arity to four. `__Func_80931ec` takes ELEVEN (4 register + 7 stack) and
 *     is what sets the 0x18-byte argument area.
 *
 * ===== NEXT =====
 * One spill. Make the `&a->f55` pointer spill and the blanket-pin candidate is
 * byte-exact on every opcode. The levers not yet tried for that: raising
 * pressure in the middle region by denying our allocator the r6 merge of
 * `zero`/`0x10000`/`0xfe`, and the declaration-order instrument (band doc: for
 * a SPILLING function the ranking instrument is the spill-slot access-count
 * table, and this function spills, so that instrument applies).
 */
struct A {
    unsigned char pad00[0xa];
    short f0a;
    unsigned char pad0c[4];
    int f10;
    short f12;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[3];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    unsigned char *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[0x5a - 0x56];
    unsigned char f5a;
};

extern char *iwram_3001ebc;
extern unsigned char gScript_883__0200e590[];
extern unsigned char gScript_883__0200e5cc[];
extern unsigned char gScript_883__0200e614[];

extern struct A *__MapActor_GetActor(int slot);
extern struct A *__Func_8093554(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_80933d4(int a, int b);
extern void __Func_800fe9c(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Surprise(int slot, int a);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);
extern void __MapTransitionIn(void);
extern void __Actor_SetSpriteFlags(struct A *a, int f);
extern void __ActorMessage(int slot, int a);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __SetCameraTarget(int a, int b);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_8092158(int a, int b, int c);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80931ec(int a, int b, int c, int d, int e, int f, int g,
                           int h, int i, int j, int k);
extern void __Func_8093530(void);
extern void __Func_80917f4(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void OvlFunc_883_200b2b0(int a, int b, int c, int d);
extern void OvlFunc_883_200b380(int a, int b, int c, int d);
extern void OvlFunc_883_200b45c(int a, int b, int c);

void OvlFunc_883_20095dc(void)
{
    struct A *pa;
    struct A *a;
    unsigned char *q;
    unsigned char *s;
    unsigned char *r;
    unsigned char *script;
    char *w;
    int saved;
    int zero;
    int one;
    int two;
    int v;
    int m;
    int m2;
    int k;
    int z;
    int i;
    int j;

    __CutsceneStart();
    __Func_80933f8(-1, -1, -1, 0);
    pa = __Func_8093554();
    zero = 0;
    pa->f55 = zero;
    __Func_80933f8(0x17f0000, 0xa00000, 0x36d0000, 0);
    __CutsceneWait(1);
    __Func_800fe9c();
    __Func_8010704(0x31, 0x29, 7, 3, 0x14, 0x32);
    two = 2;
    one = 1;
    __CopyMapTiles(2, 0x66, 0x54, 0x29, two, one);
    __CopyMapTiles(1, 0x66, 0x53, 0x29, one, one);
    __CopyMapTiles(0, 0x67, 0x52, 0x2a, one, one);
    a = __MapActor_GetActor(0);
    q = &a->f55;
    saved = *q;
    *q = zero;
    __MapActor_SetPos(0, 0x1970000, 0x2b20000);
    __MapActor_SetPos(0x15, 0x1880000, 0x3800000);
    __MapActor_SetPos(1, 0x12a0000, 0x2e00000);
    __MapActor_SetPos(5, 0x12a0000, 0x2f80000);
    __Func_8092adc(0, 0xc000, 0);
    __Func_8092adc(0x15, 0xc000, 0);
    __Func_8092adc(1, 0x8000, 0);
    __Func_8092adc(5, 0x8000, 0);
    __MapActor_SetAnim(0, 0xb);
    script = gScript_883__0200e590;
    __MapActor_SetBehavior(0, script);
    OvlFunc_883_200b45c(0x17, 2, 1);
    w = iwram_3001ebc;
    *(int *)(w + 0xe0 * 2) = zero;
    *(int *)(w + 0xe4 * 2) = 0x20;
    __MapTransitionIn();
    __MapActor_SetSpeed(5, 0x8000, 0x4000);
    __MapActor_SetSpeed(1, 0x8000, 0x4000);
    __MapActor_SetBehavior(5, gScript_883__0200e614);
    __MapActor_SetBehavior(1, gScript_883__0200e5cc);
    v = 0x80;
    __MapActor_SetBehavior(0, (unsigned char *)1);
    v <<= 9;
    a->f18 = v;
    a->f1c = v;
    __Func_8092adc(0, 0xb000, 0x28);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0xa);
    __MapActor_SetSpeed(0, 0x4ccc, 0x2666);
    __Func_80921c4(0, 0x194, 0x34b);
    __CutsceneWait(0xa);
    __Func_8092adc(0, 0xc000, 0x1e);
    __Func_80925cc(0, 1);
    __CutsceneWait(0x14);
    __Func_8092adc(0, 0x8000, 0x28);
    __Func_80925cc(0, 2);
    __CutsceneWait(0x14);
    __Func_8092950(0, 2);
    __Func_8092950(0x17, 2);
    __MapActor_SetSpeed(0x17, 0x4ccc, 0x2666);
    __Func_8092158(0x17, 0x186, 0x340);
    __CutsceneWait(0x50);
    __Func_8092adc(0, 0xc000, 0);
    __Func_8092158(0x17, 0x192, 0x33c);
    __CutsceneWait(0x50);
    __Func_8092950(0, 0);
    __Func_8092950(0x17, 0);
    __MapActor_SetPos(0x17, 0x1860000, 0x34a0000);
    __MapActor_SetAnim(0, 0xb);
    __MapActor_SetBehavior(0, gScript_883__0200e590);
    __CutsceneWait(0xc8);
    __CopyMapTiles(7, 0x66, 0x54, 0x29, two, one);
    __MapActor_SetBehavior(0, (unsigned char *)1);
    a->f18 = v;
    a->f1c = v;
    __MapActor_SetAnim(0, 1);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __Func_80921c4(0, 0x179, 0x34b);
    __CutsceneWait(0xa);
    __Func_8092adc(0, 0xc000, 0x1e);
    __Func_8092adc(0, 0, 0x14);
    __Func_8092950(0, 2);
    __Func_8092950(0x17, 2);
    __MapActor_SetSpeed(0x17, 0x4ccc, 0x2666);
    __Func_8092158(0x17, 0x186, 0x340);
    __CutsceneWait(0x50);
    __Func_8092adc(0, 0xc000, 0);
    __Func_8092158(0x17, 0x179, 0x33c);
    __CutsceneWait(0x50);
    __Func_8092950(0, 0);
    __Func_8092950(0x17, 0);
    __MapActor_SetPos(0x17, 0, 0);
    __MapActor_SetAnim(0, 0xb);
    __MapActor_SetBehavior(0, gScript_883__0200e590);
    __CutsceneWait(0xc8);
    __CopyMapTiles(6, 0x66, 0x53, 0x29, one, one);
    __MapActor_SetBehavior(0, (unsigned char *)1);
    a->f18 = v;
    a->f1c = v;
    __MapActor_SetAnim(0, 1);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __Func_80921c4(0, 0x168, 0x357);
    __Func_8092adc(0x15, 0xb000, 0xa);
    __Func_8092adc(0, 0xc000, 0x1e);
    __Func_8092adc(0, 0xd000, 0x14);
    __Func_8092950(0, 2);
    __Func_8092950(0x18, 2);
    __MapActor_SetSpeed(0x18, 0x4ccc, 0x2666);
    __Func_8092158(0x18, 0x186, 0x340);
    __CutsceneWait(0x50);
    __Func_8092adc(0, 0xc000, 0);
    __Func_8092158(0x18, 0x168, 0x34a);
    __CutsceneWait(0x50);
    __Func_8092950(0, 0);
    __Func_8092950(0x18, 0);
    __MapActor_SetPos(0x18, 0, 0);
    __MapActor_SetAnim(0, 0xb);
    __MapActor_SetBehavior(0, gScript_883__0200e590);
    __CutsceneWait(0xc8);
    __CopyMapTiles(5, 0x67, 0x52, 0x2a, one, one);
    __MapActor_SetBehavior(0, (unsigned char *)1);
    a->f18 = v;
    a->f1c = v;
    __MessageID(0xf03);
    __MapActor_Jump(0x15, 2, 0x14);
    __Func_8093040(0x15, 0, 0x14);
    __Func_8092adc(0, 0x1000, 0x14);
    OvlFunc_883_200b2b0(0x15, 5, 6, 0);
    __MapActor_SetSpeed(0x15, 0x4ccc, 0x2666);
    __Func_80921c4(0x15, 0x18d, 0x340);
    __CutsceneWait(0x14);
    __Func_8092adc(0x15, 0x4000, 0x3c);
    __Func_8092adc(0x15, 0xc000, 0x3c);
    __MapActor_DoAnim(0x15, 3);
    __MapActor_DoAnim(0x15, 3);
    __CutsceneWait(0xa);
    __Func_8093040(0x15, 0, 0x14);
    __Func_80921c4(0x15, 0x174, 0x340);
    __CutsceneWait(0x14);
    __Func_8092adc(0x15, 0x4000, 0x28);
    __Func_8092adc(0x15, 0x8000, 0x28);
    __MapActor_DoAnim(0x15, 3);
    __MapActor_DoAnim(0x15, 3);
    __CutsceneWait(0xa);
    __Func_8093040(0x15, 0, 0x14);
    __Func_8092adc(0x15, 0x5000, 0x1e);
    __MapActor_DoAnim(0x15, 3);
    __CutsceneWait(0xa);
    __Func_8093040(0x15, 0, 0x14);
    __Func_80925cc(0, 2);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(0x15, 4);
    __CutsceneWait(0x14);
    __Func_8092c40(0x15, 0);
    if (__Func_8091c7c(0, 0) == 0)
        *(unsigned short *)(iwram_3001ebc + 0xec * 2) += 1;
    __MapActor_DoAnim(0x15, 4);
    __CutsceneWait(0x14);
    __Func_8093040(0x15, 0, 0x14);
    __MessageID(0xf0a);
    __Func_80921c4(0x15, 0x182, 0x349);
    __CutsceneWait(0xa);
    __Func_8092adc(0x15, 0xd000, 0x3c);
    __Func_80925cc(0x15, 2);
    __CutsceneWait(0x14);
    __Func_8093040(0x15, 0, 0x14);
    __Func_8092adc(0x15, 0x5000, 0x1e);
    __Func_8092c40(0x15, 0);
    if (__Func_8091c7c(0, 0) == 1)
        *(unsigned short *)(iwram_3001ebc + 0xec * 2) += 1;
    __CutsceneWait(0x14);
    __Func_8093040(0x15, 0, 0x14);
    __Func_8092adc(0x15, 0xd000, 0x3c);
    __Func_80925cc(0x15, 2);
    __CutsceneWait(0x14);
    __MessageID(0xf0e);
    __Func_8093040(0x15, 0, 0x14);
    __Func_80921c4(0x15, 0x182, 0x339);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x15, 4);
    __CutsceneWait(0x3c);
    __Func_8093040(0x15, 0, 0x3c);
    __Func_8092adc(0x15, 0x5000, 0xa);
    __Func_8093040(0x15, 0, 0x14);
    __Func_80921c4(0x15, 0x174, 0x340);
    __Func_8092adc(0x15, 0x5000, 0x14);
    __Func_80925cc(0, 2);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x15, 3);
    __CutsceneWait(0x14);
    __Func_8093040(0x15, 0, 0x14);
    __MapActor_Emote(0, 0x102, 0x3c);
    __Func_80925cc(0x15, 2);
    __CutsceneWait(0x14);
    __Func_8093040(0x15, 0, 0x14);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0x15, 3);
    __CutsceneWait(0x14);
    __Func_8093040(0x15, 0, 0xa);
    __Func_80933d4(0x6666, 0xccc);
    __Func_80933f8(0x1790000, 0xa00000, 0x35c0000, 1);
    __MapActor_SetSpeed(5, 0x10000, 0x8000);
    __MapActor_SetSpeed(1, 0x10000, 0x8000);
    __Func_809218c(1, 0x171, 0x388);
    __Func_80921c4(5, 0x188, 0x388);
    __MapActor_SetAnim(1, 1);
    OvlFunc_883_200b2b0(5, 0xa, 0xb, 0);
    __Func_8092adc(5, 0xa000, 0);
    __Func_8093040(5, 0, 0xa);
    __Func_80925cc(0x15, 2);
    __CutsceneWait(0xa);
    __Func_8092adc(0x15, 0x3000, 0);
    __Func_8092adc(0, 0x1000, 0x1e);
    __MapActor_Jump(5, 4, 0);
    __Func_80921c4(5, 0x188, 0x34b);
    __Func_8092adc(5, 0x9000, 0);
    __Func_8092adc(0x15, 0x3000, 0);
    __Func_8092adc(0, 0xd000, 0x28);
    __MapActor_DoAnim(0x15, 3);
    __CutsceneWait(0x14);
    __Func_8093040(0x15, 0, 0x14);
    __MapActor_DoAnim(5, 3);
    __CutsceneWait(0x14);
    __Func_8093040(5, 0, 0x14);
    __MapActor_SetAnim(0x15, 3);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __Func_8093040(0x15, 0, 0x14);
    OvlFunc_883_200b2b0(5, 0xa, 0xb, 0);
    __MapActor_SetSpeed(5, 0x4ccc, 0x2666);
    __MapActor_SetSpeed(1, 0x4ccc, 0x2666);
    __Func_809218c(1, 0x188, 0x34b);
    s = &__MapActor_GetActor(5)->f5a;
    m = 0xfe;
    *s &= m;
    __Func_80921c4(5, 0x198, 0x34b);
    __CutsceneWait(1);
    s = &__MapActor_GetActor(5)->f5a;
    *s |= 1;
    __Func_8092adc(5, 0x8000, 0);
    __MapActor_WaitMovement(1);
    __MapActor_SetAnim(1, 1);
    __Func_8092adc(1, 0x8000, 0x1e);
    __MapActor_Jump(0x15, 4, 0x1e);
    __Func_8093040(0x15, 0, 0x14);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x1e);
    __Func_8092adc(0x15, 0x5000, 0x1e);
    __Func_8093040(0x15, 0, 0x14);
    __Func_8092adc(0, 0xd000, 0);
    __MapActor_Jump(0, 2, 0x1e);
    __MapActor_Surprise(0, 0x102);
    __CutsceneWait(0x3c);
    __Func_8092adc(0x15, 0x3000, 0x28);
    __Func_80925cc(1, 2);
    __Func_8093040(1, 0, 0x14);
    __MapActor_Emote(0x15, 0x101, 0x50);
    __Func_8092adc(0x15, 0x5000, 0x1e);
    __MapActor_Emote(0, 0x102, 0x50);
    __Func_8092adc(0x15, 0x3000, 0);
    __Func_8092adc(0, 0xd000, 0x28);
    __Func_8093040(0x15, 0, 0x14);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x64);
    __Func_8092848(5, 1, 0x1e);
    __Func_809259c(1, 2);
    __Func_80925cc(5, 2);
    __CutsceneWait(0xa);
    __MapActor_Emote(0x15, 0x105, 0x3c);
    __Func_8093040(0x15, 0, 0x14);
    __Func_8092adc(5, 0x8000, 0);
    __Func_8092adc(1, 0x8000, 0x1e);
    __Func_80925cc(5, 2);
    __CutsceneWait(0x1e);
    __Func_8093040(5, 0, 0x14);
    __Func_809259c(0x15, 2);
    __CutsceneWait(0x1e);
    __Func_8093040(0x15, 0, 0x14);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(5, 3);
    __CutsceneWait(0x1e);
    __MapActor_DoAnim(0x15, 4);
    __CutsceneWait(0x14);
    __Func_8093040(0x15, 0, 0x14);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(5, 3);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x15, 3);
    __CutsceneWait(0x14);
    __Func_8093040(0x15, 0, 0x14);
    __MapActor_SetAnim(1, 3);
    __MapActor_DoAnim(5, 3);
    __CutsceneWait(0xa);
    __Func_80925cc(0x15, 2);
    __CutsceneWait(0x1e);
    __Func_8093040(0x15, 0, 0x14);
    __Func_80933d4(0x9999, 0x1333);
    __Func_80933f8(0x1750000, 0xa00000, 0x3450000, 1);
    __Func_80921c4(0x15, 0x16c, 0x330);
    __Func_8092adc(0, 0xd000, 0);
    __Func_8092adc(0x15, 0x3000, 0x1e);
    __Func_8093040(0x15, 0, 0x28);
    __Func_8092848(5, 1, 0x1e);
    __Func_809259c(1, 2);
    __Func_80925cc(5, 2);
    __CutsceneWait(0x1e);
    __Func_8092adc(1, 0x8000, 0);
    __Func_8092adc(5, 0x8000, 0x1e);
    __Func_8092adc(0x15, 0x5000, 0x1e);
    __Func_8093040(0x15, 0, 0x1e);
    __MapActor_Emote(0, 0x105, 0x3c);
    __MapActor_DoAnim(0x15, 4);
    __CutsceneWait(0xa);
    __Func_8092c40(0x15, 0);
    z = 0;
    if (__Func_8091c7c(0, 0) == 1)
        *(unsigned short *)(iwram_3001ebc + 0xec * 2) += 1;
    __CutsceneWait(0x28);
    __Func_8093040(0x15, 0, 0x14);
    __MessageID(0xf27);
    __MapActor_Emote(0x15, 0x103, 0);
    __Func_80925cc(0x15, 3);
    __CutsceneWait(0x1e);
    __Func_8093040(0x15, 0, 0x14);
    __MapActor_Jump(0x15, 4, 0);
    __Func_80925cc(0x15, 3);
    __MapActor_SetAnim(0x15, 7);
    __CutsceneWait(5);
    __Func_80931ec(0x15, 0xe, 2, 0x18, 2, 1, 0xa, 0xe, 4, 0xe, z);
    a = __MapActor_GetActor(0x15);
    s = &a->f5a;
    r = a->f50;
    r[0x26] = z;
    *s &= m;
    __MapActor_SetSpeed(0x15, 0x30000, 0x18000);
    __Func_80921c4(0x15, 0x16c, 0x32f);
    __CutsceneWait(4);
    i = 0;
    do {
        a->f10 += 0xc0 << 9;
        a->f1c += -0x1999;
        i++;
        __CutsceneWait(1);
    } while (i != 4);
    __MapActor_SetPos(0x15, 0, 0);
    __MapActor_SetSpeed(1, 0x30000, 0x18000);
    __MapActor_Jump(1, 6, 0);
    __Func_80921c4(1, 0x176, 0x33b);
    __ActorMessage(5, 0);
    __Func_8092adc(1, 0xb000, 0);
    __MapActor_Emote(5, 0x100, 0);
    __Func_809259c(5, 2);
    __MapActor_Emote(1, 0x100, 0xa);
    __MapActor_SetAnim(1, 0xd);
    __MapActor_Jump(1, 2, 5);
    __Func_8012330(0, 0x40000, 0x10000);
    __CopyMapTiles(1, 0x66, 0x53, 0x29, 1, 1);
    __Actor_SetSpriteFlags(__MapActor_GetActor(1), 0);
    __Func_8092adc(0, 0xd000, 0xa);
    __Func_809259c(1, 3);
    __Func_8012330(-1, -1, 0xe666);
    __Func_8012350();
    __MapActor_Emote(1, 0x102, 0x50);
    __MapActor_SetAnim(0x15, 8);
    a->f1c = 0x80 << 8;
    __MapActor_SetPos(0x15, 0x16c0000, 0x32b0000);
    j = 0;
    do {
        a->f1c += 0x1999;
        j++;
        __CutsceneWait(1);
    } while (j != 5);
    __CutsceneWait(0x3c);
    __Func_80925cc(1, 2);
    __Func_8092adc(1, 0x5000, 0x1e);
    __Func_809259c(1, 2);
    __Func_80925cc(5, 2);
    __CutsceneWait(0x3c);
    __Func_80925cc(0x15, 2);
    __CutsceneWait(0x14);
    __Func_8093040(0x15, 0, 0x14);
    __Func_80933d4(0x4ccc, 0x999);
    __Func_80933f8(0x1740000, 0xa00000, 0x35b0000, 1);
    __MapActor_SetSpeed(0x15, 0x30000, 0x18000);
    __MapActor_Jump(0x15, 6, 0);
    __Func_80921c4(0x15, 0x167, 0x343);
    __CutsceneWait(0x1e);
    __Func_8092adc(0x15, 0x4000, 0x1e);
    __Func_80925cc(0x15, 2);
    __CutsceneWait(0x1e);
    a = __MapActor_GetActor(0x15);
    s = &a->f23;
    m2 = 0xfe;
    *s &= m2;
    __Func_8093040(0x15, 0, 0x50);
    __MapActor_Emote(0x15, 0x101, 0x50);
    __Func_8092adc(0x15, 0, 0x3c);
    __Func_80925cc(0x15, 3);
    __Func_8093040(0x15, 0, 0x14);
    __MapActor_Surprise(0x15, 0x102);
    __CutsceneWait(0x50);
    __Func_8092adc(1, 0x5000, 0x1e);
    __MapActor_Emote(1, 0x102, 0x50);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __Func_8093040(1, 0, 0x14);
    __Func_8092adc(1, 0x8000, 0x1e);
    __Func_80925cc(1, 3);
    __CutsceneWait(0xa);
    __Func_80925cc(1, 3);
    __Actor_SetSpriteFlags(__MapActor_GetActor(1), 1);
    __MapActor_Jump(1, 6, 0);
    __MapActor_SetAnim(1, 1);
    __MapActor_SetSpeed(1, 0x40000, 0x20000);
    a = __MapActor_GetActor(1);
    s = &a->f5a;
    *s = m2 & *s;
    __MapActor_TravelTo(1, 0x193, 0x33b);
    __MapActor_Surprise(5, 0x102);
    __Func_8092adc(5, 0xc000, 0x14);
    __Func_8093040(5, 0, 1);
    __MapActor_WaitMovement(1);
    __Func_8092adc(1, 0x5000, 0x14);
    __Func_8093040(1, 0, 0x14);
    __MapActor_Emote(1, 0x100, 0);
    __MapActor_SetAnim(1, 0xd);
    __MapActor_Jump(1, 2, 5);
    __Actor_SetSpriteFlags(__MapActor_GetActor(1), 0);
    __CopyMapTiles(2, 0x66, 0x54, 0x29, 2, 1);
    __Func_8012330(0, 0x40000, 0x10000);
    __Func_80925cc(1, 3);
    __Func_8012330(-1, -1, 0xe666);
    __Func_8012350();
    __MapActor_Emote(1, 0x102, 0x1e);
    __MapActor_SetSpeed(5, 0x4ccc, 0x2666);
    __Func_80921c4(5, 0x198, 0x357);
    __CutsceneWait(0x3c);
    __Func_80925cc(1, 2);
    __MapActor_Emote(0x15, 0x105, 0x3c);
    __Func_809259c(5, 3);
    __Func_80925cc(0, 3);
    __CutsceneWait(0x50);
    __Func_8092adc(1, 0x4000, 0x1e);
    __Func_80925cc(1, 3);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(5, 4);
    __CutsceneWait(0x50);
    __MapActor_DoAnim(0x15, 3);
    __CutsceneWait(0xa);
    __Func_8093040(0x15, 0, 0x14);
    __Func_8092adc(5, 0xb000, 0);
    __Func_8092adc(0, 0xc000, 0);
    __Func_8092adc(0x15, 0, 0x3c);
    __Func_8092adc(0x15, 0x4000, 0x3c);
    __MapActor_DoAnim(0x15, 4);
    __CutsceneWait(0x3c);
    __Func_8092adc(0x15, 0, 0x50);
    __MapActor_Emote(0x15, 0x105, 0x50);
    __Func_8093040(0x15, 0, 0x3c);
    __Func_8092adc(0, 0xc000, 0);
    __MapActor_Emote(0, 0x101, 0);
    __MapActor_Emote(5, 0x101, 0);
    __MapActor_Emote(1, 0x101, 0x3c);
    __MapActor_DoAnim(0x15, 4);
    __CutsceneWait(0x1e);
    __Func_8093040(0x15, 0, 0x46);
    __Func_809259c(1, 2);
    __Func_80925cc(5, 2);
    __CutsceneWait(0x14);
    __Func_8092adc(5, 0x8000, 0x3c);
    __Func_8092adc(0x15, 0x4000, 0x1e);
    __Func_8093040(0x15, 0, 0x1e);
    __MapActor_DoAnim(5, 3);
    __CutsceneWait(0xa);
    __Func_8093040(5, 0, 0x14);
    __Func_8092adc(0x15, 0, 0x1e);
    __MapActor_DoAnim(0x15, 4);
    __CutsceneWait(0x14);
    __Func_8093040(0x15, 0, 0x14);
    __MapActor_DoAnim(5, 3);
    __CutsceneWait(0x14);
    __Func_80925cc(1, 3);
    __CutsceneWait(0xa);
    __Func_8093040(1, 0, 0x14);
    __Func_8092adc(0, 0xd000, 0);
    __MapActor_Emote(0x15, 0x100, 0);
    __Func_80925cc(0x15, 3);
    __CutsceneWait(0x1e);
    __Func_8093040(0x15, 0, 0x3c);
    __Func_80925cc(1, 3);
    __MapActor_SetSpeed(1, 0x10000, 0x8000);
    __Actor_SetSpriteFlags(__MapActor_GetActor(1), 0);
    __MapActor_Jump(1, 4, 0);
    __Func_80921c4(1, 0x18e, 0x33c);
    __CutsceneWait(0x3c);
    __Func_8092adc(0x15, 0x4000, 0);
    __Func_8092adc(0, 0xc000, 0x3c);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x3c);
    __MapActor_DoAnim(0x15, 3);
    __CutsceneWait(0x3c);
    a = __MapActor_GetActor(1);
    s = &a->f5a;
    k = 1;
    *s |= k;
    a = __MapActor_GetActor(5);
    s = &a->f5a;
    *s |= k;
    a = __MapActor_GetActor(0);
    __MapActor_SetSpeed(1, 0x10000, 0x8000);
    __MapActor_SetSpeed(5, 0x10000, 0x8000);
    __Func_8092adc(0, 0, 0);
    __Func_809218c(5, a->f0a + 0x10, a->f12);
    __Func_80921c4(1, a->f0a + 0x10, a->f12 - 0x10);
    __MapActor_WaitMovement(1);
    __Func_8092adc(1, 0x5000, 0x1e);
    __MapActor_SetAnim(1, 3);
    __MapActor_SetAnim(5, 3);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x28);
    __Func_80921c4(5, a->f0a, a->f12);
    __MapActor_SetPos(5, 0, 0);
    __Func_80921c4(1, a->f0a, a->f12);
    __MapActor_SetPos(1, 0, 0);
    __Func_80917f4(1, 5);
    __Func_80933f8(0x1790000, 0xa00000, 0x3770000, 1);
    OvlFunc_883_200b380(0, 0xd, 0xa, 0);
    __Func_80921c4(0, 0x178, 0x390);
    __Func_8092adc(0, 0xc000, 0);
    s = &__MapActor_GetActor(0x15)->f5a;
    *s |= k;
    OvlFunc_883_200b380(0x15, 6, 5, 0);
    __Func_80921c4(0x15, 0x175, 0x377);
    __Func_8092adc(0x15, 0x4000, 0);
    __Func_8092adc(0, 0xc000, 0x28);
    __MapActor_SetAnim(0x15, 3);
    __MapActor_DoAnim(0, 3);
    __CutsceneWait(0x14);
    __SetCameraTarget(0, 1);
    __Func_8093530();
    __CutsceneWait(0x64);
    __SetFlag(0x202);
    __ClearFlag(0x12f);
    *q = saved;
    __CutsceneEnd();
}

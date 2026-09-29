/* Func_80a8914 -- NON-MATCHING, 185 of 219 encodings differ.
 * Unattempted before batch 298.  Reference asm/rom_a1000/rom_a8604_a_a_c_a_a.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_a1000/80a8914.c \
 *       asm/rom_a1000/rom_a8604_a_a_c_a_a.s --func Func_80a8914
 *
 * NOT a distance on size: count is EXACT (219 == 219) but size is 516 against 508 --
 * four extra pool words and nothing else.  All 32 relocations are identical by name
 * AND order, so there is no alias question here at all: the tree declares these
 * callees WITH the underscore (`extern unsigned char *_GetUnit(int id);`), which is
 * why they agree.
 *
 * LANDING THIS NEEDS A GCSE_CFLAGS ROW (Makefile:303 -- the group already exists and
 * is routine here).  Diagnostic, flag-conditional: -fno-gcse collapses the pool to
 * the ROM's exact EIGHT words term for term, and the residue drops from twelve sites
 * to three (pool 12 -> 8, stream 206/211 -> 212/211).  The row cannot be added
 * usefully until a .c exists at this path, and objcmp takes its flags from the
 * Makefile by path -- use a wrapper that patches tryc.CFLAGS to get an authoritative
 * figure under the flag before landing.
 *
 * MECHANISM, proved rather than inferred: gcc-2.96 NEVER chains plain CONST_INTs.
 * A five-call probe in one block with no join pools all five separately; cse's
 * related_value only chains symbol+offset.  So the ROM's `sub r0,#0x17` CANNOT come
 * from five literals -- the 0xb0e base has to be a named local, and
 * `t = 0xb0e; t - 0x17; ...` reproduces it, but only under -fno-gcse, because gcse's
 * cprop folds each `t - N` back to a literal over the real CFG (the same spelling
 * keeps the subs in an isolated probe).
 * Also established: the r9/r10/r11 constant residue was DOWNSTREAM, not its own
 * defect -- all six sites vanished once 0xb0e stayed live in r5.
 * Remaining under the flag: the 0x100 synthesis (not source-reachable, four
 * spellings inert) and an r2/r3 swap in the two stat rows.
 */
/* Func_80a8914 (DrawCharacterHeader) -- 0x080a8914,
 * asm/rom_a1000/rom_a8604_a_a_c_a_a.s
 *
 * NON-MATCHING.  PRODUCTION FLAGS: 185 encodings of 219 differ (objcmp).
 * INSTRUCTION COUNT EXACT (219 both).  SIZE 516 against 508 -- FOUR extra pool
 * words and nothing else.  RELOCATIONS ARE IDENTICAL: all 32, by NAME and in
 * ORDER, including the four .Laf2xx data labels and iwram_3001f2c.  Only their
 * byte offsets shift, and they shift only because of the four pool words.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a8914.c \
 *     asm/rom_a1000/rom_a8604_a_a_c_a_a.s --func Func_80a8914
 *
 * NO SHIMS, NO PINS, NO asm.  datacheck.py prints nothing and exits 0: the
 * reference carries no data section, and the four .Laf2xx labels this function
 * loads are ALREADY `.global` in asm/rom_a1000/rom_a8604_c_c_c_c_c.s, so the
 * split needs NO new export.  The file holds TWO functions (Func_80a8914,
 * Func_80a8b10) so it cannot convert whole until the sibling lands.
 *
 * THE UNDERSCORE CALLEES ARE NOT AN ALIAS PROBLEM.  The tree declares these
 * names verbatim (src/non_matching/rom_15000/801f818.c has
 * `extern unsigned char *_GetUnit(int id);`), so gcc emits `bl _GetUnit` and the
 * relocation NAME matches the reference exactly.  There is no alias-only
 * relocation class to characterise here -- the names simply agree.
 *
 * ==> REQUEST TO THE OWNER: ADD A GCSE_CFLAGS ROW FOR THIS OBJECT. <==
 * `-fno-gcse` (Makefile:235's sanctioned group, 20+ existing per-file rows)
 * collapses the entire pool residue.  Measured at the instruction level:
 *
 *     production flags   pool 12 words   stream 206 against 211
 *     -fno-gcse          pool  8 words   stream 212 against 211
 *
 * EIGHT WORDS IS THE ROM'S POOL EXACTLY, and its composition matches term for
 * term: iwram_3001f2c, the four .Laf2xx labels, 0x129, 0x741, 0xb0e.  Under the
 * flag the residue drops from twelve disagreeing sites to THREE.  objcmp derives
 * its flags from the Makefile by path, so it cannot be pointed at the flag from
 * a scratch path -- with the row added, objcmp can give the authoritative
 * number and I could not.  Everything in this paragraph is flag-conditional and
 * labelled as such; the 185/219 at the top is production.
 *
 * LEVER: THE 0xb0e BASE MUST BE A NAMED LOCAL.  gcc-2.96 NEVER chains plain
 * integer constants -- cse.c's `use_related_value` builds `related_value`
 * chains only for a CONST (a SYMBOL_REF plus an integer term), never for two
 * CONST_INTs.  PROVED with a five-call probe in one basic block with no join:
 * f(0xb0e); f(0xaf7); f(0xaf8); f(0xaf9); f(0xafa) pools all five separately.
 * So the ROM's
 *     ldr r5,=0xb0e  ...  mov r0,r5 / sub r0,#0x17
 * CANNOT come from five literals, and the source must have named the base.
 * `t = 0xb0e;` then `t - 0x17 / t - 0x16 / t - 0x15 / t - 0x14` reproduces the
 * ROM's `sub` forms and its single pool word -- but ONLY under -fno-gcse.  With
 * gcse on, its constant propagation folds every `t - N` back to a literal and
 * re-pools it, which is the whole four-word residue.  The same spelling in an
 * isolated probe keeps the subs WITH gcse on, so it is gcse's cprop over the
 * real CFG, not the spelling, that folds them.
 *
 * ALSO ESTABLISHED: the r9/r10/r11 constant residue was DOWNSTREAM, not its own
 * defect.  Under production flags the ROM appeared to differ in six more places
 * by keeping 0x10, 0x18 and 8 in HIGH registers (costing a `mov` before each
 * `str rN,[sp]`) where ours used r5 directly.  Every one of those vanished once
 * the 0xb0e base stayed live in r5 under the flag: with r5 occupied, the small
 * constants are pushed to r9/r10/r11 and the `mov`s appear on their own.  Do not
 * chase a high-register constant residue before the long-lived values are right.
 *
 * BLOCKER (under -fno-gcse), TWO SITES, 212 instructions against 211:
 *
 * 1. THE 0x100 SYNTHESIS, +1 instruction.  PASS: constant splitting at expand.
 *    The ROM builds the `flags & 0x100` mask as `mov r3,#1 / add r3,#0xff` and
 *    then cse DELETES the `mov r3,#1`, because r3 already holds the 1 just
 *    stored by `q[5] = 1;` -- so the mask costs ONE instruction:
 *      rom   mov r3,#1 / strb r3,[r2,#5] / mov r2,r8 / add r3,#0xff / and r2,r3
 *      ours  mov r3,#1 / strb r3,[r2,#5] / mov r3,#128 / mov r2,r8
 *                                        / lsl r3,r3,#1 / and r2,r2,r3
 *    Ours picks the OTHER two-instruction decomposition of 256 (0x80 << 1),
 *    which shares nothing with the live 1 and so cannot be CSEd away.  The
 *    choice is made in the target's constant splitter, not from the source:
 *    measured inert were `m = flags; m &= 0x100;`, `flags & 0x100u`,
 *    `(flags >> 8) & 1` (+4 instructions), and putting the mask BEFORE the store
 *    (7 sites instead of 3).  I did not find a source spelling that reaches it.
 *
 * 2. AN r2/r3 SWAP IN THE TWO STAT ROWS.  PASS .18.greg.  In each row the ROM
 *    holds the row's y in r3 and the field offset in r2
 *    (`mov r3,#0x10 / mov r2,#0x38 / ldrsh r0,[r7,r2] / mov r9,r3 / str r3,[sp]`)
 *    and ours holds them the other way round, with the `mov r3,#0x48` for the
 *    x argument then issuing one slot earlier.  Same instruction count, same
 *    registers available; only the assignment differs.
 *
 * FLAGS: the top figure is production GCC296_CFLAGS.  Every figure that depends
 * on -fno-gcse is labelled above.  No per-file Makefile rule exists for this
 * object yet; adding the GCSE_CFLAGS row is the requested next step.
 */
struct Unit {
    unsigned char pad00[0xf];
    unsigned char f0f;                  /* 0x0f */
    unsigned char pad10[0x34 - 0x10];
    short f34;                          /* 0x34 */
    short f36;                          /* 0x36 */
    short f38;                          /* 0x38 */
    short f3a;                          /* 0x3a */
    unsigned short f3c;                 /* 0x3c */
    unsigned short f3e;                 /* 0x3e */
    unsigned short f40;                 /* 0x40 */
    unsigned char f42;                  /* 0x42 */
    unsigned char pad43[0x124 - 0x43];
    int f124;                           /* 0x124 */
    unsigned char pad128[1];
    unsigned char f129;                 /* 0x129 */
};

extern unsigned char *iwram_3001f2c;
extern unsigned char Laf22c[] __asm__(".Laf22c");
extern unsigned char Laf230[] __asm__(".Laf230");
extern unsigned char Laf234[] __asm__(".Laf234");
extern unsigned char Laf238[] __asm__(".Laf238");

extern struct Unit *_GetUnit(int id);
extern void _Func_80164d4(void *box, int b, int c, int d, int e);
extern void _Func_801e8b0(void *p, void *box, int x, int y);
extern void _Func_801e7c0(int id, void *box, int x, int y);
extern void _SetTextColor(int c);
extern void _Func_801ea08(int v, int w, void *box, int x, int y);
extern void _UIDrawText(unsigned char *s, void *box, int x, int y);
extern void WaitFrames(int n);

void Func_80a8914(void *box, int id, int flags)
{
    unsigned char *s;
    unsigned char *q;
    struct Unit *u;
    int m;
    int t;

    s = iwram_3001f2c;
    u = _GetUnit(id);
    q = *(unsigned char **)(s + (0xbe << 1));
    q[5] = 1;
    m = flags & 0x100;
    if (m == 0)
        _Func_80164d4(box, 0, 0, 0x80, 0x28);
    _Func_801e8b0(u, box, 0x28, 0);
    _Func_801e7c0(u->f129 + 0x741, box, 0, 0x20);
    _Func_801e8b0(Laf22c, box, 0x68, 0);
    _SetTextColor(0xf);
    _Func_801ea08(u->f0f, 2, box, 0x80, 0);
    _Func_801e8b0(Laf234, box, 0x28, 0x10);
    _Func_801ea08(u->f38, 4, box, 0x48, 0x10);
    _Func_801ea08(u->f34, 4, box, 0x70, 0x10);
    _UIDrawText(Laf230, box, 0x68, 0x10);
    _Func_801e8b0(Laf238, box, 0x28, 0x18);
    _Func_801ea08(u->f3a, 4, box, 0x48, 0x18);
    _Func_801ea08(u->f36, 4, box, 0x70, 0x18);
    _UIDrawText(Laf230, box, 0x68, 0x18);
    t = 0xb0e;
    _Func_801e7c0(t, box, 0x28, 8);
    _Func_801ea08(u->f124, 7, box, 0x58, 8);
    if (m == 0) {
        WaitFrames(1);
        _Func_80164d4(box, 0x90, 0, 0xe0, 0x28);
    }
    _Func_801e7c0(t - 0x17, box, 0x98, 0);
    _Func_801e7c0(t - 0x16, box, 0x98, 8);
    _Func_801e7c0(t - 0x15, box, 0x98, 0x10);
    _Func_801e7c0(t - 0x14, box, 0x98, 0x18);
    _Func_801ea08(u->f3c, 3, box, 0xc8, 0);
    _Func_801ea08(u->f3e, 3, box, 0xc8, 8);
    _Func_801ea08(u->f40, 3, box, 0xc8, 0x10);
    _Func_801ea08(u->f42, 3, box, 0xc8, 0x18);
}
